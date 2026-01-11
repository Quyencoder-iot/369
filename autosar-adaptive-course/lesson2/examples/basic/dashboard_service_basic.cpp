/**
 * @file dashboard_service_basic.cpp
 * @brief VÍ DỤ 1 - DashboardService cơ bản với JSON database
 * 
 * Các yếu tố BẮT BUỘC phải có:
 * 1. Đọc dữ liệu từ JSON file
 * 2. Cung cấp methods: getSpeed, getBattery, getWarnings, getDriveMode
 * 3. Publish events định kỳ
 * 4. Offer service qua SOME/IP
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>

// ========== SERVICE IDs ==========
#define SERVICE_ID          0x2000
#define INSTANCE_ID         0x0001
#define METHOD_GET_SPEED    0x0001
#define METHOD_GET_BATTERY  0x0002
#define METHOD_GET_WARNINGS 0x0003
#define METHOD_GET_MODE     0x0004
#define EVENT_ALL_SIGNALS   0x8001
#define EVENTGROUP_ID       0x0001

// ========== SIMPLE JSON PARSER (không cần thư viện) ==========
class SimpleJsonParser {
public:
    std::string json_content;
    
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        json_content = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        return true;
    }
    
    // Lấy giá trị số từ JSON (đơn giản)
    int getInt(const std::string& key) {
        size_t pos = json_content.find("\"" + key + "\"");
        if (pos == std::string::npos) return 0;
        pos = json_content.find(":", pos);
        if (pos == std::string::npos) return 0;
        pos++;
        while (json_content[pos] == ' ') pos++;
        return std::stoi(json_content.substr(pos));
    }
    
    // Lấy giá trị string từ JSON (đơn giản)
    std::string getString(const std::string& key) {
        size_t pos = json_content.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        pos = json_content.find(":", pos);
        if (pos == std::string::npos) return "";
        pos = json_content.find("\"", pos);
        if (pos == std::string::npos) return "";
        pos++;
        size_t end = json_content.find("\"", pos);
        return json_content.substr(pos, end - pos);
    }
};

// ========== GLOBAL VARIABLES ==========
std::shared_ptr<vsomeip::application> app;
SimpleJsonParser db;
std::atomic<bool> running(true);

// ========== API: getVehicleSpeed() ==========
void handleGetSpeed(const std::shared_ptr<vsomeip::message>& request) {
    int speed = db.getInt("speed");
    std::string response = std::to_string(speed) + " km/h";
    
    auto resp = vsomeip::runtime::get()->create_response(request);
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(response.c_str()),
        response.length()
    );
    resp->set_payload(payload);
    app->send(resp);
    
    std::cout << "[API] getVehicleSpeed() = " << speed << std::endl;
}

// ========== API: getBatteryStatus() ==========
void handleGetBattery(const std::shared_ptr<vsomeip::message>& request) {
    int battery = db.getInt("battery_level");
    std::string response = std::to_string(battery) + "%";
    
    auto resp = vsomeip::runtime::get()->create_response(request);
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(response.c_str()),
        response.length()
    );
    resp->set_payload(payload);
    app->send(resp);
    
    std::cout << "[API] getBatteryStatus() = " << battery << "%" << std::endl;
}

// ========== API: getWarningList() ==========
void handleGetWarnings(const std::shared_ptr<vsomeip::message>& request) {
    // Đơn giản: trả về message của warning đầu tiên
    std::string warning = db.getString("message");
    if (warning.empty()) warning = "No warnings";
    
    auto resp = vsomeip::runtime::get()->create_response(request);
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(warning.c_str()),
        warning.length()
    );
    resp->set_payload(payload);
    app->send(resp);
    
    std::cout << "[API] getWarningList() = " << warning << std::endl;
}

// ========== API: getDriveMode() ==========
void handleGetDriveMode(const std::shared_ptr<vsomeip::message>& request) {
    std::string mode = db.getString("drive_mode");
    if (mode.empty()) mode = "normal";
    
    auto resp = vsomeip::runtime::get()->create_response(request);
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(
        reinterpret_cast<const vsomeip::byte_t*>(mode.c_str()),
        mode.length()
    );
    resp->set_payload(payload);
    app->send(resp);
    
    std::cout << "[API] getDriveMode() = " << mode << std::endl;
}

// ========== publishAllSignals() - Gửi event định kỳ ==========
void publishAllSignals() {
    while (running) {
        // Reload database để lấy giá trị mới nhất
        db.load("vehicle_database.json");
        
        // Tạo payload chứa tất cả signals
        std::string all_data = 
            "speed:" + std::to_string(db.getInt("speed")) + "," +
            "battery:" + std::to_string(db.getInt("battery_level")) + "," +
            "mode:" + db.getString("drive_mode");
        
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(all_data.c_str()),
            all_data.length()
        );
        
        app->notify(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS, payload);
        
        std::cout << "[EVENT] publishAllSignals: " << all_data << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main() {
    std::cout << "=== DashboardService Basic ===" << std::endl;
    
    // 1. Load JSON database
    if (!db.load("vehicle_database.json")) {
        std::cerr << "Failed to load vehicle_database.json" << std::endl;
        return 1;
    }
    std::cout << "Database loaded successfully" << std::endl;
    
    // 2. Create application
    app = vsomeip::runtime::get()->create_application("dashboard_service");
    if (!app->init()) {
        std::cerr << "Failed to init application" << std::endl;
        return 1;
    }
    
    // 3. Register method handlers
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, 
        METHOD_GET_SPEED, handleGetSpeed);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, 
        METHOD_GET_BATTERY, handleGetBattery);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, 
        METHOD_GET_WARNINGS, handleGetWarnings);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, 
        METHOD_GET_MODE, handleGetDriveMode);
    
    // 4. Offer event
    std::set<vsomeip::eventgroup_t> eventgroups;
    eventgroups.insert(EVENTGROUP_ID);
    app->offer_event(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS, 
                     eventgroups, vsomeip::event_type_e::ET_FIELD);
    
    // 5. Offer service
    app->offer_service(SERVICE_ID, INSTANCE_ID);
    
    // 6. Start event publisher thread
    std::thread publisher(publishAllSignals);
    
    std::cout << "DashboardService is running..." << std::endl;
    
    // 7. Start application
    app->start();
    
    running = false;
    publisher.join();
    
    return 0;
}
