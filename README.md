# LayerShellQt - Hướng Dẫn Học Toàn Diện

## 📚 Mục Lục

1. [Giới Thiệu](#giới-thiệu)
2. [Khái Niệm Cơ Bản](#khái-niệm-cơ-bản)
3. [Cài Đặt](#cài-đặt)
4. [Kiến Trúc](#kiến-trúc)
5. [Tài Liệu Chi Tiết](#tài-liệu-chi-tiết)

---

## Giới Thiệu

**LayerShellQt** là một thư viện Qt cung cấp bindings cho **wlr-layer-shell protocol** - một giao thức Wayland dùng để tạo các thành phần giao diện desktop như panels, docks, wallpapers, notifications, và các overlay UI khác.

### Tại Sao Cần LayerShellQt?

**Vấn đề:** Trong Wayland, không thể sử dụng X11 tricks để tạo window luôn ở trên cùng, trong suốt, hoặc gắn vào các vị trí cụ thể trên màn hình.

**Giải pháp:** Layer Shell Protocol cho phép:
- Đặt window vào các **layers** cụ thể (background, bottom, top, overlay)
- Xác định vị trí **anchor** (trên, dưới, trái, phải, góc)
- Thiết lập **exclusive zones** (vùng không bị các window khác che)
- Điều khiển keyboard interactivity

### Ứng Dụng Thực Tế

- **Desktop Panels** (taskbar, menu bar) - KDE Plasma
- **Docks** (application launcher)
- **Wallpapers** (background layer)
- **Notifications** (overlay layer)
- **On-screen keyboards**
- **Screen lockers**
- **System overlays**

---

## Khái Niệm Cơ Bản

### 1. Layer System (Hệ Thống Lớp)

Wayland compositor quản lý các window trong **4 layers** từ dưới lên trên:

```
┌────────────────────────────────────┐
│   OVERLAY Layer                    │  ← Trên cùng (screen keyboard, critical notifications)
├────────────────────────────────────┤
│   TOP Layer                        │  ← Panels, docks, notifications
├────────────────────────────────────┤
│   BOTTOM Layer                     │  ← Widgets, always-below windows
├────────────────────────────────────┤
│   BACKGROUND Layer                 │  ← Wallpapers
└────────────────────────────────────┘
```

**Chi tiết:**

- **`LayerShellQt::Window::LayerBackground`**: Wallpaper, desktop background
- **`LayerShellQt::Window::LayerBottom`**: Desktop widgets, below normal windows
- **`LayerShellQt::Window::LayerTop`**: Panels, docks, above normal windows
- **`LayerShellQt::Window::LayerOverlay`**: Screen keyboards, lock screens, critical UI

### 2. Anchors (Neo/Gắn)

Anchor xác định window gắn vào **cạnh hoặc góc** nào của màn hình:

```
┌─────────────────────────────────┐
│ TopLeft    Top     TopRight     │
│                                 │
│ Left               Right        │
│                                 │
│ BottomLeft Bottom  BottomRight  │
└─────────────────────────────────┘
```

**Cách hoạt động:**
- Anchor = `Top`: Window gắn vào cạnh trên, kéo dài theo chiều ngang
- Anchor = `Top | Left`: Window gắn vào góc trên-trái
- Anchor = `Top | Bottom`: Window kéo dài toàn bộ chiều dọc
- Anchor = `Top | Bottom | Left | Right`: Window phủ toàn màn hình

**Code:**
```cpp
window->setAnchors(LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight);
```

### 3. Exclusive Zone (Vùng Độc Quyền)

**Exclusive zone** là vùng mà **normal windows** (ứng dụng thông thường) **không thể xâm phạm**.

**Ví dụ:** Panel ở trên cùng cao 30px với exclusive zone = 30:
```
┌─────────────────────────────────┐
│    Panel (30px)                 │  ← Exclusive zone
├─────────────────────────────────┤
│                                 │
│  Normal Application Windows     │  ← Không thể vào vùng panel
│  (Automatically positioned      │
│   below the panel)              │
│                                 │
└─────────────────────────────────┘
```

**Giá trị:**
- `> 0`: Kích thước exclusive zone (pixels)
- `0`: Không có exclusive zone (normal windows có thể đè lên)
- `-1`: Tự động tính (compositor quyết định)

**Code:**
```cpp
window->setExclusiveZone(30); // Panel chiếm 30px, app windows không vào được
```

### 4. Keyboard Interactivity (Tương Tác Bàn Phím)

Xác định window có nhận **keyboard input** không:

- **`KeyboardInteractivityNone`**: Không nhận keyboard (wallpaper)
- **`KeyboardInteractivityExclusive`**: Nhận keyboard khi focus (panel với text input)
- **`KeyboardInteractivityOnDemand`**: Nhận keyboard khi user tương tác (Qt 5.24+)

**Code:**
```cpp
window->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityExclusive);
```

### 5. Margins (Lề)

Khoảng cách giữa window và anchor edges:

```
┌─────────────────────────────────┐
│ Screen Edge                     │
│     ↓ (margin top = 10px)       │
│   ┌───────────────────────┐     │
│   │   Window              │     │
│   └───────────────────────┘     │
└─────────────────────────────────┘
```

**Code:**
```cpp
window->setMargins({10, 20, 10, 20}); // top, right, bottom, left
```

---

## Cài Đặt

### Dependencies

```bash
# Ubuntu/Debian
sudo apt install libkf5waylandclient-dev qt5-wayland qtbase5-dev cmake

# Fedora
sudo dnf install kf5-kwayland-devel qt5-qtwayland-devel qt5-qtbase-devel cmake

# Arch Linux
sudo pacman -S layer-shell-qt qt5-wayland cmake
```

### Build từ Source

```bash
git clone https://invent.kde.org/plasma/layer-shell-qt.git
cd layer-shell-qt
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

### CMake Integration

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyLayerShellApp)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(LayerShellQt REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets
    LayerShellQt::Interface
)
```

---

## Kiến Trúc

### Class Hierarchy

```
LayerShellQt::Shell (Singleton)
    └─ Quản lý connection với Wayland compositor
    └─ Factory để tạo LayerShellQt::Window

LayerShellQt::Window
    └─ Wrapper cho QWindow
    └─ Cấu hình layer shell properties
    └─ Liên kết với wl_surface

QWindow (Qt Native)
    └─ Native Wayland surface
    └─ Rendering context
```

### Workflow Cơ Bản

1. **Tạo QWindow** (hoặc QQuickView, QWidget với windowHandle)
2. **Get LayerShellQt::Window** từ Shell
3. **Cấu hình properties** (layer, anchors, exclusive zone, etc.)
4. **Show window**

```cpp
// 1. Tạo QWindow
QQuickView *view = new QQuickView();

// 2. Get LayerShellQt::Window
LayerShellQt::Window *layerWindow = LayerShellQt::Window::get(view);

// 3. Cấu hình
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(LayerShellQt::Window::AnchorTop);
layerWindow->setExclusiveZone(30);

// 4. Show
view->show();
```

---

## Tài Liệu Chi Tiết

Để học sâu hơn, xem các tài liệu sau:

- **[01-core-concepts.md](./docs/01-core-concepts.md)** - Khái niệm core chi tiết
- **[02-api-reference.md](./docs/02-api-reference.md)** - API Reference đầy đủ
- **[03-examples.md](./docs/03-examples.md)** - Examples thực tế với code
- **[04-advanced-topics.md](./docs/04-advanced-topics.md)** - Multi-monitor, margins, scope
- **[05-best-practices.md](./docs/05-best-practices.md)** - Best practices & patterns
- **[06-quick-reference.md](./docs/06-quick-reference.md)** - Cheatsheet nhanh

### Examples Code

Xem folder `examples/` cho các ví dụ hoàn chỉnh:
- **panel/** - Top panel với clock
- **dock/** - Bottom dock
- **wallpaper/** - Animated wallpaper
- **notification/** - Notification overlay
- **widget/** - Desktop widget

---

## Quick Start: Tạo Panel Đơn Giản

### main.cpp

```cpp
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Tạo widget
    QWidget *panel = new QWidget();
    panel->setFixedHeight(30);
    panel->setStyleSheet("background-color: #2d2d2d; color: white;");
    
    QHBoxLayout *layout = new QHBoxLayout(panel);
    layout->addWidget(new QLabel("My Panel"));
    layout->addStretch();
    layout->addWidget(new QLabel("12:34"));
    
    // Cấu hình layer shell
    auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(LayerShellQt::Window::AnchorTop | 
                           LayerShellQt::Window::AnchorLeft | 
                           LayerShellQt::Window::AnchorRight);
    layerWindow->setExclusiveZone(30);
    
    panel->show();
    return app.exec();
}
```

### Compile & Run

```bash
g++ main.cpp -o panel \
    $(pkg-config --cflags --libs Qt5Widgets LayerShellQtInterface) \
    -fPIC

# Chạy trên Wayland
./panel
```

---

## Learning Path (Lộ Trình Học)

### 🚀 Level 1 - Beginner (1-2 ngày)
1. Đọc "Giới Thiệu" và "Khái Niệm Cơ Bản" (README này)
2. Chạy example "panel" và "wallpaper"
3. Thử thay đổi layer, anchors, exclusive zone
4. Đọc `docs/01-core-concepts.md`

### 🎯 Level 2 - Intermediate (3-5 ngày)
1. Đọc `docs/02-api-reference.md` - Hiểu toàn bộ API
2. Implement một dock hoặc notification system
3. Thử nghiệm với keyboard interactivity
4. Đọc `docs/03-examples.md` và implement 2-3 examples

### 🔥 Level 3 - Advanced (1-2 tuần)
1. Multi-monitor support
2. Dynamic reconfiguration (respond to screen changes)
3. Integration với QML
4. Đọc `docs/04-advanced-topics.md` và `docs/05-best-practices.md`
5. Đọc source code của KDE Plasma panels

---

## Resources

### Official
- **KDE Invent**: https://invent.kde.org/plasma/layer-shell-qt
- **API Docs**: https://api.kde.org/plasma/layer-shell-qt/html/

### Protocol Specs
- **wlr-layer-shell**: https://wayland.app/protocols/wlr-layer-shell-unstable-v1
- **Wayland Book**: https://wayland-book.com/

### Example Projects
- **KDE Plasma Desktop**: Uses LayerShellQt for panels
- **LXQt Wayland Panel**: Example implementation

---

## FAQ

**Q: LayerShellQt có hoạt động trên X11 không?**
A: Không. LayerShellQt chỉ cho Wayland. Trên X11, Qt window flags vẫn hoạt động.

**Q: Mọi compositor Wayland đều support không?**
A: Không. Cần compositor hỗ trợ wlr-layer-shell (KDE Plasma, Sway, Hyprland, etc.). GNOME không support.

**Q: Làm sao kiểm tra support?**
```cpp
if (!LayerShellQt::Shell::isAvailable()) {
    qWarning() << "Layer shell not available!";
}
```

**Q: LayerShellQt vs Qt Window Flags?**
- Qt Window Flags: X11-centric, không reliable trên Wayland
- LayerShellQt: Native Wayland, precise control, compositor integration

---

## Next Steps

### 🎓 Lộ Trình Học Chi Tiết

**→ Đọc [LEARNING-GUIDE.md](./LEARNING-GUIDE.md) để có lộ trình học chi tiết từng bước!**

### Quick Start

Bắt đầu với:
1. **Đọc** `docs/01-core-concepts.md` để hiểu sâu concepts
2. **Chạy** examples trong `examples/` folder
3. **Implement** một panel hoặc widget đơn giản
4. **Tham khảo** `docs/02-api-reference.md` khi cần API details

---

## 📂 Cấu Trúc Repository

```
/workspace/
├── README.md                      # ← Bạn đang đọc (Overview & Quick Start)
├── LEARNING-GUIDE.md              # ← Lộ trình học chi tiết
├── docs/
│   ├── 01-core-concepts.md        # Khái niệm core (45-60 phút)
│   ├── 02-api-reference.md        # API reference đầy đủ (30-45 phút)
│   ├── 03-examples.md             # 10 examples với code (1-2 giờ)
│   ├── 04-advanced-topics.md      # Advanced topics (45-60 phút)
│   ├── 05-best-practices.md       # Best practices & patterns (30-45 phút)
│   └── 06-quick-reference.md      # Cheatsheet nhanh (reference)
└── examples/
    ├── 01-simple-panel.cpp        # Top panel cơ bản
    ├── 02-notification.cpp        # Notification overlay
    ├── 03-desktop-widget.cpp      # Desktop widget
    ├── 04-animated-wallpaper.cpp  # Animated wallpaper
    ├── 05-multi-monitor-panel.cpp # Multi-monitor panel
    ├── CMakeLists.txt             # CMake build file
    ├── build.sh                   # Shell build script
    └── README.md                  # Examples documentation
```

---

## 🎯 Học Theo Level

### Level 1 - Beginner (1-2 ngày)
1. ✅ README.md (overview này)
2. ✅ docs/01-core-concepts.md
3. ✅ Build & run examples/01-simple-panel.cpp
4. ✅ Modify example, thử các anchor/layer khác

### Level 2 - Intermediate (3-5 ngày)
1. ✅ docs/02-api-reference.md
2. ✅ docs/03-examples.md
3. ✅ Build tất cả examples
4. ✅ Implement 2-3 projects nhỏ riêng

### Level 3 - Advanced (1-2 tuần)
1. ✅ docs/04-advanced-topics.md
2. ✅ docs/05-best-practices.md
3. ✅ Build complete panel system
4. ✅ Apply best practices, testing

---

Happy coding! 🚀
