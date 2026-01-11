# 📘 BÀI 1: Tổng Quan AUTOSAR Adaptive và Giới Thiệu vsomeip

## 📑 Mục Lục

1. [Kiến trúc AUTOSAR Adaptive](#1-kiến-trúc-autosar-adaptive)
2. [Kiến trúc SOA](#2-kiến-trúc-soa)
3. [Cấu trúc ara:: namespace](#3-cấu-trúc-ara-namespace)
4. [Khái niệm Manifest](#4-khái-niệm-manifest)
5. [Cài đặt vsomeip](#5-cài-đặt-vsomeip)
6. [Ví dụ 1: Hello Service/Client (Đơn giản)](#6-ví-dụ-1-hello-serviceclient-đơn-giản)
7. [Ví dụ 2: Advanced Service/Client (Nâng cao)](#7-ví-dụ-2-advanced-serviceclient-nâng-cao)

---

## 1. Kiến Trúc AUTOSAR Adaptive

### 1.1 Tổng quan

**AUTOSAR Adaptive Platform** là nền tảng phần mềm tiêu chuẩn cho các ứng dụng ô tô hiện đại, được thiết kế để hỗ trợ:

- 🚗 **Autonomous Driving** (Lái tự động)
- 📡 **V2X Communication** (Giao tiếp Vehicle-to-Everything)
- 🔄 **Over-the-Air Updates** (Cập nhật phần mềm từ xa)
- ☁️ **Cloud Services** (Dịch vụ đám mây)

### 1.2 So sánh Classic vs Adaptive

| Đặc điểm | Classic Platform | Adaptive Platform |
|----------|------------------|-------------------|
| OS | OSEK/VDX | POSIX (Linux, QNX) |
| Ngôn ngữ | C | C++14/17 |
| Kiến trúc | Signal-based | Service-Oriented |
| Cập nhật | Compile-time | Runtime |
| Bộ nhớ | Static | Dynamic |

### 1.3 Các thành phần chính

```
┌─────────────────────────────────────────────────────────────────┐
│                     ADAPTIVE APPLICATIONS                        │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐   │
│  │            ARA (AUTOSAR Runtime for Adaptive)            │   │
│  ├────────┬────────┬────────┬────────┬────────┬────────────┤   │
│  │ara::com│ara::log│ara::diag│ara::per│ara::exec│ara::core │   │
│  └────────┴────────┴────────┴────────┴────────┴────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Execution Management (EM)                    │   │
│  │              State Management (SM)                        │   │
│  │              Communication Management (CM)                │   │
│  └──────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│                    Operating System (POSIX)                      │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Kiến Trúc SOA

### 2.1 Service-Oriented Architecture

SOA là mô hình kiến trúc trong đó các thành phần giao tiếp thông qua **services**.

```
┌─────────────┐         ┌─────────────┐         ┌─────────────┐
│   Service   │ ──────► │  SOME/IP    │ ◄────── │   Client    │
│  Provider   │         │  Middleware │         │ (Consumer)  │
└─────────────┘         └─────────────┘         └─────────────┘
      │                        │                       │
      ▼                        ▼                       ▼
  Offer Service          Route Messages         Find Service
  Send Events            Service Discovery      Subscribe Events
  Handle Methods         Publish/Subscribe      Call Methods
```

### 2.2 Các thành phần SOA

| Thành phần | Mô tả |
|------------|-------|
| **Service** | Đơn vị cung cấp chức năng |
| **Service Provider** | Ứng dụng cung cấp service |
| **Service Consumer** | Ứng dụng sử dụng service |
| **Service Discovery** | Cơ chế tìm kiếm service |
| **Event** | Dữ liệu được phát (publish) |
| **Method** | Hàm được gọi từ xa (RPC) |
| **Field** | Dữ liệu với getter/setter |

### 2.3 SOME/IP Protocol

**SOME/IP** (Scalable service-Oriented MiddlewarE over IP) là giao thức truyền thông:

```
┌─────────────────────────────────────────────────┐
│              SOME/IP Message Format              │
├───────────────┬───────────────┬─────────────────┤
│  Message ID   │    Length     │   Request ID    │
│   (32 bit)    │   (32 bit)    │    (32 bit)     │
├───────────────┼───────────────┼─────────────────┤
│ Protocol Ver  │ Interface Ver │   Message Type  │
│   (8 bit)     │    (8 bit)    │     (8 bit)     │
├───────────────┴───────────────┴─────────────────┤
│                    Payload                       │
└─────────────────────────────────────────────────┘
```

**Message ID** = Service ID (16 bit) + Method/Event ID (16 bit)

---

## 3. Cấu Trúc ara:: Namespace

### 3.1 Các Cluster chính

```cpp
namespace ara {
    namespace com {    // Communication Management
        // ServiceProxy, ServiceSkeleton
        // Event, Method, Field
        // FindService, OfferService
    }
    
    namespace core {   // Core Types & Utilities
        // Result, ErrorCode
        // Future, Promise
        // Vector, String, Array
    }
    
    namespace log {    // Logging
        // Logger, LogLevel
        // LogStream
    }
    
    namespace diag {   // Diagnostics
        // DiagnosticManager
        // DTCHandler
    }
    
    namespace exec {   // Execution Management
        // ExecutionClient
        // StateClient
    }
    
    namespace per {    // Persistency
        // KeyValueStorage
        // FileStorage
    }
}
```

### 3.2 Mối quan hệ Service ↔ Client

```
┌──────────────────────┐        ┌──────────────────────┐
│   SERVICE SKELETON   │        │    SERVICE PROXY     │
│   (ara::com)         │        │    (ara::com)        │
├──────────────────────┤        ├──────────────────────┤
│                      │        │                      │
│  OfferService()      │◄──────►│  FindService()       │
│                      │        │                      │
│  event.Send()        │───────►│  event.Subscribe()   │
│                      │        │  event.GetNewSamples│
│                      │        │                      │
│  method.Register()   │◄──────►│  method()            │
│                      │        │                      │
│  field.Update()      │◄──────►│  field.Get/Set()     │
│                      │        │                      │
└──────────────────────┘        └──────────────────────┘
```

---

## 4. Khái Niệm Manifest

### 4.1 Application Manifest

Mô tả ứng dụng và các service:

```json
{
    "application": {
        "name": "DashboardApp",
        "version": "1.0.0",
        "provided_services": [
            {
                "service_id": "0x1234",
                "instance_id": "0x5678",
                "interface": "dashboard.DashboardService"
            }
        ],
        "required_services": [
            {
                "service_id": "0x2345",
                "interface": "sensor.SensorService"
            }
        ]
    }
}
```

### 4.2 Execution Manifest

Mô tả cách thực thi ứng dụng:

```json
{
    "execution": {
        "process": {
            "name": "DashboardProcess",
            "executable": "/opt/app/dashboard",
            "startup_config": {
                "state_dependent": true,
                "function_group": "MachineFG"
            }
        },
        "resource_requirements": {
            "cpu_limit": "50%",
            "memory_limit": "256MB"
        }
    }
}
```

---

## 5. Cài Đặt vsomeip

### 5.1 Tải và biên dịch từ source

```bash
#!/bin/bash
# build_vsomeip.sh

# Cài đặt dependencies
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-log-dev \
    libboost-filesystem-dev

# Clone vsomeip
git clone https://github.com/COVESA/vsomeip.git
cd vsomeip

# Tạo thư mục build
mkdir build && cd build

# Configure với CMake
cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DENABLE_SIGNAL_HANDLING=1 \
    -DDIAGNOSIS_ADDRESS=0x01

# Build
make -j$(nproc)

# Install
sudo make install
sudo ldconfig
```

### 5.2 Cấu hình vsomeip.json

```json
{
    "unicast": "127.0.0.1",
    "logging": {
        "level": "info",
        "console": "true",
        "file": { "enable": "false" }
    },
    "applications": [
        {
            "name": "hello_service",
            "id": "0x1001"
        },
        {
            "name": "hello_client",
            "id": "0x1002"
        }
    ],
    "services": [
        {
            "service": "0x1234",
            "instance": "0x5678",
            "unreliable": "30509"
        }
    ],
    "routing": "hello_service",
    "service-discovery": {
        "enable": "true",
        "multicast": "239.224.224.245",
        "port": "30490",
        "protocol": "udp"
    }
}
```

---

## 6. Ví Dụ 1: Hello Service/Client (Đơn giản)

### 📁 Cấu trúc file

```
example1/
├── hello_service.cpp    # Service provider
├── hello_client.cpp     # Service consumer
├── vsomeip.json         # Cấu hình
└── CMakeLists.txt       # Build config
```

### 🔧 Các yếu tố BẮT BUỘC

1. **Include vsomeip headers**
2. **Tạo application instance**
3. **Khởi tạo (init)**
4. **Đăng ký handler**
5. **Start/Stop application**

---

### 📝 hello_service.cpp (Minimal)

```cpp
// ============================================
// VÍ DỤ 1: HELLO SERVICE - CODE TỐI GIẢN
// Các yếu tố BẮT BUỘC:
// 1. Include vsomeip
// 2. Tạo application
// 3. Init, Register handlers, Start
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>

// ===== ĐỊNH NGHĨA ID (BẮT BUỘC) =====
#define SERVICE_ID      0x1234
#define INSTANCE_ID     0x5678
#define METHOD_ID       0x0001
#define EVENTGROUP_ID   0x0001

// ===== BIẾN GLOBAL =====
std::shared_ptr<vsomeip::application> app;

// ===== MESSAGE HANDLER (BẮT BUỘC) =====
void on_message(const std::shared_ptr<vsomeip::message>& request) {
    // Tạo response
    auto response = vsomeip::runtime::get()->create_response(request);
    
    // Payload đơn giản
    auto payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data = {'H', 'E', 'L', 'L', 'O'};
    payload->set_data(data);
    response->set_payload(payload);
    
    // Gửi response
    app->send(response);
    std::cout << "Sent response: HELLO" << std::endl;
}

int main() {
    // 1. TẠO APPLICATION (BẮT BUỘC)
    app = vsomeip::runtime::get()->create_application("hello_service");
    
    // 2. KHỞI TẠO (BẮT BUỘC)
    app->init();
    
    // 3. ĐĂNG KÝ MESSAGE HANDLER (BẮT BUỘC)
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_ID, on_message);
    
    // 4. OFFER SERVICE (BẮT BUỘC)
    app->offer_service(SERVICE_ID, INSTANCE_ID);
    
    // 5. START (BẮT BUỘC - blocking call)
    std::cout << "Service started..." << std::endl;
    app->start();
    
    return 0;
}
```

---

### 📝 hello_client.cpp (Minimal)

```cpp
// ============================================
// VÍ DỤ 1: HELLO CLIENT - CODE TỐI GIẢN
// Các yếu tố BẮT BUỘC:
// 1. Include vsomeip
// 2. Tạo application
// 3. Init, Register handlers, Start
// ============================================

#include <vsomeip/vsomeip.hpp>
#include <iostream>

// ===== ĐỊNH NGHĨA ID (PHẢI KHỚP VỚI SERVICE) =====
#define SERVICE_ID      0x1234
#define INSTANCE_ID     0x5678
#define METHOD_ID       0x0001

// ===== BIẾN GLOBAL =====
std::shared_ptr<vsomeip::application> app;

// ===== AVAILABILITY HANDLER (BẮT BUỘC) =====
void on_availability(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
    if (available) {
        std::cout << "Service available!" << std::endl;
        
        // Tạo và gửi request
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_ID);
        
        app->send(request);
        std::cout << "Request sent" << std::endl;
    }
}

// ===== RESPONSE HANDLER (BẮT BUỘC) =====
void on_message(const std::shared_ptr<vsomeip::message>& response) {
    auto payload = response->get_payload();
    std::string data(payload->get_data(), payload->get_data() + payload->get_length());
    std::cout << "Received: " << data << std::endl;
}

int main() {
    // 1. TẠO APPLICATION (BẮT BUỘC)
    app = vsomeip::runtime::get()->create_application("hello_client");
    
    // 2. KHỞI TẠO (BẮT BUỘC)
    app->init();
    
    // 3. ĐĂNG KÝ HANDLERS (BẮT BUỘC)
    app->register_availability_handler(SERVICE_ID, INSTANCE_ID, on_availability);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_ID, on_message);
    
    // 4. REQUEST SERVICE (BẮT BUỘC)
    app->request_service(SERVICE_ID, INSTANCE_ID);
    
    // 5. START (BẮT BUỘC)
    std::cout << "Client started..." << std::endl;
    app->start();
    
    return 0;
}
```

---

### 📝 vsomeip.json (Minimal)

```json
{
    "unicast": "127.0.0.1",
    "applications": [
        { "name": "hello_service", "id": "0x1001" },
        { "name": "hello_client", "id": "0x1002" }
    ],
    "services": [
        {
            "service": "0x1234",
            "instance": "0x5678",
            "unreliable": "30509"
        }
    ],
    "routing": "hello_service"
}
```

---

### 📝 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(hello_someip)

set(CMAKE_CXX_STANDARD 14)

find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

# Hello Service
add_executable(hello_service hello_service.cpp)
target_link_libraries(hello_service vsomeip3 ${Boost_LIBRARIES} pthread)

# Hello Client
add_executable(hello_client hello_client.cpp)
target_link_libraries(hello_client vsomeip3 ${Boost_LIBRARIES} pthread)
```

---

### ▶️ Cách chạy

```bash
# Terminal 1: Chạy service
export VSOMEIP_CONFIGURATION=vsomeip.json
./hello_service

# Terminal 2: Chạy client
export VSOMEIP_CONFIGURATION=vsomeip.json
./hello_client
```

---

## 7. Ví Dụ 2: Advanced Service/Client (Nâng cao)

### 📁 Cấu trúc file

```
example2/
├── advanced_service.cpp     # Service với Events
├── advanced_client.cpp      # Client với Subscribe
├── vsomeip_advanced.json    # Cấu hình đầy đủ
└── CMakeLists.txt
```

---

### 📝 advanced_service.cpp (Đầy đủ)

```cpp
// ============================================
// VÍ DỤ 2: ADVANCED SERVICE - CODE ĐẦY ĐỦ
// 
// Module 1: Includes & Definitions
// Module 2: Application Class
// Module 3: State Management
// Module 4: Event Publishing
// Module 5: Request Handling
// Module 6: Main Entry Point
// ============================================

// =============================================
// MODULE 1: INCLUDES & DEFINITIONS
// ---------------------------------------------
// Mục đích: Khai báo thư viện và hằng số
// =============================================
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>

// Service identifiers
static constexpr vsomeip::service_t SERVICE_ID      = 0x1234;
static constexpr vsomeip::instance_t INSTANCE_ID    = 0x5678;

// Method identifiers
static constexpr vsomeip::method_t METHOD_GET_TIME  = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_DATA  = 0x0002;

// Event identifiers
static constexpr vsomeip::event_t EVENT_NOTIFY      = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;


// =============================================
// MODULE 2: APPLICATION CLASS
// ---------------------------------------------
// Mục đích: Đóng gói logic service
// Giải thích:
// - Class quản lý lifecycle của service
// - Thread-safe với mutex
// - Hỗ trợ graceful shutdown
// =============================================
class AdvancedService {
public:
    AdvancedService() 
        : running_(false)
        , counter_(0)
    {
        // Tạo application với tên duy nhất
        app_ = vsomeip::runtime::get()->create_application("advanced_service");
    }
    
    ~AdvancedService() {
        stop();
    }
    
    // =============================================
    // MODULE 3: STATE MANAGEMENT
    // ---------------------------------------------
    // Mục đích: Khởi tạo và quản lý trạng thái
    // Giải thích:
    // - init(): Đăng ký tất cả handlers
    // - start(): Bắt đầu service
    // - stop(): Dừng service an toàn
    // =============================================
    
    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return false;
        }
        
        // Đăng ký state handler - được gọi khi app thay đổi trạng thái
        app_->register_state_handler(
            std::bind(&AdvancedService::on_state, this, std::placeholders::_1)
        );
        
        // Đăng ký message handlers cho các methods
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_TIME,
            std::bind(&AdvancedService::on_get_time, this, std::placeholders::_1)
        );
        
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_GET_DATA,
            std::bind(&AdvancedService::on_get_data, this, std::placeholders::_1)
        );
        
        // Cấu hình event
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(EVENTGROUP_ID);
        
        app_->offer_event(
            SERVICE_ID,
            INSTANCE_ID,
            EVENT_NOTIFY,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD,  // Field type (có initial value)
            std::chrono::milliseconds::zero(), // Không có cycle time
            false,  // Không change_resets_cycle
            true,   // update_on_change
            nullptr,
            vsomeip::reliability_type_e::RT_UNRELIABLE
        );
        
        std::cout << "[INIT] Service initialized successfully" << std::endl;
        return true;
    }
    
    void start() {
        running_ = true;
        
        // Khởi động thread publish events
        publish_thread_ = std::thread(&AdvancedService::publish_loop, this);
        
        // Start application (blocking)
        app_->start();
    }
    
    void stop() {
        running_ = false;
        
        // Đợi publish thread kết thúc
        if (publish_thread_.joinable()) {
            publish_thread_.join();
        }
        
        // Stop offering service
        app_->stop_offer_service(SERVICE_ID, INSTANCE_ID);
        
        // Clear handlers
        app_->clear_all_handler();
        
        // Stop application
        app_->stop();
        
        std::cout << "[STOP] Service stopped" << std::endl;
    }

private:
    // State handler - được gọi khi trạng thái app thay đổi
    void on_state(vsomeip::state_type_e state) {
        if (state == vsomeip::state_type_e::ST_REGISTERED) {
            // Application đã đăng ký với routing manager
            // Bây giờ có thể offer service
            app_->offer_service(SERVICE_ID, INSTANCE_ID);
            std::cout << "[STATE] Service offered" << std::endl;
        }
    }
    
    // =============================================
    // MODULE 4: EVENT PUBLISHING
    // ---------------------------------------------
    // Mục đích: Phát events định kỳ
    // Giải thích:
    // - Chạy trong thread riêng
    // - Gửi counter mỗi giây
    // - Sử dụng notify() để publish
    // =============================================
    
    void publish_loop() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            if (!running_) break;
            
            // Tạo payload với counter
            auto payload = vsomeip::runtime::get()->create_payload();
            
            std::stringstream ss;
            ss << "Event #" << ++counter_ << " at " << get_timestamp();
            std::string data = ss.str();
            
            std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
            payload->set_data(payload_data);
            
            // Publish event
            app_->notify(SERVICE_ID, INSTANCE_ID, EVENT_NOTIFY, payload);
            
            std::cout << "[EVENT] Published: " << data << std::endl;
        }
    }
    
    // =============================================
    // MODULE 5: REQUEST HANDLING
    // ---------------------------------------------
    // Mục đích: Xử lý các request từ client
    // Giải thích:
    // - on_get_time: Trả về thời gian hiện tại
    // - on_get_data: Trả về dữ liệu mẫu
    // - Sử dụng create_response() để tạo reply
    // =============================================
    
    void on_get_time(const std::shared_ptr<vsomeip::message>& request) {
        std::cout << "[METHOD] GET_TIME called" << std::endl;
        
        // Tạo response
        auto response = vsomeip::runtime::get()->create_response(request);
        
        // Payload: current time
        auto payload = vsomeip::runtime::get()->create_payload();
        std::string time_str = get_timestamp();
        std::vector<vsomeip::byte_t> data(time_str.begin(), time_str.end());
        payload->set_data(data);
        
        response->set_payload(payload);
        app_->send(response);
    }
    
    void on_get_data(const std::shared_ptr<vsomeip::message>& request) {
        std::cout << "[METHOD] GET_DATA called" << std::endl;
        
        // Lấy request payload (nếu có)
        auto req_payload = request->get_payload();
        if (req_payload && req_payload->get_length() > 0) {
            std::string req_data(
                req_payload->get_data(), 
                req_payload->get_data() + req_payload->get_length()
            );
            std::cout << "[METHOD] Request data: " << req_data << std::endl;
        }
        
        // Tạo response với dữ liệu mẫu
        auto response = vsomeip::runtime::get()->create_response(request);
        auto payload = vsomeip::runtime::get()->create_payload();
        
        // Dữ liệu mẫu: speed, temperature, battery
        std::string data = "{\"speed\":120,\"temp\":25.5,\"battery\":85}";
        std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
        payload->set_data(payload_data);
        
        response->set_payload(payload);
        app_->send(response);
    }
    
    // Helper: Lấy timestamp hiện tại
    std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread publish_thread_;
    std::atomic<bool> running_;
    std::atomic<uint32_t> counter_;
};


// =============================================
// MODULE 6: MAIN ENTRY POINT
// ---------------------------------------------
// Mục đích: Điểm khởi đầu chương trình
// Giải thích:
// - Xử lý signal để shutdown gracefully
// - Chạy service cho đến khi nhận SIGINT
// =============================================
#include <csignal>

AdvancedService* g_service = nullptr;

void signal_handler(int signal) {
    std::cout << "\n[SIGNAL] Caught signal " << signal << ", shutting down..." << std::endl;
    if (g_service) {
        g_service->stop();
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   ADVANCED SOMEIP SERVICE" << std::endl;
    std::cout << "   Service ID: 0x" << std::hex << SERVICE_ID << std::endl;
    std::cout << "   Instance ID: 0x" << INSTANCE_ID << std::dec << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Đăng ký signal handler
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // Tạo và chạy service
    AdvancedService service;
    g_service = &service;
    
    if (service.init()) {
        service.start();  // Blocking call
    }
    
    return 0;
}
```

---

### 📝 advanced_client.cpp (Đầy đủ)

```cpp
// ============================================
// VÍ DỤ 2: ADVANCED CLIENT - CODE ĐẦY ĐỦ
// 
// Module 1: Includes & Definitions
// Module 2: Application Class
// Module 3: Service Discovery
// Module 4: Event Subscription
// Module 5: Method Invocation
// Module 6: Main Entry Point
// ============================================

// =============================================
// MODULE 1: INCLUDES & DEFINITIONS
// ---------------------------------------------
// Mục đích: Khai báo thư viện và hằng số
// =============================================
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>

// Service identifiers - PHẢI KHỚP VỚI SERVICE
static constexpr vsomeip::service_t SERVICE_ID      = 0x1234;
static constexpr vsomeip::instance_t INSTANCE_ID    = 0x5678;
static constexpr vsomeip::method_t METHOD_GET_TIME  = 0x0001;
static constexpr vsomeip::method_t METHOD_GET_DATA  = 0x0002;
static constexpr vsomeip::event_t EVENT_NOTIFY      = 0x8001;
static constexpr vsomeip::eventgroup_t EVENTGROUP_ID = 0x0001;


// =============================================
// MODULE 2: APPLICATION CLASS
// ---------------------------------------------
// Mục đích: Đóng gói logic client
// Giải thích:
// - Quản lý kết nối tới service
// - Xử lý reconnect khi service unavailable
// =============================================
class AdvancedClient {
public:
    AdvancedClient()
        : service_available_(false)
        , running_(false)
    {
        app_ = vsomeip::runtime::get()->create_application("advanced_client");
    }
    
    ~AdvancedClient() {
        stop();
    }
    
    // =============================================
    // MODULE 3: SERVICE DISCOVERY
    // ---------------------------------------------
    // Mục đích: Tìm và kết nối tới service
    // Giải thích:
    // - request_service(): Yêu cầu tìm service
    // - availability_handler: Callback khi service có/mất
    // - Tự động reconnect khi service restart
    // =============================================
    
    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize client" << std::endl;
            return false;
        }
        
        // State handler
        app_->register_state_handler(
            std::bind(&AdvancedClient::on_state, this, std::placeholders::_1)
        );
        
        // Availability handler - được gọi khi service available/unavailable
        app_->register_availability_handler(
            SERVICE_ID, INSTANCE_ID,
            std::bind(&AdvancedClient::on_availability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        
        // Message handler cho responses
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&AdvancedClient::on_message, this, std::placeholders::_1)
        );
        
        // Request service
        app_->request_service(SERVICE_ID, INSTANCE_ID);
        
        std::cout << "[INIT] Client initialized" << std::endl;
        return true;
    }
    
    void start() {
        running_ = true;
        
        // Thread gọi methods định kỳ
        request_thread_ = std::thread(&AdvancedClient::request_loop, this);
        
        // Start application
        app_->start();
    }
    
    void stop() {
        running_ = false;
        
        // Unsubscribe events
        app_->unsubscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        
        // Release service
        app_->release_service(SERVICE_ID, INSTANCE_ID);
        
        if (request_thread_.joinable()) {
            request_thread_.join();
        }
        
        app_->clear_all_handler();
        app_->stop();
        
        std::cout << "[STOP] Client stopped" << std::endl;
    }

private:
    void on_state(vsomeip::state_type_e state) {
        std::cout << "[STATE] " 
            << (state == vsomeip::state_type_e::ST_REGISTERED ? "Registered" : "Deregistered")
            << std::endl;
    }
    
    void on_availability(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
        std::cout << "[AVAIL] Service 0x" << std::hex << service 
            << " instance 0x" << instance << std::dec
            << " is " << (available ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
        
        service_available_ = available;
        
        if (available) {
            // Subscribe to events khi service available
            subscribe_events();
        }
    }
    
    // =============================================
    // MODULE 4: EVENT SUBSCRIPTION
    // ---------------------------------------------
    // Mục đích: Đăng ký nhận events từ service
    // Giải thích:
    // - request_event(): Yêu cầu event
    // - subscribe(): Đăng ký eventgroup
    // - Events được nhận qua on_message()
    // =============================================
    
    void subscribe_events() {
        // Request event trước khi subscribe
        std::set<vsomeip::eventgroup_t> eventgroups;
        eventgroups.insert(EVENTGROUP_ID);
        
        app_->request_event(
            SERVICE_ID,
            INSTANCE_ID,
            EVENT_NOTIFY,
            eventgroups,
            vsomeip::event_type_e::ET_FIELD
        );
        
        // Subscribe tới eventgroup
        app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID);
        
        std::cout << "[SUBSCRIBE] Subscribed to eventgroup 0x" 
            << std::hex << EVENTGROUP_ID << std::dec << std::endl;
    }
    
    // =============================================
    // MODULE 5: METHOD INVOCATION
    // ---------------------------------------------
    // Mục đích: Gọi remote methods
    // Giải thích:
    // - Tạo request message với service/method ID
    // - Gửi request với app_->send()
    // - Response được nhận qua on_message()
    // =============================================
    
    void request_loop() {
        int request_count = 0;
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            
            if (!running_ || !service_available_) continue;
            
            request_count++;
            
            // Xen kẽ gọi GET_TIME và GET_DATA
            if (request_count % 2 == 0) {
                call_get_time();
            } else {
                call_get_data();
            }
        }
    }
    
    void call_get_time() {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_GET_TIME);
        request->set_reliable(false);  // UDP
        
        app_->send(request);
        std::cout << "[REQUEST] Called GET_TIME" << std::endl;
    }
    
    void call_get_data() {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SERVICE_ID);
        request->set_instance(INSTANCE_ID);
        request->set_method(METHOD_GET_DATA);
        
        // Gửi payload với request
        auto payload = vsomeip::runtime::get()->create_payload();
        std::string data = "give_me_all_data";
        std::vector<vsomeip::byte_t> payload_data(data.begin(), data.end());
        payload->set_data(payload_data);
        request->set_payload(payload);
        
        app_->send(request);
        std::cout << "[REQUEST] Called GET_DATA" << std::endl;
    }
    
    // Message handler - nhận cả responses và events
    void on_message(const std::shared_ptr<vsomeip::message>& message) {
        auto payload = message->get_payload();
        std::string data;
        
        if (payload && payload->get_length() > 0) {
            data = std::string(
                payload->get_data(),
                payload->get_data() + payload->get_length()
            );
        }
        
        // Phân biệt event và response
        if (message->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
            std::cout << "[EVENT] Received notification: " << data << std::endl;
        } else {
            vsomeip::method_t method = message->get_method();
            std::cout << "[RESPONSE] Method 0x" << std::hex << method 
                << std::dec << ": " << data << std::endl;
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    std::thread request_thread_;
    std::atomic<bool> service_available_;
    std::atomic<bool> running_;
};


// =============================================
// MODULE 6: MAIN ENTRY POINT
// =============================================
#include <csignal>

AdvancedClient* g_client = nullptr;

void signal_handler(int signal) {
    std::cout << "\n[SIGNAL] Shutting down..." << std::endl;
    if (g_client) {
        g_client->stop();
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   ADVANCED SOMEIP CLIENT" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    AdvancedClient client;
    g_client = &client;
    
    if (client.init()) {
        client.start();
    }
    
    return 0;
}
```

---

### 📝 vsomeip_advanced.json (Đầy đủ)

```json
{
    "unicast": "127.0.0.1",
    "netmask": "255.255.255.0",
    "logging": {
        "level": "info",
        "console": "true",
        "file": {
            "enable": "true",
            "path": "/var/log/vsomeip.log"
        },
        "dlt": "false"
    },
    "applications": [
        {
            "name": "advanced_service",
            "id": "0x1001",
            "max_dispatchers": "10",
            "max_dispatch_time": "100"
        },
        {
            "name": "advanced_client",
            "id": "0x1002",
            "max_dispatchers": "10",
            "max_dispatch_time": "100"
        }
    ],
    "services": [
        {
            "service": "0x1234",
            "instance": "0x5678",
            "unreliable": "30509",
            "reliable": {
                "port": "30510",
                "enable-magic-cookies": "false"
            },
            "events": [
                {
                    "event": "0x8001",
                    "is_field": "true",
                    "is_reliable": "false"
                }
            ],
            "eventgroups": [
                {
                    "eventgroup": "0x0001",
                    "events": ["0x8001"],
                    "is_multicast": "false"
                }
            ]
        }
    ],
    "routing": "advanced_service",
    "service-discovery": {
        "enable": "true",
        "multicast": "239.224.224.245",
        "port": "30490",
        "protocol": "udp",
        "initial_delay_min": "10",
        "initial_delay_max": "50",
        "repetitions_base_delay": "30",
        "repetitions_max": "3",
        "ttl": "3",
        "cyclic_offer_delay": "1000",
        "request_response_delay": "1500"
    }
}
```

---

## 📊 Tổng Kết Bài 1

### Điểm chính cần nhớ:

| Khái niệm | Mô tả |
|-----------|-------|
| **AUTOSAR Adaptive** | Nền tảng SOA cho ô tô hiện đại |
| **ara:: namespace** | API tiêu chuẩn (com, core, log, diag...) |
| **SOME/IP** | Giao thức truyền thông service-oriented |
| **vsomeip** | Implementation mã nguồn mở của SOME/IP |
| **Service ID** | Định danh service (16-bit) |
| **Instance ID** | Phân biệt các instance cùng service |
| **Method** | RPC call từ client → service |
| **Event** | Notification từ service → client |

### Flow cơ bản:

```
1. Service: init() → register_handlers() → offer_service() → start()
2. Client:  init() → register_handlers() → request_service() → start()
3. Communication: Client sends request → Service processes → Service responds
4. Events: Service notify() → Client receives via message_handler
```

---

## 📝 Bài Tập

1. Thêm method mới `METHOD_ECHO` để service trả lại đúng dữ liệu client gửi
2. Thêm event mới gửi random number mỗi 500ms
3. Cấu hình vsomeip sử dụng TCP thay vì UDP

---

**[← Về Trang Chủ](../../README.md) | [Bài 2: Database ảo & Service Layer →](../lesson2/README.md)**
