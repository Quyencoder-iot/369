# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Coding Patterns](#coding-patterns)
3. [UI/UX Guidelines](#uiux-guidelines)
4. [Performance Best Practices](#performance-best-practices)
5. [Error Handling](#error-handling)
6. [Testing Strategies](#testing-strategies)
7. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Strict Model-View Separation

**Model (C++)**: Business logic, data, system integration
**View (QML)**: UI presentation, styling, user interactions

```cpp
// ✓ GOOD: Model only handles data
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
private:
    QString m_time;  // Data only
    void updateTime() { /* Logic only */ }
};
```

```qml
// ✓ GOOD: View only displays
Rectangle {
    Text { text: panelModel.time }  // Display only
}
```

```cpp
// ✗ BAD: Don't put UI in model
class BadModel : public QObject {
    QQuickView *m_view;  // NO! Model shouldn't know about views
};
```

```qml
// ✗ BAD: Don't put logic in QML
Rectangle {
    Text {
        text: {
            // Complex business logic here - NO!
            var result = complexCalculation();
            return result;
        }
    }
}
```

---

### Principle 2: Use Properties for Data Binding

**Always use Q_PROPERTY with NOTIFY signal:**

```cpp
// ✓ GOOD: Property with notify
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value READ value NOTIFY valueChanged)
    
signals:
    void valueChanged();
};
```

```qml
// Auto-updates when valueChanged emitted
Text { text: model.value }
```

```cpp
// ✗ BAD: No notify signal
class BadModel : public QObject {
    Q_PROPERTY(QString value READ value)  // Missing NOTIFY
};
```

---

### Principle 3: Configure LayerShell in C++, Not QML

```cpp
// ✓ GOOD: LayerShell config in C++
QQuickView view;
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(/* ... */);
view.show();
```

```qml
// ✗ BAD: Can't configure LayerShell from QML
// No LayerShellQt QML bindings exist
```

**Rule:** Always configure LayerShellQt before showing view.

---

### Principle 4: Use Appropriate Layer

| Use Case | Layer | Reasoning |
|----------|-------|-----------|
| Panel | TOP | Above apps, exclusive zone |
| Dock | TOP | Above apps, exclusive zone |
| Notification | TOP | Above apps, no exclusive zone |
| Widget | BOTTOM | Below apps, decorative |
| Wallpaper | BACKGROUND | Lowest layer |
| Lock screen | OVERLAY | Block everything |

```cpp
// ✓ GOOD: Appropriate layer for use case
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);  // Panel
```

```cpp
// ✗ BAD: Wrong layer
layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);  // Too aggressive for panel!
```

---

## Coding Patterns

### Pattern 1: Base Model Class

**Create reusable base for all models:**

```cpp
// BaseLayerModel.h
class BaseLayerModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool available READ available CONSTANT)
    
public:
    explicit BaseLayerModel(QObject *parent = nullptr) 
        : QObject(parent) {
        m_available = LayerShellQt::Shell::isAvailable();
    }
    
    bool available() const { return m_available; }
    
protected:
    bool m_available;
};
```

**Usage:**
```cpp
class PanelModel : public BaseLayerModel {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
    // Panel-specific properties
};
```

```qml
// Check availability in QML
Rectangle {
    visible: model.available
    
    Text {
        text: model.available ? model.time : "Layer shell not available"
    }
}
```

---

### Pattern 2: View Manager Pattern

**Centralized view management:**

```cpp
// ViewManager.h
class ViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit ViewManager(QObject *parent = nullptr) 
        : QObject(parent) {}
    
    QQuickView* createView(const QString &qmlFile, QObject *model) {
        QQuickView *view = new QQuickView();
        
        // Expose model to QML
        view->rootContext()->setContextProperty("model", model);
        
        // Load QML
        view->setSource(QUrl(qmlFile));
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        return view;
    }
    
    void configureLayerShell(QQuickView *view, 
                            LayerShellQt::Window::Layer layer,
                            LayerShellQt::Window::Anchors anchors,
                            int exclusiveZone) {
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(layer);
        layerWindow->setAnchors(anchors);
        layerWindow->setExclusiveZone(exclusiveZone);
    }
};
```

**Usage:**
```cpp
// main.cpp
ViewManager manager;

PanelModel panelModel;
QQuickView *panel = manager.createView("qrc:/Panel.qml", &panelModel);
manager.configureLayerShell(
    panel,
    LayerShellQt::Window::LayerTop,
    LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight,
    35
);
panel->show();
```

---

### Pattern 3: Property Builder Pattern

**Fluent interface for model properties:**

```cpp
// PropertyBuilder.h
template<typename ModelType>
class PropertyBuilder {
public:
    PropertyBuilder(ModelType *model) : m_model(model) {}
    
    PropertyBuilder& property(const char *name, const QVariant &value) {
        m_model->setProperty(name, value);
        return *this;
    }
    
    ModelType* build() { return m_model; }
    
private:
    ModelType *m_model;
};
```

**Usage:**
```cpp
auto model = PropertyBuilder<PanelModel>(new PanelModel())
    .property("title", "My Panel")
    .property("height", 35)
    .build();
```

---

### Pattern 4: Signals as Events Pattern

**Use signals for important events:**

```cpp
// EventModel.h
class PanelModel : public QObject {
    Q_OBJECT
    
signals:
    // Events
    void panelOpened();
    void panelClosed();
    void actionTriggered(const QString &action);
    
    // State changes
    void expandedChanged();
    void themeChanged();
};
```

```qml
// Connect in QML
Connections {
    target: model
    
    function onPanelOpened() {
        console.log("Panel opened!")
        openAnimation.start()
    }
    
    function onActionTriggered(action) {
        console.log("Action:", action)
        handleAction(action)
    }
}
```

---

### Pattern 5: List Model Pattern

**For repeating elements:**

```cpp
// ListModel.h
class AppListModel : public QAbstractListModel {
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
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_apps.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid()) return QVariant();
        
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
    
    Q_INVOKABLE void addApp(const QString &icon, const QString &name, const QString &cmd) {
        beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());
        m_apps.append({icon, name, cmd});
        endInsertRows();
    }
    
private:
    QList<App> m_apps;
};
```

```qml
// Use with Repeater
Repeater {
    model: appListModel
    
    delegate: Button {
        text: model.icon
        ToolTip.text: model.name
        onClicked: Qt.openUrlExternally(model.command)
    }
}
```

---

### Pattern 6: Invokable Actions Pattern

**Expose actions to QML:**

```cpp
class ActionModel : public QObject {
    Q_OBJECT
    
public:
    // Simple action
    Q_INVOKABLE void doAction() {
        // Perform action
        emit actionCompleted();
    }
    
    // Action with parameters
    Q_INVOKABLE void launchApp(const QString &command) {
        QProcess::startDetached(command, {});
    }
    
    // Action with return value
    Q_INVOKABLE QString formatTime(const QDateTime &time) {
        return time.toString("hh:mm:ss");
    }
    
signals:
    void actionCompleted();
};
```

```qml
Button {
    onClicked: model.doAction()
}

Button {
    onClicked: model.launchApp("firefox")
}

Text {
    text: model.formatTime(new Date())
}
```

---

## UI/UX Guidelines

### Guideline 1: Consistent Visual Hierarchy

**Use layers to establish visual order:**

```qml
// ✓ GOOD: Clear hierarchy
Rectangle {
    color: "#2d2d2d"  // Base
    
    Rectangle {
        color: "#3d3d3d"  // Elevated
        
        Text {
            color: "#4fc3f7"  // Accent
        }
    }
}
```

**Color scheme best practices:**
```qml
// Define theme in root
QtObject {
    id: theme
    
    readonly property color background: "#2d2d2d"
    readonly property color surface: "#3d3d3d"
    readonly property color accent: "#4fc3f7"
    readonly property color text: "#ffffff"
    readonly property color textSecondary: "#aaaaaa"
}

// Use throughout
Rectangle {
    color: theme.background
    
    Text {
        color: theme.text
    }
}
```

---

### Guideline 2: Respect User Space

```cpp
// ✓ GOOD: Reasonable panel size
layerWindow->setExclusiveZone(35);  // Small, unobtrusive
```

```cpp
// ✗ BAD: Excessive space reservation
layerWindow->setExclusiveZone(200);  // Too much!
```

```qml
// ✓ GOOD: Auto-hide notifications
Rectangle {
    id: notification
    
    Timer {
        interval: 5000
        running: true
        onTriggered: fadeOut.start()
    }
    
    NumberAnimation {
        id: fadeOut
        target: notification
        property: "opacity"
        to: 0
        duration: 300
        onFinished: notification.destroy()
    }
}
```

---

### Guideline 3: Smooth Animations

```qml
// ✓ GOOD: Smooth transitions
Rectangle {
    color: "#2d2d2d"
    
    Behavior on color {
        ColorAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
}

Rectangle {
    height: expanded ? 200 : 35
    
    Behavior on height {
        NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
    }
}
```

```qml
// ✗ BAD: Abrupt changes
Rectangle {
    color: hovered ? "#3d3d3d" : "#2d2d2d"  // Instant change
}
```

**Standard animation durations:**
- Fast: 150-200ms (hover effects)
- Normal: 250-350ms (expand/collapse)
- Slow: 400-600ms (complex transitions)

---

### Guideline 4: Accessible Design

```qml
// ✓ GOOD: Good contrast, readable sizes
Text {
    text: "Panel Title"
    color: "white"  // High contrast on dark background
    font.pixelSize: 14  // Readable size
}

Button {
    width: 40
    height: 40  // Touch-friendly size (minimum 32x32)
}
```

```qml
// ✗ BAD: Poor accessibility
Text {
    color: "#333"  // Low contrast on dark bg
    font.pixelSize: 8  // Too small
}
```

**Accessibility checklist:**
- ✓ Minimum font size: 11px
- ✓ Minimum touch target: 32x32px
- ✓ Contrast ratio: 4.5:1 for text
- ✓ Keyboard navigation support
- ✓ Screen reader friendly (use Accessible properties)

---

### Guideline 5: Responsive Layouts

```qml
// ✓ GOOD: Responsive to screen size
Rectangle {
    width: parent.width
    height: Screen.height < 1080 ? 30 : 40  // Adapt to screen
    
    Text {
        font.pixelSize: height * 0.4  // Relative sizing
    }
}
```

```qml
// ✓ GOOD: Handle different DPI
Text {
    font.pixelSize: 12 * Screen.devicePixelRatio
}
```

---

## Performance Best Practices

### Practice 1: Lazy Loading

```qml
// ✓ GOOD: Load components on-demand
Loader {
    id: menuLoader
    active: false
    source: "Menu.qml"
}

Button {
    onClicked: menuLoader.active = true
}
```

```qml
// ✗ BAD: Load everything upfront
Menu { }  // Always loaded, even if not used
Calendar { }
Settings { }
```

---

### Practice 2: Minimize Property Bindings

```qml
// ✓ GOOD: Simple binding
Text {
    text: model.time
}
```

```qml
// ✗ BAD: Complex binding (recalculates often)
Text {
    text: {
        var result = "";
        for (var i = 0; i < 100; i++) {
            result += model.getValue(i);
        }
        return result;
    }
}
```

**Solution: Move complex logic to C++ model:**
```cpp
class Model : public QObject {
    Q_PROPERTY(QString computedValue READ computedValue NOTIFY valueChanged)
    
    QString computedValue() const {
        // Complex calculation in C++
        return m_computedValue;
    }
};
```

---

### Practice 3: Use Appropriate List Components

```qml
// ✓ GOOD: ListView for large lists (virtualization)
ListView {
    model: largeModel  // 1000+ items
    delegate: ItemDelegate { }
}
```

```qml
// ✗ BAD: Repeater for large lists (creates all items)
Repeater {
    model: largeModel  // Creates 1000+ items at once!
    delegate: ItemDelegate { }
}
```

**Rule:** 
- Repeater: < 50 items
- ListView: 50+ items (automatic virtualization)

---

### Practice 4: Optimize Model Updates

```cpp
// ✓ GOOD: Batch updates
void updateMultipleValues() {
    m_value1 = newValue1;
    m_value2 = newValue2;
    m_value3 = newValue3;
    
    // Emit once at the end
    emit allValuesChanged();
}
```

```cpp
// ✗ BAD: Multiple signals
void updateMultipleValues() {
    m_value1 = newValue1;
    emit value1Changed();  // QML re-renders
    
    m_value2 = newValue2;
    emit value2Changed();  // QML re-renders again
    
    m_value3 = newValue3;
    emit value3Changed();  // QML re-renders yet again
}
```

---

### Practice 5: Cache Expensive Computations

```cpp
// ✓ GOOD: Cache computed values
class Model : public QObject {
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY timeChanged)
    
    QString formattedTime() const {
        return m_formattedTime;  // Return cached value
    }
    
private:
    void updateTime() {
        m_formattedTime = QTime::currentTime().toString("hh:mm:ss");
        emit timeChanged();
    }
    
    QString m_formattedTime;  // Cached
};
```

---

### Practice 6: Use Asynchronous Operations

```cpp
// ✓ GOOD: Don't block UI thread
class Model : public QObject {
    Q_INVOKABLE void loadData() {
        QtConcurrent::run([this]() {
            // Heavy operation in background thread
            auto data = fetchLargeData();
            
            // Update on main thread
            QMetaObject::invokeMethod(this, [this, data]() {
                m_data = data;
                emit dataChanged();
            });
        });
    }
};
```

```cpp
// ✗ BAD: Block UI thread
Q_INVOKABLE void loadData() {
    m_data = fetchLargeData();  // UI freezes!
    emit dataChanged();
}
```

---

## Error Handling

### Pattern 1: Graceful Degradation

```cpp
// Check layer shell availability
class Model : public QObject {
    Q_PROPERTY(bool layerShellAvailable READ layerShellAvailable CONSTANT)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    
public:
    bool layerShellAvailable() const {
        return LayerShellQt::Shell::isAvailable();
    }
    
    QString errorMessage() const {
        if (!layerShellAvailable()) {
            return "Layer shell not available. Using fallback.";
        }
        return QString();
    }
};
```

```qml
// Fallback UI in QML
Rectangle {
    Text {
        visible: !model.layerShellAvailable
        text: model.errorMessage
        color: "red"
    }
    
    // Normal UI
    Text {
        visible: model.layerShellAvailable
        text: model.time
    }
}
```

---

### Pattern 2: Error States in Model

```cpp
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    
public:
    void doOperation() {
        try {
            // Operation
            clearError();
        } catch (const std::exception &e) {
            setError(e.what());
        }
    }
    
signals:
    void errorChanged();
    
private:
    void setError(const QString &message) {
        m_hasError = true;
        m_errorMessage = message;
        emit errorChanged();
    }
    
    void clearError() {
        m_hasError = false;
        m_errorMessage.clear();
        emit errorChanged();
    }
    
    bool m_hasError = false;
    QString m_errorMessage;
};
```

```qml
// Display errors in UI
Rectangle {
    // Error banner
    Rectangle {
        visible: model.hasError
        color: "#ff5555"
        
        Text {
            text: model.errorMessage
            color: "white"
        }
    }
}
```

---

### Pattern 3: Validation in Model

```cpp
class LockScreenModel : public QObject {
    Q_INVOKABLE bool validatePassword(const QString &password) {
        if (password.isEmpty()) {
            setError("Password cannot be empty");
            return false;
        }
        
        if (password.length() < 6) {
            setError("Password too short");
            return false;
        }
        
        // Check password
        if (checkPassword(password)) {
            clearError();
            return true;
        }
        
        setError("Invalid password");
        return false;
    }
};
```

```qml
// Use in QML
TextField {
    id: passwordField
    
    onAccepted: {
        if (model.validatePassword(text)) {
            // Success
        }
    }
}

Text {
    text: model.errorMessage
    visible: model.hasError
    color: "red"
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
    void testTimeProperty() {
        PanelModel model;
        QString time = model.currentTime();
        
        QVERIFY(!time.isEmpty());
        QVERIFY(time.contains(":"));
    }
    
    void testTimeUpdates() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        // Wait for update
        QTest::qWait(1100);  // Timer interval + margin
        
        QVERIFY(spy.count() > 0);
    }
    
    void testInvokableAction() {
        ActionModel model;
        QSignalSpy spy(&model, &ActionModel::actionCompleted);
        
        model.doAction();
        
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(TestPanelModel)
#include "test_model.moc"
```

---

### Strategy 2: QML Test Cases

```qml
// tst_panel.qml
import QtQuick 2.15
import QtTest 1.15

TestCase {
    name: "PanelTests"
    
    Component {
        id: panelComponent
        
        Rectangle {
            property alias model: panelModel
            
            QtObject {
                id: panelModel
                property string time: "12:34:56"
            }
            
            Text {
                id: timeText
                text: panelModel.time
            }
        }
    }
    
    function test_timeDisplayed() {
        var panel = createTemporaryObject(panelComponent, testCase)
        verify(panel !== null)
        
        var timeText = panel.children[0]
        compare(timeText.text, "12:34:56")
    }
    
    function test_timeUpdates() {
        var panel = createTemporaryObject(panelComponent, testCase)
        
        panel.model.time = "14:30:00"
        compare(panel.children[0].text, "14:30:00")
    }
}
```

---

### Strategy 3: Integration Tests

```cpp
// test_integration.cpp
class TestIntegration : public QObject {
    Q_OBJECT
    
private slots:
    void testPanelCreation() {
        if (!LayerShellQt::Shell::isAvailable()) {
            QSKIP("Layer shell not available");
        }
        
        PanelModel model;
        
        QQuickView view;
        view.rootContext()->setContextProperty("model", &model);
        view.setSource(QUrl("qrc:/Panel.qml"));
        
        auto layerWindow = LayerShellQt::Window::get(&view);
        QVERIFY(layerWindow != nullptr);
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        QCOMPARE(layerWindow->layer(), LayerShellQt::Window::LayerTop);
        
        view.show();
        QVERIFY(QTest::qWaitForWindowExposed(&view));
    }
};
```

---

### Strategy 4: Mock Models for QML Testing

```cpp
// MockModel.h - For testing QML
class MockPanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time WRITE setTime NOTIFY timeChanged)
    
public:
    QString time() const { return m_time; }
    void setTime(const QString &time) {
        m_time = time;
        emit timeChanged();
    }
    
signals:
    void timeChanged();
    
private:
    QString m_time = "12:34:56";
};
```

```qml
// Test with mock
TestCase {
    function test_panel() {
        var mockModel = Qt.createQmlObject(
            'import QtQuick 2.15; QtObject { property string time: "10:00:00" }',
            testCase
        )
        
        // Test UI with mock data
    }
}
```

---

## Common Pitfalls

### Pitfall 1: Forgetting NOTIFY Signal

```cpp
// ✗ BAD: No notify signal
class BadModel : public QObject {
    Q_PROPERTY(QString value READ value)  // Missing NOTIFY
    
    QString value() const { return m_value; }
    
private:
    QString m_value;
};
```

```qml
// QML won't auto-update!
Text { text: model.value }  // Never updates
```

**Fix:**
```cpp
// ✓ GOOD: With notify
class GoodModel : public QObject {
    Q_PROPERTY(QString value READ value NOTIFY valueChanged)
    
signals:
    void valueChanged();
};
```

---

### Pitfall 2: Setting LayerShell After Show

```cpp
// ✗ BAD: Configure after show
QQuickView view;
view.show();  // Too late!

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Won't work
```

**Fix:**
```cpp
// ✓ GOOD: Configure before show
QQuickView view;

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);

view.show();  // Now it works
```

---

### Pitfall 3: Complex Logic in QML

```qml
// ✗ BAD: Complex computation in QML
Text {
    text: {
        var result = 0;
        for (var i = 0; i < 1000; i++) {
            result += i * model.getValue(i);
        }
        return result;
    }
}
```

**Fix:**
```cpp
// ✓ GOOD: Move to C++ model
class Model : public QObject {
    Q_PROPERTY(int computedValue READ computedValue NOTIFY valueChanged)
    
    int computedValue() const {
        int result = 0;
        for (int i = 0; i < 1000; i++) {
            result += i * getValue(i);
        }
        return result;
    }
};
```

```qml
Text { text: model.computedValue }  // Simple binding
```

---

### Pitfall 4: Memory Leaks with QML Objects

```qml
// ✗ BAD: Creating objects without parent
Button {
    onClicked: {
        var obj = Qt.createQmlObject(
            'import QtQuick 2.15; Rectangle {}',
            null  // No parent = memory leak!
        )
    }
}
```

**Fix:**
```qml
// ✓ GOOD: Provide parent
Button {
    onClicked: {
        var obj = Qt.createQmlObject(
            'import QtQuick 2.15; Rectangle {}',
            parent  // Will be cleaned up
        )
    }
}
```

---

### Pitfall 5: Not Handling Model Deletion

```cpp
// ✗ BAD: Model deleted, QML still references it
{
    PanelModel model;
    view.rootContext()->setContextProperty("model", &model);
}  // model destroyed here
view.show();  // CRASH! QML tries to access deleted model
```

**Fix:**
```cpp
// ✓ GOOD: Keep model alive
PanelModel *model = new PanelModel(&app);
view.rootContext()->setContextProperty("model", model);
view.show();
// model cleaned up when app destroyed
```

---

### Pitfall 6: Blocking Main Thread

```cpp
// ✗ BAD: Heavy operation on main thread
Q_INVOKABLE void loadData() {
    m_data = expensiveOperation();  // UI freezes!
    emit dataChanged();
}
```

**Fix:**
```cpp
// ✓ GOOD: Use async
Q_INVOKABLE void loadData() {
    QtConcurrent::run([this]() {
        auto data = expensiveOperation();
        
        QMetaObject::invokeMethod(this, [this, data]() {
            m_data = data;
            emit dataChanged();
        });
    });
}
```

---

### Pitfall 7: Excessive Property Bindings

```qml
// ✗ BAD: Too many bindings
Rectangle {
    width: parent.width * 0.5 + model.offset - theme.margin
    height: parent.height * 0.3 + model.height
    x: parent.x + model.x + (parent.width - width) / 2
    y: parent.y + model.y + theme.padding
    // Every property change triggers 4 recalculations!
}
```

**Fix:**
```qml
// ✓ GOOD: Simplified bindings
Rectangle {
    anchors.centerIn: parent
    width: parent.width * 0.5
    height: model.height
}
```

---

## Checklist: Before Production

### Model Checklist
- [ ] All properties have NOTIFY signals
- [ ] Q_INVOKABLE methods are thread-safe
- [ ] Heavy operations are async
- [ ] Error states handled
- [ ] Unit tests written
- [ ] No UI dependencies in model

### QML Checklist
- [ ] No complex logic in bindings
- [ ] Animations smooth (60 FPS)
- [ ] Accessible (contrast, sizes, keyboard nav)
- [ ] Responsive to different screen sizes
- [ ] No memory leaks (proper parent/child)
- [ ] Lazy loading where appropriate

### LayerShell Checklist
- [ ] Configured before show()
- [ ] Appropriate layer for use case
- [ ] Correct exclusive zone
- [ ] Proper anchors
- [ ] Keyboard interactivity set
- [ ] Scope configured (multi-monitor)

### Integration Checklist
- [ ] Model lifetime managed
- [ ] QML resources embedded (.qrc)
- [ ] CMake configured correctly
- [ ] Tested on real Wayland compositor
- [ ] Error handling for unavailable layer shell
- [ ] Documentation written

---

## Summary: Golden Rules

### 1. Separation
**Model (C++)** = Data & Logic  
**View (QML)** = UI & Presentation  
**Never mix!**

### 2. Properties
Always use `Q_PROPERTY` with `NOTIFY`

### 3. Performance
- Keep QML simple
- Complex logic in C++
- Use async for heavy ops

### 4. Lifecycle
Configure LayerShell **before** show()

### 5. Testing
Test models independently

### 6. Error Handling
Always check layer shell availability

### 7. User Experience
Respect user space, smooth animations

---

## Quick Reference: Do's and Don'ts

### ✓ DO
- Separate Model and View
- Use Q_PROPERTY with NOTIFY
- Configure LayerShell before show()
- Test models independently
- Keep QML simple
- Use appropriate layers
- Handle errors gracefully
- Optimize for performance

### ✗ DON'T
- Mix UI code in models
- Forget NOTIFY signals
- Configure LayerShell after show()
- Put complex logic in QML
- Block main thread
- Use wrong layer
- Leak memory
- Ignore accessibility

---

## Next Steps

You now understand best practices for QML + Model-View with LayerShellQt!

**Practice by:**
1. Refactoring existing QWidget code to QML
2. Building new projects with Model-View from start
3. Reviewing code against these patterns
4. Contributing to KDE projects

**Resources:**
- Qt QML Best Practices: https://doc.qt.io/qt-5/qtquick-bestpractices.html
- Qt Performance: https://doc.qt.io/qt-5/qtquick-performance.html
- Model/View Programming: https://doc.qt.io/qt-5/model-view-programming.html

---

**Happy coding with best practices! 🎨**
