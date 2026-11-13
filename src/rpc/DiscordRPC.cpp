#include "DiscordRPC.h"

#include <QCoreApplication>

DiscordRPC::DiscordRPC(const QString &applicationId, QObject *parent) : QObject{parent}, applicationId(applicationId), ipc(nullptr) {
    connect(&ipc, &IPCClient::ready, this, [this]() {
        sessionReady = true;
        emit ready();
    });

    connect(&ipc, &IPCClient::disconnected, this, [this]() {
        sessionReady = false;
        emit disconnected();
    });

    connect(&ipc, &IPCClient::errorOccurred, this, &DiscordRPC::errorOccurred);
    connect(&ipc, &IPCClient::messageReceived, this, &DiscordRPC::handleMessage);
}

void DiscordRPC::connectToDiscord() {
    sessionReady = false;

    ipc.connectToDiscord(applicationId);
}

void DiscordRPC::disconnectFromDiscord() {
    sessionReady = false;
    ipc.disconnectFromDiscord();
}

bool DiscordRPC::isConnected() const {
    return sessionReady;
}

void DiscordRPC::setActivity(const Activity &activity) {
    if (!sessionReady) {
        emit errorOccurred("Cannot set activity: Discord is not ready.");
        return;
    }

    QJsonObject args {
        {"pid", static_cast<qint64>(QCoreApplication::applicationPid())},
        {"activity", activity.toJson()}
    };

    sendCommand("SET_ACTIVITY", args);
}

void DiscordRPC::clearActivity() {
    if (!sessionReady) {
        emit errorOccurred("Cannot clear activity: Discord is not ready.");
        return;
    }

    QJsonObject args {
        {"pid", static_cast<qint64>(QCoreApplication::applicationPid())},
        {"activity", QJsonValue(QJsonValue::Null)}
    };

    sendCommand("SET_ACTIVITY", args);
}

void DiscordRPC::sendCommand(const QString &command, const QJsonObject &args) {
    const QString nonce = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QJsonObject payload {
        {"cmd", command},
        {"args", args},
        {"nonce", nonce}
    };

    qDebug() << "Sending RPC Command:" << command;
    qDebug() << "Nonce:" << nonce;

    ipc.sendMessage(RPCProtocol::Opcode::Frame, payload);
}

void DiscordRPC::handleMessage(const RPCProtocol::Message &message) {
    if (message.opcode != RPCProtocol::Opcode::Frame) {
        return;
    }

    const QString event = message.payload.value("evt").toString();

    if (event == "ERROR") {
        const QJsonObject data = message.payload.value("data").toObject();

        const QString error = data.value("message").toString();

        qWarning() << "Discord RPC error:" << error;

        emit errorOccurred(error);
        return;
    }

    const QString command = message.payload.value("cmd").toString();

    if (command == "SET_ACTIVITY") {
        qDebug() << "Discord acknowledged SET_ACTIVITY";
    }
}