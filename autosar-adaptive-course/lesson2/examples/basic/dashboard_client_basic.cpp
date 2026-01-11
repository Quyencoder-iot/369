/**
 * @file dashboard_client_basic.cpp
 * @brief VÍ DỤ 1 - DashboardClient cơ bản
 * 
 * Các yếu tố BẮT BUỘC:
 * 1. Find service
 * 2. Gọi các methods: getSpeed, getBattery, getWarnings, getDriveMode
 * 3. Subscribe và nhận events
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

// ========== SERVICE IDs (phải khớp với service) ==========
#define SERVICE_ID          0x2000
#define INSTANCE_ID         0x0001
#define METHOD_GET_SPEED    0x0001
#define METHOD_GET_BATTERY  0x0002
#define METHOD_GET_WARNINGS 0x0003
#define METHOD_GET_MODE     0x0004
#define EVENT_ALL_SIGNALS   0x8001
#define EVENTGROUP_ID       0x0001

std::shared_ptr<vsomeip::application> app;
std::condition_variable cv;
std::mutex mtx;
bool service_available = false;

// ========== Handler nhận response ==========
void onResponse(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    std::string data(
        reinterpret_cast<const char*>(payload->get_data()),
        payload->get_length()
    );
    
    std::cout << "[RESPONSE] Method 0x" << std::hex 
              << response->get_method() << ": " << data << std::endl;
}

// ========== Handler nhận event ==========
void onEvent(const std::shared_ptr<vsomeip::message>& event) {
    auto payload = event->get_payload();
    std::string data(
        reinterpret_cast<const char*>(payload->get_data()),
        payload->get_length()
    );
    
    std::cout << "[EVENT] All Signals: " << data << std::endl;
}

// ========== Handler service availability ==========
void onAvailability(vsomeip::service_t service, vsomeip::instance_t instance, 
                    bool available) {
    std::cout << "Service " << (available ? "AVAILABLE" : "NOT AVAILABLE") 
              << std::endl;
    service_available = available;
    cv.notify_one();
}

// ========== Gửi request ==========
void sendRequest(vsomeip::method_t method) {
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(method);
    
    auto payload = vsomeip::runtime::get()->create_payload();
    request->set_payload(payload);
    
    app->send(request);
}

// ========== Thread gửi requests ==========
void requestLoop() {
    // Đợi service available
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return service_available; });
    lock.unlock();
    
    std::cout << "\n=== Sending Requests ===" << std::endl;
    
    while (service_available) {
        // Gọi tất cả các methods
        sendRequest(METHOD_GET_SPEED);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        sendRequest(METHOD_GET_BATTERY);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        sendRequest(METHOD_GET_WARNINGS);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        sendRequest(METHOD_GET_MODE);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        std::cout << "---" << std::endl;
    }
}

int main() {
    std::cout << "=== DashboardClient Basic ===" << std::endl;
    
    // 1. Create application
    app = vsomeip::runtime::get()->create_application("dashboard_client");
    if (!app->init()) {
        std::cerr << "Failed to init" << std::endl;
        return 1;
    }
    
    // 2. Register handlers
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, 
        vsomeip::ANY_METHOD, onResponse);
    
    app->register_availability_handler(SERVICE_ID, INSTANCE_ID, 
        onAvailability);
    
    // 3. Request service
    app->request_service(SERVICE_ID, INSTANCE_ID);
    
    // 4. Subscribe to events
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_ID);
    app->request_event(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS,
                       eventgroups, vsomeip::event_type_e::ET_FIELD);
    app->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
    
    // 5. Start request thread
    std::thread requester(requestLoop);
    
    std::cout << "Waiting for service..." << std::endl;
    
    // 6. Start application
    app->start();
    
    requester.join();
    return 0;
}
