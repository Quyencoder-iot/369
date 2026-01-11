# Bài 2: Mô phỏng Cơ sở Dữ liệu Ảo và Phát triển Service Layer

## 📚 Mục tiêu bài học
- Thiết kế database ảo bằng JSON cho hệ thống xe
- Tạo các API mô phỏng theo chuẩn `ara::`
- Xây dựng DashboardService hoàn chỉnh với SOME/IP
- Hiểu cách sử dụng `ara::com`, `ara::core::Future`, `ara::log`, `ara::diag`

---

## 1. Thiết kế Database Ảo bằng JSON

### 1.1 Cấu trúc dữ liệu xe

```
vehicle_database.json
├── vehicle_info          # Thông tin xe
├── sensors               # Dữ liệu cảm biến
│   ├── speed            # Tốc độ
│   ├── battery          # Pin
│   ├── temperature      # Nhiệt độ
│   └── fuel             # Nhiên liệu
├── warnings              # Cảnh báo
├── drive_mode            # Chế độ lái
└── diagnostics           # Chẩn đoán
```

### 1.2 Mẫu file JSON

```json
{
  "vehicle_info": {
    "model": "EV-2025",
    "vin": "VN1234567890",
    "year": 2025
  },
  "sensors": {
    "speed": {
      "value": 60,
      "unit": "km/h",
      "min": 0,
      "max": 200,
      "timestamp": "2025-01-11T10:00:00Z"
    },
    "battery": {
      "level": 75,
      "voltage": 400,
      "temperature": 35,
      "charging": false,
      "health": 95
    },
    "temperature": {
      "engine": 85,
      "cabin": 22,
      "outside": 28
    }
  },
  "warnings": [
    {"code": "W001", "message": "Low washer fluid", "severity": "low"},
    {"code": "W002", "message": "Tire pressure low", "severity": "medium"}
  ],
  "drive_mode": {
    "current": "eco",
    "available": ["eco", "normal", "sport", "snow"]
  },
  "diagnostics": {
    "dtc_codes": [],
    "last_service": "2024-12-01",
    "next_service_km": 5000
  }
}
```

---

## 2. Các API mô phỏng theo chuẩn ara::

### 2.1 ara::com - Truyền nhận Signal

```cpp
namespace ara::com {
    // Service Discovery
    template<typename T>
    Future<ServiceHandleContainer<T>> FindService(InstanceIdentifier id);
    
    // Event handling
    template<typename T>
    class Event {
        void Subscribe(size_t maxSamples);
        void Unsubscribe();
        Future<size_t> GetNewSamples(SampleHandler handler);
    };
    
    // Field (get/set)
    template<typename T>
    class Field {
        Future<T> Get();
        Future<void> Set(const T& value);
    };
}
```

### 2.2 ara::core::Future - Callback Bất đồng bộ

```cpp
namespace ara::core {
    template<typename T>
    class Future {
        // Blocking wait
        T get();
        
        // Non-blocking check
        bool is_ready();
        
        // Callback khi hoàn thành
        template<typename F>
        auto Then(F&& func);
        
        // Error handling
        template<typename F>
        auto OnError(F&& func);
    };
}
```

### 2.3 ara::log - Ghi Log hệ thống

```cpp
namespace ara::log {
    enum class LogLevel { kOff, kFatal, kError, kWarn, kInfo, kDebug, kVerbose };
    
    class Logger {
        LogStream LogFatal();
        LogStream LogError();
        LogStream LogWarn();
        LogStream LogInfo();
        LogStream LogDebug();
        LogStream LogVerbose();
    };
    
    Logger CreateLogger(std::string ctxId, std::string ctxDescription);
}
```

### 2.4 ara::diag - Phát hiện Lỗi

```cpp
namespace ara::diag {
    class DiagnosticMonitor {
        void ReportMonitorAction(MonitorAction action);
        // Actions: kPassed, kFailed, kPrepassed, kPrefailed
    };
    
    class DTCInformation {
        std::vector<DTCCode> GetDTCList();
        void ClearDTC(DTCCode code);
    };
}
```

---

## 3. API mô phỏng cụ thể cho DashboardService

| API | Mô tả | Return Type |
|-----|-------|-------------|
| `getVehicleSpeed()` | Lấy tốc độ từ JSON | `ara::core::Future<float>` |
| `getWarningList()` | Lấy danh sách cảnh báo | `ara::core::Future<vector<Warning>>` |
| `getBatteryStatus()` | Trạng thái pin | `ara::core::Future<BatteryInfo>` |
| `getDriveMode()` | Chế độ lái hiện tại | `ara::core::Future<string>` |
| `setDriveMode(mode)` | Đặt chế độ lái | `ara::core::Future<bool>` |
| `publishAllSignals()` | Gửi tất cả event | `void` |

---

## 4. Kiến trúc DashboardService

```
┌─────────────────────────────────────────────────────────────┐
│                    DashboardService                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐                   │
│  │  VehicleDB      │  │  DiagMonitor    │                   │
│  │  (JSON Reader)  │  │  (ara::diag)    │                   │
│  └────────┬────────┘  └────────┬────────┘                   │
│           │                    │                             │
│  ┌────────┴────────────────────┴────────┐                   │
│  │           Service Skeleton            │                   │
│  │  ┌──────────────────────────────┐    │                   │
│  │  │     Methods (ara::com)       │    │                   │
│  │  │  - getVehicleSpeed()         │    │                   │
│  │  │  - getBatteryStatus()        │    │                   │
│  │  │  - getWarningList()          │    │                   │
│  │  │  - getDriveMode()            │    │                   │
│  │  └──────────────────────────────┘    │                   │
│  │  ┌──────────────────────────────┐    │                   │
│  │  │     Events (ara::com)        │    │                   │
│  │  │  - SpeedUpdateEvent          │    │                   │
│  │  │  - BatteryUpdateEvent        │    │                   │
│  │  │  - WarningEvent              │    │                   │
│  │  └──────────────────────────────┘    │                   │
│  │  ┌──────────────────────────────┐    │                   │
│  │  │     Fields (ara::com)        │    │                   │
│  │  │  - DriveMode                 │    │                   │
│  │  │  - VehicleState              │    │                   │
│  │  └──────────────────────────────┘    │                   │
│  └──────────────────────────────────────┘                   │
├─────────────────────────────────────────────────────────────┤
│                    SOME/IP Transport                         │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. Ví dụ Code

### 📝 Ví dụ 1: Code Cơ bản
Xem: `examples/basic/`

### 📝 Ví dụ 2: Code Nâng cao
Xem: `examples/advanced/`

---

## 6. Bài tập thực hành

1. **Mở rộng JSON Database**: Thêm các sensor mới (GPS, ADAS status)
2. **Thêm API mới**: Implement `getGPSLocation()`
3. **Error Handling**: Xử lý khi sensor bị disconnect
4. **Logging**: Thêm log cho tất cả các API calls
5. **Simulation**: Tạo thread mô phỏng thay đổi tốc độ theo thời gian
