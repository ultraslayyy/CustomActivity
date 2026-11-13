#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QMenu>
#include <QSystemTrayIcon>

class TrayManager : public QObject {
    Q_OBJECT

public:
    explicit TrayManager(QObject *parent = nullptr);

    void show();

signals:
    void openRequested();
    void exitRequested();

private:
    QSystemTrayIcon *tray;
    QMenu *menu;
};

#endif // TRAYMANAGER_H
