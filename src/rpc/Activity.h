#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QString>
#include <QtTypes>
#include <QJsonObject>
#include <QJsonArray>

class Activity {
public:
    enum class Type {
        Playing = 0,
        Streaming = 1,
        Listening = 2,
        Watching = 3,
        CustomStatus = 4,
        Competing = 5,
        HangStatus = 6
    };

    struct Timestamps {
        qint64 start = 0;
        qint64 end = 0;

        QJsonObject toJson() const;
    };

    struct Assets {
        QString largeImage;
        QString largeText;
        QString smallImage;
        QString smallText;

        QJsonObject toJson() const;
    };

    struct Button {
        QString label;
        QString url;

        QJsonObject toJson() const;
    };

    Type type = Type::Playing;

    QString name;
    QString state;
    QString details;

    Timestamps timestamps;
    Assets assets;

    QList<Button> buttons;

    QJsonObject toJson() const;
};

#endif // ACTIVITY_H
