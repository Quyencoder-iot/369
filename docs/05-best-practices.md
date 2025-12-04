# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model-View Patterns](#model-view-patterns)
3. [QML Best Practices](#qml-best-practices)
4. [UI/UX Guidelines](#uiux-guidelines)
5. [Performance Best Practices](#performance-best-practices)
6. [Error Handling](#error-handling)
7. [Testing Strategies](#testing-strategies)
8. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Separation of Concerns

**Model-View architecture is mandatory for maintainable code.**

```cpp
// ✓ GOOD: Clear separation
// Model (C++) - Business logic only
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
private:
    QString m_time;
    void updateTime();  // Business logic
};
```

```qml
// View (QML) - UI only
Rectangle {
    Text { text: panelModel.time }  // Just display data
}
```

```cpp
// ✗ BAD: Mixed concerns
QWidget *panel = new QWidget();
QLabel *timeLabel = new QLabel(panel);
timeLabel->setText(QTime::currentTime().toString());  // Logic in UI code
```

**Why:**
- Testable: Test model independently
- Maintainable: Change UI without touching logic
- Reusable: Same model, different views

---

### Principle 2: Data Binding Over Manual Updates

**Use Q_PROPERTY and signals for automatic updates.**

```cpp
// ✓ GOOD: Property with signal
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(int value READ value NOTIFY valueChanged)
    
public:
    int value() const { return m_value; }
    
    void setValue(int v) {
        if (m_value != v) {
            m_value = v;
            emit valueChanged();  // QML auto-updates!
        }
    }
    
signals:
    void valueChanged();
    
private:
    int m_value = 0;
};
```

```qml
// QML binds automatically
Text {
    text: model.value  // Auto-updates when valueChanged emitted
}
```

```cpp
// ✗ BAD: Manual updates
void updateLabel() {
    label->setText(QString::number(value));  // Must call manually
}
```

---

### Principle 3: Configure Before Show

**Set all LayerShellQt properties BEFORE showing the window.**

```cpp
// ✓ GOOD: Configure then show
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
layerWindow->setExclusiveZone(35);

view.show();  // Now it works!
```

```cpp
// ✗ BAD: Show then configure
view.show();
layerWindow->setLayer(LayerTop);  // Too late!
```

---

### Principle 4: Use Appropriate Layer

**Choose layer based on use case, not convenience.**

| Use Case | Layer | Reasoning |
|----------|-------|-----------|
| Panel/Dock | TOP | Above apps, needs exclusive zone |
| Notification | TOP | Above apps, no exclusive zone |
| Widget | BOTTOM | Below apps, decorative |
| Wallpaper | BACKGROUND | Lowest layer |
| Lock screen | OVERLAY | Must block everything |

```cpp
// ✓ GOOD: Appropriate layer
layerWindow->setLayer(LayerShellQt::Window::LayerTop);  // Panel

// ✗ BAD: Wrong layer
layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);  // Overkill for panel!
```

---

## Model-View Patterns

### Pattern 1: Simple Property Model

**For widgets with simple data (clock, status, etc.)**

```cpp
// SimpleModel.h
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    
public:
    explicit SimpleModel(QObject *parent = nullptr);
    
    QString text() const { return m_text; }
    bool active() const { return m_active; }
    
signals:
    void textChanged();
    void activeChanged();
    
private:
    QString m_text;
    bool m_active;
};
```

```qml
// View.qml
Rectangle {
    color: model.active ? "#4fc3f7" : "#888"
    
    Text {
        text: model.text
        color: "white"
    }
}
```

**When to use:**
- Single window/widget
- Simple data
- Few properties

---

### Pattern 2: List Model

**For lists of items (dock apps, tray icons, notifications)**

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
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_apps.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid()) return QVariant();
        
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
    
    Q_INVOKABLE void launchApp(int index);
    
private:
    QList<App> m_apps;
};
```

```qml
// Dock.qml
Row {
    spacing: 10
    
    Repeater {
        model: appListModel
        
        delegate: Button {
            text: model.icon
            ToolTip.text: model.name
            onClicked: appListModel.launchApp(index)
        }
    }
}
```

**When to use:**
- Multiple items
- Dynamic lists
- Dock, tray, menu

---

### Pattern 3: View Manager

**For multi-monitor or multiple windows**

```cpp
// ViewManager.h
class ViewManager : public QObject {
    Q_OBJECT
    
public:
    ViewManager(QAbstractItemModel *model, QObject *parent = nullptr);
    
private slots:
    void createView(const QString &identifier);
    void removeView(const QString &identifier);
    
private:
    QAbstractItemModel *m_model;
    QHash<QString, QQuickView*> m_views;
    
    QQuickView* createViewForData(const QString &id);
};
```

```cpp
// ViewManager.cpp
QQuickView* ViewManager::createViewForData(const QString &id) {
    QQuickView *view = new QQuickView();
    
    // Share model with view
    view->rootContext()->setContextProperty("dataModel", m_model);
    view->rootContext()->setContextProperty("viewId", id);
    
    view->setSource(QUrl("qrc:/View.qml"));
    
    // Configure LayerShell
    auto layerWindow = LayerShellQt::Window::get(view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setScope(id);  // Per-screen
    
    view->show();
    return view;
}
```

**When to use:**
- Multi-monitor panels
- Multiple notification windows
- Dynamic window creation

---

### Pattern 4: Action Model

**For widgets with user actions**

```cpp
// ActionModel.h
class ActionModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    
public:
    explicit ActionModel(QObject *parent = nullptr);
    
    QString status() const { return m_status; }
    
    Q_INVOKABLE void executeAction(const QString &action);
    Q_INVOKABLE void openSettings();
    Q_INVOKABLE void toggleState();
    
signals:
    void statusChanged();
    void actionCompleted(const QString &action);
    
private:
    QString m_status;
};
```

```qml
// ActionView.qml
Column {
    Text {
        text: actionModel.status
    }
    
    Button {
        text: "Execute"
        onClicked: actionModel.executeAction("doSomething")
    }
    
    Button {
        text: "Settings"
        onClicked: actionModel.openSettings()
    }
    
    Connections {
        target: actionModel
        function onActionCompleted(action) {
            console.log("Completed:", action)
        }
    }
}
```

**When to use:**
- Interactive widgets
- Settings panels
- Control interfaces

---

### Pattern 5: Singleton Model

**For shared data across multiple views**

```cpp
// GlobalModel.h
class GlobalModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int screenCount READ screenCount NOTIFY screenCountChanged)
    Q_PROPERTY(QString theme READ theme NOTIFY themeChanged)
    
public:
    static GlobalModel* instance() {
        static GlobalModel instance;
        return &instance;
    }
    
    int screenCount() const { return m_screenCount; }
    QString theme() const { return m_theme; }
    
signals:
    void screenCountChanged();
    void themeChanged();
    
private:
    GlobalModel() = default;
    int m_screenCount = 0;
    QString m_theme;
};
```

```cpp
// Register as singleton
qmlRegisterSingletonType<GlobalModel>(
    "MyApp", 1, 0, "GlobalModel",
    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return GlobalModel::instance();
    }
);
```

```qml
// Use in any QML file
import MyApp 1.0

Text {
    text: GlobalModel.screenCount + " screens"
}
```

**When to use:**
- Shared settings
- Global state
- Theme management

---

## QML Best Practices

### Practice 1: Component Organization

**Structure QML files logically.**

```
project/
├── models/
│   ├── PanelModel.h
│   └── DockModel.h
├── qml/
│   ├── Panel.qml
│   ├── Dock.qml
│   ├── components/
│   │   ├── ClockWidget.qml
│   │   ├── TrayIcon.qml
│   │   └── AppButton.qml
│   └── styles/
│       └── Theme.qml
└── resources.qrc
```

```qml
// ✓ GOOD: Reusable component
// components/ClockWidget.qml
import QtQuick 2.15

Text {
    property var model: null
    text: model ? model.time : "--:--"
    color: "white"
    font.pixelSize: 14
}
```

```qml
// Panel.qml
import QtQuick 2.15
import "components"

Rectangle {
    ClockWidget {
        model: panelModel
        anchors.right: parent.right
    }
}
```

---

### Practice 2: Use Components, Not Copy-Paste

```qml
// ✗ BAD: Duplicated code
Rectangle {
    Button { text: "A"; /* same styling */ }
    Button { text: "B"; /* same styling */ }
    Button { text: "C"; /* same styling */ }
}
```

```qml
// ✓ GOOD: Component
// AppButton.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root
    
    contentItem: Text {
        text: root.text
        color: "white"
        horizontalAlignment: Text.AlignHCenter
    }
    
    background: Rectangle {
        color: root.hovered ? "#4d4d4d" : "#3d3d3d"
        radius: 5
    }
}
```

```qml
// Use component
import "components"

Row {
    AppButton { text: "A" }
    AppButton { text: "B" }
    AppButton { text: "C" }
}
```

---

### Practice 3: Property Binding

```qml
// ✓ GOOD: Property binding
Rectangle {
    width: parent.width * 0.8  // Auto-updates
    height: model.expanded ? 200 : 35  // Reactive
    color: model.active ? "#4fc3f7" : "#888"  // Conditional
}
```

```qml
// ✗ BAD: Manual assignment
Rectangle {
    width: 300  // Fixed, not responsive
    
    Component.onCompleted: {
        if (model.active) {
            color = "#4fc3f7"
        }
    }
}
```

---

### Practice 4: Animations

```qml
// ✓ GOOD: Smooth animations
Rectangle {
    width: 200
    height: model.expanded ? 200 : 35
    
    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
}
```

```qml
// ✗ BAD: Instant changes
Rectangle {
    height: model.expanded ? 200 : 35  // Jumps instantly
}
```

---

### Practice 5: Lazy Loading

```qml
// ✓ GOOD: Load on demand
Loader {
    id: settingsLoader
    source: settingsVisible ? "Settings.qml" : ""
}

Button {
    text: "Settings"
    onClicked: settingsVisible = true
}
```

```qml
// ✗ BAD: Always loaded
Settings {
    visible: settingsVisible  // Loaded even when hidden
}
```

---

### Practice 6: State Management

```qml
// ✓ GOOD: Use states
Rectangle {
    id: panel
    
    states: [
        State {
            name: "collapsed"
            PropertyChanges { target: panel; height: 35 }
        },
        State {
            name: "expanded"
            PropertyChanges { target: panel; height: 200 }
        }
    ]
    
    transitions: Transition {
        NumberAnimation {
            properties: "height"
            duration: 300
        }
    }
    
    state: model.expanded ? "expanded" : "collapsed"
}
```

---

## UI/UX Guidelines

### Guideline 1: Consistent Sizing

**Define sizes as constants or in theme.**

```qml
// Theme.qml
pragma Singleton
import QtQuick 2.15

QtObject {
    // Spacing
    readonly property int spacingSmall: 5
    readonly property int spacingMedium: 10
    readonly property int spacingLarge: 20
    
    // Sizes
    readonly property int panelHeight: 35
    readonly property int dockIconSize: 48
    readonly property int cornerRadius: 8
    
    // Colors
    readonly property color background: "#2d2d2d"
    readonly property color foreground: "#ffffff"
    readonly property color accent: "#4fc3f7"
}
```

```qml
// Use in components
import "styles"

Rectangle {
    height: Theme.panelHeight
    color: Theme.background
    radius: Theme.cornerRadius
}
```

---

### Guideline 2: Responsive Design

```qml
// ✓ GOOD: Responsive to screen size
Rectangle {
    width: parent.width
    height: Screen.height > 1080 ? 40 : 30  // Adapt to resolution
    
    Text {
        font.pixelSize: parent.height * 0.6  // Proportional
    }
}
```

---

### Guideline 3: Accessibility

```qml
// ✓ GOOD: Accessible
Button {
    text: "Settings"
    Accessible.name: "Settings Button"
    Accessible.description: "Open settings panel"
    Accessible.role: Accessible.Button
}
```

---

### Guideline 4: High DPI Support

```qml
// ✓ GOOD: Use relative units
Text {
    font.pixelSize: 12  // Will scale with DPI
}

Image {
    source: "icon.png"
    sourceSize: Qt.size(48, 48)  // Logical size
}
```

---

### Guideline 5: Visual Feedback

```qml
// ✓ GOOD: Hover effects
Button {
    id: btn
    
    background: Rectangle {
        color: btn.hovered ? "#4d4d4d" : "#3d3d3d"
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
```

---

## Performance Best Practices

### Practice 1: Minimize Model Updates

```cpp
// ✓ GOOD: Only emit when changed
void setValue(int v) {
    if (m_value == v) return;  // Check first!
    
    m_value = v;
    emit valueChanged();
}
```

```cpp
// ✗ BAD: Always emit
void setValue(int v) {
    m_value = v;
    emit valueChanged();  // Even if unchanged!
}
```

---

### Practice 2: Use Item Pooling for Lists

```qml
// ✓ GOOD: Reuse delegates
ListView {
    model: largeListModel
    cacheBuffer: 100  // Cache off-screen items
    
    delegate: ItemDelegate {
        // Will be reused
    }
}
```

---

### Practice 3: Optimize Bindings

```qml
// ✗ BAD: Complex binding
Text {
    text: {
        var result = "";
        for (var i = 0; i < model.count; i++) {
            result += model.get(i).name + " ";
        }
        return result;
    }
}
```

```qml
// ✓ GOOD: Simple binding, compute in model
Text {
    text: model.concatenatedNames  // Computed in C++
}
```

---

### Practice 4: Async Operations

```cpp
// ✓ GOOD: Async operations
class Model : public QObject {
    Q_INVOKABLE void loadData() {
        // Start async operation
        QtConcurrent::run([this]() {
            auto data = fetchDataFromNetwork();
            
            // Update on main thread
            QMetaObject::invokeMethod(this, [this, data]() {
                m_data = data;
                emit dataChanged();
            });
        });
    }
};
```

---

### Practice 5: Limit Timer Frequency

```qml
// ✓ GOOD: Reasonable update rate
Timer {
    interval: 1000  // 1 second for clock
    running: true
    repeat: true
    onTriggered: model.updateTime()
}
```

```qml
// ✗ BAD: Too frequent
Timer {
    interval: 1  // 1ms! CPU intensive!
    running: true
    repeat: true
}
```

---

## Error Handling

### Pattern 1: Graceful Fallback

```cpp
// ✓ GOOD: Check availability
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qWarning() << "Layer shell not available";
        
        // Fallback: Normal window
        QQuickView fallbackView;
        fallbackView.setSource(QUrl("qrc:/Panel.qml"));
        fallbackView.setFlags(Qt::Tool | Qt::FramelessWindowHint);
        fallbackView.show();
        
        return app.exec();
    }
    
    // Normal layer shell path
    // ...
}
```

---

### Pattern 2: Model Validation

```cpp
// ✓ GOOD: Validate data
class Model : public QObject {
    Q_INVOKABLE bool setData(const QString &data) {
        if (data.isEmpty()) {
            emit error("Data cannot be empty");
            return false;
        }
        
        m_data = data;
        emit dataChanged();
        return true;
    }
    
signals:
    void error(const QString &message);
};
```

```qml
// Handle errors in QML
Connections {
    target: model
    function onError(message) {
        errorDialog.text = message
        errorDialog.open()
    }
}
```

---

### Pattern 3: Try-Catch for QML

```qml
// ✓ GOOD: Handle errors
Button {
    onClicked: {
        try {
            model.riskyOperation()
        } catch (error) {
            console.error("Error:", error)
            errorText.text = "Operation failed"
        }
    }
}
```

---

## Testing Strategies

### Strategy 1: Unit Test Models

```cpp
// test_model.cpp
#include <QtTest>
#include "PanelModel.h"

class TestPanelModel : public QObject {
    Q_OBJECT
    
private slots:
    void testTimeUpdate() {
        PanelModel model;
        
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        // Trigger update
        model.updateTime();
        
        QCOMPARE(spy.count(), 1);
        QVERIFY(!model.time().isEmpty());
    }
    
    void testPropertyBinding() {
        PanelModel model;
        
        QString time1 = model.time();
        model.updateTime();
        QString time2 = model.time();
        
        // Time should change
        QVERIFY(time1 != time2 || time1.isEmpty());
    }
};

QTEST_MAIN(TestPanelModel)
#include "test_model.moc"
```

---

### Strategy 2: QML Test

```qml
// tst_panel.qml
import QtQuick 2.15
import QtTest 1.15

TestCase {
    name: "PanelTests"
    
    Component {
        id: panelComponent
        Panel {
            id: panel
        }
    }
    
    function test_initialState() {
        var panel = createTemporaryObject(panelComponent, this)
        verify(panel !== null)
        compare(panel.height, 35)
    }
    
    function test_expansion() {
        var panel = createTemporaryObject(panelComponent, this)
        
        panel.expanded = true
        wait(350)  // Wait for animation
        
        compare(panel.height, 200)
    }
}
```

---

### Strategy 3: Integration Test

```cpp
// test_integration.cpp
void TestIntegration::testViewCreation() {
    PanelModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    QVERIFY(view.rootObject() != nullptr);
    QCOMPARE(view.status(), QQuickView::Ready);
}
```

---

### Strategy 4: Mock Model for UI Testing

```cpp
// MockModel.h
class MockPanelModel : public PanelModel {
    Q_OBJECT
public:
    void setMockTime(const QString &time) {
        m_mockTime = time;
        emit timeChanged();
    }
    
    QString time() const override {
        return m_mockTime.isEmpty() ? PanelModel::time() : m_mockTime;
    }
    
private:
    QString m_mockTime;
};
```

```cpp
// Test with mock
MockPanelModel mockModel;
mockModel.setMockTime("12:34:56");

view.rootContext()->setContextProperty("panelModel", &mockModel);
// Test UI with predictable data
```

---

## Common Pitfalls

### Pitfall 1: Forgetting Q_OBJECT

```cpp
// ✗ BAD: No Q_OBJECT
class Model : public QObject {
    Q_PROPERTY(int value READ value)  // Won't work!
};
```

```cpp
// ✓ GOOD: With Q_OBJECT
class Model : public QObject {
    Q_OBJECT  // Required!
    Q_PROPERTY(int value READ value NOTIFY valueChanged)
};
```

---

### Pitfall 2: Missing NOTIFY Signal

```cpp
// ✗ BAD: No NOTIFY
Q_PROPERTY(int value READ value)
```

```cpp
// ✓ GOOD: With NOTIFY
Q_PROPERTY(int value READ value NOTIFY valueChanged)

signals:
    void valueChanged();
```

**Without NOTIFY, QML bindings won't update!**

---

### Pitfall 3: Circular Dependencies in QML

```qml
// ✗ BAD: Circular binding
Rectangle {
    width: height * 2
    height: width / 2  // Error: circular!
}
```

```qml
// ✓ GOOD: One-way binding
Rectangle {
    width: 200
    height: width / 2
}
```

---

### Pitfall 4: Not Using Loader

```qml
// ✗ BAD: Always loaded
HeavyComponent {
    visible: showSettings  // Still loaded when hidden!
}
```

```qml
// ✓ GOOD: Lazy load
Loader {
    active: showSettings
    sourceComponent: HeavyComponent {}
}
```

---

### Pitfall 5: Exposing C++ Pointers to QML

```cpp
// ✗ BAD: Raw pointer
Q_INVOKABLE QObject* getChild() {
    return new QObject(this);  // QML doesn't own it
}
```

```cpp
// ✓ GOOD: Use Q_INVOKABLE with proper ownership
Q_INVOKABLE QObject* getChild() {
    auto child = new QObject();
    QQmlEngine::setObjectOwnership(child, QQmlEngine::JavaScriptOwnership);
    return child;
}
```

---

### Pitfall 6: LayerShell After Show

```cpp
// ✗ BAD: Configure after show
view.show();
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Too late!
```

```cpp
// ✓ GOOD: Configure before show
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);
view.show();
```

---

### Pitfall 7: Not Handling Screen Changes

```cpp
// ✗ BAD: Static, doesn't react
ViewManager manager(model);
// No handling of screen add/remove
```

```cpp
// ✓ GOOD: React to changes
connect(qApp, &QGuiApplication::screenAdded,
        &manager, &ViewManager::onScreenAdded);
connect(qApp, &QGuiApplication::screenRemoved,
        &manager, &ViewManager::onScreenRemoved);
```

---

## Checklist: Production-Ready Code

### Model Checklist
- [ ] All properties have NOTIFY signals
- [ ] Q_OBJECT macro present
- [ ] Validation in setters
- [ ] Error signals defined
- [ ] Async operations on separate threads
- [ ] Thread-safe if needed
- [ ] Unit tests written

### QML Checklist
- [ ] No hardcoded values (use Theme)
- [ ] Components reused, not duplicated
- [ ] Animations smooth (300ms default)
- [ ] Lazy loading used where appropriate
- [ ] No circular bindings
- [ ] Proper anchoring/layouts
- [ ] Accessibility properties set

### LayerShell Checklist
- [ ] Check `isAvailable()` at startup
- [ ] Configure before `show()`
- [ ] Appropriate layer chosen
- [ ] Exclusive zone matches use case
- [ ] Keyboard interactivity correct
- [ ] Multi-monitor handled
- [ ] Margins set appropriately
- [ ] Scope configured if needed

### Integration Checklist
- [ ] Model exposed to QML correctly
- [ ] Context properties named clearly
- [ ] Resource file (qrc) configured
- [ ] CMake includes Qt Quick
- [ ] Build tested on target platform
- [ ] Error handling implemented
- [ ] Logging for debugging

---

## Code Review Checklist

When reviewing code, check:

### Architecture
- ✓ Clear Model-View separation?
- ✓ No business logic in QML?
- ✓ No UI code in models?

### Properties
- ✓ All Q_PROPERTYs have NOTIFY?
- ✓ Signals emitted on changes?
- ✓ Read-only properties marked CONSTANT?

### QML
- ✓ Components reused?
- ✓ Theme/constants used?
- ✓ Animations smooth?
- ✓ No performance issues?

### LayerShell
- ✓ Configured before show?
- ✓ Appropriate layer?
- ✓ Exclusive zone correct?

### Testing
- ✓ Unit tests for models?
- ✓ QML tests for views?
- ✓ Integration tests?

---

## Summary: Golden Rules

### 1. **Separate Model from View**
Model (C++) handles logic, View (QML) handles UI.

### 2. **Use Data Binding**
Q_PROPERTY + signals → automatic QML updates.

### 3. **Configure Before Show**
Set LayerShell properties before `show()`.

### 4. **Test Models Independently**
Unit test C++ models without UI.

### 5. **Reuse QML Components**
Don't duplicate code, create components.

### 6. **Handle Errors Gracefully**
Check availability, validate data, provide fallbacks.

### 7. **Optimize Performance**
Lazy loading, reasonable timers, simple bindings.

### 8. **Follow Qt Best Practices**
Use Qt's patterns, don't fight the framework.

---

## Resources

### Qt Documentation
- **QML Best Practices:** https://doc.qt.io/qt-5/qtquick-bestpractices.html
- **Model/View Programming:** https://doc.qt.io/qt-5/model-view-programming.html
- **Performance Tips:** https://doc.qt.io/qt-5/qtquick-performance.html

### This Guide
- **Examples:** `docs/03-examples.md` - See patterns in action
- **Migration:** `docs/EXAMPLES-MIGRATION-GUIDE.md` - QWidget → QML
- **Architecture:** `examples/MODEL-VIEW-EXPLANATION.md` - Deep dive

---

**Follow these practices for maintainable, performant LayerShellQt applications! 🎨**
