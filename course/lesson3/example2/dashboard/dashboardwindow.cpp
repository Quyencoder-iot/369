#include "dashboardwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStatusBar>
#include <QApplication>
#include <QDateTime>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createStatusBar();
    applyStyleSheet();
    
    // Create proxy and connect signals
    proxy_ = dashboard::DashboardProxy::FindService(this);
    
    // Use QueuedConnection for thread safety (BẮT BUỘC!)
    connect(proxy_, &dashboard::DashboardProxy::serviceAvailable,
            this, &DashboardWindow::onServiceAvailable, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::serviceUnavailable,
            this, &DashboardWindow::onServiceUnavailable, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::speedReceived,
            this, &DashboardWindow::onSpeedReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::rpmReceived,
            this, &DashboardWindow::onRpmReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::batteryReceived,
            this, &DashboardWindow::onBatteryReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::driveModeReceived,
            this, &DashboardWindow::onDriveModeReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::allDataReceived,
            this, &DashboardWindow::onAllDataReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::errorOccurred,
            this, &DashboardWindow::onError, Qt::QueuedConnection);
}

DashboardWindow::~DashboardWindow() {
    proxy_->stop();
}

void DashboardWindow::setupUI() {
    setWindowTitle("Vehicle Dashboard - AUTOSAR Adaptive");
    setMinimumSize(900, 550);
    
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Left panel - Speed Gauge
    QGroupBox* speedGroup = new QGroupBox("Speed");
    QVBoxLayout* speedLayout = new QVBoxLayout(speedGroup);
    speedGauge_ = new SpeedGauge();
    speedGauge_->setMinimumSize(280, 280);
    speedLayout->addWidget(speedGauge_, 0, Qt::AlignCenter);
    mainLayout->addWidget(speedGroup, 2);
    
    // Center panel - Info
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(10);
    
    // RPM
    QGroupBox* rpmGroup = new QGroupBox("Engine RPM");
    QVBoxLayout* rpmLayout = new QVBoxLayout(rpmGroup);
    rpmLabel_ = new QLabel("0");
    rpmLabel_->setAlignment(Qt::AlignCenter);
    rpmLabel_->setStyleSheet("font-size: 42px; font-weight: bold; color: #00ff00;");
    rpmLayout->addWidget(rpmLabel_);
    infoLayout->addWidget(rpmGroup);
    
    // Gear
    QGroupBox* gearGroup = new QGroupBox("Gear");
    QVBoxLayout* gearLayout = new QVBoxLayout(gearGroup);
    gearLabel_ = new QLabel("P");
    gearLabel_->setAlignment(Qt::AlignCenter);
    gearLabel_->setStyleSheet("font-size: 56px; font-weight: bold; color: #ffffff;");
    gearLayout->addWidget(gearLabel_);
    infoLayout->addWidget(gearGroup);
    
    // Drive Mode
    QGroupBox* modeGroup = new QGroupBox("Drive Mode");
    QVBoxLayout* modeLayout = new QVBoxLayout(modeGroup);
    modeLabel_ = new QLabel("NORMAL");
    modeLabel_->setAlignment(Qt::AlignCenter);
    modeLabel_->setStyleSheet("font-size: 28px; font-weight: bold; color: #00aaff;");
    modeLayout->addWidget(modeLabel_);
    infoLayout->addWidget(modeGroup);
    
    mainLayout->addLayout(infoLayout, 1);
    
    // Right panel - Battery & Info
    QVBoxLayout* rightLayout = new QVBoxLayout();
    
    QGroupBox* batteryGroup = new QGroupBox("Battery Status");
    QVBoxLayout* batteryLayout = new QVBoxLayout(batteryGroup);
    batteryWidget_ = new BatteryWidget();
    batteryWidget_->setMinimumSize(180, 100);
    batteryLayout->addWidget(batteryWidget_);
    rightLayout->addWidget(batteryGroup);
    
    rightLayout->addStretch();
    
    mainLayout->addLayout(rightLayout, 1);
    
    setCentralWidget(central);
}

void DashboardWindow::createStatusBar() {
    connectionLabel_ = new QLabel("● Connecting...");
    connectionLabel_->setStyleSheet("color: orange; font-weight: bold;");
    statusBar()->addPermanentWidget(connectionLabel_);
    
    statusLabel_ = new QLabel("AUTOSAR Adaptive Dashboard v1.0");
    statusBar()->addWidget(statusLabel_);
}

void DashboardWindow::applyStyleSheet() {
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a1a2e;
        }
        QGroupBox {
            color: #ffffff;
            border: 2px solid #3a3a5e;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 12px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px;
            color: #aaaacc;
        }
        QLabel {
            color: #ffffff;
        }
        QStatusBar {
            background-color: #0f0f1a;
            color: #888888;
            border-top: 1px solid #3a3a5e;
        }
    )");
}

void DashboardWindow::onServiceAvailable() {
    connectionLabel_->setText("● Connected");
    connectionLabel_->setStyleSheet("color: #00ff00; font-weight: bold;");
    statusLabel_->setText("Service connected - receiving data");
}

void DashboardWindow::onServiceUnavailable() {
    connectionLabel_->setText("● Disconnected");
    connectionLabel_->setStyleSheet("color: #ff0000; font-weight: bold;");
    statusLabel_->setText("Service disconnected - waiting for reconnection");
    
    // Reset displays
    speedGauge_->setSpeed(0);
    rpmLabel_->setText("---");
    gearLabel_->setText("-");
    batteryWidget_->setLevel(0);
}

void DashboardWindow::onSpeedReceived(int speed) {
    speedGauge_->setSpeed(speed);
}

void DashboardWindow::onRpmReceived(int rpm) {
    rpmLabel_->setText(QString::number(rpm));
    
    // Color based on RPM
    if (rpm > 6000) {
        rpmLabel_->setStyleSheet("font-size: 42px; font-weight: bold; color: #ff0000;");
    } else if (rpm > 4000) {
        rpmLabel_->setStyleSheet("font-size: 42px; font-weight: bold; color: #ffaa00;");
    } else {
        rpmLabel_->setStyleSheet("font-size: 42px; font-weight: bold; color: #00ff00;");
    }
}

void DashboardWindow::onBatteryReceived(dashboard::BatteryInfo battery) {
    batteryWidget_->setBatteryInfo(battery);
}

void DashboardWindow::onDriveModeReceived(dashboard::DriveMode mode) {
    modeLabel_->setText(dashboard::driveModeToString(mode));
    
    // Color based on mode
    switch (mode) {
        case dashboard::DriveMode::ECO:
            modeLabel_->setStyleSheet("font-size: 28px; font-weight: bold; color: #00ff00;");
            break;
        case dashboard::DriveMode::SPORT:
            modeLabel_->setStyleSheet("font-size: 28px; font-weight: bold; color: #ff4444;");
            break;
        case dashboard::DriveMode::SNOW:
            modeLabel_->setStyleSheet("font-size: 28px; font-weight: bold; color: #00ffff;");
            break;
        default:
            modeLabel_->setStyleSheet("font-size: 28px; font-weight: bold; color: #00aaff;");
    }
}

void DashboardWindow::onAllDataReceived(dashboard::DashboardData data) {
    // Update gear
    QString gearStr;
    switch (data.gear) {
        case 0: gearStr = "P"; break;
        case 1: gearStr = "R"; break;
        case 2: gearStr = "N"; break;
        default: gearStr = QString("D%1").arg(data.gear - 2); break;
    }
    gearLabel_->setText(gearStr);
    
    // Update status
    statusLabel_->setText(QString("Last update: %1")
        .arg(QDateTime::fromMSecsSinceEpoch(data.timestamp).toString("hh:mm:ss")));
}

void DashboardWindow::onError(QString message) {
    statusLabel_->setText("Error: " + message);
    statusLabel_->setStyleSheet("color: #ff0000;");
}
