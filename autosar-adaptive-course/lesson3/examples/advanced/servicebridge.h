/**
 * @file servicebridge.h
 * @brief ServiceBridge - Cầu nối giữa vsomeip và Qt
 * 
 * ================ MODULE DESCRIPTION ================
 * 
 * ServiceBridge quản lý:
 * 1. Kết nối vsomeip trong thread riêng
 * 2. Service discovery (ara::com::FindService)
 * 3. Event subscription
 * 4. Method calls với ara::core::Future pattern
 * 5. Emit Qt signals khi nhận data (thread-safe)
 */

#ifndef SERVICEBRIDGE_H
#define SERVICEBRIDGE_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <vsomeip/vsomeip.hpp>
#include <atomic>
#include <map>
#include <functional>

// Service configuration
namespace ServiceConfig {
    constexpr vsomeip::service_t SERVICE_ID = 0x2000;
    constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
    
    constexpr vsomeip::method_t METHOD_GET_SPEED    = 0x0001;
    constexpr vsomeip::method_t METHOD_GET_BATTERY  = 0x0002;
    constexpr vsomeip::method_t METHOD_GET_WARNINGS = 0x0003;
    constexpr vsomeip::method_t METHOD_GET_MODE     = 0x0004;
    constexpr vsomeip::method_t METHOD_SET_MODE     = 0x0005;
    constexpr vsomeip::method_t METHOD_GET_TEMP     = 0x0006;
    
    constexpr vsomeip::event_t EVENT_SPEED_UPDATE   = 0x8001;
    constexpr vsomeip::event_t EVENT_BATTERY_UPDATE = 0x8002;
    constexpr vsomeip::event_t EVENT_WARNING_UPDATE = 0x8003;
    constexpr vsomeip::event_t EVENT_ALL_SIGNALS    = 0x8010;
    
    constexpr vsomeip::eventgroup_t EVENTGROUP_ALL = 0x0010;
    
    constexpr int RECONNECT_INTERVAL_MS = 5000;
    constexpr int REQUEST_TIMEOUT_MS = 3000;
}

/**
 * @brief ServiceBridge - Kết nối vsomeip với Qt signals
 * 
 * Patterns sử dụng:
 * - ara::com::FindService() qua WaitForService()
 * - ara::com::Event qua Subscribe và emit signals
 * - ara::core::Future qua async callbacks
 */
class ServiceBridge : public QObject {
    Q_OBJECT

public:
    explicit ServiceBridge(QObject* parent = nullptr);
    ~ServiceBridge();
    
    /**
     * @brief Initialize vsomeip application
     */
    bool init();
    
    /**
     * @brief Start service bridge (vsomeip trong thread riêng)
     */
    void start();
    
    /**
     * @brief Stop service bridge
     */
    void stop();
    
    /**
     * @brief Kiểm tra service có available không
     */
    bool isAvailable() const { return serviceAvailable_; }
    
    /**
     * @brief Mô phỏng ara::com::FindService với timeout
     */
    bool waitForService(int timeout_ms = ServiceConfig::REQUEST_TIMEOUT_MS);

public slots:
    // Method calls - gọi từ Qt thread
    void requestSpeed();
    void requestBattery();
    void requestWarnings();
    void requestDriveMode();
    void requestTemperature();
    void setDriveMode(const QString& mode);
    void requestAllData();

signals:
    // Signals emit về Qt main thread
    void serviceAvailable(bool available);
    
    void speedUpdated(float speed);
    void batteryUpdated(float level, float voltage, float temp,
                        bool charging, float health, float range);
    void temperatureUpdated(float motor, float cabin, float outside);
    void warningsUpdated(const QStringList& warnings);
    void driveModeUpdated(const QString& mode);
    void driveModeChangeResult(bool success);
    void allSignalsUpdated(const QString& data);
    
    void errorOccurred(const QString& error);

private:
    // vsomeip callbacks (chạy trong vsomeip thread)
    void onStateChange(vsomeip::state_type_e state);
    void onAvailability(vsomeip::service_t service,
                        vsomeip::instance_t instance,
                        bool available);
    void onMessage(const std::shared_ptr<vsomeip::message>& message);
    
    // Message parsing
    void handleSpeedResponse(const std::string& data);
    void handleBatteryResponse(const std::string& data);
    void handleWarningsResponse(const std::string& data);
    void handleDriveModeResponse(const std::string& data);
    void handleTemperatureResponse(const std::string& data);
    void handleSetModeResponse(const std::string& data);
    void handleEventNotification(vsomeip::event_t event, const std::string& data);
    
    // Helper functions
    void subscribeEvents();
    void unsubscribeEvents();
    std::shared_ptr<vsomeip::message> createRequest(vsomeip::method_t method);
    void sendRequest(const std::shared_ptr<vsomeip::message>& request);

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread vsomeipThread_;
    
    std::atomic<bool> serviceAvailable_;
    std::atomic<bool> running_;
    
    QMutex mutex_;
    std::condition_variable_any cv_;
    
    // Pending request tracking
    std::map<vsomeip::session_t, vsomeip::method_t> pendingRequests_;
    QMutex pendingMutex_;
};

#endif // SERVICEBRIDGE_H
