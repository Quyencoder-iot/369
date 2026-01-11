/**
 * @file mainwindow.cpp
 * @brief MainWindow Implementation - Qt Dashboard UI
 * 
 * ================ MODULE IMPLEMENTATION ================
 * 
 * Module này implement:
 * 1. UI Setup với QTabWidget
 * 2. Connect signals từ ServiceBridge
 * 3. Update UI elements khi nhận data
 * 4. Handle user interactions
 */

#include "mainwindow.h"
#include "servicebridge.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDebug>
#include <QFont>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , isConnected_(false) {
    
    setWindowTitle("AUTOSAR Adaptive Dashboard");
    setMinimumSize(800, 600);
    
    // Setup UI
    setupUI();
    applyStyleSheet();
    
    // Create service bridge
    serviceBridge_ = new ServiceBridge(this);
    
    // ========== CONNECT SIGNALS (với Qt::QueuedConnection) ==========
    // Signals từ vsomeip thread sẽ được queue vào Qt main thread
    
    connect(serviceBridge_, &ServiceBridge::serviceAvailable,
            this, &MainWindow::onServiceAvailable,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::speedUpdated,
            this, &MainWindow::onSpeedUpdated,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::batteryUpdated,
            this, &MainWindow::onBatteryUpdated,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::temperatureUpdated,
            this, &MainWindow::onTemperatureUpdated,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::warningsUpdated,
            this, &MainWindow::onWarningsUpdated,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::driveModeUpdated,
            this, &MainWindow::onDriveModeUpdated,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::driveModeChangeResult,
            this, [this](bool success) {
                if (success) {
                    statusLabel_->setText("Drive mode changed successfully");
                } else {
                    QMessageBox::warning(this, "Error", "Failed to change drive mode");
                }
            }, Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::allSignalsUpdated,
            this, &MainWindow::onAllSignalsUpdated,
            Qt::QueuedConnection);
    
    connect(serviceBridge_, &ServiceBridge::errorOccurred,
            this, [this](const QString& error) {
                statusLabel_->setText("Error: " + error);
            }, Qt::QueuedConnection);
    
    // Initialize and start service bridge
    if (serviceBridge_->init()) {
        serviceBridge_->start();
        statusLabel_->setText("Connecting to service...");
    } else {
        statusLabel_->setText("Failed to initialize service bridge");
    }
}

MainWindow::~MainWindow() {
    if (serviceBridge_) {
        serviceBridge_->stop();
    }
}

// ================================================================
// UI SETUP
// ================================================================

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Tab widget
    tabWidget_ = new QTabWidget(this);
    
    createSpeedTab();
    createBatteryTab();
    createWarningsTab();
    createSettingsTab();
    
    mainLayout->addWidget(tabWidget_);
    
    // Status bar
    statusLabel_ = new QLabel("Initializing...");
    statusLabel_->setStyleSheet("padding: 5px; background: #2d2d2d; color: white;");
    mainLayout->addWidget(statusLabel_);
}

void MainWindow::createSpeedTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Speed display group
    QGroupBox* speedGroup = new QGroupBox("Vehicle Speed");
    QVBoxLayout* speedLayout = new QVBoxLayout(speedGroup);
    
    // Large speed value
    speedValueLabel_ = new QLabel("0");
    speedValueLabel_->setAlignment(Qt::AlignCenter);
    QFont speedFont = speedValueLabel_->font();
    speedFont.setPointSize(72);
    speedFont.setBold(true);
    speedValueLabel_->setFont(speedFont);
    speedLayout->addWidget(speedValueLabel_);
    
    // Unit label
    speedUnitLabel_ = new QLabel("km/h");
    speedUnitLabel_->setAlignment(Qt::AlignCenter);
    QFont unitFont = speedUnitLabel_->font();
    unitFont.setPointSize(24);
    speedUnitLabel_->setFont(unitFont);
    speedLayout->addWidget(speedUnitLabel_);
    
    // Progress bar
    speedBar_ = new QProgressBar();
    speedBar_->setRange(0, 200);
    speedBar_->setValue(0);
    speedBar_->setTextVisible(false);
    speedBar_->setMinimumHeight(30);
    speedLayout->addWidget(speedBar_);
    
    layout->addWidget(speedGroup);
    
    // Temperature group
    QGroupBox* tempGroup = new QGroupBox("Temperature");
    QGridLayout* tempLayout = new QGridLayout(tempGroup);
    
    tempLayout->addWidget(new QLabel("Motor:"), 0, 0);
    motorTempLabel_ = new QLabel("-- °C");
    tempLayout->addWidget(motorTempLabel_, 0, 1);
    
    tempLayout->addWidget(new QLabel("Cabin:"), 1, 0);
    cabinTempLabel_ = new QLabel("-- °C");
    tempLayout->addWidget(cabinTempLabel_, 1, 1);
    
    tempLayout->addWidget(new QLabel("Outside:"), 2, 0);
    outsideTempLabel_ = new QLabel("-- °C");
    tempLayout->addWidget(outsideTempLabel_, 2, 1);
    
    layout->addWidget(tempGroup);
    layout->addStretch();
    
    tabWidget_->addTab(tab, "Speed");
}

void MainWindow::createBatteryTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    QGroupBox* batteryGroup = new QGroupBox("Battery Status");
    QVBoxLayout* batteryLayout = new QVBoxLayout(batteryGroup);
    
    // Battery bar
    batteryBar_ = new QProgressBar();
    batteryBar_->setRange(0, 100);
    batteryBar_->setValue(0);
    batteryBar_->setMinimumHeight(40);
    batteryLayout->addWidget(batteryBar_);
    
    // Battery percent
    batteryPercentLabel_ = new QLabel("-- %");
    batteryPercentLabel_->setAlignment(Qt::AlignCenter);
    QFont percentFont = batteryPercentLabel_->font();
    percentFont.setPointSize(36);
    percentFont.setBold(true);
    batteryPercentLabel_->setFont(percentFont);
    batteryLayout->addWidget(batteryPercentLabel_);
    
    // Details grid
    QGridLayout* detailsGrid = new QGridLayout();
    
    detailsGrid->addWidget(new QLabel("Voltage:"), 0, 0);
    batteryVoltageLabel_ = new QLabel("-- V");
    detailsGrid->addWidget(batteryVoltageLabel_, 0, 1);
    
    detailsGrid->addWidget(new QLabel("Temperature:"), 1, 0);
    batteryTempLabel_ = new QLabel("-- °C");
    detailsGrid->addWidget(batteryTempLabel_, 1, 1);
    
    detailsGrid->addWidget(new QLabel("Health:"), 2, 0);
    batteryHealthLabel_ = new QLabel("-- %");
    detailsGrid->addWidget(batteryHealthLabel_, 2, 1);
    
    detailsGrid->addWidget(new QLabel("Range:"), 3, 0);
    batteryRangeLabel_ = new QLabel("-- km");
    batteryRangeLabel_->setStyleSheet("font-weight: bold; font-size: 16px;");
    detailsGrid->addWidget(batteryRangeLabel_, 3, 1);
    
    detailsGrid->addWidget(new QLabel("Charging:"), 4, 0);
    chargingStatusLabel_ = new QLabel("--");
    detailsGrid->addWidget(chargingStatusLabel_, 4, 1);
    
    batteryLayout->addLayout(detailsGrid);
    layout->addWidget(batteryGroup);
    layout->addStretch();
    
    tabWidget_->addTab(tab, "Battery");
}

void MainWindow::createWarningsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    QGroupBox* warningsGroup = new QGroupBox("Active Warnings");
    QVBoxLayout* warningsLayout = new QVBoxLayout(warningsGroup);
    
    // Warning count
    warningCountLabel_ = new QLabel("0 warnings");
    warningCountLabel_->setStyleSheet("font-size: 18px; font-weight: bold;");
    warningsLayout->addWidget(warningCountLabel_);
    
    // Warnings list
    warningsList_ = new QListWidget();
    warningsList_->setMinimumHeight(200);
    warningsLayout->addWidget(warningsList_);
    
    layout->addWidget(warningsGroup);
    layout->addStretch();
    
    tabWidget_->addTab(tab, "Warnings");
}

void MainWindow::createSettingsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Drive Mode group
    QGroupBox* modeGroup = new QGroupBox("Drive Mode");
    QHBoxLayout* modeLayout = new QHBoxLayout(modeGroup);
    
    modeCombo_ = new QComboBox();
    modeCombo_->addItems({"eco", "normal", "sport", "snow", "offroad"});
    modeLayout->addWidget(modeCombo_);
    
    modeChangeBtn_ = new QPushButton("Apply");
    connect(modeChangeBtn_, &QPushButton::clicked,
            this, &MainWindow::onModeChangeClicked);
    modeLayout->addWidget(modeChangeBtn_);
    
    layout->addWidget(modeGroup);
    
    // Connection group
    QGroupBox* connGroup = new QGroupBox("Connection");
    QHBoxLayout* connLayout = new QHBoxLayout(connGroup);
    
    refreshBtn_ = new QPushButton("Refresh Data");
    connect(refreshBtn_, &QPushButton::clicked,
            this, &MainWindow::onRefreshClicked);
    connLayout->addWidget(refreshBtn_);
    
    connectBtn_ = new QPushButton("Reconnect");
    connect(connectBtn_, &QPushButton::clicked,
            this, &MainWindow::onConnectClicked);
    connLayout->addWidget(connectBtn_);
    
    layout->addWidget(connGroup);
    layout->addStretch();
    
    tabWidget_->addTab(tab, "Settings");
}

// ================================================================
// SLOTS - Cập nhật UI khi nhận data
// ================================================================

void MainWindow::onServiceAvailable(bool available) {
    isConnected_ = available;
    updateConnectionStatus(available);
    
    if (available) {
        statusLabel_->setText("Connected to Dashboard Service");
        // Request initial data
        serviceBridge_->requestAllData();
    } else {
        statusLabel_->setText("Disconnected - Waiting for service...");
    }
}

void MainWindow::onSpeedUpdated(float speed) {
    speedValueLabel_->setText(QString::number(speed, 'f', 1));
    speedBar_->setValue(qMin(200, static_cast<int>(speed)));
    
    // Color coding based on speed
    QString color = "green";
    if (speed > 120) color = "red";
    else if (speed > 80) color = "orange";
    
    speedValueLabel_->setStyleSheet(QString("color: %1;").arg(color));
}

void MainWindow::onBatteryUpdated(float level, float voltage, float temp,
                                   bool charging, float health, float range) {
    batteryBar_->setValue(static_cast<int>(level));
    batteryPercentLabel_->setText(QString::number(level, 'f', 1) + " %");
    
    if (voltage > 0) batteryVoltageLabel_->setText(QString::number(voltage, 'f', 1) + " V");
    if (temp > 0) batteryTempLabel_->setText(QString::number(temp, 'f', 1) + " °C");
    if (health > 0) batteryHealthLabel_->setText(QString::number(health, 'f', 0) + " %");
    batteryRangeLabel_->setText(QString::number(range, 'f', 0) + " km");
    chargingStatusLabel_->setText(charging ? "Yes ⚡" : "No");
    
    // Battery bar color
    QString barColor = "green";
    if (level < 20) barColor = "red";
    else if (level < 50) barColor = "orange";
    
    batteryBar_->setStyleSheet(
        QString("QProgressBar::chunk { background-color: %1; }").arg(barColor));
}

void MainWindow::onTemperatureUpdated(float motor, float cabin, float outside) {
    motorTempLabel_->setText(QString::number(motor, 'f', 1) + " °C");
    cabinTempLabel_->setText(QString::number(cabin, 'f', 1) + " °C");
    outsideTempLabel_->setText(QString::number(outside, 'f', 1) + " °C");
    
    // Motor temperature warning color
    QString motorColor = "black";
    if (motor > 90) motorColor = "red";
    else if (motor > 70) motorColor = "orange";
    motorTempLabel_->setStyleSheet(QString("color: %1; font-weight: bold;").arg(motorColor));
}

void MainWindow::onWarningsUpdated(const QStringList& warnings) {
    warningsList_->clear();
    
    for (const QString& warning : warnings) {
        QListWidgetItem* item = new QListWidgetItem(warning);
        
        // Color based on severity
        if (warning.contains("high") || warning.contains("critical")) {
            item->setBackground(QColor(255, 200, 200));
            item->setForeground(Qt::darkRed);
        } else if (warning.contains("medium")) {
            item->setBackground(QColor(255, 230, 200));
            item->setForeground(Qt::darkYellow);
        } else {
            item->setBackground(QColor(255, 255, 200));
        }
        
        warningsList_->addItem(item);
    }
    
    int count = warnings.size();
    warningCountLabel_->setText(QString("%1 warning%2").arg(count).arg(count != 1 ? "s" : ""));
    
    if (count > 0) {
        warningCountLabel_->setStyleSheet("font-size: 18px; font-weight: bold; color: red;");
    } else {
        warningCountLabel_->setStyleSheet("font-size: 18px; font-weight: bold; color: green;");
        warningCountLabel_->setText("No warnings ✓");
    }
}

void MainWindow::onDriveModeUpdated(const QString& mode) {
    int index = modeCombo_->findText(mode);
    if (index >= 0) {
        modeCombo_->setCurrentIndex(index);
    }
}

void MainWindow::onAllSignalsUpdated(const QString& data) {
    // Parse all signals for continuous updates
    // Format: speed:X,battery:X,range:X,mode:X,motor_temp:X
    qDebug() << "[MainWindow] All signals:" << data;
}

// ================================================================
// UI INTERACTION SLOTS
// ================================================================

void MainWindow::onModeChangeClicked() {
    QString mode = modeCombo_->currentText();
    statusLabel_->setText("Changing drive mode to " + mode + "...");
    serviceBridge_->setDriveMode(mode);
}

void MainWindow::onRefreshClicked() {
    if (!isConnected_) {
        QMessageBox::warning(this, "Error", "Not connected to service");
        return;
    }
    statusLabel_->setText("Refreshing data...");
    serviceBridge_->requestAllData();
}

void MainWindow::onConnectClicked() {
    statusLabel_->setText("Attempting to reconnect...");
    serviceBridge_->stop();
    
    if (serviceBridge_->init()) {
        serviceBridge_->start();
    }
}

void MainWindow::updateConnectionStatus(bool connected) {
    QString statusStyle = connected 
        ? "padding: 5px; background: #2d5a2d; color: white;"
        : "padding: 5px; background: #5a2d2d; color: white;";
    statusLabel_->setStyleSheet(statusStyle);
    
    // Enable/disable buttons based on connection
    refreshBtn_->setEnabled(connected);
    modeChangeBtn_->setEnabled(connected);
}

void MainWindow::applyStyleSheet() {
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a1a1a;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #3d3d3d;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: #2d2d2d;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        QLabel {
            color: white;
        }
        QProgressBar {
            border: 2px solid #3d3d3d;
            border-radius: 5px;
            text-align: center;
            background-color: #1a1a1a;
        }
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
        QPushButton {
            background-color: #4a4a4a;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #5a5a5a;
        }
        QPushButton:pressed {
            background-color: #3a3a3a;
        }
        QPushButton:disabled {
            background-color: #2a2a2a;
            color: #5a5a5a;
        }
        QComboBox {
            background-color: #4a4a4a;
            color: white;
            border: none;
            padding: 8px;
            border-radius: 5px;
        }
        QListWidget {
            background-color: #2d2d2d;
            color: white;
            border: 2px solid #3d3d3d;
            border-radius: 5px;
        }
        QTabWidget::pane {
            border: 2px solid #3d3d3d;
            background-color: #1a1a1a;
        }
        QTabBar::tab {
            background-color: #2d2d2d;
            color: white;
            padding: 10px 20px;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
        }
        QTabBar::tab:selected {
            background-color: #4a4a4a;
        }
    )");
}
