# LayerShellQt - Practical Examples

## Mục Lục
1. [Example 1: Top Panel](#example-1-top-panel)
2. [Example 2: Bottom Dock](#example-2-bottom-dock)
3. [Example 3: Desktop Widget](#example-3-desktop-widget)
4. [Example 4: Wallpaper](#example-4-wallpaper)
5. [Example 5: Notification Overlay](#example-5-notification-overlay)
6. [Example 6: Corner Widget](#example-6-corner-widget)
7. [Example 7: Multi-Monitor Panel](#example-7-multi-monitor-panel)
8. [Example 8: QML Panel](#example-8-qml-panel)
9. [Example 9: Lock Screen](#example-9-lock-screen)
10. [Example 10: System Tray](#example-10-system-tray)

---

## Example 1: Top Panel

**Use case:** Panel ở trên cùng như taskbar, menu bar

### Features
- Layer: TOP
- Anchor: Top, Left, Right (full width)
- Exclusive zone: Reserve space
- Keyboard: Exclusive (có text input)

### Code

```cpp
// panel.cpp
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QTimer>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class TopPanel : public QWidget {
    Q_OBJECT
public:
    TopPanel() {
        setFixedHeight(35);
        setStyleSheet(R"(
            QWidget {
                background-color: #1e1e1e;
                color: white;
            }
            QPushButton {
                border: none;
                padding: 5px 15px;
                background-color: #2d2d2d;
            }
            QPushButton:hover {
                background-color: #3d3d3d;
            }
        )");
        
        // Layout
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 0, 10, 0);
        
        // Menu button
        QPushButton *menuBtn = new QPushButton("≡ Menu");
        layout->addWidget(menuBtn);
        
        layout->addStretch();
        
        // Clock
        clockLabel = new QLabel();
        updateClock();
        layout->addWidget(clockLabel);
        
        // Update clock every second
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TopPanel::updateClock);
        timer->start(1000);
        
        // Configure layer shell
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(35);
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityExclusive
        );
    }
    
    void updateClock() {
        clockLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
    }
    
    QLabel *clockLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    TopPanel panel;
    panel.show();
    
    return app.exec();
}

#include "panel.moc"
```

### Build

```bash
g++ panel.cpp -o panel \
    $(pkg-config --cflags --libs Qt5Widgets LayerShellQtInterface) \
    -fPIC

./panel
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(TopPanel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(LayerShellQt REQUIRED)

add_executable(panel panel.cpp)
target_link_libraries(panel Qt5::Widgets LayerShellQt::Interface)
```

---

## Example 2: Bottom Dock

**Use case:** Application launcher ở dưới cùng

### Features
- Layer: TOP
- Anchor: Bottom, Left, Right
- Exclusive zone: 60px
- Icon buttons

### Code

```cpp
// dock.cpp
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
    Dock() {
        setFixedHeight(60);
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(30, 30, 30, 0.9);
            }
            QPushButton {
                border: none;
                border-radius: 10px;
                padding: 10px;
                margin: 5px;
                background-color: #2d2d2d;
                color: white;
                font-size: 24px;
                min-width: 50px;
                min-height: 50px;
            }
            QPushButton:hover {
                background-color: #4d4d4d;
            }
        )");
        
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 5, 10, 5);
        
        // Add apps
        addApp("🌐", "firefox");
        addApp("📁", "dolphin");
        addApp("⌨️", "konsole");
        addApp("📝", "kate");
        
        setupLayerShell();
    }
    
private:
    void addApp(const QString &icon, const QString &command) {
        QPushButton *btn = new QPushButton(icon);
        connect(btn, &QPushButton::clicked, [command]() {
            QProcess::startDetached(command, {});
        });
        qobject_cast<QHBoxLayout*>(layout())->addWidget(btn);
    }
    
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(60);
        layerWindow->setMargins(QMargins(0, 0, 0, 5));
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    Dock dock;
    dock.show();
    
    return app.exec();
}

#include "dock.moc"
```

---

## Example 3: Desktop Widget

**Use case:** System monitor widget, weather widget

### Features
- Layer: BOTTOM (dưới normal windows)
- Anchor: Top-Right corner
- No exclusive zone
- No keyboard

### Code

```cpp
// widget.cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTimer>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class SystemWidget : public QWidget {
    Q_OBJECT
public:
    SystemWidget() {
        setFixedSize(200, 150);
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(30, 30, 30, 0.8);
                border-radius: 10px;
                color: white;
            }
            QLabel {
                padding: 5px;
            }
        )");
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        cpuLabel = new QLabel("CPU: --");
        memLabel = new QLabel("Memory: --");
        
        layout->addWidget(new QLabel("System Monitor"));
        layout->addWidget(cpuLabel);
        layout->addWidget(memLabel);
        layout->addStretch();
        
        // Update every 2 seconds
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SystemWidget::updateStats);
        timer->start(2000);
        updateStats();
        
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerBottom);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(0);  // No space reservation
        layerWindow->setMargins(QMargins(0, 10, 10, 0));
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
    }
    
    void updateStats() {
        // Simple CPU usage (Linux)
        QFile file("/proc/loadavg");
        if (file.open(QIODevice::ReadOnly)) {
            QString load = file.readAll().split(' ')[0];
            cpuLabel->setText("CPU: " + load);
        }
        
        // Memory usage
        QFile memFile("/proc/meminfo");
        if (memFile.open(QIODevice::ReadOnly)) {
            auto lines = QString(memFile.readAll()).split('\n');
            int total = lines[0].split(QRegExp("\\s+"))[1].toInt();
            int free = lines[1].split(QRegExp("\\s+"))[1].toInt();
            int used = total - free;
            int percent = (used * 100) / total;
            memLabel->setText(QString("Memory: %1%").arg(percent));
        }
    }
    
    QLabel *cpuLabel;
    QLabel *memLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    SystemWidget widget;
    widget.show();
    
    return app.exec();
}

#include "widget.moc"
```

---

## Example 4: Wallpaper

**Use case:** Animated wallpaper, custom background

### Features
- Layer: BACKGROUND
- Anchor: Full screen
- No exclusive zone
- No keyboard

### Code

```cpp
// wallpaper.cpp
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include <cmath>

class AnimatedWallpaper : public QWidget {
    Q_OBJECT
public:
    AnimatedWallpaper() {
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Animate at 60 FPS
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
        timer->start(16);  // ~60 FPS
        
        setupLayerShell();
    }
    
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Gradient background
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(20, 20, 40));
        gradient.setColorAt(1, QColor(60, 20, 60));
        painter.fillRect(rect(), gradient);
        
        // Animated circles
        double time = QDateTime::currentMSecsSinceEpoch() / 1000.0;
        
        for (int i = 0; i < 5; i++) {
            double x = width() / 2 + 200 * std::cos(time * 0.5 + i * 1.2);
            double y = height() / 2 + 200 * std::sin(time * 0.3 + i * 1.5);
            double size = 50 + 30 * std::sin(time + i);
            
            QColor color;
            color.setHsv(int(time * 30 + i * 60) % 360, 200, 150, 80);
            
            painter.setBrush(color);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(x, y), size, size);
        }
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerBackground);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(0);
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    AnimatedWallpaper wallpaper;
    wallpaper.showFullScreen();
    
    return app.exec();
}

#include "wallpaper.moc"
```

---

## Example 5: Notification Overlay

**Use case:** Toast notifications, alerts

### Features
- Layer: TOP (hoặc OVERLAY)
- Anchor: Top-Right
- No exclusive zone (overlay mode)
- Auto-hide after timeout

### Code

```cpp
// notification.cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class Notification : public QWidget {
    Q_OBJECT
public:
    Notification(const QString &title, const QString &message) {
        setFixedSize(350, 100);
        setStyleSheet(R"(
            QWidget {
                background-color: #2d2d2d;
                border: 2px solid #4d4d4d;
                border-radius: 8px;
                color: white;
            }
            QLabel#title {
                font-weight: bold;
                font-size: 14px;
            }
            QLabel#message {
                color: #cccccc;
            }
        )");
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(15, 10, 15, 10);
        
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setObjectName("title");
        
        QLabel *msgLabel = new QLabel(message);
        msgLabel->setObjectName("message");
        msgLabel->setWordWrap(true);
        
        layout->addWidget(titleLabel);
        layout->addWidget(msgLabel);
        
        setupLayerShell();
        
        // Auto-hide after 5 seconds
        QTimer::singleShot(5000, this, &Notification::fadeOut);
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(0);  // Overlay mode
        layerWindow->setMargins(QMargins(0, 10, 10, 0));
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
    }
    
    void fadeOut() {
        QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
        anim->setDuration(300);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, &QWidget::close);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    // Show notification
    Notification *notif = new Notification(
        "System Update",
        "Your system has been updated successfully."
    );
    notif->setAttribute(Qt::WA_DeleteOnClose);
    notif->show();
    
    return app.exec();
}

#include "notification.moc"
```

---

## Example 6: Corner Widget

**Use case:** Mini player, quick actions

### Features
- Layer: TOP
- Anchor: Bottom-Right corner
- Fixed size
- Draggable (advanced)

### Code

```cpp
// corner_widget.cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class CornerWidget : public QWidget {
    Q_OBJECT
public:
    CornerWidget() {
        setFixedSize(200, 150);
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(30, 30, 30, 0.95);
                border-radius: 10px;
                color: white;
            }
            QPushButton {
                background-color: #4d4d4d;
                border: none;
                border-radius: 5px;
                padding: 8px;
                margin: 2px;
            }
            QPushButton:hover {
                background-color: #5d5d5d;
            }
        )");
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);
        
        QLabel *title = new QLabel("Quick Actions");
        title->setAlignment(Qt::AlignCenter);
        
        QPushButton *btn1 = new QPushButton("🔊 Volume");
        QPushButton *btn2 = new QPushButton("📸 Screenshot");
        QPushButton *btn3 = new QPushButton("⚙️ Settings");
        
        layout->addWidget(title);
        layout->addWidget(btn1);
        layout->addWidget(btn2);
        layout->addWidget(btn3);
        
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(0);
        layerWindow->setMargins(QMargins(0, 0, 15, 15));
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    CornerWidget widget;
    widget.show();
    
    return app.exec();
}

#include "corner_widget.moc"
```

---

## Example 7: Multi-Monitor Panel

**Use case:** Separate panel cho mỗi monitor

### Code

```cpp
// multi_panel.cpp
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QScreen>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class Panel : public QWidget {
public:
    Panel(const QString &screenName) {
        setFixedHeight(30);
        setStyleSheet("background-color: #2d2d2d; color: white;");
        
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel("Panel on: " + screenName));
        
        setupLayerShell(screenName);
    }
    
private:
    void setupLayerShell(const QString &screenName) {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(30);
        layerWindow->setScope(screenName);  // Per-screen
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    // Create panel for each screen
    QList<Panel*> panels;
    for (QScreen *screen : QGuiApplication::screens()) {
        qDebug() << "Creating panel for:" << screen->name();
        
        Panel *panel = new Panel(screen->name());
        panel->show();
        panels.append(panel);
    }
    
    // Handle screen added/removed
    QObject::connect(qApp, &QGuiApplication::screenAdded, [&](QScreen *screen) {
        qDebug() << "Screen added:" << screen->name();
        Panel *panel = new Panel(screen->name());
        panel->show();
        panels.append(panel);
    });
    
    QObject::connect(qApp, &QGuiApplication::screenRemoved, [&](QScreen *screen) {
        qDebug() << "Screen removed:" << screen->name();
        // Remove corresponding panel
        for (Panel *panel : panels) {
            auto layerWin = LayerShellQt::Window::get(panel->windowHandle());
            if (layerWin && layerWin->scope() == screen->name()) {
                panels.removeOne(panel);
                panel->deleteLater();
                break;
            }
        }
    });
    
    return app.exec();
}
```

---

## Example 8: QML Panel

**Use case:** Modern QML-based UI

### main.cpp

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    QQuickView view;
    view.setSource(QUrl("qrc:/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorLeft |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(50);
    
    view.show();
    return app.exec();
}
```

### main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    width: 1920
    height: 50
    color: "#1e1e1e"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // Menu button
        Button {
            text: "≡ Menu"
            flat: true
            
            contentItem: Text {
                text: parent.text
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            background: Rectangle {
                color: parent.hovered ? "#3d3d3d" : "#2d2d2d"
                radius: 5
            }
        }
        
        Item { Layout.fillWidth: true }
        
        // Clock
        Text {
            id: clock
            color: "white"
            font.pixelSize: 16
            
            Timer {
                interval: 1000
                running: true
                repeat: true
                onTriggered: {
                    clock.text = Qt.formatTime(new Date(), "hh:mm:ss")
                }
            }
            
            Component.onCompleted: {
                clock.text = Qt.formatTime(new Date(), "hh:mm:ss")
            }
        }
    }
}
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(QMLPanel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Quick)
find_package(LayerShellQt REQUIRED)

qt5_add_resources(RESOURCES resources.qrc)

add_executable(qmlpanel main.cpp ${RESOURCES})
target_link_libraries(qmlpanel 
    Qt5::Quick 
    LayerShellQt::Interface
)
```

### resources.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>main.qml</file>
    </qresource>
</RCC>
```

---

## Example 9: Lock Screen

**Use case:** Screen locker

### Features
- Layer: OVERLAY (trên tất cả)
- Full screen
- Keyboard exclusive
- Block input to other apps

### Code

```cpp
// lockscreen.cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class LockScreen : public QWidget {
    Q_OBJECT
public:
    LockScreen() {
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(0, 0, 0, 0.95);
                color: white;
            }
            QLineEdit {
                background-color: rgba(255, 255, 255, 0.1);
                border: 2px solid rgba(255, 255, 255, 0.3);
                border-radius: 5px;
                padding: 10px;
                font-size: 16px;
                color: white;
            }
            QPushButton {
                background-color: #4d4d4d;
                border: none;
                border-radius: 5px;
                padding: 10px 30px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #5d5d5d;
            }
        )");
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        
        QLabel *icon = new QLabel("🔒");
        icon->setAlignment(Qt::AlignCenter);
        icon->setStyleSheet("font-size: 64px;");
        
        QLabel *title = new QLabel("Screen Locked");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 24px; margin: 20px;");
        
        passwordInput = new QLineEdit();
        passwordInput->setEchoMode(QLineEdit::Password);
        passwordInput->setPlaceholderText("Enter password");
        passwordInput->setFixedWidth(300);
        passwordInput->setAlignment(Qt::AlignCenter);
        
        QPushButton *unlockBtn = new QPushButton("Unlock");
        unlockBtn->setFixedWidth(300);
        
        layout->addWidget(icon);
        layout->addWidget(title);
        layout->addWidget(passwordInput);
        layout->addWidget(unlockBtn);
        
        connect(unlockBtn, &QPushButton::clicked, this, &LockScreen::tryUnlock);
        connect(passwordInput, &QLineEdit::returnPressed, this, &LockScreen::tryUnlock);
        
        setupLayerShell();
        
        // Focus password input
        passwordInput->setFocus();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(-1);
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityExclusive
        );
    }
    
    void tryUnlock() {
        if (passwordInput->text() == "password") {
            close();
        } else {
            passwordInput->clear();
            passwordInput->setPlaceholderText("Wrong password!");
        }
    }
    
    QLineEdit *passwordInput;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    LockScreen lockScreen;
    lockScreen.showFullScreen();
    
    return app.exec();
}

#include "lockscreen.moc"
```

---

## Example 10: System Tray

**Use case:** System tray ở góc panel

### Code

```cpp
// systemtray.cpp
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class SystemTray : public QWidget {
    Q_OBJECT
public:
    SystemTray() {
        setFixedSize(150, 30);
        setStyleSheet(R"(
            QWidget {
                background-color: #2d2d2d;
                color: white;
            }
            QPushButton {
                border: none;
                padding: 5px;
                background-color: transparent;
                font-size: 16px;
            }
            QPushButton:hover {
                background-color: #3d3d3d;
            }
        )");
        
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(5, 0, 5, 0);
        layout->setSpacing(2);
        
        addTrayIcon("🔊", createVolumeMenu());
        addTrayIcon("🔋", createBatteryMenu());
        addTrayIcon("📶", createNetworkMenu());
        
        setupLayerShell();
    }
    
private:
    void addTrayIcon(const QString &icon, QMenu *menu) {
        QPushButton *btn = new QPushButton(icon);
        btn->setMenu(menu);
        qobject_cast<QHBoxLayout*>(layout())->addWidget(btn);
    }
    
    QMenu* createVolumeMenu() {
        QMenu *menu = new QMenu(this);
        menu->addAction("Volume: 80%");
        menu->addSeparator();
        menu->addAction("Mute");
        menu->addAction("Sound Settings...");
        return menu;
    }
    
    QMenu* createBatteryMenu() {
        QMenu *menu = new QMenu(this);
        menu->addAction("Battery: 65%");
        menu->addAction("2h 30m remaining");
        menu->addSeparator();
        menu->addAction("Power Settings...");
        return menu;
    }
    
    QMenu* createNetworkMenu() {
        QMenu *menu = new QMenu(this);
        menu->addAction("Connected: WiFi");
        menu->addSeparator();
        menu->addAction("Network Settings...");
        return menu;
    }
    
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(0);  // Part of panel, no extra space
        layerWindow->setMargins(QMargins(0, 0, 10, 0));
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    SystemTray tray;
    tray.show();
    
    return app.exec();
}

#include "systemtray.moc"
```

---

## Build All Examples

### Shell Script

```bash
#!/bin/bash
# build_all.sh

examples=("panel" "dock" "widget" "wallpaper" "notification" "corner_widget" "lockscreen" "systemtray")

for example in "${examples[@]}"; do
    echo "Building $example..."
    g++ ${example}.cpp -o $example \
        $(pkg-config --cflags --libs Qt5Widgets LayerShellQtInterface) \
        -fPIC
done

echo "All examples built!"
```

---

## Summary: Use Cases Mapping

| Use Case | Layer | Anchors | Exclusive Zone | Keyboard |
|----------|-------|---------|----------------|----------|
| Top Panel | TOP | Top+Left+Right | Height | Exclusive |
| Bottom Dock | TOP | Bottom+Left+Right | Height | Exclusive |
| Desktop Widget | BOTTOM | Top+Right | 0 | None |
| Wallpaper | BACKGROUND | All | 0 | None |
| Notification | TOP | Top+Right | 0 | None |
| Corner Widget | TOP | Bottom+Right | 0 | None |
| Lock Screen | OVERLAY | All | -1 | Exclusive |
| System Tray | TOP | Top+Right | 0 | Exclusive |

---

## Next Steps

Bây giờ bạn có examples thực tế! Tiếp theo:
- **[04-advanced-topics.md](./04-advanced-topics.md)** - Advanced patterns
- **[05-best-practices.md](./05-best-practices.md)** - Best practices

Build và chạy các examples, thử modify chúng để học! 🚀
