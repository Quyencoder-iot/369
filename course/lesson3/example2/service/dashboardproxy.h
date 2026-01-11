// =============================================
// MODULE: DashboardProxy
// ---------------------------------------------
// Mục đích: Mô phỏng ara::com::ServiceProxy
// Giải thích:
// - FindService() tìm service
// - Events qua Qt signals
// - Methods qua async calls
// =============================================

#ifndef DASHBOARDPROXY_H
#define DASHBOARDPROXY_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <atomic>
#include <functional>
#include "datatypes.h"

namespace dashboard {

class DashboardProxy : public QObject {
    Q_OBJECT
    
public:
    // Service identifiers
    static constexpr uint16_t SERVICE_ID = 0x2000;
    static constexpr uint16_t INSTANCE_ID = 0x0001;
    
    explicit DashboardProxy(QObject *parent = nullptr);
    ~DashboardProxy();
    
    // ==========================================
    // ara::com::FindService() style
    // ==========================================
    static DashboardProxy* FindService(QObject* parent = nullptr);
    
    // Service state
    bool isAvailable() const { return serviceAvailable_; }
    
    // ==========================================
    // ara::com::Method calls
    // ==========================================
    void getSpeed();
    void getBattery();
    void getWarnings();
    void getDriveMode();
    void getAllData();
    
    // ==========================================
    // ara::com::Event subscriptions
    // ==========================================
    void subscribeAllSignals();
    void unsubscribeAllSignals();

signals:
    // Service state
    void serviceAvailable();
    void serviceUnavailable();
    void errorOccurred(QString message);
    
    // Data events (ara::com::Event style)
    void speedReceived(int speed);
    void rpmReceived(int rpm);
    void batteryReceived(dashboard::BatteryInfo battery);
    void warningsReceived(QVector<dashboard::Warning> warnings);
    void driveModeReceived(dashboard::DriveMode mode);
    void allDataReceived(dashboard::DashboardData data);

public slots:
    void start();
    void stop();

private:
    // vsomeip handlers
    void onState(vsomeip::state_type_e state);
    void onAvailability(vsomeip::service_t s, vsomeip::instance_t i, bool available);
    void onMessage(const std::shared_ptr<vsomeip::message>& msg);
    
    // Helpers
    void sendRequest(vsomeip::method_t method);
    void parseAllSignalsEvent(const std::shared_ptr<vsomeip::payload>& payload);
    
    // Members
    std::shared_ptr<vsomeip::application> app_;
    QThread* workerThread_;
    std::atomic<bool> running_;
    std::atomic<bool> serviceAvailable_;
    
    // Method IDs
    static constexpr vsomeip::method_t METHOD_GET_SPEED = 0x0001;
    static constexpr vsomeip::method_t METHOD_GET_BATTERY = 0x0002;
    static constexpr vsomeip::method_t METHOD_GET_WARNINGS = 0x0003;
    static constexpr vsomeip::method_t METHOD_GET_DRIVEMODE = 0x0004;
    static constexpr vsomeip::event_t EVENT_ALL_SIGNALS = 0x8001;
    static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;
};

}  // namespace dashboard

#endif // DASHBOARDPROXY_H
