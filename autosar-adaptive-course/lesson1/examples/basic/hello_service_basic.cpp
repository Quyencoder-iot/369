/**
 * @file hello_service_basic.cpp
 * @brief VÍ DỤ 1 - Code cơ bản nhất cho SOME/IP Service
 * 
 * Các yếu tố BẮT BUỘC phải có:
 * 1. Include vsomeip headers
 * 2. Tạo vsomeip application
 * 3. Offer service với Service ID và Instance ID
 * 4. Handle request và send response
 * 5. Start application
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>

// ========== ĐỊNH NGHĨA BẮT BUỘC ==========
#define SERVICE_ID      0x1234   // ID của service
#define INSTANCE_ID     0x5678   // ID của instance
#define METHOD_ID       0x0001   // ID của method "sayHello"

// Biến toàn cục cho application
std::shared_ptr<vsomeip::application> app;

// ========== HANDLER XỬ LÝ REQUEST ==========
void on_message(const std::shared_ptr<vsomeip::message>& request) {
    // Lấy dữ liệu từ request
    std::shared_ptr<vsomeip::payload> payload = request->get_payload();
    std::string data(reinterpret_cast<const char*>(payload->get_data()), 
                     payload->get_length());
    
    std::cout << "Received: " << data << std::endl;
    
    // Tạo response
    std::shared_ptr<vsomeip::message> response = 
        vsomeip::runtime::get()->create_response(request);
    
    // Set payload cho response
    std::string reply = "Hello from Service!";
    std::shared_ptr<vsomeip::payload> resp_payload = 
        vsomeip::runtime::get()->create_payload();
    resp_payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(reply.c_str()), 
        reply.length()
    );
    response->set_payload(resp_payload);
    
    // Gửi response
    app->send(response);
}

int main() {
    // 1. TẠO APPLICATION
    app = vsomeip::runtime::get()->create_application("hello_service");
    
    // 2. KHỞI TẠO APPLICATION
    if (!app->init()) {
        std::cerr << "Failed to init application!" << std::endl;
        return 1;
    }
    
    // 3. ĐĂNG KÝ MESSAGE HANDLER
    app->register_message_handler(
        SERVICE_ID, 
        INSTANCE_ID, 
        METHOD_ID, 
        on_message
    );
    
    // 4. OFFER SERVICE
    app->offer_service(SERVICE_ID, INSTANCE_ID);
    
    std::cout << "Service is running..." << std::endl;
    
    // 5. START APPLICATION (blocking)
    app->start();
    
    return 0;
}
