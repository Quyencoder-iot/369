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
#include <chrono>

namespace ara {
namespace diag {

// Severity levels
enum class DTCSeverity : uint8_t {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    CRITICAL = 3
};

// Diagnostic Trouble Code
struct DTC {
    uint32_t code;
    std::string description;
    DTCSeverity severity;
    bool active;
    uint64_t timestamp;
    uint32_t occurrence_count;
    
    DTC() : code(0), severity(DTCSeverity::INFO), active(false), 
            timestamp(0), occurrence_count(0) {}
    
    DTC(uint32_t c, const std::string& desc, DTCSeverity sev)
        : code(c)
        , description(desc)
        , severity(sev)
        , active(true)
        , occurrence_count(1)
    {
        timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    }
    
    std::string SeverityString() const {
        switch (severity) {
            case DTCSeverity::INFO:     return "INFO";
            case DTCSeverity::WARNING:  return "WARNING";
            case DTCSeverity::ERROR:    return "ERROR";
            case DTCSeverity::CRITICAL: return "CRITICAL";
            default:                    return "UNKNOWN";
        }
    }
};

// Sensor status
struct SensorStatus {
    std::string sensorId;
    bool connected;
    float lastValue;
    uint64_t lastUpdate;
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
        
        auto it = dtcs_.find(dtc.code);
        if (it != dtcs_.end()) {
            // DTC already exists, update it
            it->second.active = true;
            it->second.occurrence_count++;
            it->second.timestamp = std::chrono::system_clock::now()
                .time_since_epoch().count();
        } else {
            // New DTC
            dtcs_[dtc.code] = dtc;
        }
        
        // Call callback if registered
        if (onDtcCallback_) {
            onDtcCallback_(dtcs_[dtc.code]);
        }
    }
    
    // Report DTC with convenience overload
    void ReportDTC(uint32_t code, const std::string& desc, DTCSeverity sev) {
        ReportDTC(DTC(code, desc, sev));
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
    
    // Get all DTCs (including inactive)
    std::vector<DTC> GetAllDTCs() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<DTC> result;
        for (const auto& pair : dtcs_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    // Check if specific DTC is active
    bool IsDTCActive(uint32_t code) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = dtcs_.find(code);
        return (it != dtcs_.end()) && it->second.active;
    }
    
    // Monitor sensor với check function
    void MonitorSensor(const std::string& sensorId,
                       uint32_t dtcCode,
                       std::function<bool()> checkFunc) {
        bool ok = checkFunc();
        
        // Update sensor status
        {
            std::lock_guard<std::mutex> lock(mutex_);
            sensors_[sensorId] = {
                sensorId,
                ok,
                0.0f,
                static_cast<uint64_t>(std::chrono::system_clock::now()
                    .time_since_epoch().count())
            };
        }
        
        if (!ok) {
            ReportDTC(DTC(dtcCode, "Sensor " + sensorId + " disconnected", 
                DTCSeverity::ERROR));
        } else {
            // Clear DTC if sensor is back online
            ClearDTC(dtcCode);
        }
    }
    
    // Get sensor status
    std::vector<SensorStatus> GetSensorStatus() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<SensorStatus> result;
        for (const auto& pair : sensors_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    // Set callback for DTC events
    void SetDTCCallback(std::function<void(const DTC&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        onDtcCallback_ = callback;
    }
    
    // Get DTC count by severity
    size_t GetDTCCount(DTCSeverity minSeverity = DTCSeverity::INFO) {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = 0;
        for (const auto& pair : dtcs_) {
            if (pair.second.active && 
                pair.second.severity >= minSeverity) {
                count++;
            }
        }
        return count;
    }

private:
    DiagnosticManager() = default;
    
    std::map<uint32_t, DTC> dtcs_;
    std::map<std::string, SensorStatus> sensors_;
    std::function<void(const DTC&)> onDtcCallback_;
    std::mutex mutex_;
};

// DTC Code definitions (example)
namespace dtc_codes {
    constexpr uint32_t SPEED_SENSOR_DISCONNECT = 0x0001;
    constexpr uint32_t BATTERY_SENSOR_DISCONNECT = 0x0002;
    constexpr uint32_t TEMP_SENSOR_DISCONNECT = 0x0003;
    constexpr uint32_t CAN_BUS_ERROR = 0x0010;
    constexpr uint32_t LOW_BATTERY = 0x0020;
    constexpr uint32_t OVERTEMPERATURE = 0x0030;
}

}  // namespace diag
}  // namespace ara
