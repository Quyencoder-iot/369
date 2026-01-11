// =============================================
// MODULE: Data Types
// ---------------------------------------------
// Mục đích: Định nghĩa types cho Qt UI
// =============================================

#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QVector>
#include <QMetaType>
#include <QDateTime>

namespace dashboard {

// Battery info
struct BatteryInfo {
    int level;          // 0-100%
    float voltage;      // Volts
    int temperature;    // Celsius
    bool charging;
    
    BatteryInfo() : level(0), voltage(0), temperature(0), charging(false) {}
    
    BatteryInfo(int l, float v, int t, bool c)
        : level(l), voltage(v), temperature(t), charging(c) {}
};

// Warning
struct Warning {
    QString code;
    QString message;
    int severity;       // 0=low, 1=medium, 2=high
    
    Warning() : severity(0) {}
    Warning(const QString& c, const QString& m, int s)
        : code(c), message(m), severity(s) {}
};

// Drive mode
enum class DriveMode {
    ECO = 0,
    NORMAL = 1,
    SPORT = 2,
    SNOW = 3
};

inline QString driveModeToString(DriveMode mode) {
    switch (mode) {
        case DriveMode::ECO:    return "ECO";
        case DriveMode::NORMAL: return "NORMAL";
        case DriveMode::SPORT:  return "SPORT";
        case DriveMode::SNOW:   return "SNOW";
        default:                return "UNKNOWN";
    }
}

inline DriveMode stringToDriveMode(const QString& str) {
    if (str == "eco" || str == "ECO") return DriveMode::ECO;
    if (str == "sport" || str == "SPORT") return DriveMode::SPORT;
    if (str == "snow" || str == "SNOW") return DriveMode::SNOW;
    return DriveMode::NORMAL;
}

// All dashboard data
struct DashboardData {
    int speed;
    int rpm;
    int gear;
    BatteryInfo battery;
    QVector<Warning> warnings;
    DriveMode driveMode;
    qint64 timestamp;
    
    DashboardData() 
        : speed(0), rpm(0), gear(0)
        , driveMode(DriveMode::NORMAL)
        , timestamp(0) 
    {}
    
    QString timestampString() const {
        return QDateTime::fromMSecsSinceEpoch(timestamp).toString("hh:mm:ss");
    }
};

}  // namespace dashboard

// Register metatypes for Qt signal/slot
Q_DECLARE_METATYPE(dashboard::BatteryInfo)
Q_DECLARE_METATYPE(dashboard::Warning)
Q_DECLARE_METATYPE(dashboard::DriveMode)
Q_DECLARE_METATYPE(dashboard::DashboardData)
Q_DECLARE_METATYPE(QVector<dashboard::Warning>)

#endif // DATATYPES_H
