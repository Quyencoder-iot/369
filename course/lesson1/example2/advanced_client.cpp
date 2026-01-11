// ============================================
// VÍ DỤ 2: ADVANCED CLIENT - CODE ĐẦY ĐỦ
// 
// Module 1: Includes & Definitions
// Module 2: Application Class
// Module 3: Service Discovery
// Module 4: Event Subscription
// Module 5: Method Invocation
// Module 6: Main Entry Point
// ============================================

// =============================================
// MODULE 1: INCLUDES & DEFINITIONS
// ---------------------------------------------
// Mục đích: Khai báo thư viện và hằng số
// =============================================
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <csignal>

// Service identifiers - PHẢI KHỚP VỚI SERVICE
static constexpr vsomeip::service_t SERVICE_ID      = 0x1234;
static constexpr vsomeip::instance_t INSTANCE_ID    = 0x5678;
static constexpr vsomeip::method_t METHOD_GET_TIME  = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_DATA  = 0x0002;
static constexpr vsomeip::event_t EVENT_NOTIFY      = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;


// =============================================
// MODULE 2: APPLICATION CLASS
// ---------------------------------------------
// Mục đích: Đóng gói logic client
// Giải thích:
// - Quản lý kết nối tới service
// - Xử lý reconnect khi service unavailable
// =============================================
class AdvancedClient {
public:
    AdvancedClient()
        : service_available_(false)
        , running_(false)
    {
        app_ = vsomeip::runtime::get()->create_application("advanced_client");
    }
    
    ~AdvancedClient() {
        stop();
    }
    
    // =============================================
    // MODULE 3: SERVICE DISCOVERY
    // ---------------------------------------------
    // Mục đích: Tìm và kết nối tới service
    // Giải thích:
    // - request_service(): Yêu cầu tìm service
    // - availability_handler: Callback khi service có/mất
    // - Tự động reconnect khi service restart
    // =============================================
    
    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize client" << std::endl;
            return false;
        }
        
        // State handler
        app_->register_state_handler(
            std::bind(&AdvancedClient::on_state, this, std::placeholders::_1)
        );
        
        // Availability handler - được gọi khi service available/unavailable
        app_->register_availability_handler(
            SERVICE_ID, INSTANCE_ID,
            std::bind(&AdvancedClient::on_availability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        
        // Message handler cho responses
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&AdvancedClient::on_message, this, std::placeholders::_1)
        );
        
        // Request service
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        std::cout << "[INIT] Client initialized" << std::endl;
        return true;
    }
    
    void start() {
        running_ = true;
        
        // Thread gọi methods định kỳ
        request_thread_ = std::thread(&AdvancedClient::request_loop, this);
        
        // Start application
        app_->start();
    }
    
    void stop() {
        running_ = false;
        
        // Unsubscribe events
        app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        
        // Release service
        app_->release_service(SERVICE_ID, INSTANCE_ID);
        
        if (request_thread_.joinable()) {
            request_thread_.join();
        }
        
        app_->clear_all_handler();
        app_->stop();
        
        std::cout << "[STOP] Client stopped" << std::endl;
    }

private:
    void on_state(vsomeip::state_type_e state) {
        std::cout << "[STATE] " 
            << (state == vsomeip::state_type_e::ST_REGISTERED ? "Registered" : "Deregistered")
            << std::endl;
    }
    
    void on_availability(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
        std::cout << "[AVAIL] Service 0x" << std::hex << service 
            << " instance 0x" << instance << std::dec
            << " is " << (available ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
        
        service_available_ = available;
        
        if (available) {
            // Subscribe to events khi service available
            subscribe_events();
        }
    }
    
    // =============================================
    // MODULE 4: EVENT SUBSCRIPTION
    // ---------------------------------------------
    // Mục đích: Đăng ký nhận events từ service
    // Giải thích:
    // - request_event(): Yêu cầu event
    // - subscribe(): Đăng ký eventgroup
    // - Events được nhận qua on_message()
    // =============================================
    
    void subscribe_events() {
        // Request event trước khi subscribe
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(EVENTGROUP_ID);
        
        app_->request_event(
            SERVICE_ID,
            INSTANCE_ID,
            EVENT_NOTIFY,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD
        );
        
        // Subscribe tới eventgroup
        app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        
        std::cout << "[SUBSCRIBE] Subscribed to eventgroup 0x" 
            << std::hex << EVENTGROUP_ID << std::dec << std::endl;
    }
    
    // =============================================
    // MODULE 5: METHOD INVOCATION
    // ---------------------------------------------
    // Mục đích: Gọi remote methods
    // Giải thích:
    // - Tạo request message với service/method ID
    // - Gửi request với app_->send()
    // - Response được nhận qua on_message()
    // =============================================
    
    void request_loop() {
        int request_count = 0;
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            
            if (!running_ || !service_available_) continue;
            
            request_count++;
            
            // Xen kẽ gọi GET_TIME và GET_DATA
            if (request_count % 2 == 0) {
                call_get_time();
            } else {
                call_get_data();
            }
        }
    }
    
    void call_get_time() {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_GET_TIME);
        request->set_reliable(false);  // UDP
        
        app_->send(request);
        std::cout << "[REQUEST] Called GET_TIME" << std::endl;
    }
    
    void call_get_data() {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_GET_DATA);
        
        // Gửi payload với request
        auto payload = vsomeip::runtime::get()->create_payload();
        std::string data = "give_me_all_data";
        std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
        payload->set_data(payload_data);
        request->set_payload(payload);
        
        app_->send(request);
        std::cout << "[REQUEST] Called GET_DATA" << std::endl;
    }
    
    // Message handler - nhận cả responses và events
    void on_message(const std::shared_ptr<vsomeip::message>& message) {
        auto payload = message->get_payload();
        std::string data;
        
        if (payload && payload->get_length() > 0) {
            data = std::string(
                payload->get_data(),
                payload->get_data() + payload->get_length()
            );
        }
        
        // Phân biệt event và response
        if (message->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            std::cout << "[EVENT] Received notification: " << data << std::endl;
        } else {
            vsomeip::method_t method = message->get_method();
            std::cout << "[RESPONSE] Method 0x" << std::hex << method 
                << std::dec << ": " << data << std::endl;
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread request_thread_;
    std::atomic<bool> service_available_;
    std::atomic<bool> running_;
};


// =============================================
// MODULE 6: MAIN ENTRY POINT
// =============================================

AdvancedClient* g_client = nullptr;

void signal_handler(int signal) {
    std::cout << "\n[SIGNAL] Shutting down..." << std::endl;
    if (g_client) {
        g_client->stop();
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   ADVANCED SOMEIP CLIENT" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    AdvancedClient client;
    g_client = &client;
    
    if (client.init()) {
        client.start();
    }
    
    return 0;
}
