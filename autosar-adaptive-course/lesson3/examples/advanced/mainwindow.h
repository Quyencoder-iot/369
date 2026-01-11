/**
 * @file mainwindow.h
 * @brief VÍ DỤ 2 - Qt GUI Dashboard với vsomeip backend
 * 
 * MainWindow Header - Định nghĩa UI và connections
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QThread>
#include <QMutex>

// Forward declaration
class ServiceBridge;

/**
 * @brief MainWindow - Dashboard UI chính
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Slots nhận data từ service (Qt::QueuedConnection)
    void onServiceAvailable(bool available);
    void onSpeedUpdated(float speed);
    void onBatteryUpdated(float level, float voltage, float temp, 
                          bool charging, float health, float range);
    void onTemperatureUpdated(float motor, float cabin, float outside);
    void onWarningsUpdated(const QStringList& warnings);
    void onDriveModeUpdated(const QString& mode);
    void onAllSignalsUpdated(const QString& data);
    
    // UI interaction slots
    void onModeChangeClicked();
    void onRefreshClicked();
    void onConnectClicked();

private:
    void setupUI();
    void createSpeedTab();
    void createBatteryTab();
    void createWarningsTab();
    void createSettingsTab();
    void updateConnectionStatus(bool connected);
    void applyStyleSheet();

private:
    // Service bridge (runs vsomeip in separate thread)
    ServiceBridge* serviceBridge_;
    
    // Main layout
    QTabWidget* tabWidget_;
    QLabel* statusLabel_;
    
    // Speed Tab widgets
    QLabel* speedValueLabel_;
    QProgressBar* speedBar_;
    QLabel* speedUnitLabel_;
    
    // Battery Tab widgets
    QProgressBar* batteryBar_;
    QLabel* batteryPercentLabel_;
    QLabel* batteryVoltageLabel_;
    QLabel* batteryTempLabel_;
    QLabel* batteryHealthLabel_;
    QLabel* batteryRangeLabel_;
    QLabel* chargingStatusLabel_;
    
    // Temperature widgets
    QLabel* motorTempLabel_;
    QLabel* cabinTempLabel_;
    QLabel* outsideTempLabel_;
    
    // Warnings Tab widgets
    QListWidget* warningsList_;
    QLabel* warningCountLabel_;
    
    // Settings Tab widgets
    QComboBox* modeCombo_;
    QPushButton* modeChangeBtn_;
    QPushButton* refreshBtn_;
    QPushButton* connectBtn_;
    
    // State
    bool isConnected_;
};

#endif // MAINWINDOW_H
