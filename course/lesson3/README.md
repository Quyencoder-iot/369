# 📘 BÀI 3: Tích Hợp Qt UI với Service Backend

## 📑 Mục Lục

1. [Giới thiệu Qt với SOME/IP](#1-giới-thiệu-qt-với-someip)
2. [Kiến trúc tích hợp](#2-kiến-trúc-tích-hợp)
3. [ara::com concepts cho Qt](#3-aracom-concepts-cho-qt)
4. [Ví dụ 1: Qt Client đơn giản](#4-ví-dụ-1-qt-client-đơn-giản)
5. [Ví dụ 2: Dashboard UI hoàn chỉnh](#5-ví-dụ-2-dashboard-ui-hoàn-chỉnh)

---

## 1. Giới Thiệu Qt với SOME/IP

### 1.1 Tại sao dùng Qt?

- **Cross-platform**: Linux, Windows, macOS, Embedded
- **Mature framework**: Signal/Slot, Event loop, UI widgets
- **Automotive ready**: Qt Automotive Suite
- **Performance**: C++ native, OpenGL rendering

### 1.2 Thách thức tích hợp

```
┌─────────────────────────────────────────────────────────────┐
│                        CHALLENGES                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────┐           ┌─────────────┐                  │
│  │   Qt Event  │           │  vsomeip    │                  │
│  │    Loop     │    ???    │  Callbacks  │                  │
│  │  (Main UI)  │           │  (Thread)   │                  │
│  └─────────────┘           └─────────────┘                  │
│                                                              │
│  Problem: vsomeip callbacks run in different thread!         │
│  Solution: Qt Signal/Slot with Qt::QueuedConnection         │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 Giải pháp

```cpp
// Sử dụng Qt Signal/Slot để bridge giữa vsomeip và UI
class ServiceClient : public QObject {
    Q_OBJECT
    
signals:
    void speedUpdated(int speed);      // Emit từ vsomeip callback
    void batteryUpdated(int level);
    
public slots:
    void onSpeedChanged(int speed);    // Update UI
};

// Kết nối với QueuedConnection
connect(client, &ServiceClient::speedUpdated,
        this, &MainWindow::updateSpeedDisplay,
        Qt::QueuedConnection);  // Thread-safe!
```

---

## 2. Kiến Trúc Tích Hợp

### 2.1 Layer Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Qt APPLICATION                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────┐    │
│  │                    UI LAYER                          │    │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐             │    │
│  │  │QLabel   │  │QProgress│  │QComboBox│  ...        │    │
│  │  │(Speed)  │  │Bar(Batt)│  │(Mode)   │             │    │
│  │  └────┬────┘  └────┬────┘  └────┬────┘             │    │
│  └───────┼────────────┼────────────┼───────────────────┘    │
│          │            │            │                         │
│  ┌───────▼────────────▼────────────▼───────────────────┐    │
│  │              SERVICE CLIENT LAYER                    │    │
│  │  ┌─────────────────────────────────────────────┐    │    │
│  │  │  DashboardProxy (QObject)                   │    │    │
│  │  │  - findService()                            │    │    │
│  │  │  - subscribeEvents()                        │    │    │
│  │  │  - callMethod()                             │    │    │
│  │  │  + signals: dataUpdated(...)                │    │    │
│  │  └─────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────┘    │
│                            │                                 │
├────────────────────────────┼─────────────────────────────────┤
│                            ▼                                 │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              VSOMEIP LAYER (Separate Thread)         │    │
│  │  - application->start() runs in background           │    │
│  │  - callbacks emit Qt signals                          │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Threading Model

```cpp
// Main Thread: Qt Event Loop
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();  // Qt event loop
}

// Background Thread: vsomeip
class SomeipWorker : public QThread {
    void run() override {
        vsomeip_app->start();  // Blocking, runs in this thread
    }
};
```

---

## 3. ara::com Concepts cho Qt

### 3.1 ServiceProxy Pattern

```cpp
// Mô phỏng ara::com::FindService() với Qt
class DashboardProxy : public QObject {
    Q_OBJECT
    
public:
    // Tương tự ara::com::FindService()
    static QFuture<DashboardProxy*> FindService() {
        return QtConcurrent::run([]() -> DashboardProxy* {
            // Tìm service qua vsomeip
            // Trả về proxy khi tìm thấy
        });
    }
    
signals:
    // Tương tự ara::com::Event
    void speedEvent(uint16_t speed);
    void batteryEvent(BatteryInfo info);
    void warningsEvent(QVector<Warning> warnings);
    
public slots:
    // Tương tự ara::com::Method
    void getSpeed();
    void getBattery();
    void setDriveMode(DriveMode mode);
};
```

### 3.2 SampledDataField Pattern

```cpp
// Mô phỏng ara::com::SampledDataField
template<typename T>
class SampledDataField : public QObject {
    Q_OBJECT
    
public:
    // Lấy giá trị hiện tại
    T value() const { return value_; }
    
    // Subscribe để nhận updates
    void subscribe() {
        // Đăng ký với service
    }
    
signals:
    void valueChanged(T newValue);
    
private:
    T value_;
};

// Sử dụng
SampledDataField<uint16_t> speedField;
connect(&speedField, &SampledDataField<uint16_t>::valueChanged,
        speedLabel, [](uint16_t speed) {
            speedLabel->setText(QString::number(speed) + " km/h");
        });
```

### 3.3 Future::Then() với Qt

```cpp
// ara::core::Future::Then() với Qt
template<typename T>
class QtFuture {
public:
    template<typename F>
    void Then(F&& callback) {
        QFutureWatcher<T>* watcher = new QFutureWatcher<T>;
        
        connect(watcher, &QFutureWatcher<T>::finished, 
                [watcher, callback]() {
            callback(watcher->result());
            watcher->deleteLater();
        });
        
        watcher->setFuture(future_);
    }
    
private:
    QFuture<T> future_;
};

// Sử dụng
proxy->getSpeedAsync().Then([this](uint16_t speed) {
    ui->speedLabel->setText(QString::number(speed));
});
```

---

## 4. Ví Dụ 1: Qt Client Đơn Giản

### 📁 Cấu trúc

```
example1/
├── main.cpp              # Entry point
├── mainwindow.h          # UI class header
├── mainwindow.cpp        # UI class implementation
├── mainwindow.ui         # Qt Designer file
├── serviceclient.h       # vsomeip wrapper
├── serviceclient.cpp
└── CMakeLists.txt
```

### 📝 serviceclient.h (Minimal)

```cpp
// ============================================
// VÍ DỤ 1: SERVICE CLIENT - BRIDGE QT & VSOMEIP
// Yếu tố BẮT BUỘC:
// 1. Kế thừa QObject
// 2. Khai báo signals cho data updates
// 3. Chạy vsomeip trong thread riêng
// ============================================

#ifndef SERVICECLIENT_H
#define SERVICECLIENT_H

#include <QObject>
#include <QThread>
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <atomic>

class ServiceClient : public QObject {
    Q_OBJECT
    
public:
    explicit ServiceClient(QObject *parent = nullptr);
    ~ServiceClient();
    
    void start();
    void stop();
    bool isConnected() const { return connected_; }

signals:
    // Data signals (BẮT BUỘC)
    void connected();
    void disconnected();
    void speedUpdated(int speed);
    void batteryUpdated(int level);
    void driveModeUpdated(int mode);
    void errorOccurred(QString message);

public slots:
    void requestSpeed();
    void requestBattery();

private:
    void initVsomeip();
    void onState(vsomeip::state_type_e state);
    void onAvailability(vsomeip::service_t s, vsomeip::instance_t i, bool available);
    void onMessage(const std::shared_ptr<vsomeip::message>& msg);
    
    std::shared_ptr<vsomeip::application> app_;
    QThread* workerThread_;
    std::atomic<bool> running_;
    std::atomic<bool> connected_;
    
    static constexpr vsomeip::service_t SERVICE_ID = 0x2000;
    static constexpr vsomeip::instance_t INSTANCE_ID = 0x0001;
    static constexpr vsomeip::method_t METHOD_GET_SPEED = 0x0001;
    static constexpr vsomeip::method_t METHOD_GET_BATTERY = 0x0002;
};

#endif // SERVICECLIENT_H
```

### 📝 serviceclient.cpp (Minimal)

```cpp
#include "serviceclient.h"
#include <QDebug>

ServiceClient::ServiceClient(QObject *parent)
    : QObject(parent)
    , workerThread_(nullptr)
    , running_(false)
    , connected_(false)
{
}

ServiceClient::~ServiceClient() {
    stop();
}

void ServiceClient::start() {
    if (running_) return;
    
    running_ = true;
    
    // Tạo vsomeip trong thread riêng
    workerThread_ = QThread::create([this]() {
        initVsomeip();
        app_->start();  // Blocking
    });
    
    workerThread_->start();
    qDebug() << "Service client started";
}

void ServiceClient::stop() {
    if (!running_) return;
    
    running_ = false;
    
    if (app_) {
        app_->stop();
    }
    
    if (workerThread_ && workerThread_->isRunning()) {
        workerThread_->quit();
        workerThread_->wait(3000);
    }
    
    qDebug() << "Service client stopped";
}

void ServiceClient::initVsomeip() {
    app_ = vsomeip::runtime::get()->create_application("qt_client");
    
    if (!app_->init()) {
        emit errorOccurred("Failed to init vsomeip");
        return;
    }
    
    // Register handlers
    app_->register_state_handler(
        std::bind(&ServiceClient::onState, this, std::placeholders::_1));
    
    app_->register_availability_handler(SERVICE_ID, INSTANCE_ID,
        std::bind(&ServiceClient::onAvailability, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    app_->register_message_handler(SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
        std::bind(&ServiceClient::onMessage, this, std::placeholders::_1));
    
    app_->request_service(SERVICE_ID, INSTANCE_ID);
}

void ServiceClient::onState(vsomeip::state_type_e state) {
    qDebug() << "State:" << (state == vsomeip::state_type_e::ST_REGISTERED 
                             ? "Registered" : "Deregistered");
}

void ServiceClient::onAvailability(vsomeip::service_t, vsomeip::instance_t, bool available) {
    connected_ = available;
    
    if (available) {
        emit connected();
        qDebug() << "Service connected";
    } else {
        emit disconnected();
        qDebug() << "Service disconnected";
    }
}

void ServiceClient::onMessage(const std::shared_ptr<vsomeip::message>& msg) {
    auto payload = msg->get_payload();
    if (!payload) return;
    
    auto method = msg->get_method();
    
    // Xử lý response và emit signal (thread-safe qua queued connection)
    if (method == METHOD_GET_SPEED) {
        uint16_t speed = (payload->get_data()[0] << 8) | payload->get_data()[1];
        emit speedUpdated(speed);
    } 
    else if (method == METHOD_GET_BATTERY) {
        uint8_t level = payload->get_data()[0];
        emit batteryUpdated(level);
    }
}

void ServiceClient::requestSpeed() {
    if (!connected_ || !app_) return;
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(METHOD_GET_SPEED);
    app_->send(request);
}

void ServiceClient::requestBattery() {
    if (!connected_ || !app_) return;
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(METHOD_GET_BATTERY);
    app_->send(request);
}
```

### 📝 mainwindow.h (Minimal)

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include "serviceclient.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnected();
    void onDisconnected();
    void onSpeedUpdated(int speed);
    void onBatteryUpdated(int level);
    void onRefresh();

private:
    void setupUI();
    
    ServiceClient* client_;
    
    // UI elements
    QLabel* statusLabel_;
    QLabel* speedLabel_;
    QProgressBar* batteryBar_;
    QPushButton* refreshBtn_;
};

#endif // MAINWINDOW_H
```

### 📝 mainwindow.cpp (Minimal)

```cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    
    // Create service client
    client_ = new ServiceClient(this);
    
    // Connect signals với QueuedConnection (BẮT BUỘC cho thread safety)
    connect(client_, &ServiceClient::connected,
            this, &MainWindow::onConnected, Qt::QueuedConnection);
    connect(client_, &ServiceClient::disconnected,
            this, &MainWindow::onDisconnected, Qt::QueuedConnection);
    connect(client_, &ServiceClient::speedUpdated,
            this, &MainWindow::onSpeedUpdated, Qt::QueuedConnection);
    connect(client_, &ServiceClient::batteryUpdated,
            this, &MainWindow::onBatteryUpdated, Qt::QueuedConnection);
    
    connect(refreshBtn_, &QPushButton::clicked, this, &MainWindow::onRefresh);
    
    // Start client
    client_->start();
}

MainWindow::~MainWindow() {
    client_->stop();
}

void MainWindow::setupUI() {
    setWindowTitle("Dashboard Client");
    setMinimumSize(400, 300);
    
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    
    // Status
    statusLabel_ = new QLabel("Connecting...");
    statusLabel_->setStyleSheet("color: orange; font-weight: bold;");
    layout->addWidget(statusLabel_);
    
    // Speed
    QGroupBox* speedGroup = new QGroupBox("Speed");
    QVBoxLayout* speedLayout = new QVBoxLayout(speedGroup);
    speedLabel_ = new QLabel("--- km/h");
    speedLabel_->setStyleSheet("font-size: 48px; font-weight: bold;");
    speedLabel_->setAlignment(Qt::AlignCenter);
    speedLayout->addWidget(speedLabel_);
    layout->addWidget(speedGroup);
    
    // Battery
    QGroupBox* batteryGroup = new QGroupBox("Battery");
    QVBoxLayout* batteryLayout = new QVBoxLayout(batteryGroup);
    batteryBar_ = new QProgressBar();
    batteryBar_->setRange(0, 100);
    batteryBar_->setValue(0);
    batteryLayout->addWidget(batteryBar_);
    layout->addWidget(batteryGroup);
    
    // Refresh button
    refreshBtn_ = new QPushButton("Refresh");
    layout->addWidget(refreshBtn_);
    
    setCentralWidget(central);
}

void MainWindow::onConnected() {
    statusLabel_->setText("Connected");
    statusLabel_->setStyleSheet("color: green; font-weight: bold;");
    refreshBtn_->setEnabled(true);
    onRefresh();
}

void MainWindow::onDisconnected() {
    statusLabel_->setText("Disconnected");
    statusLabel_->setStyleSheet("color: red; font-weight: bold;");
    refreshBtn_->setEnabled(false);
}

void MainWindow::onSpeedUpdated(int speed) {
    speedLabel_->setText(QString::number(speed) + " km/h");
}

void MainWindow::onBatteryUpdated(int level) {
    batteryBar_->setValue(level);
    
    // Color based on level
    if (level < 20) {
        batteryBar_->setStyleSheet("QProgressBar::chunk { background-color: red; }");
    } else if (level < 50) {
        batteryBar_->setStyleSheet("QProgressBar::chunk { background-color: orange; }");
    } else {
        batteryBar_->setStyleSheet("QProgressBar::chunk { background-color: green; }");
    }
}

void MainWindow::onRefresh() {
    client_->requestSpeed();
    client_->requestBattery();
}
```

### 📝 main.cpp

```cpp
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
```

### 📝 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(qt_dashboard_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

add_executable(qt_client
    main.cpp
    mainwindow.cpp
    mainwindow.h
    serviceclient.cpp
    serviceclient.h
)

target_link_libraries(qt_client
    Qt5::Widgets
    Qt5::Core
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)
```

---

## 5. Ví Dụ 2: Dashboard UI Hoàn Chỉnh

### 📁 Cấu trúc

```
example2/
├── main.cpp
├── dashboard/
│   ├── dashboardwindow.h
│   ├── dashboardwindow.cpp
│   └── dashboardwindow.ui
├── widgets/
│   ├── speedgauge.h        # Custom speedometer
│   ├── speedgauge.cpp
│   ├── batterywidget.h     # Battery display
│   └── batterywidget.cpp
├── service/
│   ├── dashboardproxy.h    # ara::com style proxy
│   ├── dashboardproxy.cpp
│   ├── datatypes.h         # Shared data types
│   └── vsomeipworker.h     # Worker thread
├── resources/
│   └── dashboard.qrc
└── CMakeLists.txt
```

### 📝 service/datatypes.h

```cpp
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

namespace dashboard {

// Battery info
struct BatteryInfo {
    int level;          // 0-100%
    float voltage;      // Volts
    int temperature;    // Celsius
    bool charging;
    
    BatteryInfo() : level(0), voltage(0), temperature(0), charging(false) {}
};

// Warning
struct Warning {
    QString code;
    QString message;
    int severity;       // 0=low, 1=medium, 2=high
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

// All dashboard data
struct DashboardData {
    int speed;
    int rpm;
    int gear;
    BatteryInfo battery;
    QVector<Warning> warnings;
    DriveMode driveMode;
    qint64 timestamp;
    
    DashboardData() : speed(0), rpm(0), gear(0), 
                      driveMode(DriveMode::NORMAL), timestamp(0) {}
};

}  // namespace dashboard

// Register metatypes for Qt signal/slot
Q_DECLARE_METATYPE(dashboard::BatteryInfo)
Q_DECLARE_METATYPE(dashboard::Warning)
Q_DECLARE_METATYPE(dashboard::DriveMode)
Q_DECLARE_METATYPE(dashboard::DashboardData)

#endif // DATATYPES_H
```

### 📝 service/dashboardproxy.h

```cpp
// =============================================
// MODULE: DashboardProxy
// ---------------------------------------------
// Mục đích: Mô phỏng ara::com::ServiceProxy
// Giải thích:
// - FindService() tìm service
// - Events qua Qt signals
// - Methods qua async calls
// =============================================

#ifndef DASHBOARDPROXY_H
#define DASHBOARDPROXY_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <atomic>
#include <functional>
#include "datatypes.h"

namespace dashboard {

class DashboardProxy : public QObject {
    Q_OBJECT
    
public:
    // Service identifiers
    static constexpr uint16_t SERVICE_ID = 0x2000;
    static constexpr uint16_t INSTANCE_ID = 0x0001;
    
    explicit DashboardProxy(QObject *parent = nullptr);
    ~DashboardProxy();
    
    // ==========================================
    // ara::com::FindService() style
    // ==========================================
    static DashboardProxy* FindService(QObject* parent = nullptr);
    
    // Service state
    bool isAvailable() const { return serviceAvailable_; }
    
    // ==========================================
    // ara::com::Method calls
    // ==========================================
    void getSpeed();
    void getBattery();
    void getWarnings();
    void getDriveMode();
    void getAllData();
    
    // ==========================================
    // ara::com::Event subscriptions
    // ==========================================
    void subscribeAllSignals();
    void unsubscribeAllSignals();

signals:
    // Service state
    void serviceAvailable();
    void serviceUnavailable();
    void errorOccurred(QString message);
    
    // Data events (ara::com::Event style)
    void speedReceived(int speed);
    void rpmReceived(int rpm);
    void batteryReceived(BatteryInfo battery);
    void warningsReceived(QVector<Warning> warnings);
    void driveModeReceived(DriveMode mode);
    void allDataReceived(DashboardData data);

public slots:
    void start();
    void stop();

private:
    // vsomeip handlers
    void onState(vsomeip::state_type_e state);
    void onAvailability(vsomeip::service_t s, vsomeip::instance_t i, bool available);
    void onMessage(const std::shared_ptr<vsomeip::message>& msg);
    
    // Helpers
    void sendRequest(vsomeip::method_t method);
    void parseAllSignalsEvent(const std::shared_ptr<vsomeip::payload>& payload);
    
    // Members
    std::shared_ptr<vsomeip::application> app_;
    QThread* workerThread_;
    std::atomic<bool> running_;
    std::atomic<bool> serviceAvailable_;
    
    // Method IDs
    static constexpr vsomeip::method_t METHOD_GET_SPEED = 0x0001;
    static constexpr vsomeip::method_t METHOD_GET_BATTERY = 0x0002;
    static constexpr vsomeip::method_t METHOD_GET_WARNINGS = 0x0003;
    static constexpr vsomeip::method_t METHOD_GET_DRIVEMODE = 0x0004;
    static constexpr vsomeip::event_t EVENT_ALL_SIGNALS = 0x8001;
    static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;
};

}  // namespace dashboard

#endif // DASHBOARDPROXY_H
```

### 📝 service/dashboardproxy.cpp

```cpp
#include "dashboardproxy.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace dashboard {

DashboardProxy::DashboardProxy(QObject *parent)
    : QObject(parent)
    , workerThread_(nullptr)
    , running_(false)
    , serviceAvailable_(false)
{
    // Register metatypes
    qRegisterMetaType<BatteryInfo>("BatteryInfo");
    qRegisterMetaType<Warning>("Warning");
    qRegisterMetaType<DriveMode>("DriveMode");
    qRegisterMetaType<DashboardData>("DashboardData");
    qRegisterMetaType<QVector<Warning>>("QVector<Warning>");
}

DashboardProxy::~DashboardProxy() {
    stop();
}

DashboardProxy* DashboardProxy::FindService(QObject* parent) {
    auto* proxy = new DashboardProxy(parent);
    proxy->start();
    return proxy;
}

void DashboardProxy::start() {
    if (running_) return;
    
    running_ = true;
    
    workerThread_ = QThread::create([this]() {
        qDebug() << "[Proxy] Creating vsomeip application...";
        
        app_ = vsomeip::runtime::get()->create_application("dashboard_qt_client");
        
        if (!app_->init()) {
            emit errorOccurred("Failed to initialize vsomeip");
            return;
        }
        
        // Register handlers
        app_->register_state_handler(
            std::bind(&DashboardProxy::onState, this, std::placeholders::_1));
        
        app_->register_availability_handler(SERVICE_ID, INSTANCE_ID,
            std::bind(&DashboardProxy::onAvailability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        
        app_->register_message_handler(SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&DashboardProxy::onMessage, this, std::placeholders::_1));
        
        // Request service
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        qDebug() << "[Proxy] Starting vsomeip...";
        app_->start();  // Blocking
    });
    
    workerThread_->start();
}

void DashboardProxy::stop() {
    if (!running_) return;
    
    qDebug() << "[Proxy] Stopping...";
    running_ = false;
    
    if (serviceAvailable_) {
        app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        app_->release_service(SERVICE_ID, INSTANCE_ID);
    }
    
    if (app_) {
        app_->clear_all_handler();
        app_->stop();
    }
    
    if (workerThread_ && workerThread_->isRunning()) {
        workerThread_->quit();
        workerThread_->wait(3000);
    }
    
    qDebug() << "[Proxy] Stopped";
}

void DashboardProxy::onState(vsomeip::state_type_e state) {
    qDebug() << "[Proxy] State:" 
             << (state == vsomeip::state_type_e::ST_REGISTERED ? "Registered" : "Deregistered");
}

void DashboardProxy::onAvailability(vsomeip::service_t, vsomeip::instance_t, bool available) {
    qDebug() << "[Proxy] Service" << (available ? "available" : "unavailable");
    
    serviceAvailable_ = available;
    
    if (available) {
        subscribeAllSignals();
        emit serviceAvailable();
    } else {
        emit serviceUnavailable();
    }
}

void DashboardProxy::onMessage(const std::shared_ptr<vsomeip::message>& msg) {
    auto payload = msg->get_payload();
    if (!payload || payload->get_length() == 0) return;
    
    auto msgType = msg->get_message_type();
    auto method = msg->get_method();
    
    // Event notification
    if (msgType == vsomeip::message_type_e::MT_NOTIFICATION) {
        if (method == EVENT_ALL_SIGNALS) {
            parseAllSignalsEvent(payload);
        }
        return;
    }
    
    // Method response
    auto data = payload->get_data();
    auto len = payload->get_length();
    
    switch (method) {
        case METHOD_GET_SPEED: {
            if (len >= 2) {
                int speed = (data[0] << 8) | data[1];
                emit speedReceived(speed);
            }
            break;
        }
        
        case METHOD_GET_BATTERY: {
            if (len >= 6) {
                BatteryInfo battery;
                battery.level = data[0];
                battery.voltage = ((data[1] << 8) | data[2]) / 100.0f;
                battery.temperature = static_cast<int16_t>((data[3] << 8) | data[4]);
                battery.charging = data[5] != 0;
                emit batteryReceived(battery);
            }
            break;
        }
        
        case METHOD_GET_WARNINGS: {
            QString jsonStr = QString::fromUtf8(
                reinterpret_cast<const char*>(data), len);
            QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
            
            QVector<Warning> warnings;
            for (const auto& item : doc.array()) {
                QJsonObject obj = item.toObject();
                Warning w;
                w.code = obj["code"].toString();
                w.message = obj["message"].toString();
                w.severity = obj["severity"].toInt();
                warnings.append(w);
            }
            emit warningsReceived(warnings);
            break;
        }
        
        case METHOD_GET_DRIVEMODE: {
            if (len >= 1) {
                emit driveModeReceived(static_cast<DriveMode>(data[0]));
            }
            break;
        }
    }
}

void DashboardProxy::parseAllSignalsEvent(const std::shared_ptr<vsomeip::payload>& payload) {
    QString jsonStr = QString::fromUtf8(
        reinterpret_cast<const char*>(payload->get_data()),
        payload->get_length());
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = doc.object();
    
    DashboardData data;
    data.speed = obj["speed"].toInt();
    data.rpm = obj["rpm"].toInt();
    data.gear = obj["gear"].toInt();
    data.driveMode = static_cast<DriveMode>(
        obj["driveMode"].toString() == "sport" ? 2 :
        obj["driveMode"].toString() == "eco" ? 0 : 1);
    data.timestamp = obj["timestamp"].toVariant().toLongLong();
    
    // Battery
    QJsonObject battObj = obj["battery"].toObject();
    data.battery.level = battObj["level"].toInt();
    data.battery.voltage = battObj["voltage"].toDouble();
    data.battery.temperature = battObj["temp"].toInt();
    data.battery.charging = battObj["charging"].toBool();
    
    emit allDataReceived(data);
    emit speedReceived(data.speed);
    emit rpmReceived(data.rpm);
    emit batteryReceived(data.battery);
    emit driveModeReceived(data.driveMode);
}

void DashboardProxy::subscribeAllSignals() {
    if (!app_ || !serviceAvailable_) return;
    
    std::set<vsomeip::eventgroup_t> groups;
    groups.insert(EVENTGROUP_ID);
    
    app_->request_event(SERVICE_ID, INSTANCE_ID, EVENT_ALL_SIGNALS,
        groups, vsomeip::event_type_e::ET_FIELD);
    
    app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
    
    qDebug() << "[Proxy] Subscribed to events";
}

void DashboardProxy::unsubscribeAllSignals() {
    if (!app_) return;
    app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
}

void DashboardProxy::sendRequest(vsomeip::method_t method) {
    if (!app_ || !serviceAvailable_) {
        emit errorOccurred("Service not available");
        return;
    }
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(method);
    app_->send(request);
}

void DashboardProxy::getSpeed() {
    sendRequest(METHOD_GET_SPEED);
}

void DashboardProxy::getBattery() {
    sendRequest(METHOD_GET_BATTERY);
}

void DashboardProxy::getWarnings() {
    sendRequest(METHOD_GET_WARNINGS);
}

void DashboardProxy::getDriveMode() {
    sendRequest(METHOD_GET_DRIVEMODE);
}

void DashboardProxy::getAllData() {
    getSpeed();
    getBattery();
    getWarnings();
    getDriveMode();
}

}  // namespace dashboard
```

### 📝 widgets/speedgauge.h

```cpp
// =============================================
// MODULE: SpeedGauge
// ---------------------------------------------
// Mục đích: Custom speedometer widget
// =============================================

#ifndef SPEEDGAUGE_H
#define SPEEDGAUGE_H

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>

class SpeedGauge : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    
public:
    explicit SpeedGauge(QWidget *parent = nullptr);
    
    int speed() const { return speed_; }
    int maxSpeed() const { return maxSpeed_; }
    
public slots:
    void setSpeed(int speed);
    void setMaxSpeed(int max);
    
signals:
    void speedChanged(int speed);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBackground(QPainter& painter);
    void drawScale(QPainter& painter);
    void drawNeedle(QPainter& painter);
    void drawValue(QPainter& painter);
    
    int speed_;
    int maxSpeed_;
    int animatedSpeed_;
    QPropertyAnimation* animation_;
};

#endif // SPEEDGAUGE_H
```

### 📝 widgets/speedgauge.cpp

```cpp
#include "speedgauge.h"
#include <QPainterPath>
#include <cmath>

SpeedGauge::SpeedGauge(QWidget *parent)
    : QWidget(parent)
    , speed_(0)
    , maxSpeed_(260)
    , animatedSpeed_(0)
{
    setMinimumSize(200, 200);
    
    animation_ = new QPropertyAnimation(this, "speed");
    animation_->setDuration(300);
    animation_->setEasingCurve(QEasingCurve::OutCubic);
}

void SpeedGauge::setSpeed(int speed) {
    if (speed_ == speed) return;
    
    speed_ = qBound(0, speed, maxSpeed_);
    emit speedChanged(speed_);
    update();
}

void SpeedGauge::setMaxSpeed(int max) {
    maxSpeed_ = max;
    update();
}

void SpeedGauge::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 220.0, side / 220.0);
    
    drawBackground(painter);
    drawScale(painter);
    drawNeedle(painter);
    drawValue(painter);
}

void SpeedGauge::drawBackground(QPainter& painter) {
    // Outer circle
    QRadialGradient gradient(0, 0, 100);
    gradient.setColorAt(0, QColor(40, 40, 50));
    gradient.setColorAt(1, QColor(20, 20, 30));
    
    painter.setBrush(gradient);
    painter.setPen(QPen(QColor(60, 60, 70), 3));
    painter.drawEllipse(-100, -100, 200, 200);
}

void SpeedGauge::drawScale(QPainter& painter) {
    painter.save();
    
    // Draw scale marks
    for (int i = 0; i <= maxSpeed_; i += 20) {
        painter.save();
        
        // Calculate angle (240 degrees sweep, starting at 240)
        double angle = 240 - (i * 240.0 / maxSpeed_);
        painter.rotate(-angle);
        
        // Major tick
        if (i % 40 == 0) {
            painter.setPen(QPen(Qt::white, 2));
            painter.drawLine(75, 0, 90, 0);
            
            // Label
            painter.save();
            painter.translate(65, 0);
            painter.rotate(angle);
            painter.drawText(-15, -8, 30, 16, Qt::AlignCenter, 
                           QString::number(i));
            painter.restore();
        } else {
            painter.setPen(QPen(QColor(150, 150, 150), 1));
            painter.drawLine(80, 0, 90, 0);
        }
        
        painter.restore();
    }
    
    painter.restore();
}

void SpeedGauge::drawNeedle(QPainter& painter) {
    painter.save();
    
    // Calculate angle
    double angle = 240 - (speed_ * 240.0 / maxSpeed_);
    painter.rotate(-angle);
    
    // Needle
    QPainterPath needle;
    needle.moveTo(-5, 0);
    needle.lineTo(0, -70);
    needle.lineTo(5, 0);
    needle.lineTo(0, 15);
    needle.closeSubpath();
    
    painter.setPen(Qt::NoPen);
    
    // Red needle with gradient
    QLinearGradient needleGradient(0, -70, 0, 15);
    needleGradient.setColorAt(0, QColor(255, 50, 50));
    needleGradient.setColorAt(1, QColor(180, 30, 30));
    painter.setBrush(needleGradient);
    painter.drawPath(needle);
    
    // Center cap
    painter.setBrush(QColor(60, 60, 70));
    painter.drawEllipse(-10, -10, 20, 20);
    
    painter.restore();
}

void SpeedGauge::drawValue(QPainter& painter) {
    painter.setPen(Qt::white);
    
    // Speed value
    QFont font = painter.font();
    font.setPointSize(24);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(-50, 20, 100, 40, Qt::AlignCenter, 
                    QString::number(speed_));
    
    // Unit
    font.setPointSize(10);
    font.setBold(false);
    painter.setFont(font);
    painter.setPen(QColor(150, 150, 150));
    painter.drawText(-50, 50, 100, 20, Qt::AlignCenter, "km/h");
}
```

### 📝 widgets/batterywidget.h

```cpp
#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include <QWidget>
#include "../service/datatypes.h"

class BatteryWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit BatteryWidget(QWidget *parent = nullptr);
    
public slots:
    void setBatteryInfo(const dashboard::BatteryInfo& info);
    void setLevel(int level);
    
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int level_;
    float voltage_;
    int temperature_;
    bool charging_;
};

#endif // BATTERYWIDGET_H
```

### 📝 widgets/batterywidget.cpp

```cpp
#include "batterywidget.h"
#include <QPainter>
#include <QPainterPath>

BatteryWidget::BatteryWidget(QWidget *parent)
    : QWidget(parent)
    , level_(0)
    , voltage_(0)
    , temperature_(0)
    , charging_(false)
{
    setMinimumSize(100, 50);
}

void BatteryWidget::setBatteryInfo(const dashboard::BatteryInfo& info) {
    level_ = info.level;
    voltage_ = info.voltage;
    temperature_ = info.temperature;
    charging_ = info.charging;
    update();
}

void BatteryWidget::setLevel(int level) {
    level_ = qBound(0, level, 100);
    update();
}

void BatteryWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int w = width();
    int h = height();
    int margin = 5;
    
    // Battery outline
    QRect batteryRect(margin, margin, w - margin * 2 - 10, h - margin * 2);
    QRect tipRect(w - margin - 10, h / 2 - 5, 10, 10);
    
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(batteryRect, 5, 5);
    painter.drawRoundedRect(tipRect, 2, 2);
    
    // Fill based on level
    int fillWidth = (batteryRect.width() - 6) * level_ / 100;
    QRect fillRect(batteryRect.x() + 3, batteryRect.y() + 3, 
                   fillWidth, batteryRect.height() - 6);
    
    // Color based on level
    QColor fillColor;
    if (level_ < 20) {
        fillColor = QColor(255, 50, 50);
    } else if (level_ < 50) {
        fillColor = QColor(255, 180, 0);
    } else {
        fillColor = QColor(50, 200, 50);
    }
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(fillColor);
    painter.drawRoundedRect(fillRect, 3, 3);
    
    // Text
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);
    
    QString text = QString::number(level_) + "%";
    if (charging_) text += " ⚡";
    
    painter.drawText(batteryRect, Qt::AlignCenter, text);
}
```

### 📝 dashboard/dashboardwindow.h

```cpp
#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include "../service/dashboardproxy.h"
#include "../widgets/speedgauge.h"
#include "../widgets/batterywidget.h"

class DashboardWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow();

private slots:
    void onServiceAvailable();
    void onServiceUnavailable();
    void onSpeedReceived(int speed);
    void onRpmReceived(int rpm);
    void onBatteryReceived(dashboard::BatteryInfo battery);
    void onDriveModeReceived(dashboard::DriveMode mode);
    void onAllDataReceived(dashboard::DashboardData data);
    void onError(QString message);

private:
    void setupUI();
    void createStatusBar();
    void applyStyleSheet();
    
    dashboard::DashboardProxy* proxy_;
    
    // Widgets
    SpeedGauge* speedGauge_;
    BatteryWidget* batteryWidget_;
    QLabel* rpmLabel_;
    QLabel* gearLabel_;
    QLabel* modeLabel_;
    QLabel* statusLabel_;
    QLabel* connectionLabel_;
};

#endif // DASHBOARDWINDOW_H
```

### 📝 dashboard/dashboardwindow.cpp

```cpp
#include "dashboardwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStatusBar>
#include <QApplication>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createStatusBar();
    applyStyleSheet();
    
    // Create proxy and connect signals
    proxy_ = dashboard::DashboardProxy::FindService(this);
    
    // Use QueuedConnection for thread safety
    connect(proxy_, &dashboard::DashboardProxy::serviceAvailable,
            this, &DashboardWindow::onServiceAvailable, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::serviceUnavailable,
            this, &DashboardWindow::onServiceUnavailable, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::speedReceived,
            this, &DashboardWindow::onSpeedReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::rpmReceived,
            this, &DashboardWindow::onRpmReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::batteryReceived,
            this, &DashboardWindow::onBatteryReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::driveModeReceived,
            this, &DashboardWindow::onDriveModeReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::allDataReceived,
            this, &DashboardWindow::onAllDataReceived, Qt::QueuedConnection);
    connect(proxy_, &dashboard::DashboardProxy::errorOccurred,
            this, &DashboardWindow::onError, Qt::QueuedConnection);
}

DashboardWindow::~DashboardWindow() {
    proxy_->stop();
}

void DashboardWindow::setupUI() {
    setWindowTitle("Vehicle Dashboard");
    setMinimumSize(800, 500);
    
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    
    // Left panel - Speed
    QGroupBox* speedGroup = new QGroupBox("Speed");
    QVBoxLayout* speedLayout = new QVBoxLayout(speedGroup);
    speedGauge_ = new SpeedGauge();
    speedGauge_->setMinimumSize(250, 250);
    speedLayout->addWidget(speedGauge_);
    mainLayout->addWidget(speedGroup);
    
    // Center panel - Info
    QVBoxLayout* infoLayout = new QVBoxLayout();
    
    // RPM
    QGroupBox* rpmGroup = new QGroupBox("RPM");
    QVBoxLayout* rpmLayout = new QVBoxLayout(rpmGroup);
    rpmLabel_ = new QLabel("0");
    rpmLabel_->setAlignment(Qt::AlignCenter);
    rpmLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #00ff00;");
    rpmLayout->addWidget(rpmLabel_);
    infoLayout->addWidget(rpmGroup);
    
    // Gear
    QGroupBox* gearGroup = new QGroupBox("Gear");
    QVBoxLayout* gearLayout = new QVBoxLayout(gearGroup);
    gearLabel_ = new QLabel("P");
    gearLabel_->setAlignment(Qt::AlignCenter);
    gearLabel_->setStyleSheet("font-size: 48px; font-weight: bold; color: #ffffff;");
    gearLayout->addWidget(gearLabel_);
    infoLayout->addWidget(gearGroup);
    
    // Drive Mode
    QGroupBox* modeGroup = new QGroupBox("Drive Mode");
    QVBoxLayout* modeLayout = new QVBoxLayout(modeGroup);
    modeLabel_ = new QLabel("NORMAL");
    modeLabel_->setAlignment(Qt::AlignCenter);
    modeLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #00aaff;");
    modeLayout->addWidget(modeLabel_);
    infoLayout->addWidget(modeGroup);
    
    mainLayout->addLayout(infoLayout);
    
    // Right panel - Battery
    QGroupBox* batteryGroup = new QGroupBox("Battery");
    QVBoxLayout* batteryLayout = new QVBoxLayout(batteryGroup);
    batteryWidget_ = new BatteryWidget();
    batteryWidget_->setMinimumSize(150, 80);
    batteryLayout->addWidget(batteryWidget_);
    batteryLayout->addStretch();
    mainLayout->addWidget(batteryGroup);
    
    setCentralWidget(central);
}

void DashboardWindow::createStatusBar() {
    connectionLabel_ = new QLabel("Connecting...");
    connectionLabel_->setStyleSheet("color: orange;");
    statusBar()->addPermanentWidget(connectionLabel_);
    
    statusLabel_ = new QLabel("Dashboard v1.0");
    statusBar()->addWidget(statusLabel_);
}

void DashboardWindow::applyStyleSheet() {
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a1a2e;
        }
        QGroupBox {
            color: #ffffff;
            border: 1px solid #3a3a5e;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        QLabel {
            color: #ffffff;
        }
        QStatusBar {
            background-color: #0f0f1a;
            color: #888888;
        }
    )");
}

void DashboardWindow::onServiceAvailable() {
    connectionLabel_->setText("Connected ●");
    connectionLabel_->setStyleSheet("color: #00ff00;");
    statusLabel_->setText("Service connected");
}

void DashboardWindow::onServiceUnavailable() {
    connectionLabel_->setText("Disconnected ●");
    connectionLabel_->setStyleSheet("color: #ff0000;");
    statusLabel_->setText("Service disconnected");
    
    // Reset displays
    speedGauge_->setSpeed(0);
    rpmLabel_->setText("---");
    batteryWidget_->setLevel(0);
}

void DashboardWindow::onSpeedReceived(int speed) {
    speedGauge_->setSpeed(speed);
}

void DashboardWindow::onRpmReceived(int rpm) {
    rpmLabel_->setText(QString::number(rpm));
}

void DashboardWindow::onBatteryReceived(dashboard::BatteryInfo battery) {
    batteryWidget_->setBatteryInfo(battery);
}

void DashboardWindow::onDriveModeReceived(dashboard::DriveMode mode) {
    modeLabel_->setText(dashboard::driveModeToString(mode));
    
    // Color based on mode
    switch (mode) {
        case dashboard::DriveMode::ECO:
            modeLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #00ff00;");
            break;
        case dashboard::DriveMode::SPORT:
            modeLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #ff0000;");
            break;
        case dashboard::DriveMode::SNOW:
            modeLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #00ffff;");
            break;
        default:
            modeLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #00aaff;");
    }
}

void DashboardWindow::onAllDataReceived(dashboard::DashboardData data) {
    // All data received at once
    gearLabel_->setText(data.gear == 0 ? "P" : QString::number(data.gear));
    statusLabel_->setText(QString("Last update: %1").arg(
        QDateTime::fromMSecsSinceEpoch(data.timestamp).toString("hh:mm:ss")));
}

void DashboardWindow::onError(QString message) {
    statusLabel_->setText("Error: " + message);
}
```

### 📝 main.cpp

```cpp
#include <QApplication>
#include "dashboard/dashboardwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("Vehicle Dashboard");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("AUTOSAR Adaptive Course");
    
    DashboardWindow window;
    window.show();
    
    return app.exec();
}
```

### 📝 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(qt_dashboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

# Sources
set(SOURCES
    main.cpp
    dashboard/dashboardwindow.cpp
    widgets/speedgauge.cpp
    widgets/batterywidget.cpp
    service/dashboardproxy.cpp
)

set(HEADERS
    dashboard/dashboardwindow.h
    widgets/speedgauge.h
    widgets/batterywidget.h
    service/dashboardproxy.h
    service/datatypes.h
)

# Create executable
add_executable(qt_dashboard ${SOURCES} ${HEADERS})

target_include_directories(qt_dashboard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(qt_dashboard
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)

# Copy vsomeip config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../lesson2/example2/vsomeip.json
    ${CMAKE_BINARY_DIR}/vsomeip.json
    COPYONLY
)
```

---

## 📊 Tổng Kết Bài 3

### Điểm chính:

| Concept | Qt Implementation |
|---------|-------------------|
| `ara::com::FindService()` | `DashboardProxy::FindService()` |
| `ara::com::Event` | Qt Signal |
| `ara::com::Method` | Async request + signal response |
| `ara::core::Future::Then()` | `QFutureWatcher` + signal |
| Thread safety | `Qt::QueuedConnection` |

### Pattern quan trọng:

```cpp
// 1. vsomeip chạy trong thread riêng
QThread::create([this]() { app_->start(); })->start();

// 2. Emit signals từ vsomeip callbacks
void onMessage(...) {
    emit speedReceived(speed);  // Thread-safe via Qt
}

// 3. Connect với QueuedConnection
connect(proxy, &Proxy::speedReceived,
        this, &Window::updateSpeed,
        Qt::QueuedConnection);  // BẮT BUỘC!
```

---

**[← Bài 2](../lesson2/README.md) | [Bài 4: Demo & Packaging →](../lesson4/README.md)**
