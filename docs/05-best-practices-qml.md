# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model-View Patterns](#model-view-patterns)
3. [QML Best Practices](#qml-best-practices)
4. [C++ Model Best Practices](#c-model-best-practices)
5. [Performance Best Practices](#performance-best-practices)
6. [Error Handling](#error-handling)
7. [Testing Strategies](#testing-strategies)
8. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Separate Model from View

**Always separate business logic (C++) from UI (QML)!**

```cpp
// ✓ GOOD: Model has logic only
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
private:
    QString m_time;
    // Logic to update time
};
```

```qml
// ✓ GOOD: View has UI only
Rectangle {
    Text { text: panelModel.time }  // Just display data
}
```

```cpp
// ✗ BAD: Mixing UI and logic
class Panel : public QWidget {
    QLabel *label;  // UI
    void updateTime() { /* logic */ }  // Mixed!
};
```

---

### Principle 2: Use Appropriate Layer

**Choose layer theo use case:**

```qml
// ✓ GOOD: Panel on TOP
// main.cpp
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setExclusiveZone(35);
```

```qml
// ✓ GOOD: Wallpaper on BACKGROUND
layerWindow->setLayer(LayerShellQt::Window::LayerBackground);
layerWindow->setExclusiveZone(0);
```

```qml
// ✗ BAD: Wallpaper on OVERLAY
layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);  // Too aggressive!
```

**Layer Decision Matrix:**

| Use Case | Layer | Exclusive Zone | Keyboard |
|----------|-------|----------------|----------|
| Panel/Taskbar | TOP | > 0 | Exclusive |
| Dock | TOP | > 0 | Exclusive |
| Notification | TOP | 0 | None |
| Widget | BOTTOM | 0 | None |
| Wallpaper | BACKGROUND | 0 | None |
| Lock Screen | OVERLAY | -1 | Exclusive |

---

### Principle 3: Configure Before Show

**Set LayerShellQt properties BEFORE showing window!**

```cpp
// ✓ GOOD
QQuickView view;
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Configure first
view.show();  // Then show
```

```cpp
// ✗ BAD
QQuickView view;
view.show();  // Show first - creates normal window!
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Too late!
```

---

### Principle 4: Data Binding over Manual Updates

**Use QML property binding instead of manual updates:**

```qml
// ✓ GOOD: Automatic binding
Text {
    text: panelModel.currentTime  // Auto-updates!
}
```

```cpp
// Model emits signal
emit timeChanged();  // QML updates automatically
```

```qml
// ✗ BAD: Manual update (don't do this in QML)
Text {
    id: timeText
    Component.onCompleted: {
        // Manually updating - anti-pattern!
        timer.triggered.connect(function() {
            timeText.text = getTime()
        })
    }
}
```

---

## Model-View Patterns

### Pattern 1: Simple Property Model

**For basic data exposure:**

```cpp
// Model (C++)
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    
public:
    QString title() const { return m_title; }
    QString status() const { return m_status; }
    
signals:
    void titleChanged();
    void statusChanged();
    
private:
    QString m_title;
    QString m_status;
};
```

```qml
// View (QML)
Rectangle {
    Text { text: model.title }
    Text { text: model.status }
}
```

```cpp
// Main
QQuickView view;
SimpleModel model;
view.rootContext()->setContextProperty("model", &model);
view.setSource(QUrl("qrc:/View.qml"));
```

**When to use:** Simple data display, single model

---

### Pattern 2: List Model

**For dynamic lists:**

```cpp
// Model (C++)
class ItemsModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        IconRole,
        StatusRole
    };
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_items.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_items.count())
            return QVariant();
        
        const Item &item = m_items.at(index.row());
        switch (role) {
        case NameRole: return item.name;
        case IconRole: return item.icon;
        case StatusRole: return item.status;
        }
        return QVariant();
    }
    
    QHash<int, QByteArray> roleNames() const override {
        return {
            {NameRole, "name"},
            {IconRole, "icon"},
            {StatusRole, "status"}
        };
    }
    
private:
    struct Item {
        QString name;
        QString icon;
        QString status;
    };
    QList<Item> m_items;
};
```

```qml
// View (QML)
ListView {
    model: itemsModel
    delegate: Rectangle {
        Text { text: model.name }
        Text { text: model.icon }
    }
}

// Or with Repeater
Repeater {
    model: itemsModel
    delegate: Button {
        text: model.name
        icon.name: model.icon
    }
}
```

**When to use:** Lists, dynamic content, collections

---

### Pattern 3: Command Model

**For actions and commands:**

```cpp
// Model (C++)
class CommandModel : public QObject {
    Q_OBJECT
    
public:
    Q_INVOKABLE void executeAction(const QString &action) {
        if (action == "save") {
            doSave();
        } else if (action == "load") {
            doLoad();
        }
        emit actionCompleted(action);
    }
    
    Q_INVOKABLE bool canExecute(const QString &action) const {
        // Check if action is available
        return true;
    }
    
signals:
    void actionCompleted(const QString &action);
    
private:
    void doSave() { /* ... */ }
    void doLoad() { /* ... */ }
};
```

```qml
// View (QML)
Button {
    text: "Save"
    enabled: commandModel.canExecute("save")
    onClicked: commandModel.executeAction("save")
}

Connections {
    target: commandModel
    function onActionCompleted(action) {
        console.log("Action completed:", action)
    }
}
```

**When to use:** User actions, commands, operations

---

### Pattern 4: Singleton Model

**For global state:**

```cpp
// Model (C++)
class GlobalState : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme NOTIFY themeChanged)
    Q_PROPERTY(bool darkMode READ darkMode NOTIFY darkModeChanged)
    
public:
    static GlobalState* instance() {
        static GlobalState inst;
        return &inst;
    }
    
    QString theme() const { return m_theme; }
    bool darkMode() const { return m_darkMode; }
    
signals:
    void themeChanged();
    void darkModeChanged();
    
private:
    GlobalState() = default;
    QString m_theme = "default";
    bool m_darkMode = false;
};
```

```cpp
// Register as singleton in main()
qmlRegisterSingletonType<GlobalState>("MyApp", 1, 0, "GlobalState",
    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return GlobalState::instance();
    });
```

```qml
// View (QML)
import MyApp 1.0

Rectangle {
    color: GlobalState.darkMode ? "#2d2d2d" : "#ffffff"
}
```

**When to use:** Global settings, shared state, theme

---

### Pattern 5: View Manager

**For multi-window/multi-screen:**

```cpp
// Manager (C++)
class ViewManager : public QObject {
    Q_OBJECT
    
public:
    ViewManager(QObject *parent = nullptr) : QObject(parent) {
        // Watch for screen changes
        connect(qApp, &QGuiApplication::screenAdded,
                this, &ViewManager::onScreenAdded);
        connect(qApp, &QGuiApplication::screenRemoved,
                this, &ViewManager::onScreenRemoved);
    }
    
private slots:
    void onScreenAdded(QScreen *screen) {
        createViewForScreen(screen);
    }
    
    void onScreenRemoved(QScreen *screen) {
        removeViewForScreen(screen);
    }
    
private:
    void createViewForScreen(QScreen *screen) {
        QQuickView *view = new QQuickView();
        view->rootContext()->setContextProperty("model", m_model);
        view->setSource(QUrl("qrc:/View.qml"));
        
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setScope(screen->name());
        
        view->show();
        m_views.insert(screen->name(), view);
    }
    
    void removeViewForScreen(QScreen *screen) {
        QQuickView *view = m_views.take(screen->name());
        if (view) view->deleteLater();
    }
    
    QObject *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

**When to use:** Multi-monitor, screen hotplug, multiple windows

---

## QML Best Practices

### Practice 1: Use Layouts

**Use layouts for flexible UI:**

```qml
// ✓ GOOD: Use layouts
Rectangle {
    RowLayout {
        anchors.fill: parent
        spacing: 10
        
        Text { text: "Left" }
        Item { Layout.fillWidth: true }  // Spacer
        Text { text: "Right" }
    }
}
```

```qml
// ✗ BAD: Hardcoded positions
Rectangle {
    Text {
        x: 10
        y: 5
        text: "Left"
    }
    Text {
        x: 1850  // Hardcoded!
        y: 5
        text: "Right"
    }
}
```

---

### Practice 2: Use Components

**Extract reusable components:**

```qml
// ✓ GOOD: Reusable component
// Button.qml
Button {
    property string icon
    property string label
    
    contentItem: RowLayout {
        Text { text: icon; font.pixelSize: 20 }
        Text { text: label; color: "white" }
    }
    
    background: Rectangle {
        color: parent.hovered ? "#4d4d4d" : "#3d3d3d"
        radius: 5
    }
}

// Usage
MyButton { icon: "🔊"; label: "Volume" }
MyButton { icon: "📸"; label: "Screenshot" }
```

```qml
// ✗ BAD: Repeated code
Button {
    contentItem: RowLayout {
        Text { text: "🔊"; font.pixelSize: 20 }
        Text { text: "Volume"; color: "white" }
    }
    background: Rectangle { color: "#3d3d3d"; radius: 5 }
}
Button {
    contentItem: RowLayout {
        Text { text: "📸"; font.pixelSize: 20 }
        Text { text: "Screenshot"; color: "white" }
    }
    background: Rectangle { color: "#3d3d3d"; radius: 5 }
}
// Copy-paste hell!
```

---

### Practice 3: Use Property Aliases

**For better API:**

```qml
// ✓ GOOD: Clean API with aliases
// CustomPanel.qml
Rectangle {
    id: root
    property alias title: titleText.text
    property alias backgroundColor: root.color
    
    Text {
        id: titleText
        color: "white"
    }
}

// Usage
CustomPanel {
    title: "My Panel"  // Clean!
    backgroundColor: "#2d2d2d"
}
```

```qml
// ✗ BAD: Exposing internals
CustomPanel {
    titleText.text: "My Panel"  // Bad API!
    color: "#2d2d2d"
}
```

---

### Practice 4: Use States for UI Changes

**Instead of manual property changes:**

```qml
// ✓ GOOD: Use states
Rectangle {
    id: panel
    
    states: [
        State {
            name: "collapsed"
            PropertyChanges { target: panel; height: 35 }
            PropertyChanges { target: expandedContent; visible: false }
        },
        State {
            name: "expanded"
            PropertyChanges { target: panel; height: 200 }
            PropertyChanges { target: expandedContent; visible: true }
        }
    ]
    
    transitions: Transition {
        NumberAnimation { properties: "height"; duration: 300 }
    }
    
    Button {
        text: "Toggle"
        onClicked: panel.state = (panel.state === "collapsed" ? "expanded" : "collapsed")
    }
    
    Item {
        id: expandedContent
        // Extra content
    }
}
```

```qml
// ✗ BAD: Manual changes
Button {
    onClicked: {
        if (panel.height === 35) {
            panel.height = 200
            expandedContent.visible = true
        } else {
            panel.height = 35
            expandedContent.visible = false
        }
    }
}
```

---

### Practice 5: Avoid JavaScript Logic

**Keep logic in C++ model:**

```cpp
// ✓ GOOD: Logic in C++ model
class Calculator : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE int calculate(int a, int b, const QString &op) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") return b != 0 ? a / b : 0;
        return 0;
    }
};
```

```qml
// ✓ GOOD: QML just calls model
Button {
    text: "Calculate"
    onClicked: {
        var result = calculator.calculate(5, 3, "+")
        resultText.text = result
    }
}
```

```qml
// ✗ BAD: Business logic in QML
Button {
    text: "Calculate"
    onClicked: {
        var result
        if (operation === "+") {
            result = a + b
        } else if (operation === "-") {
            result = a - b
        }
        // ... complex logic in QML!
    }
}
```

---

## C++ Model Best Practices

### Practice 1: Use Q_PROPERTY

**For QML binding:**

```cpp
// ✓ GOOD: Q_PROPERTY for QML access
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    
public:
    QString text() const { return m_text; }
    int count() const { return m_count; }
    bool ready() const { return m_ready; }
    
signals:
    void textChanged();
    void countChanged();
    void readyChanged();
    
private:
    QString m_text;
    int m_count = 0;
    bool m_ready = false;
};
```

```qml
// Access from QML
Text { text: model.text }
Text { text: model.count }
Rectangle { visible: model.ready }
```

```cpp
// ✗ BAD: No Q_PROPERTY
class Model : public QObject {
public:
    QString getText() { return m_text; }  // Can't access from QML!
};
```

---

### Practice 2: Emit Signals on Change

**For automatic QML updates:**

```cpp
// ✓ GOOD: Emit signal when data changes
void Model::setText(const QString &text) {
    if (m_text != text) {
        m_text = text;
        emit textChanged();  // QML updates automatically!
    }
}
```

```cpp
// ✗ BAD: No signal
void Model::setText(const QString &text) {
    m_text = text;  // QML won't update!
}
```

---

### Practice 3: Use Q_INVOKABLE for Actions

**For methods callable from QML:**

```cpp
// ✓ GOOD: Q_INVOKABLE
class Model : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE void save() {
        // Save logic
        emit saved();
    }
    
    Q_INVOKABLE bool canSave() const {
        return m_hasChanges && m_isValid;
    }
    
signals:
    void saved();
};
```

```qml
// Call from QML
Button {
    text: "Save"
    enabled: model.canSave()
    onClicked: model.save()
}
```

---

### Practice 4: Thread Safety

**For background operations:**

```cpp
// ✓ GOOD: Use QThread properly
class DataLoader : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    
public:
    Q_INVOKABLE void loadData() {
        setStatus("Loading...");
        
        // Run in thread pool
        QtConcurrent::run([this]() {
            // Heavy operation
            QThread::sleep(2);
            
            // Update in main thread
            QMetaObject::invokeMethod(this, [this]() {
                setStatus("Loaded!");
                emit dataLoaded();
            }, Qt::QueuedConnection);
        });
    }
    
signals:
    void statusChanged();
    void dataLoaded();
};
```

```qml
// QML stays responsive
Button {
    text: "Load"
    onClicked: dataLoader.loadData()
}
Text {
    text: dataLoader.status  // Updates from background
}
```

---

### Practice 5: Resource Management

**Use smart pointers and proper ownership:**

```cpp
// ✓ GOOD: Clear ownership
class Manager : public QObject {
    Q_OBJECT
public:
    Manager() {
        m_model = new DataModel(this);  // QObject parent
    }
    
private:
    DataModel *m_model;  // Auto-deleted with parent
};

// Or with unique_ptr
class Manager : public QObject {
private:
    std::unique_ptr<DataModel> m_model;
};
```

```cpp
// ✗ BAD: Memory leak
class Manager {
public:
    Manager() {
        m_model = new DataModel();  // Who deletes this?
    }
private:
    DataModel *m_model;
};
```

---

## Performance Best Practices

### Practice 1: Minimize QML Updates

**Use `Binding` with `when` condition:**

```qml
// ✓ GOOD: Conditional binding
Rectangle {
    Binding {
        target: expensiveItem
        property: "value"
        value: model.complexCalculation()
        when: panel.visible  // Only update when visible
    }
}
```

```qml
// ✗ BAD: Always updating
Rectangle {
    property var value: model.complexCalculation()  // Expensive!
}
```

---

### Practice 2: Use Loaders for Heavy UI

**Lazy load complex components:**

```qml
// ✓ GOOD: Lazy loading
Rectangle {
    Loader {
        id: complexWidgetLoader
        active: false  // Don't create until needed
        source: "ComplexWidget.qml"
    }
    
    Button {
        text: "Show Widget"
        onClicked: complexWidgetLoader.active = true
    }
}
```

```qml
// ✗ BAD: Always created
Rectangle {
    ComplexWidget {
        visible: false  // Still created and consumes memory!
    }
}
```

---

### Practice 3: Optimize List Models

**Use beginInsertRows/endInsertRows:**

```cpp
// ✓ GOOD: Proper model updates
void ItemsModel::addItem(const Item &item) {
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(item);
    endInsertRows();  // QML updates efficiently
}
```

```cpp
// ✗ BAD: Reset entire model
void ItemsModel::addItem(const Item &item) {
    m_items.append(item);
    beginResetModel();  // Recreates entire view!
    endResetModel();
}
```

---

### Practice 4: Cache Expensive Calculations

**In C++ model:**

```cpp
// ✓ GOOD: Cache result
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString formattedText READ formattedText NOTIFY dataChanged)
    
public:
    QString formattedText() const {
        if (m_cachedText.isEmpty()) {
            m_cachedText = expensiveFormat(m_data);
        }
        return m_cachedText;
    }
    
    void setData(const QString &data) {
        m_data = data;
        m_cachedText.clear();  // Invalidate cache
        emit dataChanged();
    }
    
private:
    QString m_data;
    mutable QString m_cachedText;
};
```

---

### Practice 5: Profile with QML Profiler

**Identify bottlenecks:**

```bash
# Run with profiler
QML_PROFILER=1 ./myapp

# Or use Qt Creator's QML Profiler
```

**Focus on:**
- Binding loops
- Expensive expressions
- Large models
- Complex delegates

---

## Error Handling

### Pattern 1: Check LayerShell Availability

```cpp
// ✓ GOOD: Check and handle gracefully
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qWarning() << "Layer shell not available!";
        
        // Show error dialog in QML
        QQuickView errorView;
        errorView.setSource(QUrl("qrc:/Error.qml"));
        errorView.show();
        
        return app.exec();
    }
    
    // Continue with layer shell setup
}
```

```qml
// Error.qml
Rectangle {
    width: 400
    height: 200
    color: "#2d2d2d"
    
    ColumnLayout {
        anchors.centerIn: parent
        
        Text {
            text: "❌ Layer Shell Not Available"
            color: "#ff5555"
            font.pixelSize: 16
        }
        
        Text {
            text: "Please use KDE Plasma Wayland, Sway, or Hyprland"
            color: "white"
        }
        
        Button {
            text: "Exit"
            onClicked: Qt.quit()
        }
    }
}
```

---

### Pattern 2: Model Error States

```cpp
// ✓ GOOD: Error property in model
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorChanged)
    
public:
    QString errorMessage() const { return m_errorMessage; }
    bool hasError() const { return !m_errorMessage.isEmpty(); }
    
    Q_INVOKABLE void performAction() {
        clearError();
        
        if (!validate()) {
            setError("Invalid input!");
            return;
        }
        
        try {
            doAction();
        } catch (const std::exception &e) {
            setError(QString::fromStdString(e.what()));
        }
    }
    
signals:
    void errorChanged();
    
private:
    void setError(const QString &msg) {
        m_errorMessage = msg;
        emit errorChanged();
    }
    
    void clearError() {
        if (!m_errorMessage.isEmpty()) {
            m_errorMessage.clear();
            emit errorChanged();
        }
    }
    
    QString m_errorMessage;
};
```

```qml
// View shows error
Rectangle {
    Button {
        text: "Action"
        onClicked: model.performAction()
    }
    
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

### Pattern 3: Validation Before Actions

```cpp
// ✓ GOOD: Validate in model
class FormModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(bool canSubmit READ canSubmit NOTIFY canSubmitChanged)
    
public:
    bool canSubmit() const {
        return !m_input.isEmpty() && m_input.length() >= 3;
    }
    
    void setInput(const QString &input) {
        if (m_input != input) {
            m_input = input;
            emit inputChanged();
            emit canSubmitChanged();
        }
    }
    
signals:
    void inputChanged();
    void canSubmitChanged();
};
```

```qml
// View enforces validation
TextField {
    text: formModel.input
    onTextChanged: formModel.input = text
}

Button {
    text: "Submit"
    enabled: formModel.canSubmit  // Auto-disabled if invalid
    onClicked: formModel.submit()
}
```

---

## Testing Strategies

### Strategy 1: Unit Test Models

```cpp
// Test file: test_model.cpp
class ModelTest : public QObject {
    Q_OBJECT
    
private slots:
    void testPropertyChange() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        model.updateTime();
        
        QCOMPARE(spy.count(), 1);
        QVERIFY(!model.currentTime().isEmpty());
    }
    
    void testAction() {
        CommandModel model;
        QSignalSpy spy(&model, &CommandModel::actionCompleted);
        
        model.executeAction("save");
        
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toString(), "save");
    }
};

QTEST_MAIN(ModelTest)
#include "test_model.moc"
```

---

### Strategy 2: QML Test Cases

```qml
// test_view.qml
import QtTest 1.0

TestCase {
    name: "PanelViewTest"
    
    Rectangle {
        id: panel
        Text {
            id: timeText
            text: "12:34"
        }
    }
    
    function test_textVisible() {
        verify(timeText.visible)
        compare(timeText.text, "12:34")
    }
    
    function test_click() {
        var button = findChild(panel, "myButton")
        verify(button !== null)
        mouseClick(button)
        // Assert result
    }
}
```

---

### Strategy 3: Integration Tests

```cpp
// Test with LayerShellQt
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
        
        // Test LayerShell properties
        QCOMPARE(layerWindow->layer(), LayerShellQt::Window::LayerTop);
    }
};
```

---

## Common Pitfalls

### Pitfall 1: Forgetting Q_PROPERTY NOTIFY

```cpp
// ✗ BAD: No NOTIFY signal
Q_PROPERTY(QString text READ text)  // QML won't update!

// ✓ GOOD: With NOTIFY
Q_PROPERTY(QString text READ text NOTIFY textChanged)
```

**Result:** QML bindings won't update when data changes!

---

### Pitfall 2: Creating Models in QML

```qml
// ✗ BAD: Creating C++ object in QML
Rectangle {
    Component.onCompleted: {
        var model = Qt.createQmlObject(...)  // Don't!
    }
}
```

```cpp
// ✓ GOOD: Create in C++, expose to QML
PanelModel model;
view.rootContext()->setContextProperty("panelModel", &model);
```

---

### Pitfall 3: Binding Loops

```qml
// ✗ BAD: Binding loop
Rectangle {
    width: height  // Binds to height
    height: width  // Binds to width - LOOP!
}
```

**Error:** "QML Rectangle: Binding loop detected"

```qml
// ✓ GOOD: One-way binding
Rectangle {
    width: 100
    height: width  // OK
}
```

---

### Pitfall 4: Setting Properties Before Show

```cpp
// ✗ BAD: Properties after show
view.show();
layerWindow->setLayer(LayerTop);  // Too late!

// ✓ GOOD: Properties before show
layerWindow->setLayer(LayerTop);
view.show();
```

---

### Pitfall 5: Not Using Layouts

```qml
// ✗ BAD: Hardcoded positions
Rectangle {
    width: 1920  // What about other screen sizes?
    
    Text {
        x: 10
        y: 5
    }
    Text {
        x: 1850  // Breaks on small screens!
        y: 5
    }
}
```

```qml
// ✓ GOOD: Use layouts
Rectangle {
    RowLayout {
        anchors.fill: parent
        Text { text: "Left" }
        Item { Layout.fillWidth: true }
        Text { text: "Right" }
    }
}
```

---

## Production Checklist

### Before Deployment

- [ ] All models tested independently
- [ ] QML views tested with mock data
- [ ] LayerShell availability checked
- [ ] Properties set before window show
- [ ] All Q_PROPERTY have NOTIFY signals
- [ ] No binding loops in QML
- [ ] Layouts used instead of hardcoded sizes
- [ ] Error handling in place
- [ ] Performance profiled
- [ ] Memory leaks checked
- [ ] Multi-monitor tested
- [ ] Screen hotplug tested
- [ ] Compositor compatibility tested

---

## Architecture Checklist

### Model (C++)
- [ ] Business logic only
- [ ] Q_PROPERTY for QML access
- [ ] Q_INVOKABLE for actions
- [ ] Signals for changes
- [ ] No UI code
- [ ] Thread-safe if needed
- [ ] Proper resource management

### View (QML)
- [ ] UI only
- [ ] Property bindings used
- [ ] Layouts for flexibility
- [ ] Components extracted
- [ ] States for UI changes
- [ ] No business logic
- [ ] Responsive design

### Integration (main.cpp)
- [ ] LayerShell availability checked
- [ ] Models created
- [ ] Models exposed to QML
- [ ] LayerShell configured
- [ ] Properties set before show

---

## Summary: Golden Rules

### Architecture
1. **Separate Model from View**
2. **Logic in C++, UI in QML**
3. **Data binding over manual updates**
4. **Use Q_PROPERTY and signals**

### Performance
5. **Cache expensive calculations**
6. **Lazy load complex components**
7. **Profile before optimizing**
8. **Use proper model updates**

### Error Handling
9. **Check availability first**
10. **Validate before actions**
11. **Provide error feedback**
12. **Handle failures gracefully**

### Testing
13. **Unit test models**
14. **Test views separately**
15. **Integration tests**
16. **Test on real compositor**

---

## Resources

### Qt Documentation
- **QML Best Practices:** https://doc.qt.io/qt-5/qtquick-bestpractices.html
- **Model/View Programming:** https://doc.qt.io/qt-5/model-view-programming.html
- **QML Performance:** https://doc.qt.io/qt-5/qtquick-performance.html

### Tools
- **QML Profiler** - Qt Creator built-in
- **GammaRay** - Qt introspection tool
- **Valgrind** - Memory profiling

---

## Next Steps

Bây giờ bạn biết best practices! Apply chúng:
- **Refactor** existing code theo patterns
- **Test** models independently
- **Profile** performance
- **Document** your architecture

**Happy coding with clean architecture! 🎨**
