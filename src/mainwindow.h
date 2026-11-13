#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "rpc/DiscordRPC.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void connectToDiscord();
    void disconnectFromDiscord();

    void setActivity();
    void clearActivity();

private:
    Ui::MainWindow *ui;

    DiscordRPC *rpc = nullptr;

    bool buildActivity(Activity &activity);
    void updateConnectionStatus(const QString &status);

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
