#include "serviceclient.h"
#include <QDebug>

ServiceClient::ServiceClient(QObject *parent)
    : QObject(parent)
    , workerThread_(nullptr)
    , running_(false)
    , connected_(false)
{
}

ServiceClient::~ServiceClient() {
    stop();
}

void ServiceClient::start() {
    if (running_) return;
    
    running_ = true;
    
    // Tạo vsomeip trong thread riêng
    workerThread_ = QThread::create([this]() {
        initVsomeip();
        if (app_) {
            app_->start();  // Blocking
        }
    });
    
    workerThread_->start();
    qDebug() << "Service client started";
}

void ServiceClient::stop() {
    if (!running_) return;
    
    running_ = false;
    
    if (app_) {
        app_->clear_all_handler();
        app_->release_service(SERVICE_ID, INSTANCE_ID);
        app_->stop();
    }
    
    if (workerThread_ && workerThread_->isRunning()) {
        workerThread_->quit();
        workerThread_->wait(3000);
        delete workerThread_;
        workerThread_ = nullptr;
    }
    
    qDebug() << "Service client stopped";
}

void ServiceClient::initVsomeip() {
    app_ = vsomeip::runtime::get()->create_application("qt_client");
    
    if (!app_->init()) {
        emit errorOccurred("Failed to init vsomeip");
        return;
    }
    
    // Register handlers
    app_->register_state_handler(
        std::bind(&ServiceClient::onState, this, std::placeholders::_1));
    
    app_->register_availability_handler(SERVICE_ID, INSTANCE_ID,
        std::bind(&ServiceClient::onAvailability, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    app_->register_message_handler(SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
        std::bind(&ServiceClient::onMessage, this, std::placeholders::_1));
    
    app_->request_service(SERVICE_ID, INSTANCE_ID);
    
    qDebug() << "vsomeip initialized";
}

void ServiceClient::onState(vsomeip::state_type_e state) {
    qDebug() << "State:" << (state == vsomeip::state_type_e::ST_REGISTERED 
                             ? "Registered" : "Deregistered");
}

void ServiceClient::onAvailability(vsomeip::service_t, vsomeip::instance_t, bool available) {
    connected_ = available;
    
    if (available) {
        emit connected();
        qDebug() << "Service connected";
    } else {
        emit disconnected();
        qDebug() << "Service disconnected";
    }
}

void ServiceClient::onMessage(const std::shared_ptr<vsomeip::message>& msg) {
    auto payload = msg->get_payload();
    if (!payload || payload->get_length() == 0) return;
    
    auto method = msg->get_method();
    auto data = payload->get_data();
    
    // Xử lý response và emit signal (thread-safe qua queued connection)
    if (method == METHOD_GET_SPEED) {
        uint16_t speed = (data[0] << 8) | data[1];
        emit speedUpdated(speed);
        qDebug() << "Speed received:" << speed;
    } 
    else if (method == METHOD_GET_BATTERY) {
        uint8_t level = data[0];
        emit batteryUpdated(level);
        qDebug() << "Battery received:" << level;
    }
}

void ServiceClient::requestSpeed() {
    if (!connected_ || !app_) return;
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(METHOD_GET_SPEED);
    app_->send(request);
    
    qDebug() << "Speed request sent";
}

void ServiceClient::requestBattery() {
    if (!connected_ || !app_) return;
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(METHOD_GET_BATTERY);
    app_->send(request);
    
    qDebug() << "Battery request sent";
}
