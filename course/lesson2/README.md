# 📘 BÀI 2: Mô Phỏng Cơ Sở Dữ Liệu Ảo và Phát Triển Service Layer

## 📑 Mục Lục

1. [Thiết kế Database ảo bằng JSON](#1-thiết-kế-database-ảo-bằng-json)
2. [Mô phỏng ara:: namespace](#2-mô-phỏng-ara-namespace)
3. [Các API mô phỏng](#3-các-api-mô-phỏng)
4. [Ví dụ 1: Simple Service (Đơn giản)](#4-ví-dụ-1-simple-service-đơn-giản)
5. [Ví dụ 2: Dashboard Service (Nâng cao)](#5-ví-dụ-2-dashboard-service-nâng-cao)

---

## 1. Thiết Kế Database Ảo Bằng JSON

### 1.1 Tại sao dùng JSON?

- **Đơn giản**: Dễ đọc, dễ chỉnh sửa
- **Linh hoạt**: Mô phỏng dữ liệu thực tế từ ECU
- **Portable**: Không cần database server
- **Testing**: Dễ dàng thay đổi dữ liệu test

### 1.2 Cấu trúc Database

```json
{
    "vehicle": {
        "speed": 120,
        "rpm": 3500,
        "gear": 4,
        "odometer": 45678.5
    },
    "battery": {
        "level": 85,
        "voltage": 12.6,
        "temperature": 35,
        "charging": false
    },
    "sensors": {
        "engine_temp": 92,
        "oil_pressure": 45,
        "fuel_level": 65,
        "coolant_temp": 88
    },
    "warnings": [
        {"code": "W001", "message": "Low fuel", "severity": "medium"},
        {"code": "W002", "message": "Service due", "severity": "low"}
    ],
    "drive_mode": {
        "current": "sport",
        "available": ["eco", "normal", "sport", "snow"]
    }
}
```

### 1.3 Mapping dữ liệu

| JSON Field | Signal Name | Unit | Range |
|------------|------------|------|-------|
| `vehicle.speed` | VehicleSpeed | km/h | 0-300 |
| `vehicle.rpm` | EngineRPM | rpm | 0-8000 |
| `battery.level` | BatterySOC | % | 0-100 |
| `sensors.engine_temp` | EngineTemp | °C | -40 to 150 |
| `drive_mode.current` | DriveMode | enum | eco/normal/sport |

---

## 2. Mô Phỏng ara:: Namespace

### 2.1 Tổng quan các cluster

```
ara::
├── com/        → Communication (SOME/IP)
├── core/       → Core types (Future, Result)
├── log/        → Logging
├── diag/       → Diagnostics
├── per/        → Persistency
└── exec/       → Execution Management
```

### 2.2 ara::com - Communication

```cpp
namespace ara {
namespace com {

// Mô phỏng ServiceProxy
template<typename ServiceInterface>
class ServiceProxy {
public:
    using HandleType = uint32_t;
    
    // Tìm service
    static ara::core::Future<std::vector<HandleType>> FindService();
    
    // Tạo proxy từ handle
    static std::unique_ptr<ServiceProxy> Create(HandleType handle);
    
    // Events & Methods
    Event<DataType> myEvent;
    Method<ReturnType(Args...)> myMethod;
};

// Mô phỏng ServiceSkeleton
template<typename ServiceInterface>
class ServiceSkeleton {
public:
    void OfferService();
    void StopOfferService();
    
    // Events để publish
    Event<DataType> myEvent;
};

// Event template
template<typename T>
class Event {
public:
    void Subscribe();
    void Unsubscribe();
    ara::core::Future<T> GetNewSamples();
    void Send(const T& data);  // For skeleton
};

}  // namespace com
}  // namespace ara
```

### 2.3 ara::core - Core Types

```cpp
namespace ara {
namespace core {

// Future - Kết quả bất đồng bộ
template<typename T>
class Future {
public:
    T get();                    // Blocking wait
    bool is_ready() const;      // Check if ready
    
    // Callback khi hoàn thành
    template<typename F>
    auto Then(F&& func);
};

// Promise - Nguồn tạo Future
template<typename T>
class Promise {
public:
    Future<T> get_future();
    void set_value(T value);
    void set_exception(std::exception_ptr);
};

// Result - Error handling
template<typename T, typename E = ErrorCode>
class Result {
public:
    bool HasValue() const;
    T Value() const;
    E Error() const;
    
    static Result FromValue(T value);
    static Result FromError(E error);
};

// ErrorCode
class ErrorCode {
public:
    int32_t Value() const;
    std::string Message() const;
};

}  // namespace core
}  // namespace ara
```

### 2.4 ara::log - Logging

```cpp
namespace ara {
namespace log {

enum class LogLevel {
    kOff,
    kFatal,
    kError,
    kWarn,
    kInfo,
    kDebug,
    kVerbose
};

class Logger {
public:
    static Logger CreateLogger(const std::string& ctxId, 
                               const std::string& ctxDesc);
    
    void LogFatal(const std::string& msg);
    void LogError(const std::string& msg);
    void LogWarn(const std::string& msg);
    void LogInfo(const std::string& msg);
    void LogDebug(const std::string& msg);
    void LogVerbose(const std::string& msg);
};

}  // namespace log
}  // namespace ara
```

### 2.5 ara::diag - Diagnostics

```cpp
namespace ara {
namespace diag {

// Diagnostic Trouble Code
struct DTC {
    uint32_t code;
    std::string description;
    uint8_t severity;  // 0=info, 1=warning, 2=error, 3=critical
};

class DiagnosticManager {
public:
    void ReportDTC(const DTC& dtc);
    void ClearDTC(uint32_t code);
    std::vector<DTC> GetActiveDTCs();
    
    // Sensor monitoring
    void MonitorSensor(const std::string& sensorId, 
                       std::function<bool()> checkFunc);
};

}  // namespace diag
}  // namespace ara
```

---

## 3. Các API Mô Phỏng

### 3.1 Danh sách API

| API | Mô tả | Return Type |
|-----|-------|-------------|
| `getVehicleSpeed()` | Tốc độ xe | `Future<uint16_t>` |
| `getBatteryStatus()` | Trạng thái pin | `Future<BatteryInfo>` |
| `getWarningList()` | Danh sách cảnh báo | `Future<vector<Warning>>` |
| `getDriveMode()` | Chế độ lái | `Future<DriveMode>` |
| `publishAllSignals()` | Gửi tất cả events | `void` |

### 3.2 Data Types

```cpp
// Thông tin pin
struct BatteryInfo {
    uint8_t level;          // 0-100%
    float voltage;          // Volts
    int16_t temperature;    // Celsius
    bool isCharging;
};

// Cảnh báo
struct Warning {
    std::string code;
    std::string message;
    uint8_t severity;       // 0=low, 1=medium, 2=high
};

// Chế độ lái
enum class DriveMode : uint8_t {
    ECO = 0,
    NORMAL = 1,
    SPORT = 2,
    SNOW = 3
};

// Tất cả signals
struct DashboardSignals {
    uint16_t speed;
    uint16_t rpm;
    BatteryInfo battery;
    std::vector<Warning> warnings;
    DriveMode driveMode;
    uint32_t timestamp;
};
```

---

## 4. Ví Dụ 1: Simple Service (Đơn giản)

### 📁 Cấu trúc

```
example1/
├── simple_service.cpp      # Service với database JSON
├── simple_client.cpp       # Client lấy dữ liệu
├── vehicle_data.json       # Database ảo
└── CMakeLists.txt
```

### 📝 vehicle_data.json

```json
{
    "speed": 120,
    "battery": 85,
    "temperature": 25,
    "warnings": ["Low fuel", "Service due"],
    "drive_mode": "sport"
}
```

### 📝 simple_service.cpp (Minimal)

```cpp
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
#define SERVICE_ID      0x2000
#define INSTANCE_ID     0x0001
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
```

### 📝 simple_client.cpp (Minimal)

```cpp
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
```

---

## 5. Ví Dụ 2: Dashboard Service (Nâng cao)

### 📁 Cấu trúc

```
example2/
├── ara/                        # Mô phỏng ara:: namespace
│   ├── core/
│   │   ├── future.hpp
│   │   └── result.hpp
│   ├── com/
│   │   └── types.hpp
│   ├── log/
│   │   └── logger.hpp
│   └── diag/
│       └── diagnostic.hpp
├── dashboard_service.cpp       # Service chính
├── dashboard_client.cpp        # Client test
├── database.json               # Database đầy đủ
├── vsomeip.json
└── CMakeLists.txt
```

### 📝 database.json (Đầy đủ)

```json
{
    "vehicle": {
        "speed": 120,
        "rpm": 3500,
        "gear": 4,
        "odometer": 45678.5
    },
    "battery": {
        "level": 85,
        "voltage": 12.6,
        "temperature": 35,
        "charging": false
    },
    "sensors": {
        "engine_temp": 92,
        "oil_pressure": 45,
        "fuel_level": 65,
        "coolant_temp": 88
    },
    "warnings": [
        {"code": "W001", "message": "Low fuel", "severity": 1},
        {"code": "W002", "message": "Service due", "severity": 0}
    ],
    "drive_mode": {
        "current": "sport",
        "available": ["eco", "normal", "sport", "snow"]
    },
    "diagnostics": {
        "sensors_status": {
            "speed_sensor": true,
            "battery_sensor": true,
            "temp_sensor": false
        }
    }
}
```

### 📝 ara/core/future.hpp

```cpp
// =============================================
// MODULE: ara::core::Future
// ---------------------------------------------
// Mục đích: Mô phỏng Future/Promise pattern
// Giải thích:
// - Future đại diện cho kết quả bất đồng bộ
// - Promise được dùng để set kết quả
// - Then() cho phép chain callbacks
// =============================================

#pragma once
#include <future>
#include <functional>
#include <memory>

namespace ara {
namespace core {

template<typename T>
class Future {
public:
    Future() = default;
    
    explicit Future(std::future<T>&& fut) 
        : future_(std::make_shared<std::future<T>>(std::move(fut))) {}
    
    // Blocking wait for result
    T get() {
        if (future_) {
            return future_->get();
        }
        throw std::runtime_error("Invalid future");
    }
    
    // Check if result is ready
    bool is_ready() const {
        if (future_) {
            return future_->wait_for(std::chrono::seconds(0)) == 
                   std::future_status::ready;
        }
        return false;
    }
    
    // Chain callback khi future complete
    template<typename F>
    auto Then(F&& func) -> Future<decltype(func(std::declval<T>()))> {
        using ReturnType = decltype(func(std::declval<T>()));
        
        auto shared_future = future_;
        auto promise = std::make_shared<std::promise<ReturnType>>();
        
        std::thread([shared_future, func, promise]() {
            try {
                if (shared_future) {
                    T value = shared_future->get();
                    if constexpr (std::is_void_v<ReturnType>) {
                        func(value);
                        promise->set_value();
                    } else {
                        promise->set_value(func(value));
                    }
                }
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        }).detach();
        
        return Future<ReturnType>(promise->get_future());
    }

private:
    std::shared_ptr<std::future<T>> future_;
};

template<typename T>
class Promise {
public:
    Promise() : promise_(std::make_shared<std::promise<T>>()) {}
    
    Future<T> get_future() {
        return Future<T>(promise_->get_future());
    }
    
    void set_value(T value) {
        promise_->set_value(std::move(value));
    }
    
    void set_exception(std::exception_ptr eptr) {
        promise_->set_exception(eptr);
    }

private:
    std::shared_ptr<std::promise<T>> promise_;
};

}  // namespace core
}  // namespace ara
```

### 📝 ara/core/result.hpp

```cpp
// =============================================
// MODULE: ara::core::Result
// ---------------------------------------------
// Mục đích: Error handling không dùng exception
// Giải thích:
// - Result chứa hoặc value hoặc error
// - Pattern phổ biến trong Adaptive AUTOSAR
// =============================================

#pragma once
#include <variant>
#include <string>

namespace ara {
namespace core {

class ErrorCode {
public:
    ErrorCode() : value_(0) {}
    ErrorCode(int32_t value, const std::string& msg = "") 
        : value_(value), message_(msg) {}
    
    int32_t Value() const { return value_; }
    const std::string& Message() const { return message_; }
    
    explicit operator bool() const { return value_ != 0; }

private:
    int32_t value_;
    std::string message_;
};

template<typename T, typename E = ErrorCode>
class Result {
public:
    // Constructors
    Result(const T& value) : data_(value) {}
    Result(T&& value) : data_(std::move(value)) {}
    Result(const E& error) : data_(error) {}
    
    // Factory methods
    static Result FromValue(T value) { 
        return Result(std::move(value)); 
    }
    
    static Result FromError(E error) { 
        return Result(error); 
    }
    
    // Accessors
    bool HasValue() const { 
        return std::holds_alternative<T>(data_); 
    }
    
    const T& Value() const { 
        return std::get<T>(data_); 
    }
    
    T& Value() { 
        return std::get<T>(data_); 
    }
    
    const E& Error() const { 
        return std::get<E>(data_); 
    }
    
    // Operators
    explicit operator bool() const { 
        return HasValue(); 
    }
    
    const T& operator*() const { 
        return Value(); 
    }

private:
    std::variant<T, E> data_;
};

}  // namespace core
}  // namespace ara
```

### 📝 ara/log/logger.hpp

```cpp
// =============================================
// MODULE: ara::log::Logger
// ---------------------------------------------
// Mục đích: Logging theo chuẩn AUTOSAR
// Giải thích:
// - Context ID để phân loại log
// - Multiple log levels
// - Có thể mở rộng để ghi file
// =============================================

#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace ara {
namespace log {

enum class LogLevel {
    kOff = 0,
    kFatal,
    kError,
    kWarn,
    kInfo,
    kDebug,
    kVerbose
};

class Logger {
public:
    Logger(const std::string& ctxId, const std::string& ctxDesc)
        : contextId_(ctxId)
        , contextDesc_(ctxDesc)
        , minLevel_(LogLevel::kInfo)
    {}
    
    static Logger CreateLogger(const std::string& ctxId, 
                               const std::string& ctxDesc) {
        return Logger(ctxId, ctxDesc);
    }
    
    void SetLogLevel(LogLevel level) {
        minLevel_ = level;
    }
    
    void LogFatal(const std::string& msg) {
        log(LogLevel::kFatal, msg);
    }
    
    void LogError(const std::string& msg) {
        log(LogLevel::kError, msg);
    }
    
    void LogWarn(const std::string& msg) {
        log(LogLevel::kWarn, msg);
    }
    
    void LogInfo(const std::string& msg) {
        log(LogLevel::kInfo, msg);
    }
    
    void LogDebug(const std::string& msg) {
        log(LogLevel::kDebug, msg);
    }
    
    void LogVerbose(const std::string& msg) {
        log(LogLevel::kVerbose, msg);
    }

private:
    void log(LogLevel level, const std::string& msg) {
        if (level > minLevel_) return;
        
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        
        std::cout << "[" << ss.str() << "]"
                  << "[" << levelToString(level) << "]"
                  << "[" << contextId_ << "] "
                  << msg << std::endl;
    }
    
    static const char* levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::kFatal:   return "FATAL";
            case LogLevel::kError:   return "ERROR";
            case LogLevel::kWarn:    return "WARN ";
            case LogLevel::kInfo:    return "INFO ";
            case LogLevel::kDebug:   return "DEBUG";
            case LogLevel::kVerbose: return "VERB ";
            default:                 return "?????";
        }
    }
    
    std::string contextId_;
    std::string contextDesc_;
    LogLevel minLevel_;
};

}  // namespace log
}  // namespace ara
```

### 📝 ara/diag/diagnostic.hpp

```cpp
// =============================================
// MODULE: ara::diag::DiagnosticManager
// ---------------------------------------------
// Mục đích: Quản lý diagnostic và DTC
// Giải thích:
// - DTC = Diagnostic Trouble Code
// - Theo dõi lỗi sensor, ECU
// - Report lỗi theo severity
// =============================================

#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <mutex>

namespace ara {
namespace diag {

// Diagnostic Trouble Code
struct DTC {
    uint32_t code;
    std::string description;
    uint8_t severity;       // 0=info, 1=warning, 2=error, 3=critical
    bool active;
    uint64_t timestamp;
    
    DTC(uint32_t c, const std::string& desc, uint8_t sev)
        : code(c), description(desc), severity(sev), active(true)
        , timestamp(std::chrono::system_clock::now().time_since_epoch().count())
    {}
};

class DiagnosticManager {
public:
    static DiagnosticManager& Instance() {
        static DiagnosticManager instance;
        return instance;
    }
    
    // Report new DTC
    void ReportDTC(const DTC& dtc) {
        std::lock_guard<std::mutex> lock(mutex_);
        dtcs_[dtc.code] = dtc;
        
        if (onDtcCallback_) {
            onDtcCallback_(dtc);
        }
    }
    
    // Clear specific DTC
    void ClearDTC(uint32_t code) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = dtcs_.find(code);
        if (it != dtcs_.end()) {
            it->second.active = false;
        }
    }
    
    // Clear all DTCs
    void ClearAllDTCs() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : dtcs_) {
            pair.second.active = false;
        }
    }
    
    // Get active DTCs
    std::vector<DTC> GetActiveDTCs() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<DTC> result;
        for (const auto& pair : dtcs_) {
            if (pair.second.active) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
    
    // Monitor sensor với check function
    void MonitorSensor(const std::string& sensorId,
                       uint32_t dtcCode,
                       std::function<bool()> checkFunc) {
        bool ok = checkFunc();
        if (!ok) {
            ReportDTC(DTC(dtcCode, "Sensor " + sensorId + " disconnected", 2));
        }
    }
    
    // Set callback for DTC events
    void SetDTCCallback(std::function<void(const DTC&)> callback) {
        onDtcCallback_ = callback;
    }

private:
    DiagnosticManager() = default;
    
    std::map<uint32_t, DTC> dtcs_;
    std::function<void(const DTC&)> onDtcCallback_;
    std::mutex mutex_;
};

}  // namespace diag
}  // namespace ara
```

### 📝 ara/com/types.hpp

```cpp
// =============================================
// MODULE: ara::com types
// ---------------------------------------------
// Mục đích: Data types cho Dashboard Service
// =============================================

#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace ara {
namespace com {

// Battery information
struct BatteryInfo {
    uint8_t level;          // 0-100%
    float voltage;          // Volts
    int16_t temperature;    // Celsius
    bool isCharging;
    
    // Serialize to bytes
    std::vector<uint8_t> Serialize() const {
        std::vector<uint8_t> data;
        data.push_back(level);
        
        // Voltage as fixed point (x100)
        uint16_t v = static_cast<uint16_t>(voltage * 100);
        data.push_back((v >> 8) & 0xFF);
        data.push_back(v & 0xFF);
        
        // Temperature (signed)
        data.push_back((temperature >> 8) & 0xFF);
        data.push_back(temperature & 0xFF);
        
        data.push_back(isCharging ? 1 : 0);
        return data;
    }
    
    // Deserialize from bytes
    static BatteryInfo Deserialize(const uint8_t* data) {
        BatteryInfo info;
        info.level = data[0];
        info.voltage = ((data[1] << 8) | data[2]) / 100.0f;
        info.temperature = (data[3] << 8) | data[4];
        info.isCharging = data[5] != 0;
        return info;
    }
};

// Warning
struct Warning {
    std::string code;
    std::string message;
    uint8_t severity;       // 0=low, 1=medium, 2=high
};

// Drive mode
enum class DriveMode : uint8_t {
    ECO = 0,
    NORMAL = 1,
    SPORT = 2,
    SNOW = 3
};

inline std::string DriveModeToString(DriveMode mode) {
    switch (mode) {
        case DriveMode::ECO:    return "eco";
        case DriveMode::NORMAL: return "normal";
        case DriveMode::SPORT:  return "sport";
        case DriveMode::SNOW:   return "snow";
        default:                return "unknown";
    }
}

inline DriveMode StringToDriveMode(const std::string& str) {
    if (str == "eco")    return DriveMode::ECO;
    if (str == "sport")  return DriveMode::SPORT;
    if (str == "snow")   return DriveMode::SNOW;
    return DriveMode::NORMAL;
}

// All dashboard signals
struct DashboardSignals {
    uint16_t speed;
    uint16_t rpm;
    uint8_t gear;
    BatteryInfo battery;
    std::vector<Warning> warnings;
    DriveMode driveMode;
    uint64_t timestamp;
};

}  // namespace com
}  // namespace ara
```

### 📝 dashboard_service.cpp (Đầy đủ)

```cpp
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
            if (!file.is_open()) return false;
            file >> data_;
            filepath_ = path;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load database: " << e.what() << std::endl;
            return false;
        }
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
                w["code"],
                w["message"],
                w["severity"]
            });
        }
        return warnings;
    }
    
    DriveMode GetDriveMode() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string mode = data_["drive_mode"]["current"];
        return StringToDriveMode(mode);
    }
    
    DashboardSignals GetAllSignals() {
        DashboardSignals signals;
        signals.speed = GetSpeed();
        signals.rpm = GetRPM();
        signals.battery = GetBatteryInfo();
        signals.warnings = GetWarnings();
        signals.driveMode = GetDriveMode();
        signals.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
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
            logger_.LogInfo("Service offered");
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
    }
    
    void SetupDiagnostics() {
        DiagnosticManager::Instance().SetDTCCallback([this](const DTC& dtc) {
            logger_.LogWarn("DTC reported: " + std::to_string(dtc.code) + 
                           " - " + dtc.description);
        });
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
    }
    
    // =============================================
    // MODULE 5: EVENT PUBLISHING
    // ---------------------------------------------
    // Mục đích: Publish tất cả signals định kỳ
    // =============================================
    
    void PublishLoop() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            if (!running_) break;
            
            PublishAllSignals();
        }
    }
    
    void PublishAllSignals() {
        auto signals = DatabaseManager::Instance().GetAllSignals();
        
        // Serialize to JSON
        json j = {
            {"speed", signals.speed},
            {"rpm", signals.rpm},
            {"gear", signals.battery.level},  // Simplified
            {"battery", {
                {"level", signals.battery.level},
                {"voltage", signals.battery.voltage},
                {"temp", signals.battery.temperature},
                {"charging", signals.battery.isCharging}
            }},
            {"driveMode", DriveModeToString(signals.driveMode)},
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
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            if (!running_) break;
            
            // Check các sensors
            auto& db = DatabaseManager::Instance();
            auto& diag = DiagnosticManager::Instance();
            
            diag.MonitorSensor("speed_sensor", 0x0001, [&db]() {
                return db.IsSensorConnected("speed_sensor");
            });
            
            diag.MonitorSensor("battery_sensor", 0x0002, [&db]() {
                return db.IsSensorConnected("battery_sensor");
            });
            
            diag.MonitorSensor("temp_sensor", 0x0003, [&db]() {
                return db.IsSensorConnected("temp_sensor");
            });
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

void signal_handler(int) {
    std::cout << "\nShutting down..." << std::endl;
    if (g_service) g_service->Stop();
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "   DASHBOARD SERVICE" << std::endl;
    std::cout << "   Service ID: 0x2000" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Load database
    std::string dbPath = (argc > 1) ? argv[1] : "database.json";
    if (!DatabaseManager::Instance().Load(dbPath)) {
        std::cerr << "Failed to load database: " << dbPath << std::endl;
        return 1;
    }
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    DashboardService service;
    g_service = &service;
    
    if (service.Init()) {
        service.Start();
    }
    
    return 0;
}
```

### 📝 dashboard_client.cpp

```cpp
// ============================================
// DASHBOARD CLIENT - Test client
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

static constexpr vsomeip::service_t SERVICE_ID = 0x2000;
static constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_SPEED = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_BATTERY = 0x0002;
static constexpr vsomeip::method_t METHOD_GET_WARNINGS = 0x0003;
static constexpr vsomeip::method_t METHOD_GET_DRIVEMODE = 0x0004;
static constexpr vsomeip::event_t EVENT_ALL_SIGNALS = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;

class DashboardClient {
public:
    DashboardClient() : running_(false), service_available_(false) {
        app_ = vsomeip::runtime::get()->create_application("dashboard_client");
    }
    
    bool Init() {
        if (!app_->init()) return false;
        
        app_->register_state_handler(
            std::bind(&DashboardClient::OnState, this, std::placeholders::_1));
        
        app_->register_availability_handler(
            SERVICE_ID, INSTANCE_ID,
            std::bind(&DashboardClient::OnAvailability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&DashboardClient::OnMessage, this, std::placeholders::_1));
        
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        return true;
    }
    
    void Start() {
        running_ = true;
        requestThread_ = std::thread(&DashboardClient::RequestLoop, this);
        app_->start();
    }
    
    void Stop() {
        running_ = false;
        app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        app_->release_service(SERVICE_ID, INSTANCE_ID);
        if (requestThread_.joinable()) requestThread_.join();
        app_->stop();
    }

private:
    void OnState(vsomeip::state_type_e state) {
        std::cout << "[STATE] " << (state == vsomeip::state_type_e::ST_REGISTERED 
            ? "Registered" : "Deregistered") << std::endl;
    }
    
    void OnAvailability(vsomeip::service_t, vsomeip::instance_t, bool available) {
        std::cout << "[AVAIL] Service " << (available ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
        service_available_ = available;
        
        if (available) {
            // Subscribe to events
            std::set<vsomeip::eventgroup_t> groups;
            groups.insert(EVENTGROUP_ID);
            app_->request_event(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS, 
                groups, vsomeip::event_type_e::ET_FIELD);
            app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        }
    }
    
    void OnMessage(const std::shared_ptr<vsomeip::message>& msg) {
        auto payload = msg->get_payload();
        
        if (msg->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            std::string data(payload->get_data(), 
                payload->get_data() + payload->get_length());
            std::cout << "[EVENT] All Signals: " << data << std::endl;
        } else {
            auto method = msg->get_method();
            
            switch (method) {
                case METHOD_GET_SPEED: {
                    uint16_t speed = (payload->get_data()[0] << 8) | payload->get_data()[1];
                    std::cout << "[RESP] Speed: " << speed << " km/h" << std::endl;
                    break;
                }
                case METHOD_GET_BATTERY: {
                    uint8_t level = payload->get_data()[0];
                    std::cout << "[RESP] Battery: " << (int)level << "%" << std::endl;
                    break;
                }
                case METHOD_GET_WARNINGS: {
                    std::string data(payload->get_data(), 
                        payload->get_data() + payload->get_length());
                    std::cout << "[RESP] Warnings: " << data << std::endl;
                    break;
                }
                case METHOD_GET_DRIVEMODE: {
                    uint8_t mode = payload->get_data()[0];
                    std::cout << "[RESP] DriveMode: " << (int)mode << std::endl;
                    break;
                }
            }
        }
    }
    
    void RequestLoop() {
        int counter = 0;
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (!running_ || !service_available_) continue;
            
            // Rotate through different requests
            vsomeip::method_t method;
            switch (counter++ % 4) {
                case 0: method = METHOD_GET_SPEED; break;
                case 1: method = METHOD_GET_BATTERY; break;
                case 2: method = METHOD_GET_WARNINGS; break;
                case 3: method = METHOD_GET_DRIVEMODE; break;
            }
            
            auto request = vsomeip::runtime::get()->create_request();
            request->set_service(SERVICE_ID);
            request->set_instance(INSTANCE_ID);
            request->set_method(method);
            app_->send(request);
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread requestThread_;
    std::atomic<bool> running_;
    std::atomic<bool> service_available_;
};

DashboardClient* g_client = nullptr;

void signal_handler(int) {
    if (g_client) g_client->Stop();
}

int main() {
    std::cout << "Dashboard Client starting..." << std::endl;
    
    std::signal(SIGINT, signal_handler);
    
    DashboardClient client;
    g_client = &client;
    
    if (client.Init()) {
        client.Start();
    }
    
    return 0;
}
```

---

## 📊 Tổng Kết Bài 2

### Các điểm chính:

| Thành phần | Mô tả |
|------------|-------|
| **JSON Database** | Mô phỏng dữ liệu ECU thực |
| **ara::core::Future** | Xử lý bất đồng bộ |
| **ara::log::Logger** | Logging chuẩn AUTOSAR |
| **ara::diag** | Quản lý diagnostic/DTC |
| **DatabaseManager** | Singleton quản lý data |
| **DashboardService** | Service SOME/IP hoàn chỉnh |

### API Flow:

```
Client                          Service
  │                                │
  │──── request(GET_SPEED) ───────►│
  │                                │── DatabaseManager.GetSpeed()
  │◄──── response(120) ───────────│
  │                                │
  │◄──── event(ALL_SIGNALS) ──────│ (mỗi 500ms)
  │                                │
```

---

**[← Bài 1](../lesson1/README.md) | [Bài 3: Qt UI Integration →](../lesson3/README.md)**
