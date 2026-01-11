/**
 * @file hello_service_advanced.cpp
 * @brief VÍ DỤ 2 - Service SOME/IP hoàn chỉnh với đầy đủ tính năng
 * 
 * ================ MODULE ARCHITECTURE ================
 * 
 * Module 1: Configuration & Constants
 *   - Service/Instance/Method IDs
 *   - Event và EventGroup definitions
 * 
 * Module 2: ara:: namespace simulation
 *   - ara::core::Future, Result
 *   - ara::log simulation
 * 
 * Module 3: ServiceSkeleton Class
 *   - Offer/Stop service lifecycle
 *   - Method handlers
 *   - Event publishing
 * 
 * Module 4: Main Application
 *   - Initialization
 *   - Signal handling
 *   - Graceful shutdown
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

// ================================================================
// MODULE 1: CONFIGURATION & CONSTANTS
// ================================================================

namespace config {
    // Service identification
    constexpr vsomeip::service_t SERVICE_ID     = 0x1234;
    constexpr vsomeip::instance_t INSTANCE_ID   = 0x5678;
    constexpr vsomeip::major_version_t MAJOR_V  = 1;
    constexpr vsomeip::minor_version_t MINOR_V  = 0;
    
    // Methods
    constexpr vsomeip::method_t METHOD_SAY_HELLO    = 0x0001;
    constexpr vsomeip::method_t METHOD_GET_TIME     = 0x0002;
    constexpr vsomeip::method_t METHOD_ADD_NUMBERS  = 0x0003;
    
    // Events
    constexpr vsomeip::event_t EVENT_HEARTBEAT      = 0x8001;
    constexpr vsomeip::eventgroup_t EVENTGROUP_ALL  = 0x0001;
    
    // Timing
    constexpr int HEARTBEAT_INTERVAL_MS = 1000;
}

// ================================================================
// MODULE 2: ARA:: NAMESPACE SIMULATION
// ================================================================

namespace ara {
namespace core {
    /**
     * @brief Result type để xử lý lỗi theo chuẩn AUTOSAR
     */
    template<typename T, typename E = int>
    class Result {
    public:
        static Result FromValue(T value) {
            Result r;
            r.value_ = value;
            r.has_value_ = true;
            return r;
        }
        
        static Result FromError(E error) {
            Result r;
            r.error_ = error;
            r.has_value_ = false;
            return r;
        }
        
        bool HasValue() const { return has_value_; }
        T Value() const { return value_; }
        E Error() const { return error_; }
        
    private:
        T value_;
        E error_;
        bool has_value_ = false;
    };
    
    /**
     * @brief Future pattern cho async operations
     */
    template<typename T>
    class Future {
    public:
        using Callback = std::function<void(const T&)>;
        
        void Then(Callback cb) {
            callback_ = cb;
            if (ready_) {
                callback_(value_);
            }
        }
        
        void SetValue(const T& value) {
            value_ = value;
            ready_ = true;
            if (callback_) {
                callback_(value_);
            }
        }
        
    private:
        T value_;
        bool ready_ = false;
        Callback callback_;
    };
}

namespace log {
    /**
     * @brief Log levels theo chuẩn AUTOSAR
     */
    enum class LogLevel { 
        kDebug, 
        kInfo, 
        kWarning, 
        kError, 
        kFatal 
    };
    
    /**
     * @brief Logger class mô phỏng ara::log
     */
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
    
    // Global logger instance
    #define LOG_DEBUG(msg) ara::log::Logger::GetInstance().Debug(msg)
    #define LOG_INFO(msg)  ara::log::Logger::GetInstance().Info(msg)
    #define LOG_WARN(msg)  ara::log::Logger::GetInstance().Warn(msg)
    #define LOG_ERROR(msg) ara::log::Logger::GetInstance().Error(msg)
}
}

// ================================================================
// MODULE 3: SERVICE SKELETON CLASS
// ================================================================

/**
 * @brief HelloServiceSkeleton - Server-side implementation
 * 
 * Theo mô hình AUTOSAR Adaptive:
 * - ServiceSkeleton là base class cho service implementation
 * - Cung cấp methods, events, fields
 * - Quản lý lifecycle: Offer/Stop
 */
class HelloServiceSkeleton {
public:
    HelloServiceSkeleton() 
        : running_(false), heartbeat_thread_running_(false) {
        LOG_INFO("HelloServiceSkeleton constructor");
    }
    
    ~HelloServiceSkeleton() {
        Stop();
        LOG_INFO("HelloServiceSkeleton destructor");
    }
    
    /**
     * @brief Initialize service
     * @return true nếu thành công
     */
    bool Init() {
        LOG_INFO("Initializing service...");
        
        // Tạo vsomeip application
        app_ = vsomeip::runtime::get()->create_application("hello_service");
        if (!app_) {
            LOG_ERROR("Failed to create application");
            return false;
        }
        
        // Initialize application
        if (!app_->init()) {
            LOG_ERROR("Failed to initialize application");
            return false;
        }
        
        // Register state handler
        app_->register_state_handler(
            std::bind(&HelloServiceSkeleton::OnStateChange, this, 
                      std::placeholders::_1));
        
        // Register message handlers cho các methods
        RegisterMethodHandlers();
        
        LOG_INFO("Service initialized successfully");
        return true;
    }
    
    /**
     * @brief Offer service để clients có thể tìm thấy
     */
    void OfferService() {
        LOG_INFO("Offering service...");
        
        // Offer event
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(config::EVENTGROUP_ALL);
        
        app_->offer_event(
            config::SERVICE_ID,
            config::INSTANCE_ID,
            config::EVENT_HEARTBEAT,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD
        );
        
        // Offer service
        app_->offer_service(
            config::SERVICE_ID, 
            config::INSTANCE_ID,
            config::MAJOR_V,
            config::MINOR_V
        );
        
        LOG_INFO("Service offered: ID=0x" + 
                 std::to_string(config::SERVICE_ID));
    }
    
    /**
     * @brief Start service và bắt đầu publish events
     */
    void Start() {
        running_ = true;
        
        // Start heartbeat thread
        StartHeartbeatPublisher();
        
        LOG_INFO("Starting application main loop...");
        app_->start();
    }
    
    /**
     * @brief Stop service gracefully
     */
    void Stop() {
        LOG_INFO("Stopping service...");
        running_ = false;
        
        // Stop heartbeat thread
        StopHeartbeatPublisher();
        
        if (app_) {
            // Stop offering service
            app_->stop_offer_service(
                config::SERVICE_ID, 
                config::INSTANCE_ID);
            
            // Stop application
            app_->stop();
        }
        
        LOG_INFO("Service stopped");
    }

private:
    /**
     * @brief Đăng ký handlers cho tất cả methods
     */
    void RegisterMethodHandlers() {
        // Handler cho METHOD_SAY_HELLO
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID,
            config::METHOD_SAY_HELLO,
            std::bind(&HelloServiceSkeleton::OnSayHello, this,
                      std::placeholders::_1));
        
        // Handler cho METHOD_GET_TIME
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID,
            config::METHOD_GET_TIME,
            std::bind(&HelloServiceSkeleton::OnGetTime, this,
                      std::placeholders::_1));
        
        // Handler cho METHOD_ADD_NUMBERS
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID,
            config::METHOD_ADD_NUMBERS,
            std::bind(&HelloServiceSkeleton::OnAddNumbers, this,
                      std::placeholders::_1));
                      
        LOG_INFO("Method handlers registered");
    }
    
    /**
     * @brief Callback khi application state thay đổi
     */
    void OnStateChange(vsomeip::state_type_e state) {
        if (state == vsomeip::state_type_e::ST_REGISTERED) {
            LOG_INFO("Application registered with routing manager");
            OfferService();
        }
    }
    
    /**
     * @brief Handler cho method sayHello
     */
    void OnSayHello(const std::shared_ptr<vsomeip::message>& request) {
        LOG_DEBUG("OnSayHello called");
        
        // Parse request payload
        auto payload = request->get_payload();
        std::string name(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length()
        );
        
        LOG_INFO("Received greeting from: " + name);
        
        // Prepare response
        std::string response_str = "Hello, " + name + "! Welcome to AUTOSAR Adaptive!";
        
        // Send response
        SendResponse(request, response_str);
    }
    
    /**
     * @brief Handler cho method getTime
     */
    void OnGetTime(const std::shared_ptr<vsomeip::message>& request) {
        LOG_DEBUG("OnGetTime called");
        
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::string time_str = std::ctime(&time_t);
        time_str.pop_back(); // Remove newline
        
        SendResponse(request, time_str);
    }
    
    /**
     * @brief Handler cho method addNumbers
     * @details Expect payload format: "num1,num2"
     */
    void OnAddNumbers(const std::shared_ptr<vsomeip::message>& request) {
        LOG_DEBUG("OnAddNumbers called");
        
        auto payload = request->get_payload();
        std::string data(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length()
        );
        
        // Parse "num1,num2"
        size_t comma = data.find(',');
        if (comma != std::string::npos) {
            int a = std::stoi(data.substr(0, comma));
            int b = std::stoi(data.substr(comma + 1));
            int result = a + b;
            
            std::string response = std::to_string(a) + " + " + 
                                   std::to_string(b) + " = " + 
                                   std::to_string(result);
            SendResponse(request, response);
        } else {
            SendResponse(request, "Error: Invalid format");
        }
    }
    
    /**
     * @brief Helper để gửi response
     */
    void SendResponse(const std::shared_ptr<vsomeip::message>& request,
                      const std::string& data) {
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(data.c_str()),
            data.length()
        );
        response->set_payload(payload);
        
        app_->send(response);
        LOG_DEBUG("Response sent: " + data);
    }
    
    /**
     * @brief Start thread để publish heartbeat event định kỳ
     */
    void StartHeartbeatPublisher() {
        heartbeat_thread_running_ = true;
        heartbeat_thread_ = std::thread([this]() {
            int count = 0;
            while (heartbeat_thread_running_) {
                // Publish heartbeat
                PublishHeartbeat(count++);
                
                // Wait interval
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(config::HEARTBEAT_INTERVAL_MS));
            }
        });
        LOG_INFO("Heartbeat publisher started");
    }
    
    /**
     * @brief Stop heartbeat publisher thread
     */
    void StopHeartbeatPublisher() {
        heartbeat_thread_running_ = false;
        if (heartbeat_thread_.joinable()) {
            heartbeat_thread_.join();
        }
        LOG_INFO("Heartbeat publisher stopped");
    }
    
    /**
     * @brief Publish heartbeat event
     */
    void PublishHeartbeat(int count) {
        if (!running_) return;
        
        std::string heartbeat = "Heartbeat #" + std::to_string(count);
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(heartbeat.c_str()),
            heartbeat.length()
        );
        
        app_->notify(
            config::SERVICE_ID,
            config::INSTANCE_ID,
            config::EVENT_HEARTBEAT,
            payload
        );
        
        LOG_DEBUG("Published: " + heartbeat);
    }
    
private:
    std::shared_ptr<vsomeip::application> app_;
    std::atomic<bool> running_;
    std::atomic<bool> heartbeat_thread_running_;
    std::thread heartbeat_thread_;
};

// ================================================================
// MODULE 4: MAIN APPLICATION
// ================================================================

// Global service instance cho signal handler
std::unique_ptr<HelloServiceSkeleton> g_service;

/**
 * @brief Signal handler cho graceful shutdown
 */
void SignalHandler(int signal) {
    LOG_INFO("Signal " + std::to_string(signal) + " received");
    if (g_service) {
        g_service->Stop();
    }
}

/**
 * @brief Main entry point
 */
int main(int argc, char** argv) {
    LOG_INFO("===========================================");
    LOG_INFO("   AUTOSAR Adaptive - Hello Service        ");
    LOG_INFO("   Example 2: Advanced Implementation      ");
    LOG_INFO("===========================================");
    
    // Register signal handlers
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
    // Create and initialize service
    g_service = std::make_unique<HelloServiceSkeleton>();
    
    if (!g_service->Init()) {
        LOG_ERROR("Failed to initialize service");
        return 1;
    }
    
    LOG_INFO("Service starting...");
    LOG_INFO("Press Ctrl+C to stop");
    
    // Start service (blocking)
    g_service->Start();
    
    LOG_INFO("Service exited");
    return 0;
}
