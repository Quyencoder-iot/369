# Hướng Dẫn Học LayerShellQt - Từ Cơ Bản Đến Nâng Cao

## Mục Lục
1. [Giới Thiệu](#giới-thiệu)
2. [Khái Niệm Cơ Bản](#khái-niệm-cơ-bản)
3. [Kiến Trúc và Cách Hoạt Động](#kiến-trúc-và-cách-hoạt-động)
4. [Cài Đặt và Thiết Lập](#cài-đặt-và-thiết-lập)
5. [API và Các Lớp Chính](#api-và-các-lớp-chính)
6. [Ví Dụ Thực Tế](#ví-dụ-thực-tế)
7. [Best Practices](#best-practices)
8. [Troubleshooting](#troubleshooting)
9. [Tài Liệu Tham Khảo](#tài-liệu-tham-khảo)

---

## Giới Thiệu

### LayerShellQt là gì?

**LayerShellQt** là một thư viện Qt binding cho **wlr-layer-shell** protocol, một phần của Wayland compositor ecosystem. Nó cho phép các ứng dụng Qt tạo ra các cửa sổ "layer" - các cửa sổ có thể được đặt ở các lớp khác nhau trên màn hình, không tuân theo quy tắc quản lý cửa sổ thông thường.

### Tại sao cần LayerShellQt?

- **Panel/Taskbar**: Tạo các thanh công cụ luôn hiển thị ở trên cùng hoặc dưới cùng màn hình
- **Notifications**: Hiển thị thông báo overlay
- **Screen Overlays**: Tạo các lớp phủ màn hình (ví dụ: lock screen, screen saver)
- **Desktop Widgets**: Tạo các widget desktop không bị quản lý bởi window manager
- **Status Bars**: Tạo thanh trạng thái hệ thống

### LayerShell vs Window Manager

| Đặc điểm | Window Manager | Layer Shell |
|----------|---------------|-------------|
| Quản lý | Window manager quản lý | Compositor quản lý trực tiếp |
| Vị trí | Có thể di chuyển | Cố định theo layer |
| Z-order | Theo quy tắc WM | Theo layer được chỉ định |
| Tương tác | Đầy đủ | Có thể giới hạn |

---

## Khái Niệm Cơ Bản

### 1. Layers (Lớp)

LayerShellQt định nghĩa 4 lớp chính, từ dưới lên trên:

```
┌─────────────────────────────────┐
│   Overlay Layer (cao nhất)      │  ← Notifications, popups
├─────────────────────────────────┤
│   Top Layer                     │  ← Panels, bars
├─────────────────────────────────┤
│   Bottom Layer                  │  ← Dock, taskbar
├─────────────────────────────────┤
│   Background Layer (thấp nhất)  │  ← Wallpapers, desktop
└─────────────────────────────────┘
```

**Các lớp trong Qt:**
- `QWaylandLayerShellV1::anchor_top`
- `QWaylandLayerShellV1::anchor_bottom`
- `QWaylandLayerShellV1::anchor_left`
- `QWaylandLayerShellV1::anchor_right`

### 2. Anchors (Điểm Neo)

Anchors xác định cách cửa sổ được gắn vào các cạnh màn hình:

```cpp
// Neo vào cạnh trên
layerSurface->setAnchor(Qt::TopEdge);

// Neo vào nhiều cạnh
layerSurface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
```

### 3. Keyboard Interactivity

Xác định cách cửa sổ tương tác với bàn phím:

- **None**: Không nhận input từ bàn phím
- **Exclusive**: Nhận tất cả input từ bàn phím
- **OnDemand**: Nhận input khi được focus

### 4. Exclusive Zone

Vùng mà layer surface chiếm dụng, các cửa sổ thông thường sẽ không được đặt vào vùng này.

---

## Kiến Trúc và Cách Hoạt Động

### Kiến Trúc Tổng Quan

```
┌─────────────────────────────────────────────┐
│         Qt Application                      │
│  ┌──────────────────────────────────────┐  │
│  │    LayerShellQt API                  │  │
│  │  (QWaylandLayerShellV1, etc.)        │  │
│  └──────────────┬───────────────────────┘  │
└─────────────────┼─────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────┐
│      Wayland Protocol Layer                │
│  (wlr-layer-shell protocol)                │
└──────────────┬──────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────┐
│      Wayland Compositor                     │
│  (Sway, Hyprland, wlroots-based)           │
└─────────────────────────────────────────────┘
```

### Quy Trình Hoạt Động

1. **Khởi Tạo**: Ứng dụng Qt kết nối với Wayland compositor
2. **Tạo Layer Surface**: Tạo một layer surface với các thuộc tính (layer, anchor, size)
3. **Commit**: Gửi yêu cầu đến compositor
4. **Compositor Xử Lý**: Compositor đặt cửa sổ vào layer tương ứng
5. **Render**: Qt render nội dung vào surface
6. **Update**: Có thể cập nhật thuộc tính và commit lại

### Wayland Protocol Flow

```
Client                    Compositor
  │                          │
  │─── get_layer_surface───>│
  │<── layer_surface ───────│
  │                          │
  │─── set_anchor ──────────>│
  │─── set_size ────────────>│
  │─── set_exclusive_zone ──>│
  │─── commit ─────────────>│
  │                          │
  │<── configure ────────────│
  │─── ack_configure ────────>│
  │                          │
```

---

## Cài Đặt và Thiết Lập

### Yêu Cầu Hệ Thống

- Qt 5.15+ hoặc Qt 6.x
- Wayland compositor hỗ trợ wlr-layer-shell (Sway, Hyprland, wlroots-based)
- CMake 3.16+ hoặc qmake

### Cài Đặt từ Source

```bash
# Clone repository
git clone https://github.com/sedwards2009/layershellqt.git
cd layershellqt

# Build với CMake
mkdir build && cd build
cmake ..
make
sudo make install

# Hoặc với qmake
qmake
make
sudo make install
```

### Thêm vào Project Qt

**CMakeLists.txt:**
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Wayland)
find_package(LayerShellQt REQUIRED)

target_link_libraries(your_app
    Qt6::Core
    Qt6::Gui
    Qt6::Wayland
    LayerShellQt::LayerShellQt
)
```

**qmake (.pro):**
```qmake
QT += core gui wayland
CONFIG += link_pkgconfig
PKGCONFIG += layershellqt
```

---

## API và Các Lớp Chính

### QWaylandLayerShellV1

Lớp chính để tương tác với layer shell protocol.

**Các phương thức quan trọng:**

```cpp
class QWaylandLayerShellV1 {
public:
    // Tạo layer surface từ một QWindow
    QWaylandLayerSurfaceV1 *getLayerSurface(
        QWindow *window,
        QWaylandOutput *output = nullptr
    );
    
    // Lấy layer shell interface
    static QWaylandLayerShellV1 *instance();
};
```

### QWaylandLayerSurfaceV1

Đại diện cho một layer surface, quản lý các thuộc tính của nó.

**Thuộc tính chính:**

```cpp
class QWaylandLayerSurfaceV1 : public QObject {
    Q_OBJECT
    
public:
    // Thiết lập layer
    void setLayer(Layer layer);
    // Layer: Background, Bottom, Top, Overlay
    
    // Thiết lập anchor (điểm neo)
    void setAnchor(Qt::Edges edges);
    // Qt::TopEdge, Qt::BottomEdge, Qt::LeftEdge, Qt::RightEdge
    
    // Thiết lập kích thước
    void setSize(const QSize &size);
    
    // Thiết lập exclusive zone
    void setExclusiveZone(int32_t zone);
    
    // Thiết lập margin (khoảng cách từ cạnh)
    void setMargin(int32_t top, int32_t right, 
                   int32_t bottom, int32_t left);
    
    // Thiết lập keyboard interactivity
    void setKeyboardInteractivity(KeyboardInteractivity interactivity);
    
    // Commit các thay đổi
    void commit();
    
signals:
    // Khi compositor gửi configure event
    void configureRequested(const QSize &size, uint32_t serial);
    
    // Khi surface bị đóng
    void closed();
};
```

### Enums và Constants

```cpp
namespace QWaylandLayerShellV1 {
    enum Layer {
        BackgroundLayer = 0,
        BottomLayer = 1,
        TopLayer = 2,
        OverlayLayer = 3
    };
    
    enum KeyboardInteractivity {
        None = 0,
        Exclusive = 1,
        OnDemand = 2
    };
}
```

---

## Ví Dụ Thực Tế

### Ví Dụ 1: Tạo Top Panel (Thanh Công Cụ Trên)

```cpp
#include <QApplication>
#include <QWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QWaylandLayerShellV1>
#include <QWaylandLayerSurfaceV1>

class TopPanel : public QWidget {
    Q_OBJECT
    
public:
    TopPanel(QWidget *parent = nullptr) : QWidget(parent) {
        // Thiết lập cửa sổ
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Tạo layout
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 5, 10, 5);
        
        // Thêm nội dung
        auto *label = new QLabel("Top Panel", this);
        layout->addWidget(label);
        
        // Thiết lập LayerShell
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        // Lấy instance của LayerShell
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) {
            qWarning() << "LayerShell not available";
            return;
        }
        
        // Lấy QWindow từ widget
        QWindow *window = windowHandle();
        if (!window) {
            qWarning() << "Window handle not available";
            return;
        }
        
        // Tạo layer surface
        layerSurface = layerShell->getLayerSurface(window);
        if (!layerSurface) {
            qWarning() << "Failed to create layer surface";
            return;
        }
        
        // Thiết lập thuộc tính
        layerSurface->setLayer(QWaylandLayerShellV1::TopLayer);
        layerSurface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
        layerSurface->setExclusiveZone(30); // 30px chiều cao
        layerSurface->setKeyboardInteractivity(
            QWaylandLayerShellV1::OnDemand
        );
        
        // Kết nối signals
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, &TopPanel::onConfigure);
        
        // Commit
        layerSurface->commit();
        
        // Thiết lập kích thước ban đầu
        resize(1920, 30); // Giả sử màn hình 1920px
    }
    
    void onConfigure(const QSize &size, uint32_t serial) {
        // Cập nhật kích thước theo yêu cầu của compositor
        resize(size);
        
        // ACK configure
        if (layerSurface) {
            layerSurface->ackConfigure(serial);
        }
    }
    
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Đảm bảo sử dụng Wayland
    qputenv("QT_QPA_PLATFORM", "wayland");
    
    TopPanel panel;
    panel.show();
    
    return app.exec();
}
```

### Ví Dụ 2: Tạo Notification Overlay

```cpp
class NotificationWindow : public QWidget {
    Q_OBJECT
    
public:
    NotificationWindow(const QString &message, QWidget *parent = nullptr)
        : QWidget(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Layout
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(20, 20, 20, 20);
        
        auto *label = new QLabel(message, this);
        label->setStyleSheet("background: rgba(0,0,0,200); color: white; "
                            "padding: 10px; border-radius: 5px;");
        layout->addWidget(label);
        
        setupLayerShell();
        
        // Auto-close sau 3 giây
        QTimer::singleShot(3000, this, &QWidget::close);
    }
    
private:
    void setupLayerShell() {
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) return;
        
        QWindow *window = windowHandle();
        if (!window) return;
        
        layerSurface = layerShell->getLayerSurface(window);
        if (!layerSurface) return;
        
        // Overlay layer ở trên cùng
        layerSurface->setLayer(QWaylandLayerShellV1::OverlayLayer);
        
        // Neo vào góc trên bên phải
        layerSurface->setAnchor(Qt::TopEdge | Qt::RightEdge);
        
        // Không chiếm exclusive zone
        layerSurface->setExclusiveZone(0);
        
        // Không nhận keyboard input
        layerSurface->setKeyboardInteractivity(
            QWaylandLayerShellV1::None
        );
        
        // Margin từ cạnh
        layerSurface->setMargin(20, 20, 0, 0);
        
        layerSurface->commit();
        
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, [this](const QSize &size, uint32_t serial) {
                    resize(size);
                    layerSurface->ackConfigure(serial);
                });
        
        resize(300, 100);
    }
    
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};
```

### Ví Dụ 3: Tạo Bottom Dock

```cpp
class BottomDock : public QWidget {
    Q_OBJECT
    
public:
    BottomDock(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        auto *layout = new QHBoxLayout(this);
        layout->setSpacing(10);
        layout->setContentsMargins(10, 5, 10, 5);
        
        // Thêm các icon dock
        for (int i = 0; i < 5; ++i) {
            auto *button = new QPushButton(QString("App %1").arg(i+1), this);
            button->setFixedSize(50, 50);
            layout->addWidget(button);
        }
        
        layout->addStretch();
        
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) return;
        
        QWindow *window = windowHandle();
        if (!window) return;
        
        layerSurface = layerShell->getLayerSurface(window);
        if (!layerSurface) return;
        
        // Bottom layer
        layerSurface->setLayer(QWaylandLayerShellV1::BottomLayer);
        
        // Neo vào cạnh dưới
        layerSurface->setAnchor(Qt::BottomEdge | Qt::LeftEdge | Qt::RightEdge);
        
        // Chiếm 60px chiều cao
        layerSurface->setExclusiveZone(60);
        
        // Có thể nhận keyboard input khi focus
        layerSurface->setKeyboardInteractivity(
            QWaylandLayerShellV1::OnDemand
        );
        
        layerSurface->commit();
        
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, [this](const QSize &size, uint32_t serial) {
                    resize(size.width(), 60); // Giữ chiều cao cố định
                    layerSurface->ackConfigure(serial);
                });
        
        resize(1920, 60);
    }
    
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};
```

### Ví Dụ 4: Multi-Monitor Support

```cpp
class MultiMonitorPanel : public QWidget {
    Q_OBJECT
    
public:
    MultiMonitorPanel(QWidget *parent = nullptr) : QWidget(parent) {
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) return;
        
        QWindow *window = windowHandle();
        if (!window) return;
        
        // Lấy output hiện tại (màn hình)
        QWaylandOutput *output = QWaylandCompositor::instance()
            ->outputForWindow(window);
        
        // Tạo layer surface cho output cụ thể
        layerSurface = layerShell->getLayerSurface(window, output);
        
        if (!layerSurface) return;
        
        layerSurface->setLayer(QWaylandLayerShellV1::TopLayer);
        layerSurface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
        layerSurface->setExclusiveZone(30);
        layerSurface->commit();
        
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, [this](const QSize &size, uint32_t serial) {
                    resize(size);
                    layerSurface->ackConfigure(serial);
                });
    }
    
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};
```

---

## Best Practices

### 1. Luôn Xử Lý Configure Events

```cpp
// ✅ ĐÚNG
connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
        this, [this](const QSize &size, uint32_t serial) {
            resize(size);
            layerSurface->ackConfigure(serial); // Quan trọng!
        });

// ❌ SAI - Không ACK configure
connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
        this, [this](const QSize &size, uint32_t serial) {
            resize(size);
            // Thiếu ackConfigure!
        });
```

### 2. Kiểm Tra Availability

```cpp
void setupLayerShell() {
    // Kiểm tra platform
    if (QGuiApplication::platformName() != "wayland") {
        qWarning() << "Not running on Wayland!";
        return;
    }
    
    // Kiểm tra LayerShell availability
    auto *layerShell = QWaylandLayerShellV1::instance();
    if (!layerShell) {
        qWarning() << "LayerShell not available";
        return;
    }
    
    // Kiểm tra window handle
    QWindow *window = windowHandle();
    if (!window) {
        qWarning() << "Window handle not available";
        return;
    }
}
```

### 3. Quản Lý Vòng Đời

```cpp
class LayerShellWidget : public QWidget {
    Q_OBJECT
    
public:
    ~LayerShellWidget() {
        // Cleanup layer surface
        if (layerSurface) {
            layerSurface->deleteLater();
        }
    }
    
private:
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};
```

### 4. Xử Lý Lỗi

```cpp
void setupLayerShell() {
    auto *layerShell = QWaylandLayerShellV1::instance();
    if (!layerShell) {
        // Fallback: sử dụng window manager thông thường
        setWindowFlags(Qt::WindowStaysOnTopHint);
        return;
    }
    
    layerSurface = layerShell->getLayerSurface(windowHandle());
    if (!layerSurface) {
        qCritical() << "Failed to create layer surface";
        // Fallback behavior
        return;
    }
    
    connect(layerSurface, &QWaylandLayerSurfaceV1::closed,
            this, &QWidget::close);
}
```

### 5. Performance Optimization

```cpp
// Batch các thay đổi trước khi commit
void updateLayerProperties() {
    layerSurface->setAnchor(Qt::TopEdge);
    layerSurface->setExclusiveZone(30);
    layerSurface->setMargin(0, 0, 0, 0);
    
    // Chỉ commit một lần sau tất cả thay đổi
    layerSurface->commit();
}

// Tránh commit nhiều lần không cần thiết
void badUpdate() {
    layerSurface->setAnchor(Qt::TopEdge);
    layerSurface->commit(); // ❌ Không cần thiết
    
    layerSurface->setExclusiveZone(30);
    layerSurface->commit(); // ❌ Không cần thiết
}
```

### 6. Responsive Design

```cpp
void onConfigure(const QSize &size, uint32_t serial) {
    // Lưu kích thước được yêu cầu
    requestedSize = size;
    
    // Cập nhật layout nếu cần
    updateLayoutForSize(size);
    
    // Resize và ACK
    resize(size);
    layerSurface->ackConfigure(serial);
}
```

---

## Troubleshooting

### Vấn Đề 1: LayerShell không khả dụng

**Triệu chứng:**
```cpp
auto *layerShell = QWaylandLayerShellV1::instance();
// layerShell == nullptr
```

**Giải pháp:**
1. Kiểm tra compositor có hỗ trợ wlr-layer-shell:
   ```bash
   # Kiểm tra với Sway
   swaymsg -t get_version
   
   # Kiểm tra với Hyprland
   hyprctl version
   ```

2. Đảm bảo đang chạy trên Wayland:
   ```cpp
   qputenv("QT_QPA_PLATFORM", "wayland");
   ```

3. Kiểm tra library đã được cài đặt:
   ```bash
   pkg-config --modversion layershellqt
   ```

### Vấn Đề 2: Cửa sổ không hiển thị

**Triệu chứng:** Cửa sổ không xuất hiện hoặc bị ẩn

**Giải pháp:**
```cpp
// Đảm bảo window handle đã sẵn sàng
void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    
    // Setup layer shell sau khi window được show
    if (!layerSurface) {
        setupLayerShell();
    }
}

// Hoặc sử dụng QWindow::exposeEvent
void exposeEvent(QExposeEvent *event) override {
    if (event->region().isEmpty()) {
        return;
    }
    
    if (!layerSurface) {
        setupLayerShell();
    }
}
```

### Vấn Đề 3: Kích thước không đúng

**Triệu chứng:** Cửa sổ có kích thước không mong muốn

**Giải pháp:**
```cpp
// Luôn xử lý configure event
connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
        this, [this](const QSize &size, uint32_t serial) {
            // Compositor có thể yêu cầu kích thước khác
            // Điều chỉnh nếu cần
            QSize adjustedSize = size;
            if (size.height() < minimumHeight()) {
                adjustedSize.setHeight(minimumHeight());
            }
            
            resize(adjustedSize);
            layerSurface->ackConfigure(serial);
        });
```

### Vấn Đề 4: Exclusive Zone không hoạt động

**Triệu chứng:** Các cửa sổ khác vẫn hiển thị trên layer surface

**Giải pháp:**
```cpp
// Đảm bảo exclusive zone được set đúng
layerSurface->setExclusiveZone(height()); // Sử dụng chiều cao thực tế

// Đảm bảo anchor được set đúng
layerSurface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);

// Commit sau khi set
layerSurface->commit();
```

### Vấn Đề 5: Keyboard Input không hoạt động

**Triệu chứng:** Không thể nhận input từ bàn phím

**Giải pháp:**
```cpp
// Set keyboard interactivity
layerSurface->setKeyboardInteractivity(
    QWaylandLayerShellV1::Exclusive // hoặc OnDemand
);

// Đảm bảo widget có thể nhận focus
setFocusPolicy(Qt::StrongFocus);

// Request focus
setFocus();
```

### Debug Tips

```cpp
// Enable debug logging
qputenv("QT_LOGGING_RULES", "qt.wayland*=true");

// Kiểm tra layer surface state
qDebug() << "Layer:" << layerSurface->layer();
qDebug() << "Anchor:" << layerSurface->anchor();
qDebug() << "Exclusive Zone:" << layerSurface->exclusiveZone();
qDebug() << "Keyboard Interactivity:" 
         << layerSurface->keyboardInteractivity();
```

---

## Tài Liệu Tham Khảo

### Official Documentation
- **LayerShellQt GitHub**: https://github.com/sedwards2009/layershellqt
- **wlr-layer-shell Protocol**: https://github.com/swaywm/wlr-protocols
- **Qt Wayland Documentation**: https://doc.qt.io/qt-6/qtwayland-index.html

### Related Projects
- **Sway**: https://github.com/swaywm/sway
- **Hyprland**: https://github.com/hyprwm/Hyprland
- **wlroots**: https://gitlab.freedesktop.org/wlroots/wlroots

### Protocol Specifications
- **Wayland Protocol**: https://wayland.freedesktop.org/
- **wlr-layer-shell Protocol Spec**: 
  - File: `wlr-layer-shell-unstable-v1.xml`
  - Location: `/usr/share/wayland-protocols/unstable/`

### Example Projects
- **Qt Wayland Examples**: Qt installation examples
- **Sway Bar**: https://github.com/Alexays/Waybar (C++ implementation)
- **Hyprland Bar**: Various implementations in Hyprland ecosystem

---

## Kết Luận

LayerShellQt cung cấp một cách mạnh mẽ để tạo các ứng dụng Qt chạy trên các layer khác nhau trong Wayland compositor. Bằng cách hiểu rõ các khái niệm cơ bản, API, và best practices, bạn có thể tạo ra các ứng dụng như panels, notifications, và overlays một cách hiệu quả.

### Điểm Quan Trọng Cần Nhớ:

1. ✅ Luôn xử lý `configureRequested` và ACK configure
2. ✅ Kiểm tra availability trước khi sử dụng
3. ✅ Hiểu rõ sự khác biệt giữa các layers
4. ✅ Sử dụng exclusive zone đúng cách
5. ✅ Xử lý multi-monitor nếu cần
6. ✅ Test trên nhiều compositor khác nhau

### Bước Tiếp Theo:

1. Thử nghiệm với các ví dụ trên
2. Đọc source code của LayerShellQt
3. Xem cách các project khác sử dụng (Waybar, etc.)
4. Đóng góp vào cộng đồng nếu tìm thấy bug hoặc cải tiến

Chúc bạn thành công với LayerShellQt! 🚀
