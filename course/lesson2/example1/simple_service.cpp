// ============================================
// VÍ DỤ 1: SIMPLE SERVICE - CODE TỐI GIẢN
// Yếu tố BẮT BUỘC:
// 1. Đọc JSON database
// 2. Cung cấp methods lấy dữ liệu
// 3. Publish events
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ===== SERVICE IDs =====
#define SERVICE_ID          0x2000
#define INSTANCE_ID         0x0001
#define METHOD_GET_SPEED    0x0001
#define METHOD_GET_BATTERY  0x0002
#define EVENT_SIGNALS       0x8001
#define EVENTGROUP_ID       0x0001

// ===== GLOBAL =====
std::shared_ptr<vsomeip::application> app;
json database;

// ===== ĐỌC DATABASE (BẮT BUỘC) =====
bool load_database(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    file >> database;
    return true;
}

// ===== HANDLERS =====
void on_get_speed(const std::shared_ptr<vsomeip::message>& request) {
    uint16_t speed = database["speed"];
    
    auto response = vsomeip::runtime::get()->create_response(request);
    auto payload = vsomeip::runtime::get()->create_payload();
    
    std::vector<vsomeip::byte_t> data(2);
    data[0] = (speed >> 8) & 0xFF;
    data[1] = speed & 0xFF;
    payload->set_data(data);
    
    response->set_payload(payload);
    app->send(response);
    
    std::cout << "Sent speed: " << speed << " km/h" << std::endl;
}

void on_get_battery(const std::shared_ptr<vsomeip::message>& request) {
    uint8_t battery = database["battery"];
    
    auto response = vsomeip::runtime::get()->create_response(request);
    auto payload = vsomeip::runtime::get()->create_payload();
    
    std::vector<vsomeip::byte_t> data = {battery};
    payload->set_data(data);
    
    response->set_payload(payload);
    app->send(response);
    
    std::cout << "Sent battery: " << (int)battery << "%" << std::endl;
}

int main() {
    // 1. Load database
    if (!load_database("vehicle_data.json")) {
        std::cerr << "Failed to load database" << std::endl;
        return 1;
    }
    std::cout << "Database loaded successfully" << std::endl;
    
    // 2. Create & init app
    app = vsomeip::runtime::get()->create_application("simple_service");
    app->init();
    
    // 3. Register handlers
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_GET_SPEED, on_get_speed);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_GET_BATTERY, on_get_battery);
    
    // 4. Offer service
    app->offer_service(SERVICE_ID, INSTANCE_ID);
    
    std::cout << "Simple Service started" << std::endl;
    app->start();
    
    return 0;
}
