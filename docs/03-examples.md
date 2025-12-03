# LayerShellQt - Practical Examples (QML + Model-View)

## Mục Lục
1. [Example 1: Top Panel](#example-1-top-panel)
2. [Example 2: Bottom Dock](#example-2-bottom-dock)
3. [Example 3: Desktop Widget](#example-3-desktop-widget)
4. [Example 4: Wallpaper](#example-4-wallpaper)
5. [Example 5: Notification Overlay](#example-5-notification-overlay)
6. [Example 6: Corner Widget](#example-6-corner-widget)
7. [Example 7: Multi-Monitor Panel](#example-7-multi-monitor-panel)
8. [Example 8: Dynamic Panel](#example-8-dynamic-panel)
9. [Example 9: Lock Screen](#example-9-lock-screen)
10. [Example 10: System Tray](#example-10-system-tray)

**Tất cả examples sử dụng Model-View architecture với QML!**

---

## Example 1: Top Panel

**Use case:** Panel ở trên cùng như taskbar, menu bar

### Architecture

```
PanelModel (C++) → Panel.qml (View)
  └─ Clock data
  └─ System info
```

### Model (C++)

```cpp
// PanelModel.h
#include <QObject>
#include <QTimer>
#include <QDateTime>

class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
    Q_PROPERTY(QString appName READ appName CONSTANT)
    
public:
    explicit PanelModel(QObject *parent = nullptr) : QObject(parent) {
        // Update time every second
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PanelModel::updateTime);
        timer->start(1000);
        updateTime();
    }
    
    QString currentTime() const { return m_currentTime; }
    QString appName() const { return "My Panel"; }
    
signals:
    void timeChanged();
    
private slots:
    void updateTime() {
        QString newTime = QTime::currentTime().toString("hh:mm:ss");
        if (m_currentTime != newTime) {
            m_currentTime = newTime;
            emit timeChanged();
        }
    }
    
private:
    QString m_currentTime;
};
```

### View (QML)

```qml
// Panel.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 1920
    height: 35
    color: "#2d2d2d"
    
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 10
        
        // Left: App name
        Text {
            text: panelModel.appName
            color: "white"
            font.pixelSize: 14
            font.bold: true
        }
        
        // Center: Spacer
        Item { Layout.fillWidth: true }
        
        // Right: Clock
        Text {
            text: panelModel.currentTime
            color: "white"
            font.pixelSize: 14
            font.family: "monospace"
        }
    }
}
```

### Main (C++)

```cpp
// main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "PanelModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    // Create model
    PanelModel model;
    
    // Create view
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/Panel.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    
    // Configure layer shell
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorLeft |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(35);
    
    view.show();
    return app.exec();
}
```

### resources.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>Panel.qml</file>
    </qresource>
</RCC>
```

---

## Example 2: Bottom Dock

**Use case:** Application launcher ở dưới cùng

### Model (C++)

```cpp
// DockModel.h
#include <QAbstractListModel>

class DockModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum Roles {
        IconRole = Qt::UserRole + 1,
        NameRole,
        CommandRole
    };
    
    struct App {
        QString icon;
        QString name;
        QString command;
    };
    
    explicit DockModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        m_apps = {
            {"🌐", "Browser", "firefox"},
            {"📁", "Files", "dolphin"},
            {"⌨️", "Terminal", "konsole"},
            {"📝", "Editor", "kate"},
            {"🎵", "Music", "spotify"}
        };
    }
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_apps.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_apps.count())
            return QVariant();
        
        const App &app = m_apps.at(index.row());
        switch (role) {
        case IconRole: return app.icon;
        case NameRole: return app.name;
        case CommandRole: return app.command;
        }
        return QVariant();
    }
    
    QHash<int, QByteArray> roleNames() const override {
        return {
            {IconRole, "icon"},
            {NameRole, "name"},
            {CommandRole, "command"}
        };
    }
    
    Q_INVOKABLE void launchApp(const QString &command) {
        QProcess::startDetached(command, {});
    }
    
private:
    QList<App> m_apps;
};
```

### View (QML)

```qml
// Dock.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 1920
    height: 60
    color: "transparent"
    
    Rectangle {
        anchors.centerIn: parent
        width: appRow.width + 20
        height: 60
        color: "#2d2d2d"
        opacity: 0.9
        radius: 10
        
        RowLayout {
            id: appRow
            anchors.centerIn: parent
            spacing: 10
            
            Repeater {
                model: dockModel
                
                delegate: Button {
                    id: appButton
                    
                    contentItem: Text {
                        text: model.icon
                        font.pixelSize: 28
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        implicitWidth: 50
                        implicitHeight: 50
                        color: appButton.hovered ? "#4d4d4d" : "#3d3d3d"
                        radius: 10
                        
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    ToolTip.visible: hovered
                    ToolTip.text: model.name
                    
                    onClicked: {
                        dockModel.launchApp(model.command)
                    }
                }
            }
        }
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "DockModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    DockModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("dockModel", &model);
    view.setSource(QUrl("qrc:/Dock.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setColor(Qt::transparent);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorBottom |
        LayerShellQt::Window::AnchorLeft |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(70);
    layerWindow->setMargins(QMargins(0, 0, 0, 5));
    
    view.show();
    return app.exec();
}
```

---

## Example 3: Desktop Widget

**Use case:** System monitor widget

### Model (C++)

```cpp
// WidgetModel.h
#include <QObject>
#include <QTimer>
#include <QFile>

class WidgetModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString cpuLoad READ cpuLoad NOTIFY cpuLoadChanged)
    Q_PROPERTY(int memoryUsage READ memoryUsage NOTIFY memoryUsageChanged)
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
    
public:
    explicit WidgetModel(QObject *parent = nullptr) : QObject(parent) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &WidgetModel::updateStats);
        timer->start(2000);
        updateStats();
    }
    
    QString cpuLoad() const { return m_cpuLoad; }
    int memoryUsage() const { return m_memoryUsage; }
    QString currentTime() const { return m_currentTime; }
    
signals:
    void cpuLoadChanged();
    void memoryUsageChanged();
    void timeChanged();
    
private slots:
    void updateStats() {
        // Update time
        m_currentTime = QTime::currentTime().toString("hh:mm:ss");
        emit timeChanged();
        
        // Update CPU load
        QFile loadFile("/proc/loadavg");
        if (loadFile.open(QIODevice::ReadOnly)) {
            m_cpuLoad = QString(loadFile.readAll()).split(' ')[0];
            emit cpuLoadChanged();
        }
        
        // Update memory
        QFile memFile("/proc/meminfo");
        if (memFile.open(QIODevice::ReadOnly)) {
            QString content = memFile.readAll();
            QStringList lines = content.split('\n');
            
            auto getValue = [](const QString &line) {
                return line.split(QRegExp("\\s+"))[1].toInt();
            };
            
            int total = getValue(lines[0]);
            int available = getValue(lines[2]);
            int used = total - available;
            m_memoryUsage = (used * 100) / total;
            emit memoryUsageChanged();
        }
    }
    
private:
    QString m_cpuLoad;
    int m_memoryUsage = 0;
    QString m_currentTime;
};
```

### View (QML)

```qml
// Widget.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 250
    height: 200
    color: "#2d2d2d"
    opacity: 0.85
    radius: 12
    border.color: "#4d4d4d"
    border.width: 1
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        // Title
        Text {
            text: "System Monitor"
            color: "#4fc3f7"
            font.pixelSize: 16
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Time
        Text {
            text: widgetModel.currentTime
            color: "white"
            font.pixelSize: 24
            font.bold: true
            font.family: "monospace"
            Layout.alignment: Qt.AlignHCenter
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#4d4d4d"
        }
        
        // CPU
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                text: "CPU:"
                color: "#aaa"
                font.pixelSize: 12
            }
            
            Item { Layout.fillWidth: true }
            
            Text {
                text: widgetModel.cpuLoad
                color: "white"
                font.pixelSize: 12
                font.bold: true
            }
        }
        
        // Memory
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                text: "RAM:"
                color: "#aaa"
                font.pixelSize: 12
            }
            
            Item { Layout.fillWidth: true }
            
            Text {
                text: widgetModel.memoryUsage + "%"
                color: widgetModel.memoryUsage > 80 ? "#ff5555" : "white"
                font.pixelSize: 12
                font.bold: true
            }
        }
        
        // Memory bar
        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: "#1e1e1e"
            radius: 4
            
            Rectangle {
                width: parent.width * (widgetModel.memoryUsage / 100)
                height: parent.height
                color: widgetModel.memoryUsage > 80 ? "#ff5555" : "#4fc3f7"
                radius: 4
                
                Behavior on width {
                    NumberAnimation { duration: 300 }
                }
            }
        }
        
        Item { Layout.fillHeight: true }
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "WidgetModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    WidgetModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("widgetModel", &model);
    view.setSource(QUrl("qrc:/Widget.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setColor(Qt::transparent);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerBottom);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(0);
    layerWindow->setMargins(QMargins(0, 10, 10, 0));
    layerWindow->setKeyboardInteractivity(
        LayerShellQt::Window::KeyboardInteractivityNone
    );
    
    view.show();
    return app.exec();
}
```

---

## Example 4: Wallpaper

**Use case:** Animated wallpaper

### Model (C++)

```cpp
// WallpaperModel.h
#include <QObject>
#include <QDateTime>

class WallpaperModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal time READ time NOTIFY timeChanged)
    
public:
    explicit WallpaperModel(QObject *parent = nullptr) : QObject(parent) {}
    
    qreal time() const {
        return QDateTime::currentMSecsSinceEpoch() / 1000.0;
    }
    
signals:
    void timeChanged();
    
public slots:
    void tick() {
        emit timeChanged();
    }
};
```

### View (QML)

```qml
// Wallpaper.qml
import QtQuick 2.15

Rectangle {
    id: root
    
    // Gradient background
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#0f2027" }
        GradientStop { position: 0.5; color: "#203a43" }
        GradientStop { position: 1.0; color: "#2c5364" }
    }
    
    // Animated circles
    Repeater {
        model: 8
        
        delegate: Rectangle {
            id: circle
            
            property real angle: wallpaperModel.time * 0.3 + index * (Math.PI * 2 / 8)
            property real distance: 300 + 100 * Math.sin(wallpaperModel.time * 0.5 + index * 0.5)
            
            x: root.width / 2 + distance * Math.cos(angle) - width / 2
            y: root.height / 2 + distance * Math.sin(angle) - height / 2
            
            width: 80 + 40 * Math.sin(wallpaperModel.time + index)
            height: width
            radius: width / 2
            
            color: Qt.hsva((wallpaperModel.time * 0.02 + index * 0.125) % 1.0, 0.6, 0.3, 0.25)
            
            Behavior on x { NumberAnimation { duration: 50 } }
            Behavior on y { NumberAnimation { duration: 50 } }
        }
    }
    
    // Center glow
    Rectangle {
        anchors.centerIn: parent
        width: 800 + 200 * Math.sin(wallpaperModel.time * 0.5)
        height: width
        radius: width / 2
        
        gradient: RadialGradient {
            GradientStop { position: 0.0; color: Qt.rgba(1, 1, 1, 0.12) }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
    
    // Clock
    Text {
        anchors.centerIn: parent
        text: Qt.formatTime(new Date(), "hh:mm")
        color: Qt.rgba(1, 1, 1, 0.4)
        font.pixelSize: 48
        font.weight: Font.Light
    }
    
    // Animation timer
    Timer {
        interval: 16  // ~60 FPS
        running: true
        repeat: true
        onTriggered: wallpaperModel.tick()
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "WallpaperModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    WallpaperModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("wallpaperModel", &model);
    view.setSource(QUrl("qrc:/Wallpaper.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
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
    
    view.showFullScreen();
    return app.exec();
}
```

---

## Example 5: Notification Overlay

**Use case:** Toast notifications

### Model (C++)

```cpp
// NotificationModel.h
#include <QObject>
#include <QTimer>

class NotificationModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString message READ message CONSTANT)
    Q_PROPERTY(qreal opacity READ opacity NOTIFY opacityChanged)
    
public:
    NotificationModel(const QString &title, const QString &message, 
                     QObject *parent = nullptr)
        : QObject(parent), m_title(title), m_message(message), m_opacity(1.0) {
        
        // Auto-hide after 5 seconds
        QTimer::singleShot(5000, this, &NotificationModel::fadeOut);
    }
    
    QString title() const { return m_title; }
    QString message() const { return m_message; }
    qreal opacity() const { return m_opacity; }
    
signals:
    void opacityChanged();
    void closeRequested();
    
private slots:
    void fadeOut() {
        // Fade out animation will be handled by QML
        emit closeRequested();
    }
    
private:
    QString m_title;
    QString m_message;
    qreal m_opacity;
};
```

### View (QML)

```qml
// Notification.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 350
    height: 100
    color: "#2d2d2d"
    border.color: "#555"
    border.width: 2
    radius: 10
    
    // Fade in on appear
    opacity: 0
    Component.onCompleted: fadeIn.start()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 5
        
        // Title
        Text {
            text: notifModel.title
            color: "white"
            font.pixelSize: 14
            font.bold: true
        }
        
        // Message
        Text {
            text: notifModel.message
            color: "#ccc"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
    
    // Close button
    Button {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 5
        
        width: 20
        height: 20
        
        contentItem: Text {
            text: "×"
            color: "white"
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        
        background: Rectangle {
            color: parent.hovered ? "#555" : "transparent"
            radius: 3
        }
        
        onClicked: fadeOut.start()
    }
    
    // Fade in animation
    NumberAnimation {
        id: fadeIn
        target: root
        property: "opacity"
        from: 0
        to: 1
        duration: 300
    }
    
    // Fade out animation
    NumberAnimation {
        id: fadeOut
        target: root
        property: "opacity"
        from: 1
        to: 0
        duration: 300
        onFinished: Qt.quit()  // Close the notification window
    }
    
    // Connect to model signal
    Connections {
        target: notifModel
        function onCloseRequested() {
            fadeOut.start()
        }
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "NotificationModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    // Create notification
    NotificationModel model("System Update", 
        "Your system has been updated successfully.");
    
    QQuickView view;
    view.rootContext()->setContextProperty("notifModel", &model);
    view.setSource(QUrl("qrc:/Notification.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setColor(Qt::transparent);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(0);
    layerWindow->setMargins(QMargins(0, 10, 10, 0));
    layerWindow->setKeyboardInteractivity(
        LayerShellQt::Window::KeyboardInteractivityNone
    );
    
    view.show();
    return app.exec();
}
```

---

## Example 6: Corner Widget

**Use case:** Quick actions widget

### Model (C++)

```cpp
// CornerWidgetModel.h
#include <QObject>
#include <QProcess>

class CornerWidgetModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    
public:
    explicit CornerWidgetModel(QObject *parent = nullptr) 
        : QObject(parent) {}
    
    QString title() const { return "Quick Actions"; }
    
    Q_INVOKABLE void executeAction(const QString &action) {
        if (action == "volume") {
            QProcess::startDetached("pavucontrol", {});
        } else if (action == "screenshot") {
            QProcess::startDetached("spectacle", {"-r"});
        } else if (action == "settings") {
            QProcess::startDetached("systemsettings5", {});
        }
    }
};
```

### View (QML)

```qml
// CornerWidget.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 200
    height: 150
    color: "#2d2d2d"
    opacity: 0.95
    radius: 10
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        
        // Title
        Text {
            text: widgetModel.title
            color: "white"
            font.pixelSize: 14
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Action buttons
        Repeater {
            model: [
                {icon: "🔊", text: "Volume", action: "volume"},
                {icon: "📸", text: "Screenshot", action: "screenshot"},
                {icon: "⚙️", text: "Settings", action: "settings"}
            ]
            
            delegate: Button {
                Layout.fillWidth: true
                height: 30
                
                contentItem: RowLayout {
                    spacing: 8
                    
                    Text {
                        text: modelData.icon
                        font.pixelSize: 18
                    }
                    
                    Text {
                        text: modelData.text
                        color: "white"
                        font.pixelSize: 12
                        Layout.fillWidth: true
                    }
                }
                
                background: Rectangle {
                    color: parent.hovered ? "#4d4d4d" : "#3d3d3d"
                    radius: 5
                }
                
                onClicked: widgetModel.executeAction(modelData.action)
            }
        }
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "CornerWidgetModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    CornerWidgetModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("widgetModel", &model);
    view.setSource(QUrl("qrc:/CornerWidget.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setColor(Qt::transparent);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorBottom |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(0);
    layerWindow->setMargins(QMargins(0, 0, 15, 15));
    
    view.show();
    return app.exec();
}
```

---

## Example 7: Multi-Monitor Panel

**Use case:** Separate panel cho mỗi monitor

### Model (C++)

```cpp
// MultiPanelModel.h
#include <QAbstractListModel>
#include <QGuiApplication>
#include <QScreen>

class MultiPanelModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    enum Roles {
        ScreenNameRole = Qt::UserRole + 1,
        ScreenWidthRole,
        ScreenHeightRole
    };
    
    explicit MultiPanelModel(QObject *parent = nullptr) 
        : QAbstractListModel(parent) {
        
        refreshScreens();
        
        connect(qApp, &QGuiApplication::screenAdded,
                this, &MultiPanelModel::onScreenAdded);
        connect(qApp, &QGuiApplication::screenRemoved,
                this, &MultiPanelModel::onScreenRemoved);
    }
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_screens.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_screens.count())
            return QVariant();
        
        QScreen *screen = m_screens.at(index.row());
        switch (role) {
        case ScreenNameRole: return screen->name();
        case ScreenWidthRole: return screen->geometry().width();
        case ScreenHeightRole: return screen->geometry().height();
        }
        return QVariant();
    }
    
    QHash<int, QByteArray> roleNames() const override {
        return {
            {ScreenNameRole, "screenName"},
            {ScreenWidthRole, "screenWidth"},
            {ScreenHeightRole, "screenHeight"}
        };
    }
    
    int count() const { return m_screens.count(); }
    
    Q_INVOKABLE QString getScreenName(int index) const {
        if (index >= 0 && index < m_screens.count())
            return m_screens.at(index)->name();
        return QString();
    }
    
signals:
    void countChanged();
    void screenAdded(const QString &name);
    void screenRemoved(const QString &name);
    
private slots:
    void onScreenAdded(QScreen *screen) {
        beginInsertRows(QModelIndex(), m_screens.count(), m_screens.count());
        m_screens.append(screen);
        endInsertRows();
        emit countChanged();
        emit screenAdded(screen->name());
    }
    
    void onScreenRemoved(QScreen *screen) {
        int index = m_screens.indexOf(screen);
        if (index >= 0) {
            beginRemoveRows(QModelIndex(), index, index);
            m_screens.removeAt(index);
            endRemoveRows();
            emit countChanged();
            emit screenRemoved(screen->name());
        }
    }
    
private:
    void refreshScreens() {
        beginResetModel();
        m_screens = QGuiApplication::screens();
        endResetModel();
        emit countChanged();
    }
    
    QList<QScreen*> m_screens;
};
```

### View (QML)

```qml
// MultiPanel.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 1920
    height: 35
    color: "#1e1e1e"
    
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 15
        
        // Screen info
        RowLayout {
            spacing: 10
            
            Text {
                text: "🖥️"
                font.pixelSize: 18
            }
            
            Text {
                text: currentScreenName || "Unknown"
                color: "#4fc3f7"
                font.pixelSize: 14
                font.bold: true
            }
        }
        
        Item { Layout.fillWidth: true }
        
        // Total screens
        Rectangle {
            width: 100
            height: 25
            color: "#2d2d2d"
            radius: 5
            
            RowLayout {
                anchors.centerIn: parent
                spacing: 5
                
                Text {
                    text: "📺"
                    font.pixelSize: 14
                }
                
                Text {
                    text: panelModel.count + " screens"
                    color: "white"
                    font.pixelSize: 11
                }
            }
        }
        
        // Clock
        Text {
            id: clockText
            color: "white"
            font.pixelSize: 14
            font.family: "monospace"
            
            function updateTime() {
                clockText.text = Qt.formatTime(new Date(), "hh:mm:ss")
            }
            
            Component.onCompleted: updateTime()
            
            Timer {
                interval: 1000
                running: true
                repeat: true
                onTriggered: clockText.updateTime()
            }
        }
    }
}
```

### Manager (C++)

```cpp
// PanelViewManager.h
#include <QObject>
#include <QQuickView>
#include <QHash>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include "MultiPanelModel.h"

class PanelViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit PanelViewManager(MultiPanelModel *model, QObject *parent = nullptr)
        : QObject(parent), m_model(model) {
        
        // Create panels for existing screens
        for (int i = 0; i < model->count(); ++i) {
            createPanel(model->getScreenName(i));
        }
        
        // Watch for screen changes
        connect(model, &MultiPanelModel::screenAdded,
                this, &PanelViewManager::createPanel);
        connect(model, &MultiPanelModel::screenRemoved,
                this, &PanelViewManager::removePanel);
    }
    
    ~PanelViewManager() {
        qDeleteAll(m_views);
    }
    
private slots:
    void createPanel(const QString &screenName) {
        if (m_views.contains(screenName)) return;
        
        QQuickView *view = new QQuickView();
        view->rootContext()->setContextProperty("panelModel", m_model);
        view->rootContext()->setContextProperty("currentScreenName", screenName);
        view->setSource(QUrl("qrc:/MultiPanel.qml"));
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(35);
        layerWindow->setScope(screenName);
        
        view->show();
        m_views.insert(screenName, view);
    }
    
    void removePanel(const QString &screenName) {
        QQuickView *view = m_views.take(screenName);
        if (view) view->deleteLater();
    }
    
private:
    MultiPanelModel *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <LayerShellQt/Shell>
#include "MultiPanelModel.h"
#include "PanelViewManager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    MultiPanelModel model;
    PanelViewManager manager(&model);
    
    return app.exec();
}
```

---

## Example 8: Dynamic Panel

**Use case:** Panel expand/collapse động

### Model (C++)

```cpp
// DynamicPanelModel.h
#include <QObject>

class DynamicPanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool expanded READ expanded NOTIFY expandedChanged)
    Q_PROPERTY(int panelHeight READ panelHeight NOTIFY panelHeightChanged)
    
public:
    explicit DynamicPanelModel(QObject *parent = nullptr)
        : QObject(parent), m_expanded(false) {}
    
    bool expanded() const { return m_expanded; }
    int panelHeight() const { return m_expanded ? 200 : 35; }
    
    Q_INVOKABLE void toggle() {
        m_expanded = !m_expanded;
        emit expandedChanged();
        emit panelHeightChanged();
    }
    
signals:
    void expandedChanged();
    void panelHeightChanged();
    
private:
    bool m_expanded;
};
```

### View (QML)

```qml
// DynamicPanel.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 1920
    height: panelModel.panelHeight
    color: "#2d2d2d"
    
    Behavior on height {
        NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        
        // Top bar (always visible)
        RowLayout {
            Layout.fillWidth: true
            height: 30
            
            Text {
                text: "Dynamic Panel"
                color: "white"
                font.pixelSize: 14
                font.bold: true
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: panelModel.expanded ? "▲ Collapse" : "▼ Expand"
                
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 12
                }
                
                background: Rectangle {
                    color: parent.hovered ? "#4d4d4d" : "#3d3d3d"
                    radius: 5
                }
                
                onClicked: panelModel.toggle()
            }
        }
        
        // Expanded content
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            radius: 5
            visible: panelModel.expanded
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                
                Text {
                    text: "Expanded Content"
                    color: "#4fc3f7"
                    font.pixelSize: 14
                    font.bold: true
                }
                
                // Widgets or additional content
                GridLayout {
                    Layout.fillWidth: true
                    columns: 3
                    columnSpacing: 10
                    rowSpacing: 10
                    
                    Repeater {
                        model: 6
                        
                        Rectangle {
                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 50
                            color: "#3d3d3d"
                            radius: 5
                            
                            Text {
                                anchors.centerIn: parent
                                text: "Widget " + (index + 1)
                                color: "white"
                            }
                        }
                    }
                }
            }
        }
    }
}
```

### Main with Dynamic Exclusive Zone

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "DynamicPanelModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    DynamicPanelModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/DynamicPanel.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorLeft |
        LayerShellQt::Window::AnchorRight
    );
    
    // Update exclusive zone when height changes
    QObject::connect(&model, &DynamicPanelModel::panelHeightChanged, [&]() {
        layerWindow->setExclusiveZone(model.panelHeight());
    });
    layerWindow->setExclusiveZone(model.panelHeight());
    
    view.show();
    return app.exec();
}
```

---

## Example 9: Lock Screen

**Use case:** Screen locker với password

### Model (C++)

```cpp
// LockScreenModel.h
#include <QObject>
#include <QCryptographicHash>

class LockScreenModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorMessageChanged)
    
public:
    explicit LockScreenModel(QObject *parent = nullptr)
        : QObject(parent), m_hasError(false) {
        // In real app, load hashed password from config
        m_passwordHash = QCryptographicHash::hash(
            "password".toUtf8(), 
            QCryptographicHash::Sha256
        ).toHex();
    }
    
    QString errorMessage() const { return m_errorMessage; }
    bool hasError() const { return m_hasError; }
    
    Q_INVOKABLE void tryUnlock(const QString &password) {
        QByteArray hash = QCryptographicHash::hash(
            password.toUtf8(), 
            QCryptographicHash::Sha256
        ).toHex();
        
        if (hash == m_passwordHash) {
            emit unlockSuccess();
        } else {
            m_errorMessage = "Wrong password!";
            m_hasError = true;
            emit errorMessageChanged();
            
            // Clear error after 2 seconds
            QTimer::singleShot(2000, this, &LockScreenModel::clearError);
        }
    }
    
signals:
    void errorMessageChanged();
    void unlockSuccess();
    
private slots:
    void clearError() {
        m_errorMessage.clear();
        m_hasError = false;
        emit errorMessageChanged();
    }
    
private:
    QString m_errorMessage;
    bool m_hasError;
    QByteArray m_passwordHash;
};
```

### View (QML)

```qml
// LockScreen.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: Qt.rgba(0, 0, 0, 0.95)
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        
        // Lock icon
        Text {
            text: "🔒"
            font.pixelSize: 64
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Title
        Text {
            text: "Screen Locked"
            color: "white"
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Password input
        TextField {
            id: passwordField
            Layout.preferredWidth: 300
            Layout.alignment: Qt.AlignHCenter
            
            placeholderText: "Enter password"
            echoMode: TextInput.Password
            horizontalAlignment: TextInput.AlignHCenter
            
            background: Rectangle {
                color: Qt.rgba(1, 1, 1, 0.1)
                border.color: passwordField.activeFocus ? "#4fc3f7" : Qt.rgba(1, 1, 1, 0.3)
                border.width: 2
                radius: 5
            }
            
            color: "white"
            font.pixelSize: 16
            
            onAccepted: lockModel.tryUnlock(text)
            
            Keys.onEscapePressed: text = ""
        }
        
        // Error message
        Text {
            text: lockModel.errorMessage
            color: "#ff5555"
            font.pixelSize: 12
            visible: lockModel.hasError
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Unlock button
        Button {
            Layout.preferredWidth: 300
            Layout.alignment: Qt.AlignHCenter
            height: 40
            
            text: "Unlock"
            
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            background: Rectangle {
                color: parent.hovered ? "#5d5d5d" : "#4d4d4d"
                radius: 5
            }
            
            onClicked: lockModel.tryUnlock(passwordField.text)
        }
        
        // Hint
        Text {
            text: "(Hint: password is 'password')"
            color: Qt.rgba(1, 1, 1, 0.3)
            font.pixelSize: 10
            font.italic: true
            Layout.alignment: Qt.AlignHCenter
        }
    }
    
    // Connections
    Connections {
        target: lockModel
        
        function onUnlockSuccess() {
            Qt.quit()  // Close lock screen
        }
    }
    
    Component.onCompleted: {
        passwordField.forceActiveFocus()
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "LockScreenModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    LockScreenModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("lockModel", &model);
    view.setSource(QUrl("qrc:/LockScreen.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
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
    
    view.showFullScreen();
    return app.exec();
}
```

---

## Example 10: System Tray

**Use case:** System tray với status icons

### Model (C++)

```cpp
// SystemTrayModel.h
#include <QAbstractListModel>

class SystemTrayModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum Roles {
        IconRole = Qt::UserRole + 1,
        NameRole,
        StatusRole
    };
    
    struct TrayItem {
        QString icon;
        QString name;
        QString status;
    };
    
    explicit SystemTrayModel(QObject *parent = nullptr) 
        : QAbstractListModel(parent) {
        
        m_items = {
            {"🔊", "Volume", "80%"},
            {"🔋", "Battery", "65%"},
            {"📶", "Network", "Connected"}
        };
        
        // Update battery periodically
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SystemTrayModel::updateStatus);
        timer->start(5000);
    }
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_items.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_items.count())
            return QVariant();
        
        const TrayItem &item = m_items.at(index.row());
        switch (role) {
        case IconRole: return item.icon;
        case NameRole: return item.name;
        case StatusRole: return item.status;
        }
        return QVariant();
    }
    
    QHash<int, QByteArray> roleNames() const override {
        return {
            {IconRole, "icon"},
            {NameRole, "name"},
            {StatusRole, "status"}
        };
    }
    
    Q_INVOKABLE void clickItem(int index) {
        if (index >= 0 && index < m_items.count()) {
            qDebug() << "Clicked:" << m_items[index].name;
            // Open corresponding settings
        }
    }
    
private slots:
    void updateStatus() {
        // Simulate battery drain
        if (m_items[1].status.contains("%")) {
            int battery = m_items[1].status.left(2).toInt();
            battery = qMax(0, battery - 1);
            m_items[1].status = QString::number(battery) + "%";
            
            QModelIndex idx = index(1, 0);
            emit dataChanged(idx, idx, {StatusRole});
        }
    }
    
private:
    QList<TrayItem> m_items;
};
```

### View (QML)

```qml
// SystemTray.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 180
    height: 35
    color: "#2d2d2d"
    radius: 5
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        spacing: 3
        
        Repeater {
            model: trayModel
            
            delegate: Button {
                Layout.preferredWidth: 50
                Layout.fillHeight: true
                
                contentItem: Column {
                    spacing: 2
                    
                    Text {
                        text: model.icon
                        font.pixelSize: 16
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    
                    Text {
                        text: model.status
                        color: "white"
                        font.pixelSize: 8
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                
                background: Rectangle {
                    color: parent.hovered ? "#4d4d4d" : "transparent"
                    radius: 3
                }
                
                ToolTip.visible: hovered
                ToolTip.text: model.name + ": " + model.status
                
                onClicked: trayModel.clickItem(index)
            }
        }
    }
}
```

### Main (C++)

```cpp
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include "SystemTrayModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    SystemTrayModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("trayModel", &model);
    view.setSource(QUrl("qrc:/SystemTray.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setColor(Qt::transparent);
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(0);
    layerWindow->setMargins(QMargins(0, 0, 10, 0));
    
    view.show();
    return app.exec();
}
```

---

## Summary: All Examples với Model-View

| Example | Model Class | View File | Use Case |
|---------|------------|-----------|----------|
| 1. Panel | PanelModel | Panel.qml | Top panel với clock |
| 2. Dock | DockModel | Dock.qml | App launcher |
| 3. Widget | WidgetModel | Widget.qml | System monitor |
| 4. Wallpaper | WallpaperModel | Wallpaper.qml | Animated bg |
| 5. Notification | NotificationModel | Notification.qml | Toast notif |
| 6. Corner Widget | CornerWidgetModel | CornerWidget.qml | Quick actions |
| 7. Multi-Monitor | MultiPanelModel | MultiPanel.qml | Per-screen |
| 8. Dynamic Panel | DynamicPanelModel | DynamicPanel.qml | Expand/collapse |
| 9. Lock Screen | LockScreenModel | LockScreen.qml | Screen lock |
| 10. System Tray | SystemTrayModel | SystemTray.qml | Status icons |

---

## Build Configuration

### CMakeLists.txt Template

```cmake
cmake_minimum_required(VERSION 3.16)
project(ExampleName)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Quick Qml)
find_package(LayerShellQt REQUIRED)

qt5_add_resources(RESOURCES resources.qrc)

add_executable(examplename 
    main.cpp
    ModelClass.h
    ${RESOURCES}
)

target_link_libraries(examplename
    Qt5::Quick
    Qt5::Qml
    LayerShellQt::Interface
)
```

### resources.qrc Template

```xml
<RCC>
    <qresource prefix="/">
        <file>ViewFile.qml</file>
    </qresource>
</RCC>
```

---

## Common Patterns

### Pattern 1: Simple Property Model

```cpp
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value READ value NOTIFY valueChanged)
signals:
    void valueChanged();
};
```

### Pattern 2: List Model

```cpp
class ListModel : public QAbstractListModel {
    Q_OBJECT
    // rowCount(), data(), roleNames()
};
```

### Pattern 3: Complex Model with Actions

```cpp
class ActionModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(...)
    Q_INVOKABLE void doAction();
signals:
    void actionCompleted();
};
```

---

## Advantages of QML Architecture

### ✅ Separation of Concerns
- **Model (C++)**: Data, logic, system calls
- **View (QML)**: UI, presentation, animations

### ✅ Declarative UI
```qml
// Easy to read and modify
Rectangle {
    color: "#2d2d2d"
    Text { text: model.value }
    Button { onClicked: model.action() }
}
```

### ✅ Data Binding
```qml
// Auto-updates when model changes
Text { text: model.currentTime }
```

### ✅ Hot Reload
- Change QML → See changes without recompile
- Fast iteration

### ✅ Designer-Friendly
- Qt Designer support
- Visual editing
- Non-programmers can design UI

---

## Next Steps

Sau khi học 10 examples này:
1. Combine multiple examples (panel + tray + notifications)
2. Customize UI theo ý thích
3. Add more features to models
4. Build complete desktop shell!

**Happy coding với QML + Model-View! 🎨**
