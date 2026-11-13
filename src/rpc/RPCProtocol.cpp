#include "RPCProtocol.h"

#include <QtEndian>

QByteArray RPCProtocol::encode(Opcode opcode, const QJsonObject &payload) {
    QByteArray json = QJsonDocument(payload).toJson(QJsonDocument::Compact);

    // 8 byte RPC header
    // 0-3 is opcode, 4-7 is payload length
    QByteArray frame;
    frame.resize(8);

    qToLittleEndian(static_cast<quint32>(opcode), reinterpret_cast<uchar *>(frame.data()));
    qToLittleEndian(static_cast<quint32>(json.size()), reinterpret_cast<uchar *>(frame.data() + 4));

    // Rest of frame is json payload
    frame.append(json);

    return frame;
}

bool RPCProtocol::append(const QByteArray &data) {
    if (!error.isEmpty()) {
        return false;
    }

    buffer.append(data);

    constexpr quint32 MAX_FRAME_SIZE = 64 * 1024;

    while (buffer.size() >= 8) {
        const auto *header = reinterpret_cast<const uchar *>(buffer.constData());

        quint32 opcode = qFromLittleEndian<quint32>(header);
        quint32 length = qFromLittleEndian<quint32>(header + 4);

        if (length > MAX_FRAME_SIZE) {
            error = "RPC frame exceeds maximum size";
            return false;
        }

        if (buffer.size() < 8 + length) {
            break;
        }

        QByteArray json = buffer.mid(8, length);
        buffer.remove(0, 8 + length);

        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(json, &parseError);

        if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
            error = "Invalid RPC JSON payload";
            return false;
        }

        messages.append({static_cast<Opcode>(opcode), document.object()});
    }

    return true;
}

QList<RPCProtocol::Message> RPCProtocol::takeMessages() {
    QList<Message> result = std::move(messages);
    messages.clear();
    return result;
}

QString RPCProtocol::errorString() const {
    return error;
}

void RPCProtocol::reset() {
    buffer.clear();
    messages.clear();
    error.clear();
}