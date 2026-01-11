#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include "serviceclient.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnected();
    void onDisconnected();
    void onSpeedUpdated(int speed);
    void onBatteryUpdated(int level);
    void onRefresh();

private:
    void setupUI();
    
    ServiceClient* client_;
    
    // UI elements
    QLabel* statusLabel_;
    QLabel* speedLabel_;
    QProgressBar* batteryBar_;
    QPushButton* refreshBtn_;
};

#endif // MAINWINDOW_H
