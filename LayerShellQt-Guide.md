# LayerShellQt - Hướng dẫn toàn diện

## Mục lục
1. [Tổng quan](#1-tổng-quan)
2. [Kiến trúc Layer Shell](#2-kiến-trúc-layer-shell)
3. [Cài đặt và cấu hình](#3-cài-đặt-và-cấu-hình)
4. [API chi tiết](#4-api-chi-tiết)
5. [Ví dụ thực tế](#5-ví-dụ-thực-tế)
6. [Best Practices](#6-best-practices)
7. [Troubleshooting](#7-troubleshooting)

---

## 1. Tổng quan

### 1.1 LayerShellQt là gì?

**LayerShellQt** là một thư viện Qt/KDE cung cấp binding cho giao thức **wlr-layer-shell** của Wayland. Thư viện này cho phép các ứng dụng Qt tạo ra các "layer surfaces" - các bề mặt đặc biệt có thể:

- Đặt ở các vị trí cố định trên màn hình (top, bottom, left, right)
- Nằm trên hoặc dưới các cửa sổ thông thường
- Tương tác với compositor Wayland để tạo panels, docks, overlays, v.v.

### 1.2 Tại sao cần LayerShellQt?

```
┌─────────────────────────────────────────────────────────────┐
│                    WAYLAND vs X11                           │
├─────────────────────────────────────────────────────────────┤
│  X11:                                                       │
│  - Window hints (DOCK, DESKTOP, ABOVE, BELOW)              │
│  - Không có chuẩn thống nhất                               │
│  - Compositor có thể bỏ qua hints                          │
│                                                             │
│  Wayland + Layer Shell:                                     │
│  - Giao thức chuẩn hóa                                     │
│  - Đảm bảo hành vi nhất quán                               │
│  - Kiểm soát chính xác layer và anchoring                  │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 Ai sử dụng LayerShellQt?

- **KDE Plasma**: Panel, System Tray, OSD notifications
- **Latte Dock**: Dock/panel nổi tiếng
- **KRunner**: Application launcher
- **Các ứng dụng tùy chỉnh**: Widgets, overlays, bars

---

## 2. Kiến trúc Layer Shell

### 2.1 Các Layer (Tầng)

Layer Shell định nghĩa 4 tầng, theo thứ tự từ dưới lên trên:

```
┌─────────────────────────────────────────────────────────────┐
│                         OVERLAY                             │
│   (Notifications, OSD, Screen lockers, Critical alerts)    │
├─────────────────────────────────────────────────────────────┤
│                           TOP                               │
│   (Panels, Docks, Bars - phía trên cửa sổ)                 │
├─────────────────────────────────────────────────────────────┤
│                         BOTTOM                              │
│   (Panels, Docks - phía dưới cửa sổ)                       │
├─────────────────────────────────────────────────────────────┤
│                       BACKGROUND                            │
│   (Desktop widgets, Wallpaper)                              │
└─────────────────────────────────────────────────────────────┘
                              ↑
                        Normal Windows
                        (ở giữa BOTTOM và TOP)
```

### 2.2 Anchoring (Neo đậu)

Anchoring xác định cửa sổ "dính" vào cạnh nào của màn hình:

```
                    ┌── AnchorTop ──┐
                    │               │
              ┌─────┴───────────────┴─────┐
              │                           │
  AnchorLeft ─┤       Screen Area         ├─ AnchorRight
              │                           │
              └─────┬───────────────┬─────┘
                    │               │
                    └─ AnchorBottom ┘

Kết hợp anchors:
- Top + Left + Right = Thanh ngang dọc theo cạnh trên (full-width bar)
- Left + Top + Bottom = Sidebar dọc cạnh trái (full-height sidebar)
- Chỉ Top = Cửa sổ neo vào cạnh trên, kích thước tự do
- Tất cả 4 = Full-screen layer surface
```

### 2.3 Exclusive Zone

**Exclusive Zone** là vùng mà các cửa sổ khác không được phép chiếm:

```
┌────────────────────────────────────────────────────────────┐
│ Panel (exclusive zone = 32px)                              │
├────────────────────────────────────────────────────────────┤
│                                                            │
│              Vùng làm việc của ứng dụng                    │
│              (tự động điều chỉnh để tránh panel)           │
│                                                            │
│                                                            │
└────────────────────────────────────────────────────────────┘

Exclusive Zone values:
- > 0: Chiếm không gian cụ thể (pixels)
- = 0: Không chiếm không gian, nhưng vẫn hiển thị
- = -1: Bỏ qua exclusive zones của các surface khác
```

### 2.4 Keyboard Interactivity

```cpp
enum KeyboardInteractivity {
    KeyboardInteractivityNone,        // Không nhận keyboard focus
    KeyboardInteractivityExclusive,   // Chiếm toàn bộ keyboard (như screen locker)
    KeyboardInteractivityOnDemand     // Nhận focus khi được click/focus
};
```

---

## 3. Cài đặt và cấu hình

### 3.1 Dependencies

```bash
# Arch Linux
sudo pacman -S layer-shell-qt qt6-wayland

# Ubuntu/Debian (22.04+)
sudo apt install layer-shell-qt libkf6windowsystem6

# Fedora
sudo dnf install layer-shell-qt kf6-kwindowsystem

# Build from source
git clone https://invent.kde.org/plasma/layer-shell-qt.git
cd layer-shell-qt
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

### 3.2 CMakeLists.txt cho dự án của bạn

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyLayerShellApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Tìm các package cần thiết
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WaylandClient)
find_package(LayerShellQt REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    # Thêm các file khác
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WaylandClient
    LayerShellQt::Interface
)
```

### 3.3 QML Setup

```cmake
# Thêm cho QML
find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    Qt6::Qml
    LayerShellQt::Interface
)
```

---

## 4. API chi tiết

### 4.1 Namespace và Header

```cpp
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

// Namespace chính
using namespace LayerShellQt;
```

### 4.2 Lớp Shell

Singleton quản lý khởi tạo layer shell:

```cpp
// Khởi tạo layer shell (PHẢI gọi trước QGuiApplication)
LayerShellQt::Shell::useLayerShell();

// Kiểm tra layer shell có sẵn không
if (LayerShellQt::Shell::isLayerShellSupported()) {
    qDebug() << "Layer Shell được hỗ trợ!";
}
```

### 4.3 Lớp Window

Đây là class chính để cấu hình layer surface:

```cpp
class Window {
public:
    // Lấy Window wrapper từ QWindow
    static Window *get(QWindow *window);
    
    // Setters
    void setLayer(Layer layer);
    void setAnchors(Anchors anchors);
    void setExclusiveZone(int32_t zone);
    void setMargins(const QMargins &margins);
    void setKeyboardInteractivity(KeyboardInteractivity interactivity);
    void setScope(const QString &scope);
    void setScreenConfiguration(ScreenConfiguration config);
    void setDesiredOutput(QScreen *screen);
    void setCloseOnDismissed(bool close);
    
    // Getters
    Layer layer() const;
    Anchors anchors() const;
    int32_t exclusiveZone() const;
    QMargins margins() const;
    KeyboardInteractivity keyboardInteractivity() const;
    QString scope() const;
    ScreenConfiguration screenConfiguration() const;
    
signals:
    void layerChanged();
    void anchorsChanged();
    void exclusiveZoneChanged();
    void marginsChanged();
    void keyboardInteractivityChanged();
};
```

### 4.4 Enums chi tiết

```cpp
// Các layer (tầng)
enum Layer {
    LayerBackground = 0,  // Dưới cùng, như desktop wallpaper
    LayerBottom = 1,      // Dưới cửa sổ thường
    LayerTop = 2,         // Trên cửa sổ thường (mặc định cho panels)
    LayerOverlay = 3      // Trên cùng (notifications, screen lockers)
};

// Các anchor (neo)
enum Anchor {
    AnchorNone   = 0,
    AnchorTop    = 1 << 0,  // 1
    AnchorBottom = 1 << 1,  // 2
    AnchorLeft   = 1 << 2,  // 4
    AnchorRight  = 1 << 3   // 8
};
Q_DECLARE_FLAGS(Anchors, Anchor)

// Keyboard interactivity
enum KeyboardInteractivity {
    KeyboardInteractivityNone = 0,       // Không nhận bàn phím
    KeyboardInteractivityExclusive = 1,  // Độc quyền (screen locker)
    KeyboardInteractivityOnDemand = 2    // Theo yêu cầu
};

// Cấu hình màn hình
enum ScreenConfiguration {
    ScreenFromQWindow = 0,    // Sử dụng screen từ QWindow
    ScreenFromCompositor = 1  // Để compositor quyết định
};
```

### 4.5 Flow điển hình

```
┌─────────────────────────────────────────────────────────────┐
│  1. Shell::useLayerShell()                                  │
│       ↓                                                     │
│  2. QGuiApplication app(argc, argv)                         │
│       ↓                                                     │
│  3. Tạo QWindow hoặc QQuickView                             │
│       ↓                                                     │
│  4. Window *layerWindow = Window::get(window)               │
│       ↓                                                     │
│  5. Cấu hình: setLayer(), setAnchors(), setExclusiveZone()  │
│       ↓                                                     │
│  6. window->show()                                          │
│       ↓                                                     │
│  7. app.exec()                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. Ví dụ thực tế

### 5.1 Ví dụ 1: Panel cơ bản (Top Bar)

```cpp
// main.cpp - Top Panel đơn giản
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QTimer>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class TopPanel : public QWidget {
    Q_OBJECT
public:
    TopPanel(QWidget *parent = nullptr) : QWidget(parent) {
        // Thiết lập UI
        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 5, 10, 5);
        
        auto titleLabel = new QLabel("My Panel");
        titleLabel->setStyleSheet("font-weight: bold; color: white;");
        
        m_timeLabel = new QLabel();
        m_timeLabel->setStyleSheet("color: white;");
        
        layout->addWidget(titleLabel);
        layout->addStretch();
        layout->addWidget(m_timeLabel);
        
        // Cập nhật thời gian mỗi giây
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TopPanel::updateTime);
        timer->start(1000);
        updateTime();
        
        // Style
        setStyleSheet("background-color: #2d2d2d;");
        setFixedHeight(32);
    }
    
private slots:
    void updateTime() {
        m_timeLabel->setText(QDateTime::currentDateTime()
            .toString("hh:mm:ss - dd/MM/yyyy"));
    }
    
private:
    QLabel *m_timeLabel;
};

int main(int argc, char *argv[]) {
    // QUAN TRỌNG: Phải gọi TRƯỚC QApplication
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    TopPanel panel;
    
    // Lấy layer shell window wrapper
    if (auto *lsWindow = LayerShellQt::Window::get(panel.windowHandle())) {
        // Đặt ở layer Top (trên các cửa sổ thường)
        lsWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // Neo vào cạnh trên, trải dài từ trái sang phải
        lsWindow->setAnchors(LayerShellQt::Window::AnchorTop |
                            LayerShellQt::Window::AnchorLeft |
                            LayerShellQt::Window::AnchorRight);
        
        // Chiếm 32 pixels từ trên xuống
        lsWindow->setExclusiveZone(32);
        
        // Không cần keyboard focus
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone);
    }
    
    panel.show();
    return app.exec();
}

#include "main.moc"
```

### 5.2 Ví dụ 2: Dock (Bottom Bar)

```cpp
// dock.cpp - Dock ở dưới cùng
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProcess>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class Dock : public QWidget {
    Q_OBJECT
public:
    Dock(QWidget *parent = nullptr) : QWidget(parent) {
        auto layout = new QHBoxLayout(this);
        layout->setSpacing(8);
        layout->setContentsMargins(12, 8, 12, 8);
        
        // Thêm các launcher icons
        addLauncher(layout, "📁", "dolphin");
        addLauncher(layout, "🌐", "firefox");
        addLauncher(layout, "📝", "kate");
        addLauncher(layout, "⚙️", "systemsettings");
        addLauncher(layout, "🖥️", "konsole");
        
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(30, 30, 30, 0.9);
                border-radius: 12px;
            }
            QPushButton {
                background-color: transparent;
                border: none;
                font-size: 24px;
                padding: 8px;
                min-width: 48px;
                min-height: 48px;
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.1);
                border-radius: 8px;
            }
        )");
        
        setFixedHeight(70);
    }
    
private:
    void addLauncher(QHBoxLayout *layout, const QString &icon, 
                     const QString &command) {
        auto btn = new QPushButton(icon);
        connect(btn, &QPushButton::clicked, [command]() {
            QProcess::startDetached(command, QStringList());
        });
        layout->addWidget(btn);
    }
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    Dock dock;
    dock.setAttribute(Qt::WA_TranslucentBackground);
    
    // Tạo window handle trước khi cấu hình
    dock.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(dock.windowHandle())) {
        lsWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // Chỉ neo vào cạnh dưới (dock nổi, không full-width)
        lsWindow->setAnchors(LayerShellQt::Window::AnchorBottom);
        
        // Không chiếm exclusive zone (dock nổi)
        lsWindow->setExclusiveZone(0);
        
        // Margin từ cạnh dưới
        lsWindow->setMargins(QMargins(0, 0, 0, 10));
    }
    
    dock.show();
    return app.exec();
}

#include "dock.moc"
```

### 5.3 Ví dụ 3: Sidebar (Left Panel)

```cpp
// sidebar.cpp - Sidebar bên trái
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class Sidebar : public QWidget {
public:
    Sidebar(QWidget *parent = nullptr) : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        layout->setSpacing(4);
        layout->setContentsMargins(8, 12, 8, 12);
        
        // Logo/Title
        auto logo = new QLabel("🚀");
        logo->setAlignment(Qt::AlignCenter);
        logo->setStyleSheet("font-size: 32px;");
        layout->addWidget(logo);
        
        layout->addSpacing(20);
        
        // Menu items
        const QStringList items = {"🏠", "📊", "⚙️", "❓"};
        for (const auto &item : items) {
            auto btn = new QPushButton(item);
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: transparent;
                    border: none;
                    font-size: 20px;
                    padding: 12px;
                }
                QPushButton:hover {
                    background-color: rgba(255, 255, 255, 0.1);
                    border-radius: 8px;
                }
            )");
            layout->addWidget(btn);
        }
        
        layout->addStretch();
        
        setStyleSheet("background-color: #1e1e2e;");
        setFixedWidth(60);
    }
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    Sidebar sidebar;
    sidebar.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(sidebar.windowHandle())) {
        lsWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // Neo vào cạnh trái, từ trên xuống dưới
        lsWindow->setAnchors(LayerShellQt::Window::AnchorLeft |
                            LayerShellQt::Window::AnchorTop |
                            LayerShellQt::Window::AnchorBottom);
        
        // Chiếm 60 pixels từ trái
        lsWindow->setExclusiveZone(60);
    }
    
    sidebar.show();
    return app.exec();
}

#include "sidebar.moc"
```

### 5.4 Ví dụ 4: Notification Overlay

```cpp
// notification.cpp - Notification popup
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class NotificationPopup : public QWidget {
    Q_OBJECT
public:
    NotificationPopup(const QString &title, const QString &message,
                      QWidget *parent = nullptr) 
        : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(16, 12, 16, 12);
        
        auto titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: white;");
        
        auto messageLabel = new QLabel(message);
        messageLabel->setStyleSheet("font-size: 12px; color: #cccccc;");
        messageLabel->setWordWrap(true);
        
        layout->addWidget(titleLabel);
        layout->addWidget(messageLabel);
        
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(40, 40, 50, 0.95);
                border-radius: 8px;
                border: 1px solid rgba(255, 255, 255, 0.1);
            }
        )");
        
        setFixedWidth(300);
        
        // Tự động đóng sau 5 giây
        QTimer::singleShot(5000, this, &QWidget::close);
    }
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    NotificationPopup notification(
        "📬 Thông báo mới",
        "Đây là một notification sử dụng LayerShellQt. "
        "Nó sẽ tự động biến mất sau 5 giây."
    );
    
    notification.setAttribute(Qt::WA_TranslucentBackground);
    notification.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(notification.windowHandle())) {
        // Overlay layer - trên tất cả
        lsWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        
        // Neo vào góc trên phải
        lsWindow->setAnchors(LayerShellQt::Window::AnchorTop |
                            LayerShellQt::Window::AnchorRight);
        
        // Không chiếm exclusive zone
        lsWindow->setExclusiveZone(0);
        
        // Margin từ góc
        lsWindow->setMargins(QMargins(0, 20, 20, 0));
        
        // Không cần keyboard
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone);
    }
    
    notification.show();
    return app.exec();
}

#include "notification.moc"
```

### 5.5 Ví dụ 5: Desktop Widget (Background Layer)

```cpp
// desktop_widget.cpp - Widget trên desktop
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDateTime>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class DesktopClock : public QWidget {
    Q_OBJECT
public:
    DesktopClock(QWidget *parent = nullptr) : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        
        m_timeLabel = new QLabel();
        m_timeLabel->setStyleSheet(R"(
            font-size: 72px;
            font-weight: 200;
            color: rgba(255, 255, 255, 0.8);
            font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
        )");
        m_timeLabel->setAlignment(Qt::AlignCenter);
        
        m_dateLabel = new QLabel();
        m_dateLabel->setStyleSheet(R"(
            font-size: 24px;
            color: rgba(255, 255, 255, 0.6);
        )");
        m_dateLabel->setAlignment(Qt::AlignCenter);
        
        layout->addWidget(m_timeLabel);
        layout->addWidget(m_dateLabel);
        
        setStyleSheet("background: transparent;");
        
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &DesktopClock::updateTime);
        timer->start(1000);
        updateTime();
        
        setFixedSize(400, 200);
    }
    
private slots:
    void updateTime() {
        auto now = QDateTime::currentDateTime();
        m_timeLabel->setText(now.toString("HH:mm"));
        m_dateLabel->setText(now.toString("dddd, d MMMM yyyy"));
    }
    
private:
    QLabel *m_timeLabel;
    QLabel *m_dateLabel;
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    DesktopClock clock;
    clock.setAttribute(Qt::WA_TranslucentBackground);
    clock.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(clock.windowHandle())) {
        // Background layer - dưới tất cả các cửa sổ
        lsWindow->setLayer(LayerShellQt::Window::LayerBackground);
        
        // Góc dưới phải của desktop
        lsWindow->setAnchors(LayerShellQt::Window::AnchorBottom |
                            LayerShellQt::Window::AnchorRight);
        
        lsWindow->setExclusiveZone(0);
        lsWindow->setMargins(QMargins(0, 0, 50, 50));
        
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone);
    }
    
    clock.show();
    return app.exec();
}

#include "desktop_widget.moc"
```

### 5.6 Ví dụ 6: QML với LayerShellQt

```qml
// main.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: Screen.width
    height: 40
    color: "#1a1a2e"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 16
        
        // App launcher
        Text {
            text: "🚀 Apps"
            color: "white"
            font.pixelSize: 14
            
            MouseArea {
                anchors.fill: parent
                onClicked: console.log("Open app menu")
            }
        }
        
        // Spacer
        Item { Layout.fillWidth: true }
        
        // System tray area
        Row {
            spacing: 12
            
            Text { text: "🔊"; color: "white"; font.pixelSize: 16 }
            Text { text: "📶"; color: "white"; font.pixelSize: 16 }
            Text { text: "🔋"; color: "white"; font.pixelSize: 16 }
        }
        
        // Clock
        Text {
            id: clock
            color: "white"
            font.pixelSize: 14
            
            Timer {
                interval: 1000
                running: true
                repeat: true
                triggeredOnStart: true
                onTriggered: {
                    clock.text = Qt.formatDateTime(new Date(), "HH:mm:ss")
                }
            }
        }
    }
}
```

```cpp
// main.cpp cho QML
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

int main(int argc, char *argv[]) {
    // Phải gọi trước QGuiApplication
    LayerShellQt::Shell::useLayerShell();
    
    QGuiApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    
    // Kết nối để cấu hình khi window được tạo
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        [](QObject *obj, const QUrl &) {
            if (auto *window = qobject_cast<QQuickWindow*>(obj)) {
                if (auto *lsWindow = LayerShellQt::Window::get(window)) {
                    lsWindow->setLayer(LayerShellQt::Window::LayerTop);
                    lsWindow->setAnchors(
                        LayerShellQt::Window::AnchorTop |
                        LayerShellQt::Window::AnchorLeft |
                        LayerShellQt::Window::AnchorRight
                    );
                    lsWindow->setExclusiveZone(40);
                }
            }
        });
    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    
    return app.exec();
}
```

### 5.7 Ví dụ 7: Screen Locker (Exclusive Keyboard)

```cpp
// locker.cpp - Screen locker đơn giản
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class ScreenLocker : public QWidget {
    Q_OBJECT
public:
    ScreenLocker(QWidget *parent = nullptr) : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        
        auto icon = new QLabel("🔒");
        icon->setStyleSheet("font-size: 64px;");
        icon->setAlignment(Qt::AlignCenter);
        
        auto label = new QLabel("Nhập mật khẩu để mở khóa");
        label->setStyleSheet("color: white; font-size: 18px;");
        label->setAlignment(Qt::AlignCenter);
        
        m_passwordEdit = new QLineEdit();
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        m_passwordEdit->setFixedWidth(300);
        m_passwordEdit->setStyleSheet(R"(
            QLineEdit {
                padding: 12px;
                font-size: 16px;
                border: 2px solid #444;
                border-radius: 8px;
                background: rgba(255, 255, 255, 0.1);
                color: white;
            }
            QLineEdit:focus {
                border-color: #6c5ce7;
            }
        )");
        
        auto unlockBtn = new QPushButton("Mở khóa");
        unlockBtn->setFixedWidth(300);
        unlockBtn->setStyleSheet(R"(
            QPushButton {
                padding: 12px;
                font-size: 16px;
                background-color: #6c5ce7;
                color: white;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #5b4cdb;
            }
        )");
        
        connect(unlockBtn, &QPushButton::clicked, this, &ScreenLocker::tryUnlock);
        connect(m_passwordEdit, &QLineEdit::returnPressed, 
                this, &ScreenLocker::tryUnlock);
        
        layout->addWidget(icon);
        layout->addSpacing(20);
        layout->addWidget(label);
        layout->addSpacing(20);
        layout->addWidget(m_passwordEdit);
        layout->addSpacing(10);
        layout->addWidget(unlockBtn);
        
        setStyleSheet("background-color: #1a1a2e;");
    }
    
private slots:
    void tryUnlock() {
        if (m_passwordEdit->text() == "1234") { // Demo password
            close();
        } else {
            m_passwordEdit->clear();
            m_passwordEdit->setPlaceholderText("Sai mật khẩu!");
        }
    }
    
private:
    QLineEdit *m_passwordEdit;
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    ScreenLocker locker;
    locker.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(locker.windowHandle())) {
        // Overlay - trên tất cả
        lsWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        
        // Full screen
        lsWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        
        // Bỏ qua exclusive zones của các surface khác
        lsWindow->setExclusiveZone(-1);
        
        // QUAN TRỌNG: Chiếm độc quyền keyboard
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityExclusive);
    }
    
    locker.show();
    return app.exec();
}

#include "locker.moc"
```

---

## 6. Best Practices

### 6.1 Khởi tạo đúng cách

```cpp
// ✅ ĐÚNG: Gọi useLayerShell() TRƯỚC QApplication
int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();  // ĐẦU TIÊN
    QApplication app(argc, argv);
    // ...
}

// ❌ SAI: Gọi sau QApplication
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    LayerShellQt::Shell::useLayerShell();  // QUÁ MUỘN!
    // ...
}
```

### 6.2 Tạo Window Handle trước khi cấu hình

```cpp
// ✅ ĐÚNG: Đảm bảo window handle tồn tại
QWidget widget;
widget.createWinId();  // Tạo native window
auto *lsWindow = LayerShellQt::Window::get(widget.windowHandle());
if (lsWindow) {
    // Cấu hình...
}

// ❌ SAI: Truy cập windowHandle() trước khi nó được tạo
QWidget widget;
auto *lsWindow = LayerShellQt::Window::get(widget.windowHandle()); 
// windowHandle() có thể trả về nullptr!
```

### 6.3 Kiểm tra hỗ trợ Layer Shell

```cpp
// Kiểm tra trước khi sử dụng các tính năng
if (!LayerShellQt::Shell::isLayerShellSupported()) {
    qWarning() << "Layer Shell không được hỗ trợ!";
    qWarning() << "Có thể bạn đang chạy trên X11 hoặc compositor không hỗ trợ";
    // Fallback hoặc thoát
    return 1;
}
```

### 6.4 Xử lý Multi-monitor

```cpp
// Đặt surface trên một màn hình cụ thể
QScreen *targetScreen = QGuiApplication::screens().at(1); // Màn hình thứ 2

if (auto *lsWindow = LayerShellQt::Window::get(window)) {
    lsWindow->setDesiredOutput(targetScreen);
    // Hoặc để compositor quyết định
    lsWindow->setScreenConfiguration(
        LayerShellQt::Window::ScreenFromCompositor);
}
```

### 6.5 Exclusive Zone Guidelines

```
┌─────────────────────────────────────────────────────────────┐
│ Loại ứng dụng              │ Exclusive Zone               │
├─────────────────────────────────────────────────────────────┤
│ Panel cố định              │ = chiều cao/rộng của panel    │
│ Dock nổi (auto-hide)       │ = 0                          │
│ Notification               │ = 0                          │
│ Desktop widget             │ = 0                          │
│ Screen locker              │ = -1 (bỏ qua mọi thứ)        │
│ OSD (volume, brightness)   │ = 0                          │
└─────────────────────────────────────────────────────────────┘
```

### 6.6 Keyboard Interactivity Guidelines

```
┌─────────────────────────────────────────────────────────────┐
│ Loại ứng dụng              │ Keyboard Interactivity        │
├─────────────────────────────────────────────────────────────┤
│ Panel/Dock (không input)   │ None                          │
│ Notification               │ None                          │
│ Desktop widget             │ None hoặc OnDemand            │
│ Application launcher       │ OnDemand                      │
│ Screen locker              │ Exclusive                     │
│ Popup với text input       │ OnDemand                      │
└─────────────────────────────────────────────────────────────┘
```

---

## 7. Troubleshooting

### 7.1 Lỗi thường gặp

#### "Layer Shell is not supported"

```bash
# Kiểm tra Wayland session
echo $XDG_SESSION_TYPE  # Phải là "wayland"

# Kiểm tra compositor có hỗ trợ
# KWin, Sway, wlroots-based compositors thường hỗ trợ
# GNOME Mutter KHÔNG hỗ trợ wlr-layer-shell
```

#### Window không hiển thị

```cpp
// Đảm bảo:
// 1. useLayerShell() được gọi trước QApplication
// 2. Window handle tồn tại (createWinId())
// 3. show() được gọi
// 4. Kích thước được đặt (nếu cần)

widget.createWinId();
widget.setFixedSize(200, 50);  // Đặt kích thước rõ ràng
widget.show();
```

#### Window hiển thị sai vị trí

```cpp
// Kiểm tra anchors và margins
lsWindow->setAnchors(LayerShellQt::Window::AnchorTop |
                     LayerShellQt::Window::AnchorLeft);
lsWindow->setMargins(QMargins(10, 10, 0, 0));  // left, top, right, bottom
```

### 7.2 Debug Tips

```cpp
// In thông tin layer shell
if (auto *lsWindow = LayerShellQt::Window::get(window)) {
    qDebug() << "Layer:" << lsWindow->layer();
    qDebug() << "Anchors:" << lsWindow->anchors();
    qDebug() << "Exclusive zone:" << lsWindow->exclusiveZone();
    qDebug() << "Margins:" << lsWindow->margins();
}
```

### 7.3 Fallback cho X11

```cpp
#include <QGuiApplication>

int main(int argc, char *argv[]) {
    // Kiểm tra Wayland
    bool isWayland = qEnvironmentVariable("XDG_SESSION_TYPE") == "wayland";
    
    if (isWayland) {
        LayerShellQt::Shell::useLayerShell();
    }
    
    QApplication app(argc, argv);
    
    MyPanel panel;
    
    if (isWayland) {
        // Cấu hình Layer Shell
        setupLayerShell(panel.windowHandle());
    } else {
        // Fallback X11: sử dụng window flags
        panel.setWindowFlags(Qt::FramelessWindowHint | 
                            Qt::WindowStaysOnTopHint |
                            Qt::X11BypassWindowManagerHint);
    }
    
    panel.show();
    return app.exec();
}
```

---

## 8. Tài liệu tham khảo

### 8.1 Links hữu ích

- **Source code**: https://invent.kde.org/plasma/layer-shell-qt
- **API Docs**: https://api.kde.org/plasma/layer-shell-qt/html/
- **wlr-layer-shell protocol**: https://wayland.app/protocols/wlr-layer-shell-unstable-v1
- **KDE Developer docs**: https://develop.kde.org/

### 8.2 Các project tham khảo

```
┌─────────────────────────────────────────────────────────────┐
│ Project              │ Mô tả                                │
├─────────────────────────────────────────────────────────────┤
│ KDE Plasma Shell     │ Desktop shell sử dụng LayerShellQt  │
│ Latte Dock           │ Dock/Panel nâng cao                  │
│ KRunner              │ Application launcher                 │
│ plasma-workspace     │ OSD, notifications                   │
└─────────────────────────────────────────────────────────────┘
```

### 8.3 Tóm tắt nhanh (Cheat Sheet)

```cpp
// === QUICK REFERENCE ===

// 1. Khởi tạo (TRƯỚC QApplication)
LayerShellQt::Shell::useLayerShell();

// 2. Lấy window wrapper
auto *lsWindow = LayerShellQt::Window::get(window);

// 3. Các layer
LayerShellQt::Window::LayerBackground  // Dưới cùng
LayerShellQt::Window::LayerBottom      // Dưới windows
LayerShellQt::Window::LayerTop         // Trên windows
LayerShellQt::Window::LayerOverlay     // Trên cùng

// 4. Anchors (có thể combine với |)
AnchorTop | AnchorBottom | AnchorLeft | AnchorRight

// 5. Exclusive zone
setExclusiveZone(40);   // Chiếm 40px
setExclusiveZone(0);    // Không chiếm
setExclusiveZone(-1);   // Bỏ qua các exclusive zones khác

// 6. Keyboard
KeyboardInteractivityNone       // Không nhận keyboard
KeyboardInteractivityOnDemand   // Khi được click
KeyboardInteractivityExclusive  // Độc quyền keyboard

// 7. Margins
setMargins(QMargins(left, top, right, bottom));
```

---

## 9. Bài tập thực hành

### Bài 1: Tạo một top bar đơn giản
- Hiển thị thời gian
- Neo vào cạnh trên màn hình
- Chiếm 32 pixels exclusive zone

### Bài 2: Tạo một floating dock
- 5 icon launcher
- Neo vào cạnh dưới, không exclusive zone
- Có margin 10px từ bottom

### Bài 3: Desktop weather widget
- Hiển thị trên background layer
- Góc dưới trái màn hình
- Trong suốt background

### Bài 4: Application launcher overlay
- Full-screen overlay
- Keyboard interactivity on-demand
- Có thể đóng bằng Escape

---

*Tài liệu được tạo để giúp bạn nhanh chóng nắm vững LayerShellQt*
