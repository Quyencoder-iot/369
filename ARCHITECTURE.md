# LayerShellQt - Kiến Trúc và Cách Hoạt Động

## Tổng Quan Kiến Trúc

```
┌─────────────────────────────────────────────────────────┐
│                    Qt Application                       │
│  ┌──────────────────────────────────────────────────┐  │
│  │  QWidget / QWindow                               │  │
│  │  ┌────────────────────────────────────────────┐  │  │
│  │  │  QWaylandLayerShellV1                      │  │  │
│  │  │  ┌──────────────────────────────────────┐  │  │  │
│  │  │  │  QWaylandLayerSurfaceV1             │  │  │  │
│  │  │  │  - Layer                            │  │  │  │
│  │  │  │  - Anchor                           │  │  │  │
│  │  │  │  - Exclusive Zone                   │  │  │  │
│  │  │  │  - Keyboard Interactivity            │  │  │  │
│  │  │  └──────────────────────────────────────┘  │  │  │
│  │  └────────────────────────────────────────────┘  │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────┬───────────────────────────────────┘
                      │
                      │ Qt Wayland Integration
                      │
                      ▼
┌─────────────────────────────────────────────────────────┐
│            Wayland Protocol Layer                       │
│  ┌──────────────────────────────────────────────────┐  │
│  │  wlr-layer-shell-unstable-v1                    │  │
│  │  - zwlr_layer_shell_v1                          │  │
│  │  - zwlr_layer_surface_v1                        │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────┬───────────────────────────────────┘
                      │
                      │ Wayland Protocol
                      │
                      ▼
┌─────────────────────────────────────────────────────────┐
│            Wayland Compositor                           │
│  ┌──────────────────────────────────────────────────┐  │
│  │  wlroots / Sway / Hyprland                      │  │
│  │  - Layer Management                             │  │
│  │  - Surface Compositing                          │  │
│  │  - Input Handling                               │  │
│  │  - Output Management                            │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## Luồng Hoạt Động Chi Tiết

### 1. Khởi Tạo (Initialization)

```
Application Start
    │
    ├─> QApplication::instance()
    │
    ├─> QGuiApplication::platformName() == "wayland"
    │
    ├─> QWaylandLayerShellV1::instance()
    │   │
    │   └─> Tìm layer_shell interface từ compositor
    │
    └─> Ready to create layer surfaces
```

### 2. Tạo Layer Surface

```
getLayerSurface(window, output)
    │
    ├─> Kiểm tra window handle
    │
    ├─> Tạo QWaylandLayerSurfaceV1 instance
    │
    ├─> Gửi get_layer_surface request đến compositor
    │   │
    │   └─> Wayland protocol: zwlr_layer_shell_v1.get_layer_surface()
    │
    ├─> Nhận layer_surface object từ compositor
    │
    └─> Return QWaylandLayerSurfaceV1*
```

### 3. Configure Surface

```
setLayer(TopLayer)
    │
setAnchor(TopEdge | LeftEdge | RightEdge)
    │
setExclusiveZone(30)
    │
setKeyboardInteractivity(OnDemand)
    │
commit()
    │
    ├─> Gửi các set_* requests đến compositor
    │   │
    │   ├─> zwlr_layer_surface_v1.set_layer()
    │   ├─> zwlr_layer_surface_v1.set_anchor()
    │   ├─> zwlr_layer_surface_v1.set_exclusive_zone()
    │   ├─> zwlr_layer_surface_v1.set_keyboard_interactivity()
    │   └─> zwlr_layer_surface_v1.commit()
    │
    └─> Compositor xử lý và gửi configure event
```

### 4. Configure Event Flow

```
Compositor
    │
    ├─> Tính toán kích thước và vị trí
    │   │
    │   ├─> Dựa trên anchor
    │   ├─> Dựa trên exclusive zone
    │   ├─> Dựa trên output size
    │   └─> Dựa trên các layer surface khác
    │
    └─> Gửi configure event
        │
        └─> Wayland protocol: zwlr_layer_surface_v1.configure()
            │
            ├─> size: QSize(width, height)
            └─> serial: uint32_t (sequence number)
```

### 5. ACK Configure

```
configureRequested(size, serial)
    │
    ├─> Client nhận configure event
    │
    ├─> Cập nhật kích thước cửa sổ
    │   resize(size)
    │
    ├─> Render nội dung mới
    │
    └─> ACK configure
        │
        └─> ackConfigure(serial)
            │
            └─> Wayland protocol: zwlr_layer_surface_v1.ack_configure(serial)
```

## Layer Stacking Order

```
┌─────────────────────────────────────────────┐
│  Overlay Layer (Layer 3)                   │  ← Cao nhất
│  ┌───────────────────────────────────────┐ │
│  │  Notifications                        │ │
│  │  Popups                               │ │
│  │  Lock Screen                          │ │
│  └───────────────────────────────────────┘ │
├─────────────────────────────────────────────┤
│  Top Layer (Layer 2)                       │
│  ┌───────────────────────────────────────┐ │
│  │  Status Bar                           │ │
│  │  Top Panel                            │ │
│  │  Menu Bar                              │ │
│  └───────────────────────────────────────┘ │
├─────────────────────────────────────────────┤
│  Normal Windows                             │  ← Window Manager quản lý
│  ┌───────────────────────────────────────┐ │
│  │  Applications                         │ │
│  │  Dialogs                              │ │
│  └───────────────────────────────────────┘ │
├─────────────────────────────────────────────┤
│  Bottom Layer (Layer 1)                     │
│  ┌───────────────────────────────────────┐ │
│  │  Dock                                 │ │
│  │  Taskbar                              │ │
│  │  System Tray                           │ │
│  └───────────────────────────────────────┘ │
├─────────────────────────────────────────────┤
│  Background Layer (Layer 0)                 │  ← Thấp nhất
│  ┌───────────────────────────────────────┐ │
│  │  Wallpaper                            │ │
│  │  Desktop Icons                        │ │
│  └───────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

## Exclusive Zone Mechanism

```
┌─────────────────────────────────────────────┐
│  Screen (1920x1080)                         │
│                                             │
│  ┌───────────────────────────────────────┐ │
│  │  Top Panel (exclusive_zone = 30)      │ │ ← Exclusive Zone
│  │  Height: 30px                         │ │
│  └───────────────────────────────────────┘ │
│                                             │
│  ┌───────────────────────────────────────┐ │
│  │                                       │ │
│  │  Normal Windows                       │ │ ← Không được đặt vào
│  │  (tránh exclusive zone)               │ │   exclusive zone
│  │                                       │ │
│  └───────────────────────────────────────┘ │
│                                             │
│  ┌───────────────────────────────────────┐ │
│  │  Bottom Dock (exclusive_zone = 60)    │ │ ← Exclusive Zone
│  │  Height: 60px                         │ │
│  └───────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

## Anchor System

### Single Anchor

```
TopEdge:        BottomEdge:     LeftEdge:      RightEdge:
┌─────────┐     ┌─────────┐     ┌─┐           ┌─────────┐
│ Panel   │     │         │     │ │           │         │
└─────────┘     │         │     │P│           │         │
                │         │     │a│           │         │
                │         │     │n│           │         │
                │  Dock   │     │e│           │  Panel  │
                └─────────┘     │l│           └─────────┘
                                └─┘
```

### Multiple Anchors

```
Top + Left + Right:        Bottom + Left + Right:
┌─────────────────┐        ┌─────────────────┐
│     Panel       │        │                 │
└─────────────────┘        │                 │
                           │                 │
                           │                 │
                           │      Dock       │
                           └─────────────────┘

All Edges (Full Screen):
┌─────────────────┐
│                 │
│                 │
│   Full Screen   │
│     Overlay     │
│                 │
│                 │
└─────────────────┘
```

## State Machine

```
┌─────────────┐
│   INIT      │
└──────┬──────┘
       │
       │ getLayerSurface()
       ▼
┌─────────────┐
│  CREATED    │
└──────┬──────┘
       │
       │ setLayer(), setAnchor(), etc.
       │ commit()
       ▼
┌─────────────┐
│ CONFIGURING │ ────> configure event từ compositor
└──────┬──────┘
       │
       │ ackConfigure()
       ▼
┌─────────────┐
│  CONFIGURED │ ────> Surface sẵn sàng render
└──────┬──────┘
       │
       │ closed event
       ▼
┌─────────────┐
│   CLOSED    │
└─────────────┘
```

## Memory Management

```
QWidget
    │
    ├─> QWindow (windowHandle())
    │       │
    │       └─> QWaylandLayerSurfaceV1
    │               │
    │               └─> Owned by QWindow
    │
    └─> Cleanup khi widget bị destroy
            │
            └─> QWaylandLayerSurfaceV1::deleteLater()
```

## Thread Safety

- **Qt Wayland objects**: Chỉ được truy cập từ main thread
- **LayerShellQt API**: Thread-safe trong giới hạn Qt Wayland
- **Best Practice**: Luôn sử dụng từ main thread (GUI thread)

## Performance Considerations

1. **Batch Operations**: Set nhiều thuộc tính trước khi commit
2. **Minimize Commits**: Tránh commit không cần thiết
3. **Efficient Rendering**: Sử dụng QOpenGLWidget cho animation
4. **Memory**: Cleanup surface khi không dùng

## Protocol Details

### Wayland Messages

**Client → Compositor:**
- `get_layer_surface(surface, output, layer, namespace)`
- `set_anchor(anchor)`
- `set_exclusive_zone(zone)`
- `set_margin(top, right, bottom, left)`
- `set_keyboard_interactivity(interactivity)`
- `get_popup(popup)`
- `ack_configure(serial)`
- `destroy()`

**Compositor → Client:**
- `configure(serial, width, height)`
- `closed()`

### Serial Numbers

- Mỗi configure event có một serial number duy nhất
- Client phải ACK với đúng serial number
- Serial tăng dần theo thời gian
- Dùng để đảm bảo thứ tự xử lý

## Integration với Qt

### QWidget Integration

```cpp
QWidget
    └─> QWindow (windowHandle())
            └─> QWaylandWindow
                    └─> QWaylandSurface
                            └─> QWaylandLayerSurfaceV1
```

### Signal/Slot System

LayerShellQt sử dụng Qt signal/slot để:
- Thông báo configure events
- Thông báo khi surface bị đóng
- Tích hợp với Qt event loop

## Compositor Compatibility

| Compositor | Support | Notes |
|------------|---------|-------|
| Sway | ✅ Full | Reference implementation |
| Hyprland | ✅ Full | wlroots-based |
| wlroots | ✅ Full | Base library |
| KDE Plasma | ⚠️ Partial | Có thể cần config |
| GNOME | ❌ No | Không hỗ trợ wlr-layer-shell |

## Debugging Architecture

```
Application
    │
    ├─> Qt Logging (QT_LOGGING_RULES)
    │
    ├─> Wayland Debug (WAYLAND_DEBUG=1)
    │
    └─> Compositor Logs
            │
            ├─> Sway: swaymsg -t get_tree
            └─> Hyprland: hyprctl clients
```
