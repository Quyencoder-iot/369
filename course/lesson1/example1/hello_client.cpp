// ============================================
// VÍ DỤ 1: HELLO CLIENT - CODE TỐI GIẢN
// Các yếu tố BẮT BUỘC:
// 1. Include vsomeip
// 2. Tạo application
// 3. Init, Register handlers, Start
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>

// ===== ĐỊNH NGHĨA ID (PHẢI KHỚP VỚI SERVICE) =====
#define SERVICE_ID      0x1234
#define INSTANCE_ID     0x5678
#define METHOD_ID       0x0001

// ===== BIẾN GLOBAL =====
std::shared_ptr<vsomeip::application> app;

// ===== AVAILABILITY HANDLER (BẮT BUỘC) =====
void on_availability(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
    if (available) {
        std::cout << "Service available!" << std::endl;
        
        // Tạo và gửi request
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_ID);
        
        app->send(request);
        std::cout << "Request sent" << std::endl;
    }
}

// ===== RESPONSE HANDLER (BẮT BUỘC) =====
void on_message(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    std::string data(payload->get_data(), payload->get_data() + payload->get_length());
    std::cout << "Received: " << data << std::endl;
}

int main() {
    // 1. TẠO APPLICATION (BẮT BUỘC)
    app = vsomeip::runtime::get()->create_application("hello_client");
    
    // 2. KHỞI TẠO (BẮT BUỘC)
    app->init();
    
    // 3. ĐĂNG KÝ HANDLERS (BẮT BUỘC)
    app->register_availability_handler(SERVICE_ID, INSTANCE_ID, on_availability);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_ID, on_message);
    
    // 4. REQUEST SERVICE (BẮT BUỘC)
    app->request_service(SERVICE_ID, INSTANCE_ID);
    
    // 5. START (BẮT BUỘC)
    std::cout << "Client started..." << std::endl;
    app->start();
    
    return 0;
}
