#include "dashboardproxy.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace dashboard {

DashboardProxy::DashboardProxy(QObject *parent)
    : QObject(parent)
    , workerThread_(nullptr)
    , running_(false)
    , serviceAvailable_(false)
{
    // Register metatypes
    qRegisterMetaType<BatteryInfo>("dashboard::BatteryInfo");
    qRegisterMetaType<Warning>("dashboard::Warning");
    qRegisterMetaType<DriveMode>("dashboard::DriveMode");
    qRegisterMetaType<DashboardData>("dashboard::DashboardData");
    qRegisterMetaType<QVector<Warning>>("QVector<dashboard::Warning>");
}

DashboardProxy::~DashboardProxy() {
    stop();
}

DashboardProxy* DashboardProxy::FindService(QObject* parent) {
    auto* proxy = new DashboardProxy(parent);
    proxy->start();
    return proxy;
}

void DashboardProxy::start() {
    if (running_) return;
    
    running_ = true;
    
    workerThread_ = QThread::create([this]() {
        qDebug() << "[Proxy] Creating vsomeip application...";
        
        app_ = vsomeip::runtime::get()->create_application("dashboard_qt_client");
        
        if (!app_->init()) {
            emit errorOccurred("Failed to initialize vsomeip");
            return;
        }
        
        // Register handlers
        app_->register_state_handler(
            std::bind(&DashboardProxy::onState, this, std::placeholders::_1));
        
        app_->register_availability_handler(SERVICE_ID, INSTANCE_ID,
            std::bind(&DashboardProxy::onAvailability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        
        app_->register_message_handler(SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&DashboardProxy::onMessage, this, std::placeholders::_1));
        
        // Request service
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        qDebug() << "[Proxy] Starting vsomeip...";
        app_->start();  // Blocking
    });
    
    workerThread_->start();
}

void DashboardProxy::stop() {
    if (!running_) return;
    
    qDebug() << "[Proxy] Stopping...";
    running_ = false;
    
    if (serviceAvailable_ && app_) {
        app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        app_->release_service(SERVICE_ID, INSTANCE_ID);
    }
    
    if (app_) {
        app_->clear_all_handler();
        app_->stop();
    }
    
    if (workerThread_ && workerThread_->isRunning()) {
        workerThread_->quit();
        workerThread_->wait(3000);
        delete workerThread_;
        workerThread_ = nullptr;
    }
    
    qDebug() << "[Proxy] Stopped";
}

void DashboardProxy::onState(vsomeip::state_type_e state) {
    qDebug() << "[Proxy] State:" 
             << (state == vsomeip::state_type_e::ST_REGISTERED ? "Registered" : "Deregistered");
}

void DashboardProxy::onAvailability(vsomeip::service_t, vsomeip::instance_t, bool available) {
    qDebug() << "[Proxy] Service" << (available ? "available" : "unavailable");
    
    serviceAvailable_ = available;
    
    if (available) {
        subscribeAllSignals();
        emit serviceAvailable();
    } else {
        emit serviceUnavailable();
    }
}

void DashboardProxy::onMessage(const std::shared_ptr<vsomeip::message>& msg) {
    auto payload = msg->get_payload();
    if (!payload || payload->get_length() == 0) return;
    
    auto msgType = msg->get_message_type();
    auto method = msg->get_method();
    
    // Event notification
    if (msgType == vsomeip::message_type_e::MT_NOTIFICATION) {
        if (method == EVENT_ALL_SIGNALS) {
            parseAllSignalsEvent(payload);
        }
        return;
    }
    
    // Method response
    auto data = payload->get_data();
    auto len = payload->get_length();
    
    switch (method) {
        case METHOD_GET_SPEED: {
            if (len >= 2) {
                int speed = (data[0] << 8) | data[1];
                emit speedReceived(speed);
            }
            break;
        }
        
        case METHOD_GET_BATTERY: {
            if (len >= 6) {
                BatteryInfo battery;
                battery.level = data[0];
                battery.voltage = ((data[1] << 8) | data[2]) / 100.0f;
                battery.temperature = static_cast<int16_t>((data[3] << 8) | data[4]);
                battery.charging = data[5] != 0;
                emit batteryReceived(battery);
            }
            break;
        }
        
        case METHOD_GET_WARNINGS: {
            QString jsonStr = QString::fromUtf8(
                reinterpret_cast<const char*>(data), len);
            QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
            
            QVector<Warning> warnings;
            for (const auto& item : doc.array()) {
                QJsonObject obj = item.toObject();
                Warning w;
                w.code = obj["code"].toString();
                w.message = obj["message"].toString();
                w.severity = obj["severity"].toInt();
                warnings.append(w);
            }
            emit warningsReceived(warnings);
            break;
        }
        
        case METHOD_GET_DRIVEMODE: {
            if (len >= 1) {
                emit driveModeReceived(static_cast<DriveMode>(data[0]));
            }
            break;
        }
    }
}

void DashboardProxy::parseAllSignalsEvent(const std::shared_ptr<vsomeip::payload>& payload) {
    QString jsonStr = QString::fromUtf8(
        reinterpret_cast<const char*>(payload->get_data()),
        payload->get_length());
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = doc.object();
    
    DashboardData data;
    data.speed = obj["speed"].toInt();
    data.rpm = obj["rpm"].toInt();
    data.gear = obj["gear"].toInt();
    data.driveMode = stringToDriveMode(obj["driveMode"].toString());
    data.timestamp = obj["timestamp"].toVariant().toLongLong();
    
    // Battery
    QJsonObject battObj = obj["battery"].toObject();
    data.battery.level = battObj["level"].toInt();
    data.battery.voltage = battObj["voltage"].toDouble();
    data.battery.temperature = battObj["temp"].toInt();
    data.battery.charging = battObj["charging"].toBool();
    
    emit allDataReceived(data);
    emit speedReceived(data.speed);
    emit rpmReceived(data.rpm);
    emit batteryReceived(data.battery);
    emit driveModeReceived(data.driveMode);
}

void DashboardProxy::subscribeAllSignals() {
    if (!app_ || !serviceAvailable_) return;
    
    std::set<vsomeip::eventgroup_t> groups;
    groups.insert(EVENTGROUP_ID);
    
    app_->request_event(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS,
        groups, vsomeip::event_type_e::ET_FIELD);
    
    app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
    
    qDebug() << "[Proxy] Subscribed to events";
}

void DashboardProxy::unsubscribeAllSignals() {
    if (!app_) return;
    app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
}

void DashboardProxy::sendRequest(vsomeip::method_t method) {
    if (!app_ || !serviceAvailable_) {
        emit errorOccurred("Service not available");
        return;
    }
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(method);
    app_->send(request);
}

void DashboardProxy::getSpeed() {
    sendRequest(METHOD_GET_SPEED);
}

void DashboardProxy::getBattery() {
    sendRequest(METHOD_GET_BATTERY);
}

void DashboardProxy::getWarnings() {
    sendRequest(METHOD_GET_WARNINGS);
}

void DashboardProxy::getDriveMode() {
    sendRequest(METHOD_GET_DRIVEMODE);
}

void DashboardProxy::getAllData() {
    getSpeed();
    getBattery();
    getWarnings();
    getDriveMode();
}

}  // namespace dashboard
