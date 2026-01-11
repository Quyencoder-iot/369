# Bài 1: Tổng quan AUTOSAR Adaptive và Giới thiệu vsomeip

## 📚 Mục tiêu bài học
- Hiểu kiến trúc SOA (Service-Oriented Architecture) trong AUTOSAR Adaptive
- Nắm vững cấu trúc `ara::` namespace và các cluster
- Cài đặt và cấu hình vsomeip
- Viết ứng dụng hello_service và hello_client đầu tiên

---

## 1. Tổng quan Kiến trúc AUTOSAR Adaptive

### 1.1 Kiến trúc SOA (Service-Oriented Architecture)

```
┌─────────────────────────────────────────────────────────────────┐
│                    AUTOSAR Adaptive Platform                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │   App 1      │  │   App 2      │  │   App 3      │          │
│  │  (Service)   │  │  (Client)    │  │  (Client)    │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
│         │                 │                 │                   │
│  ═══════╪═════════════════╪═════════════════╪═══════════════   │
│         │        ara::com (SOME/IP)         │                   │
│  ═══════╪═════════════════╪═════════════════╪═══════════════   │
│         │                 │                 │                   │
│  ┌──────┴─────────────────┴─────────────────┴──────┐           │
│  │              Service Discovery (SD)              │           │
│  └──────────────────────────────────────────────────┘           │
├─────────────────────────────────────────────────────────────────┤
│  ara::core  │  ara::log  │  ara::diag  │  ara::exec            │
└─────────────────────────────────────────────────────────────────┘
```

**Đặc điểm chính của SOA:**
- **Service Provider**: Cung cấp dịch vụ (methods, events, fields)
- **Service Consumer**: Sử dụng dịch vụ thông qua proxy
- **Service Discovery**: Tự động tìm kiếm và kết nối services
- **Loose Coupling**: Các thành phần độc lập, dễ thay thế

### 1.2 Cấu trúc ara:: namespace

```cpp
ara::                           // Root namespace
├── com::                       // Communication (SOME/IP)
│   ├── ServiceProxy            // Client-side proxy
│   ├── ServiceSkeleton         // Server-side skeleton
│   ├── FindService()           // Service discovery
│   ├── Event<T>               // Event notification
│   ├── Field<T>               // Data field (get/set)
│   └── Method<T>              // Remote method call
│
├── core::                      // Core utilities
│   ├── Future<T>              // Async result
│   ├── Promise<T>             // Async provider
│   ├── Result<T,E>            // Error handling
│   └── ErrorCode              // Error codes
│
├── log::                       // Logging
│   ├── Logger                 // Logger instance
│   └── LogLevel               // Debug/Info/Warn/Error
│
├── diag::                      // Diagnostics
│   ├── DiagnosticEvent        // DTC events
│   └── Monitor                // Monitoring
│
└── exec::                      // Execution management
    ├── StateClient            // State management
    └── DeterministicClient    // Deterministic execution
```

### 1.3 Application Manifest và Execution Manifest

**ApplicationManifest.json** - Định nghĩa ứng dụng:
```json
{
  "application": {
    "name": "HelloService",
    "version": "1.0.0",
    "provided_services": [
      {
        "service_id": "0x1234",
        "instance_id": "0x5678"
      }
    ]
  }
}
```

**ExecutionManifest.json** - Định nghĩa cách chạy:
```json
{
  "execution": {
    "process_name": "hello_service_process",
    "startup_config": {
      "function_group": "MachineFG",
      "mode": "Running"
    },
    "resource_limits": {
      "max_memory": "64MB",
      "cpu_time": "100ms"
    }
  }
}
```

---

## 2. Cài đặt vsomeip

### 2.1 Yêu cầu hệ thống

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    git
```

### 2.2 Biên dịch vsomeip từ source

```bash
# Clone vsomeip
git clone https://github.com/COVESA/vsomeip.git
cd vsomeip

# Tạo thư mục build
mkdir build && cd build

# Configure với CMake
cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DENABLE_SIGNAL_HANDLING=1 \
    -DDIAGNOSIS_ADDRESS=0x10

# Biên dịch
make -j$(nproc)

# Cài đặt
sudo make install
sudo ldconfig
```

### 2.3 Cấu trúc file vsomeip.json

```json
{
    "unicast": "127.0.0.1",
    "netmask": "255.255.255.0",
    "logging": {
        "level": "debug",
        "console": "true",
        "file": { "enable": "false" }
    },
    "applications": [
        {
            "name": "hello_service",
            "id": "0x1111"
        },
        {
            "name": "hello_client", 
            "id": "0x2222"
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
        "multicast": "224.244.224.245",
        "port": "30490",
        "protocol": "udp"
    }
}
```

---

## 3. Ví dụ Code

### 📝 Ví dụ 1: Code Cơ bản (Minimal)

Xem thư mục: `examples/basic/`

**Đặc điểm:**
- Code ngắn gọn nhất có thể
- Chỉ bao gồm các yếu tố bắt buộc
- Dễ hiểu cho người mới bắt đầu

### 📝 Ví dụ 2: Code Nâng cao (Complete)

Xem thư mục: `examples/advanced/`

**Đặc điểm:**
- Code đầy đủ với error handling
- Có logging và diagnostics
- Modular design
- Production-ready

---

## 4. Khái niệm quan trọng

### Service ID, Instance ID, EventGroup

| Thành phần | Mô tả | Ví dụ |
|-----------|-------|-------|
| **Service ID** | Định danh loại dịch vụ | 0x1234 (Dashboard Service) |
| **Instance ID** | Định danh instance cụ thể | 0x5678 (Instance 1) |
| **Method ID** | ID của remote method | 0x0001 (sayHello) |
| **Event ID** | ID của event | 0x8001 (speed update) |
| **EventGroup ID** | Nhóm các events liên quan | 0x0001 (vehicle events) |

### Luồng giao tiếp Service-Client

```
┌─────────────┐                      ┌─────────────┐
│   Client    │                      │   Service   │
└──────┬──────┘                      └──────┬──────┘
       │                                    │
       │  1. FindService(0x1234, 0x5678)   │
       │ ─────────────────────────────────>│
       │                                    │
       │  2. OfferService(0x1234, 0x5678)  │
       │ <─────────────────────────────────│
       │                                    │
       │  3. Subscribe(EventGroup 0x0001)  │
       │ ─────────────────────────────────>│
       │                                    │
       │  4. SubscribeAck                  │
       │ <─────────────────────────────────│
       │                                    │
       │  5. Event Notification (data)     │
       │ <─────────────────────────────────│
       │                                    │
       │  6. Request Method (sayHello)     │
       │ ─────────────────────────────────>│
       │                                    │
       │  7. Response (result)             │
       │ <─────────────────────────────────│
       │                                    │
```

---

## 5. Bài tập thực hành

1. **Cài đặt vsomeip** trên máy của bạn
2. **Chạy ví dụ basic**: Compile và chạy hello_service + hello_client
3. **Chạy ví dụ advanced**: Quan sát logs và các tính năng nâng cao
4. **Thay đổi Service ID**: Sửa ID và observe service discovery
5. **Thêm Event mới**: Mở rộng service với event tùy chỉnh

---

## 6. Tài liệu tham khảo

- [AUTOSAR Adaptive Platform](https://www.autosar.org/standards/adaptive-platform/)
- [vsomeip GitHub](https://github.com/COVESA/vsomeip)
- [SOME/IP Protocol Specification](https://www.autosar.org/fileadmin/standards/R22-11/FO/AUTOSAR_PRS_SOMEIPProtocol.pdf)
