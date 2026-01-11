/**
 * @file qt_client_basic.cpp
 * @brief VÍ DỤ 1 - Qt Console Client cơ bản với vsomeip
 * 
 * Các yếu tố BẮT BUỘC phải có:
 * 1. QCoreApplication cho event loop
 * 2. QTimer để request dữ liệu định kỳ
 * 3. ServiceBridge kết nối vsomeip và Qt signals
 * 4. Xử lý thread-safe giữa vsomeip thread và Qt thread
 */

#include <QCoreApplication>
#include <QTimer>
#include <QObject>
#include <QDebug>
#include <vsomeip/vsomeip.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

// ========== SERVICE IDs ==========
#define SERVICE_ID      0x2000
#define INSTANCE_ID     0x0001
#define METHOD_GET_SPEED    0x0001
#define METHOD_GET_BATTERY  0x0002
#define EVENT_ALL_SIGNALS   0x8010
#define EVENTGROUP_ALL      0x0010

/**
 * @brief ServiceBridge - Cầu nối giữa vsomeip và Qt
 * 
 * Kế thừa QObject để sử dụng Qt signals/slots
 * Chạy vsomeip trong thread riêng
 */
class ServiceBridge : public QObject {
    Q_OBJECT
    
public:
    explicit ServiceBridge(QObject* parent = nullptr) 
        : QObject(parent), service_available_(false), running_(false) {}
    
    ~ServiceBridge() {
        stop();
    }
    
    /**
     * @brief Initialize vsomeip application
     */
    bool init() {
        app_ = vsomeip::runtime::get()->create_application("qt_client");
        if (!app_ || !app_->init()) {
            qCritical() << "Failed to init vsomeip";
            return false;
        }
        
        // Register handlers
        app_->register_availability_handler(
            SERVICE_ID, INSTANCE_ID,
            [this](vsomeip::service_t, vsomeip::instance_t, bool available) {
                onAvailability(available);
            });
        
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            [this](const std::shared_ptr<vsomeip::message>& msg) {
                onMessage(msg);
            });
        
        qInfo() << "ServiceBridge initialized";
        return true;
    }
    
    /**
     * @brief Start vsomeip trong thread riêng
     */
    void start() {
        running_ = true;
        
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        // Subscribe events
        std::set<vsomeip::eventgroup_t> groups = {EVENTGROUP_ALL};
        app_->request_event(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS, 
                           groups, vsomeip::event_type_e::ET_FIELD);
        app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ALL);
        
        // Start vsomeip trong thread riêng
        vsomeip_thread_ = std::thread([this]() {
            qInfo() << "vsomeip thread started";
            app_->start();
            qInfo() << "vsomeip thread stopped";
        });
        
        qInfo() << "ServiceBridge started";
    }
    
    void stop() {
        running_ = false;
        if (app_) {
            app_->stop();
        }
        if (vsomeip_thread_.joinable()) {
            vsomeip_thread_.join();
        }
    }
    
    bool isAvailable() const { return service_available_; }
    
    /**
     * @brief Request speed từ service
     */
    void requestSpeed() {
        if (!service_available_) return;
        
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_GET_SPEED);
        
        auto payload = vsomeip::runtime::get()->create_payload();
        request->set_payload(payload);
        
        app_->send(request);
    }
    
    /**
     * @brief Request battery từ service
     */
    void requestBattery() {
        if (!service_available_) return;
        
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_GET_BATTERY);
        
        auto payload = vsomeip::runtime::get()->create_payload();
        request->set_payload(payload);
        
        app_->send(request);
    }

signals:
    // Qt signals - emit từ vsomeip thread, xử lý trong Qt thread
    void serviceAvailable(bool available);
    void speedReceived(float speed);
    void batteryReceived(float level, float range);
    void allSignalsReceived(const QString& data);

private:
    void onAvailability(bool available) {
        qInfo() << "Service" << (available ? "AVAILABLE" : "NOT AVAILABLE");
        service_available_ = available;
        
        // Emit Qt signal (thread-safe với queued connection)
        emit serviceAvailable(available);
    }
    
    void onMessage(const std::shared_ptr<vsomeip::message>& message) {
        auto payload = message->get_payload();
        if (!payload || payload->get_length() == 0) return;
        
        std::string data(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length());
        
        // Check message type
        if (message->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            // Event notification
            emit allSignalsReceived(QString::fromStdString(data));
        } else {
            // Response to method call
            auto method = message->get_method();
            
            if (method == METHOD_GET_SPEED) {
                try {
                    float speed = std::stof(data);
                    emit speedReceived(speed);
                } catch (...) {}
            }
            else if (method == METHOD_GET_BATTERY) {
                try {
                    // Parse: level,voltage,temp,charging,health,range
                    size_t pos = 0;
                    float level = std::stof(data);
                    pos = data.find(',', pos);
                    for (int i = 0; i < 4 && pos != std::string::npos; i++) {
                        pos = data.find(',', pos + 1);
                    }
                    float range = std::stof(data.substr(pos + 1));
                    emit batteryReceived(level, range);
                } catch (...) {}
            }
        }
    }
    
private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread vsomeip_thread_;
    std::atomic<bool> service_available_;
    std::atomic<bool> running_;
};

/**
 * @brief DashboardController - Điều khiển logic ứng dụng
 */
class DashboardController : public QObject {
    Q_OBJECT
    
public:
    DashboardController(QObject* parent = nullptr) : QObject(parent) {
        // Tạo service bridge
        bridge_ = new ServiceBridge(this);
        
        // Connect signals với queued connection (thread-safe)
        connect(bridge_, &ServiceBridge::serviceAvailable,
                this, &DashboardController::onServiceAvailable,
                Qt::QueuedConnection);
                
        connect(bridge_, &ServiceBridge::speedReceived,
                this, &DashboardController::onSpeedReceived,
                Qt::QueuedConnection);
                
        connect(bridge_, &ServiceBridge::batteryReceived,
                this, &DashboardController::onBatteryReceived,
                Qt::QueuedConnection);
                
        connect(bridge_, &ServiceBridge::allSignalsReceived,
                this, &DashboardController::onAllSignalsReceived,
                Qt::QueuedConnection);
        
        // Timer để request dữ liệu định kỳ
        request_timer_ = new QTimer(this);
        connect(request_timer_, &QTimer::timeout,
                this, &DashboardController::onRequestTimer);
    }
    
    bool start() {
        if (!bridge_->init()) {
            return false;
        }
        bridge_->start();
        
        // Start request timer (mỗi 2 giây)
        request_timer_->start(2000);
        
        return true;
    }

private slots:
    void onServiceAvailable(bool available) {
        qInfo() << "=== Service status:" << (available ? "Connected" : "Disconnected");
    }
    
    void onSpeedReceived(float speed) {
        qInfo() << "Speed:" << speed << "km/h";
    }
    
    void onBatteryReceived(float level, float range) {
        qInfo() << "Battery:" << level << "% | Range:" << range << "km";
    }
    
    void onAllSignalsReceived(const QString& data) {
        qInfo() << "Signals:" << data;
    }
    
    void onRequestTimer() {
        if (bridge_->isAvailable()) {
            bridge_->requestSpeed();
            bridge_->requestBattery();
        }
    }
    
private:
    ServiceBridge* bridge_;
    QTimer* request_timer_;
};

// Include moc file cho Qt meta-object
#include "qt_client_basic.moc"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    
    qInfo() << "=== Qt Console Client Basic ===";
    qInfo() << "Press Ctrl+C to exit";
    
    DashboardController controller;
    if (!controller.start()) {
        qCritical() << "Failed to start controller";
        return 1;
    }
    
    return app.exec();
}
