# Bài 3: Tích hợp Qt UI với Service Backend

## 📚 Mục tiêu bài học
- Tích hợp client vsomeip trong Qt application
- Sử dụng `ara::com::FindService()` để phát hiện service
- Sử dụng `ara::com::SampledDataField` để nhận dữ liệu
- Gắn sự kiện với `ara::core::Future::Then()` để cập nhật UI
- Mapping dữ liệu từ backend → Qt widgets (QLabel, QProgressBar, etc.)

---

## 1. Kiến trúc Qt + SOME/IP

```
┌─────────────────────────────────────────────────────────────────┐
│                        Qt Application                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    MainWindow                            │    │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐        │    │
│  │  │ SpeedWidget │ │BatteryWidget│ │WarningWidget│        │    │
│  │  │  QLabel     │ │ QProgressBar│ │ QListWidget │        │    │
│  │  │  QLCDNumber │ │ QLabel      │ │             │        │    │
│  │  └─────────────┘ └─────────────┘ └─────────────┘        │    │
│  └──────────────────────────┬──────────────────────────────┘    │
│                             │                                    │
│  ┌──────────────────────────▼──────────────────────────────┐    │
│  │              ServiceBridge (QObject)                     │    │
│  │  - Chuyển đổi signals giữa vsomeip và Qt                │    │
│  │  - emit Qt signals khi nhận data từ service             │    │
│  └──────────────────────────┬──────────────────────────────┘    │
│                             │                                    │
│  ┌──────────────────────────▼──────────────────────────────┐    │
│  │              DashboardServiceProxy                       │    │
│  │  - ara::com::FindService                                │    │
│  │  - ara::com::Event subscription                         │    │
│  │  - ara::core::Future handling                           │    │
│  └──────────────────────────┬──────────────────────────────┘    │
├─────────────────────────────┼───────────────────────────────────┤
│                             ▼                                    │
│                    SOME/IP (vsomeip)                            │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Qt + vsomeip Threading Model

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                  │
│   Main Thread (Qt Event Loop)    │   Worker Thread (vsomeip)   │
│                                   │                              │
│   ┌─────────────────────────┐    │   ┌─────────────────────┐   │
│   │ QApplication::exec()    │    │   │ app->start()        │   │
│   │                         │    │   │                     │   │
│   │  ┌─────────────────┐    │    │   │ Message Handlers    │   │
│   │  │ Qt Signal/Slot  │<───┼────┼───│ OnSpeedUpdate()    │   │
│   │  └────────┬────────┘    │    │   │ OnBatteryUpdate()  │   │
│   │           │             │    │   │ OnWarningUpdate()  │   │
│   │  ┌────────▼────────┐    │    │   └─────────────────────┘   │
│   │  │ Update UI       │    │    │                              │
│   │  │ QLabel->setText │    │    │                              │
│   │  └─────────────────┘    │    │                              │
│   └─────────────────────────┘    │   ┌─────────────────────┐   │
│                                   │   │ Event Subscription  │   │
│                                   │   └─────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘

QUAN TRỌNG: 
- Không cập nhật UI trực tiếp từ vsomeip thread!
- Sử dụng Qt signals/slots với Qt::QueuedConnection
- Hoặc sử dụng QMetaObject::invokeMethod(obj, Qt::QueuedConnection)
```

---

## 3. Các ara::com Types trong Context Qt

### 3.1 ServiceProxy

```cpp
class DashboardServiceProxy {
public:
    // Mô phỏng ara::com::FindService
    static std::shared_ptr<DashboardServiceProxy> FindService(
        const ara::com::InstanceIdentifier& id);
    
    // Events - subscribe và nhận updates
    ara::com::Event<SpeedData> speedEvent;
    ara::com::Event<BatteryData> batteryEvent;
    
    // Fields - get/set với caching
    ara::com::Field<std::string> driveMode;
    
    // Methods - remote procedure calls
    ara::core::Future<float> GetSpeed();
    ara::core::Future<bool> SetDriveMode(const std::string& mode);
};
```

### 3.2 Event Subscription trong Qt

```cpp
// Subscribe với callback cập nhật UI
proxy->speedEvent.Subscribe([this](const SpeedData& data) {
    // KHÔNG gọi UI methods trực tiếp!
    // Emit Qt signal thay vì update trực tiếp
    emit speedUpdated(data.value);
});

// Qt slot sẽ xử lý trong main thread
void MainWindow::onSpeedUpdated(float speed) {
    ui->speedLabel->setText(QString::number(speed) + " km/h");
    ui->speedometer->setValue(static_cast<int>(speed));
}
```

### 3.3 ara::core::Future::Then() với Qt

```cpp
// Gọi method async
proxy->GetSpeed().Then([this](float speed) {
    // Callback chạy trong vsomeip thread
    // Sử dụng QMetaObject để chuyển sang main thread
    QMetaObject::invokeMethod(this, [this, speed]() {
        ui->speedLabel->setText(QString::number(speed) + " km/h");
    }, Qt::QueuedConnection);
});
```

---

## 4. Cấu hình Timeout, Reconnect, Callback

### 4.1 Service Discovery Timeout

```cpp
// Timeout khi tìm service
bool WaitForService(int timeout_ms = 5000) {
    std::unique_lock<std::mutex> lock(mutex_);
    return cv_.wait_for(lock, 
        std::chrono::milliseconds(timeout_ms),
        [this] { return service_available_; });
}
```

### 4.2 Auto Reconnect

```cpp
void OnAvailabilityChange(bool available) {
    if (!available) {
        emit serviceDisconnected();
        
        // Start reconnect timer
        reconnect_timer_->start(5000); // Retry every 5s
    } else {
        reconnect_timer_->stop();
        emit serviceConnected();
    }
}
```

### 4.3 Data Update Callbacks

```cpp
// Callback pattern cho continuous updates
proxy->SubscribeAllSignals([this](const VehicleState& state) {
    // Buffer dữ liệu cho thread-safe access
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        cached_state_ = state;
    }
    
    // Notify Qt main thread
    emit vehicleStateUpdated();
});
```

---

## 5. Mapping dữ liệu Backend → UI

| Backend Data | Qt Widget | Update Method |
|-------------|-----------|---------------|
| `speed` (float) | QLabel, QLCDNumber | `setText()`, `display()` |
| `battery_level` (%) | QProgressBar | `setValue()` |
| `warnings` (list) | QListWidget | `addItem()`, `clear()` |
| `drive_mode` (string) | QComboBox | `setCurrentText()` |
| `temperature` (float) | QLabel với color | `setStyleSheet()` |
| `charging` (bool) | QLabel icon | `setPixmap()` |

### Ví dụ Mapping

```cpp
void MainWindow::updateUI(const VehicleState& state) {
    // Speed
    ui->speedLabel->setText(QString::number(state.speed, 'f', 1) + " km/h");
    ui->speedProgress->setValue(qMin(200, static_cast<int>(state.speed)));
    
    // Battery với color coding
    ui->batteryBar->setValue(static_cast<int>(state.battery_level));
    if (state.battery_level < 20) {
        ui->batteryBar->setStyleSheet("QProgressBar::chunk { background: red; }");
    } else if (state.battery_level < 50) {
        ui->batteryBar->setStyleSheet("QProgressBar::chunk { background: orange; }");
    } else {
        ui->batteryBar->setStyleSheet("QProgressBar::chunk { background: green; }");
    }
    
    // Warnings
    ui->warningList->clear();
    for (const auto& w : state.warnings) {
        auto item = new QListWidgetItem(QString::fromStdString(w.message));
        if (w.severity == "high") {
            item->setBackground(Qt::red);
        }
        ui->warningList->addItem(item);
    }
    
    // Drive Mode
    ui->modeCombo->setCurrentText(QString::fromStdString(state.drive_mode));
}
```

---

## 6. Ví dụ Code

### 📝 Ví dụ 1: Code Cơ bản
Xem: `examples/basic/`
- Qt console application đơn giản
- Nhận data từ service và print

### 📝 Ví dụ 2: Code Nâng cao
Xem: `examples/advanced/`
- Full Qt GUI với widgets
- Real-time dashboard
- Tab interface

---

## 7. Best Practices

### Thread Safety
```cpp
// ✓ Đúng: Emit signal, Qt sẽ queue vào main thread
emit speedUpdated(speed);

// ✗ Sai: Gọi trực tiếp UI từ vsomeip thread
ui->speedLabel->setText(...); // CRASH hoặc undefined behavior!
```

### Resource Management
```cpp
// Disconnect signals khi destroy
~MainWindow() {
    if (service_bridge_) {
        disconnect(service_bridge_.get(), nullptr, this, nullptr);
    }
}
```

### Error Handling
```cpp
auto result = proxy->GetSpeed().GetWithTimeout(5000);
if (result.HasValue()) {
    updateSpeed(result.Value());
} else {
    switch (result.Error()) {
        case ErrorCode::kTimeout:
            showError("Request timeout");
            break;
        case ErrorCode::kServiceNotAvailable:
            showError("Service disconnected");
            break;
        default:
            showError("Unknown error");
    }
}
```

---

## 8. Bài tập thực hành

1. **Tạo Speedometer Widget**: Sử dụng QPainter vẽ đồng hồ tốc độ
2. **Battery Animation**: Thêm animation khi pin đang sạc
3. **Warning Popup**: Hiện dialog khi có warning mức "high"
4. **Multiple Services**: Kết nối nhiều services cùng lúc
5. **Settings Dialog**: Cho phép cấu hình Service ID/Instance từ UI
