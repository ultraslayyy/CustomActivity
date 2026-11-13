#include "TrayManager.h"

TrayManager::TrayManager(QObject *parent) : QObject{parent} {
    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/icons/icon.ico"));
    tray->setToolTip("Custom Discord RPC");

    menu = new QMenu();

    QAction *openAction = menu->addAction("Open");
    QAction *exitAction = menu->addAction("Exit");

    connect(openAction, &QAction::triggered, this, &TrayManager::openRequested);
    connect(exitAction, &QAction::triggered, this, &TrayManager::exitRequested);

    tray->setContextMenu(menu);

    connect(tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
            emit openRequested();
        }
    });
}

void TrayManager::show() {
    tray->show();
}
