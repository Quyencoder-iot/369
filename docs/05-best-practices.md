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

### Principle 1: Separate Model from View

**Always separate business logic (Model) from UI (View)!**

```cpp
// ✓ GOOD: Model (C++) - Logic only
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
// ✓ GOOD: View (QML) - UI only
Rectangle {
    Text {
        text: panelModel.time  // Just display data
    }
}
```

```cpp
// ✗ BAD: Mixed logic and UI
Rectangle {
    Text {
        id: timeText
        
        Timer {
            interval: 1000
            running: true
            onTriggered: {
                // Business logic in QML - BAD!
                timeText.text = Qt.formatTime(new Date())
            }
        }
    }
}
```

**Why?**
- ✅ Testable: Test model independently
- ✅ Reusable: Same model, different views
- ✅ Maintainable: Change logic without touching UI

---

### Principle 2: Use Q_PROPERTY for Data Binding

**Expose data via Q_PROPERTY for automatic QML binding.**

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
// ✓ Automatic binding in QML
Text {
    text: model.value  // Auto-updates when valueChanged emitted
}
```

```cpp
// ✗ BAD: No NOTIFY signal
Q_PROPERTY(int value READ value)  // No auto-update!
```

**Rule:** Every Q_PROPERTY should have a NOTIFY signal (except CONSTANT).

---

### Principle 3: Configure LayerShell Before Show

**Always configure LayerShellQt properties BEFORE showing the view!**

```cpp
// ✓ GOOD
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(...);

view.show();  // Now it works!
```

```cpp
// ✗ BAD
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));
view.show();  // Surface created!

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);  // Too late!
```

---

### Principle 4: Use Appropriate Layer

**Choose layer based on use case:**

```cpp
// ✓ GOOD: Panel on TOP
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setExclusiveZone(35);

// ✓ GOOD: Widget on BOTTOM
layerWindow->setLayer(LayerShellQt::Window::LayerBottom);
layerWindow->setExclusiveZone(0);

// ✓ GOOD: Wallpaper on BACKGROUND
layerWindow->setLayer(LayerShellQt::Window::LayerBackground);

// ✗ BAD: Panel on OVERLAY (too aggressive!)
layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
```

**Layer Decision Tree:**
```
Need to block all input? → OVERLAY (lock screen)
Need exclusive space?   → TOP (panel, dock)
Passive display?        → BOTTOM (widget)
Background?             → BACKGROUND (wallpaper)
```

---

## Coding Patterns

### Pattern 1: Base Model Class

**Create reusable base model for common functionality.**

```cpp
// BaseModel.h
class BaseModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    
public:
    explicit BaseModel(QObject *parent = nullptr) 
        : QObject(parent), m_ready(false) {}
    
    bool ready() const { return m_ready; }
    
protected:
    void setReady(bool ready) {
        if (m_ready != ready) {
            m_ready = ready;
            emit readyChanged();
        }
    }
    
signals:
    void readyChanged();
    void error(const QString &message);
    
private:
    bool m_ready;
};
```

**Usage:**
```cpp
class PanelModel : public BaseModel {
    Q_OBJECT
    
public:
    PanelModel() {
        // Initialize
        loadData();
        setReady(true);  // Use base functionality
    }
};
```

---

### Pattern 2: View Manager for Multi-Window

**Centralized management of multiple QML views.**

```cpp
// ViewManager.h
class ViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit ViewManager(QObject *parent = nullptr) 
        : QObject(parent) {}
    
    // Create view with standard setup
    QQuickView* createView(const QUrl &source, 
                          QObject *model,
                          const QString &modelName) {
        QQuickView *view = new QQuickView();
        
        // Set model
        view->rootContext()->setContextProperty(modelName, model);
        
        // Load QML
        view->setSource(source);
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        // Store
        m_views.append(view);
        
        return view;
    }
    
    // Configure LayerShell with defaults
    void configureLayerShell(QQuickView *view,
                            LayerShellQt::Window::Layer layer,
                            LayerShellQt::Window::Anchors anchors,
                            int exclusiveZone = 0) {
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(layer);
        layerWindow->setAnchors(anchors);
        layerWindow->setExclusiveZone(exclusiveZone);
    }
    
    ~ViewManager() {
        qDeleteAll(m_views);
    }
    
private:
    QList<QQuickView*> m_views;
};
```

**Usage:**
```cpp
ViewManager manager;
PanelModel model;

// Create and configure view
auto view = manager.createView(
    QUrl("qrc:/Panel.qml"), 
    &model, 
    "panelModel"
);

manager.configureLayerShell(
    view,
    LayerShellQt::Window::LayerTop,
    LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight,
    35
);

view->show();
```

---

### Pattern 3: Model Factory

**Factory pattern for creating models.**

```cpp
// ModelFactory.h
class ModelFactory {
public:
    enum ModelType {
        PanelModel,
        DockModel,
        WidgetModel
    };
    
    static QObject* create(ModelType type) {
        switch (type) {
        case PanelModel:
            return new PanelModel();
        case DockModel:
            return new DockModel();
        case WidgetModel:
            return new WidgetModel();
        }
        return nullptr;
    }
};
```

---

### Pattern 4: Singleton for Global State

**Use singleton for shared application state.**

```cpp
// AppState.h
class AppState : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme NOTIFY themeChanged)
    Q_PROPERTY(int screenCount READ screenCount NOTIFY screenCountChanged)
    
public:
    static AppState* instance() {
        static AppState instance;
        return &instance;
    }
    
    QString theme() const { return m_theme; }
    int screenCount() const { return m_screenCount; }
    
    Q_INVOKABLE void setTheme(const QString &theme) {
        if (m_theme != theme) {
            m_theme = theme;
            emit themeChanged();
        }
    }
    
signals:
    void themeChanged();
    void screenCountChanged();
    
private:
    AppState() = default;
    QString m_theme = "dark";
    int m_screenCount = 1;
};
```

**Register as QML singleton:**
```cpp
// main.cpp
qmlRegisterSingletonType<AppState>(
    "App", 1, 0, "AppState",
    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return AppState::instance();
    }
);
```

**Use in QML:**
```qml
import App 1.0

Rectangle {
    color: AppState.theme === "dark" ? "#2d2d2d" : "#f0f0f0"
    
    Button {
        onClicked: AppState.setTheme("light")
    }
}
```

---

### Pattern 5: Model with List Data

**Use QAbstractListModel for list data.**

```cpp
// ListModel.h
class AppListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
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
        Q_UNUSED(parent);
        return m_apps.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_apps.count())
            return QVariant();
        
        const App &app = m_apps[index.row()];
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
    
    Q_INVOKABLE void addApp(const QString &icon, 
                           const QString &name, 
                           const QString &command) {
        beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());
        m_apps.append({icon, name, command});
        endInsertRows();
        emit countChanged();
    }
    
signals:
    void countChanged();
    
private:
    QList<App> m_apps;
};
```

**Use in QML:**
```qml
ListView {
    model: appListModel
    
    delegate: Button {
        text: model.icon + " " + model.name
        onClicked: Qt.openUrlExternally(model.command)
    }
}
```

---

## UI/UX Guidelines

### Guideline 1: Declarative QML

**Write declarative, not imperative QML.**

```qml
// ✓ GOOD: Declarative
Rectangle {
    color: model.isActive ? "#4fc3f7" : "#2d2d2d"
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
}
```

```qml
// ✗ BAD: Imperative
Rectangle {
    id: rect
    color: "#2d2d2d"
    
    Connections {
        target: model
        onIsActiveChanged: {
            if (model.isActive) {
                rect.color = "#4fc3f7"
            } else {
                rect.color = "#2d2d2d"
            }
        }
    }
}
```

---

### Guideline 2: Use Layouts

**Always use layouts for proper sizing.**

```qml
// ✓ GOOD: RowLayout
RowLayout {
    anchors.fill: parent
    spacing: 10
    
    Text { text: "Left" }
    Item { Layout.fillWidth: true }  // Spacer
    Text { text: "Right" }
}
```

```qml
// ✗ BAD: Manual positioning
Item {
    Text { 
        x: 0
        text: "Left" 
    }
    
    Text { 
        x: parent.width - width
        text: "Right"
    }
}
```

---

### Guideline 3: Responsive Design

**Design for different screen sizes.**

```qml
// ✓ GOOD: Responsive
Rectangle {
    width: parent.width
    height: parent.height < 1080 ? 30 : 40  // Smaller on small screens
    
    Text {
        font.pixelSize: parent.height * 0.4  // Relative size
    }
}
```

---

### Guideline 4: Consistent Theming

**Use theme properties.**

```qml
// Theme.qml
pragma Singleton
import QtQuick 2.15

QtObject {
    readonly property color backgroundColor: "#2d2d2d"
    readonly property color foregroundColor: "#ffffff"
    readonly property color accentColor: "#4fc3f7"
    readonly property int panelHeight: 35
    readonly property int borderRadius: 8
}
```

**Register singleton:**
```cpp
qmlRegisterSingletonType(QUrl("qrc:/Theme.qml"), "Theme", 1, 0, "Theme");
```

**Use:**
```qml
import Theme 1.0

Rectangle {
    color: Theme.backgroundColor
    radius: Theme.borderRadius
    
    Text {
        color: Theme.foregroundColor
    }
}
```

---

### Guideline 5: Animations

**Use smooth animations.**

```qml
// ✓ GOOD: Smooth transition
Rectangle {
    width: 100
    height: model.expanded ? 200 : 35
    
    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
}
```

---

## Performance Best Practices

### Practice 1: Lazy Loading

**Load components only when needed.**

```qml
// ✓ GOOD: Loader
Rectangle {
    Loader {
        id: heavyComponentLoader
        active: false  // Not loaded initially
        sourceComponent: HeavyComponent {}
    }
    
    Button {
        text: "Load"
        onClicked: heavyComponentLoader.active = true
    }
}
```

---

### Practice 2: Minimize Property Bindings

**Avoid complex bindings in loops.**

```qml
// ✗ BAD: Complex binding in Repeater
Repeater {
    model: 1000
    
    delegate: Rectangle {
        color: Qt.rgba(
            Math.sin(index * 0.1), 
            Math.cos(index * 0.2), 
            0.5, 
            1.0
        )  // Recalculated on every change!
    }
}
```

```qml
// ✓ GOOD: Pre-calculate in model
ListView {
    model: colorModel  // Model pre-calculates colors
    
    delegate: Rectangle {
        color: model.color  // Simple binding
    }
}
```

---

### Practice 3: Use Image Caching

**Cache images properly.**

```qml
// ✓ GOOD: Cached image
Image {
    source: "qrc:/images/icon.png"
    cache: true
    asynchronous: true
}
```

---

### Practice 4: Limit Timer Usage

**Use timers wisely.**

```cpp
// ✓ GOOD: Single timer in model
class Model : public QObject {
    QTimer *m_timer;
    
    Model() {
        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, &Model::update);
        m_timer->start();
    }
};
```

```qml
// ✗ BAD: Multiple timers in QML
Repeater {
    model: 10
    
    delegate: Text {
        Timer {  // 10 timers!
            interval: 1000
            running: true
            repeat: true
            onTriggered: { /* ... */ }
        }
    }
}
```

---

### Practice 5: Optimize QML Rendering

**Use clipping and visibility wisely.**

```qml
// ✓ GOOD: Use visible instead of opacity
Rectangle {
    visible: model.isVisible  // Not rendered when false
}
```

```qml
// ✗ BAD: Use opacity for hiding
Rectangle {
    opacity: model.isVisible ? 1 : 0  // Still rendered!
}
```

---

## Error Handling

### Pattern 1: Check LayerShell Availability

```cpp
// ✓ GOOD: Check and handle
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qWarning() << "Layer shell not available!";
        qWarning() << "Compositor:" << qEnvironmentVariable("XDG_SESSION_DESKTOP");
        
        // Show error dialog
        QQuickView errorView;
        errorView.setSource(QUrl("qrc:/ErrorDialog.qml"));
        errorView.show();
        
        return app.exec();
    }
    
    // Normal flow
    // ...
}
```

---

### Pattern 2: Model Error Handling

```cpp
class SafeModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorOccurred)
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorOccurred)
    
public:
    QString errorMessage() const { return m_errorMessage; }
    bool hasError() const { return !m_errorMessage.isEmpty(); }
    
    Q_INVOKABLE void loadData() {
        try {
            // Load data
            m_data = loadFromFile();
            m_errorMessage.clear();
        } catch (const std::exception &e) {
            m_errorMessage = e.what();
            emit errorOccurred();
        }
    }
    
signals:
    void errorOccurred();
    
private:
    QString m_errorMessage;
    QVariant m_data;
};
```

**Handle in QML:**
```qml
Rectangle {
    Connections {
        target: model
        
        function onErrorOccurred() {
            errorDialog.open()
        }
    }
    
    Dialog {
        id: errorDialog
        title: "Error"
        standardButtons: Dialog.Ok
        
        Text {
            text: model.errorMessage
        }
    }
}
```

---

### Pattern 3: Validation

```cpp
class ValidatedModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    
public:
    QString input() const { return m_input; }
    bool isValid() const { return m_isValid; }
    
    void setInput(const QString &input) {
        if (m_input != input) {
            m_input = input;
            emit inputChanged();
            
            validate();
        }
    }
    
signals:
    void inputChanged();
    void isValidChanged();
    
private:
    void validate() {
        bool valid = !m_input.isEmpty() && m_input.length() >= 3;
        
        if (m_isValid != valid) {
            m_isValid = valid;
            emit isValidChanged();
        }
    }
    
    QString m_input;
    bool m_isValid = false;
};
```

**Use in QML:**
```qml
TextField {
    text: model.input
    onTextChanged: model.input = text
    
    color: model.isValid ? "white" : "red"
}

Button {
    text: "Submit"
    enabled: model.isValid
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
    void testInitialState() {
        PanelModel model;
        QVERIFY(!model.currentTime().isEmpty());
    }
    
    void testTimeUpdate() {
        PanelModel model;
        
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        // Wait for signal
        QVERIFY(spy.wait(2000));
        QVERIFY(spy.count() > 0);
    }
    
    void testProperty() {
        PanelModel model;
        
        QString time = model.property("currentTime").toString();
        QVERIFY(!time.isEmpty());
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
        Panel {}
    }
    
    function test_panel_visible() {
        var panel = createTemporaryObject(panelComponent, testCase)
        verify(panel !== null)
        compare(panel.visible, true)
    }
    
    function test_panel_height() {
        var panel = createTemporaryObject(panelComponent, testCase)
        compare(panel.height, 35)
    }
}
```

---

### Strategy 3: Mock Models

```cpp
// MockModel.h
class MockModel : public PanelModel {
    Q_OBJECT
    
public:
    MockModel() {
        // Override with test data
        setCurrentTime("12:34:56");
    }
    
    void setCurrentTime(const QString &time) {
        m_currentTime = time;
        emit timeChanged();
    }
};
```

**Use in tests:**
```cpp
void testView() {
    MockModel model;
    model.setCurrentTime("00:00:00");
    
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    // Test view behavior
}
```

---

## Common Pitfalls

### Pitfall 1: Forgetting NOTIFY Signal

```cpp
// ✗ BAD: No NOTIFY
Q_PROPERTY(QString value READ value)
```

```qml
// QML binding won't update!
Text { text: model.value }
```

**Fix:**
```cpp
// ✓ GOOD: With NOTIFY
Q_PROPERTY(QString value READ value NOTIFY valueChanged)

signals:
    void valueChanged();
```

---

### Pitfall 2: Setting Properties After Show

```cpp
// ✗ BAD
view.show();
layerWindow->setLayer(LayerTop);  // Too late!
```

**Fix:**
```cpp
// ✓ GOOD
layerWindow->setLayer(LayerTop);
view.show();
```

---

### Pitfall 3: Memory Leaks in QML

```qml
// ✗ BAD: Creates object but never deletes
Button {
    onClicked: {
        var obj = Qt.createQmlObject(
            'import QtQuick 2.15; Rectangle { width: 100; height: 100 }',
            parent
        )
        // obj never deleted!
    }
}
```

**Fix:**
```qml
// ✓ GOOD: Use Loader
Loader {
    id: dynamicLoader
    active: false
}

Button {
    onClicked: dynamicLoader.active = !dynamicLoader.active
}
```

---

### Pitfall 4: Blocking Main Thread

```cpp
// ✗ BAD: Blocking operation in model
Q_INVOKABLE void loadData() {
    // Heavy operation blocks UI!
    processLargeFile();
}
```

**Fix:**
```cpp
// ✓ GOOD: Use QtConcurrent
Q_INVOKABLE void loadData() {
    auto future = QtConcurrent::run([this]() {
        return processLargeFile();
    });
    
    auto watcher = new QFutureWatcher<Data>(this);
    connect(watcher, &QFutureWatcher<Data>::finished, [this, watcher]() {
        m_data = watcher->result();
        emit dataLoaded();
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}
```

---

### Pitfall 5: Not Using QML States

```qml
// ✗ BAD: Complex conditional logic
Rectangle {
    color: expanded ? (hovered ? "#4d4d4d" : "#3d3d3d") : "#2d2d2d"
    height: expanded ? (maximized ? 400 : 200) : 35
}
```

**Fix:**
```qml
// ✓ GOOD: Use states
Rectangle {
    states: [
        State {
            name: "collapsed"
            PropertyChanges { target: root; height: 35; color: "#2d2d2d" }
        },
        State {
            name: "expanded"
            PropertyChanges { target: root; height: 200; color: "#3d3d3d" }
        },
        State {
            name: "maximized"
            PropertyChanges { target: root; height: 400; color: "#4d4d4d" }
        }
    ]
    
    transitions: Transition {
        NumberAnimation { properties: "height"; duration: 300 }
        ColorAnimation { duration: 200 }
    }
}
```

---

## Checklist: Production-Ready QML App

### Code Organization
- [ ] Models separated from Views
- [ ] All Q_PROPERTY have NOTIFY signals
- [ ] Q_INVOKABLE for QML-callable methods
- [ ] Models tested independently
- [ ] QML components modular

### LayerShellQt
- [ ] Check `isAvailable()` at startup
- [ ] Configure before `show()`
- [ ] Appropriate layer for use case
- [ ] Correct exclusive zone
- [ ] Handle multi-monitor

### Performance
- [ ] Lazy loading for heavy components
- [ ] Image caching enabled
- [ ] Minimal timers
- [ ] Optimize property bindings
- [ ] Use `visible` not `opacity`

### UI/UX
- [ ] Responsive design
- [ ] Consistent theming
- [ ] Smooth animations
- [ ] Declarative QML
- [ ] Use layouts

### Error Handling
- [ ] Model error properties
- [ ] Validation in models
- [ ] User-friendly error messages
- [ ] Graceful degradation

### Testing
- [ ] Unit tests for models
- [ ] QML tests for views
- [ ] Mock models for testing
- [ ] Integration tests

---

## Summary: QML Best Practices

### Golden Rules

1. **Separate Model & View** - Always!
2. **Use Q_PROPERTY** - For data binding
3. **Configure Before Show** - LayerShell setup
4. **Declarative QML** - Not imperative
5. **Test Models** - Independently
6. **Handle Errors** - Gracefully
7. **Optimize Performance** - Lazy load, cache
8. **Consistent Theme** - Use singleton
9. **Smooth Animations** - Behavior & Transition
10. **Clean Architecture** - Maintainable code

---

## Resources

### Qt Documentation
- **QML Applications:** https://doc.qt.io/qt-5/qmlapplications.html
- **Model/View:** https://doc.qt.io/qt-5/model-view-programming.html
- **Best Practices:** https://doc.qt.io/qt-5/qtquick-bestpractices.html
- **Performance:** https://doc.qt.io/qt-5/qtquick-performance.html

### Examples
- See `docs/03-examples.md` for complete QML examples
- See `examples/` folder for working code

---

**Build clean, maintainable, production-ready LayerShellQt apps with QML! 🎨**
