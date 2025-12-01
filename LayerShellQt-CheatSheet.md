# LayerShellQt Cheat Sheet 🚀

## Khởi tạo nhanh

```cpp
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

int main(int argc, char *argv[]) {
    // ⚠️ PHẢI gọi TRƯỚC QApplication
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    MyWidget widget;
    widget.createWinId();  // Tạo window handle
    
    auto *lsWindow = LayerShellQt::Window::get(widget.windowHandle());
    // Cấu hình...
    
    widget.show();
    return app.exec();
}
```

---

## Các Layer (từ dưới lên)

| Layer | Enum | Sử dụng cho |
|-------|------|-------------|
| 🔽 Background | `LayerBackground` | Desktop widgets, wallpaper |
| ⬇️ Bottom | `LayerBottom` | Panels dưới cửa sổ |
| ⬆️ Top | `LayerTop` | Panels, docks (mặc định) |
| 🔼 Overlay | `LayerOverlay` | Notifications, screen locker |

```cpp
lsWindow->setLayer(LayerShellQt::Window::LayerTop);
```

---

## Anchors (Neo đậu)

```
         ┌─── AnchorTop ───┐
         │                 │
AnchorLeft       ⬛        AnchorRight
         │                 │
         └─ AnchorBottom ──┘
```

| Pattern | Kết quả |
|---------|---------|
| `Top + Left + Right` | Thanh ngang cạnh trên (full-width) |
| `Bottom + Left + Right` | Thanh ngang cạnh dưới (full-width) |
| `Left + Top + Bottom` | Sidebar trái (full-height) |
| `Chỉ Bottom` | Dock nổi ở dưới (centered) |
| `Top + Right` | Corner góc trên phải |
| `Tất cả 4` | Full-screen |

```cpp
lsWindow->setAnchors(
    LayerShellQt::Window::AnchorTop |
    LayerShellQt::Window::AnchorLeft |
    LayerShellQt::Window::AnchorRight
);
```

---

## Exclusive Zone

| Giá trị | Ý nghĩa |
|---------|---------|
| `> 0` | Chiếm N pixels (cửa sổ khác tránh) |
| `= 0` | Không chiếm (nổi trên cửa sổ) |
| `= -1` | Bỏ qua exclusive zones khác |

```cpp
// Panel cố định
lsWindow->setExclusiveZone(36);

// Dock nổi
lsWindow->setExclusiveZone(0);

// Screen locker
lsWindow->setExclusiveZone(-1);
```

---

## Keyboard Interactivity

| Mode | Enum | Sử dụng |
|------|------|---------|
| None | `KeyboardInteractivityNone` | Panel, dock, notification |
| On Demand | `KeyboardInteractivityOnDemand` | Launcher, input widgets |
| Exclusive | `KeyboardInteractivityExclusive` | Screen locker |

```cpp
lsWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityOnDemand
);
```

---

## Margins

```cpp
// QMargins(left, top, right, bottom)
lsWindow->setMargins(QMargins(10, 20, 10, 20));
```

---

## Multi-monitor

```cpp
// Chọn màn hình cụ thể
QScreen *screen = QGuiApplication::screens().at(1);
lsWindow->setDesiredOutput(screen);

// Hoặc để compositor quyết định
lsWindow->setScreenConfiguration(
    LayerShellQt::Window::ScreenFromCompositor
);
```

---

## Recipes phổ biến

### Top Panel
```cpp
setLayer(LayerTop);
setAnchors(AnchorTop | AnchorLeft | AnchorRight);
setExclusiveZone(36);
setKeyboardInteractivity(None);
```

### Floating Dock
```cpp
setLayer(LayerTop);
setAnchors(AnchorBottom);
setExclusiveZone(0);
setMargins(QMargins(0, 0, 0, 10));
```

### Sidebar
```cpp
setLayer(LayerTop);
setAnchors(AnchorLeft | AnchorTop | AnchorBottom);
setExclusiveZone(60);
```

### Notification
```cpp
setLayer(LayerOverlay);
setAnchors(AnchorTop | AnchorRight);
setExclusiveZone(0);
setMargins(QMargins(0, 20, 20, 0));
```

### Desktop Widget
```cpp
setLayer(LayerBackground);
setAnchors(AnchorBottom | AnchorRight);
setExclusiveZone(0);
setMargins(QMargins(0, 0, 40, 40));
```

### Screen Locker
```cpp
setLayer(LayerOverlay);
setAnchors(AnchorTop | AnchorBottom | AnchorLeft | AnchorRight);
setExclusiveZone(-1);
setKeyboardInteractivity(Exclusive);
```

---

## CMake

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(LayerShellQt REQUIRED)

target_link_libraries(myapp PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
    LayerShellQt::Interface
)
```

---

## Debug

```cpp
if (!LayerShellQt::Shell::isLayerShellSupported()) {
    qWarning() << "Layer Shell không được hỗ trợ!";
}

// In thông tin cấu hình
qDebug() << "Layer:" << lsWindow->layer();
qDebug() << "Anchors:" << lsWindow->anchors();
qDebug() << "Exclusive:" << lsWindow->exclusiveZone();
```

---

## ⚠️ Lưu ý quan trọng

1. **Gọi `useLayerShell()` TRƯỚC `QApplication`**
2. **Gọi `createWinId()` trước khi cấu hình**
3. **Kiểm tra `windowHandle() != nullptr`**
4. **Layer Shell chỉ hoạt động trên Wayland + wlr-layer-shell**
5. **GNOME Mutter KHÔNG hỗ trợ** (sử dụng ext-layer-shell extension)
