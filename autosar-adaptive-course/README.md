# 🚗 Khóa học AUTOSAR Adaptive Platform với vsomeip

## Giới thiệu

Đây là khóa học ngắn gọn và đầy đủ về AUTOSAR Adaptive Platform, tập trung vào việc xây dựng hệ thống Dashboard xe với giao thức SOME/IP (vsomeip) và giao diện Qt.

### Mục tiêu khóa học

Sau khi hoàn thành, học viên có thể:
- ✅ Hiểu tổng quan kiến trúc AUTOSAR Adaptive
- ✅ Thiết kế dự án theo quy trình chuẩn (Use Case, Sequence, State Machine diagrams)
- ✅ Cài đặt và sử dụng SOME/IP (vsomeip)
- ✅ Tích hợp hệ thống và chạy trên PC

---

## 📚 Cấu trúc khóa học

```
autosar-adaptive-course/
├── README.md                      # File này
├── lesson1/                       # Bài 1: Tổng quan AUTOSAR & vsomeip
│   ├── README.md                  # Lý thuyết bài 1
│   └── examples/
│       ├── basic/                 # Ví dụ 1: Hello Service/Client cơ bản
│       └── advanced/              # Ví dụ 2: Service nâng cao với ara::
│
├── lesson2/                       # Bài 2: Database ảo & Service Layer
│   ├── README.md                  # Lý thuyết bài 2
│   └── examples/
│       ├── basic/                 # Ví dụ 1: Dashboard Service cơ bản
│       └── advanced/              # Ví dụ 2: Full ara:: API simulation
│
├── lesson3/                       # Bài 3: Tích hợp Qt UI
│   ├── README.md                  # Lý thuyết bài 3
│   └── examples/
│       ├── basic/                 # Ví dụ 1: Qt Console client
│       └── advanced/              # Ví dụ 2: Qt GUI Dashboard
│
└── lesson4/                       # Bài 4: Demo & Đóng gói
    ├── README.md                  # Lý thuyết bài 4
    └── examples/
        ├── basic/                 # Build script cơ bản
        └── advanced/              # Đóng gói hoàn chỉnh + Docker
```

---

## 📋 Nội dung chi tiết

### Bài 1: Tổng quan AUTOSAR Adaptive và vsomeip

| Chủ đề | Nội dung |
|--------|----------|
| Kiến trúc SOA | Service-Oriented Architecture trong automotive |
| ara:: namespace | com, core, log, diag và các cluster khác |
| Manifest files | ApplicationManifest, ExecutionManifest |
| vsomeip | Cài đặt, cấu hình vsomeip.json |
| **Ví dụ 1** | Hello Service/Client cơ bản |
| **Ví dụ 2** | Service nâng cao với logging, events, multiple methods |

### Bài 2: Mô phỏng Database ảo và Service Layer

| Chủ đề | Nội dung |
|--------|----------|
| JSON Database | Thiết kế dữ liệu xe: speed, battery, warnings, drive mode |
| ara::com | Truyền nhận signal qua SOME/IP |
| ara::core::Future | Xử lý callback bất đồng bộ |
| ara::log | Ghi log hệ thống |
| ara::diag | Phát hiện lỗi sensor |
| **Ví dụ 1** | Dashboard Service cơ bản với JSON |
| **Ví dụ 2** | Full DashboardService với ara:: simulation |

### Bài 3: Tích hợp Qt UI với Service Backend

| Chủ đề | Nội dung |
|--------|----------|
| ServiceBridge | Kết nối vsomeip với Qt signals/slots |
| Thread Safety | Xử lý Qt::QueuedConnection |
| ara::com::FindService | Service discovery |
| ara::core::Future::Then | Callback pattern cho UI |
| UI Mapping | QLabel, QProgressBar, QListWidget |
| **Ví dụ 1** | Qt Console client |
| **Ví dụ 2** | Full Qt GUI Dashboard với tabs |

### Bài 4: Trình diễn và Đóng gói

| Chủ đề | Nội dung |
|--------|----------|
| Thiết kế chuẩn | Use Case, Component, Sequence, State Machine diagrams |
| Build System | CMake project chuẩn |
| Packaging | CPack, TGZ, DEB/RPM |
| Docker | Container deployment |
| **Ví dụ 1** | Build script cơ bản |
| **Ví dụ 2** | Đóng gói hoàn chỉnh với Docker |

---

## 🛠 Yêu cầu hệ thống

### Phần cứng
- CPU: x86_64 hoặc ARM64
- RAM: 4GB minimum, 8GB recommended
- Disk: 5GB free space

### Phần mềm

```bash
# Ubuntu 20.04 / 22.04
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    qtbase5-dev \
    qttools5-dev \
    pkg-config
```

### vsomeip Installation

```bash
git clone https://github.com/COVESA/vsomeip.git
cd vsomeip
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
```

---

## 🚀 Quick Start

### 1. Clone và build

```bash
# Clone course
cd autosar-adaptive-course

# Build lesson 1 basic example
cd lesson1/examples/basic
mkdir build && cd build
cmake ..
make
```

### 2. Chạy demo

**Terminal 1 - Start Service:**
```bash
export VSOMEIP_CONFIGURATION=$(pwd)/../vsomeip_basic.json
./hello_service_basic
```

**Terminal 2 - Start Client:**
```bash
export VSOMEIP_CONFIGURATION=$(pwd)/../vsomeip_basic.json
./hello_client_basic
```

---

## 📖 Quy ước ví dụ

### Ví dụ 1: Code Cơ bản (Basic)
- Code ngắn gọn nhất có thể
- Chỉ bao gồm các yếu tố **BẮT BUỘC**
- Dễ hiểu cho người mới bắt đầu
- Comment giải thích ngắn gọn

### Ví dụ 2: Code Nâng cao (Advanced)
- Code đầy đủ, production-ready
- Tổ chức theo **Module Architecture**
- Mô phỏng đầy đủ ara:: namespace
- Error handling, logging, diagnostics
- Comment giải thích chi tiết từng module

---

## 📐 Kiến trúc hệ thống

```
┌─────────────────────────────────────────────────────────────────┐
│                     Qt Dashboard Client                          │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    MainWindow (UI)                       │    │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐    │    │
│  │  │  Speed   │ │ Battery  │ │ Warnings │ │ Settings │    │    │
│  │  │  Tab     │ │   Tab    │ │   Tab    │ │   Tab    │    │    │
│  │  └────┬─────┘ └────┬─────┘ └────┬─────┘ └────┬─────┘    │    │
│  └───────┼────────────┼────────────┼────────────┼──────────┘    │
│          │            │            │            │                │
│          └────────────┴────────────┴────────────┘                │
│                              │                                   │
│  ┌───────────────────────────▼───────────────────────────────┐  │
│  │              ServiceBridge (ara::com Proxy)               │  │
│  │  - FindService()    - Event Subscription                  │  │
│  │  - Method Calls     - Future::Then() callbacks            │  │
│  └───────────────────────────┬───────────────────────────────┘  │
└──────────────────────────────┼───────────────────────────────────┘
                               │
                    ═══════════╪═══════════  SOME/IP
                               │
┌──────────────────────────────┼───────────────────────────────────┐
│                              │                                   │
│  ┌───────────────────────────▼───────────────────────────────┐  │
│  │           DashboardServiceSkeleton (ara::com)             │  │
│  │  - OfferService()   - Method Handlers                     │  │
│  │  - Event Publishing - Field Updates                       │  │
│  └───────────────────────────┬───────────────────────────────┘  │
│                              │                                   │
│  ┌───────────────────────────▼───────────────────────────────┐  │
│  │              VehicleDatabase (JSON)                       │  │
│  │  - Speed, Battery, Temperature sensors                    │  │
│  │  - Warnings, Drive Mode                                   │  │
│  │  - Simulation engine                                      │  │
│  └───────────────────────────────────────────────────────────┘  │
│                                                                  │
│                     Dashboard Service                            │
└──────────────────────────────────────────────────────────────────┘
```

---

## 📝 ara:: Namespace Reference

```cpp
ara::
├── com::                    // Communication
│   ├── FindService()       // Service discovery
│   ├── ServiceProxy        // Client-side proxy
│   ├── ServiceSkeleton     // Server-side skeleton
│   ├── Event<T>           // Event notification
│   ├── Field<T>           // Data field (get/set)
│   └── Method<T>          // Remote method call
│
├── core::                   // Core utilities
│   ├── Future<T>          // Async result with Then()
│   ├── Promise<T>         // Async provider
│   ├── Result<T,E>        // Error handling
│   └── ErrorCode          // Error codes
│
├── log::                    // Logging
│   ├── Logger             // Logger instance
│   ├── LogLevel           // Debug/Info/Warn/Error
│   └── CreateLogger()     // Factory
│
└── diag::                   // Diagnostics
    ├── DiagnosticMonitor  // Sensor monitoring
    ├── DTCCode            // Diagnostic Trouble Code
    └── MonitorAction      // Pass/Fail actions
```

---

## 🔧 Troubleshooting

### vsomeip không tìm thấy service

```bash
# Kiểm tra configuration
echo $VSOMEIP_CONFIGURATION
cat $VSOMEIP_CONFIGURATION

# Kiểm tra routing
# Service phải là routing manager (đầu tiên trong applications)
```

### Library not found

```bash
sudo ldconfig
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

### Qt GUI không hiển thị

```bash
# Nếu chạy qua SSH
export DISPLAY=:0
# hoặc
ssh -X user@host
```

---

## 📚 Tài liệu tham khảo

1. **AUTOSAR**
   - [AUTOSAR Adaptive Platform](https://www.autosar.org/standards/adaptive-platform/)
   - ara::com SWS Specification
   - ara::core SWS Specification

2. **vsomeip**
   - [GitHub - COVESA/vsomeip](https://github.com/COVESA/vsomeip)
   - [vsomeip User Guide](https://github.com/COVESA/vsomeip/wiki)

3. **SOME/IP**
   - SOME/IP Protocol Specification (AUTOSAR)
   - SOME/IP Service Discovery Specification

4. **Qt**
   - [Qt Documentation](https://doc.qt.io/)
   - Signals and Slots
   - Threading with Qt

---

## 📄 License

This course material is provided for educational purposes.

---

## 🤝 Contributing

Feel free to submit issues and enhancement requests!

---

**Happy Learning! 🎓**
