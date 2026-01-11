// =============================================
// MODULE: ara::com types
// ---------------------------------------------
// Mục đích: Data types cho Dashboard Service
// =============================================

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace ara {
namespace com {

// Battery information
struct BatteryInfo {
    uint8_t level;          // 0-100%
    float voltage;          // Volts
    int16_t temperature;    // Celsius
    bool isCharging;
    
    BatteryInfo() : level(0), voltage(0.0f), temperature(0), isCharging(false) {}
    
    BatteryInfo(uint8_t l, float v, int16_t t, bool c)
        : level(l), voltage(v), temperature(t), isCharging(c) {}
    
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
    static BatteryInfo Deserialize(const uint8_t* data, size_t len) {
        BatteryInfo info;
        if (len >= 6) {
            info.level = data[0];
            info.voltage = ((data[1] << 8) | data[2]) / 100.0f;
            info.temperature = static_cast<int16_t>((data[3] << 8) | data[4]);
            info.isCharging = data[5] != 0;
        }
        return info;
    }
    
    // Size in bytes
    static constexpr size_t SerializedSize() { return 6; }
};

// Warning
struct Warning {
    std::string code;
    std::string message;
    uint8_t severity;       // 0=low, 1=medium, 2=high
    
    Warning() : severity(0) {}
    Warning(const std::string& c, const std::string& m, uint8_t s)
        : code(c), message(m), severity(s) {}
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

// Vehicle info
struct VehicleInfo {
    uint16_t speed;         // km/h
    uint16_t rpm;           // RPM
    uint8_t gear;           // Gear (0=P, 1=R, 2=N, 3-10=D1-D8)
    float odometer;         // km
    
    VehicleInfo() : speed(0), rpm(0), gear(0), odometer(0.0f) {}
    
    std::vector<uint8_t> Serialize() const {
        std::vector<uint8_t> data(10);
        data[0] = (speed >> 8) & 0xFF;
        data[1] = speed & 0xFF;
        data[2] = (rpm >> 8) & 0xFF;
        data[3] = rpm & 0xFF;
        data[4] = gear;
        
        // Odometer as uint32 (x10 for one decimal)
        uint32_t odo = static_cast<uint32_t>(odometer * 10);
        data[5] = (odo >> 24) & 0xFF;
        data[6] = (odo >> 16) & 0xFF;
        data[7] = (odo >> 8) & 0xFF;
        data[8] = odo & 0xFF;
        
        return data;
    }
};

// All dashboard signals
struct DashboardSignals {
    uint16_t speed;
    uint16_t rpm;
    uint8_t gear;
    BatteryInfo battery;
    std::vector<Warning> warnings;
    DriveMode driveMode;
    uint64_t timestamp;
    
    DashboardSignals() 
        : speed(0), rpm(0), gear(0), driveMode(DriveMode::NORMAL), timestamp(0) {}
};

// Sensor data
struct SensorData {
    int16_t engineTemp;     // Celsius
    uint8_t oilPressure;    // PSI
    uint8_t fuelLevel;      // %
    int16_t coolantTemp;    // Celsius
    
    SensorData() : engineTemp(0), oilPressure(0), fuelLevel(0), coolantTemp(0) {}
};

}  // namespace com
}  // namespace ara
