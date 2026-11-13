#include "IPCClient.h"

IPCClient::IPCClient(QObject *parent) : QObject{parent} {
    connectionTimer.setSingleShot(true);

    connect(&socket, &QLocalSocket::connected, this, &IPCClient::handleConnected);
    connect(&socket, &QLocalSocket::disconnected, this, &IPCClient::handleDisconnected);

    connect(&socket, &QLocalSocket::readyRead, this, &IPCClient::handleReadyRead);

    connect(&socket, &QLocalSocket::errorOccurred, this, [this](QLocalSocket::LocalSocketError error) {
        Q_UNUSED(error);

        if (connecting) {
            connectionTimer.stop();
            socket.abort();
            tryNextPipe();
        } else {
            fail(socket.errorString());
        }
    });

    connect(&connectionTimer, &QTimer::timeout, this, [this]() {
        if (!connecting) {
            return;
        }

        socket.abort();
        tryNextPipe();
    });
}

IPCClient::~IPCClient() {
    connectionTimer.stop();
    socket.abort();
}

void IPCClient::connectToDiscord(const QString &clientId) {
    if (clientId.isEmpty()) {
        emit errorOccurred("Discord Application ID cannot be empty.");
        return;
    }

    if (socket.state() != QLocalSocket::UnconnectedState) {
        socket.abort();
    }

    connectionTimer.stop();
    protocol.reset();

    applicationId = clientId;
    currentPipe = 0;
    connecting = true;
    handshakeSent = false;
    activePipe.clear();

    tryNextPipe();
}

void IPCClient::tryNextPipe() {
    if (!connecting) {
        return;
    }

    if (currentPipe >= MAX_PIPES) {
        connecting = false;

        emit errorOccurred("Could not find a running Discord IPC endpoint.");
        return;
    }

    const QString pipeName = QString("discord-ipc-%1").arg(currentPipe++);

    qDebug() << "Trying Dicsord IPC:" << pipeName;

    socket.abort();

    connectionTimer.start(CONNECT_TIMEOUT_MS);
    socket.connectToServer(pipeName);
}

void IPCClient::handleConnected() {
    connectionTimer.stop();

    connecting = false;
    handshakeSent = true;

    activePipe = socket.serverName();

    qDebug() << "Connected to Discord RPC:" << activePipe;

    emit connected();

    sendMessage(RPCProtocol::Opcode::Handshake, {{"v", 1}, {"client_id", applicationId}});
}

void IPCClient::handleDisconnected() {
    connectionTimer.stop();
    activePipe.clear();

    if (connecting) {
        return;
    }

    emit disconnected();
}

void IPCClient::handleReadyRead() {
    const QByteArray data = socket.readAll();

    if (!protocol.append(data)) {
        fail(QString("RPC protocol error: %1").arg(protocol.errorString()));
        return;
    }

    const auto messages = protocol.takeMessages();

    for (const auto &message : messages) {
        if (message.opcode == RPCProtocol::Opcode::Frame) {
            const QString event = message.payload.value("evt").toString();

            if (event == "READY" && handshakeSent) {
                handshakeSent = false;
                qDebug() << "Discord RPC handshake completed.";

                emit ready();
            }
        }

        emit messageReceived(message);
    }
}

void IPCClient::sendMessage(RPCProtocol::Opcode opcode, const QJsonObject &payload) {
    if (socket.state() != QLocalSocket::ConnectedState) {
        emit errorOccurred("Cannot send RPC message: socket is disconnected");
        return;
    }

    const QByteArray frame = RPCProtocol::encode(opcode, payload);
    const quint64 written = socket.write(frame);

    if (written != frame.size()) {
        fail("Failed to write complete RPC frame.");
        return;
    }

    if (!socket.flush()) {
        fail("Failed to flush RPC data.");
    }
}

void IPCClient::disconnectFromDiscord() {
    connecting = false;
    connectionTimer.stop();

    if (socket.state() != QLocalSocket::UnconnectedState) {
        socket.disconnectFromServer();
    }

    activePipe.clear();
    protocol.reset();
}

bool IPCClient::isConnected() const {
    return socket.state() == QLocalSocket::ConnectedState;
}

QString IPCClient::connectedPipe() const {
    return activePipe;
}

void IPCClient::fail(const QString &reason) {
    connecting = false;
    connectionTimer.stop();

    socket.abort();
    activePipe.clear();

    emit errorOccurred(reason);
}