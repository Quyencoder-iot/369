# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model-View Patterns](#model-view-patterns)
3. [QML Best Practices](#qml-best-practices)
4. [Performance Optimization](#performance-optimization)
5. [Error Handling](#error-handling)
6. [Testing Strategies](#testing-strategies)
7. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Strict Model-View Separation

**Always separate business logic (C++ Model) from presentation (QML View)!**

**✅ GOOD: Clear Separation**
```cpp
// Model (C++) - Business logic only
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
public:
    QString time() const { return m_time; }
    
private slots:
    void updateTime() {
        m_time = QTime::currentTime().toString();
        emit timeChanged();
    }
    
private:
    QString m_time;
};
```

```qml
// View (QML) - UI only
Rectangle {
    Text {
        text: panelModel.time  // Just display data
    }
}
```

**✗ BAD: Mixed Logic**
```qml
// DON'T put business logic in QML
Rectangle {
    Text {
        text: {
            // Bad: Time calculation in QML
            var now = new Date();
            return now.getHours() + ":" + now.getMinutes();
        }
    }
}
```

**Rule:** 
- **Model (C++)**: Data, logic, system calls, timers
- **View (QML)**: Display, animations, user interactions

---

### Principle 2: Use Properties for Data Exposure

**Expose data via Q_PROPERTY, not direct methods!**

**✅ GOOD: Q_PROPERTY**
```cpp
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    int count() const { return m_count; }
    
signals:
    void countChanged();
    
private:
    int m_count = 0;
};
```

```qml
Text { text: model.count }  // Auto-binding!
```

**✗ BAD: Direct Method Calls**
```cpp
class Model : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE int getCount() const { return m_count; }
};
```

```qml
// Bad: Manual calls, no auto-update
Text { 
    text: model.getCount()
    // Won't update when model changes!
}
```

**Why?** Q_PROPERTY với NOTIFY signal enables automatic binding updates!

---

### Principle 3: Configure LayerShell in C++, Not QML

**LayerShellQt configuration should be in C++ main, not QML!**

**✅ GOOD: C++ Configuration**
```cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    Model model;
    QQuickView view;
    view.rootContext()->setContextProperty("model", &model);
    view.setSource(QUrl("qrc:/View.qml"));
    
    // Configure LayerShell here
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop |
        LayerShellQt::Window::AnchorLeft |
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(40);
    
    view.show();
    return app.exec();
}
```

**✗ BAD: Trying to Configure in QML**
```qml
// This won't work!
Window {
    // Can't access LayerShellQt from QML
}
```

**Why?** LayerShellQt is C++ only, must configure before/during window creation.

---

### Principle 4: Use Appropriate Layer for Use Case

**Choose layer based on purpose, document why!**

```cpp
// ✅ GOOD: Clear reasoning
class PanelModel : public QObject {
    // Panel should be above apps, reserve space
    static constexpr auto LAYER = LayerShellQt::Window::LayerTop;
    static constexpr int EXCLUSIVE_ZONE = 40;
};

// In main:
layerWindow->setLayer(PanelModel::LAYER);
layerWindow->setExclusiveZone(PanelModel::EXCLUSIVE_ZONE);
```

**Layer decision matrix:**

| Use Case | Layer | Exclusive Zone | Reasoning |
|----------|-------|----------------|-----------|
| Panel | TOP | Height | Above apps, reserve space |
| Dock | TOP | Height | Above apps, reserve space |
| Notification | TOP | 0 | Above apps, no reservation |
| Widget | BOTTOM | 0 | Below apps, decorative |
| Wallpaper | BACKGROUND | 0 | Lowest layer |
| Lock Screen | OVERLAY | -1 | Block everything |

---

## Model-View Patterns

### Pattern 1: Simple Property Model

**For single-value data sources**

```cpp
// SimpleModel.h
class ClockModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
    
public:
    explicit ClockModel(QObject *parent = nullptr) : QObject(parent) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ClockModel::updateTime);
        timer->start(1000);
        updateTime();
    }
    
    QString currentTime() const { return m_currentTime; }
    
signals:
    void timeChanged();
    
private:
    void updateTime() {
        QString newTime = QTime::currentTime().toString("hh:mm:ss");
        if (m_currentTime != newTime) {
            m_currentTime = newTime;
            emit timeChanged();
        }
    }
    
    QString m_currentTime;
};
```

```qml
// ClockView.qml
Text {
    text: clockModel.currentTime
    // Automatically updates every second!
}
```

**When to use:** Simple data, few properties

---

### Pattern 2: List Model

**For collections of items**

```cpp
// ListModel.h
class AppListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        IconRole,
        CommandRole
    };
    
    struct App {
        QString name;
        QString icon;
        QString command;
    };
    
    explicit AppListModel(QObject *parent = nullptr) 
        : QAbstractListModel(parent) {
        m_apps = {
            {"Browser", "🌐", "firefox"},
            {"Files", "📁", "dolphin"},
            {"Terminal", "⌨️", "konsole"}
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
        case NameRole: return app.name;
        case IconRole: return app.icon;
        case CommandRole: return app.command;
        }
        return QVariant();
    }
    
    QHash<int, QByteArray> roleNames() const override {
        return {
            {NameRole, "name"},
            {IconRole, "icon"},
            {CommandRole, "command"}
        };
    }
    
    Q_INVOKABLE void launchApp(int index) {
        if (index >= 0 && index < m_apps.count()) {
            QProcess::startDetached(m_apps[index].command, {});
        }
    }
    
private:
    QList<App> m_apps;
};
```

```qml
// AppListView.qml
Repeater {
    model: appListModel
    
    delegate: Button {
        text: model.icon
        ToolTip.text: model.name
        onClicked: appListModel.launchApp(index)
    }
}
```

**When to use:** Multiple items, lists, grids

---

### Pattern 3: Action Model

**For user interactions and commands**

```cpp
// ActionModel.h
class PanelActionModel : public QObject {
    Q_OBJECT
    
public:
    explicit PanelActionModel(QObject *parent = nullptr) 
        : QObject(parent) {}
    
    Q_INVOKABLE void openSettings() {
        QProcess::startDetached("systemsettings5", {});
    }
    
    Q_INVOKABLE void takeScreenshot() {
        QProcess::startDetached("spectacle", {"-r"});
    }
    
    Q_INVOKABLE void lockScreen() {
        QProcess::startDetached("loginctl", {"lock-session"});
    }
    
signals:
    void actionExecuted(const QString &actionName);
    
private:
    void logAction(const QString &action) {
        qDebug() << "Action executed:" << action;
        emit actionExecuted(action);
    }
};
```

```qml
// ActionView.qml
Column {
    Button {
        text: "⚙️ Settings"
        onClicked: actionModel.openSettings()
    }
    
    Button {
        text: "📸 Screenshot"
        onClicked: actionModel.takeScreenshot()
    }
    
    Button {
        text: "🔒 Lock"
        onClicked: actionModel.lockScreen()
    }
}
```

**When to use:** User actions, system commands

---

### Pattern 4: Composite Model

**Combining multiple data sources**

```cpp
// CompositeModel.h
class PanelCompositeModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryChanged)
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY networkChanged)
    
public:
    explicit PanelCompositeModel(QObject *parent = nullptr)
        : QObject(parent) {
        
        // Time updater
        QTimer *timeTimer = new QTimer(this);
        connect(timeTimer, &QTimer::timeout, this, &PanelCompositeModel::updateTime);
        timeTimer->start(1000);
        
        // Battery updater
        QTimer *batteryTimer = new QTimer(this);
        connect(batteryTimer, &QTimer::timeout, this, &PanelCompositeModel::updateBattery);
        batteryTimer->start(30000);  // Every 30s
        
        // Network updater
        QTimer *networkTimer = new QTimer(this);
        connect(networkTimer, &QTimer::timeout, this, &PanelCompositeModel::updateNetwork);
        networkTimer->start(5000);  // Every 5s
        
        updateTime();
        updateBattery();
        updateNetwork();
    }
    
    QString time() const { return m_time; }
    int batteryLevel() const { return m_batteryLevel; }
    QString networkStatus() const { return m_networkStatus; }
    
signals:
    void timeChanged();
    void batteryChanged();
    void networkChanged();
    
private:
    void updateTime() { /* ... */ }
    void updateBattery() { /* ... */ }
    void updateNetwork() { /* ... */ }
    
    QString m_time;
    int m_batteryLevel = 0;
    QString m_networkStatus;
};
```

```qml
// CompositeView.qml
RowLayout {
    Text { text: compositeModel.time }
    Text { text: "🔋 " + compositeModel.batteryLevel + "%" }
    Text { text: "📶 " + compositeModel.networkStatus }
}
```

**When to use:** Multiple related data sources

---

### Pattern 5: View Manager

**Managing multiple QQuickViews**

```cpp
// ViewManager.h
class PanelViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit PanelViewManager(QAbstractListModel *model, QObject *parent = nullptr)
        : QObject(parent), m_model(model) {
        
        // Create views for all screens
        for (int i = 0; i < model->rowCount(); ++i) {
            QString screenName = model->data(model->index(i, 0), Qt::UserRole).toString();
            createView(screenName);
        }
        
        // Watch for model changes
        connect(model, &QAbstractListModel::rowsInserted,
                this, &PanelViewManager::onRowsInserted);
        connect(model, &QAbstractListModel::rowsRemoved,
                this, &PanelViewManager::onRowsRemoved);
    }
    
    ~PanelViewManager() {
        qDeleteAll(m_views);
    }
    
private:
    void createView(const QString &screenName) {
        QQuickView *view = new QQuickView();
        
        // Expose model to QML
        view->rootContext()->setContextProperty("panelModel", m_model);
        view->rootContext()->setContextProperty("currentScreen", screenName);
        
        // Load QML
        view->setSource(QUrl("qrc:/Panel.qml"));
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        // Configure LayerShell
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(40);
        layerWindow->setScope(screenName);  // Per-screen
        
        view->show();
        m_views.insert(screenName, view);
    }
    
    void removeView(const QString &screenName) {
        QQuickView *view = m_views.take(screenName);
        if (view) view->deleteLater();
    }
    
    void onRowsInserted(const QModelIndex &parent, int first, int last) {
        for (int i = first; i <= last; ++i) {
            QString screenName = m_model->data(m_model->index(i, 0), Qt::UserRole).toString();
            createView(screenName);
        }
    }
    
    void onRowsRemoved(const QModelIndex &parent, int first, int last) {
        // Handle removal
    }
    
private:
    QAbstractListModel *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

**When to use:** Multi-monitor, multiple windows

---

## QML Best Practices

### Practice 1: Use Proper ID Naming

**✅ GOOD: Clear, descriptive IDs**
```qml
Rectangle {
    id: mainPanel
    
    Text {
        id: clockLabel
        text: model.time
    }
    
    Button {
        id: settingsButton
        text: "⚙️"
    }
}
```

**✗ BAD: Generic or missing IDs**
```qml
Rectangle {
    id: rect1  // Too generic
    
    Text {
        // No ID - can't reference
    }
}
```

---

### Practice 2: Use Layouts, Not Absolute Positioning

**✅ GOOD: Layouts**
```qml
Rectangle {
    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        Text { text: "Left" }
        Item { Layout.fillWidth: true }
        Text { text: "Right" }
    }
}
```

**✗ BAD: Absolute positioning**
```qml
Rectangle {
    Text {
        x: 10
        y: 5
        text: "Left"
    }
    
    Text {
        x: parent.width - width - 10  // Brittle!
        y: 5
        text: "Right"
    }
}
```

---

### Practice 3: Use Property Bindings

**✅ GOOD: Declarative bindings**
```qml
Rectangle {
    width: parent.width
    height: 40
    color: mouseArea.containsMouse ? "#4d4d4d" : "#2d2d2d"
    
    Text {
        anchors.centerIn: parent
        text: model.value
        color: model.isActive ? "white" : "gray"
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
```

**✗ BAD: Imperative updates**
```qml
Rectangle {
    id: rect
    
    MouseArea {
        onEntered: {
            rect.color = "#4d4d4d"  // Manual update
        }
        onExited: {
            rect.color = "#2d2d2d"
        }
    }
}
```

---

### Practice 4: Use Behavior for Animations

**✅ GOOD: Declarative animations**
```qml
Rectangle {
    width: expanded ? 300 : 200
    height: 40
    color: hovered ? "#4d4d4d" : "#2d2d2d"
    
    Behavior on width {
        NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
    }
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
}
```

**✗ BAD: Manual animations**
```qml
Rectangle {
    id: rect
    
    function expand() {
        animation.start()
    }
    
    NumberAnimation {
        id: animation
        target: rect
        property: "width"
        to: 300
        duration: 300
    }
}
```

---

### Practice 5: Avoid JavaScript Logic in QML

**✅ GOOD: Minimal QML logic**
```qml
Text {
    text: model.formattedTime  // Model does formatting
    color: model.isWarning ? "red" : "white"
}
```

**✗ BAD: Complex JavaScript**
```qml
Text {
    text: {
        // Complex logic in QML - BAD!
        var time = new Date();
        var hours = time.getHours();
        var mins = time.getMinutes();
        if (hours < 10) hours = "0" + hours;
        if (mins < 10) mins = "0" + mins;
        return hours + ":" + mins;
    }
}
```

**Rule:** If logic is complex, put it in C++ model!

---

### Practice 6: Component Reusability

**✅ GOOD: Reusable components**
```qml
// StatusIcon.qml - Reusable component
import QtQuick 2.15

Rectangle {
    id: root
    
    property string icon: ""
    property string tooltip: ""
    property bool active: false
    
    width: 30
    height: 30
    color: active ? "#4d4d4d" : "transparent"
    radius: 5
    
    Text {
        anchors.centerIn: parent
        text: root.icon
        font.pixelSize: 16
    }
    
    ToolTip.visible: mouseArea.containsMouse
    ToolTip.text: root.tooltip
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
```

```qml
// Usage
RowLayout {
    StatusIcon {
        icon: "🔊"
        tooltip: "Volume"
        active: model.volumeActive
    }
    
    StatusIcon {
        icon: "🔋"
        tooltip: "Battery: " + model.battery + "%"
        active: model.batteryLow
    }
}
```

---

### Practice 7: Use States for Complex UI

**✅ GOOD: States**
```qml
Rectangle {
    id: panel
    
    states: [
        State {
            name: "collapsed"
            PropertyChanges { target: panel; height: 40 }
            PropertyChanges { target: expandedContent; visible: false }
        },
        State {
            name: "expanded"
            PropertyChanges { target: panel; height: 200 }
            PropertyChanges { target: expandedContent; visible: true }
        }
    ]
    
    state: model.isExpanded ? "expanded" : "collapsed"
    
    transitions: Transition {
        NumberAnimation { properties: "height"; duration: 300 }
    }
}
```

---

## Performance Optimization

### Optimization 1: Use Loaders for Heavy Content

**✅ GOOD: Lazy loading**
```qml
Rectangle {
    Loader {
        id: settingsLoader
        active: false
        source: "Settings.qml"
    }
    
    Button {
        text: "⚙️"
        onClicked: settingsLoader.active = !settingsLoader.active
    }
}
```

**✗ BAD: Always loaded**
```qml
Rectangle {
    Settings {
        visible: false  // Still loaded!
    }
}
```

---

### Optimization 2: Minimize Property Bindings

**✅ GOOD: Simple bindings**
```qml
Text {
    text: model.value
}
```

**✗ BAD: Complex bindings**
```qml
Text {
    text: {
        // Recalculated on every property change!
        var result = "";
        for (var i = 0; i < 100; i++) {
            result += model.getValue(i);
        }
        return result;
    }
}
```

**Fix:** Calculate in C++ model, expose as property!

---

### Optimization 3: Use ListView Instead of Repeater

**For large lists:**

**✅ GOOD: ListView (virtualized)**
```qml
ListView {
    model: largeListModel
    delegate: ItemDelegate {
        text: model.name
    }
}
```

**✗ BAD: Repeater (all items created)**
```qml
Column {
    Repeater {
        model: largeListModel  // Creates ALL items!
        delegate: Text { text: model.name }
    }
}
```

---

### Optimization 4: Batch Model Updates

**✅ GOOD: Batch updates**
```cpp
void Model::updateMultiple() {
    emit beginResetModel();
    // Update all data
    m_data = newData;
    emit endResetModel();
}
```

**✗ BAD: Multiple signals**
```cpp
void Model::updateMultiple() {
    for (int i = 0; i < count; ++i) {
        m_data[i] = newData[i];
        emit dataChanged(index(i), index(i));  // Many signals!
    }
}
```

---

### Optimization 5: Use C++ for Heavy Computation

**✅ GOOD: C++ computation**
```cpp
class Model : public QObject {
    Q_PROPERTY(QVariantList processedData READ processedData NOTIFY dataChanged)
    
    QVariantList processedData() const {
        // Heavy computation in C++
        QVariantList result;
        for (const auto &item : m_data) {
            result.append(processItem(item));
        }
        return result;
    }
};
```

```qml
Repeater {
    model: dataModel.processedData  // Already processed!
}
```

**✗ BAD: QML computation**
```qml
Repeater {
    model: rawDataModel
    delegate: Item {
        // Heavy computation in QML - BAD!
        property var processed: processInJavaScript(model.data)
    }
}
```

---

## Error Handling

### Error 1: Model Not Available

**✅ GOOD: Check availability**
```cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qCritical() << "Layer shell not available!";
        
        // Show error dialog or fallback
        QMessageBox::critical(nullptr, "Error",
            "LayerShellQt not available.\n"
            "Please use a supported Wayland compositor.");
        return 1;
    }
    
    // Continue with setup...
}
```

---

### Error 2: QML Loading Errors

**✅ GOOD: Check for errors**
```cpp
QQuickView view;
view.setSource(QUrl("qrc:/View.qml"));

if (view.status() == QQuickView::Error) {
    qCritical() << "Failed to load QML:";
    for (const QQmlError &error : view.errors()) {
        qCritical() << "  " << error.toString();
    }
    return 1;
}
```

---

### Error 3: Property Binding Errors

**✅ GOOD: Validate in model**
```cpp
class Model : public QObject {
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    
    void setValue(int value) {
        if (value < 0 || value > 100) {
            qWarning() << "Invalid value:" << value;
            return;
        }
        
        if (m_value != value) {
            m_value = value;
            emit valueChanged();
        }
    }
};
```

---

### Error 4: Context Property Not Found

**✅ GOOD: Verify context properties**
```cpp
Model model;
view.rootContext()->setContextProperty("panelModel", &model);

// Verify in QML with error checking
```

```qml
Component.onCompleted: {
    if (typeof panelModel === "undefined") {
        console.error("panelModel not found!");
    }
}
```

---

## Testing Strategies

### Strategy 1: Unit Test Models

**Test models independently of UI:**

```cpp
// test_model.cpp
#include <QtTest>
#include "PanelModel.h"

class TestPanelModel : public QObject {
    Q_OBJECT
    
private slots:
    void testTimeProperty() {
        PanelModel model;
        
        QString time = model.currentTime();
        QVERIFY(!time.isEmpty());
        QVERIFY(time.contains(":"));
    }
    
    void testTimeUpdates() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        // Wait for signal
        QVERIFY(spy.wait(2000));
        QVERIFY(spy.count() > 0);
    }
    
    void testPropertyBinding() {
        PanelModel model;
        
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        model.updateTime();  // Trigger update
        
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(TestPanelModel)
#include "test_model.moc"
```

---

### Strategy 2: Test QML Components

**Use qmlscene for QML testing:**

```qml
// TestView.qml
import QtQuick 2.15
import QtTest 1.15

TestCase {
    name: "PanelTests"
    
    Component {
        id: panelComponent
        Panel {
            // Mock model
            panelModel: QtObject {
                property string time: "12:34:56"
            }
        }
    }
    
    function test_displayTime() {
        var panel = createTemporaryObject(panelComponent, this);
        verify(panel !== null);
        
        var clockText = findChild(panel, "clockLabel");
        compare(clockText.text, "12:34:56");
    }
}
```

---

### Strategy 3: Integration Tests

**Test model + view integration:**

```cpp
// integration_test.cpp
#include <QtTest>
#include <QQuickView>
#include <QQmlContext>
#include "PanelModel.h"

class IntegrationTest : public QObject {
    Q_OBJECT
    
private slots:
    void testModelViewIntegration() {
        PanelModel model;
        
        QQuickView view;
        view.rootContext()->setContextProperty("panelModel", &model);
        view.setSource(QUrl("qrc:/Panel.qml"));
        
        QCOMPARE(view.status(), QQuickView::Ready);
        
        view.show();
        QVERIFY(QTest::qWaitForWindowExposed(&view));
    }
};

QTEST_MAIN(IntegrationTest)
```

---

### Strategy 4: Mock Models for UI Testing

**Create mock models for UI development:**

```cpp
// MockModel.h
class MockPanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time CONSTANT)
    Q_PROPERTY(int battery READ battery CONSTANT)
    
public:
    QString time() const { return "12:34:56"; }
    int battery() const { return 75; }
};
```

```cpp
// In test or development mode:
#ifdef DEVELOPMENT_MODE
    MockPanelModel model;
#else
    PanelModel model;
#endif

view.rootContext()->setContextProperty("panelModel", &model);
```

---

## Common Pitfalls

### Pitfall 1: Forgetting to Emit Signals

**❌ WRONG:**
```cpp
class Model : public QObject {
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    
    void setValue(int value) {
        m_value = value;
        // Forgot to emit valueChanged()!
    }
};
```

**✅ CORRECT:**
```cpp
void setValue(int value) {
    if (m_value != value) {
        m_value = value;
        emit valueChanged();  // Don't forget!
    }
}
```

---

### Pitfall 2: Circular Property Bindings

**❌ WRONG:**
```qml
Rectangle {
    width: height + 10
    height: width - 10  // Circular!
}
```

**✅ CORRECT:**
```qml
Rectangle {
    property int baseSize: 100
    width: baseSize
    height: baseSize - 10
}
```

---

### Pitfall 3: Not Using Const Correctness

**❌ WRONG:**
```cpp
QString time() { return m_time; }  // Should be const!
```

**✅ CORRECT:**
```cpp
QString time() const { return m_time; }
```

---

### Pitfall 4: Exposing C++ Objects Directly

**❌ WRONG:**
```cpp
view.rootContext()->setContextProperty("screen", screen);
// QScreen* exposed directly - can crash if screen deleted!
```

**✅ CORRECT:**
```cpp
// Wrap in model
class ScreenModel : public QObject {
    Q_PROPERTY(QString name READ name CONSTANT)
    
    QString name() const { return m_screen->name(); }
    
private:
    QScreen *m_screen;
};
```

---

### Pitfall 5: Memory Leaks in View Manager

**❌ WRONG:**
```cpp
void createView() {
    QQuickView *view = new QQuickView();
    // Forgot to store and delete later!
}
```

**✅ CORRECT:**
```cpp
~ViewManager() {
    qDeleteAll(m_views);  // Cleanup!
}
```

---

### Pitfall 6: Not Setting ResizeMode

**❌ WRONG:**
```cpp
QQuickView view;
view.setSource(QUrl("qrc:/View.qml"));
// QML root item size ignored!
```

**✅ CORRECT:**
```cpp
view.setResizeMode(QQuickView::SizeRootObjectToView);
// Or:
view.setResizeMode(QQuickView::SizeViewToRootObject);
```

---

### Pitfall 7: Hardcoding Screen Dimensions

**❌ WRONG:**
```qml
Rectangle {
    width: 1920  // Hardcoded!
    height: 40
}
```

**✅ CORRECT:**
```qml
Rectangle {
    // Let LayerShell anchors determine width
    height: 40
}
```

---

## Production Checklist

Before deploying, verify:

### Model Checklist
- [ ] All properties have NOTIFY signals
- [ ] Const correctness on getters
- [ ] No memory leaks
- [ ] Error handling for system calls
- [ ] Unit tests pass

### View Checklist
- [ ] No complex JavaScript logic
- [ ] Uses layouts, not absolute positioning
- [ ] Proper ID naming
- [ ] Animations use Behavior
- [ ] Reusable components extracted

### Integration Checklist
- [ ] Check `isAvailable()` before use
- [ ] Set ResizeMode correctly
- [ ] Configure LayerShell before show
- [ ] Handle QML loading errors
- [ ] Proper cleanup in destructors

### Performance Checklist
- [ ] Heavy computation in C++
- [ ] Use ListView for large lists
- [ ] Lazy loading with Loader
- [ ] Batch model updates
- [ ] Minimal property bindings

---

## Summary: Golden Rules

### Architecture
1. **Strict Model-View separation**
2. **Business logic in C++, UI in QML**
3. **Configure LayerShell in C++**

### Models
4. **Use Q_PROPERTY with NOTIFY**
5. **Const correctness**
6. **Emit signals on changes**

### QML
7. **Declarative bindings, not imperative**
8. **Use Layouts**
9. **Minimal JavaScript**
10. **Reusable components**

### Performance
11. **Heavy computation in C++**
12. **ListView for large lists**
13. **Lazy loading**

### Testing
14. **Unit test models**
15. **Mock models for UI testing**
16. **Integration tests**

---

## Next Steps

You're now ready for production! Check:
- **[06-quick-reference.md](./06-quick-reference.md)** - Quick lookup

**Happy coding with clean architecture! 🎨**
