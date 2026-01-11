#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include "../service/dashboardproxy.h"
#include "../widgets/speedgauge.h"
#include "../widgets/batterywidget.h"

class DashboardWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow();

private slots:
    void onServiceAvailable();
    void onServiceUnavailable();
    void onSpeedReceived(int speed);
    void onRpmReceived(int rpm);
    void onBatteryReceived(dashboard::BatteryInfo battery);
    void onDriveModeReceived(dashboard::DriveMode mode);
    void onAllDataReceived(dashboard::DashboardData data);
    void onError(QString message);

private:
    void setupUI();
    void createStatusBar();
    void applyStyleSheet();
    
    dashboard::DashboardProxy* proxy_;
    
    // Widgets
    SpeedGauge* speedGauge_;
    BatteryWidget* batteryWidget_;
    QLabel* rpmLabel_;
    QLabel* gearLabel_;
    QLabel* modeLabel_;
    QLabel* statusLabel_;
    QLabel* connectionLabel_;
};

#endif // DASHBOARDWINDOW_H
