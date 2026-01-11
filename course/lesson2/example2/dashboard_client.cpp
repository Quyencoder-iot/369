// ============================================
// DASHBOARD CLIENT - Test client
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>
#include <iomanip>

// Service IDs - phải khớp với service
static constexpr vsomeip::service_t SERVICE_ID = 0x2000;
static constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_SPEED = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_BATTERY = 0x0002;
static constexpr vsomeip::method_t METHOD_GET_WARNINGS = 0x0003;
static constexpr vsomeip::method_t METHOD_GET_DRIVEMODE = 0x0004;
static constexpr vsomeip::event_t EVENT_ALL_SIGNALS = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;

class DashboardClient {
public:
    DashboardClient() 
        : running_(false)
        , service_available_(false)
        , event_count_(0)
        , response_count_(0)
    {
        app_ = vsomeip::runtime::get()->create_application("dashboard_client");
    }
    
    ~DashboardClient() {
        Stop();
    }
    
    bool Init() {
        if (!app_->init()) {
            std::cerr << "[ERROR] Failed to init application" << std::endl;
            return false;
        }
        
        // State handler
        app_->register_state_handler(
            std::bind(&DashboardClient::OnState, this, std::placeholders::_1));
        
        // Availability handler
        app_->register_availability_handler(
            SERVICE_ID, INSTANCE_ID,
            std::bind(&DashboardClient::OnAvailability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        
        // Message handler (for responses and events)
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&DashboardClient::OnMessage, this, std::placeholders::_1));
        
        // Request service
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        std::cout << "[INIT] Dashboard Client initialized" << std::endl;
        return true;
    }
    
    void Start() {
        running_ = true;
        
        // Start request thread
        requestThread_ = std::thread(&DashboardClient::RequestLoop, this);
        
        std::cout << "[START] Dashboard Client starting..." << std::endl;
        app_->start();  // Blocking
    }
    
    void Stop() {
        if (!running_) return;
        
        running_ = false;
        
        // Unsubscribe and release
        app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        app_->release_service(SERVICE_ID, INSTANCE_ID);
        
        if (requestThread_.joinable()) {
            requestThread_.join();
        }
        
        app_->clear_all_handler();
        app_->stop();
        
        std::cout << "\n[STOP] Dashboard Client stopped" << std::endl;
        std::cout << "       Events received: " << event_count_ << std::endl;
        std::cout << "       Responses received: " << response_count_ << std::endl;
    }

private:
    void OnState(vsomeip::state_type_e state) {
        std::cout << "[STATE] " 
            << (state == vsomeip::state_type_e::ST_REGISTERED 
                ? "Registered with routing" : "Deregistered") 
            << std::endl;
    }
    
    void OnAvailability(vsomeip::service_t service, 
                        vsomeip::instance_t instance, 
                        bool available) {
        std::cout << "[AVAIL] Service 0x" << std::hex << service 
            << " instance 0x" << instance << std::dec
            << " is " << (available ? "AVAILABLE" : "UNAVAILABLE") 
            << std::endl;
        
        service_available_ = available;
        
        if (available) {
            SubscribeToEvents();
        }
    }
    
    void SubscribeToEvents() {
        std::set<vsomeip::eventgroup_t> groups;
        groups.insert(EVENTGROUP_ID);
        
        app_->request_event(
            SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS,
            groups, vsomeip::event_type_e::ET_FIELD);
        
        app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        
        std::cout << "[SUBSCRIBE] Subscribed to events" << std::endl;
    }
    
    void OnMessage(const std::shared_ptr<vsomeip::message>& msg) {
        auto payload = msg->get_payload();
        
        if (msg->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            // Event notification
            event_count_++;
            HandleEvent(payload);
        } else {
            // Method response
            response_count_++;
            HandleResponse(msg->get_method(), payload);
        }
    }
    
    void HandleEvent(const std::shared_ptr<vsomeip::payload>& payload) {
        if (!payload || payload->get_length() == 0) return;
        
        std::string data(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length());
        
        // Parse and display
        std::cout << "\r[EVENT #" << std::setw(4) << event_count_ << "] " 
                  << data.substr(0, 80) 
                  << (data.length() > 80 ? "..." : "")
                  << "          " << std::flush;
    }
    
    void HandleResponse(vsomeip::method_t method, 
                        const std::shared_ptr<vsomeip::payload>& payload) {
        if (!payload || payload->get_length() == 0) {
            std::cout << "\n[RESP] Method 0x" << std::hex << method 
                      << std::dec << ": (empty)" << std::endl;
            return;
        }
        
        std::cout << "\n[RESP] ";
        
        switch (method) {
            case METHOD_GET_SPEED: {
                auto data = payload->get_data();
                uint16_t speed = (data[0] << 8) | data[1];
                std::cout << "Speed: " << speed << " km/h";
                break;
            }
            case METHOD_GET_BATTERY: {
                auto data = payload->get_data();
                uint8_t level = data[0];
                float voltage = ((data[1] << 8) | data[2]) / 100.0f;
                int16_t temp = (data[3] << 8) | data[4];
                bool charging = data[5] != 0;
                
                std::cout << "Battery: " << (int)level << "%, "
                          << voltage << "V, " << temp << "°C"
                          << (charging ? " [CHARGING]" : "");
                break;
            }
            case METHOD_GET_WARNINGS: {
                std::string data(
                    reinterpret_cast<const char*>(payload->get_data()),
                    payload->get_length());
                std::cout << "Warnings: " << data;
                break;
            }
            case METHOD_GET_DRIVEMODE: {
                uint8_t mode = payload->get_data()[0];
                const char* modeStr[] = {"ECO", "NORMAL", "SPORT", "SNOW"};
                std::cout << "Drive Mode: " << modeStr[mode % 4];
                break;
            }
            default:
                std::cout << "Unknown method 0x" << std::hex << method << std::dec;
        }
        
        std::cout << std::endl;
    }
    
    void RequestLoop() {
        int counter = 0;
        const vsomeip::method_t methods[] = {
            METHOD_GET_SPEED,
            METHOD_GET_BATTERY,
            METHOD_GET_WARNINGS,
            METHOD_GET_DRIVEMODE
        };
        const char* methodNames[] = {
            "GET_SPEED", "GET_BATTERY", "GET_WARNINGS", "GET_DRIVEMODE"
        };
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            
            if (!running_ || !service_available_) continue;
            
            // Call next method
            int idx = counter++ % 4;
            vsomeip::method_t method = methods[idx];
            
            auto request = vsomeip::runtime::get()->create_request();
            request->set_service(SERVICE_ID);
            request->set_instance(INSTANCE_ID);
            request->set_method(method);
            
            app_->send(request);
            std::cout << "\n[REQ] Called " << methodNames[idx] << std::endl;
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread requestThread_;
    std::atomic<bool> running_;
    std::atomic<bool> service_available_;
    std::atomic<uint32_t> event_count_;
    std::atomic<uint32_t> response_count_;
};

// Global for signal handler
DashboardClient* g_client = nullptr;

void signal_handler(int sig) {
    std::cout << "\n[SIGNAL] Received " << sig << std::endl;
    if (g_client) {
        g_client->Stop();
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   DASHBOARD CLIENT" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    DashboardClient client;
    g_client = &client;
    
    if (client.Init()) {
        client.Start();
    }
    
    return 0;
}
