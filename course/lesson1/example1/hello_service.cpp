// ============================================
// VÍ DỤ 1: HELLO SERVICE - CODE TỐI GIẢN
// Các yếu tố BẮT BUỘC:
// 1. Include vsomeip
// 2. Tạo application
// 3. Init, Register handlers, Start
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>

// ===== ĐỊNH NGHĨA ID (BẮT BUỘC) =====
#define SERVICE_ID      0x1234
#define INSTANCE_ID     0x5678
#define METHOD_ID       0x0001
#define EVENTGROUP_ID   0x0001

// ===== BIẾN GLOBAL =====
std::shared_ptr<vsomeip::application> app;

// ===== MESSAGE HANDLER (BẮT BUỘC) =====
void on_message(const std::shared_ptr<vsomeip::message>& request) {
    // Tạo response
    auto response = vsomeip::runtime::get()->create_response(request);
    
    // Payload đơn giản
    auto payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data = {'H', 'E', 'L', 'L', 'O'};
    payload->set_data(data);
    response->set_payload(payload);
    
    // Gửi response
    app->send(response);
    std::cout << "Sent response: HELLO" << std::endl;
}

int main() {
    // 1. TẠO APPLICATION (BẮT BUỘC)
    app = vsomeip::runtime::get()->create_application("hello_service");
    
    // 2. KHỞI TẠO (BẮT BUỘC)
    app->init();
    
    // 3. ĐĂNG KÝ MESSAGE HANDLER (BẮT BUỘC)
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_ID, on_message);
    
    // 4. OFFER SERVICE (BẮT BUỘC)
    app->offer_service(SERVICE_ID, INSTANCE_ID);
    
    // 5. START (BẮT BUỘC - blocking call)
    std::cout << "Service started..." << std::endl;
    app->start();
    
    return 0;
}
