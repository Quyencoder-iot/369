# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model-View Patterns](#model-view-patterns)
3. [QML Best Practices](#qml-best-practices)
4. [UI/UX Guidelines](#uiux-guidelines)
5. [Performance Optimization](#performance-optimization)
6. [Error Handling](#error-handling)
7. [Testing Strategies](#testing-strategies)
8. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Separate Model from View

**Always separate business logic (C++ Model) from UI (QML View)!**

```cpp
// ✓ GOOD: Model handles logic
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
private:
    QString m_time;
    QTimer *m_timer;
    
    void updateTime() {
        m_time = QTime::currentTime().toString();
        emit timeChanged();
    }
};
```

```qml
// ✓ GOOD: View only displays
Rectangle {
    Text { text: panelModel.time }  // Pure UI
}
```

```cpp
// ✗ BAD: Logic in QML
Rectangle {
    Text {
        text: {
            // Don't do complex logic in QML!
            var time = new Date();
            return time.getHours() + ":" + time.getMinutes();
        }
    }
}
```

**Why?**
- Business logic testable independently
- UI can change without affecting logic
- Clear responsibilities

---

### Principle 2: Use Q_PROPERTY for Data Binding

**Expose data via Q_PROPERTY for automatic QML binding**

```cpp
// ✓ GOOD: Q_PROPERTY with NOTIFY
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(int value READ value NOTIFY valueChanged)
    
public:
    int value() const { return m_value; }
    
signals:
    void valueChanged();
    
private:
    int m_value;
};
```

```qml
// QML automatically updates when valueChanged emitted
Text { text: model.value }
```

```cpp
// ✗ BAD: Getter without NOTIFY
Q_PROPERTY(int value READ value)  // No NOTIFY = no auto-update!
```

**Always include NOTIFY signal for mutable properties!**

---

### Principle 3: Configure LayerShell Before Show

**Set all LayerShellQt properties BEFORE showing window**

```cpp
// ✓ GOOD
QQuickView view;
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(/* ... */);
view.show();  // Configure first, then show

// ✗ BAD
QQuickView view;
view.show();  // Show first = wrong!
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);  // Too late!
```

---

### Principle 4: Use Appropriate Layer

**Choose layer based on z-order needs**

| Use Case | Layer | Reasoning |
|----------|-------|-----------|
| Panel/Dock | TOP | Above apps, needs exclusive zone |
| Notification | TOP | Above apps, no exclusive zone |
| Widget | BOTTOM | Below apps, decorative |
| Wallpaper | BACKGROUND | Lowest layer |
| Lock screen | OVERLAY | Must block everything |

```cpp
// ✓ GOOD: Correct layer choice
layerWindow->setLayer(LayerShellQt::Window::LayerTop);  // For panel

// ✗ BAD: Overkill
layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);  // For simple panel - too aggressive!
```

---

## Model-View Patterns

### Pattern 1: Simple Property Model

**For single-window apps with simple data**

```cpp
// SimpleModel.h
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    
public:
    explicit SimpleModel(QObject *parent = nullptr)
        : QObject(parent), m_title("My App") {}
    
    QString title() const { return m_title; }
    QString status() const { return m_status; }
    
    Q_INVOKABLE void doSomething() {
        m_status = "Processing...";
        emit statusChanged();
        // ... do work
        m_status = "Done";
        emit statusChanged();
    }
    
signals:
    void statusChanged();
    
private:
    QString m_title;
    QString m_status;
};
```

```qml
// View.qml
Rectangle {
    Text { text: model.title }
    Text { text: model.status }
    Button {
        text: "Action"
        onClicked: model.doSomething()
    }
}
```

```cpp
// main.cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    SimpleModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("model", &model);
    view.setSource(QUrl("qrc:/View.qml"));
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    
    view.show();
    return app.exec();
}
```

**When to use:** Single window, simple data, few properties

---

### Pattern 2: List Model

**For repeating data (menus, icons, notifications)**

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
    
    struct AppInfo {
        QString name;
        QString icon;
        QString command;
    };
    
    explicit AppListModel(QObject *parent = nullptr)
        : QAbstractListModel(parent) {
        m_apps = {
            {"Firefox", "🌐", "firefox"},
            {"Terminal", "⌨️", "konsole"},
            {"Files", "📁", "dolphin"}
        };
    }
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_apps.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_apps.count())
            return QVariant();
        
        const AppInfo &app = m_apps.at(index.row());
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
    
    Q_INVOKABLE void launch(int index) {
        if (index >= 0 && index < m_apps.count()) {
            QProcess::startDetached(m_apps[index].command, {});
        }
    }
    
private:
    QList<AppInfo> m_apps;
};
```

```qml
// Dock.qml
Rectangle {
    RowLayout {
        Repeater {
            model: appListModel
            
            delegate: Button {
                contentItem: Text {
                    text: model.icon
                    font.pixelSize: 24
                }
                ToolTip.text: model.name
                onClicked: appListModel.launch(index)
            }
        }
    }
}
```

**When to use:** Dynamic lists, repeated items, data collections

---

### Pattern 3: Multi-Window Manager

**For managing multiple windows (multi-monitor panels)**

```cpp
// WindowManager.h
class WindowManager : public QObject {
    Q_OBJECT
    
public:
    WindowManager(QObject *parent = nullptr) : QObject(parent) {
        // Create window for each screen
        for (QScreen *screen : QGuiApplication::screens()) {
            createWindow(screen);
        }
        
        // Handle screen hotplug
        connect(qApp, &QGuiApplication::screenAdded,
                this, &WindowManager::createWindow);
        connect(qApp, &QGuiApplication::screenRemoved,
                this, &WindowManager::destroyWindow);
    }
    
    ~WindowManager() {
        qDeleteAll(m_views);
    }
    
private:
    void createWindow(QScreen *screen) {
        QString screenName = screen->name();
        if (m_views.contains(screenName)) return;
        
        QQuickView *view = new QQuickView();
        
        // Expose screen-specific data
        view->rootContext()->setContextProperty("screenName", screenName);
        view->rootContext()->setContextProperty("sharedModel", &m_sharedModel);
        
        view->setSource(QUrl("qrc:/Panel.qml"));
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        // Configure layer shell
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(35);
        layerWindow->setScope(screenName);  // Per-screen
        
        view->show();
        m_views.insert(screenName, view);
    }
    
    void destroyWindow(QScreen *screen) {
        QQuickView *view = m_views.take(screen->name());
        if (view) view->deleteLater();
    }
    
    QHash<QString, QQuickView*> m_views;
    SharedDataModel m_sharedModel;  // Shared across all windows
};
```

**When to use:** Multi-monitor apps, dynamic window creation

---

### Pattern 4: Singleton Model

**For shared state across multiple views**

```cpp
// GlobalState.h
class GlobalState : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isLocked READ isLocked WRITE setIsLocked NOTIFY isLockedChanged)
    
public:
    static GlobalState* instance() {
        static GlobalState inst;
        return &inst;
    }
    
    QString theme() const { return m_theme; }
    void setTheme(const QString &theme) {
        if (m_theme != theme) {
            m_theme = theme;
            emit themeChanged();
        }
    }
    
    bool isLocked() const { return m_isLocked; }
    void setIsLocked(bool locked) {
        if (m_isLocked != locked) {
            m_isLocked = locked;
            emit isLockedChanged();
        }
    }
    
signals:
    void themeChanged();
    void isLockedChanged();
    
private:
    GlobalState(QObject *parent = nullptr) : QObject(parent) {}
    QString m_theme = "dark";
    bool m_isLocked = false;
};
```

```cpp
// main.cpp
view->rootContext()->setContextProperty("globalState", GlobalState::instance());
```

```qml
// Any view can access
Rectangle {
    color: globalState.theme === "dark" ? "#2d2d2d" : "#ffffff"
}
```

**When to use:** Shared settings, global state, cross-window communication

---

### Pattern 5: Reactive Model

**Model that reacts to external changes**

```cpp
// ReactiveModel.h
class SystemMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(int cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(int memoryUsage READ memoryUsage NOTIFY memoryUsageChanged)
    
public:
    SystemMonitor(QObject *parent = nullptr) : QObject(parent) {
        // Update every 2 seconds
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SystemMonitor::update);
        timer->start(2000);
        update();
    }
    
    int cpuUsage() const { return m_cpuUsage; }
    int memoryUsage() const { return m_memoryUsage; }
    
signals:
    void cpuUsageChanged();
    void memoryUsageChanged();
    
private slots:
    void update() {
        // Read system stats
        int newCpu = readCpuUsage();
        if (m_cpuUsage != newCpu) {
            m_cpuUsage = newCpu;
            emit cpuUsageChanged();
        }
        
        int newMem = readMemoryUsage();
        if (m_memoryUsage != newMem) {
            m_memoryUsage = newMem;
            emit memoryUsageChanged();
        }
    }
    
    int m_cpuUsage = 0;
    int m_memoryUsage = 0;
};
```

```qml
// Widget.qml - Auto-updates when system changes
Rectangle {
    Text {
        text: "CPU: " + monitor.cpuUsage + "%"
        color: monitor.cpuUsage > 80 ? "red" : "white"
    }
}
```

**When to use:** System monitoring, live data, external events

---

## QML Best Practices

### Practice 1: Use Property Bindings

**Prefer declarative bindings over imperative updates**

```qml
// ✓ GOOD: Declarative binding
Rectangle {
    color: model.isDark ? "#2d2d2d" : "#ffffff"  // Auto-updates!
    opacity: model.isVisible ? 1.0 : 0.0
}

// ✗ BAD: Imperative update
Rectangle {
    id: rect
    
    Connections {
        target: model
        onIsDarkChanged: {
            rect.color = model.isDark ? "#2d2d2d" : "#ffffff"  // Manual!
        }
    }
}
```

**Bindings automatically update when properties change!**

---

### Practice 2: Avoid JavaScript Logic in QML

**Keep complex logic in C++ Model**

```cpp
// ✓ GOOD: Logic in C++ Model
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY timeChanged)
    
public:
    QString formattedTime() const {
        QTime time = QTime::currentTime();
        if (m_use24Hour) {
            return time.toString("HH:mm:ss");
        } else {
            return time.toString("hh:mm:ss AP");
        }
    }
};
```

```qml
// ✓ GOOD: Simple display
Text { text: model.formattedTime }

// ✗ BAD: Complex JS logic
Text {
    text: {
        var time = new Date();
        var hours = time.getHours();
        var ampm = hours >= 12 ? "PM" : "AM";
        hours = hours % 12 || 12;
        return hours + ":" + time.getMinutes() + " " + ampm;
    }
}
```

**Why?**
- JS in QML is slower
- Hard to test
- Hard to maintain
- C++ is faster and testable

---

### Practice 3: Use Loaders for Dynamic Content

**Lazy-load heavy components**

```qml
// ✓ GOOD: Lazy loading with Loader
Rectangle {
    Loader {
        id: settingsLoader
        active: false  // Not loaded initially
        source: "Settings.qml"
    }
    
    Button {
        text: "Settings"
        onClicked: settingsLoader.active = true
    }
}

// ✗ BAD: Always loaded
Rectangle {
    Settings {
        id: settings
        visible: false  // Still loaded and using memory!
    }
}
```

**Benefits:**
- Faster startup
- Lower memory usage
- Load on demand

---

### Practice 4: Minimize Property Bindings

**Avoid binding chains and complex expressions**

```qml
// ✓ GOOD: Simple bindings
Rectangle {
    width: parent.width
    height: model.panelHeight
}

// ✗ BAD: Complex binding chain
Rectangle {
    width: parent.parent.parent.width * 0.8 - someOffset + otherValue
    // Hard to understand, can cause performance issues
}

// ✓ BETTER: Use intermediate properties
Rectangle {
    property int targetWidth: parent.width * 0.8
    width: targetWidth
}
```

---

### Practice 5: Use Item IDs Sparingly

**Prefer property bindings over ID references**

```qml
// ✓ GOOD: Property-based
ColumnLayout {
    property int itemHeight: 30
    
    Rectangle { height: itemHeight }
    Rectangle { height: itemHeight }
    Rectangle { height: itemHeight }
}

// ✗ BAD: ID-based coupling
ColumnLayout {
    Rectangle { id: item1; height: 30 }
    Rectangle { height: item1.height }  // Tight coupling!
    Rectangle { height: item1.height }
}
```

---

### Practice 6: Optimize Repeater & ListView

**Use appropriate component for lists**

```qml
// ✓ GOOD: Repeater for small, static lists
Repeater {
    model: 5  // Small count
    delegate: Button { text: "Item " + index }
}

// ✓ GOOD: ListView for large, dynamic lists
ListView {
    model: 1000  // Large count
    delegate: ItemDelegate { text: model.name }
    cacheBuffer: 100  // Cache off-screen items
}

// ✗ BAD: Repeater for large lists
Repeater {
    model: 1000  // Creates 1000 items immediately - slow!
    delegate: Button { text: "Item " + index }
}
```

**Rule:** Repeater for < 50 items, ListView for more

---

## UI/UX Guidelines

### Guideline 1: Respect Exclusive Zones

**Use exclusive zones appropriately**

```cpp
// ✓ GOOD: Panel reserves space
layerWindow->setLayer(LayerTop);
layerWindow->setExclusiveZone(35);  // Apps avoid this area

// ✓ GOOD: Notification doesn't reserve
layerWindow->setLayer(LayerTop);
layerWindow->setExclusiveZone(0);  // Overlays apps

// ✗ BAD: Notification blocking screen
layerWindow->setLayer(LayerOverlay);
layerWindow->setExclusiveZone(500);  // Blocks too much!
```

---

### Guideline 2: Consistent Sizing

**Follow platform conventions**

```qml
// ✓ GOOD: Standard sizes
Rectangle {
    height: 35  // Standard panel height
}

// ✗ BAD: Odd sizes
Rectangle {
    height: 47.3  // Why this specific number?
}
```

**Common sizes:**
- Small panel: 28-32px
- Medium panel: 35-40px
- Large panel: 44-56px

---

### Guideline 3: Responsive Design

**Adapt to screen size and DPI**

```qml
// ✓ GOOD: Relative sizing
Rectangle {
    width: parent.width * 0.8
    height: parent.height / 10
}

// ✓ GOOD: DPI-aware fonts
Text {
    font.pixelSize: 12 * Screen.devicePixelRatio
}

// ✗ BAD: Hardcoded pixels
Rectangle {
    width: 1920  // Breaks on different resolutions!
    height: 1080
}
```

---

### Guideline 4: Smooth Animations

**Use animations for state transitions**

```qml
// ✓ GOOD: Smooth transition
Rectangle {
    height: model.expanded ? 200 : 35
    
    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
}

// ✗ BAD: Instant change
Rectangle {
    height: model.expanded ? 200 : 35  // Jarring!
}
```

**Always animate:**
- Height/width changes
- Opacity changes
- Position changes
- Color changes

---

### Guideline 5: Keyboard Focus

**Handle keyboard properly**

```qml
// ✓ GOOD: Focus management
TextField {
    id: input
    focus: true  // Auto-focus
    
    Keys.onEscapePressed: text = ""
    Keys.onReturnPressed: model.submit(text)
}

// ✗ BAD: No focus handling
TextField {
    // No focus set, user must click
}
```

```cpp
// Set keyboard interactivity in layer shell
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityExclusive
);
```

---

## Performance Optimization

### Optimization 1: Lazy Initialization

**Create components on-demand**

```qml
// ✓ GOOD: Lazy loading
Rectangle {
    Loader {
        id: heavyComponent
        active: false
        source: "HeavyComponent.qml"
    }
    
    Button {
        onClicked: heavyComponent.active = true
    }
}

// ✗ BAD: Always created
Rectangle {
    HeavyComponent {
        visible: false  // Still in memory!
    }
}
```

---

### Optimization 2: Minimize Property Bindings

**Cache computed values**

```qml
// ✓ GOOD: Cached property
Rectangle {
    property string formattedTime: model.time
    Text { text: formattedTime }
}

// ✗ BAD: Re-evaluated every frame
Text {
    text: model.hours + ":" + model.minutes + ":" + model.seconds
}
```

---

### Optimization 3: Use Appropriate Containers

**Choose right container for data**

```cpp
// ✓ GOOD: QAbstractListModel for large lists
class LargeListModel : public QAbstractListModel {
    // Efficient for thousands of items
};

// ✗ BAD: QList of QObjects as context properties
QList<QObject*> items;  // Slow for large lists!
view->rootContext()->setContextProperty("items", QVariant::fromValue(items));
```

---

### Optimization 4: Batch Updates

**Group property changes**

```cpp
// ✓ GOOD: Batch updates
void Model::updateAll() {
    beginResetModel();
    // Update all data
    endResetModel();  // Single update signal
}

// ✗ BAD: Multiple updates
void Model::updateAll() {
    for (int i = 0; i < count; ++i) {
        updateItem(i);  // Signals for each item!
    }
}
```

---

### Optimization 5: Reduce Repaints

**Minimize unnecessary updates**

```qml
// ✓ GOOD: Update only when changed
Text {
    text: model.value
    // Only repaints when model.value actually changes
}

// ✗ BAD: Constant updates
Text {
    text: new Date().toString()  // Updates every frame!
}
```

---

## Error Handling

### Pattern 1: Check Layer Shell Availability

**Always check before using**

```cpp
// ✓ GOOD: Check and fallback
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        QMessageBox::critical(nullptr, "Error",
            "Layer shell not available!\n"
            "This compositor doesn't support wlr-layer-shell.\n"
            "Please use KDE Plasma Wayland, Sway, or Hyprland.");
        return 1;
    }
    
    // ... continue
}

// ✗ BAD: Assume it works
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    // Crash if not available!
    auto layerWindow = LayerShellQt::Window::get(&view);
}
```

---

### Pattern 2: Validate Model Data

**Check data before using**

```cpp
// ✓ GOOD: Validation in model
Q_INVOKABLE void setVolume(int volume) {
    if (volume < 0 || volume > 100) {
        qWarning() << "Invalid volume:" << volume;
        return;
    }
    m_volume = volume;
    emit volumeChanged();
}

// ✗ BAD: No validation
Q_INVOKABLE void setVolume(int volume) {
    m_volume = volume;  // Could be -100 or 1000!
    emit volumeChanged();
}
```

---

### Pattern 3: Handle QML Loading Errors

**Check if QML loaded successfully**

```cpp
// ✓ GOOD: Error handling
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));

if (view.status() == QQuickView::Error) {
    qCritical() << "Failed to load QML:" << view.errors();
    return 1;
}

// ✗ BAD: Assume it loads
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));
view.show();  // Might show nothing!
```

---

### Pattern 4: Signal Connection Safety

**Check connections succeed**

```cpp
// ✓ GOOD: Check connection
bool connected = connect(model, &Model::dataChanged,
                        this, &Manager::onDataChanged);
if (!connected) {
    qWarning() << "Failed to connect signal!";
}

// ✗ BAD: Assume it works
connect(model, &Model::dataChanged,
        this, &Manager::onDataChanged);
```

---

## Testing Strategies

### Strategy 1: Unit Test Models

**Test business logic independently**

```cpp
// Test file: test_panelmodel.cpp
class TestPanelModel : public QObject {
    Q_OBJECT
    
private slots:
    void testTimeFormat() {
        PanelModel model;
        QString time = model.currentTime();
        
        // Verify format
        QVERIFY(!time.isEmpty());
        QVERIFY(time.contains(":"));
    }
    
    void testSignals() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        // Trigger update
        model.updateTime();
        
        // Verify signal emitted
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(TestPanelModel)
#include "test_panelmodel.moc"
```

**CMakeLists.txt:**
```cmake
find_package(Qt5 REQUIRED COMPONENTS Test)

add_executable(test_panelmodel test_panelmodel.cpp PanelModel.cpp)
target_link_libraries(test_panelmodel Qt5::Test)

add_test(NAME PanelModelTest COMMAND test_panelmodel)
```

---

### Strategy 2: Mock Models for QML Testing

**Test QML with mock data**

```cpp
// MockModel.h - For testing QML
class MockPanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime CONSTANT)
    
public:
    QString currentTime() const { return "12:34:56"; }  // Fixed test data
};
```

```cpp
// test_qml.cpp
void TestQML::testPanelUI() {
    QQmlApplicationEngine engine;
    MockPanelModel mockModel;
    
    engine.rootContext()->setContextProperty("panelModel", &mockModel);
    engine.load(QUrl("qrc:/Panel.qml"));
    
    QObject *root = engine.rootObjects().first();
    QVERIFY(root != nullptr);
    
    // Verify UI elements exist
    QObject *clockText = root->findChild<QObject*>("clockText");
    QVERIFY(clockText != nullptr);
}
```

---

### Strategy 3: Integration Tests

**Test Model + View + LayerShell together**

```cpp
class IntegrationTest : public QObject {
    Q_OBJECT
    
private slots:
    void initTestCase() {
        if (!LayerShellQt::Shell::isAvailable()) {
            QSKIP("Layer shell not available");
        }
    }
    
    void testPanelCreation() {
        PanelModel model;
        
        QQuickView view;
        view.rootContext()->setContextProperty("panelModel", &model);
        view.setSource(QUrl("qrc:/Panel.qml"));
        
        auto layerWindow = LayerShellQt::Window::get(&view);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        view.show();
        QVERIFY(QTest::qWaitForWindowExposed(&view));
        
        // Verify layer configuration
        QCOMPARE(layerWindow->layer(), LayerShellQt::Window::LayerTop);
    }
};
```

---

### Strategy 4: Visual Regression Testing

**Capture screenshots for comparison**

```cpp
void TestVisual::testPanelAppearance() {
    QQuickView view;
    // ... setup
    
    view.show();
    QTest::qWaitForWindowExposed(&view);
    
    // Capture screenshot
    QImage screenshot = view.grabWindow();
    
    // Compare with baseline
    QImage baseline(":/test_baseline.png");
    QCOMPARE(screenshot, baseline);
}
```

---

## Common Pitfalls

### Pitfall 1: Forgetting NOTIFY Signal

```cpp
// ✗ BAD: No NOTIFY
Q_PROPERTY(QString value READ value)

// QML won't auto-update when value changes!
```

```cpp
// ✓ GOOD: With NOTIFY
Q_PROPERTY(QString value READ value NOTIFY valueChanged)

signals:
    void valueChanged();
```

**Always include NOTIFY for mutable properties!**

---

### Pitfall 2: Heavy Logic in Property Getters

```cpp
// ✗ BAD: Expensive computation in getter
QString formattedTime() const {
    // Complex formatting every time QML accesses!
    return complexFormatting(QTime::currentTime());
}

// ✓ GOOD: Cache the value
void updateTime() {
    m_formattedTime = complexFormatting(QTime::currentTime());
    emit timeChanged();
}

QString formattedTime() const {
    return m_formattedTime;  // Fast!
}
```

---

### Pitfall 3: Not Checking QML Load Errors

```cpp
// ✗ BAD
view.setSource(QUrl("qrc:/Panel.qml"));
view.show();  // Blank if typo in filename!

// ✓ GOOD
view.setSource(QUrl("qrc:/Panel.qml"));
if (view.status() == QQuickView::Error) {
    qCritical() << view.errors();
    return 1;
}
```

---

### Pitfall 4: Circular Dependencies in QML

```qml
// ✗ BAD: Circular binding
Rectangle {
    width: height
    height: width  // Binding loop!
}

// ✓ GOOD: One-way binding
Rectangle {
    width: height
    height: 100  // Fixed value
}
```

---

### Pitfall 5: Memory Leaks with Context Properties

```cpp
// ✗ BAD: Local variable
void createView() {
    SimpleModel model;  // Destroyed when function returns!
    view->rootContext()->setContextProperty("model", &model);
}

// ✓ GOOD: Proper lifetime
class Manager {
    SimpleModel m_model;  // Lives as long as Manager
    
    void createView() {
        view->rootContext()->setContextProperty("model", &m_model);
    }
};
```

---

### Pitfall 6: Configuring LayerShell After Show

```cpp
// ✗ BAD: Too late!
view.show();
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Ignored!

// ✓ GOOD: Before show
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);
view.show();
```

---

### Pitfall 7: Not Handling Screen Changes

```cpp
// ✗ BAD: Static setup
void createPanels() {
    for (QScreen *screen : QGuiApplication::screens()) {
        createPanel(screen);
    }
    // If user plugs in monitor, no panel created!
}

// ✓ GOOD: Dynamic handling
void setupManager() {
    for (QScreen *screen : QGuiApplication::screens()) {
        createPanel(screen);
    }
    
    connect(qApp, &QGuiApplication::screenAdded,
            this, &Manager::createPanel);
    connect(qApp, &QGuiApplication::screenRemoved,
            this, &Manager::removePanel);
}
```

---

## Checklist: Before Production

### Model Layer
- [ ] All mutable properties have NOTIFY signals
- [ ] Q_INVOKABLE methods validate input
- [ ] No heavy computation in property getters
- [ ] Signals emitted only when values actually change
- [ ] Thread-safe if used across threads
- [ ] Unit tests written for all logic

### View Layer (QML)
- [ ] No complex JavaScript logic
- [ ] Property bindings used instead of imperative code
- [ ] Loaders used for heavy/optional components
- [ ] Animations for all state transitions
- [ ] IDs used sparingly
- [ ] Responsive to different screen sizes

### Integration Layer
- [ ] LayerShellQt availability checked
- [ ] All properties configured before show()
- [ ] Appropriate layer chosen for use case
- [ ] Exclusive zone set correctly
- [ ] QML loading errors handled
- [ ] Context properties have correct lifetime

### Performance
- [ ] No unnecessary property bindings
- [ ] ListView used for large lists (not Repeater)
- [ ] Lazy loading for heavy components
- [ ] Batch updates in models
- [ ] Cached computed properties

### Error Handling
- [ ] Layer shell availability checked
- [ ] QML load errors handled
- [ ] Model input validated
- [ ] Signal connections verified
- [ ] Graceful degradation on unsupported compositors

### Testing
- [ ] Unit tests for all models
- [ ] Integration tests for critical flows
- [ ] Tested on multiple compositors
- [ ] Tested on multiple screen sizes/DPIs
- [ ] Memory leaks checked
- [ ] Performance profiled

---

## Summary: Golden Rules for QML + Model-View

### 1. **Separate Concerns**
- Model (C++): Business logic, data
- View (QML): UI presentation
- Main (C++): Integration, LayerShell config

### 2. **Use Q_PROPERTY + NOTIFY**
- All data exposed via Q_PROPERTY
- All mutable properties have NOTIFY
- Automatic QML binding

### 3. **Keep QML Simple**
- No complex JavaScript
- Declarative bindings
- Logic in C++ Model

### 4. **Configure Before Show**
- Set LayerShell properties first
- Then show window
- Check availability

### 5. **Optimize Performance**
- Cache computed values
- Lazy loading
- Batch updates
- Appropriate containers

### 6. **Handle Errors**
- Check availability
- Validate input
- Handle QML errors
- Graceful fallbacks

### 7. **Test Thoroughly**
- Unit test models
- Integration test flows
- Test on different compositors
- Visual regression testing

### 8. **Follow UX Guidelines**
- Smooth animations
- Responsive design
- Appropriate exclusive zones
- Keyboard handling

---

## Next Steps

You've learned best practices! Now:
1. **Apply patterns** to your code
2. **Refactor** existing code to Model-View
3. **Test** thoroughly
4. **Profile** performance
5. **Deploy** with confidence!

Check:
- **[03-examples.md](./03-examples.md)** - See patterns in action
- **[06-quick-reference.md](./06-quick-reference.md)** - Quick lookup

---

**Happy coding with clean, maintainable QML architecture! 🎨**

*All best practices follow modern Qt Model-View pattern with QML*
