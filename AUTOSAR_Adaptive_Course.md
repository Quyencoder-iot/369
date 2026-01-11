
---

## Bài 4: Trình diễn dự án và đóng gói sản phẩm

### 1. Quy trình phát triển chuẩn (V-Model trong Automotive)
Trong thực tế, trước khi code, bạn cần thiết kế hệ thống rõ ràng:
1. **Use Case Diagram:** Mô tả người dùng (Driver) tương tác với hệ thống (Dashboard).
2. **Component Diagram:** Chia nhỏ hệ thống thành `Service Application`, `Client Application`, `SOME/IP Middleware`.
3. **Sequence Diagram:** Vẽ luồng khởi tạo (Service Offer) -> Kết nối (Find/Subscribe) -> Truyền tin (Notify).

### 2. Ví dụ thực hành: Hệ thống Dashboard hoàn chỉnh

#### Ví dụ 1: Code Ngắn Gọn Nhất (Build Script)
*Mục tiêu: Script Bash để build và chạy toàn bộ hệ thống.*

**File: run_system.sh**
```bash
#!/bin/bash
# 1. Build
mkdir -p build && cd build
cmake ..
make

# 2. Setup Environment
export VSOMEIP_CONFIGURATION=../config

# 3. Run Service (Background)
./dashboard_service &
PID_SVC=$!

# 4. Run Client (Foreground)
./dashboard_gui

# 5. Cleanup
kill $PID_SVC
```

#### Ví dụ 2: Nâng Cao (Full Project Structure & CMake)
*Mục tiêu: Tổ chức thư mục dự án chuyên nghiệp, CMakeLists.txt chuẩn để build cả Qt và vsomeip.*

**Cấu trúc thư mục dự án:**
```
/AutosarDashboard
  ├── CMakeLists.txt
  ├── config/
  │   ├── service.json
  │   └── client.json
  ├── src/
  │   ├── service/
  │   │   └── main.cpp
  │   └── client/
  │       ├── main.cpp
  │       ├── DashboardUI.h
  │       └── DashboardUI.cpp
  └── scripts/
      └── run.sh
```

**File: CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.10)
project(AutosarDashboard)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Packages
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(vsomeip3 REQUIRED)
find_package(nlohmann_json REQUIRED)

# --- SERVICE TARGET ---
add_executable(dashboard_service src/service/main.cpp)
target_link_libraries(dashboard_service vsomeip3 nlohmann_json::nlohmann_json)

# --- CLIENT TARGET ---
add_executable(dashboard_gui 
    src/client/main.cpp 
    src/client/DashboardUI.cpp 
    src/client/DashboardUI.h
)
target_link_libraries(dashboard_gui Qt5::Widgets vsomeip3)

# --- POST BUILD: Copy Config ---
file(COPY config/service.json config/client.json DESTINATION ${CMAKE_BINARY_DIR})
```

**Giải thích Module:**
- **CMake Automation:** Tự động tìm thư viện (`find_package`), xử lý các file Qt (`AUTOMOC` cho Signal/Slot), và link thư viện `vsomeip3`.
- **Deployment:** Lệnh `file(COPY ...)` đảm bảo file cấu hình json luôn nằm cạnh file thực thi, giúp việc đóng gói (deploy) dễ dàng hơn.
- **Microservices:** Service và Client được biên dịch thành 2 file thực thi độc lập, mô phỏng đúng kiến trúc Distributed System trên xe hơi.

---
**Tổng kết:** Khóa học đã hướng dẫn bạn từ khái niệm cơ bản của AUTOSAR Adaptive, cách sử dụng `vsomeip`, giả lập dữ liệu xe, đến việc tích hợp giao diện HMI chuyên nghiệp với Qt. Bạn đã có nền tảng vững chắc để tham gia các dự án Automotive thực tế.
