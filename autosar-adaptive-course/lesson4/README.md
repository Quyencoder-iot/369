# Bài 4: Trình diễn Dự án và Đóng gói Sản phẩm

## 📚 Mục tiêu bài học
- Tổng kết kiến thức AUTOSAR Adaptive đã học
- Áp dụng các mô hình thiết kế chuẩn (Use Case, Component, Sequence, Activity, State Machine)
- Đóng gói và triển khai ứng dụng
- Demo hệ thống hoàn chỉnh

---

## 1. Tổng quan Kiến trúc AUTOSAR Adaptive

```
┌─────────────────────────────────────────────────────────────────────┐
│                     AUTOSAR Adaptive Platform                        │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌─────────────────────────────────────────────────────────────┐   │
│   │                    Application Layer                         │   │
│   │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │   │
│   │  │ Dashboard    │  │  Qt GUI      │  │  Diagnostic  │       │   │
│   │  │ Service      │  │  Client      │  │  Manager     │       │   │
│   │  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘       │   │
│   └─────────┼─────────────────┼─────────────────┼───────────────┘   │
│             │                 │                 │                    │
│   ┌─────────┴─────────────────┴─────────────────┴───────────────┐   │
│   │                     ara:: API Layer                          │   │
│   │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐            │   │
│   │  │ara::com │ │ara::core│ │ara::log │ │ara::diag│            │   │
│   │  └─────────┘ └─────────┘ └─────────┘ └─────────┘            │   │
│   └──────────────────────────┬──────────────────────────────────┘   │
│                              │                                       │
│   ┌──────────────────────────┴──────────────────────────────────┐   │
│   │                   Communication Layer                        │   │
│   │                      SOME/IP (vsomeip)                       │   │
│   └──────────────────────────┬──────────────────────────────────┘   │
│                              │                                       │
│   ┌──────────────────────────┴──────────────────────────────────┐   │
│   │                    Operating System                          │   │
│   │                   Linux / POSIX                              │   │
│   └─────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Thiết kế theo Mô hình Chuẩn

### 2.1 Use Case Diagram

```
                    ┌─────────────────────────────────────────┐
                    │          Dashboard System                │
                    │                                          │
   ┌─────┐          │  ┌─────────────────────────────────┐    │
   │     │──────────┼──│ View Vehicle Speed              │    │
   │     │          │  └─────────────────────────────────┘    │
   │     │          │                                          │
   │     │          │  ┌─────────────────────────────────┐    │
   │ User│──────────┼──│ View Battery Status             │    │
   │     │          │  └─────────────────────────────────┘    │
   │     │          │                                          │
   │     │          │  ┌─────────────────────────────────┐    │
   │     │──────────┼──│ View/Acknowledge Warnings       │    │
   │     │          │  └─────────────────────────────────┘    │
   │     │          │                                          │
   │     │          │  ┌─────────────────────────────────┐    │
   │     │──────────┼──│ Change Drive Mode               │    │
   └─────┘          │  └─────────────────────────────────┘    │
                    │                                          │
                    │  ┌─────────────────────────────────┐    │
   ┌─────┐          │  │ Provide Vehicle Data            │    │
   │Sensor│─────────┼──│ (via SOME/IP)                   │    │
   │System│         │  └─────────────────────────────────┘    │
   └─────┘          │                                          │
                    └─────────────────────────────────────────┘
```

### 2.2 Component Architecture Diagram

```
┌───────────────────────────────────────────────────────────────────┐
│                        System Architecture                         │
├───────────────────────────────────────────────────────────────────┤
│                                                                    │
│  ┌──────────────────────┐         ┌──────────────────────┐        │
│  │    Qt Dashboard      │         │   Dashboard Service   │        │
│  │    Application       │         │                      │        │
│  │  ┌────────────────┐  │         │  ┌────────────────┐  │        │
│  │  │  MainWindow    │  │         │  │ ServiceSkeleton│  │        │
│  │  │  (UI Layer)    │  │         │  │ (ara::com)     │  │        │
│  │  └───────┬────────┘  │         │  └───────┬────────┘  │        │
│  │          │           │         │          │           │        │
│  │  ┌───────▼────────┐  │         │  ┌───────▼────────┐  │        │
│  │  │ ServiceBridge  │  │◄───────►│  │ VehicleDatabase│  │        │
│  │  │ (ServiceProxy) │  │ SOME/IP │  │ (JSON Data)    │  │        │
│  │  └───────┬────────┘  │         │  └───────┬────────┘  │        │
│  │          │           │         │          │           │        │
│  │  ┌───────▼────────┐  │         │  ┌───────▼────────┐  │        │
│  │  │    vsomeip     │  │         │  │    vsomeip     │  │        │
│  │  │    Client      │  │         │  │    Service     │  │        │
│  │  └────────────────┘  │         │  └────────────────┘  │        │
│  └──────────────────────┘         └──────────────────────┘        │
│                                                                    │
│  ┌────────────────────────────────────────────────────────────┐   │
│  │                    vsomeip Configuration                    │   │
│  │                    (vsomeip.json)                           │   │
│  └────────────────────────────────────────────────────────────┘   │
│                                                                    │
└───────────────────────────────────────────────────────────────────┘
```

### 2.3 Sequence Diagram - Get Vehicle Speed

```
┌──────────┐     ┌──────────────┐     ┌─────────────┐     ┌────────────┐
│ Qt UI    │     │ServiceBridge │     │   vsomeip   │     │  Dashboard │
│(MainWin) │     │   (Proxy)    │     │  Transport  │     │  Service   │
└────┬─────┘     └──────┬───────┘     └──────┬──────┘     └─────┬──────┘
     │                  │                    │                   │
     │ requestSpeed()   │                    │                   │
     │─────────────────>│                    │                   │
     │                  │                    │                   │
     │                  │ create_request()   │                   │
     │                  │───────────────────>│                   │
     │                  │                    │                   │
     │                  │ send(request)      │                   │
     │                  │───────────────────>│                   │
     │                  │                    │                   │
     │                  │                    │ [SOME/IP Request] │
     │                  │                    │──────────────────>│
     │                  │                    │                   │
     │                  │                    │                   │ GetSpeed()
     │                  │                    │                   │────────┐
     │                  │                    │                   │        │
     │                  │                    │                   │<───────┘
     │                  │                    │                   │
     │                  │                    │ [SOME/IP Response]│
     │                  │                    │<──────────────────│
     │                  │                    │                   │
     │                  │ onMessage()        │                   │
     │                  │<───────────────────│                   │
     │                  │                    │                   │
     │ emit speedUpdated(speed)              │                   │
     │<─────────────────│                    │                   │
     │                  │                    │                   │
     │ [Qt::QueuedConnection]                │                   │
     │ onSpeedUpdated() │                    │                   │
     │────────┐         │                    │                   │
     │        │         │                    │                   │
     │<───────┘         │                    │                   │
     │                  │                    │                   │
     │ Update UI        │                    │                   │
     │                  │                    │                   │
```

### 2.4 Activity Diagram - Service Discovery

```
                    ┌─────────────────┐
                    │     Start       │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │  Initialize     │
                    │  vsomeip App    │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Register        │
                    │ Handlers        │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Request         │
                    │ Service         │
                    └────────┬────────┘
                             │
                             ▼
               ┌─────────────────────────────┐
               │  Wait for Service           │
               │  (FindService with timeout) │
               └─────────────┬───────────────┘
                             │
              ┌──────────────┴──────────────┐
              │                             │
              ▼                             ▼
    ┌─────────────────┐           ┌─────────────────┐
    │ Service Found   │           │ Timeout         │
    │ (Available)     │           │ (Not Available) │
    └────────┬────────┘           └────────┬────────┘
             │                             │
             ▼                             ▼
    ┌─────────────────┐           ┌─────────────────┐
    │ Subscribe       │           │ Retry /         │
    │ Events          │           │ Show Error      │
    └────────┬────────┘           └────────┬────────┘
             │                             │
             ▼                             │
    ┌─────────────────┐                    │
    │ Start           │                    │
    │ Communication   │                    │
    └────────┬────────┘                    │
             │                             │
             └──────────────┬──────────────┘
                            │
                            ▼
                    ┌─────────────────┐
                    │ Running State   │
                    │ (Handle Events) │
                    └─────────────────┘
```

### 2.5 State Machine - Service Connection

```
                            ┌───────────────────┐
                            │                   │
                            ▼                   │
              ┌─────────────────────────┐       │
              │    DISCONNECTED         │       │ timeout
              │  (Initial State)        │       │
              └───────────┬─────────────┘       │
                          │                     │
                          │ init()              │
                          ▼                     │
              ┌─────────────────────────┐       │
              │    INITIALIZING         │       │
              └───────────┬─────────────┘       │
                          │                     │
                          │ success             │
                          ▼                     │
              ┌─────────────────────────┐       │
              │    DISCOVERING          │───────┘
              │  (FindService)          │
              └───────────┬─────────────┘
                          │
                          │ service available
                          ▼
              ┌─────────────────────────┐
              │    CONNECTING           │
              │  (Subscribe Events)     │
              └───────────┬─────────────┘
                          │
                          │ subscribed
                          ▼
              ┌─────────────────────────┐
        ┌────►│    CONNECTED            │◄────┐
        │     │  (Normal Operation)     │     │
        │     └───────────┬─────────────┘     │
        │                 │                   │
        │                 │ service lost      │ reconnect
        │                 ▼                   │
        │     ┌─────────────────────────┐     │
        │     │    RECONNECTING         │─────┘
        │     └───────────┬─────────────┘
        │                 │
        │                 │ max retries exceeded
        │                 ▼
        │     ┌─────────────────────────┐
        │     │    ERROR                │
        │     └───────────┬─────────────┘
        │                 │
        │                 │ user retry
        └─────────────────┘
```

---

## 3. Cài đặt và Sử dụng SOME/IP (vsomeip)

### 3.1 Yêu cầu hệ thống

```bash
# Ubuntu 20.04 / 22.04
sudo apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    git \
    qt5-default \  # hoặc qtbase5-dev cho Ubuntu 22.04
    qttools5-dev
```

### 3.2 Build vsomeip

```bash
# Clone và build
git clone https://github.com/COVESA/vsomeip.git
cd vsomeip
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
```

### 3.3 Cấu hình Environment

```bash
# Thêm vào ~/.bashrc
export VSOMEIP_CONFIGURATION=/path/to/vsomeip.json
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

---

## 4. Ví dụ Code

### 📝 Ví dụ 1: Build Script Cơ bản
Xem: `examples/basic/`

### 📝 Ví dụ 2: Project Đóng gói Hoàn chỉnh
Xem: `examples/advanced/`

---

## 5. Kết quả Học tập

Sau khi hoàn thành khóa học, học viên có thể:

### 5.1 Hiểu Tổng quan Kiến trúc AUTOSAR Adaptive

- [ ] Giải thích được kiến trúc SOA
- [ ] Mô tả các thành phần của ara:: namespace
- [ ] Hiểu vai trò của SOME/IP trong giao tiếp
- [ ] Phân biệt Service Provider và Service Consumer

### 5.2 Thiết kế Dự án theo Quy trình Chuẩn

- [ ] Vẽ Use Case Diagram cho requirements
- [ ] Thiết kế Component Architecture
- [ ] Tạo Sequence Diagram cho luồng chính
- [ ] Mô tả Activity Diagram cho processes
- [ ] Định nghĩa State Machine cho states

### 5.3 Cài đặt và Sử dụng SOME/IP

- [ ] Build vsomeip từ source
- [ ] Cấu hình vsomeip.json
- [ ] Implement Service với ServiceSkeleton pattern
- [ ] Implement Client với ServiceProxy pattern
- [ ] Xử lý events và methods

### 5.4 Tích hợp Hệ thống

- [ ] Kết nối Qt UI với vsomeip backend
- [ ] Xử lý thread-safe giữa Qt và vsomeip
- [ ] Implement real-time data updates
- [ ] Error handling và reconnection

---

## 6. Checklist Đóng gói Sản phẩm

### Build Checklist

```
□ Source code compiled without errors
□ All dependencies resolved
□ Unit tests passed
□ Integration tests passed
□ Static analysis clean
□ Documentation complete
```

### Packaging Checklist

```
□ Executable files
□ Shared libraries
□ Configuration files (vsomeip.json)
□ Database files (vehicle_database.json)
□ README với hướng dẫn
□ License file
□ Version information
```

### Deployment Checklist

```
□ Target system requirements verified
□ Dependencies installed
□ Environment variables configured
□ Network configuration (for distributed)
□ Permissions set correctly
□ Service auto-start configured (optional)
```

---

## 7. Demo Script

### Terminal 1 - Start Service

```bash
cd /path/to/build
export VSOMEIP_CONFIGURATION=./vsomeip.json
./dashboard_service_advanced
```

### Terminal 2 - Start Qt Client

```bash
cd /path/to/build
export VSOMEIP_CONFIGURATION=./vsomeip.json
./qt_dashboard_advanced
```

### Expected Output

1. Service khởi động và offer service
2. Client phát hiện service (FindService)
3. Client subscribe events
4. UI hiển thị real-time data:
   - Speed gauge cập nhật liên tục
   - Battery level với progress bar
   - Warnings list
   - Temperature displays
5. User có thể thay đổi drive mode
6. Graceful shutdown khi đóng application

---

## 8. Troubleshooting

### Service không tìm thấy

```bash
# Kiểm tra vsomeip configuration
cat $VSOMEIP_CONFIGURATION

# Kiểm tra network
ip addr show

# Kiểm tra multicast
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev lo
```

### Lỗi library not found

```bash
# Cập nhật library cache
sudo ldconfig

# Kiểm tra library path
echo $LD_LIBRARY_PATH
ldd ./dashboard_service
```

### Qt không hiển thị

```bash
# Kiểm tra DISPLAY (nếu chạy qua SSH)
export DISPLAY=:0

# Hoặc dùng X forwarding
ssh -X user@host
```

---

## 9. Tài liệu Tham khảo

1. **AUTOSAR Adaptive Platform**
   - [AUTOSAR Specification](https://www.autosar.org/standards/adaptive-platform/)
   - ara::com SWS
   - ara::core SWS

2. **vsomeip**
   - [GitHub Repository](https://github.com/COVESA/vsomeip)
   - [User Guide](https://github.com/COVESA/vsomeip/wiki)

3. **Qt**
   - [Qt Documentation](https://doc.qt.io/)
   - Signals & Slots
   - Threading with QThread

4. **SOME/IP**
   - SOME/IP Protocol Specification
   - SOME/IP Service Discovery

---

## 🎓 Chúc mừng bạn đã hoàn thành khóa học!

Bạn đã nắm vững các kiến thức cốt lõi về AUTOSAR Adaptive Platform và có thể:
- Thiết kế hệ thống theo SOA
- Implement services với SOME/IP
- Tích hợp UI với backend
- Deploy và troubleshoot

**Next Steps:**
- Tham gia cộng đồng AUTOSAR
- Tìm hiểu thêm về ara::exec, ara::per
- Nghiên cứu về Adaptive AUTOSAR trên hardware thực
- Contribute to vsomeip project
