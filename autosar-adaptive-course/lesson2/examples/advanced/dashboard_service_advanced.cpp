/**
 * @file dashboard_service_advanced.cpp
 * @brief VÍ DỤ 2 - DashboardService hoàn chỉnh với ara:: API simulation
 * 
 * ================ MODULE ARCHITECTURE ================
 * 
 * Module 1: Configuration
 * Module 2: VehicleDatabase - Đọc và mô phỏng dữ liệu từ JSON
 * Module 3: DiagnosticManager - Quản lý diagnostics (ara::diag)
 * Module 4: DashboardServiceSkeleton - Service implementation
 * Module 5: Main Application
 * 
 * APIs theo chuẩn ara::
 * - getVehicleSpeed() → ara::core::Future<float>
 * - getBatteryStatus() → ara::core::Future<BatteryData>
 * - getWarningList() → ara::core::Future<vector<Warning>>
 * - getDriveMode() → ara::core::Future<string>
 * - setDriveMode(mode) → ara::core::Future<bool>
 * - publishAllSignals() → Events to subscribed clients
 */

#include <vsomeip/vsomeip.hpp>
#include "ara_types.hpp"
#include "json_parser.hpp"
#include <atomic>
#include <csignal>

// ================================================================
// MODULE 1: CONFIGURATION
// ================================================================
namespace config {
    // Service identification
    constexpr vsomeip::service_t SERVICE_ID = 0x2000;
    constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
    constexpr vsomeip::major_version_t MAJOR_V = 1;
    constexpr vsomeip::minor_version_t MINOR_V = 0;
    
    // Methods
    constexpr vsomeip::method_t METHOD_GET_SPEED     = 0x0001;
    constexpr vsomeip::method_t METHOD_GET_BATTERY   = 0x0002;
    constexpr vsomeip::method_t METHOD_GET_WARNINGS  = 0x0003;
    constexpr vsomeip::method_t METHOD_GET_MODE      = 0x0004;
    constexpr vsomeip::method_t METHOD_SET_MODE      = 0x0005;
    constexpr vsomeip::method_t METHOD_GET_TEMP      = 0x0006;
    constexpr vsomeip::method_t METHOD_GET_STATE     = 0x0007;
    
    // Events
    constexpr vsomeip::event_t EVENT_SPEED_UPDATE    = 0x8001;
    constexpr vsomeip::event_t EVENT_BATTERY_UPDATE  = 0x8002;
    constexpr vsomeip::event_t EVENT_WARNING_UPDATE  = 0x8003;
    constexpr vsomeip::event_t EVENT_ALL_SIGNALS     = 0x8010;
    
    // EventGroups
    constexpr vsomeip::eventgroup_t EVENTGROUP_SPEED   = 0x0001;
    constexpr vsomeip::eventgroup_t EVENTGROUP_BATTERY = 0x0002;
    constexpr vsomeip::eventgroup_t EVENTGROUP_WARNING = 0x0003;
    constexpr vsomeip::eventgroup_t EVENTGROUP_ALL     = 0x0010;
    
    // Timing
    constexpr int SIMULATION_INTERVAL_MS = 100;
    constexpr int PUBLISH_INTERVAL_MS = 500;
    
    // Database
    const std::string DATABASE_FILE = "vehicle_database_full.json";
}

// ================================================================
// MODULE 2: VEHICLE DATABASE
// Đọc dữ liệu từ JSON và mô phỏng sensor updates
// ================================================================
class VehicleDatabase {
public:
    VehicleDatabase() : logger_(ara::log::CreateLogger("VEHDB", "Vehicle Database")) {}
    
    /**
     * @brief Load database từ JSON file
     */
    bool Load(const std::string& filename) {
        try {
            json::Parser parser;
            root_ = parser.ParseFile(filename);
            logger_->Info("Database loaded: " + filename);
            
            // Parse initial state
            ParseVehicleState();
            return true;
        } catch (const std::exception& e) {
            logger_->Error("Failed to load database: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief API: getVehicleSpeed() - Trả về tốc độ từ database
     */
    std::shared_ptr<ara::core::Future<float>> GetVehicleSpeed() {
        auto promise = std::make_shared<ara::core::Promise<float>>();
        auto future = promise->GetFuture();
        
        // Check sensor status
        if (state_.speed.status != "online") {
            promise->SetError(ara::core::ErrorCode::kSensorDisconnected);
            logger_->Warn("Speed sensor disconnected");
        } else {
            promise->SetValue(state_.speed.value);
            logger_->Debug("getVehicleSpeed() = " + std::to_string(state_.speed.value));
        }
        
        return future;
    }
    
    /**
     * @brief API: getBatteryStatus() - Trả về trạng thái pin
     */
    std::shared_ptr<ara::core::Future<vehicle::BatteryData>> GetBatteryStatus() {
        auto promise = std::make_shared<ara::core::Promise<vehicle::BatteryData>>();
        auto future = promise->GetFuture();
        
        if (state_.battery.status != "online") {
            promise->SetError(ara::core::ErrorCode::kSensorDisconnected);
            logger_->Warn("Battery sensor disconnected");
        } else {
            promise->SetValue(state_.battery);
            logger_->Debug("getBatteryStatus() = " + std::to_string(state_.battery.level) + "%");
        }
        
        return future;
    }
    
    /**
     * @brief API: getWarningList() - Trả về danh sách cảnh báo
     */
    std::shared_ptr<ara::core::Future<std::vector<vehicle::Warning>>> GetWarningList() {
        auto promise = std::make_shared<ara::core::Promise<std::vector<vehicle::Warning>>>();
        promise->SetValue(state_.warnings);
        logger_->Debug("getWarningList() count = " + std::to_string(state_.warnings.size()));
        return promise->GetFuture();
    }
    
    /**
     * @brief API: getDriveMode() - Trả về chế độ lái hiện tại
     */
    std::shared_ptr<ara::core::Future<std::string>> GetDriveMode() {
        auto promise = std::make_shared<ara::core::Promise<std::string>>();
        promise->SetValue(state_.drive_mode.current);
        logger_->Debug("getDriveMode() = " + state_.drive_mode.current);
        return promise->GetFuture();
    }
    
    /**
     * @brief API: setDriveMode(mode) - Đặt chế độ lái
     */
    std::shared_ptr<ara::core::Future<bool>> SetDriveMode(const std::string& mode) {
        auto promise = std::make_shared<ara::core::Promise<bool>>();
        
        // Check if mode is valid
        auto& available = state_.drive_mode.available;
        if (std::find(available.begin(), available.end(), mode) != available.end()) {
            state_.drive_mode.current = mode;
            promise->SetValue(true);
            logger_->Info("Drive mode changed to: " + mode);
        } else {
            promise->SetValue(false);
            logger_->Warn("Invalid drive mode: " + mode);
        }
        
        return promise->GetFuture();
    }
    
    /**
     * @brief API: getTemperature() - Trả về dữ liệu nhiệt độ
     */
    std::shared_ptr<ara::core::Future<vehicle::TemperatureData>> GetTemperature() {
        auto promise = std::make_shared<ara::core::Promise<vehicle::TemperatureData>>();
        promise->SetValue(state_.temperature);
        return promise->GetFuture();
    }
    
    /**
     * @brief Lấy toàn bộ state (cho publishAllSignals)
     */
    const vehicle::VehicleState& GetState() const { return state_; }
    
    /**
     * @brief Mô phỏng sensor updates
     */
    void SimulateUpdate() {
        // Simulate speed changes (city driving pattern)
        static float target_speed = 0;
        static int cycle = 0;
        cycle++;
        
        // Change target every ~5 seconds
        if (cycle % 50 == 0) {
            target_speed = static_cast<float>(rand() % 80);
        }
        
        // Gradually approach target
        if (state_.speed.value < target_speed) {
            state_.speed.value += 0.5f;
        } else if (state_.speed.value > target_speed) {
            state_.speed.value -= 0.3f;
        }
        if (state_.speed.value < 0) state_.speed.value = 0;
        
        // Simulate battery drain
        if (!state_.battery.charging && state_.battery.level > 0) {
            state_.battery.level -= 0.001f;
            state_.battery.range_km = state_.battery.level * 4.0f; // ~400km at 100%
        }
        
        // Simulate motor temperature based on speed
        state_.temperature.motor = 30 + (state_.speed.value * 0.3f);
    }
    
private:
    void ParseVehicleState() {
        // Parse speed
        if (auto sensors = root_->Get("sensors")) {
            if (auto speed = sensors->Get("speed")) {
                state_.speed.value = speed->Get("value")->AsFloat();
                state_.speed.unit = speed->Get("unit")->AsString();
                state_.speed.status = speed->Get("status")->AsString();
            }
            
            if (auto battery = sensors->Get("battery")) {
                state_.battery.level = battery->Get("level")->AsFloat();
                state_.battery.voltage = battery->Get("voltage")->AsFloat();
                state_.battery.current = battery->Get("current")->AsFloat();
                state_.battery.temperature = battery->Get("temperature")->AsFloat();
                state_.battery.charging = battery->Get("charging")->AsBool();
                state_.battery.health = battery->Get("health")->AsFloat();
                state_.battery.range_km = battery->Get("range_km")->AsFloat();
                state_.battery.status = battery->Get("status")->AsString();
            }
            
            if (auto temp = sensors->Get("temperature")) {
                state_.temperature.engine = temp->Get("engine")->AsFloat();
                state_.temperature.motor = temp->Get("motor")->AsFloat();
                state_.temperature.cabin = temp->Get("cabin")->AsFloat();
                state_.temperature.outside = temp->Get("outside")->AsFloat();
                state_.temperature.hvac_target = temp->Get("hvac_target")->AsFloat();
                state_.temperature.status = temp->Get("status")->AsString();
            }
        }
        
        // Parse warnings
        if (auto warnings = root_->Get("warnings")) {
            for (size_t i = 0; i < warnings->Size(); i++) {
                auto w = warnings->Get(i);
                vehicle::Warning warning;
                warning.code = w->Get("code")->AsString();
                warning.message = w->Get("message")->AsString();
                warning.severity = w->Get("severity")->AsString();
                warning.acknowledged = w->Get("acknowledged")->AsBool();
                state_.warnings.push_back(warning);
            }
        }
        
        // Parse drive mode
        if (auto mode = root_->Get("drive_mode")) {
            state_.drive_mode.current = mode->Get("current")->AsString();
            state_.drive_mode.auto_mode = mode->Get("auto_mode")->AsBool();
            
            auto available = mode->Get("available");
            for (size_t i = 0; i < available->Size(); i++) {
                state_.drive_mode.available.push_back(available->Get(i)->AsString());
            }
        }
        
        logger_->Info("Vehicle state parsed successfully");
    }
    
    std::shared_ptr<json::Value> root_;
    vehicle::VehicleState state_;
    std::shared_ptr<ara::log::Logger> logger_;
};

// ================================================================
// MODULE 3: DIAGNOSTIC MANAGER
// Quản lý diagnostics theo ara::diag
// ================================================================
class DiagnosticManager {
public:
    DiagnosticManager() 
        : logger_(ara::log::CreateLogger("DIAG", "Diagnostic Manager")),
          speed_monitor_("SpeedSensor"),
          battery_monitor_("BatterySensor"),
          temp_monitor_("TemperatureSensor") {}
    
    /**
     * @brief Kiểm tra và cập nhật diagnostics
     */
    void CheckSensors(const vehicle::VehicleState& state) {
        // Check speed sensor
        if (state.speed.status == "online") {
            speed_monitor_.ReportMonitorAction(ara::diag::MonitorAction::kPassed);
        } else {
            speed_monitor_.ReportMonitorAction(ara::diag::MonitorAction::kFailed);
            logger_->Error("Speed sensor failure detected");
        }
        
        // Check battery sensor
        if (state.battery.status == "online") {
            battery_monitor_.ReportMonitorAction(ara::diag::MonitorAction::kPassed);
        } else {
            battery_monitor_.ReportMonitorAction(ara::diag::MonitorAction::kFailed);
            logger_->Error("Battery sensor failure detected");
        }
        
        // Check for critical warnings
        for (const auto& warning : state.warnings) {
            if (warning.severity == "critical" || warning.severity == "high") {
                logger_->Warn("Critical warning active: " + warning.message);
            }
        }
    }
    
    /**
     * @brief Lấy danh sách DTC codes
     */
    std::vector<ara::diag::DTCCode> GetAllDTCs() const {
        std::vector<ara::diag::DTCCode> all_dtcs;
        
        auto speed_dtcs = speed_monitor_.GetDTCList();
        all_dtcs.insert(all_dtcs.end(), speed_dtcs.begin(), speed_dtcs.end());
        
        auto battery_dtcs = battery_monitor_.GetDTCList();
        all_dtcs.insert(all_dtcs.end(), battery_dtcs.begin(), battery_dtcs.end());
        
        auto temp_dtcs = temp_monitor_.GetDTCList();
        all_dtcs.insert(all_dtcs.end(), temp_dtcs.begin(), temp_dtcs.end());
        
        return all_dtcs;
    }
    
    void ClearAllDTCs() {
        speed_monitor_.ClearAllDTC();
        battery_monitor_.ClearAllDTC();
        temp_monitor_.ClearAllDTC();
        logger_->Info("All DTCs cleared");
    }
    
private:
    std::shared_ptr<ara::log::Logger> logger_;
    ara::diag::DiagnosticMonitor speed_monitor_;
    ara::diag::DiagnosticMonitor battery_monitor_;
    ara::diag::DiagnosticMonitor temp_monitor_;
};

// ================================================================
// MODULE 4: DASHBOARD SERVICE SKELETON
// Service implementation với SOME/IP
// ================================================================
class DashboardServiceSkeleton {
public:
    DashboardServiceSkeleton() 
        : logger_(ara::log::CreateLogger("DASH", "Dashboard Service")),
          running_(false) {}
    
    ~DashboardServiceSkeleton() { Stop(); }
    
    bool Init() {
        logger_->Info("Initializing DashboardService...");
        
        // Load database
        if (!database_.Load(config::DATABASE_FILE)) {
            logger_->Error("Failed to load database");
            return false;
        }
        
        // Create vsomeip application
        app_ = vsomeip::runtime::get()->create_application("dashboard_service");
        if (!app_ || !app_->init()) {
            logger_->Error("Failed to init vsomeip application");
            return false;
        }
        
        // Register handlers
        RegisterStateHandler();
        RegisterMethodHandlers();
        
        logger_->Info("Service initialized successfully");
        return true;
    }
    
    void OfferService() {
        // Offer events
        OfferEvents();
        
        // Offer service
        app_->offer_service(
            config::SERVICE_ID, config::INSTANCE_ID,
            config::MAJOR_V, config::MINOR_V);
        
        logger_->Info("Service offered: ID=0x" + 
                      std::to_string(config::SERVICE_ID));
    }
    
    void Start() {
        running_ = true;
        
        // Start simulation thread
        simulation_thread_ = std::thread(&DashboardServiceSkeleton::SimulationLoop, this);
        
        // Start publisher thread
        publisher_thread_ = std::thread(&DashboardServiceSkeleton::PublisherLoop, this);
        
        logger_->Info("Starting service main loop...");
        app_->start();
    }
    
    void Stop() {
        logger_->Info("Stopping service...");
        running_ = false;
        
        if (simulation_thread_.joinable()) simulation_thread_.join();
        if (publisher_thread_.joinable()) publisher_thread_.join();
        
        if (app_) {
            app_->stop_offer_service(config::SERVICE_ID, config::INSTANCE_ID);
            app_->stop();
        }
        
        logger_->Info("Service stopped");
    }
    
private:
    void RegisterStateHandler() {
        app_->register_state_handler([this](vsomeip::state_type_e state) {
            if (state == vsomeip::state_type_e::ST_REGISTERED) {
                logger_->Info("Application registered");
                OfferService();
            }
        });
    }
    
    void RegisterMethodHandlers() {
        // getVehicleSpeed
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, config::METHOD_GET_SPEED,
            [this](const std::shared_ptr<vsomeip::message>& req) {
                HandleGetSpeed(req);
            });
        
        // getBatteryStatus
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, config::METHOD_GET_BATTERY,
            [this](const std::shared_ptr<vsomeip::message>& req) {
                HandleGetBattery(req);
            });
        
        // getWarningList
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, config::METHOD_GET_WARNINGS,
            [this](const std::shared_ptr<vsomeip::message>& req) {
                HandleGetWarnings(req);
            });
        
        // getDriveMode
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, config::METHOD_GET_MODE,
            [this](const std::shared_ptr<vsomeip::message>& req) {
                HandleGetDriveMode(req);
            });
        
        // setDriveMode
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, config::METHOD_SET_MODE,
            [this](const std::shared_ptr<vsomeip::message>& req) {
                HandleSetDriveMode(req);
            });
        
        // getTemperature
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, config::METHOD_GET_TEMP,
            [this](const std::shared_ptr<vsomeip::message>& req) {
                HandleGetTemperature(req);
            });
        
        logger_->Info("Method handlers registered");
    }
    
    void OfferEvents() {
        // Speed update event
        std::set<vsomeip::eventgroup_t> speed_groups = {config::EVENTGROUP_SPEED, config::EVENTGROUP_ALL};
        app_->offer_event(config::SERVICE_ID, config::INSTANCE_ID,
                          config::EVENT_SPEED_UPDATE, speed_groups,
                          vsomeip::event_type_e::ET_FIELD);
        
        // Battery update event
        std::set<vsomeip::eventgroup_t> battery_groups = {config::EVENTGROUP_BATTERY, config::EVENTGROUP_ALL};
        app_->offer_event(config::SERVICE_ID, config::INSTANCE_ID,
                          config::EVENT_BATTERY_UPDATE, battery_groups,
                          vsomeip::event_type_e::ET_FIELD);
        
        // Warning update event
        std::set<vsomeip::eventgroup_t> warning_groups = {config::EVENTGROUP_WARNING, config::EVENTGROUP_ALL};
        app_->offer_event(config::SERVICE_ID, config::INSTANCE_ID,
                          config::EVENT_WARNING_UPDATE, warning_groups,
                          vsomeip::event_type_e::ET_EVENT);
        
        // All signals event
        std::set<vsomeip::eventgroup_t> all_groups = {config::EVENTGROUP_ALL};
        app_->offer_event(config::SERVICE_ID, config::INSTANCE_ID,
                          config::EVENT_ALL_SIGNALS, all_groups,
                          vsomeip::event_type_e::ET_FIELD);
        
        logger_->Info("Events offered");
    }
    
    // ========== METHOD HANDLERS ==========
    
    void HandleGetSpeed(const std::shared_ptr<vsomeip::message>& request) {
        auto future = database_.GetVehicleSpeed();
        auto result = future->GetWithTimeout(1000);
        
        std::string response_str;
        if (result.HasValue()) {
            response_str = std::to_string(result.Value());
        } else {
            response_str = "ERROR:SENSOR_DISCONNECTED";
        }
        
        SendResponse(request, response_str);
    }
    
    void HandleGetBattery(const std::shared_ptr<vsomeip::message>& request) {
        auto future = database_.GetBatteryStatus();
        auto result = future->GetWithTimeout(1000);
        
        std::string response_str;
        if (result.HasValue()) {
            auto& b = result.Value();
            // Format: level,voltage,temperature,charging,health,range
            response_str = std::to_string(b.level) + "," +
                          std::to_string(b.voltage) + "," +
                          std::to_string(b.temperature) + "," +
                          (b.charging ? "1" : "0") + "," +
                          std::to_string(b.health) + "," +
                          std::to_string(b.range_km);
        } else {
            response_str = "ERROR:SENSOR_DISCONNECTED";
        }
        
        SendResponse(request, response_str);
    }
    
    void HandleGetWarnings(const std::shared_ptr<vsomeip::message>& request) {
        auto future = database_.GetWarningList();
        auto warnings = future->Get();
        
        std::string response_str;
        for (size_t i = 0; i < warnings.size(); i++) {
            if (i > 0) response_str += ";";
            response_str += warnings[i].code + ":" + 
                           warnings[i].message + ":" + 
                           warnings[i].severity;
        }
        if (response_str.empty()) response_str = "NO_WARNINGS";
        
        SendResponse(request, response_str);
    }
    
    void HandleGetDriveMode(const std::shared_ptr<vsomeip::message>& request) {
        auto future = database_.GetDriveMode();
        SendResponse(request, future->Get());
    }
    
    void HandleSetDriveMode(const std::shared_ptr<vsomeip::message>& request) {
        auto payload = request->get_payload();
        std::string mode(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length());
        
        auto future = database_.SetDriveMode(mode);
        bool success = future->Get();
        
        SendResponse(request, success ? "OK" : "FAILED");
    }
    
    void HandleGetTemperature(const std::shared_ptr<vsomeip::message>& request) {
        auto future = database_.GetTemperature();
        auto temp = future->Get();
        
        // Format: motor,cabin,outside,hvac_target
        std::string response_str = 
            std::to_string(temp.motor) + "," +
            std::to_string(temp.cabin) + "," +
            std::to_string(temp.outside) + "," +
            std::to_string(temp.hvac_target);
        
        SendResponse(request, response_str);
    }
    
    void SendResponse(const std::shared_ptr<vsomeip::message>& request,
                      const std::string& data) {
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(data.c_str()),
            data.length());
        response->set_payload(payload);
        app_->send(response);
    }
    
    // ========== BACKGROUND THREADS ==========
    
    /**
     * @brief Simulation loop - Mô phỏng sensor updates
     */
    void SimulationLoop() {
        while (running_) {
            database_.SimulateUpdate();
            diagnostic_manager_.CheckSensors(database_.GetState());
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(config::SIMULATION_INTERVAL_MS));
        }
    }
    
    /**
     * @brief Publisher loop - publishAllSignals() định kỳ
     */
    void PublisherLoop() {
        while (running_) {
            PublishAllSignals();
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(config::PUBLISH_INTERVAL_MS));
        }
    }
    
    /**
     * @brief publishAllSignals() - Gửi tất cả events tới clients
     */
    void PublishAllSignals() {
        auto& state = database_.GetState();
        
        // Publish speed event
        std::string speed_str = std::to_string(state.speed.value);
        PublishEvent(config::EVENT_SPEED_UPDATE, speed_str);
        
        // Publish battery event
        std::string battery_str = std::to_string(state.battery.level) + "," +
                                  std::to_string(state.battery.range_km);
        PublishEvent(config::EVENT_BATTERY_UPDATE, battery_str);
        
        // Publish all signals
        std::string all_str = 
            "speed:" + std::to_string(state.speed.value) + "," +
            "battery:" + std::to_string(state.battery.level) + "," +
            "range:" + std::to_string(state.battery.range_km) + "," +
            "mode:" + state.drive_mode.current + "," +
            "motor_temp:" + std::to_string(state.temperature.motor);
        PublishEvent(config::EVENT_ALL_SIGNALS, all_str);
    }
    
    void PublishEvent(vsomeip::event_t event_id, const std::string& data) {
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(data.c_str()),
            data.length());
        
        app_->notify(config::SERVICE_ID, config::INSTANCE_ID, event_id, payload);
    }
    
private:
    std::shared_ptr<ara::log::Logger> logger_;
    std::shared_ptr<vsomeip::application> app_;
    VehicleDatabase database_;
    DiagnosticManager diagnostic_manager_;
    
    std::atomic<bool> running_;
    std::thread simulation_thread_;
    std::thread publisher_thread_;
};

// ================================================================
// MODULE 5: MAIN APPLICATION
// ================================================================
std::unique_ptr<DashboardServiceSkeleton> g_service;

void SignalHandler(int signal) {
    std::cout << "\n[SIGNAL] Received signal " << signal << std::endl;
    if (g_service) {
        g_service->Stop();
    }
}

int main(int argc, char** argv) {
    std::cout << "================================================" << std::endl;
    std::cout << "   AUTOSAR Adaptive - Dashboard Service" << std::endl;
    std::cout << "   Example 2: Advanced Implementation with ara::" << std::endl;
    std::cout << "================================================" << std::endl;
    
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
    g_service = std::make_unique<DashboardServiceSkeleton>();
    
    if (!g_service->Init()) {
        std::cerr << "Failed to initialize service" << std::endl;
        return 1;
    }
    
    std::cout << "\nService starting... Press Ctrl+C to stop\n" << std::endl;
    
    g_service->Start();
    
    std::cout << "Service exited" << std::endl;
    return 0;
}
