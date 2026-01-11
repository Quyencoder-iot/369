/**
 * @file hello_client_advanced.cpp
 * @brief VÍ DỤ 2 - Client SOME/IP hoàn chỉnh với đầy đủ tính năng
 * 
 * ================ MODULE ARCHITECTURE ================
 * 
 * Module 1: Configuration (Shared with Service)
 * Module 2: ara:: namespace simulation
 * Module 3: ServiceProxy Class
 * Module 4: Main Application with Interactive Menu
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <chrono>
#include <csignal>
#include <memory>
#include <future>

// ================================================================
// MODULE 1: CONFIGURATION (phải khớp với Service)
// ================================================================

namespace config {
    constexpr vsomeip::service_t SERVICE_ID     = 0x1234;
    constexpr vsomeip::instance_t INSTANCE_ID   = 0x5678;
    constexpr vsomeip::major_version_t MAJOR_V  = 1;
    constexpr vsomeip::minor_version_t MINOR_V  = 0;
    
    constexpr vsomeip::method_t METHOD_SAY_HELLO    = 0x0001;
    constexpr vsomeip::method_t METHOD_GET_TIME     = 0x0002;
    constexpr vsomeip::method_t METHOD_ADD_NUMBERS  = 0x0003;
    
    constexpr vsomeip::event_t EVENT_HEARTBEAT      = 0x8001;
    constexpr vsomeip::eventgroup_t EVENTGROUP_ALL  = 0x0001;
    
    constexpr int REQUEST_TIMEOUT_MS = 5000;
}

// ================================================================
// MODULE 2: ARA:: NAMESPACE SIMULATION
// ================================================================

namespace ara {
namespace log {
    enum class LogLevel { kDebug, kInfo, kWarning, kError, kFatal };
    
    class Logger {
    public:
        static Logger& GetInstance() {
            static Logger instance;
            return instance;
        }
        
        void Log(LogLevel level, const std::string& msg) {
            std::lock_guard<std::mutex> lock(mutex_);
            std::string prefix;
            switch (level) {
                case LogLevel::kDebug:   prefix = "[DEBUG]"; break;
                case LogLevel::kInfo:    prefix = "[INFO]"; break;
                case LogLevel::kWarning: prefix = "[WARN]"; break;
                case LogLevel::kError:   prefix = "[ERROR]"; break;
                case LogLevel::kFatal:   prefix = "[FATAL]"; break;
            }
            std::cout << prefix << " " << msg << std::endl;
        }
        
        void Debug(const std::string& msg) { Log(LogLevel::kDebug, msg); }
        void Info(const std::string& msg) { Log(LogLevel::kInfo, msg); }
        void Warn(const std::string& msg) { Log(LogLevel::kWarning, msg); }
        void Error(const std::string& msg) { Log(LogLevel::kError, msg); }
        
    private:
        std::mutex mutex_;
    };
    
    #define LOG_DEBUG(msg) ara::log::Logger::GetInstance().Debug(msg)
    #define LOG_INFO(msg)  ara::log::Logger::GetInstance().Info(msg)
    #define LOG_WARN(msg)  ara::log::Logger::GetInstance().Warn(msg)
    #define LOG_ERROR(msg) ara::log::Logger::GetInstance().Error(msg)
}

namespace core {
    /**
     * @brief Future với Then() callback - mô phỏng ara::core::Future
     */
    template<typename T>
    class Future {
    public:
        using Callback = std::function<void(const T&)>;
        
        Future() : ready_(false) {}
        
        void Then(Callback cb) {
            std::lock_guard<std::mutex> lock(mutex_);
            callback_ = cb;
            if (ready_ && callback_) {
                callback_(value_);
            }
        }
        
        void SetValue(const T& value) {
            std::lock_guard<std::mutex> lock(mutex_);
            value_ = value;
            ready_ = true;
            if (callback_) {
                callback_(value_);
            }
            cv_.notify_all();
        }
        
        T Get(int timeout_ms = 0) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (timeout_ms > 0) {
                cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                             [this] { return ready_; });
            } else {
                cv_.wait(lock, [this] { return ready_; });
            }
            return value_;
        }
        
        bool IsReady() const { return ready_; }
        
    private:
        T value_;
        std::atomic<bool> ready_;
        Callback callback_;
        std::mutex mutex_;
        std::condition_variable cv_;
    };
}

namespace com {
    /**
     * @brief Mô phỏng ara::com::FindService
     */
    template<typename ProxyType>
    class ServiceFinder {
    public:
        using FindCallback = std::function<void(bool available)>;
        
        static void FindService(vsomeip::service_t service,
                                vsomeip::instance_t instance,
                                FindCallback callback) {
            // Implementation trong proxy
            if (callback) callback(true);
        }
    };
}
}

// ================================================================
// MODULE 3: SERVICE PROXY CLASS
// ================================================================

/**
 * @brief HelloServiceProxy - Client-side proxy cho HelloService
 * 
 * Theo mô hình AUTOSAR Adaptive:
 * - ServiceProxy đại diện cho remote service trên client
 * - Cung cấp methods để gọi remote methods
 * - Subscribe events
 * - Handle reconnection
 */
class HelloServiceProxy {
public:
    using AvailabilityCallback = std::function<void(bool)>;
    using EventCallback = std::function<void(const std::string&)>;
    
    HelloServiceProxy() 
        : service_available_(false), running_(false) {
        LOG_INFO("HelloServiceProxy constructor");
    }
    
    ~HelloServiceProxy() {
        Stop();
        LOG_INFO("HelloServiceProxy destructor");
    }
    
    /**
     * @brief Initialize proxy
     */
    bool Init() {
        LOG_INFO("Initializing proxy...");
        
        app_ = vsomeip::runtime::get()->create_application("hello_client");
        if (!app_ || !app_->init()) {
            LOG_ERROR("Failed to initialize application");
            return false;
        }
        
        // Register state handler
        app_->register_state_handler(
            std::bind(&HelloServiceProxy::OnStateChange, this,
                      std::placeholders::_1));
        
        // Register availability handler
        app_->register_availability_handler(
            config::SERVICE_ID, config::INSTANCE_ID,
            std::bind(&HelloServiceProxy::OnAvailability, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));
        
        // Register message handler cho responses
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID,
            vsomeip::ANY_METHOD,
            std::bind(&HelloServiceProxy::OnMessage, this,
                      std::placeholders::_1));
        
        LOG_INFO("Proxy initialized");
        return true;
    }
    
    /**
     * @brief Start proxy
     */
    void Start() {
        running_ = true;
        
        // Request service
        app_->request_service(
            config::SERVICE_ID, config::INSTANCE_ID,
            config::MAJOR_V, config::MINOR_V);
        
        // Subscribe to events
        SubscribeEvents();
        
        LOG_INFO("Starting proxy...");
        
        // Start app in background thread
        app_thread_ = std::thread([this]() {
            app_->start();
        });
    }
    
    /**
     * @brief Stop proxy
     */
    void Stop() {
        running_ = false;
        
        if (app_) {
            app_->release_service(config::SERVICE_ID, config::INSTANCE_ID);
            app_->stop();
        }
        
        if (app_thread_.joinable()) {
            app_thread_.join();
        }
        
        LOG_INFO("Proxy stopped");
    }
    
    /**
     * @brief Set callback for availability changes
     */
    void SetAvailabilityCallback(AvailabilityCallback cb) {
        availability_callback_ = cb;
    }
    
    /**
     * @brief Set callback for heartbeat events
     */
    void SetEventCallback(EventCallback cb) {
        event_callback_ = cb;
    }
    
    /**
     * @brief Wait for service to become available
     */
    bool WaitForService(int timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(availability_mutex_);
        if (timeout_ms > 0) {
            return availability_cv_.wait_for(
                lock, 
                std::chrono::milliseconds(timeout_ms),
                [this] { return service_available_.load(); });
        } else {
            availability_cv_.wait(lock, 
                [this] { return service_available_.load(); });
            return true;
        }
    }
    
    /**
     * @brief Check if service is available
     */
    bool IsAvailable() const {
        return service_available_;
    }
    
    // ========== METHOD CALLS ==========
    
    /**
     * @brief Gọi method sayHello (async với Future)
     */
    std::shared_ptr<ara::core::Future<std::string>> SayHello(const std::string& name) {
        auto future = std::make_shared<ara::core::Future<std::string>>();
        
        if (!service_available_) {
            future->SetValue("Error: Service not available");
            return future;
        }
        
        // Create request
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(config::SERVICE_ID);
        request->set_instance(config::INSTANCE_ID);
        request->set_method(config::METHOD_SAY_HELLO);
        
        // Set payload
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(name.c_str()),
            name.length());
        request->set_payload(payload);
        
        // Store future for response handling
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = future;
        }
        
        // Send request
        app_->send(request);
        LOG_DEBUG("SayHello request sent");
        
        return future;
    }
    
    /**
     * @brief Gọi method getTime (async với Future)
     */
    std::shared_ptr<ara::core::Future<std::string>> GetTime() {
        auto future = std::make_shared<ara::core::Future<std::string>>();
        
        if (!service_available_) {
            future->SetValue("Error: Service not available");
            return future;
        }
        
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(config::SERVICE_ID);
        request->set_instance(config::INSTANCE_ID);
        request->set_method(config::METHOD_GET_TIME);
        
        // Empty payload
        auto payload = vsomeip::runtime::get()->create_payload();
        request->set_payload(payload);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = future;
        }
        
        app_->send(request);
        LOG_DEBUG("GetTime request sent");
        
        return future;
    }
    
    /**
     * @brief Gọi method addNumbers (async với Future)
     */
    std::shared_ptr<ara::core::Future<std::string>> AddNumbers(int a, int b) {
        auto future = std::make_shared<ara::core::Future<std::string>>();
        
        if (!service_available_) {
            future->SetValue("Error: Service not available");
            return future;
        }
        
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(config::SERVICE_ID);
        request->set_instance(config::INSTANCE_ID);
        request->set_method(config::METHOD_ADD_NUMBERS);
        
        // Format: "a,b"
        std::string data = std::to_string(a) + "," + std::to_string(b);
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(data.c_str()),
            data.length());
        request->set_payload(payload);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = future;
        }
        
        app_->send(request);
        LOG_DEBUG("AddNumbers request sent");
        
        return future;
    }

private:
    /**
     * @brief Subscribe to events
     */
    void SubscribeEvents() {
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(config::EVENTGROUP_ALL);
        
        app_->request_event(
            config::SERVICE_ID,
            config::INSTANCE_ID,
            config::EVENT_HEARTBEAT,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD);
            
        app_->subscribe(
            config::SERVICE_ID,
            config::INSTANCE_ID,
            config::EVENTGROUP_ALL);
            
        LOG_INFO("Subscribed to events");
    }
    
    /**
     * @brief Handle state changes
     */
    void OnStateChange(vsomeip::state_type_e state) {
        if (state == vsomeip::state_type_e::ST_REGISTERED) {
            LOG_INFO("Client registered with routing manager");
        }
    }
    
    /**
     * @brief Handle service availability changes
     */
    void OnAvailability(vsomeip::service_t service,
                        vsomeip::instance_t instance,
                        bool available) {
        LOG_INFO("Service availability: " + 
                 std::string(available ? "AVAILABLE" : "NOT AVAILABLE"));
        
        service_available_ = available;
        
        if (availability_callback_) {
            availability_callback_(available);
        }
        
        availability_cv_.notify_all();
    }
    
    /**
     * @brief Handle incoming messages (responses and events)
     */
    void OnMessage(const std::shared_ptr<vsomeip::message>& message) {
        // Check if it's an event notification
        if (message->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            HandleEvent(message);
            return;
        }
        
        // It's a response
        HandleResponse(message);
    }
    
    /**
     * @brief Handle response messages
     */
    void HandleResponse(const std::shared_ptr<vsomeip::message>& response) {
        auto session = response->get_session();
        
        std::shared_ptr<ara::core::Future<std::string>> future;
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            auto it = pending_requests_.find(session);
            if (it != pending_requests_.end()) {
                future = it->second;
                pending_requests_.erase(it);
            }
        }
        
        if (future) {
            auto payload = response->get_payload();
            std::string data(
                reinterpret_cast<const char*>(payload->get_data()),
                payload->get_length());
            
            LOG_DEBUG("Response received: " + data);
            future->SetValue(data);
        }
    }
    
    /**
     * @brief Handle event notifications
     */
    void HandleEvent(const std::shared_ptr<vsomeip::message>& event) {
        auto payload = event->get_payload();
        if (payload && payload->get_length() > 0) {
            std::string data(
                reinterpret_cast<const char*>(payload->get_data()),
                payload->get_length());
            
            LOG_DEBUG("Event received: " + data);
            
            if (event_callback_) {
                event_callback_(data);
            }
        }
    }
    
private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread app_thread_;
    std::atomic<bool> service_available_;
    std::atomic<bool> running_;
    
    AvailabilityCallback availability_callback_;
    EventCallback event_callback_;
    
    std::mutex availability_mutex_;
    std::condition_variable availability_cv_;
    
    std::mutex pending_mutex_;
    std::map<vsomeip::session_t, 
             std::shared_ptr<ara::core::Future<std::string>>> pending_requests_;
};

// ================================================================
// MODULE 4: MAIN APPLICATION WITH INTERACTIVE MENU
// ================================================================

std::unique_ptr<HelloServiceProxy> g_proxy;
std::atomic<bool> g_running(true);

void SignalHandler(int signal) {
    LOG_INFO("Signal received, shutting down...");
    g_running = false;
}

void PrintMenu() {
    std::cout << "\n========== HELLO SERVICE CLIENT ==========\n";
    std::cout << "1. Say Hello\n";
    std::cout << "2. Get Server Time\n";
    std::cout << "3. Add Two Numbers\n";
    std::cout << "4. Check Service Status\n";
    std::cout << "0. Exit\n";
    std::cout << "============================================\n";
    std::cout << "Choice: ";
}

void RunInteractiveMode() {
    while (g_running) {
        PrintMenu();
        
        int choice;
        std::cin >> choice;
        
        if (!g_running) break;
        
        switch (choice) {
            case 1: {
                std::cout << "Enter your name: ";
                std::string name;
                std::cin >> name;
                
                auto future = g_proxy->SayHello(name);
                
                // Sử dụng Then() callback - mô phỏng ara::core::Future
                future->Then([](const std::string& response) {
                    std::cout << "\n>>> Response: " << response << "\n";
                });
                
                // Hoặc dùng Get() để blocking wait
                // std::string result = future->Get(config::REQUEST_TIMEOUT_MS);
                // std::cout << "Response: " << result << std::endl;
                
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            
            case 2: {
                auto future = g_proxy->GetTime();
                std::string result = future->Get(config::REQUEST_TIMEOUT_MS);
                std::cout << "\n>>> Server time: " << result << "\n";
                break;
            }
            
            case 3: {
                int a, b;
                std::cout << "Enter first number: ";
                std::cin >> a;
                std::cout << "Enter second number: ";
                std::cin >> b;
                
                auto future = g_proxy->AddNumbers(a, b);
                std::string result = future->Get(config::REQUEST_TIMEOUT_MS);
                std::cout << "\n>>> Result: " << result << "\n";
                break;
            }
            
            case 4: {
                std::cout << "\n>>> Service status: " 
                          << (g_proxy->IsAvailable() ? "AVAILABLE" : "NOT AVAILABLE") 
                          << "\n";
                break;
            }
            
            case 0:
                g_running = false;
                break;
                
            default:
                std::cout << "Invalid choice!\n";
        }
    }
}

int main(int argc, char** argv) {
    LOG_INFO("===========================================");
    LOG_INFO("   AUTOSAR Adaptive - Hello Client         ");
    LOG_INFO("   Example 2: Advanced Implementation      ");
    LOG_INFO("===========================================");
    
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
    // Create and initialize proxy
    g_proxy = std::make_unique<HelloServiceProxy>();
    
    if (!g_proxy->Init()) {
        LOG_ERROR("Failed to initialize proxy");
        return 1;
    }
    
    // Set event callback for heartbeat
    g_proxy->SetEventCallback([](const std::string& event) {
        // Optionally print heartbeat events
        // LOG_DEBUG("Heartbeat: " + event);
    });
    
    // Set availability callback
    g_proxy->SetAvailabilityCallback([](bool available) {
        if (available) {
            LOG_INFO("Service is now available - Ready for commands");
        } else {
            LOG_WARN("Service became unavailable");
        }
    });
    
    // Start proxy
    g_proxy->Start();
    
    LOG_INFO("Waiting for service...");
    if (!g_proxy->WaitForService(10000)) {
        LOG_ERROR("Timeout waiting for service");
        return 1;
    }
    
    // Run interactive mode
    RunInteractiveMode();
    
    // Cleanup
    g_proxy->Stop();
    
    LOG_INFO("Client exited");
    return 0;
}
