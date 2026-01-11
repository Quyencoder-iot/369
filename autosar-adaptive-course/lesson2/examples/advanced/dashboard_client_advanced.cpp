/**
 * @file dashboard_client_advanced.cpp
 * @brief VÍ DỤ 2 - DashboardClient hoàn chỉnh với ara::com simulation
 * 
 * ================ MODULE ARCHITECTURE ================
 * 
 * Module 1: Configuration (shared với Service)
 * Module 2: DashboardServiceProxy - Client proxy
 * Module 3: Interactive Console Application
 * 
 * Sử dụng các ara::com concepts:
 * - FindService() - Service discovery
 * - ServiceProxy - Client-side proxy
 * - Event subscription với callback
 * - ara::core::Future::Then() cho async operations
 */

#include <vsomeip/vsomeip.hpp>
#include "ara_types.hpp"
#include <atomic>
#include <csignal>
#include <iomanip>

// ================================================================
// MODULE 1: CONFIGURATION
// ================================================================
namespace config {
    constexpr vsomeip::service_t SERVICE_ID = 0x2000;
    constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
    constexpr vsomeip::major_version_t MAJOR_V = 1;
    constexpr vsomeip::minor_version_t MINOR_V = 0;
    
    constexpr vsomeip::method_t METHOD_GET_SPEED     = 0x0001;
    constexpr vsomeip::method_t METHOD_GET_BATTERY   = 0x0002;
    constexpr vsomeip::method_t METHOD_GET_WARNINGS  = 0x0003;
    constexpr vsomeip::method_t METHOD_GET_MODE      = 0x0004;
    constexpr vsomeip::method_t METHOD_SET_MODE      = 0x0005;
    constexpr vsomeip::method_t METHOD_GET_TEMP      = 0x0006;
    
    constexpr vsomeip::event_t EVENT_SPEED_UPDATE    = 0x8001;
    constexpr vsomeip::event_t EVENT_BATTERY_UPDATE  = 0x8002;
    constexpr vsomeip::event_t EVENT_WARNING_UPDATE  = 0x8003;
    constexpr vsomeip::event_t EVENT_ALL_SIGNALS     = 0x8010;
    
    constexpr vsomeip::eventgroup_t EVENTGROUP_SPEED   = 0x0001;
    constexpr vsomeip::eventgroup_t EVENTGROUP_BATTERY = 0x0002;
    constexpr vsomeip::eventgroup_t EVENTGROUP_WARNING = 0x0003;
    constexpr vsomeip::eventgroup_t EVENTGROUP_ALL     = 0x0010;
    
    constexpr int REQUEST_TIMEOUT_MS = 5000;
}

// ================================================================
// MODULE 2: DASHBOARD SERVICE PROXY
// Client-side proxy mô phỏng ara::com::ServiceProxy
// ================================================================
class DashboardServiceProxy {
public:
    using AvailabilityCallback = std::function<void(bool)>;
    using SpeedEventCallback = std::function<void(float)>;
    using BatteryEventCallback = std::function<void(float level, float range)>;
    using AllSignalsCallback = std::function<void(const std::string&)>;
    
    DashboardServiceProxy()
        : logger_(ara::log::CreateLogger("PROXY", "Dashboard Proxy")),
          service_available_(false), running_(false) {}
    
    ~DashboardServiceProxy() { Stop(); }
    
    // ========== INITIALIZATION ==========
    
    bool Init() {
        logger_->Info("Initializing proxy...");
        
        app_ = vsomeip::runtime::get()->create_application("dashboard_client");
        if (!app_ || !app_->init()) {
            logger_->Error("Failed to init application");
            return false;
        }
        
        RegisterHandlers();
        
        logger_->Info("Proxy initialized");
        return true;
    }
    
    void Start() {
        running_ = true;
        
        // Request service (mô phỏng ara::com::FindService)
        app_->request_service(config::SERVICE_ID, config::INSTANCE_ID,
                              config::MAJOR_V, config::MINOR_V);
        
        logger_->Info("Starting proxy...");
        
        app_thread_ = std::thread([this]() {
            app_->start();
        });
    }
    
    void Stop() {
        running_ = false;
        
        if (app_) {
            UnsubscribeEvents();
            app_->release_service(config::SERVICE_ID, config::INSTANCE_ID);
            app_->stop();
        }
        
        if (app_thread_.joinable()) app_thread_.join();
        
        logger_->Info("Proxy stopped");
    }
    
    // ========== SERVICE AVAILABILITY ==========
    
    bool IsAvailable() const { return service_available_; }
    
    bool WaitForService(int timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(availability_mutex_);
        if (timeout_ms > 0) {
            return availability_cv_.wait_for(lock, 
                std::chrono::milliseconds(timeout_ms),
                [this] { return service_available_.load(); });
        } else {
            availability_cv_.wait(lock, [this] { return service_available_.load(); });
            return true;
        }
    }
    
    void SetAvailabilityCallback(AvailabilityCallback cb) {
        availability_callback_ = cb;
    }
    
    // ========== EVENT SUBSCRIPTIONS ==========
    
    void SubscribeSpeedEvent(SpeedEventCallback callback) {
        speed_callback_ = callback;
        SubscribeToEventGroup(config::EVENTGROUP_SPEED, config::EVENT_SPEED_UPDATE);
    }
    
    void SubscribeBatteryEvent(BatteryEventCallback callback) {
        battery_callback_ = callback;
        SubscribeToEventGroup(config::EVENTGROUP_BATTERY, config::EVENT_BATTERY_UPDATE);
    }
    
    void SubscribeAllSignals(AllSignalsCallback callback) {
        all_signals_callback_ = callback;
        SubscribeToEventGroup(config::EVENTGROUP_ALL, config::EVENT_ALL_SIGNALS);
    }
    
    // ========== METHOD CALLS (với ara::core::Future) ==========
    
    /**
     * @brief getVehicleSpeed() - Async với Future
     */
    std::shared_ptr<ara::core::Future<float>> GetVehicleSpeed() {
        auto promise = std::make_shared<ara::core::Promise<float>>();
        
        if (!service_available_) {
            promise->SetError(ara::core::ErrorCode::kServiceNotAvailable);
            return promise->GetFuture();
        }
        
        auto request = CreateRequest(config::METHOD_GET_SPEED);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = 
                [promise](const std::string& data) {
                    try {
                        float value = std::stof(data);
                        promise->SetValue(value);
                    } catch (...) {
                        promise->SetError(ara::core::ErrorCode::kDataInvalid);
                    }
                };
        }
        
        app_->send(request);
        return promise->GetFuture();
    }
    
    /**
     * @brief getBatteryStatus() - Async với Future
     */
    std::shared_ptr<ara::core::Future<vehicle::BatteryData>> GetBatteryStatus() {
        auto promise = std::make_shared<ara::core::Promise<vehicle::BatteryData>>();
        
        if (!service_available_) {
            promise->SetError(ara::core::ErrorCode::kServiceNotAvailable);
            return promise->GetFuture();
        }
        
        auto request = CreateRequest(config::METHOD_GET_BATTERY);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = 
                [promise](const std::string& data) {
                    vehicle::BatteryData battery;
                    // Parse: level,voltage,temperature,charging,health,range
                    std::stringstream ss(data);
                    std::string token;
                    
                    if (std::getline(ss, token, ',')) battery.level = std::stof(token);
                    if (std::getline(ss, token, ',')) battery.voltage = std::stof(token);
                    if (std::getline(ss, token, ',')) battery.temperature = std::stof(token);
                    if (std::getline(ss, token, ',')) battery.charging = (token == "1");
                    if (std::getline(ss, token, ',')) battery.health = std::stof(token);
                    if (std::getline(ss, token, ',')) battery.range_km = std::stof(token);
                    
                    promise->SetValue(battery);
                };
        }
        
        app_->send(request);
        return promise->GetFuture();
    }
    
    /**
     * @brief getWarningList() - Async với Future
     */
    std::shared_ptr<ara::core::Future<std::vector<std::string>>> GetWarningList() {
        auto promise = std::make_shared<ara::core::Promise<std::vector<std::string>>>();
        
        if (!service_available_) {
            promise->SetError(ara::core::ErrorCode::kServiceNotAvailable);
            return promise->GetFuture();
        }
        
        auto request = CreateRequest(config::METHOD_GET_WARNINGS);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = 
                [promise](const std::string& data) {
                    std::vector<std::string> warnings;
                    
                    if (data != "NO_WARNINGS") {
                        std::stringstream ss(data);
                        std::string warning;
                        while (std::getline(ss, warning, ';')) {
                            warnings.push_back(warning);
                        }
                    }
                    
                    promise->SetValue(warnings);
                };
        }
        
        app_->send(request);
        return promise->GetFuture();
    }
    
    /**
     * @brief getDriveMode() - Async với Future
     */
    std::shared_ptr<ara::core::Future<std::string>> GetDriveMode() {
        auto promise = std::make_shared<ara::core::Promise<std::string>>();
        
        if (!service_available_) {
            promise->SetError(ara::core::ErrorCode::kServiceNotAvailable);
            return promise->GetFuture();
        }
        
        auto request = CreateRequest(config::METHOD_GET_MODE);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = 
                [promise](const std::string& data) {
                    promise->SetValue(data);
                };
        }
        
        app_->send(request);
        return promise->GetFuture();
    }
    
    /**
     * @brief setDriveMode(mode) - Async với Future
     */
    std::shared_ptr<ara::core::Future<bool>> SetDriveMode(const std::string& mode) {
        auto promise = std::make_shared<ara::core::Promise<bool>>();
        
        if (!service_available_) {
            promise->SetError(ara::core::ErrorCode::kServiceNotAvailable);
            return promise->GetFuture();
        }
        
        auto request = CreateRequest(config::METHOD_SET_MODE);
        
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(
            reinterpret_cast<const vsomeip::byte_t*>(mode.c_str()),
            mode.length());
        request->set_payload(payload);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = 
                [promise](const std::string& data) {
                    promise->SetValue(data == "OK");
                };
        }
        
        app_->send(request);
        return promise->GetFuture();
    }
    
    /**
     * @brief getTemperature() - Async với Future
     */
    std::shared_ptr<ara::core::Future<vehicle::TemperatureData>> GetTemperature() {
        auto promise = std::make_shared<ara::core::Promise<vehicle::TemperatureData>>();
        
        if (!service_available_) {
            promise->SetError(ara::core::ErrorCode::kServiceNotAvailable);
            return promise->GetFuture();
        }
        
        auto request = CreateRequest(config::METHOD_GET_TEMP);
        
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            pending_requests_[request->get_session()] = 
                [promise](const std::string& data) {
                    vehicle::TemperatureData temp;
                    // Parse: motor,cabin,outside,hvac_target
                    std::stringstream ss(data);
                    std::string token;
                    
                    if (std::getline(ss, token, ',')) temp.motor = std::stof(token);
                    if (std::getline(ss, token, ',')) temp.cabin = std::stof(token);
                    if (std::getline(ss, token, ',')) temp.outside = std::stof(token);
                    if (std::getline(ss, token, ',')) temp.hvac_target = std::stof(token);
                    
                    promise->SetValue(temp);
                };
        }
        
        app_->send(request);
        return promise->GetFuture();
    }

private:
    void RegisterHandlers() {
        // State handler
        app_->register_state_handler([this](vsomeip::state_type_e state) {
            if (state == vsomeip::state_type_e::ST_REGISTERED) {
                logger_->Info("Client registered");
            }
        });
        
        // Availability handler
        app_->register_availability_handler(
            config::SERVICE_ID, config::INSTANCE_ID,
            [this](vsomeip::service_t, vsomeip::instance_t, bool available) {
                OnAvailabilityChange(available);
            });
        
        // Message handler
        app_->register_message_handler(
            config::SERVICE_ID, config::INSTANCE_ID, vsomeip::ANY_METHOD,
            [this](const std::shared_ptr<vsomeip::message>& msg) {
                OnMessage(msg);
            });
    }
    
    void OnAvailabilityChange(bool available) {
        logger_->Info("Service " + std::string(available ? "AVAILABLE" : "NOT AVAILABLE"));
        
        service_available_ = available;
        
        if (available) {
            // Subscribe to events when service becomes available
            SubscribeEvents();
        }
        
        if (availability_callback_) {
            availability_callback_(available);
        }
        
        availability_cv_.notify_all();
    }
    
    void OnMessage(const std::shared_ptr<vsomeip::message>& message) {
        if (message->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            HandleEvent(message);
        } else {
            HandleResponse(message);
        }
    }
    
    void HandleResponse(const std::shared_ptr<vsomeip::message>& response) {
        auto session = response->get_session();
        
        std::function<void(const std::string&)> callback;
        {
            std::lock_guard<std::mutex> lock(pending_mutex_);
            auto it = pending_requests_.find(session);
            if (it != pending_requests_.end()) {
                callback = it->second;
                pending_requests_.erase(it);
            }
        }
        
        if (callback) {
            auto payload = response->get_payload();
            std::string data(
                reinterpret_cast<const char*>(payload->get_data()),
                payload->get_length());
            callback(data);
        }
    }
    
    void HandleEvent(const std::shared_ptr<vsomeip::message>& event) {
        auto payload = event->get_payload();
        if (!payload || payload->get_length() == 0) return;
        
        std::string data(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length());
        
        auto event_id = event->get_method();
        
        if (event_id == config::EVENT_SPEED_UPDATE && speed_callback_) {
            try {
                float speed = std::stof(data);
                speed_callback_(speed);
            } catch (...) {}
        }
        else if (event_id == config::EVENT_BATTERY_UPDATE && battery_callback_) {
            try {
                std::stringstream ss(data);
                std::string level_str, range_str;
                std::getline(ss, level_str, ',');
                std::getline(ss, range_str, ',');
                battery_callback_(std::stof(level_str), std::stof(range_str));
            } catch (...) {}
        }
        else if (event_id == config::EVENT_ALL_SIGNALS && all_signals_callback_) {
            all_signals_callback_(data);
        }
    }
    
    void SubscribeEvents() {
        // Request and subscribe to all events
        std::set<vsomeip::eventgroup_t> groups = {
            config::EVENTGROUP_SPEED,
            config::EVENTGROUP_BATTERY,
            config::EVENTGROUP_ALL
        };
        
        for (auto group : groups) {
            app_->subscribe(config::SERVICE_ID, config::INSTANCE_ID, group);
        }
        
        logger_->Info("Subscribed to events");
    }
    
    void SubscribeToEventGroup(vsomeip::eventgroup_t group, vsomeip::event_t event) {
        std::set<vsomeip::eventgroup_t> groups = {group};
        app_->request_event(config::SERVICE_ID, config::INSTANCE_ID, event, groups,
                           vsomeip::event_type_e::ET_FIELD);
        app_->subscribe(config::SERVICE_ID, config::INSTANCE_ID, group);
    }
    
    void UnsubscribeEvents() {
        app_->unsubscribe(config::SERVICE_ID, config::INSTANCE_ID, config::EVENTGROUP_ALL);
    }
    
    std::shared_ptr<vsomeip::message> CreateRequest(vsomeip::method_t method) {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(config::SERVICE_ID);
        request->set_instance(config::INSTANCE_ID);
        request->set_method(method);
        
        auto payload = vsomeip::runtime::get()->create_payload();
        request->set_payload(payload);
        
        return request;
    }
    
private:
    std::shared_ptr<ara::log::Logger> logger_;
    std::shared_ptr<vsomeip::application> app_;
    std::thread app_thread_;
    
    std::atomic<bool> service_available_;
    std::atomic<bool> running_;
    
    std::mutex availability_mutex_;
    std::condition_variable availability_cv_;
    AvailabilityCallback availability_callback_;
    
    SpeedEventCallback speed_callback_;
    BatteryEventCallback battery_callback_;
    AllSignalsCallback all_signals_callback_;
    
    std::mutex pending_mutex_;
    std::map<vsomeip::session_t, std::function<void(const std::string&)>> pending_requests_;
};

// ================================================================
// MODULE 3: INTERACTIVE CONSOLE APPLICATION
// ================================================================
std::unique_ptr<DashboardServiceProxy> g_proxy;
std::atomic<bool> g_running(true);

void SignalHandler(int signal) {
    std::cout << "\n[SIGNAL] Shutting down...\n";
    g_running = false;
}

void PrintHeader() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════╗\n";
    std::cout << "║         AUTOSAR Adaptive Dashboard Client         ║\n";
    std::cout << "╠═══════════════════════════════════════════════════╣\n";
    std::cout << "║  1. Get Vehicle Speed                             ║\n";
    std::cout << "║  2. Get Battery Status                            ║\n";
    std::cout << "║  3. Get Warning List                              ║\n";
    std::cout << "║  4. Get Drive Mode                                ║\n";
    std::cout << "║  5. Set Drive Mode                                ║\n";
    std::cout << "║  6. Get Temperature                               ║\n";
    std::cout << "║  7. Live Dashboard (events)                       ║\n";
    std::cout << "║  8. Check Service Status                          ║\n";
    std::cout << "║  0. Exit                                          ║\n";
    std::cout << "╚═══════════════════════════════════════════════════╝\n";
    std::cout << "Choice: ";
}

void PrintDashboard(const std::string& data) {
    // Parse: speed:X,battery:X,range:X,mode:X,motor_temp:X
    std::cout << "\r";
    std::cout << "┌─────────────────────────────────────────┐\n";
    std::cout << "│ " << std::left << std::setw(40) << data.substr(0, 40) << "│\n";
    std::cout << "└─────────────────────────────────────────┘";
    std::cout.flush();
}

void RunLiveDashboard() {
    std::cout << "\n=== Live Dashboard (Press Enter to stop) ===\n\n";
    
    // Subscribe to all signals
    g_proxy->SubscribeAllSignals([](const std::string& data) {
        PrintDashboard(data);
    });
    
    // Wait for Enter key
    std::cin.ignore();
    std::cin.get();
    
    std::cout << "\n=== Live Dashboard stopped ===\n";
}

void RunInteractiveMode() {
    while (g_running) {
        PrintHeader();
        
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        
        if (!g_running) break;
        
        switch (choice) {
            case 1: {
                std::cout << "\n>>> Getting vehicle speed...\n";
                auto future = g_proxy->GetVehicleSpeed();
                
                // Sử dụng ara::core::Future::Then() callback
                future->Then([](float speed) {
                    std::cout << ">>> Speed: " << std::fixed << std::setprecision(1) 
                              << speed << " km/h\n";
                    return speed;
                });
                
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            
            case 2: {
                std::cout << "\n>>> Getting battery status...\n";
                auto future = g_proxy->GetBatteryStatus();
                auto result = future->GetWithTimeout(config::REQUEST_TIMEOUT_MS);
                
                if (result.HasValue()) {
                    auto& b = result.Value();
                    std::cout << ">>> Battery Status:\n";
                    std::cout << "    Level:       " << b.level << "%\n";
                    std::cout << "    Voltage:     " << b.voltage << "V\n";
                    std::cout << "    Temperature: " << b.temperature << "°C\n";
                    std::cout << "    Charging:    " << (b.charging ? "Yes" : "No") << "\n";
                    std::cout << "    Health:      " << b.health << "%\n";
                    std::cout << "    Range:       " << b.range_km << " km\n";
                } else {
                    std::cout << ">>> Error getting battery status\n";
                }
                break;
            }
            
            case 3: {
                std::cout << "\n>>> Getting warning list...\n";
                auto future = g_proxy->GetWarningList();
                auto result = future->GetWithTimeout(config::REQUEST_TIMEOUT_MS);
                
                if (result.HasValue()) {
                    auto& warnings = result.Value();
                    if (warnings.empty()) {
                        std::cout << ">>> No active warnings\n";
                    } else {
                        std::cout << ">>> Active Warnings (" << warnings.size() << "):\n";
                        for (const auto& w : warnings) {
                            std::cout << "    - " << w << "\n";
                        }
                    }
                }
                break;
            }
            
            case 4: {
                std::cout << "\n>>> Getting drive mode...\n";
                auto future = g_proxy->GetDriveMode();
                std::string mode = future->Get();
                std::cout << ">>> Current Drive Mode: " << mode << "\n";
                break;
            }
            
            case 5: {
                std::cout << "\n>>> Available modes: eco, normal, sport, snow, offroad\n";
                std::cout << ">>> Enter new mode: ";
                std::string mode;
                std::cin >> mode;
                
                auto future = g_proxy->SetDriveMode(mode);
                bool success = future->Get();
                
                if (success) {
                    std::cout << ">>> Drive mode changed to: " << mode << "\n";
                } else {
                    std::cout << ">>> Failed to change drive mode (invalid mode?)\n";
                }
                break;
            }
            
            case 6: {
                std::cout << "\n>>> Getting temperature...\n";
                auto future = g_proxy->GetTemperature();
                auto result = future->GetWithTimeout(config::REQUEST_TIMEOUT_MS);
                
                if (result.HasValue()) {
                    auto& t = result.Value();
                    std::cout << ">>> Temperature Data:\n";
                    std::cout << "    Motor:       " << t.motor << "°C\n";
                    std::cout << "    Cabin:       " << t.cabin << "°C\n";
                    std::cout << "    Outside:     " << t.outside << "°C\n";
                    std::cout << "    HVAC Target: " << t.hvac_target << "°C\n";
                }
                break;
            }
            
            case 7: {
                RunLiveDashboard();
                break;
            }
            
            case 8: {
                std::cout << "\n>>> Service status: "
                          << (g_proxy->IsAvailable() ? "AVAILABLE ✓" : "NOT AVAILABLE ✗")
                          << "\n";
                break;
            }
            
            case 0:
                g_running = false;
                break;
                
            default:
                std::cout << ">>> Invalid choice!\n";
        }
    }
}

int main(int argc, char** argv) {
    std::cout << "================================================\n";
    std::cout << "   AUTOSAR Adaptive - Dashboard Client\n";
    std::cout << "   Example 2: Advanced with ara::com\n";
    std::cout << "================================================\n";
    
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
    g_proxy = std::make_unique<DashboardServiceProxy>();
    
    if (!g_proxy->Init()) {
        std::cerr << "Failed to initialize proxy\n";
        return 1;
    }
    
    // Set availability callback
    g_proxy->SetAvailabilityCallback([](bool available) {
        if (available) {
            std::cout << "\n>>> Service connected!\n";
        } else {
            std::cout << "\n>>> Service disconnected!\n";
        }
    });
    
    g_proxy->Start();
    
    std::cout << "\nWaiting for DashboardService...\n";
    if (!g_proxy->WaitForService(10000)) {
        std::cerr << "Timeout waiting for service\n";
        return 1;
    }
    
    // Subscribe to speed events
    g_proxy->SubscribeSpeedEvent([](float speed) {
        // Background speed updates (optional logging)
    });
    
    // Run interactive mode
    RunInteractiveMode();
    
    g_proxy->Stop();
    
    std::cout << "Client exited\n";
    return 0;
}
