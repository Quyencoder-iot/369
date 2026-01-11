/**
 * @file hello_client_basic.cpp
 * @brief VÍ DỤ 1 - Code cơ bản nhất cho SOME/IP Client
 * 
 * Các yếu tố BẮT BUỘC phải có:
 * 1. Include vsomeip headers
 * 2. Tạo vsomeip application
 * 3. Request service với Service ID và Instance ID
 * 4. Send request và handle response
 * 5. Start application
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <condition_variable>

// ========== ĐỊNH NGHĨA BẮT BUỘC ==========
#define SERVICE_ID      0x1234   // Phải khớp với service
#define INSTANCE_ID     0x5678   // Phải khớp với service  
#define METHOD_ID       0x0001   // ID method muốn gọi

std::shared_ptr<vsomeip::application> app;
std::condition_variable cv;
std::mutex mtx;
bool service_available = false;

// ========== HANDLER KHI NHẬN RESPONSE ==========
void on_message(const std::shared_ptr<vsomeip::message>& response) {
    std::shared_ptr<vsomeip::payload> payload = response->get_payload();
    std::string data(reinterpret_cast<const char*>(payload->get_data()),
                     payload->get_length());
    
    std::cout << "Response: " << data << std::endl;
}

// ========== HANDLER KHI TÌM THẤY SERVICE ==========
void on_availability(vsomeip::service_t service, vsomeip::instance_t instance, 
                     bool available) {
    if (available) {
        std::cout << "Service available!" << std::endl;
        service_available = true;
        cv.notify_one();
    }
}

// ========== GỬI REQUEST ==========
void send_request() {
    // Đợi service available
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return service_available; });
    
    // Tạo request message
    std::shared_ptr<vsomeip::message> request = 
        vsomeip::runtime::get()->create_request();
    
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(METHOD_ID);
    
    // Set payload
    std::string msg = "Hello from Client!";
    std::shared_ptr<vsomeip::payload> payload = 
        vsomeip::runtime::get()->create_payload();
    payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(msg.c_str()), 
        msg.length()
    );
    request->set_payload(payload);
    
    // Gửi request
    app->send(request);
    std::cout << "Request sent!" << std::endl;
}

int main() {
    // 1. TẠO APPLICATION
    app = vsomeip::runtime::get()->create_application("hello_client");
    
    // 2. KHỞI TẠO
    if (!app->init()) {
        std::cerr << "Failed to init!" << std::endl;
        return 1;
    }
    
    // 3. ĐĂNG KÝ HANDLERS
    app->register_message_handler(
        SERVICE_ID, INSTANCE_ID, METHOD_ID, 
        on_message
    );
    
    app->register_availability_handler(
        SERVICE_ID, INSTANCE_ID, 
        on_availability
    );
    
    // 4. REQUEST SERVICE
    app->request_service(SERVICE_ID, INSTANCE_ID);
    
    // 5. GỬI REQUEST TRONG THREAD RIÊNG
    std::thread sender(send_request);
    
    // 6. START APPLICATION
    app->start();
    
    sender.join();
    return 0;
}
