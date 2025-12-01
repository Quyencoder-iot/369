# LayerShellQt - Quick Start Guide

## Tóm Tắt Nhanh

LayerShellQt cho phép bạn tạo các cửa sổ Qt chạy trên các "lớp" khác nhau trong Wayland compositor, không bị quản lý bởi window manager thông thường.

## 5 Phút Để Bắt Đầu

### Bước 1: Kiểm Tra Yêu Cầu

```bash
# Kiểm tra Qt version
qmake --version

# Kiểm tra Wayland compositor
echo $XDG_SESSION_TYPE  # Phải là "wayland"

# Kiểm tra LayerShellQt đã cài đặt
pkg-config --modversion layershellqt
```

### Bước 2: Tạo Project Đơn Giản

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyLayerApp)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Wayland)
find_package(LayerShellQt REQUIRED)

set(CMAKE_AUTOMOC ON)

add_executable(myapp main.cpp)
target_link_libraries(myapp
    Qt6::Core Qt6::Gui Qt6::Wayland
    LayerShellQt::LayerShellQt
)
```

**main.cpp:**
```cpp
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QWaylandLayerShellV1>
#include <QWaylandLayerSurfaceV1>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qputenv("QT_QPA_PLATFORM", "wayland");
    
    QWidget widget;
    widget.setWindowFlags(Qt::FramelessWindowHint);
    widget.setAttribute(Qt::WA_TranslucentBackground);
    
    auto *label = new QLabel("Hello LayerShell!", &widget);
    label->setAlignment(Qt::AlignCenter);
    
    // Setup LayerShell
    auto *layerShell = QWaylandLayerShellV1::instance();
    if (layerShell) {
        QWindow *window = widget.windowHandle();
        if (window) {
            auto *surface = layerShell->getLayerSurface(window);
            if (surface) {
                surface->setLayer(QWaylandLayerShellV1::TopLayer);
                surface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
                surface->setExclusiveZone(50);
                surface->commit();
                
                connect(surface, &QWaylandLayerSurfaceV1::configureRequested,
                        [surface](const QSize &size, uint32_t serial) {
                            surface->ackConfigure(serial);
                        });
            }
        }
    }
    
    widget.resize(1920, 50);
    widget.show();
    
    return app.exec();
}
```

### Bước 3: Build và Chạy

```bash
mkdir build && cd build
cmake ..
make
./myapp
```

## Các Pattern Thường Dùng

### Pattern 1: Top Panel
```cpp
surface->setLayer(QWaylandLayerShellV1::TopLayer);
surface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
surface->setExclusiveZone(height());
```

### Pattern 2: Bottom Dock
```cpp
surface->setLayer(QWaylandLayerShellV1::BottomLayer);
surface->setAnchor(Qt::BottomEdge | Qt::LeftEdge | Qt::RightEdge);
surface->setExclusiveZone(height());
surface->setMargin(0, 0, 10, 0); // 10px từ dưới
```

### Pattern 3: Notification
```cpp
surface->setLayer(QWaylandLayerShellV1::OverlayLayer);
surface->setAnchor(Qt::TopEdge | Qt::RightEdge);
surface->setExclusiveZone(0);
surface->setKeyboardInteractivity(QWaylandLayerShellV1::None);
surface->setMargin(20, 20, 0, 0);
```

## Checklist Trước Khi Deploy

- [ ] Đã kiểm tra chạy trên Wayland
- [ ] Đã xử lý `configureRequested` và ACK
- [ ] Đã set `exclusiveZone` đúng
- [ ] Đã test trên compositor target (Sway/Hyprland)
- [ ] Đã xử lý multi-monitor nếu cần
- [ ] Đã cleanup resources khi đóng

## Troubleshooting Nhanh

| Vấn đề | Giải pháp |
|--------|-----------|
| Không hiển thị | Kiểm tra `windowHandle()` và `show()` |
| Kích thước sai | Xử lý `configureRequested` |
| Không nhận input | Set `keyboardInteractivity` |
| Bị che bởi cửa sổ khác | Tăng `exclusiveZone` hoặc đổi layer |

## Tài Liệu Đầy Đủ

Xem `LAYERSHELLQT_GUIDE.md` để có hướng dẫn chi tiết hơn.
