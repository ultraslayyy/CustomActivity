#ifndef RPCPROTOCOL_H
#define RPCPROTOCOL_H

#include <QtTypes>
#include <QJsonObject>
#include <QByteArray>

class RPCProtocol {
public:
    enum class Opcode : quint32 {
        Handshake = 0,
        Frame = 1,
        Close = 2,
        Ping = 3,
        Pong = 4
    };

    struct Message {
        Opcode opcode;
        QJsonObject payload;
    };

    static QByteArray encode(Opcode opcode, const QJsonObject &payload);

    bool append(const QByteArray &data);

    QList<Message> takeMessages();
    QString errorString() const;
    void reset();

private:
    QByteArray buffer;
    QList<Message> messages;
    QString error;
};

#endif // RPCPROTOCOL_H
