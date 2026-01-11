# 🚗 KHÓA HỌC AUTOSAR ADAPTIVE VỚI VSOMEIP

## 📚 Mục Lục

| Bài | Nội Dung | Thời Lượng |
|-----|----------|------------|
| 1 | [Tổng quan AUTOSAR Adaptive và vsomeip](./course/lesson1/README.md) | 2-3 giờ |
| 2 | [Database ảo và Service Layer](./course/lesson2/README.md) | 3-4 giờ |
| 3 | [Qt UI với Service backend](./course/lesson3/README.md) | 3-4 giờ |
| 4 | [Trình diễn và đóng gói](./course/lesson4/README.md) | 2-3 giờ |

---

## 🎯 Mục Tiêu Khóa Học

Sau khi hoàn thành khóa học, bạn sẽ:

1. ✅ Hiểu tổng quan kiến trúc **AUTOSAR Adaptive**
2. ✅ Thiết kế dự án theo quy trình chuẩn (Use Case, Component, Sequence Diagram)
3. ✅ Cài đặt và sử dụng **SOME/IP** (vsomeip)
4. ✅ Xây dựng **Service Layer** với ara:: namespace
5. ✅ Tích hợp **Qt UI** với backend service
6. ✅ Chạy hệ thống hoàn chỉnh trên PC

---

## 🏗️ Kiến Trúc Tổng Quan

```
┌─────────────────────────────────────────────────────────────┐
│                    AUTOSAR ADAPTIVE PLATFORM                 │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │  Qt UI App  │  │  Dashboard  │  │   Sensor Service    │  │
│  │   (Client)  │  │   Service   │  │    (Provider)       │  │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘  │
│         │                │                     │             │
│  ┌──────┴────────────────┴─────────────────────┴──────────┐ │
│  │                    ara::com (SOME/IP)                   │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │  ara::core  │  ara::log  │  ara::diag  │  ara::exec    │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                              │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              vsomeip (SOME/IP Implementation)           │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Yêu Cầu Hệ Thống

```bash
# Ubuntu 20.04+ hoặc tương đương
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    git \
    qt5-default \
    qtcreator \
    nlohmann-json3-dev
```

---

## 📁 Cấu Trúc Dự Án

```
course/
├── lesson1/                    # Bài 1: AUTOSAR Adaptive & vsomeip
│   ├── example1/              # Ví dụ đơn giản
│   │   ├── hello_service.cpp
│   │   ├── hello_client.cpp
│   │   └── vsomeip.json
│   └── example2/              # Ví dụ nâng cao
│       ├── advanced_service.cpp
│       ├── advanced_client.cpp
│       └── vsomeip_advanced.json
│
├── lesson2/                    # Bài 2: Database ảo & Service Layer
│   ├── example1/              # Ví dụ đơn giản
│   │   ├── simple_service.cpp
│   │   └── vehicle_data.json
│   └── example2/              # Ví dụ nâng cao - Dashboard Service
│       ├── ara/               # Mô phỏng ara:: namespace
│       ├── dashboard_service.cpp
│       └── database.json
│
├── lesson3/                    # Bài 3: Qt UI Integration
│   ├── example1/              # Qt Client đơn giản
│   └── example2/              # Qt Dashboard hoàn chỉnh
│
└── lesson4/                    # Bài 4: Demo & Packaging
    ├── diagrams/              # UML Diagrams
    └── integration/           # Full integration
```

---

## 🚀 Bắt Đầu Nhanh

```bash
# Clone repository
git clone <repo-url>
cd autosar-adaptive-course

# Biên dịch vsomeip (xem Bài 1)
cd course/lesson1
./build_vsomeip.sh

# Chạy ví dụ đầu tiên
./example1/hello_service &
./example1/hello_client
```

---

## 📖 Quy Ước Code

- **Ví dụ 1**: Code ngắn gọn nhất, chỉ chứa các yếu tố **BẮT BUỘC**
- **Ví dụ 2**: Code đầy đủ, nâng cao với giải thích **từng module**

---

## 👨‍🏫 Tác Giả

Khóa học được thiết kế cho kỹ sư phần mềm ô tô muốn học AUTOSAR Adaptive Platform.

---

**Chúc bạn học tốt! 🎓**
