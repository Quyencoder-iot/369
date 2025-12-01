# LayerShellQt Cheat Sheet

## Quick Reference

### Import Cần Thiết

```cpp
#include <QWaylandLayerShellV1>
#include <QWaylandLayerSurfaceV1>
#include <QWindow>
```

### Setup Cơ Bản

```cpp
// 1. Lấy instance
auto *layerShell = QWaylandLayerShellV1::instance();

// 2. Lấy window handle
QWindow *window = widget.windowHandle();

// 3. Tạo layer surface
auto *surface = layerShell->getLayerSurface(window);

// 4. Configure
surface->setLayer(QWaylandLayerShellV1::TopLayer);
surface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
surface->setExclusiveZone(height());
surface->commit();

// 5. Xử lý configure event
connect(surface, &QWaylandLayerSurfaceV1::configureRequested,
        [surface](const QSize &size, uint32_t serial) {
            surface->ackConfigure(serial);
        });
```

## Layers

| Layer | Value | Use Case |
|-------|-------|----------|
| `BackgroundLayer` | 0 | Wallpapers, desktop |
| `BottomLayer` | 1 | Dock, taskbar |
| `TopLayer` | 2 | Panel, status bar |
| `OverlayLayer` | 3 | Notifications, popups |

## Anchors

```cpp
Qt::TopEdge      // Neo vào cạnh trên
Qt::BottomEdge   // Neo vào cạnh dưới
Qt::LeftEdge     // Neo vào cạnh trái
Qt::RightEdge    // Neo vào cạnh phải

// Kết hợp nhiều anchor
Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge
```

## Keyboard Interactivity

| Value | Behavior |
|-------|----------|
| `None` | Không nhận keyboard input |
| `Exclusive` | Nhận tất cả keyboard input |
| `OnDemand` | Nhận khi được focus |

## Common Patterns

### Top Panel
```cpp
surface->setLayer(QWaylandLayerShellV1::TopLayer);
surface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
surface->setExclusiveZone(height());
```

### Bottom Dock
```cpp
surface->setLayer(QWaylandLayerShellV1::BottomLayer);
surface->setAnchor(Qt::BottomEdge | Qt::LeftEdge | Qt::RightEdge);
surface->setExclusiveZone(height());
surface->setMargin(0, 0, 10, 0);
```

### Notification
```cpp
surface->setLayer(QWaylandLayerShellV1::OverlayLayer);
surface->setAnchor(Qt::TopEdge | Qt::RightEdge);
surface->setExclusiveZone(0);
surface->setKeyboardInteractivity(QWaylandLayerShellV1::None);
surface->setMargin(20, 20, 0, 0);
```

### Full Screen Overlay
```cpp
surface->setLayer(QWaylandLayerShellV1::OverlayLayer);
surface->setAnchor(Qt::TopEdge | Qt::BottomEdge | 
                   Qt::LeftEdge | Qt::RightEdge);
surface->setExclusiveZone(0);
```

## Widget Setup

```cpp
// Cần thiết cho mọi layer surface
widget.setWindowFlags(Qt::FramelessWindowHint);
widget.setAttribute(Qt::WA_TranslucentBackground);

// Tùy chọn
widget.setWindowFlags(Qt::WindowStaysOnTopHint); // Cho overlay
```

## Environment Setup

```cpp
// Đảm bảo chạy trên Wayland
qputenv("QT_QPA_PLATFORM", "wayland");

// Debug logging
qputenv("QT_LOGGING_RULES", "qt.wayland*=true");
```

## Error Checking

```cpp
// Kiểm tra platform
if (QGuiApplication::platformName() != "wayland") {
    qWarning() << "Not on Wayland!";
    return;
}

// Kiểm tra LayerShell
auto *layerShell = QWaylandLayerShellV1::instance();
if (!layerShell) {
    qWarning() << "LayerShell not available";
    return;
}

// Kiểm tra window handle
QWindow *window = widget.windowHandle();
if (!window) {
    qWarning() << "Window handle not available";
    return;
}
```

## Signals

```cpp
// Configure event (QUAN TRỌNG!)
connect(surface, &QWaylandLayerSurfaceV1::configureRequested,
        [surface](const QSize &size, uint32_t serial) {
            // Resize nếu cần
            resize(size);
            // Phải ACK!
            surface->ackConfigure(serial);
        });

// Surface bị đóng
connect(surface, &QWaylandLayerSurfaceV1::closed,
        widget, &QWidget::close);
```

## Methods

| Method | Mô tả |
|--------|------|
| `setLayer(Layer)` | Thiết lập layer |
| `setAnchor(Qt::Edges)` | Thiết lập anchor |
| `setSize(QSize)` | Thiết lập kích thước |
| `setExclusiveZone(int32_t)` | Thiết lập vùng độc quyền |
| `setMargin(top, right, bottom, left)` | Thiết lập margin |
| `setKeyboardInteractivity(KeyboardInteractivity)` | Thiết lập keyboard interactivity |
| `commit()` | Commit các thay đổi |
| `ackConfigure(uint32_t serial)` | ACK configure event |

## Multi-Monitor

```cpp
// Lấy output cụ thể
QWaylandOutput *output = QWaylandCompositor::instance()
    ->outputForWindow(window);

// Tạo surface cho output đó
surface = layerShell->getLayerSurface(window, output);
```

## Best Practices Checklist

- [ ] Luôn xử lý `configureRequested` và ACK
- [ ] Kiểm tra availability trước khi dùng
- [ ] Set `exclusiveZone` đúng với chiều cao
- [ ] Cleanup surface khi destroy widget
- [ ] Test trên nhiều compositor
- [ ] Xử lý multi-monitor nếu cần
- [ ] Set `keyboardInteractivity` phù hợp

## Common Mistakes

❌ **Quên ACK configure**
```cpp
// SAI
connect(surface, &QWaylandLayerSurfaceV1::configureRequested,
        [](const QSize &size, uint32_t serial) {
            resize(size);
            // Thiếu ackConfigure!
        });
```

❌ **Commit nhiều lần không cần thiết**
```cpp
// SAI
surface->setAnchor(Qt::TopEdge);
surface->commit();
surface->setExclusiveZone(30);
surface->commit(); // Không cần!
```

✅ **Đúng**
```cpp
// ĐÚNG
surface->setAnchor(Qt::TopEdge);
surface->setExclusiveZone(30);
surface->commit(); // Chỉ commit một lần
```

## Debug Commands

```bash
# Kiểm tra Wayland session
echo $XDG_SESSION_TYPE

# Kiểm tra compositor
swaymsg -t get_version        # Sway
hyprctl version               # Hyprland

# Kiểm tra LayerShellQt
pkg-config --modversion layershellqt

# Debug Qt Wayland
QT_LOGGING_RULES="qt.wayland*=true" ./your_app
```
