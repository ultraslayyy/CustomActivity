#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include <QObject>
#include <QLocalSocket>
#include <QTimer>

#include "RPCProtocol.h"

class IPCClient : public QObject {
    Q_OBJECT

public:
    explicit IPCClient(QObject *parent = nullptr);
    ~IPCClient() override;

    void connectToDiscord(const QString &clientId);
    void disconnectFromDiscord();

    bool isConnected() const;

    void sendMessage(RPCProtocol::Opcode opcode, const QJsonObject &payload);

    QString connectedPipe() const;

signals:
    void connected();
    void ready();
    void disconnected();

    void messageReceived(const RPCProtocol::Message &message);

    void errorOccurred(const QString &error);

private:
    static constexpr int MAX_PIPES = 10;
    static constexpr int CONNECT_TIMEOUT_MS = 1500;

    QLocalSocket socket;
    QTimer connectionTimer;

    RPCProtocol protocol;

    int currentPipe = 0;
    bool connecting = false;
    QString activePipe;

    QString applicationId;
    bool handshakeSent = false;

    void tryNextPipe();
    void handleConnected();
    void handleDisconnected();
    void handleReadyRead();

    void fail(const QString &reason);
};

#endif // IPCCLIENT_H
