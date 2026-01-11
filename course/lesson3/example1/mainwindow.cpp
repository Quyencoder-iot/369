#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    
    // Create service client
    client_ = new ServiceClient(this);
    
    // Connect signals với QueuedConnection (BẮT BUỘC cho thread safety)
    connect(client_, &ServiceClient::connected,
            this, &MainWindow::onConnected, Qt::QueuedConnection);
    connect(client_, &ServiceClient::disconnected,
            this, &MainWindow::onDisconnected, Qt::QueuedConnection);
    connect(client_, &ServiceClient::speedUpdated,
            this, &MainWindow::onSpeedUpdated, Qt::QueuedConnection);
    connect(client_, &ServiceClient::batteryUpdated,
            this, &MainWindow::onBatteryUpdated, Qt::QueuedConnection);
    
    connect(refreshBtn_, &QPushButton::clicked, this, &MainWindow::onRefresh);
    
    // Start client
    client_->start();
}

MainWindow::~MainWindow() {
    client_->stop();
}

void MainWindow::setupUI() {
    setWindowTitle("Dashboard Client - Simple");
    setMinimumSize(400, 300);
    
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    
    // Status
    statusLabel_ = new QLabel("Connecting...");
    statusLabel_->setStyleSheet("color: orange; font-weight: bold; font-size: 14px;");
    layout->addWidget(statusLabel_);
    
    // Speed
    QGroupBox* speedGroup = new QGroupBox("Speed");
    QVBoxLayout* speedLayout = new QVBoxLayout(speedGroup);
    speedLabel_ = new QLabel("--- km/h");
    speedLabel_->setStyleSheet("font-size: 48px; font-weight: bold;");
    speedLabel_->setAlignment(Qt::AlignCenter);
    speedLayout->addWidget(speedLabel_);
    layout->addWidget(speedGroup);
    
    // Battery
    QGroupBox* batteryGroup = new QGroupBox("Battery");
    QVBoxLayout* batteryLayout = new QVBoxLayout(batteryGroup);
    batteryBar_ = new QProgressBar();
    batteryBar_->setRange(0, 100);
    batteryBar_->setValue(0);
    batteryBar_->setTextVisible(true);
    batteryBar_->setFormat("%v%");
    batteryBar_->setMinimumHeight(30);
    batteryLayout->addWidget(batteryBar_);
    layout->addWidget(batteryGroup);
    
    // Refresh button
    refreshBtn_ = new QPushButton("Refresh Data");
    refreshBtn_->setMinimumHeight(40);
    refreshBtn_->setEnabled(false);
    layout->addWidget(refreshBtn_);
    
    layout->addStretch();
    
    setCentralWidget(central);
}

void MainWindow::onConnected() {
    statusLabel_->setText("● Connected");
    statusLabel_->setStyleSheet("color: green; font-weight: bold; font-size: 14px;");
    refreshBtn_->setEnabled(true);
    
    // Auto refresh on connect
    onRefresh();
}

void MainWindow::onDisconnected() {
    statusLabel_->setText("● Disconnected");
    statusLabel_->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");
    refreshBtn_->setEnabled(false);
    
    speedLabel_->setText("--- km/h");
    batteryBar_->setValue(0);
}

void MainWindow::onSpeedUpdated(int speed) {
    speedLabel_->setText(QString::number(speed) + " km/h");
    
    // Color based on speed
    if (speed > 120) {
        speedLabel_->setStyleSheet("font-size: 48px; font-weight: bold; color: red;");
    } else if (speed > 80) {
        speedLabel_->setStyleSheet("font-size: 48px; font-weight: bold; color: orange;");
    } else {
        speedLabel_->setStyleSheet("font-size: 48px; font-weight: bold; color: green;");
    }
}

void MainWindow::onBatteryUpdated(int level) {
    batteryBar_->setValue(level);
    
    // Color based on level
    QString style;
    if (level < 20) {
        style = "QProgressBar::chunk { background-color: red; }";
    } else if (level < 50) {
        style = "QProgressBar::chunk { background-color: orange; }";
    } else {
        style = "QProgressBar::chunk { background-color: green; }";
    }
    batteryBar_->setStyleSheet(style);
}

void MainWindow::onRefresh() {
    client_->requestSpeed();
    client_->requestBattery();
}
