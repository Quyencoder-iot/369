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

### Principle 1: Separation of Concerns

**Model (C++) = Business Logic, View (QML) = UI**

```cpp
// ✓ GOOD: Clear separation
// Model.h - Business logic only
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
private:
    void updateTime() {
        m_time = QTime::currentTime().toString();
        emit timeChanged();
    }
};
```

```qml
// View.qml - UI only
Rectangle {
    Text {
        text: panelModel.time  // Data binding
        color: "white"
    }
}
```

```cpp
// ✗ BAD: Mixed concerns (old QWidget way)
class Panel : public QWidget {
    void updateUI() {
        // Logic + UI mixed
        QString time = QTime::currentTime().toString();
        m_label->setText(time);  // Direct UI manipulation
    }
};
```

**Rule:** Model không biết về UI, View không có business logic.

---

### Principle 2: Configure Layer Shell Before Show

**Không đổi!** Vẫn phải configure LayerShellQt trước khi show.

```cpp
// ✓ GOOD
QQuickView view;
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Configure first
view.show();  // Then show

// ✗ BAD
view.show();
layerWindow->setLayer(LayerTop);  // Too late!
```

---

### Principle 3: Use Appropriate Layer

Không đổi từ version cũ, nhưng apply cho QML:

| Use Case | Layer | QML View |
|----------|-------|----------|
| Panel | TOP | Panel.qml with exclusive zone |
| Dock | TOP | Dock.qml with app list |
| Widget | BOTTOM | Widget.qml, passive |
| Wallpaper | BACKGROUND | Wallpaper.qml, animated |
| Lock Screen | OVERLAY | LockScreen.qml, full screen |

---

### Principle 4: Declarative UI Over Imperative

**QML là declarative - embrace it!**

```qml
// ✓ GOOD: Declarative
Rectangle {
    color: panelModel.isActive ? "#4fc3f7" : "#2d2d2d"
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
}
```

```qml
// ✗ BAD: Imperative (fighting QML)
Rectangle {
    id: rect
    
    Connections {
        target: panelModel
        onIsActiveChanged: {
            if (panelModel.isActive) {
                rect.color = "#4fc3f7"
            } else {
                rect.color = "#2d2d2d"
            }
        }
    }
}
```

---

### Principle 5: Data Binding Over Manual Updates

```qml
// ✓ GOOD: Automatic binding
Text {
    text: panelModel.currentTime  // Auto-updates!
}
```

```cpp
// ✗ BAD: Manual updates (old way)
void updateUI() {
    label->setText(model.currentTime());  // Manual
}
```

---

## Model-View Patterns

### Pattern 1: Simple Property Model

**Use case:** Single values (strings, numbers, booleans)

```cpp
// SimpleModel.h
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
    Q_PROPERTY(int value READ value NOTIFY valueChanged)
    
public:
    SimpleModel(QObject *parent = nullptr) : QObject(parent) {}
    
    QString title() const { return m_title; }
    void setTitle(const QString &title) {
        if (m_title != title) {
            m_title = title;
            emit titleChanged();
        }
    }
    
    bool enabled() const { return m_enabled; }
    int value() const { return m_value; }
    
signals:
    void titleChanged();
    void enabledChanged();
    void valueChanged();
    
private:
    QString m_title;
    bool m_enabled = true;
    int m_value = 0;
};
```

```qml
// View.qml
Rectangle {
    Text {
        text: model.title
        color: model.enabled ? "white" : "gray"
    }
    
    ProgressBar {
        value: model.value / 100
    }
}
```

---

### Pattern 2: List Model

**Use case:** Dynamic lists (docks, menus, notifications)

```cpp
// ListItemModel.h
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
    
    Q_INVOKABLE void addApp(const QString &name, const QString &icon, const QString &cmd) {
        beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());
        m_apps.append({name, icon, cmd});
        endInsertRows();
    }
    
    Q_INVOKABLE void removeApp(int index) {
        if (index >= 0 && index < m_apps.count()) {
            beginRemoveRows(QModelIndex(), index, index);
            m_apps.removeAt(index);
            endRemoveRows();
        }
    }
    
private:
    QList<App> m_apps;
};
```

```qml
// Dock.qml
Rectangle {
    RowLayout {
        Repeater {
            model: appListModel  // Bind to list model
            
            delegate: Button {
                text: model.icon
                ToolTip.text: model.name
                
                onClicked: {
                    // Launch app
                    Qt.application.execute(model.command)
                }
            }
        }
    }
}
```

**Key points:**
- Use `QAbstractListModel` for lists
- Define custom roles
- Implement `roleNames()` for QML access
- Use `Q_INVOKABLE` for actions

---

### Pattern 3: Action Model

**Use case:** Models với methods (execute actions)

```cpp
// ActionModel.h
class PanelActionModel : public QObject {
    Q_OBJECT
    
public:
    Q_INVOKABLE void showMenu() {
        // Show application menu
    }
    
    Q_INVOKABLE void launchApp(const QString &appName) {
        QProcess::startDetached(appName, {});
    }
    
    Q_INVOKABLE void toggleFullscreen() {
        emit fullscreenRequested();
    }
    
signals:
    void fullscreenRequested();
};
```

```qml
// Panel.qml
Rectangle {
    Button {
        text: "Menu"
        onClicked: panelModel.showMenu()
    }
    
    Button {
        text: "Launch"
        onClicked: panelModel.launchApp("firefox")
    }
}
```

---

### Pattern 4: Manager Pattern

**Use case:** Manage multiple views (multi-monitor)

```cpp
// ViewManager.h
class PanelViewManager : public QObject {
    Q_OBJECT
    
public:
    PanelViewManager(MyModel *model, QObject *parent = nullptr)
        : QObject(parent), m_model(model) {
        
        // Create views for existing screens
        for (int i = 0; i < model->screenCount(); ++i) {
            createViewForScreen(model->getScreenName(i));
        }
        
        // Watch for screen changes
        connect(model, &MyModel::screenAdded, 
                this, &PanelViewManager::createViewForScreen);
        connect(model, &MyModel::screenRemoved,
                this, &PanelViewManager::removeViewForScreen);
    }
    
    ~PanelViewManager() {
        qDeleteAll(m_views);
    }
    
private slots:
    void createViewForScreen(const QString &screenName) {
        QQuickView *view = new QQuickView();
        
        // Expose model to QML
        view->rootContext()->setContextProperty("model", m_model);
        view->rootContext()->setContextProperty("screenName", screenName);
        
        // Load QML
        view->setSource(QUrl("qrc:/Panel.qml"));
        
        // Configure LayerShell
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setScope(screenName);  // Per-screen
        
        view->show();
        m_views.insert(screenName, view);
    }
    
    void removeViewForScreen(const QString &screenName) {
        QQuickView *view = m_views.take(screenName);
        if (view) view->deleteLater();
    }
    
private:
    MyModel *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

**Usage:**
```cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    MyModel model;
    PanelViewManager manager(&model);  // Manages all views
    
    return app.exec();
}
```

---

### Pattern 5: Singleton Model (Global State)

**Use case:** Shared state across multiple views

```cpp
// GlobalModel.h
class GlobalModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme NOTIFY themeChanged)
    
public:
    static GlobalModel* instance() {
        static GlobalModel instance;
        return &instance;
    }
    
    QString theme() const { return m_theme; }
    
    Q_INVOKABLE void setTheme(const QString &theme) {
        if (m_theme != theme) {
            m_theme = theme;
            emit themeChanged();
        }
    }
    
signals:
    void themeChanged();
    
private:
    GlobalModel() = default;
    QString m_theme = "dark";
};
```

```cpp
// Expose to all views
view->rootContext()->setContextProperty("globalModel", GlobalModel::instance());
```

```qml
// Any view can access
Rectangle {
    color: globalModel.theme === "dark" ? "#2d2d2d" : "#f0f0f0"
}
```

---

## QML Best Practices

### Practice 1: Component Organization

**Organize QML files by component:**

```
qml/
├── components/
│   ├── Button.qml
│   ├── Panel.qml
│   └── Widget.qml
├── views/
│   ├── TopPanel.qml
│   ├── BottomDock.qml
│   └── LockScreen.qml
└── main.qml
```

**Reusable components:**
```qml
// components/CustomButton.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root
    
    property color accentColor: "#4fc3f7"
    
    contentItem: Text {
        text: root.text
        color: "white"
        horizontalAlignment: Text.AlignHCenter
    }
    
    background: Rectangle {
        color: root.hovered ? Qt.lighter(root.accentColor) : root.accentColor
        radius: 5
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
```

**Usage:**
```qml
// TopPanel.qml
import "./components"

Rectangle {
    CustomButton {
        text: "Menu"
        accentColor: "#ff5555"
    }
}
```

---

### Practice 2: Property Aliases

**Expose internal properties:**

```qml
// ✓ GOOD: Clean API
// Panel.qml
Rectangle {
    id: root
    
    // Public API
    property alias title: titleText.text
    property alias backgroundColor: root.color
    
    Text {
        id: titleText
        // ... internal implementation
    }
}

// Usage
Panel {
    title: "My Panel"
    backgroundColor: "#2d2d2d"
}
```

```qml
// ✗ BAD: Exposing internals
Panel {
    children[0].text = "My Panel"  // Fragile!
}
```

---

### Practice 3: Layouts Over Absolute Positioning

```qml
// ✓ GOOD: Responsive layout
RowLayout {
    anchors.fill: parent
    spacing: 10
    
    Text { text: "Left" }
    Item { Layout.fillWidth: true }  // Spacer
    Text { text: "Right" }
}
```

```qml
// ✗ BAD: Absolute positioning
Item {
    Text {
        x: 10
        y: 10
        text: "Left"
    }
    
    Text {
        x: parent.width - 50  // Fragile!
        y: 10
        text: "Right"
    }
}
```

---

### Practice 4: Behavior Animations

**Smooth transitions automatically:**

```qml
Rectangle {
    color: model.isActive ? "#4fc3f7" : "#2d2d2d"
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
}

Text {
    opacity: model.visible ? 1.0 : 0.0
    
    Behavior on opacity {
        NumberAnimation { duration: 300 }
    }
}
```

---

### Practice 5: States for Complex UI

```qml
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
            easing.type: Easing.OutCubic
        }
    }
    
    // Change state
    MouseArea {
        anchors.fill: parent
        onClicked: panel.state = (panel.state === "collapsed") ? "expanded" : "collapsed"
    }
}
```

---

### Practice 6: Connections Block

**Handle model signals cleanly:**

```qml
Rectangle {
    Connections {
        target: panelModel
        
        function onDataUpdated() {
            console.log("Data updated")
            // React to signal
        }
        
        function onError(message) {
            errorDialog.show(message)
        }
    }
}
```

---

## Performance Optimization

### Optimization 1: Lazy Loading

**Don't create everything upfront:**

```qml
// ✓ GOOD: Lazy loader
Rectangle {
    Loader {
        id: menuLoader
        active: false
        source: "Menu.qml"
    }
    
    Button {
        text: "Show Menu"
        onClicked: menuLoader.active = true
    }
}
```

```qml
// ✗ BAD: Always loaded
Rectangle {
    Menu {
        id: menu
        visible: false  // Still loaded!
    }
    
    Button {
        text: "Show Menu"
        onClicked: menu.visible = true
    }
}
```

---

### Optimization 2: ListView Over Repeater

**For large lists:**

```qml
// ✓ GOOD: Efficient for large lists
ListView {
    model: appListModel
    delegate: AppItem {
        name: model.name
        icon: model.icon
    }
    
    // Only visible items are created!
}
```

```qml
// ✗ BAD: All items created
Repeater {
    model: appListModel  // All 1000 items created!
    delegate: AppItem { }
}
```

---

### Optimization 3: Efficient Property Bindings

```qml
// ✓ GOOD: Simple binding
Text {
    text: model.value
}
```

```qml
// ✗ BAD: Complex expression (recalculated often)
Text {
    text: {
        var result = "";
        for (var i = 0; i < 1000; i++) {
            result += model.value + " ";
        }
        return result;
    }
}
```

**Solution:** Move complex logic to model:
```cpp
// Model
Q_PROPERTY(QString formattedValue READ formattedValue NOTIFY valueChanged)

QString formattedValue() const {
    // Complex calculation here
    return result;
}
```

```qml
Text {
    text: model.formattedValue  // Simple binding
}
```

---

### Optimization 4: Cache Shapes

```qml
Rectangle {
    layer.enabled: true  // Cache as texture
    layer.smooth: true
    
    // Complex content
}
```

---

### Optimization 5: Model Updates

**Batch model changes:**

```cpp
// ✓ GOOD: Batch updates
void updateMultipleRows() {
    emit layoutAboutToBeChanged();
    // Make multiple changes
    for (int i = 0; i < 100; ++i) {
        m_data[i] = newValue;
    }
    emit layoutChanged();  // Single update signal
}
```

```cpp
// ✗ BAD: Individual updates
void updateMultipleRows() {
    for (int i = 0; i < 100; ++i) {
        m_data[i] = newValue;
        emit dataChanged(index(i), index(i));  // 100 signals!
    }
}
```

---

## Error Handling

### Pattern 1: Model Error Properties

```cpp
// Model.h
class SafeModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorChanged)
    
public:
    QString errorMessage() const { return m_errorMessage; }
    bool hasError() const { return !m_errorMessage.isEmpty(); }
    
    Q_INVOKABLE void riskyOperation() {
        try {
            // Do something
        } catch (const std::exception &e) {
            setError(e.what());
        }
    }
    
signals:
    void errorChanged();
    
private:
    void setError(const QString &error) {
        m_errorMessage = error;
        emit errorChanged();
    }
    
    void clearError() {
        m_errorMessage.clear();
        emit errorChanged();
    }
    
    QString m_errorMessage;
};
```

```qml
// View.qml
Rectangle {
    // Error banner
    Rectangle {
        visible: model.hasError
        color: "#ff5555"
        height: 30
        
        Text {
            text: model.errorMessage
            color: "white"
        }
    }
    
    Button {
        text: "Try Again"
        onClicked: {
            model.riskyOperation()
        }
    }
}
```

---

### Pattern 2: Check Layer Shell Availability

```cpp
// main.cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        // Show error in QML
        QQuickView errorView;
        errorView.rootContext()->setContextProperty("errorMsg", 
            "Layer shell not available. Please use a supported compositor.");
        errorView.setSource(QUrl("qrc:/Error.qml"));
        errorView.show();
        return app.exec();
    }
    
    // Normal initialization
    // ...
}
```

```qml
// Error.qml
Rectangle {
    color: "#2d2d2d"
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        
        Text {
            text: "❌"
            font.pixelSize: 64
            color: "#ff5555"
        }
        
        Text {
            text: "Error"
            font.pixelSize: 24
            color: "white"
            font.bold: true
        }
        
        Text {
            text: errorMsg
            font.pixelSize: 14
            color: "#ccc"
            wrapMode: Text.WordWrap
            Layout.preferredWidth: 400
        }
        
        Button {
            text: "Exit"
            onClicked: Qt.quit()
        }
    }
}
```

---

### Pattern 3: Graceful Degradation

```cpp
// main.cpp
if (!LayerShellQt::Shell::isAvailable()) {
    qWarning() << "Layer shell not available, using fallback";
    
    // Fallback: Normal window
    QQuickView view;
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    // No layer shell configuration
    view.setFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    view.show();
    
} else {
    // Normal layer shell path
    QQuickView view;
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    
    view.show();
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
        QVERIFY(!model.title().isEmpty());
        QCOMPARE(model.isEnabled(), true);
    }
    
    void testPropertyChange() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::titleChanged);
        
        model.setTitle("New Title");
        
        QCOMPARE(spy.count(), 1);
        QCOMPARE(model.title(), "New Title");
    }
    
    void testAction() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::actionCompleted);
        
        model.executeAction();
        
        QCOMPARE(spy.count(), 1);
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
            // Test component
        }
    }
    
    function test_creation() {
        var panel = createTemporaryObject(panelComponent, testCase)
        verify(panel !== null)
    }
    
    function test_button_click() {
        var panel = createTemporaryObject(panelComponent, testCase)
        var button = findChild(panel, "menuButton")
        
        mouseClick(button)
        
        // Verify result
        verify(panel.menuVisible)
    }
}
```

---

### Strategy 3: Mock Models for QML Testing

```cpp
// MockModel.h
class MockPanelModel : public PanelModel {
    Q_OBJECT
public:
    Q_INVOKABLE void simulateUpdate() {
        // Simulate data update for testing
        setTitle("Test Title");
        emit dataUpdated();
    }
};
```

```qml
// Test with mock
TestCase {
    function test_data_binding() {
        var model = MockPanelModel {}
        var panel = Panel {
            panelModel: model
        }
        
        model.simulateUpdate()
        
        compare(panel.titleText, "Test Title")
    }
}
```

---

### Strategy 4: Integration Tests

```cpp
// test_integration.cpp
void testLayerShellIntegration() {
    if (!LayerShellQt::Shell::isAvailable()) {
        QSKIP("Layer shell not available");
    }
    
    PanelModel model;
    QQuickView view;
    
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    
    view.show();
    
    QVERIFY(QTest::qWaitForWindowExposed(&view));
    
    // Test layer shell properties
    QCOMPARE(layerWindow->layer(), LayerShellQt::Window::LayerTop);
}
```

---

## Common Pitfalls

### Pitfall 1: Forgetting NOTIFY Signal

```cpp
// ✗ WRONG: No NOTIFY signal
Q_PROPERTY(QString title READ title WRITE setTitle)
```

```cpp
// ✓ CORRECT: Include NOTIFY
Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

signals:
    void titleChanged();
```

**Result:** QML won't auto-update when property changes!

---

### Pitfall 2: Direct QML Object Manipulation from C++

```cpp
// ✗ WRONG: Accessing QML objects from C++
QObject *rootObject = view.rootObject();
QObject *button = rootObject->findChild<QObject*>("menuButton");
button->setProperty("text", "New Text");  // Anti-pattern!
```

```cpp
// ✓ CORRECT: Use Model
model.setButtonText("New Text");
```

```qml
// QML binds to model
Button {
    text: model.buttonText
}
```

---

### Pitfall 3: Heavy Computation in Property Getters

```cpp
// ✗ WRONG: Expensive calculation in getter
QString computedValue() const {
    QString result;
    for (int i = 0; i < 10000; ++i) {
        result += QString::number(i);
    }
    return result;  // Called every frame!
}
```

```cpp
// ✓ CORRECT: Cache result
void updateValue() {
    m_cachedValue = expensiveCalculation();
    emit valueChanged();
}

QString value() const {
    return m_cachedValue;  // Fast getter
}
```

---

### Pitfall 4: Memory Leaks with QQuickView

```cpp
// ✗ WRONG: Memory leak
void createPanel(const QString &screen) {
    QQuickView *view = new QQuickView();
    // ... configure
    view->show();
    // Never deleted!
}
```

```cpp
// ✓ CORRECT: Track and cleanup
QHash<QString, QQuickView*> m_views;

void createPanel(const QString &screen) {
    QQuickView *view = new QQuickView();
    // ... configure
    view->show();
    m_views.insert(screen, view);
}

void removePanel(const QString &screen) {
    QQuickView *view = m_views.take(screen);
    if (view) view->deleteLater();
}

~Manager() {
    qDeleteAll(m_views);
}
```

---

### Pitfall 5: Not Using Anchors in QML

```qml
// ✗ WRONG: Absolute positioning
Rectangle {
    Text {
        x: 10
        y: 10
    }
}
```

```qml
// ✓ CORRECT: Anchors or layouts
Rectangle {
    Text {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
    }
}

// Or use layouts
RowLayout {
    Text { }
}
```

---

### Pitfall 6: Exposing Too Much to QML

```cpp
// ✗ WRONG: Everything public
class Model : public QObject {
    Q_INVOKABLE void internalMethod();  // Shouldn't be invokable!
    Q_INVOKABLE void privateHelper();   // Shouldn't be invokable!
    Q_INVOKABLE void publicAction();
};
```

```cpp
// ✓ CORRECT: Expose only what's needed
class Model : public QObject {
    Q_INVOKABLE void publicAction();  // Only this is invokable
    
private:
    void internalMethod();
    void privateHelper();
};
```

---

### Pitfall 7: Blocking UI Thread

```cpp
// ✗ WRONG: Long operation on main thread
Q_INVOKABLE void loadData() {
    for (int i = 0; i < 1000000; ++i) {
        processItem(i);  // UI freezes!
    }
}
```

```cpp
// ✓ CORRECT: Use QtConcurrent or QThread
Q_INVOKABLE void loadData() {
    QtConcurrent::run([this]() {
        for (int i = 0; i < 1000000; ++i) {
            processItem(i);
        }
        
        // Update UI on main thread
        QMetaObject::invokeMethod(this, [this]() {
            emit dataLoaded();
        }, Qt::QueuedConnection);
    });
}
```

---

## Architecture Checklist

### Before Production

- [ ] **Models tested independently**
- [ ] **QML views use data binding (not imperative)**
- [ ] **Layer shell configured before show()**
- [ ] **Error handling implemented**
- [ ] **Memory leaks checked (Valgrind/Qt Creator)**
- [ ] **Performance profiled (QML Profiler)**
- [ ] **Multi-monitor tested**
- [ ] **Fallback for unsupported compositors**
- [ ] **All Q_PROPERTY have NOTIFY signals**
- [ ] **No heavy computation in property getters**
- [ ] **Thread safety verified**
- [ ] **Resources (.qrc) properly configured**

---

## Code Review Checklist

### Model (C++)
- [ ] Clear responsibilities (no UI code)
- [ ] All properties have NOTIFY signals
- [ ] Invokable methods documented
- [ ] Error handling implemented
- [ ] Thread-safe if needed
- [ ] Memory management correct

### View (QML)
- [ ] Declarative, not imperative
- [ ] Uses data binding
- [ ] Anchors or layouts (not absolute positioning)
- [ ] Animations smooth (Behavior)
- [ ] No business logic
- [ ] Reusable components extracted

### Integration (main.cpp)
- [ ] Layer shell availability checked
- [ ] Models exposed correctly
- [ ] Layer shell configured properly
- [ ] Resource cleanup handled
- [ ] Multi-monitor support if needed

---

## Summary: Golden Rules

### Model (C++)
1. **Business logic only** - No UI manipulation
2. **NOTIFY signals** - Always include for Q_PROPERTY
3. **Fast getters** - Cache expensive computations
4. **Q_INVOKABLE** - Only expose what QML needs
5. **Thread safety** - Use QtConcurrent for long operations

### View (QML)
1. **Declarative** - Use property bindings, not imperative code
2. **Layouts** - Use anchors/layouts, not absolute positioning
3. **Components** - Extract reusable components
4. **Animations** - Use Behavior for smooth transitions
5. **No logic** - Keep business logic in model

### Integration
1. **Check availability** - LayerShellQt::Shell::isAvailable()
2. **Configure first** - Layer shell before show()
3. **Proper scope** - Per-screen for multi-monitor
4. **Cleanup** - Delete views when screens removed
5. **Error handling** - Graceful degradation

---

## Resources

### QML Learning
- **Qt QML**: https://doc.qt.io/qt-5/qmlapplications.html
- **QML Performance**: https://doc.qt.io/qt-5/qtquick-performance.html
- **QML Best Practices**: https://doc.qt.io/qt-5/qtquick-bestpractices.html

### Model-View
- **Model/View**: https://doc.qt.io/qt-5/model-view-programming.html
- **C++ Integration**: https://doc.qt.io/qt-5/qtqml-cppintegration-topic.html

### Testing
- **Qt Test**: https://doc.qt.io/qt-5/qtest-overview.html
- **QML Test**: https://doc.qt.io/qt-5/qttest-qmlmodule.html

---

**Follow these best practices for clean, maintainable, and performant LayerShellQt applications! 🚀**

---

*Updated: December 2024*
*QML + Model-View Architecture*
