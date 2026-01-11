// ============================================
// VÍ DỤ 2: ADVANCED SERVICE - CODE ĐẦY ĐỦ
// 
// Module 1: Includes & Definitions
// Module 2: Application Class
// Module 3: State Management
// Module 4: Event Publishing
// Module 5: Request Handling
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
#include <iomanip>
#include <sstream>
#include <csignal>

// Service identifiers
static constexpr vsomeip::service_t SERVICE_ID      = 0x1234;
static constexpr vsomeip::instance_t INSTANCE_ID    = 0x5678;

// Method identifiers
static constexpr vsomeip::method_t METHOD_GET_TIME  = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_DATA  = 0x0002;

// Event identifiers
static constexpr vsomeip::event_t EVENT_NOTIFY      = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;


// =============================================
// MODULE 2: APPLICATION CLASS
// ---------------------------------------------
// Mục đích: Đóng gói logic service
// Giải thích:
// - Class quản lý lifecycle của service
// - Thread-safe với mutex
// - Hỗ trợ graceful shutdown
// =============================================
class AdvancedService {
public:
    AdvancedService() 
        : running_(false)
        , counter_(0)
    {
        // Tạo application với tên duy nhất
        app_ = vsomeip::runtime::get()->create_application("advanced_service");
    }
    
    ~AdvancedService() {
        stop();
    }
    
    // =============================================
    // MODULE 3: STATE MANAGEMENT
    // ---------------------------------------------
    // Mục đích: Khởi tạo và quản lý trạng thái
    // Giải thích:
    // - init(): Đăng ký tất cả handlers
    // - start(): Bắt đầu service
    // - stop(): Dừng service an toàn
    // =============================================
    
    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return false;
        }
        
        // Đăng ký state handler - được gọi khi app thay đổi trạng thái
        app_->register_state_handler(
            std::bind(&AdvancedService::on_state, this, std::placeholders::_1)
        );
        
        // Đăng ký message handlers cho các methods
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_TIME,
            std::bind(&AdvancedService::on_get_time, this, std::placeholders::_1)
        );
        
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_DATA,
            std::bind(&AdvancedService::on_get_data, this, std::placeholders::_1)
        );
        
        // Cấu hình event
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(EVENTGROUP_ID);
        
        app_->offer_event(
            SERVICE_ID,
            INSTANCE_ID,
            EVENT_NOTIFY,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD,  // Field type (có initial value)
            std::chrono::milliseconds::zero(), // Không có cycle time
            false,  // Không change_resets_cycle
            true,   // update_on_change
            nullptr,
            vsomeip::reliability_type_e::RT_UNRELIABLE
        );
        
        std::cout << "[INIT] Service initialized successfully" << std::endl;
        return true;
    }
    
    void start() {
        running_ = true;
        
        // Khởi động thread publish events
        publish_thread_ = std::thread(&AdvancedService::publish_loop, this);
        
        // Start application (blocking)
        app_->start();
    }
    
    void stop() {
        running_ = false;
        
        // Đợi publish thread kết thúc
        if (publish_thread_.joinable()) {
            publish_thread_.join();
        }
        
        // Stop offering service
        app_->stop_offer_service(SERVICE_ID, INSTANCE_ID);
        
        // Clear handlers
        app_->clear_all_handler();
        
        // Stop application
        app_->stop();
        
        std::cout << "[STOP] Service stopped" << std::endl;
    }

private:
    // State handler - được gọi khi trạng thái app thay đổi
    void on_state(vsomeip::state_type_e state) {
        if (state == vsomeip::state_type_e::ST_REGISTERED) {
            // Application đã đăng ký với routing manager
            // Bây giờ có thể offer service
            app_->offer_service(SERVICE_ID, INSTANCE_ID);
            std::cout << "[STATE] Service offered" << std::endl;
        }
    }
    
    // =============================================
    // MODULE 4: EVENT PUBLISHING
    // ---------------------------------------------
    // Mục đích: Phát events định kỳ
    // Giải thích:
    // - Chạy trong thread riêng
    // - Gửi counter mỗi giây
    // - Sử dụng notify() để publish
    // =============================================
    
    void publish_loop() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            if (!running_) break;
            
            // Tạo payload với counter
            auto payload = vsomeip::runtime::get()->create_payload();
            
            std::stringstream ss;
            ss << "Event #" << ++counter_ << " at " << get_timestamp();
            std::string data = ss.str();
            
            std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
            payload->set_data(payload_data);
            
            // Publish event
            app_->notify(SERVICE_ID, INSTANCE_ID, EVENT_NOTIFY, payload);
            
            std::cout << "[EVENT] Published: " << data << std::endl;
        }
    }
    
    // =============================================
    // MODULE 5: REQUEST HANDLING
    // ---------------------------------------------
    // Mục đích: Xử lý các request từ client
    // Giải thích:
    // - on_get_time: Trả về thời gian hiện tại
    // - on_get_data: Trả về dữ liệu mẫu
    // - Sử dụng create_response() để tạo reply
    // =============================================
    
    void on_get_time(const std::shared_ptr<vsomeip::message>& request) {
        std::cout << "[METHOD] GET_TIME called" << std::endl;
        
        // Tạo response
        auto response = vsomeip::runtime::get()->create_response(request);
        
        // Payload: current time
        auto payload = vsomeip::runtime::get()->create_payload();
        std::string time_str = get_timestamp();
        std::vector<vsomeip::byte_t> data(time_str.begin(), time_str.end());
        payload->set_data(data);
        
        response->set_payload(payload);
        app_->send(response);
    }
    
    void on_get_data(const std::shared_ptr<vsomeip::message>& request) {
        std::cout << "[METHOD] GET_DATA called" << std::endl;
        
        // Lấy request payload (nếu có)
        auto req_payload = request->get_payload();
        if (req_payload && req_payload->get_length() > 0) {
            std::string req_data(
                req_payload->get_data(), 
                req_payload->get_data() + req_payload->get_length()
            );
            std::cout << "[METHOD] Request data: " << req_data << std::endl;
        }
        
        // Tạo response với dữ liệu mẫu
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        // Dữ liệu mẫu: speed, temperature, battery
        std::string data = "{\"speed\":120,\"temp\":25.5,\"battery\":85}";
        std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
        payload->set_data(payload_data);
        
        response->set_payload(payload);
        app_->send(response);
    }
    
    // Helper: Lấy timestamp hiện tại
    std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread publish_thread_;
    std::atomic<bool> running_;
    std::atomic<uint32_t> counter_;
};


// =============================================
// MODULE 6: MAIN ENTRY POINT
// ---------------------------------------------
// Mục đích: Điểm khởi đầu chương trình
// Giải thích:
// - Xử lý signal để shutdown gracefully
// - Chạy service cho đến khi nhận SIGINT
// =============================================

AdvancedService* g_service = nullptr;

void signal_handler(int signal) {
    std::cout << "\n[SIGNAL] Caught signal " << signal << ", shutting down..." << std::endl;
    if (g_service) {
        g_service->stop();
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   ADVANCED SOMEIP SERVICE" << std::endl;
    std::cout << "   Service ID: 0x" << std::hex << SERVICE_ID << std::endl;
    std::cout << "   Instance ID: 0x" << INSTANCE_ID << std::dec << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Đăng ký signal handler
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // Tạo và chạy service
    AdvancedService service;
    g_service = &service;
    
    if (service.init()) {
        service.start();  // Blocking call
    }
    
    return 0;
}
