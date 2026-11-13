#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->disconnectButton->setEnabled(false);
    ui->setActivityButton->setEnabled(false);
    ui->clearActivityButton->setEnabled(false);

    const QDateTime now = QDateTime::currentDateTime();

    ui->startTimeEdit->setDateTime(now);
    ui->endTimeEdit->setDateTime(now.addSecs(3600));

    connect(ui->applicationIdEnabledCheck, &QCheckBox::toggled, this, [this](bool checked) {
        ui->applicationIdEdit->setEnabled(checked);

        if (!checked) {
            ui->applicationIdEdit->setText("1570349057879183471");
        }
    });

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectToDiscord);
    connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectFromDiscord);
    connect(ui->setActivityButton, &QPushButton::clicked, this, &MainWindow::setActivity);
    connect(ui->clearActivityButton,&QPushButton::clicked, this, &MainWindow::clearActivity);
    connect(ui->exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    connect(ui->startTimeEnabledCheck, &QCheckBox::toggled, ui->startTimeEdit, &QDateTimeEdit::setEnabled);
    connect(ui->endTimeEnabledCheck, &QCheckBox::toggled, ui->endTimeEdit, &QDateTimeEdit::setEnabled);
}

MainWindow::~MainWindow() {
    if (rpc) {
        rpc->disconnectFromDiscord();
    }

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}


void MainWindow::connectToDiscord() {
    const QString applicationId = ui->applicationIdEdit->text().trimmed();

    if (applicationId.isEmpty()) {
        QMessageBox::warning(this, "Missing Application ID", "Please enter your Discord Application ID.");
        return;
    }

    if (rpc) {
        rpc->disconnectFromDiscord();
        delete rpc;
        rpc = nullptr;
    }

    rpc = new DiscordRPC(applicationId, this);

    connect(rpc, &DiscordRPC::ready, this, [this]() {
        updateConnectionStatus("Connected");

        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);
        ui->setActivityButton->setEnabled(true);
        ui->clearActivityButton->setEnabled(true);

        ui->applicationIdEdit->setEnabled(false);
    });

    connect(rpc, &DiscordRPC::disconnected, this, [this]() {
        updateConnectionStatus("Disconnected");

        ui->connectButton->setEnabled(true);
        ui->disconnectButton->setEnabled(false);
        ui->setActivityButton->setEnabled(false);
        ui->clearActivityButton->setEnabled(false);

        ui->applicationIdEdit->setEnabled(ui->applicationIdEnabledCheck->isChecked());
    });

    connect(rpc, &DiscordRPC::errorOccurred, this, [this](const QString &error) {
        qWarning() << "Discord RPC:" << error;

        updateConnectionStatus("Error");

        ui->connectButton->setEnabled(true);
        ui->disconnectButton->setEnabled(false);
        ui->setActivityButton->setEnabled(false);
        ui->clearActivityButton->setEnabled(false);

        ui->applicationIdEdit->setEnabled(ui->applicationIdEnabledCheck->isChecked());
    });

    updateConnectionStatus("Connecting...");
    ui->connectButton->setEnabled(false);

    rpc->connectToDiscord();
}

void MainWindow::disconnectFromDiscord() {
    if (!rpc) {
        return;
    }

    rpc->disconnectFromDiscord();

    updateConnectionStatus("Disconnected");

    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->setActivityButton->setEnabled(false);
    ui->clearActivityButton->setEnabled(false);

    ui->applicationIdEdit->setEnabled(ui->applicationIdEnabledCheck->isChecked());
}

void MainWindow::updateConnectionStatus(const QString &status) {
    ui->connectionStatusLabel->setText(status);
}

bool MainWindow::buildActivity(Activity &activity) {
    switch (ui->activityTypeCombo->currentIndex()) {
    case 0:
        activity.type = Activity::Type::Playing;
        break;

    case 1:
        activity.type = Activity::Type::Streaming;
        break;

    case 2:
        activity.type = Activity::Type::Listening;
        break;

    case 3:
        activity.type = Activity::Type::Watching;
        break;

    case 4:
        activity.type = Activity::Type::CustomStatus;
        break;

    case 5:
        activity.type = Activity::Type::Competing;
        break;

    case 6:
        activity.type = Activity::Type::HangStatus;
        break;

    default:
        return false;
    }

    // Basics
    if (!ui->nameEdit->text().trimmed().isEmpty()) {
        activity.name = ui->nameEdit->text().trimmed();
    }
    activity.details = ui->detailsEdit->text().trimmed();
    activity.state = ui->stateEdit->text().trimmed();

    // Assets
    activity.assets.largeImage = ui->largeImageEdit->text().trimmed();
    activity.assets.largeText = ui->largeTextEdit->text().trimmed();
    activity.assets.smallImage = ui->smallImageEdit->text().trimmed();
    activity.assets.smallText = ui->smallTextEdit->text().trimmed();

    // Timestamps
    if (ui->startTimeEnabledCheck->isChecked()) {
        const QDateTime start = ui->startTimeEdit->dateTime();
        activity.timestamps.start = start.toSecsSinceEpoch();
    }

    if (ui->endTimeEnabledCheck->isChecked()) {
        const QDateTime end = ui->endTimeEdit->dateTime();
        activity.timestamps.end = end.toSecsSinceEpoch();

        if (ui->startTimeEnabledCheck->isChecked()) {
            if (activity.timestamps.end <= activity.timestamps.start) {
                QMessageBox::warning(this, "Invalid Timestamps", "The end time must be after the start time.");
                return false;
            }
        }
    }

    // Buttons
    auto addButton = [&](const QString &label, const QString &urlString) -> bool {
        if (label.isEmpty() && urlString.isEmpty()) {
            return true;
        }

        if (label.isEmpty() || urlString.isEmpty()) {
            QMessageBox::warning(this, "Invalid Button", "Both a button label and URL are required.");
            return false;
        }

        const QUrl url(urlString);

        if (!url.isValid() || (url.scheme() != "https" && url.scheme() != "http") || url.host().isEmpty()) {
            QMessageBox::warning(this, "Invalid URL", "Button URLs must be valid HTTP or HTTPS URLs.");
            return false;
        }

        if (label.size() > 32) {
            QMessageBox::warning(this, "Invalid Button", "Button labels cannot exceed 32 characters.");
            return false;
        }

        activity.buttons.append({label, url.toString()});

        return true;
    };

    if (!addButton(ui->button1TextEdit->text().trimmed(), ui->button1UrlEdit->text().trimmed())) {
        return false;
    }

    if (!addButton(ui->button2TextEdit->text().trimmed(), ui->button2UrlEdit->text().trimmed())) {
        return false;
    }

    return true;
}

void MainWindow::setActivity() {
    if (!rpc || !rpc->isConnected()) {
        QMessageBox::warning(this, "Not Connected", "Connect to Discord before setting an activity.");
        return;
    }

    Activity activity;

    if (!buildActivity(activity)) {
        return;
    }

    rpc->setActivity(activity);

    qDebug() << "Activity submitted.";
}

void MainWindow::clearActivity() {
    if (!rpc || !rpc->isConnected()) {
        return;
    }

    rpc->clearActivity();

    qDebug() << "Activity cleared.";
}