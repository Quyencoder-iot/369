// ============================================
// VÍ DỤ 2: DASHBOARD SERVICE - CODE ĐẦY ĐỦ
//
// Module 1: Includes & Definitions
// Module 2: Database Manager
// Module 3: Dashboard Service Class
// Module 4: API Implementations
// Module 5: Event Publishing
// Module 6: Main Entry
// ============================================

// =============================================
// MODULE 1: INCLUDES & DEFINITIONS
// =============================================
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <csignal>
#include <nlohmann/json.hpp>

// ara:: mô phỏng
#include "ara/core/future.hpp"
#include "ara/core/result.hpp"
#include "ara/log/logger.hpp"
#include "ara/diag/diagnostic.hpp"
#include "ara/com/types.hpp"

using json = nlohmann::json;
using namespace ara::com;
using namespace ara::log;
using namespace ara::diag;

// Service IDs
static constexpr vsomeip::service_t SERVICE_ID = 0x2000;
static constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;

// Method IDs
static constexpr vsomeip::method_t METHOD_GET_SPEED     = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_BATTERY   = 0x0002;
static constexpr vsomeip::method_t METHOD_GET_WARNINGS  = 0x0003;
static constexpr vsomeip::method_t METHOD_GET_DRIVEMODE = 0x0004;

// Event IDs
static constexpr vsomeip::event_t EVENT_ALL_SIGNALS     = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID    = 0x0001;


// =============================================
// MODULE 2: DATABASE MANAGER
// ---------------------------------------------
// Mục đích: Quản lý đọc/ghi database JSON
// Giải thích:
// - Singleton pattern
// - Thread-safe với mutex
// - Auto-reload khi file thay đổi
// =============================================
class DatabaseManager {
public:
    static DatabaseManager& Instance() {
        static DatabaseManager instance;
        return instance;
    }
    
    bool Load(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            std::ifstream file(path);
            if (!file.is_open()) {
                std::cerr << "Cannot open file: " << path << std::endl;
                return false;
            }
            file >> data_;
            filepath_ = path;
            std::cout << "Database loaded: " << path << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load database: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Reload database from file
    bool Reload() {
        return Load(filepath_);
    }
    
    // Getters cho các trường dữ liệu
    uint16_t GetSpeed() {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_["vehicle"]["speed"];
    }
    
    uint16_t GetRPM() {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_["vehicle"]["rpm"];
    }
    
    uint8_t GetGear() {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_["vehicle"]["gear"];
    }
    
    BatteryInfo GetBatteryInfo() {
        std::lock_guard<std::mutex> lock(mutex_);
        BatteryInfo info;
        info.level = data_["battery"]["level"];
        info.voltage = data_["battery"]["voltage"];
        info.temperature = data_["battery"]["temperature"];
        info.isCharging = data_["battery"]["charging"];
        return info;
    }
    
    std::vector<Warning> GetWarnings() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Warning> warnings;
        for (const auto& w : data_["warnings"]) {
            warnings.push_back({
                w["code"].get<std::string>(),
                w["message"].get<std::string>(),
                w["severity"].get<uint8_t>()
            });
        }
        return warnings;
    }
    
    DriveMode GetDriveMode() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string mode = data_["drive_mode"]["current"];
        return StringToDriveMode(mode);
    }
    
    SensorData GetSensorData() {
        std::lock_guard<std::mutex> lock(mutex_);
        SensorData data;
        data.engineTemp = data_["sensors"]["engine_temp"];
        data.oilPressure = data_["sensors"]["oil_pressure"];
        data.fuelLevel = data_["sensors"]["fuel_level"];
        data.coolantTemp = data_["sensors"]["coolant_temp"];
        return data;
    }
    
    DashboardSignals GetAllSignals() {
        DashboardSignals signals;
        signals.speed = GetSpeed();
        signals.rpm = GetRPM();
        signals.gear = GetGear();
        signals.battery = GetBatteryInfo();
        signals.warnings = GetWarnings();
        signals.driveMode = GetDriveMode();
        signals.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return signals;
    }
    
    // Check sensor status (for diagnostics)
    bool IsSensorConnected(const std::string& sensorId) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            return data_["diagnostics"]["sensors_status"][sensorId];
        } catch (...) {
            return false;
        }
    }

private:
    DatabaseManager() = default;
    
    json data_;
    std::string filepath_;
    std::mutex mutex_;
};


// =============================================
// MODULE 3: DASHBOARD SERVICE CLASS
// ---------------------------------------------
// Mục đích: Service cung cấp dữ liệu Dashboard
// Giải thích:
// - Expose methods qua SOME/IP
// - Publish events định kỳ
// - Tích hợp diagnostics
// =============================================
class DashboardService {
public:
    DashboardService()
        : running_(false)
        , logger_(Logger::CreateLogger("DASH", "Dashboard Service"))
    {
        app_ = vsomeip::runtime::get()->create_application("dashboard_service");
        logger_.SetLogLevel(LogLevel::kDebug);
    }
    
    ~DashboardService() {
        Stop();
    }
    
    bool Init() {
        if (!app_->init()) {
            logger_.LogError("Failed to init application");
            return false;
        }
        
        // State handler
        app_->register_state_handler(
            std::bind(&DashboardService::OnState, this, std::placeholders::_1)
        );
        
        // Register method handlers
        RegisterMethodHandlers();
        
        // Configure events
        ConfigureEvents();
        
        // Setup diagnostics
        SetupDiagnostics();
        
        logger_.LogInfo("Dashboard Service initialized");
        return true;
    }
    
    void Start() {
        running_ = true;
        
        // Start publish thread
        publishThread_ = std::thread(&DashboardService::PublishLoop, this);
        
        // Start diagnostic thread
        diagThread_ = std::thread(&DashboardService::DiagnosticLoop, this);
        
        logger_.LogInfo("Dashboard Service starting...");
        app_->start();
    }
    
    void Stop() {
        if (!running_) return;
        
        running_ = false;
        
        if (publishThread_.joinable()) publishThread_.join();
        if (diagThread_.joinable()) diagThread_.join();
        
        app_->stop_offer_service(SERVICE_ID, INSTANCE_ID);
        app_->clear_all_handler();
        app_->stop();
        
        logger_.LogInfo("Dashboard Service stopped");
    }

private:
    void OnState(vsomeip::state_type_e state) {
        if (state == vsomeip::state_type_e::ST_REGISTERED) {
            app_->offer_service(SERVICE_ID, INSTANCE_ID);
            logger_.LogInfo("Service offered - ID: 0x2000");
        }
    }
    
    void RegisterMethodHandlers() {
        // GET_SPEED
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_SPEED,
            std::bind(&DashboardService::OnGetSpeed, this, std::placeholders::_1)
        );
        
        // GET_BATTERY
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_BATTERY,
            std::bind(&DashboardService::OnGetBattery, this, std::placeholders::_1)
        );
        
        // GET_WARNINGS
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_WARNINGS,
            std::bind(&DashboardService::OnGetWarnings, this, std::placeholders::_1)
        );
        
        // GET_DRIVEMODE
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_DRIVEMODE,
            std::bind(&DashboardService::OnGetDriveMode, this, std::placeholders::_1)
        );
        
        logger_.LogDebug("Method handlers registered");
    }
    
    void ConfigureEvents() {
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(EVENTGROUP_ID);
        
        app_->offer_event(
            SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD,
            std::chrono::milliseconds::zero(),
            false, true, nullptr,
            vsomeip::reliability_type_e::RT_UNRELIABLE
        );
        
        logger_.LogDebug("Events configured");
    }
    
    void SetupDiagnostics() {
        DiagnosticManager::Instance().SetDTCCallback([this](const DTC& dtc) {
            std::string msg = "DTC " + std::to_string(dtc.code) + 
                             " [" + dtc.SeverityString() + "]: " + dtc.description;
            if (dtc.severity >= DTCSeverity::ERROR) {
                logger_.LogError(msg);
            } else {
                logger_.LogWarn(msg);
            }
        });
        
        logger_.LogDebug("Diagnostics setup complete");
    }
    
    // =============================================
    // MODULE 4: API IMPLEMENTATIONS
    // ---------------------------------------------
    // Mục đích: Xử lý các method requests
    // =============================================
    
    void OnGetSpeed(const std::shared_ptr<vsomeip::message>& request) {
        logger_.LogDebug("GetSpeed called");
        
        uint16_t speed = DatabaseManager::Instance().GetSpeed();
        
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        std::vector<vsomeip::byte_t> data = {
            static_cast<uint8_t>((speed >> 8) & 0xFF),
            static_cast<uint8_t>(speed & 0xFF)
        };
        payload->set_data(data);
        response->set_payload(payload);
        
        app_->send(response);
        logger_.LogInfo("Sent speed: " + std::to_string(speed) + " km/h");
    }
    
    void OnGetBattery(const std::shared_ptr<vsomeip::message>& request) {
        logger_.LogDebug("GetBattery called");
        
        BatteryInfo battery = DatabaseManager::Instance().GetBatteryInfo();
        
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        auto data = battery.Serialize();
        std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
        payload->set_data(payload_data);
        response->set_payload(payload);
        
        app_->send(response);
        logger_.LogInfo("Sent battery: " + std::to_string(battery.level) + "%");
    }
    
    void OnGetWarnings(const std::shared_ptr<vsomeip::message>& request) {
        logger_.LogDebug("GetWarnings called");
        
        auto warnings = DatabaseManager::Instance().GetWarnings();
        
        // Serialize warnings to JSON string
        json j = json::array();
        for (const auto& w : warnings) {
            j.push_back({
                {"code", w.code},
                {"message", w.message},
                {"severity", w.severity}
            });
        }
        std::string jsonStr = j.dump();
        
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        std::vector<vsomeip::byte_t> data(jsonStr.begin(), jsonStr.end());
        payload->set_data(data);
        response->set_payload(payload);
        
        app_->send(response);
        logger_.LogInfo("Sent " + std::to_string(warnings.size()) + " warnings");
    }
    
    void OnGetDriveMode(const std::shared_ptr<vsomeip::message>& request) {
        logger_.LogDebug("GetDriveMode called");
        
        DriveMode mode = DatabaseManager::Instance().GetDriveMode();
        
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        std::vector<vsomeip::byte_t> data = {static_cast<uint8_t>(mode)};
        payload->set_data(data);
        response->set_payload(payload);
        
        app_->send(response);
        logger_.LogInfo("Sent drive mode: " + DriveModeToString(mode));
    }
    
    // =============================================
    // MODULE 5: EVENT PUBLISHING
    // ---------------------------------------------
    // Mục đích: Publish tất cả signals định kỳ
    // =============================================
    
    void PublishLoop() {
        logger_.LogInfo("Publish loop started (500ms interval)");
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            if (!running_) break;
            
            PublishAllSignals();
        }
        
        logger_.LogInfo("Publish loop stopped");
    }
    
    void PublishAllSignals() {
        auto signals = DatabaseManager::Instance().GetAllSignals();
        
        // Serialize to JSON
        json j = {
            {"speed", signals.speed},
            {"rpm", signals.rpm},
            {"gear", signals.gear},
            {"battery", {
                {"level", signals.battery.level},
                {"voltage", signals.battery.voltage},
                {"temp", signals.battery.temperature},
                {"charging", signals.battery.isCharging}
            }},
            {"driveMode", DriveModeToString(signals.driveMode)},
            {"warningCount", signals.warnings.size()},
            {"timestamp", signals.timestamp}
        };
        
        std::string jsonStr = j.dump();
        
        auto payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> data(jsonStr.begin(), jsonStr.end());
        payload->set_data(data);
        
        app_->notify(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS, payload);
    }
    
    // =============================================
    // MODULE 6: DIAGNOSTIC LOOP
    // ---------------------------------------------
    // Mục đích: Kiểm tra sensor định kỳ
    // =============================================
    
    void DiagnosticLoop() {
        logger_.LogInfo("Diagnostic loop started (5s interval)");
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            if (!running_) break;
            
            CheckSensors();
        }
        
        logger_.LogInfo("Diagnostic loop stopped");
    }
    
    void CheckSensors() {
        auto& db = DatabaseManager::Instance();
        auto& diag = DiagnosticManager::Instance();
        
        // Check speed sensor
        diag.MonitorSensor("speed_sensor", dtc_codes::SPEED_SENSOR_DISCONNECT, 
            [&db]() { return db.IsSensorConnected("speed_sensor"); });
        
        // Check battery sensor
        diag.MonitorSensor("battery_sensor", dtc_codes::BATTERY_SENSOR_DISCONNECT,
            [&db]() { return db.IsSensorConnected("battery_sensor"); });
        
        // Check temperature sensor
        diag.MonitorSensor("temp_sensor", dtc_codes::TEMP_SENSOR_DISCONNECT,
            [&db]() { return db.IsSensorConnected("temp_sensor"); });
        
        // Log diagnostic summary
        auto activeDtcs = diag.GetActiveDTCs();
        if (!activeDtcs.empty()) {
            logger_.LogWarn("Active DTCs: " + std::to_string(activeDtcs.size()));
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread publishThread_;
    std::thread diagThread_;
    std::atomic<bool> running_;
    Logger logger_;
};


// =============================================
// MODULE 7: MAIN ENTRY POINT
// =============================================
DashboardService* g_service = nullptr;

void signal_handler(int sig) {
    std::cout << "\n[SIGNAL] Received signal " << sig << ", shutting down..." << std::endl;
    if (g_service) {
        g_service->Stop();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "   DASHBOARD SERVICE (Advanced)" << std::endl;
    std::cout << "   Service ID: 0x2000" << std::endl;
    std::cout << "   Instance ID: 0x0001" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Load database
    std::string dbPath = (argc > 1) ? argv[1] : "database.json";
    if (!DatabaseManager::Instance().Load(dbPath)) {
        std::cerr << "Failed to load database: " << dbPath << std::endl;
        return 1;
    }
    
    // Setup signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // Create and run service
    DashboardService service;
    g_service = &service;
    
    if (service.Init()) {
        service.Start();  // Blocking
    }
    
    std::cout << "Dashboard Service exited" << std::endl;
    return 0;
}
