/**
 * @file servicebridge.cpp
 * @brief ServiceBridge Implementation
 * 
 * ================ MODULE IMPLEMENTATION ================
 */

#include "servicebridge.h"
#include <QDebug>
#include <sstream>

ServiceBridge::ServiceBridge(QObject* parent)
    : QObject(parent)
    , serviceAvailable_(false)
    , running_(false) {
    qDebug() << "[ServiceBridge] Constructor";
}

ServiceBridge::~ServiceBridge() {
    stop();
    qDebug() << "[ServiceBridge] Destructor";
}

// ================================================================
// INITIALIZATION
// ================================================================

bool ServiceBridge::init() {
    qDebug() << "[ServiceBridge] Initializing...";
    
    // Create vsomeip application
    app_ = vsomeip::runtime::get()->create_application("qt_dashboard");
    if (!app_) {
        qCritical() << "[ServiceBridge] Failed to create application";
        return false;
    }
    
    if (!app_->init()) {
        qCritical() << "[ServiceBridge] Failed to init application";
        return false;
    }
    
    // Register state handler
    app_->register_state_handler(
        std::bind(&ServiceBridge::onStateChange, this, std::placeholders::_1));
    
    // Register availability handler
    app_->register_availability_handler(
        ServiceConfig::SERVICE_ID,
        ServiceConfig::INSTANCE_ID,
        std::bind(&ServiceBridge::onAvailability, this,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
    
    // Register message handler
    app_->register_message_handler(
        ServiceConfig::SERVICE_ID,
        ServiceConfig::INSTANCE_ID,
        vsomeip::ANY_METHOD,
        std::bind(&ServiceBridge::onMessage, this, std::placeholders::_1));
    
    qDebug() << "[ServiceBridge] Initialized successfully";
    return true;
}

void ServiceBridge::start() {
    if (running_) return;
    
    running_ = true;
    
    // Request service
    app_->request_service(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID);
    
    // Start vsomeip trong thread riêng
    vsomeipThread_ = std::thread([this]() {
        qDebug() << "[ServiceBridge] vsomeip thread started";
        app_->start();
        qDebug() << "[ServiceBridge] vsomeip thread stopped";
    });
    
    qDebug() << "[ServiceBridge] Started";
}

void ServiceBridge::stop() {
    if (!running_) return;
    
    running_ = false;
    
    if (app_) {
        unsubscribeEvents();
        app_->release_service(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID);
        app_->stop();
    }
    
    if (vsomeipThread_.joinable()) {
        vsomeipThread_.join();
    }
    
    qDebug() << "[ServiceBridge] Stopped";
}

bool ServiceBridge::waitForService(int timeout_ms) {
    QMutexLocker locker(&mutex_);
    return cv_.wait_for(mutex_, std::chrono::milliseconds(timeout_ms),
                        [this] { return serviceAvailable_.load(); });
}

// ================================================================
// VSOMEIP CALLBACKS (chạy trong vsomeip thread)
// ================================================================

void ServiceBridge::onStateChange(vsomeip::state_type_e state) {
    if (state == vsomeip::state_type_e::ST_REGISTERED) {
        qDebug() << "[ServiceBridge] Application registered";
    }
}

void ServiceBridge::onAvailability(vsomeip::service_t service,
                                    vsomeip::instance_t instance,
                                    bool available) {
    qDebug() << "[ServiceBridge] Service" 
             << (available ? "AVAILABLE" : "NOT AVAILABLE");
    
    serviceAvailable_ = available;
    
    if (available) {
        subscribeEvents();
    }
    
    // Emit Qt signal (thread-safe với QueuedConnection)
    emit serviceAvailable(available);
    
    // Notify waiting threads
    cv_.notify_all();
}

void ServiceBridge::onMessage(const std::shared_ptr<vsomeip::message>& message) {
    auto payload = message->get_payload();
    if (!payload || payload->get_length() == 0) return;
    
    std::string data(
        reinterpret_cast<const char*>(payload->get_data()),
        payload->get_length());
    
    auto method = message->get_method();
    auto type = message->get_message_type();
    
    // Event notification
    if (type == vsomeip::message_type_e::MT_NOTIFICATION) {
        handleEventNotification(method, data);
        return;
    }
    
    // Response to method call
    switch (method) {
        case ServiceConfig::METHOD_GET_SPEED:
            handleSpeedResponse(data);
            break;
        case ServiceConfig::METHOD_GET_BATTERY:
            handleBatteryResponse(data);
            break;
        case ServiceConfig::METHOD_GET_WARNINGS:
            handleWarningsResponse(data);
            break;
        case ServiceConfig::METHOD_GET_MODE:
            handleDriveModeResponse(data);
            break;
        case ServiceConfig::METHOD_GET_TEMP:
            handleTemperatureResponse(data);
            break;
        case ServiceConfig::METHOD_SET_MODE:
            handleSetModeResponse(data);
            break;
    }
}

// ================================================================
// RESPONSE HANDLERS
// ================================================================

void ServiceBridge::handleSpeedResponse(const std::string& data) {
    try {
        float speed = std::stof(data);
        emit speedUpdated(speed);
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Speed parse error: %1").arg(e.what()));
    }
}

void ServiceBridge::handleBatteryResponse(const std::string& data) {
    try {
        // Parse: level,voltage,temp,charging,health,range
        std::stringstream ss(data);
        std::string token;
        
        float level = 0, voltage = 0, temp = 0, health = 0, range = 0;
        bool charging = false;
        
        if (std::getline(ss, token, ',')) level = std::stof(token);
        if (std::getline(ss, token, ',')) voltage = std::stof(token);
        if (std::getline(ss, token, ',')) temp = std::stof(token);
        if (std::getline(ss, token, ',')) charging = (token == "1");
        if (std::getline(ss, token, ',')) health = std::stof(token);
        if (std::getline(ss, token, ',')) range = std::stof(token);
        
        emit batteryUpdated(level, voltage, temp, charging, health, range);
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Battery parse error: %1").arg(e.what()));
    }
}

void ServiceBridge::handleWarningsResponse(const std::string& data) {
    QStringList warnings;
    
    if (data != "NO_WARNINGS") {
        std::stringstream ss(data);
        std::string warning;
        while (std::getline(ss, warning, ';')) {
            warnings.append(QString::fromStdString(warning));
        }
    }
    
    emit warningsUpdated(warnings);
}

void ServiceBridge::handleDriveModeResponse(const std::string& data) {
    emit driveModeUpdated(QString::fromStdString(data));
}

void ServiceBridge::handleTemperatureResponse(const std::string& data) {
    try {
        // Parse: motor,cabin,outside,hvac_target
        std::stringstream ss(data);
        std::string token;
        
        float motor = 0, cabin = 0, outside = 0;
        
        if (std::getline(ss, token, ',')) motor = std::stof(token);
        if (std::getline(ss, token, ',')) cabin = std::stof(token);
        if (std::getline(ss, token, ',')) outside = std::stof(token);
        
        emit temperatureUpdated(motor, cabin, outside);
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Temperature parse error: %1").arg(e.what()));
    }
}

void ServiceBridge::handleSetModeResponse(const std::string& data) {
    emit driveModeChangeResult(data == "OK");
}

void ServiceBridge::handleEventNotification(vsomeip::event_t event,
                                             const std::string& data) {
    switch (event) {
        case ServiceConfig::EVENT_SPEED_UPDATE: {
            try {
                float speed = std::stof(data);
                emit speedUpdated(speed);
            } catch (...) {}
            break;
        }
        case ServiceConfig::EVENT_BATTERY_UPDATE: {
            // Parse: level,range
            try {
                std::stringstream ss(data);
                std::string level_str, range_str;
                std::getline(ss, level_str, ',');
                std::getline(ss, range_str, ',');
                // Simplified - emit partial update
                emit batteryUpdated(std::stof(level_str), 0, 0, false, 0, 
                                    std::stof(range_str));
            } catch (...) {}
            break;
        }
        case ServiceConfig::EVENT_ALL_SIGNALS:
            emit allSignalsUpdated(QString::fromStdString(data));
            break;
    }
}

// ================================================================
// REQUEST METHODS (gọi từ Qt thread)
// ================================================================

void ServiceBridge::requestSpeed() {
    if (!serviceAvailable_) return;
    auto request = createRequest(ServiceConfig::METHOD_GET_SPEED);
    sendRequest(request);
}

void ServiceBridge::requestBattery() {
    if (!serviceAvailable_) return;
    auto request = createRequest(ServiceConfig::METHOD_GET_BATTERY);
    sendRequest(request);
}

void ServiceBridge::requestWarnings() {
    if (!serviceAvailable_) return;
    auto request = createRequest(ServiceConfig::METHOD_GET_WARNINGS);
    sendRequest(request);
}

void ServiceBridge::requestDriveMode() {
    if (!serviceAvailable_) return;
    auto request = createRequest(ServiceConfig::METHOD_GET_MODE);
    sendRequest(request);
}

void ServiceBridge::requestTemperature() {
    if (!serviceAvailable_) return;
    auto request = createRequest(ServiceConfig::METHOD_GET_TEMP);
    sendRequest(request);
}

void ServiceBridge::setDriveMode(const QString& mode) {
    if (!serviceAvailable_) {
        emit driveModeChangeResult(false);
        return;
    }
    
    auto request = createRequest(ServiceConfig::METHOD_SET_MODE);
    
    std::string modeStr = mode.toStdString();
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(modeStr.c_str()),
        modeStr.length());
    request->set_payload(payload);
    
    sendRequest(request);
}

void ServiceBridge::requestAllData() {
    requestSpeed();
    requestBattery();
    requestWarnings();
    requestDriveMode();
    requestTemperature();
}

// ================================================================
// HELPER FUNCTIONS
// ================================================================

void ServiceBridge::subscribeEvents() {
    std::set<vsomeip::eventgroup_t> groups = {ServiceConfig::EVENTGROUP_ALL};
    
    // Request events
    app_->request_event(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID,
                        ServiceConfig::EVENT_SPEED_UPDATE, groups,
                        vsomeip::event_type_e::ET_FIELD);
    app_->request_event(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID,
                        ServiceConfig::EVENT_BATTERY_UPDATE, groups,
                        vsomeip::event_type_e::ET_FIELD);
    app_->request_event(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID,
                        ServiceConfig::EVENT_ALL_SIGNALS, groups,
                        vsomeip::event_type_e::ET_FIELD);
    
    // Subscribe
    app_->subscribe(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID,
                    ServiceConfig::EVENTGROUP_ALL);
    
    qDebug() << "[ServiceBridge] Subscribed to events";
}

void ServiceBridge::unsubscribeEvents() {
    app_->unsubscribe(ServiceConfig::SERVICE_ID, ServiceConfig::INSTANCE_ID,
                      ServiceConfig::EVENTGROUP_ALL);
}

std::shared_ptr<vsomeip::message> ServiceBridge::createRequest(vsomeip::method_t method) {
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(ServiceConfig::SERVICE_ID);
    request->set_instance(ServiceConfig::INSTANCE_ID);
    request->set_method(method);
    
    auto payload = vsomeip::runtime::get()->create_payload();
    request->set_payload(payload);
    
    return request;
}

void ServiceBridge::sendRequest(const std::shared_ptr<vsomeip::message>& request) {
    if (app_ && running_) {
        {
            QMutexLocker locker(&pendingMutex_);
            pendingRequests_[request->get_session()] = request->get_method();
        }
        app_->send(request);
    }
}
