// ============================================
// VÍ DỤ 1: SIMPLE CLIENT - CODE TỐI GIẢN
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>

#define SERVICE_ID          0x2000
#define INSTANCE_ID         0x0001
#define METHOD_GET_SPEED    0x0001
#define METHOD_GET_BATTERY  0x0002

std::shared_ptr<vsomeip::application> app;

void on_availability(vsomeip::service_t, vsomeip::instance_t, bool available) {
    if (available) {
        std::cout << "Service available - requesting data..." << std::endl;
        
        // Request speed
        auto req1 = vsomeip::runtime::get()->create_request();
        req1->set_service(SERVICE_ID);
        req1->set_instance(INSTANCE_ID);
        req1->set_method(METHOD_GET_SPEED);
        app->send(req1);
        
        // Request battery
        auto req2 = vsomeip::runtime::get()->create_request();
        req2->set_service(SERVICE_ID);
        req2->set_instance(INSTANCE_ID);
        req2->set_method(METHOD_GET_BATTERY);
        app->send(req2);
    }
}

void on_message(const std::shared_ptr<vsomeip::message>& msg) {
    auto payload = msg->get_payload();
    auto method = msg->get_method();
    
    if (method == METHOD_GET_SPEED) {
        uint16_t speed = (payload->get_data()[0] << 8) | payload->get_data()[1];
        std::cout << "Speed: " << speed << " km/h" << std::endl;
    } else if (method == METHOD_GET_BATTERY) {
        uint8_t battery = payload->get_data()[0];
        std::cout << "Battery: " << (int)battery << "%" << std::endl;
    }
}

int main() {
    app = vsomeip::runtime::get()->create_application("simple_client");
    app->init();
    
    app->register_availability_handler(SERVICE_ID, INSTANCE_ID, on_availability);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD, on_message);
    app->request_service(SERVICE_ID, INSTANCE_ID);
    
    std::cout << "Simple Client started" << std::endl;
    app->start();
    
    return 0;
}
