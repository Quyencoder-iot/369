# 📘 BÀI 4: Trình Diễn Dự Án và Đóng Gói Sản Phẩm

## 📑 Mục Lục

1. [Tổng quan dự án](#1-tổng-quan-dự-án)
2. [UML Diagrams](#2-uml-diagrams)
3. [Quy trình phát triển](#3-quy-trình-phát-triển)
4. [Ví dụ 1: Script chạy hệ thống (Đơn giản)](#4-ví-dụ-1-script-chạy-hệ-thống-đơn-giản)
5. [Ví dụ 2: Full Integration (Nâng cao)](#5-ví-dụ-2-full-integration-nâng-cao)
6. [Đóng gói và triển khai](#6-đóng-gói-và-triển-khai)

---

## 1. Tổng Quan Dự Án

### 1.1 Kết quả đạt được

Sau khóa học, học viên có thể:

| Kỹ năng | Mô tả |
|---------|-------|
| ✅ **Kiến trúc AUTOSAR Adaptive** | Hiểu SOA, ara:: namespace, Manifest |
| ✅ **SOME/IP với vsomeip** | Cài đặt, cấu hình, viết Service/Client |
| ✅ **Service Layer Design** | Thiết kế API theo chuẩn ara::com |
| ✅ **Database ảo** | Mô phỏng dữ liệu ECU bằng JSON |
| ✅ **Qt Integration** | Tích hợp UI với SOME/IP backend |
| ✅ **UML Modeling** | Thiết kế hệ thống theo chuẩn |

### 1.2 Kiến trúc hệ thống hoàn chỉnh

```
┌─────────────────────────────────────────────────────────────────────┐
│                        DASHBOARD SYSTEM                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────────┐              ┌──────────────────┐             │
│  │   Qt Dashboard   │              │  Dashboard       │             │
│  │   Application    │◄────────────►│  Service         │             │
│  │                  │   SOME/IP    │                  │             │
│  │  - SpeedGauge    │              │  - getSpeed()    │             │
│  │  - BatteryWidget │              │  - getBattery()  │             │
│  │  - DriveMode     │              │  - getWarnings() │             │
│  └──────────────────┘              │  - getDriveMode()│             │
│         ▲                          │  - publishAll()  │             │
│         │                          └────────┬─────────┘             │
│         │ Qt Signal/Slot                    │                       │
│         │                                   │ JSON Database         │
│  ┌──────┴──────────┐               ┌────────▼─────────┐             │
│  │ DashboardProxy  │               │  database.json   │             │
│  │ (ara::com style)│               │  - vehicle       │             │
│  │                 │               │  - battery       │             │
│  │ - FindService() │               │  - sensors       │             │
│  │ - subscribeAll()│               │  - warnings      │             │
│  └─────────────────┘               └──────────────────┘             │
│                                                                      │
├─────────────────────────────────────────────────────────────────────┤
│                          VSOMEIP LAYER                               │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │  Service Discovery │ Routing │ Event Management │ Serialization│ │
│  └─────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. UML Diagrams

### 2.1 Use Case Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                     Vehicle Dashboard System                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│     ┌─────────┐                                                  │
│     │ Driver  │                                                  │
│     └────┬────┘                                                  │
│          │                                                       │
│          ├──────────► (View Speed)                              │
│          │                                                       │
│          ├──────────► (View Battery Status)                     │
│          │                                                       │
│          ├──────────► (View Warnings)                           │
│          │                                                       │
│          ├──────────► (Change Drive Mode)                       │
│          │                                                       │
│          └──────────► (View System Status)                      │
│                                                                  │
│     ┌───────────┐                                                │
│     │ Technician│                                                │
│     └─────┬─────┘                                                │
│           │                                                      │
│           ├──────────► (View Diagnostics)                       │
│           │                                                      │
│           └──────────► (Clear DTCs)                             │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 Component Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                        <<component>>                                 │
│                      Dashboard System                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                  <<component>>                                │   │
│  │               Qt Dashboard Application                        │   │
│  │  ┌────────────────┐  ┌─────────────────┐  ┌───────────────┐  │   │
│  │  │ SpeedGauge     │  │ BatteryWidget   │  │ DashboardWindow│  │   │
│  │  │ <<widget>>     │  │ <<widget>>      │  │ <<window>>    │  │   │
│  │  └────────────────┘  └─────────────────┘  └───────────────┘  │   │
│  │                              │                                │   │
│  │                   ┌──────────▼──────────┐                    │   │
│  │                   │ DashboardProxy      │                    │   │
│  │                   │ <<service proxy>>   │                    │   │
│  │                   └──────────┬──────────┘                    │   │
│  └──────────────────────────────┼───────────────────────────────┘   │
│                                 │                                    │
│                          <<interface>>                               │
│                         IDashboardService                            │
│                                 │                                    │
│  ┌──────────────────────────────┼───────────────────────────────┐   │
│  │                  <<component>>                                │   │
│  │               Dashboard Service                               │   │
│  │                   ┌──────────▼──────────┐                    │   │
│  │                   │ DashboardService    │                    │   │
│  │                   │ <<service>>         │                    │   │
│  │                   └──────────┬──────────┘                    │   │
│  │                              │                                │   │
│  │  ┌────────────────┐  ┌──────┴──────┐  ┌───────────────────┐  │   │
│  │  │ DatabaseManager│  │ara::log     │  │ DiagnosticManager │  │   │
│  │  │ <<singleton>>  │  │<<logging>>  │  │ <<singleton>>     │  │   │
│  │  └────────────────┘  └─────────────┘  └───────────────────┘  │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                  <<component>>                                │   │
│  │                    VSOMEIP                                    │   │
│  │  ┌─────────────┐  ┌──────────────┐  ┌────────────────────┐   │   │
│  │  │ Service     │  │ Routing      │  │ Service Discovery  │   │   │
│  │  │ Management  │  │ Manager      │  │                    │   │   │
│  │  └─────────────┘  └──────────────┘  └────────────────────┘   │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 2.3 Sequence Diagram - Get Speed

```
┌──────────┐     ┌───────────────┐     ┌─────────────────┐     ┌──────────────┐
│ Qt UI    │     │DashboardProxy │     │ DashboardService│     │DatabaseManager│
└────┬─────┘     └───────┬───────┘     └────────┬────────┘     └──────┬───────┘
     │                   │                      │                      │
     │ onRefreshClicked()│                      │                      │
     │──────────────────►│                      │                      │
     │                   │                      │                      │
     │                   │ sendRequest(GET_SPEED)                      │
     │                   │─────────────────────►│                      │
     │                   │      SOME/IP         │                      │
     │                   │                      │ GetSpeed()           │
     │                   │                      │─────────────────────►│
     │                   │                      │                      │
     │                   │                      │◄─────────────────────│
     │                   │                      │   speed: 120         │
     │                   │                      │                      │
     │                   │◄─────────────────────│                      │
     │                   │  Response: 120       │                      │
     │                   │                      │                      │
     │ emit speedReceived(120)                  │                      │
     │◄──────────────────│                      │                      │
     │                   │                      │                      │
     │ updateSpeedGauge()│                      │                      │
     │                   │                      │                      │
     ▼                   ▼                      ▼                      ▼
```

### 2.4 Sequence Diagram - Event Subscription

```
┌──────────┐     ┌───────────────┐     ┌─────────────────┐
│ Qt UI    │     │DashboardProxy │     │ DashboardService│
└────┬─────┘     └───────┬───────┘     └────────┬────────┘
     │                   │                      │
     │ onServiceAvailable()                     │
     │◄──────────────────│                      │
     │                   │                      │
     │                   │ subscribe(EVENTGROUP)│
     │                   │─────────────────────►│
     │                   │                      │
     │                   │◄─────────────────────│
     │                   │   Subscribe ACK      │
     │                   │                      │
     │                   │                      │ (Timer: 500ms)
     │                   │                      │────┐
     │                   │                      │    │ PublishAllSignals()
     │                   │◄─────────────────────│◄───┘
     │                   │ Event: ALL_SIGNALS   │
     │                   │                      │
     │ emit allDataReceived(data)               │
     │◄──────────────────│                      │
     │                   │                      │
     │ updateAllWidgets()│                      │
     │                   │                      │
     │                   │ (Repeat every 500ms) │
     │                   │                      │
     ▼                   ▼                      ▼
```

### 2.5 Activity Diagram - System Startup

```
┌─────────────────────────────────────────────────────────────────┐
│                      System Startup                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│                         ● Start                                  │
│                           │                                      │
│                           ▼                                      │
│                  ┌────────────────┐                             │
│                  │ Load Database  │                             │
│                  │ (database.json)│                             │
│                  └────────┬───────┘                             │
│                           │                                      │
│                           ▼                                      │
│                     ◇ Success?                                  │
│                    /         \                                   │
│                   /           \                                  │
│                 Yes           No                                 │
│                  │             │                                 │
│                  ▼             ▼                                 │
│    ┌──────────────────┐  ┌──────────────┐                       │
│    │ Init vsomeip     │  │ Log Error    │                       │
│    │ Application      │  │ Exit(1)      │                       │
│    └────────┬─────────┘  └──────────────┘                       │
│             │                                                    │
│             ▼                                                    │
│       ◇ Init OK?                                                │
│       /         \                                                │
│     Yes          No                                              │
│      │            │                                              │
│      ▼            ▼                                              │
│ ┌────────────┐  ┌──────────────┐                                │
│ │ Register   │  │ Log Error    │                                │
│ │ Handlers   │  │ Exit(1)      │                                │
│ └─────┬──────┘  └──────────────┘                                │
│       │                                                          │
│       ▼                                                          │
│ ┌────────────────┐                                              │
│ │ Offer Service  │                                              │
│ └───────┬────────┘                                              │
│         │                                                        │
│         ├───────────────────┐                                   │
│         │                   │                                    │
│         ▼                   ▼                                    │
│  ┌────────────┐     ┌──────────────┐                            │
│  │ Start      │     │ Start        │                            │
│  │ Publish    │     │ Diagnostic   │                            │
│  │ Thread     │     │ Thread       │                            │
│  └─────┬──────┘     └──────┬───────┘                            │
│        │                   │                                     │
│        └─────────┬─────────┘                                    │
│                  │                                               │
│                  ▼                                               │
│           ┌────────────┐                                        │
│           │ app->start()│                                       │
│           │ (blocking)  │                                       │
│           └─────────────┘                                       │
│                  │                                               │
│                  ▼                                               │
│                  ○ End                                          │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 2.6 State Machine - Service State

```
┌─────────────────────────────────────────────────────────────────┐
│                    Service State Machine                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│                         ● Initial                                │
│                           │                                      │
│                           │ create_application()                 │
│                           ▼                                      │
│                  ┌─────────────────┐                            │
│                  │                 │                             │
│                  │   CREATED       │                             │
│                  │                 │                             │
│                  └────────┬────────┘                            │
│                           │ init()                               │
│                           ▼                                      │
│                  ┌─────────────────┐                            │
│                  │                 │                             │
│                  │  INITIALIZED    │                             │
│                  │                 │                             │
│                  └────────┬────────┘                            │
│                           │ start()                              │
│                           ▼                                      │
│                  ┌─────────────────┐                            │
│        ┌────────►│                 │◄─────────┐                 │
│        │         │   REGISTERING   │          │                 │
│        │         │                 │          │                 │
│        │         └────────┬────────┘          │                 │
│        │                  │ ST_REGISTERED     │                 │
│        │                  ▼                   │                 │
│        │         ┌─────────────────┐          │                 │
│        │         │                 │          │                 │
│  reconnect       │   REGISTERED    │   ST_DEREGISTERED         │
│        │         │                 │          │                 │
│        │         └────────┬────────┘          │                 │
│        │                  │ offer_service()   │                 │
│        │                  ▼                   │                 │
│        │         ┌─────────────────┐          │                 │
│        │         │                 │──────────┘                 │
│        └─────────│    OFFERING     │                            │
│                  │                 │                             │
│                  └────────┬────────┘                            │
│                           │ stop()                               │
│                           ▼                                      │
│                  ┌─────────────────┐                            │
│                  │                 │                             │
│                  │    STOPPED      │                             │
│                  │                 │                             │
│                  └─────────────────┘                            │
│                           │                                      │
│                           ▼                                      │
│                          ○ Final                                │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 2.7 Class Diagram (Core Classes)

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Class Diagram                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                    <<interface>>                               │  │
│  │                  IDashboardService                             │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ + getSpeed(): uint16_t                                        │  │
│  │ + getBattery(): BatteryInfo                                   │  │
│  │ + getWarnings(): vector<Warning>                              │  │
│  │ + getDriveMode(): DriveMode                                   │  │
│  │ + publishAllSignals(): void                                   │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                              △                                       │
│                              │ implements                            │
│                              │                                       │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                    DashboardService                            │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ - app_: shared_ptr<vsomeip::application>                      │  │
│  │ - running_: atomic<bool>                                      │  │
│  │ - publishThread_: thread                                      │  │
│  │ - diagThread_: thread                                         │  │
│  │ - logger_: ara::log::Logger                                   │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ + init(): bool                                                │  │
│  │ + start(): void                                               │  │
│  │ + stop(): void                                                │  │
│  │ - OnState(state): void                                        │  │
│  │ - OnGetSpeed(msg): void                                       │  │
│  │ - OnGetBattery(msg): void                                     │  │
│  │ - PublishLoop(): void                                         │  │
│  │ - DiagnosticLoop(): void                                      │  │
│  └───────────────────────────────────────────────────────────────┘  │
│         │                                                            │
│         │ uses                                                       │
│         ▼                                                            │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                    DatabaseManager                             │  │
│  │                    <<singleton>>                               │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ - data_: json                                                 │  │
│  │ - mutex_: mutex                                               │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ + Instance(): DatabaseManager&                                │  │
│  │ + Load(path): bool                                            │  │
│  │ + GetSpeed(): uint16_t                                        │  │
│  │ + GetBatteryInfo(): BatteryInfo                               │  │
│  │ + GetWarnings(): vector<Warning>                              │  │
│  │ + GetDriveMode(): DriveMode                                   │  │
│  │ + GetAllSignals(): DashboardSignals                           │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                                                                      │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                    DashboardProxy                              │  │
│  │                    <<QObject>>                                 │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ - app_: shared_ptr<vsomeip::application>                      │  │
│  │ - workerThread_: QThread*                                     │  │
│  │ - serviceAvailable_: atomic<bool>                             │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ + FindService(parent): DashboardProxy*  <<static>>            │  │
│  │ + start(): void                                               │  │
│  │ + stop(): void                                                │  │
│  │ + getSpeed(): void                                            │  │
│  │ + subscribeAllSignals(): void                                 │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │ signals:                                                      │  │
│  │   + serviceAvailable()                                        │  │
│  │   + speedReceived(int)                                        │  │
│  │   + batteryReceived(BatteryInfo)                              │  │
│  │   + allDataReceived(DashboardData)                            │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                                                                      │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                   Data Types                                   │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │  BatteryInfo          │  Warning           │  DashboardData   │  │
│  │  - level: uint8_t     │  - code: string    │  - speed: uint16 │  │
│  │  - voltage: float     │  - message: string │  - rpm: uint16   │  │
│  │  - temperature: int16 │  - severity: uint8 │  - battery: ...  │  │
│  │  - isCharging: bool   │                    │  - warnings: ... │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. Quy Trình Phát Triển

### 3.1 Workflow

```
┌─────────────────────────────────────────────────────────────────┐
│                     Development Workflow                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. REQUIREMENTS                                                 │
│     ├── Use Case Diagram                                        │
│     └── Feature List                                            │
│             │                                                    │
│             ▼                                                    │
│  2. DESIGN                                                       │
│     ├── Component Architecture                                  │
│     ├── Class Diagram                                           │
│     ├── Sequence Diagram                                        │
│     └── State Machine                                           │
│             │                                                    │
│             ▼                                                    │
│  3. IMPLEMENTATION                                               │
│     ├── Service Layer (vsomeip)                                 │
│     ├── Database Layer (JSON)                                   │
│     ├── ara:: Simulation                                        │
│     └── Qt UI                                                   │
│             │                                                    │
│             ▼                                                    │
│  4. TESTING                                                      │
│     ├── Unit Tests                                              │
│     ├── Integration Tests                                       │
│     └── System Tests                                            │
│             │                                                    │
│             ▼                                                    │
│  5. DEPLOYMENT                                                   │
│     ├── Build & Package                                         │
│     ├── Configuration                                           │
│     └── Documentation                                           │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 Checklist phát triển

- [ ] **Requirements**
  - [ ] Xác định Use Cases
  - [ ] Định nghĩa Service Interface
  - [ ] Xác định Data Types

- [ ] **Design**
  - [ ] Vẽ Component Diagram
  - [ ] Vẽ Sequence Diagram cho mỗi Use Case
  - [ ] Định nghĩa State Machine

- [ ] **Implementation**
  - [ ] Tạo database.json
  - [ ] Implement ara:: simulation
  - [ ] Implement DashboardService
  - [ ] Implement DashboardProxy
  - [ ] Implement Qt UI

- [ ] **Testing**
  - [ ] Test Service standalone
  - [ ] Test Client standalone
  - [ ] Test Integration

- [ ] **Deployment**
  - [ ] Build scripts
  - [ ] Configuration files
  - [ ] Documentation

---

## 4. Ví Dụ 1: Script Chạy Hệ Thống (Đơn giản)

### 📁 Cấu trúc

```
example1/
├── run_demo.sh          # Script chạy demo
├── build_all.sh         # Script build
└── stop_all.sh          # Script dừng
```

### 📝 build_all.sh

```bash
#!/bin/bash
# ============================================
# BUILD SCRIPT - Biên dịch toàn bộ hệ thống
# ============================================

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COURSE_DIR="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "  AUTOSAR ADAPTIVE DASHBOARD - BUILD"
echo "=========================================="

# Build Dashboard Service
echo ""
echo "[1/2] Building Dashboard Service..."
cd "$COURSE_DIR/lesson2/example2"
mkdir -p build && cd build
cmake ..
make -j$(nproc)
echo "[OK] Dashboard Service built"

# Build Qt Dashboard
echo ""
echo "[2/2] Building Qt Dashboard..."
cd "$COURSE_DIR/lesson3/example2"
mkdir -p build && cd build
cmake ..
make -j$(nproc)
echo "[OK] Qt Dashboard built"

echo ""
echo "=========================================="
echo "  BUILD COMPLETE!"
echo "=========================================="
echo ""
echo "To run the demo, execute: ./run_demo.sh"
```

### 📝 run_demo.sh

```bash
#!/bin/bash
# ============================================
# RUN DEMO - Chạy hệ thống Dashboard
# ============================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COURSE_DIR="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "  AUTOSAR ADAPTIVE DASHBOARD - DEMO"
echo "=========================================="

# Set vsomeip configuration
export VSOMEIP_CONFIGURATION="$COURSE_DIR/lesson2/example2/vsomeip.json"
export VSOMEIP_APPLICATION_NAME="dashboard_demo"

# Check if service exists
SERVICE_BIN="$COURSE_DIR/lesson2/example2/build/dashboard_service"
CLIENT_BIN="$COURSE_DIR/lesson3/example2/build/qt_dashboard"

if [ ! -f "$SERVICE_BIN" ]; then
    echo "[ERROR] Dashboard Service not found. Run build_all.sh first."
    exit 1
fi

# Start Dashboard Service in background
echo ""
echo "[1/2] Starting Dashboard Service..."
cd "$COURSE_DIR/lesson2/example2/build"
./dashboard_service &
SERVICE_PID=$!
echo "[OK] Service started (PID: $SERVICE_PID)"

# Wait for service to initialize
sleep 2

# Start Qt Dashboard
echo ""
echo "[2/2] Starting Qt Dashboard..."
if [ -f "$CLIENT_BIN" ]; then
    cd "$COURSE_DIR/lesson3/example2/build"
    ./qt_dashboard
else
    echo "[INFO] Qt Dashboard not built. Running console client instead..."
    cd "$COURSE_DIR/lesson2/example2/build"
    ./dashboard_client
fi

# Cleanup
echo ""
echo "Stopping service..."
kill $SERVICE_PID 2>/dev/null || true
echo "[OK] Demo finished"
```

### 📝 stop_all.sh

```bash
#!/bin/bash
# ============================================
# STOP ALL - Dừng tất cả processes
# ============================================

echo "Stopping all Dashboard processes..."

pkill -f dashboard_service 2>/dev/null || true
pkill -f dashboard_client 2>/dev/null || true
pkill -f qt_dashboard 2>/dev/null || true

echo "[OK] All processes stopped"
```

---

## 5. Ví Dụ 2: Full Integration (Nâng cao)

### 📁 Cấu trúc

```
example2/
├── CMakeLists.txt           # Top-level CMake
├── src/
│   ├── service/             # Dashboard Service
│   ├── client/              # Console Client
│   └── qt_ui/               # Qt Dashboard
├── config/
│   ├── vsomeip.json
│   └── database.json
├── scripts/
│   ├── build.sh
│   ├── run.sh
│   └── package.sh
└── docker/
    └── Dockerfile
```

### 📝 CMakeLists.txt (Top-level)

```cmake
cmake_minimum_required(VERSION 3.10)
project(autosar_dashboard VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_SERVICE "Build Dashboard Service" ON)
option(BUILD_CLIENT "Build Console Client" ON)
option(BUILD_QT_UI "Build Qt Dashboard" ON)
option(BUILD_TESTS "Build Unit Tests" OFF)

# Find common dependencies
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Build Service
if(BUILD_SERVICE)
    add_subdirectory(src/service)
endif()

# Build Client
if(BUILD_CLIENT)
    add_subdirectory(src/client)
endif()

# Build Qt UI
if(BUILD_QT_UI)
    find_package(Qt5 COMPONENTS Widgets Core Gui)
    if(Qt5_FOUND)
        add_subdirectory(src/qt_ui)
    else()
        message(WARNING "Qt5 not found, skipping Qt UI build")
    endif()
endif()

# Copy configuration files
configure_file(config/vsomeip.json ${CMAKE_BINARY_DIR}/vsomeip.json COPYONLY)
configure_file(config/database.json ${CMAKE_BINARY_DIR}/database.json COPYONLY)

# Install
install(FILES 
    config/vsomeip.json 
    config/database.json 
    DESTINATION etc/dashboard)

# CPack for packaging
set(CPACK_PACKAGE_NAME "autosar-dashboard")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "AUTOSAR Adaptive Dashboard Demo")
set(CPACK_GENERATOR "DEB;TGZ")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev, libqt5widgets5")
include(CPack)
```

### 📝 Dockerfile

```dockerfile
# ============================================
# DOCKERFILE - Build và chạy Dashboard System
# ============================================

FROM ubuntu:20.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    nlohmann-json3-dev \
    qt5-default \
    && rm -rf /var/lib/apt/lists/*

# Build vsomeip
WORKDIR /opt
RUN git clone https://github.com/COVESA/vsomeip.git && \
    cd vsomeip && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Copy source
WORKDIR /app
COPY . .

# Build
RUN mkdir build && cd build && \
    cmake .. -DBUILD_QT_UI=OFF && \
    make -j$(nproc)

# Configuration
ENV VSOMEIP_CONFIGURATION=/app/config/vsomeip.json

# Run
CMD ["./build/dashboard_service", "./config/database.json"]
```

### 📝 scripts/package.sh

```bash
#!/bin/bash
# ============================================
# PACKAGE SCRIPT - Đóng gói sản phẩm
# ============================================

set -e

VERSION="1.0.0"
PACKAGE_NAME="autosar-dashboard-$VERSION"
BUILD_DIR="build"
PACKAGE_DIR="$PACKAGE_NAME"

echo "=========================================="
echo "  Packaging AUTOSAR Dashboard v$VERSION"
echo "=========================================="

# Create package directory
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR"/{bin,etc,lib,share/doc}

# Copy binaries
if [ -f "$BUILD_DIR/dashboard_service" ]; then
    cp "$BUILD_DIR/dashboard_service" "$PACKAGE_DIR/bin/"
fi
if [ -f "$BUILD_DIR/dashboard_client" ]; then
    cp "$BUILD_DIR/dashboard_client" "$PACKAGE_DIR/bin/"
fi
if [ -f "$BUILD_DIR/qt_dashboard" ]; then
    cp "$BUILD_DIR/qt_dashboard" "$PACKAGE_DIR/bin/"
fi

# Copy configuration
cp config/*.json "$PACKAGE_DIR/etc/"

# Copy documentation
cp README.md "$PACKAGE_DIR/share/doc/"

# Create startup script
cat > "$PACKAGE_DIR/bin/start-dashboard.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export VSOMEIP_CONFIGURATION="$SCRIPT_DIR/../etc/vsomeip.json"
"$SCRIPT_DIR/dashboard_service" "$SCRIPT_DIR/../etc/database.json" &
sleep 2
"$SCRIPT_DIR/qt_dashboard"
EOF
chmod +x "$PACKAGE_DIR/bin/start-dashboard.sh"

# Create tarball
tar -czvf "$PACKAGE_NAME.tar.gz" "$PACKAGE_DIR"

# Cleanup
rm -rf "$PACKAGE_DIR"

echo ""
echo "=========================================="
echo "  Package created: $PACKAGE_NAME.tar.gz"
echo "=========================================="
```

### 📝 Integration Test Script

```bash
#!/bin/bash
# ============================================
# INTEGRATION TEST
# ============================================

set -e

echo "=========================================="
echo "  Integration Test"
echo "=========================================="

# Start service
./build/dashboard_service ./config/database.json &
SERVICE_PID=$!
sleep 2

# Run test client
echo ""
echo "Testing GET_SPEED..."
timeout 5 ./build/dashboard_client || true

# Check service is still running
if kill -0 $SERVICE_PID 2>/dev/null; then
    echo "[PASS] Service still running"
else
    echo "[FAIL] Service crashed"
    exit 1
fi

# Cleanup
kill $SERVICE_PID 2>/dev/null || true

echo ""
echo "=========================================="
echo "  All tests passed!"
echo "=========================================="
```

---

## 6. Đóng Gói và Triển Khai

### 6.1 Cấu trúc đóng gói

```
autosar-dashboard-1.0.0/
├── bin/
│   ├── dashboard_service       # Service executable
│   ├── dashboard_client        # Console client
│   ├── qt_dashboard           # Qt application
│   └── start-dashboard.sh     # Startup script
├── etc/
│   ├── vsomeip.json           # SOME/IP config
│   └── database.json          # Vehicle data
├── lib/
│   └── (shared libraries)
└── share/
    └── doc/
        └── README.md
```

### 6.2 Hướng dẫn cài đặt

```bash
# Giải nén package
tar -xzvf autosar-dashboard-1.0.0.tar.gz

# Di chuyển vào thư mục
cd autosar-dashboard-1.0.0

# Chạy demo
./bin/start-dashboard.sh
```

### 6.3 Cấu hình môi trường

```bash
# ~/.bashrc hoặc /etc/environment

# VSOMEIP
export VSOMEIP_CONFIGURATION=/opt/dashboard/etc/vsomeip.json

# Library path (nếu cần)
export LD_LIBRARY_PATH=/opt/dashboard/lib:$LD_LIBRARY_PATH
```

### 6.4 Systemd Service (Production)

```ini
# /etc/systemd/system/dashboard.service

[Unit]
Description=AUTOSAR Adaptive Dashboard Service
After=network.target

[Service]
Type=simple
User=dashboard
WorkingDirectory=/opt/dashboard
Environment=VSOMEIP_CONFIGURATION=/opt/dashboard/etc/vsomeip.json
ExecStart=/opt/dashboard/bin/dashboard_service /opt/dashboard/etc/database.json
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

---

## 📊 Tổng Kết Khóa Học

### Kiến thức đã học

| Bài | Nội dung | Kết quả |
|-----|----------|---------|
| **1** | AUTOSAR Adaptive & vsomeip | Hiểu kiến trúc, viết được Service/Client |
| **2** | Database ảo & Service Layer | Thiết kế API theo chuẩn ara:: |
| **3** | Qt UI Integration | Tích hợp UI với SOME/IP |
| **4** | Demo & Packaging | Triển khai hệ thống hoàn chỉnh |

### Kỹ năng đạt được

- ✅ Hiểu kiến trúc **AUTOSAR Adaptive Platform**
- ✅ Sử dụng **SOME/IP** với vsomeip
- ✅ Thiết kế **Service-Oriented Architecture**
- ✅ Mô phỏng **ara:: namespace**
- ✅ Tích hợp **Qt UI** với service backend
- ✅ Thiết kế hệ thống với **UML Diagrams**
- ✅ Đóng gói và triển khai sản phẩm

### Bước tiếp theo

1. **Học thêm**: Adaptive AUTOSAR specification chính thức
2. **Thực hành**: Thêm features mới vào Dashboard
3. **Nâng cao**: Tìm hiểu ara::per, ara::exec
4. **Production**: Triển khai trên embedded Linux

---

## 📚 Tài Liệu Tham Khảo

1. **AUTOSAR Adaptive Platform**
   - [AUTOSAR Website](https://www.autosar.org/standards/adaptive-platform/)
   - Specification Documents

2. **vsomeip**
   - [GitHub Repository](https://github.com/COVESA/vsomeip)
   - [User Guide](https://github.com/COVESA/vsomeip/wiki)

3. **Qt Framework**
   - [Qt Documentation](https://doc.qt.io/)
   - [Qt for Automotive](https://www.qt.io/qt-automotive-suite)

---

**🎓 Chúc mừng bạn đã hoàn thành khóa học!**

**[← Bài 3](../lesson3/README.md) | [Về Trang Chủ](../../README.md)**
