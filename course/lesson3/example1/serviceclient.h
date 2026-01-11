// ============================================
// VÍ DỤ 1: SERVICE CLIENT - BRIDGE QT & VSOMEIP
// Yếu tố BẮT BUỘC:
// 1. Kế thừa QObject
// 2. Khai báo signals cho data updates
// 3. Chạy vsomeip trong thread riêng
// ============================================

#ifndef SERVICECLIENT_H
#define SERVICECLIENT_H

#include <QObject>
#include <QThread>
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <atomic>

class ServiceClient : public QObject {
    Q_OBJECT
    
public:
    explicit ServiceClient(QObject *parent = nullptr);
    ~ServiceClient();
    
    void start();
    void stop();
    bool isConnected() const { return connected_; }

signals:
    // Data signals (BẮT BUỘC)
    void connected();
    void disconnected();
    void speedUpdated(int speed);
    void batteryUpdated(int level);
    void driveModeUpdated(int mode);
    void errorOccurred(QString message);

public slots:
    void requestSpeed();
    void requestBattery();

private:
    void initVsomeip();
    void onState(vsomeip::state_type_e state);
    void onAvailability(vsomeip::service_t s, vsomeip::instance_t i, bool available);
    void onMessage(const std::shared_ptr<vsomeip::message>& msg);
    
    std::shared_ptr<vsomeip::application> app_;
    QThread* workerThread_;
    std::atomic<bool> running_;
    std::atomic<bool> connected_;
    
    static constexpr vsomeip::service_t SERVICE_ID = 0x2000;
    static constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
    static constexpr vsomeip::method_t METHOD_GET_SPEED = 0x0001;
    static constexpr vsomeip::method_t METHOD_GET_BATTERY = 0x0002;
};

#endif // SERVICECLIENT_H
