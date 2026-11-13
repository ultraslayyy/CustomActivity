#ifndef DISCORDRPC_H
#define DISCORDRPC_H

#include <QObject>

#include "Activity.h"
#include "IPCClient.h"

class DiscordRPC : public QObject {
    Q_OBJECT

public:
    explicit DiscordRPC(const QString &applicationId, QObject *parent = nullptr);

    void connectToDiscord();
    void disconnectFromDiscord();

    void setActivity(const Activity &activity);
    void clearActivity();

    bool isConnected() const;

signals:
    void ready();
    void disconnected();
    void errorOccurred(const QString &error);

private:
    QString applicationId;
    IPCClient ipc;

    bool sessionReady = false;

    void handleMessage(const RPCProtocol::Message &message);
    void sendCommand(const QString &command, const QJsonObject &args);
};

#endif // DISCORDRPC_H
