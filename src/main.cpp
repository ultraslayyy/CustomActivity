#include "mainwindow.h"

#include <QApplication>
#include <QFile>

#include "platform/windows/TrayManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false);
    a.setWindowIcon(QIcon(":/icons/icon.ico"));

    MainWindow w;
    TrayManager t;

    QObject::connect(&t, &TrayManager::openRequested, &w, [&w]() {
        w.show();
        w.raise();
        w.activateWindow();
    });

    QObject::connect(&t, &TrayManager::exitRequested, &a, &QApplication::quit);

    t.show();
    w.show();

    return QApplication::exec();
}
