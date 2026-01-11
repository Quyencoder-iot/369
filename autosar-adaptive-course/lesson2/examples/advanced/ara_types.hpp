/**
 * @file ara_types.hpp
 * @brief Mô phỏng các types theo chuẩn AUTOSAR Adaptive ara:: namespace
 * 
 * File này cung cấp các types cơ bản:
 * - ara::core::Future, Promise, Result
 * - ara::log::Logger
 * - ara::diag::DiagnosticMonitor
 * - Data types cho vehicle sensors
 */

#ifndef ARA_TYPES_HPP
#define ARA_TYPES_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <queue>

// ================================================================
// NAMESPACE: ara::core
// Core utilities theo chuẩn AUTOSAR Adaptive
// ================================================================
namespace ara {
namespace core {

/**
 * @brief Error codes theo chuẩn AUTOSAR
 */
enum class ErrorCode {
    kSuccess = 0,
    kNotFound = 1,
    kTimeout = 2,
    kInvalidArgument = 3,
    kServiceNotAvailable = 4,
    kCommunicationError = 5,
    kSensorDisconnected = 6,
    kDataInvalid = 7
};

/**
 * @brief Result type - Error handling theo AUTOSAR
 * @tparam T Value type
 * @tparam E Error type (default: ErrorCode)
 */
template<typename T, typename E = ErrorCode>
class Result {
public:
    Result() : has_value_(false), error_(E()) {}
    
    static Result FromValue(const T& value) {
        Result r;
        r.value_ = value;
        r.has_value_ = true;
        return r;
    }
    
    static Result FromError(E error) {
        Result r;
        r.error_ = error;
        r.has_value_ = false;
        return r;
    }
    
    bool HasValue() const { return has_value_; }
    const T& Value() const { return value_; }
    T& Value() { return value_; }
    E Error() const { return error_; }
    
    // Operator overloads
    explicit operator bool() const { return has_value_; }
    const T& operator*() const { return value_; }
    
private:
    T value_;
    E error_;
    bool has_value_;
};

/**
 * @brief Promise - Provides value for Future
 */
template<typename T>
class Promise;

/**
 * @brief Future - Async result container với Then() callback
 * @tparam T Result type
 */
template<typename T>
class Future {
    friend class Promise<T>;
    
public:
    using Callback = std::function<void(const Result<T>&)>;
    
    Future() : ready_(false), state_(std::make_shared<State>()) {}
    
    /**
     * @brief Register callback khi result ready
     */
    template<typename F>
    auto Then(F&& func) -> Future<decltype(func(std::declval<T>()))> {
        using ReturnType = decltype(func(std::declval<T>()));
        auto next_future = std::make_shared<Future<ReturnType>>();
        
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->ready) {
            if (state_->result.HasValue()) {
                next_future->SetValue(func(state_->result.Value()));
            }
        } else {
            state_->callbacks.push_back([next_future, func](const Result<T>& result) {
                if (result.HasValue()) {
                    next_future->SetValue(func(result.Value()));
                }
            });
        }
        
        return *next_future;
    }
    
    /**
     * @brief Blocking wait for result
     */
    T Get() {
        std::unique_lock<std::mutex> lock(state_->mutex);
        state_->cv.wait(lock, [this] { return state_->ready; });
        return state_->result.Value();
    }
    
    /**
     * @brief Wait with timeout
     * @return Result với giá trị hoặc timeout error
     */
    Result<T> GetWithTimeout(int timeout_ms) {
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                                 [this] { return state_->ready; })) {
            return state_->result;
        }
        return Result<T>::FromError(ErrorCode::kTimeout);
    }
    
    bool IsReady() const { 
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->ready; 
    }
    
    void SetValue(const T& value) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->result = Result<T>::FromValue(value);
        state_->ready = true;
        
        for (auto& cb : state_->callbacks) {
            cb(state_->result);
        }
        state_->callbacks.clear();
        state_->cv.notify_all();
    }
    
    void SetError(ErrorCode error) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->result = Result<T>::FromError(error);
        state_->ready = true;
        state_->cv.notify_all();
    }
    
private:
    struct State {
        Result<T> result;
        bool ready = false;
        mutable std::mutex mutex;
        std::condition_variable cv;
        std::vector<Callback> callbacks;
    };
    
    std::atomic<bool> ready_;
    std::shared_ptr<State> state_;
};

/**
 * @brief Promise - Tạo Future và set value
 */
template<typename T>
class Promise {
public:
    Promise() : future_(std::make_shared<Future<T>>()) {}
    
    std::shared_ptr<Future<T>> GetFuture() { return future_; }
    
    void SetValue(const T& value) { future_->SetValue(value); }
    void SetError(ErrorCode error) { future_->SetError(error); }
    
private:
    std::shared_ptr<Future<T>> future_;
};

} // namespace core

// ================================================================
// NAMESPACE: ara::log
// Logging theo chuẩn AUTOSAR
// ================================================================
namespace log {

enum class LogLevel {
    kOff = 0,
    kFatal = 1,
    kError = 2,
    kWarn = 3,
    kInfo = 4,
    kDebug = 5,
    kVerbose = 6
};

/**
 * @brief Logger class với context ID
 */
class Logger {
public:
    Logger(const std::string& ctx_id, const std::string& ctx_desc)
        : context_id_(ctx_id), context_desc_(ctx_desc), 
          level_(LogLevel::kInfo) {}
    
    void SetLogLevel(LogLevel level) { level_ = level; }
    
    void Fatal(const std::string& msg) { Log(LogLevel::kFatal, msg); }
    void Error(const std::string& msg) { Log(LogLevel::kError, msg); }
    void Warn(const std::string& msg) { Log(LogLevel::kWarn, msg); }
    void Info(const std::string& msg) { Log(LogLevel::kInfo, msg); }
    void Debug(const std::string& msg) { Log(LogLevel::kDebug, msg); }
    void Verbose(const std::string& msg) { Log(LogLevel::kVerbose, msg); }
    
private:
    void Log(LogLevel level, const std::string& msg) {
        if (level > level_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::string level_str;
        switch (level) {
            case LogLevel::kFatal:   level_str = "FATAL"; break;
            case LogLevel::kError:   level_str = "ERROR"; break;
            case LogLevel::kWarn:    level_str = "WARN "; break;
            case LogLevel::kInfo:    level_str = "INFO "; break;
            case LogLevel::kDebug:   level_str = "DEBUG"; break;
            case LogLevel::kVerbose: level_str = "VERB "; break;
            default: level_str = "?????";
        }
        
        std::cout << "[" << level_str << "][" << context_id_ << "] " 
                  << msg << std::endl;
    }
    
    std::string context_id_;
    std::string context_desc_;
    LogLevel level_;
    std::mutex mutex_;
};

/**
 * @brief Factory function tạo Logger
 */
inline std::shared_ptr<Logger> CreateLogger(const std::string& ctx_id,
                                            const std::string& ctx_desc) {
    return std::make_shared<Logger>(ctx_id, ctx_desc);
}

} // namespace log

// ================================================================
// NAMESPACE: ara::diag
// Diagnostics theo chuẩn AUTOSAR
// ================================================================
namespace diag {

enum class MonitorAction {
    kPassed,
    kFailed,
    kPrepassed,
    kPrefailed,
    kFreezeDebouncing,
    kResetDebouncing
};

enum class DTCStatus {
    kPending,
    kConfirmed,
    kCleared
};

struct DTCCode {
    uint32_t code;
    std::string description;
    DTCStatus status;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief Diagnostic Monitor để theo dõi sensors
 */
class DiagnosticMonitor {
public:
    DiagnosticMonitor(const std::string& name) 
        : name_(name), status_(MonitorAction::kPassed) {}
    
    void ReportMonitorAction(MonitorAction action) {
        status_ = action;
        if (action == MonitorAction::kFailed) {
            // Tạo DTC code khi fail
            DTCCode dtc;
            dtc.code = GenerateDTCCode();
            dtc.description = name_ + " failure detected";
            dtc.status = DTCStatus::kPending;
            dtc.timestamp = std::chrono::system_clock::now();
            dtc_list_.push_back(dtc);
        }
    }
    
    MonitorAction GetStatus() const { return status_; }
    
    std::vector<DTCCode> GetDTCList() const { return dtc_list_; }
    
    void ClearDTC(uint32_t code) {
        dtc_list_.erase(
            std::remove_if(dtc_list_.begin(), dtc_list_.end(),
                          [code](const DTCCode& dtc) { return dtc.code == code; }),
            dtc_list_.end()
        );
    }
    
    void ClearAllDTC() { dtc_list_.clear(); }
    
private:
    uint32_t GenerateDTCCode() {
        static uint32_t counter = 0;
        return 0xC0000 + (++counter);
    }
    
    std::string name_;
    MonitorAction status_;
    std::vector<DTCCode> dtc_list_;
};

} // namespace diag
} // namespace ara

// ================================================================
// DATA TYPES cho Vehicle Sensors
// ================================================================
namespace vehicle {

struct SpeedData {
    float value;
    std::string unit;
    std::string status;
    uint64_t timestamp;
};

struct BatteryData {
    float level;
    float voltage;
    float current;
    float temperature;
    bool charging;
    float health;
    float range_km;
    std::string status;
};

struct TemperatureData {
    float engine;
    float motor;
    float cabin;
    float outside;
    float hvac_target;
    std::string status;
};

struct TirePressureData {
    float front_left;
    float front_right;
    float rear_left;
    float rear_right;
    std::string unit;
    std::string status;
};

struct Warning {
    std::string code;
    std::string message;
    std::string severity;  // low, medium, high, critical
    uint64_t timestamp;
    bool acknowledged;
};

struct DriveMode {
    std::string current;
    std::vector<std::string> available;
    bool auto_mode;
};

struct VehicleState {
    SpeedData speed;
    BatteryData battery;
    TemperatureData temperature;
    TirePressureData tire_pressure;
    std::vector<Warning> warnings;
    DriveMode drive_mode;
    float odometer_km;
    float trip_km;
};

} // namespace vehicle

#endif // ARA_TYPES_HPP
