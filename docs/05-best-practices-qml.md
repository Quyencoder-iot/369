# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model-View Architecture](#model-view-architecture)
3. [QML Best Practices](#qml-best-practices)
4. [Coding Patterns](#coding-patterns)
5. [UI/UX Guidelines](#uiux-guidelines)
6. [Performance Best Practices](#performance-best-practices)
7. [Error Handling](#error-handling)
8. [Testing Strategies](#testing-strategies)
9. [Common Pitfalls](#common-pitfalls)

**Note:** This guide focuses on **Model-View architecture with QML** for LayerShellQt applications.

---

## Design Principles

### Principle 1: Separate Model from View

**Golden Rule:** Business logic trong C++ Model, UI trong QML View.

```cpp
// ✅ GOOD: Model (C++)
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
    QString time() const { return m_time; }
    void updateTime() { /* logic here */ }
signals:
    void timeChanged();
};
```

```qml
// ✅ GOOD: View (QML)
Rectangle {
    Text { text: panelModel.time }  // Just display
}
```

```cpp
// ✗ BAD: Mixed (don't do this)
class Panel : public QWidget {
    void updateUI() {
        // Logic + UI mixed
        label->setText(calculateTime());
        layout->addWidget(label);
    }
};
```

---

### Principle 2: Use Data Binding

**Don't manually update UI - use automatic binding!**

```qml
// ✅ GOOD: Automatic binding
Text {
    text: model.value  // Auto-updates when value changes
}
```

```qml
// ✗ BAD: Manual updates
Text {
    id: myText
    // Model emits signal → manually update
    Connections {
        target: model
        onValueChanged: myText.text = model.value  // Unnecessary!
    }
}
```

---

### Principle 3: Configure Layer Shell Before Show

**Always configure LayerShellQt properties before showing view!**

```cpp
// ✅ GOOD
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(...);
// Configure first

view.show();  // Then show
```

```cpp
// ✗ BAD
QQuickView view;
view.show();  // Showed first!

auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Too late!
```

---

### Principle 4: Choose Appropriate Layer

**Match layer to use case:**

| Use Case | Layer | Exclusive Zone |
|----------|-------|----------------|
| Panel | TOP | > 0 (reserve space) |
| Dock | TOP | > 0 |
| Notification | TOP | 0 (overlay) |
| Widget | BOTTOM | 0 |
| Wallpaper | BACKGROUND | 0 |
| Lock Screen | OVERLAY | -1 |

```cpp
// ✅ GOOD: Panel reserves space
layerWindow->setLayer(LayerTop);
layerWindow->setExclusiveZone(35);

// ✗ BAD: Using OVERLAY for normal panel (too aggressive)
layerWindow->setLayer(LayerOverlay);  // Don't!
```

---

## Model-View Architecture

### Core Pattern

```
┌──────────────┐
│ Model (C++)  │  ← Business logic, data, system integration
│  - Logic     │
│  - Data      │
└──────┬───────┘
       │ Q_PROPERTY, signals
       ↓
┌──────────────┐
│ View (QML)   │  ← UI presentation, user interaction
│  - UI        │
│  - Styling   │
└──────┬───────┘
       │
       ↓
┌──────────────┐
│ LayerShellQt │  ← Wayland layer shell integration
│  (C++ setup) │
└──────────────┘
```

---

### Pattern 1: Simple Property Model

**Use for:** Single values, simple data

```cpp
// Model
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    QString text() const { return m_text; }
    void setText(const QString &text) {
        if (m_text != text) {
            m_text = text;
            emit textChanged();
        }
    }
    
    int count() const { return m_count; }
    
signals:
    void textChanged();
    void countChanged();
    
private:
    QString m_text;
    int m_count = 0;
};
```

```qml
// View - Automatic binding
Rectangle {
    Text { text: model.text }  // Auto-updates
    Text { text: "Count: " + model.count }
}
```

**Key Points:**
- ✅ Use Q_PROPERTY for QML access
- ✅ Emit signals when values change
- ✅ Const getters, non-const setters
- ✅ Check for changes in setters

---

### Pattern 2: List Model

**Use for:** Lists, repeating elements

```cpp
// Model
class ListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        IconRole,
        StatusRole
    };
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
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
    
    Q_INVOKABLE void addItem(const QString &name) {
        beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
        m_items.append({name, "🔷", "Active"});
        endInsertRows();
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
// View - ListView or Repeater
ListView {
    model: listModel
    
    delegate: Rectangle {
        width: parent.width
        height: 40
        
        RowLayout {
            Text { text: model.icon }
            Text { text: model.name }
            Text { text: model.status }
        }
    }
}

// Or with Repeater
Repeater {
    model: listModel
    delegate: Item { /* ... */ }
}
```

**Key Points:**
- ✅ Inherit QAbstractListModel
- ✅ Define custom roles for data access
- ✅ Use beginInsertRows/endInsertRows
- ✅ Expose via roleNames()

---

### Pattern 3: Action Model

**Use for:** User actions, commands

```cpp
// Model with actions
class ActionModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    
public:
    bool busy() const { return m_busy; }
    
    Q_INVOKABLE void performAction() {
        m_busy = true;
        emit busyChanged();
        
        // Do work...
        QProcess::startDetached("command", {});
        
        QTimer::singleShot(1000, this, [this]() {
            m_busy = false;
            emit busyChanged();
            emit actionCompleted();
        });
    }
    
signals:
    void busyChanged();
    void actionCompleted();
    
private:
    bool m_busy = false;
};
```

```qml
// View - Call action from QML
Button {
    text: "Action"
    enabled: !model.busy
    onClicked: model.performAction()
}

BusyIndicator {
    running: model.busy
}

Connections {
    target: model
    onActionCompleted: {
        console.log("Done!")
    }
}
```

**Key Points:**
- ✅ Use Q_INVOKABLE for methods called from QML
- ✅ Provide feedback (busy state)
- ✅ Emit signals for completion/errors

---

### Pattern 4: Manager Pattern

**Use for:** Multi-window scenarios (multi-monitor panels)

```cpp
// Manager coordinates multiple views
class PanelManager : public QObject {
    Q_OBJECT
    
public:
    PanelManager(PanelModel *model) : m_model(model) {
        // Create view for each screen
        for (QScreen *screen : QGuiApplication::screens()) {
            createPanelForScreen(screen->name());
        }
        
        // Handle hotplug
        connect(qApp, &QGuiApplication::screenAdded,
                this, &PanelManager::onScreenAdded);
    }
    
private slots:
    void createPanelForScreen(const QString &screenName) {
        QQuickView *view = new QQuickView();
        
        // Share model
        view->rootContext()->setContextProperty("panelModel", m_model);
        view->rootContext()->setContextProperty("screenName", screenName);
        
        view->setSource(QUrl("qrc:/Panel.qml"));
        
        // Configure layer shell
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setScope(screenName);  // Per-screen
        
        view->show();
        m_views.insert(screenName, view);
    }
    
    void onScreenAdded(QScreen *screen) {
        createPanelForScreen(screen->name());
    }
    
private:
    PanelModel *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

**Key Points:**
- ✅ One manager for multiple views
- ✅ Share model across views
- ✅ Handle screen hotplug
- ✅ Clean up views properly

---

## QML Best Practices

### Practice 1: Component Organization

**Structure QML files properly:**

```qml
// ✅ GOOD: Well-organized
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root  // Root ID
    
    // Public properties
    property alias title: titleText.text
    
    // Private properties
    QtObject {
        id: internal
        property color hoverColor: "#4d4d4d"
    }
    
    // Visual children
    ColumnLayout {
        anchors.fill: parent
        
        Text {
            id: titleText
            text: "Title"
        }
        
        // Sub-components
        Loader {
            source: "SubComponent.qml"
        }
    }
    
    // Logic
    Connections {
        target: model
        onDataChanged: updateUI()
    }
    
    // Functions
    function updateUI() {
        // Implementation
    }
}
```

**Order:**
1. Imports
2. Root item with ID
3. Public properties
4. Private properties (QtObject)
5. Visual children
6. Connections/logic
7. Functions

---

### Practice 2: Use Layouts

**Don't use absolute positioning - use layouts!**

```qml
// ✅ GOOD: Responsive layout
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
        text: "Fixed position"  // Breaks on different sizes
    }
}
```

---

### Practice 3: Optimize Bindings

**Avoid expensive operations in bindings:**

```qml
// ✅ GOOD: Simple binding
Text {
    text: model.value
}

// ✗ BAD: Complex calculation in binding
Text {
    text: {
        // This runs on every frame!
        var result = 0;
        for (var i = 0; i < 1000; i++) {
            result += Math.sqrt(i);
        }
        return result;
    }
}

// ✅ BETTER: Calculate in model
Text {
    text: model.calculatedValue  // Pre-computed
}
```

---

### Practice 4: Use Loaders for Lazy Loading

**Don't load everything upfront:**

```qml
// ✅ GOOD: Lazy load
Loader {
    id: detailsLoader
    active: false
    source: "DetailView.qml"
}

Button {
    text: "Show Details"
    onClicked: detailsLoader.active = true
}
```

```qml
// ✗ BAD: Load everything
DetailView {
    visible: false  // Still loaded in memory!
}
```

---

### Practice 5: Proper Resource Management

```qml
// ✅ GOOD: Cleanup when destroyed
Item {
    Component.onCompleted: {
        timer.start()
    }
    
    Component.onDestruction: {
        timer.stop()
        // Cleanup
    }
    
    Timer {
        id: timer
        interval: 1000
        repeat: true
        onTriggered: { /* ... */ }
    }
}
```

---

## Coding Patterns

### Pattern 1: Builder Pattern for LayerShell Config

```cpp
// LayerShellBuilder.h
class LayerShellBuilder {
public:
    explicit LayerShellBuilder(QQuickView *view) : m_view(view) {
        m_window = LayerShellQt::Window::get(view);
    }
    
    LayerShellBuilder& layer(LayerShellQt::Window::Layer layer) {
        m_window->setLayer(layer);
        return *this;
    }
    
    LayerShellBuilder& anchors(LayerShellQt::Window::Anchors anchors) {
        m_window->setAnchors(anchors);
        return *this;
    }
    
    LayerShellBuilder& exclusiveZone(int zone) {
        m_window->setExclusiveZone(zone);
        return *this;
    }
    
    LayerShellBuilder& margins(const QMargins &margins) {
        m_window->setMargins(margins);
        return *this;
    }
    
    LayerShellBuilder& keyboard(LayerShellQt::Window::KeyboardInteractivity mode) {
        m_window->setKeyboardInteractivity(mode);
        return *this;
    }
    
    LayerShellBuilder& scope(const QString &scope) {
        m_window->setScope(scope);
        return *this;
    }
    
    void apply() { /* Optional finalize */ }
    
private:
    QQuickView *m_view;
    LayerShellQt::Window *m_window;
};
```

**Usage:**
```cpp
QQuickView view;
view.setSource(QUrl("qrc:/Panel.qml"));

LayerShellBuilder(&view)
    .layer(LayerShellQt::Window::LayerTop)
    .anchors(LayerShellQt::Window::AnchorTop | 
             LayerShellQt::Window::AnchorLeft | 
             LayerShellQt::Window::AnchorRight)
    .exclusiveZone(35)
    .keyboard(LayerShellQt::Window::KeyboardInteractivityExclusive)
    .apply();

view.show();
```

---

### Pattern 2: Factory Pattern for Views

```cpp
// ViewFactory.h
class ViewFactory {
public:
    static QQuickView* createPanel(QObject *model, const QString &scope = "") {
        QQuickView *view = new QQuickView();
        view->rootContext()->setContextProperty("panelModel", model);
        view->setSource(QUrl("qrc:/Panel.qml"));
        
        configureLayerShell(view, LayerTop, 35, scope);
        return view;
    }
    
    static QQuickView* createDock(QObject *model, const QString &scope = "") {
        QQuickView *view = new QQuickView();
        view->rootContext()->setContextProperty("dockModel", model);
        view->setSource(QUrl("qrc:/Dock.qml"));
        
        configureLayerShell(view, LayerTop, 60, scope);
        return view;
    }
    
private:
    static void configureLayerShell(QQuickView *view, 
                                    LayerShellQt::Window::Layer layer,
                                    int exclusiveZone,
                                    const QString &scope) {
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(layer);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(exclusiveZone);
        if (!scope.isEmpty()) {
            layerWindow->setScope(scope);
        }
    }
};
```

**Usage:**
```cpp
PanelModel model;
QQuickView *panel = ViewFactory::createPanel(&model, "HDMI-A-1");
panel->show();
```

---

### Pattern 3: Singleton Model

**For global state shared across views:**

```cpp
// GlobalState.h
class GlobalState : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme NOTIFY themeChanged)
    
public:
    static GlobalState* instance() {
        static GlobalState instance;
        return &instance;
    }
    
    QString currentTheme() const { return m_theme; }
    
    Q_INVOKABLE void setTheme(const QString &theme) {
        if (m_theme != theme) {
            m_theme = theme;
            emit themeChanged();
        }
    }
    
signals:
    void themeChanged();
    
private:
    GlobalState() = default;
    Q_DISABLE_COPY(GlobalState)
    
    QString m_theme = "dark";
};
```

**Register as singleton:**
```cpp
// main.cpp
qmlRegisterSingletonType<GlobalState>("MyApp", 1, 0, "GlobalState",
    [](QQmlEngine *, QJSEngine *) -> QObject* {
        return GlobalState::instance();
    });
```

**Use in QML:**
```qml
import MyApp 1.0

Rectangle {
    color: GlobalState.currentTheme === "dark" ? "#2d2d2d" : "white"
}

Button {
    onClicked: GlobalState.setTheme("light")
}
```

---

## UI/UX Guidelines

### Guideline 1: Respect User Space

**Don't be intrusive:**

```qml
// ✅ GOOD: Small notification, auto-hide
Rectangle {
    width: 300
    height: 80
    // Position: top-right corner
    // Auto-hide after 5s
}
```

```qml
// ✗ BAD: Huge notification, blocks screen
Rectangle {
    anchors.fill: parent  // Takes entire screen!
    // Never auto-hides
}
```

**Configure appropriately:**
```cpp
// Notification - no exclusive zone
layerWindow->setExclusiveZone(0);  // Apps can go under

// Panel - reserve space
layerWindow->setExclusiveZone(35);  // Apps avoid panel
```

---

### Guideline 2: Consistent Sizing

**Use standard panel sizes:**

```qml
// Standard panel heights
QtObject {
    readonly property int panelHeightSmall: 28
    readonly property int panelHeightMedium: 35
    readonly property int panelHeightLarge: 48
}

Rectangle {
    height: panelHeightMedium  // Consistent
}
```

---

### Guideline 3: Visual Feedback

**Always provide feedback for actions:**

```qml
Button {
    text: "Action"
    enabled: !model.busy
    
    contentItem: RowLayout {
        Text { text: parent.text }
        
        BusyIndicator {
            running: model.busy
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
        }
    }
    
    onClicked: model.performAction()
}
```

---

### Guideline 4: Animations

**Use smooth transitions:**

```qml
Rectangle {
    id: panel
    height: expanded ? 200 : 35
    
    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
}
```

**Don't overdo it:**
```qml
// ✗ BAD: Too many animations
Rectangle {
    color: "red"
    Behavior on color { ColorAnimation { duration: 5000 } }  // Too slow
    Behavior on x { SpringAnimation { } }  // Unnecessary
    Behavior on y { SpringAnimation { } }
    // Everything animated = nothing special
}
```

---

### Guideline 5: Accessibility

**Support high DPI:**

```qml
// ✅ GOOD: Use logical pixels
Text {
    font.pixelSize: 12  // Scales with DPI
}

Rectangle {
    width: 200  // Logical pixels
}
```

```qml
// ✗ BAD: Physical pixels
Text {
    font.pixelSize: 12 * Screen.devicePixelRatio  // Don't do this
}
```

---

## Performance Best Practices

### Practice 1: Minimize Model Updates

**Batch updates:**

```cpp
// ✅ GOOD: Batch updates
void updateData() {
    // Update all data first
    m_value1 = newValue1;
    m_value2 = newValue2;
    m_value3 = newValue3;
    
    // Emit once
    emit dataChanged();
}
```

```cpp
// ✗ BAD: Multiple updates
void updateData() {
    m_value1 = newValue1;
    emit value1Changed();  // UI updates
    
    m_value2 = newValue2;
    emit value2Changed();  // UI updates again
    
    m_value3 = newValue3;
    emit value3Changed();  // UI updates again
}
```

---

### Practice 2: Use Loaders for Complex UI

```qml
// ✅ GOOD: Load on demand
Loader {
    id: settingsLoader
    active: false
    asynchronous: true
    source: "SettingsView.qml"
}

Button {
    text: "Settings"
    onClicked: settingsLoader.active = true
}
```

---

### Practice 3: Optimize List Models

**Virtual scrolling with ListView:**

```qml
// ✅ GOOD: ListView virtualizes
ListView {
    model: 1000  // Only visible items rendered
    delegate: Rectangle { /* ... */ }
    cacheBuffer: 50  // Cache off-screen items
}
```

```qml
// ✗ BAD: Repeater loads all
Repeater {
    model: 1000  // All 1000 items rendered!
    delegate: Rectangle { /* ... */ }
}
```

---

### Practice 4: Async Image Loading

```qml
// ✅ GOOD: Async loading
Image {
    source: "large-image.png"
    asynchronous: true  // Load in background
    
    BusyIndicator {
        anchors.centerIn: parent
        running: parent.status === Image.Loading
    }
}
```

---

### Practice 5: Minimize QML<->C++ Calls

**Pre-compute in model:**

```cpp
// ✅ GOOD: Compute in model
class Model : public QObject {
    Q_PROPERTY(QString formattedValue READ formattedValue NOTIFY valueChanged)
    
    QString formattedValue() const {
        return QString("Value: %1 (%2%)").arg(m_value).arg(m_percent);
    }
};
```

```qml
Text { text: model.formattedValue }  // Pre-formatted
```

```qml
// ✗ BAD: Format in QML
Text {
    text: "Value: " + model.value + " (" + model.percent + "%)"
    // String concatenation on every frame
}
```

---

## Error Handling

### Pattern 1: Model Error States

```cpp
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    
public:
    bool hasError() const { return !m_errorMessage.isEmpty(); }
    QString errorMessage() const { return m_errorMessage; }
    
    Q_INVOKABLE void performAction() {
        clearError();
        
        if (!validate()) {
            setError("Invalid input");
            return;
        }
        
        // Do work...
    }
    
signals:
    void errorChanged();
    
private:
    void setError(const QString &error) {
        m_errorMessage = error;
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
// View - Show errors
Column {
    Button {
        text: "Action"
        onClicked: model.performAction()
    }
    
    Text {
        text: model.errorMessage
        color: "red"
        visible: model.hasError
    }
}
```

---

### Pattern 2: Graceful Fallback

```cpp
// Check layer shell support
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qWarning() << "Layer shell not available!";
        
        // Show error dialog
        QQuickView errorView;
        errorView.setSource(QUrl("qrc:/ErrorDialog.qml"));
        errorView.show();
        
        return app.exec();
    }
    
    // Normal flow...
}
```

```qml
// ErrorDialog.qml
Rectangle {
    width: 400
    height: 200
    
    ColumnLayout {
        anchors.centerIn: parent
        
        Text {
            text: "❌ Layer Shell Not Supported"
            font.pixelSize: 18
        }
        
        Text {
            text: "Please use a supported Wayland compositor:\n" +
                  "• KDE Plasma (Wayland)\n" +
                  "• Sway\n" +
                  "• Hyprland"
        }
    }
}
```

---

### Pattern 3: Try-Catch in Model

```cpp
Q_INVOKABLE void loadData() {
    try {
        QFile file("data.json");
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot open file");
        }
        
        // Process...
        emit dataLoaded();
        
    } catch (const std::exception &e) {
        setError(e.what());
    }
}
```

---

## Testing Strategies

### Strategy 1: Unit Test Models

**Test models independently from views:**

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
        
        model.updateTime();
        
        QCOMPARE(spy.count(), 1);
        QVERIFY(!model.currentTime().isEmpty());
    }
    
    void testProperties() {
        PanelModel model;
        
        QString time = model.currentTime();
        QVERIFY(!time.isEmpty());
        QVERIFY(time.contains(":"));
    }
};

QTEST_MAIN(TestPanelModel)
#include "test_model.moc"
```

---

### Strategy 2: QML Test

```qml
// tst_panel.qml
import QtTest 1.0
import QtQuick 2.15

TestCase {
    name: "PanelTests"
    
    Component {
        id: panelComponent
        
        Rectangle {
            property alias clockText: clock.text
            
            Text {
                id: clock
                text: "12:34:56"
            }
        }
    }
    
    function test_clock() {
        var panel = createTemporaryObject(panelComponent, testCase)
        compare(panel.clockText.length, 8)
        verify(panel.clockText.includes(":"))
    }
}
```

---

### Strategy 3: Integration Test

```cpp
// test_integration.cpp
void testPanelIntegration() {
    PanelModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    QVERIFY(view.rootObject() != nullptr);
    
    // Trigger model update
    model.updateTime();
    QTest::qWait(100);  // Wait for QML to update
    
    // Check QML state
    QObject *root = view.rootObject();
    QString displayedTime = root->property("clockText").toString();
    QVERIFY(!displayedTime.isEmpty());
}
```

---

### Strategy 4: Mock Models for UI Testing

```cpp
// MockModel.h
class MockPanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
public:
    QString time() const { return "12:34:56"; }  // Fixed value for testing
signals:
    void timeChanged();
};
```

```qml
// Test with mock
TestCase {
    MockPanelModel { id: mockModel }
    
    Panel {
        panelModel: mockModel
    }
    
    function test_display() {
        compare(panel.displayedTime, "12:34:56")
    }
}
```

---

## Common Pitfalls

### Pitfall 1: Configuring After Show

```cpp
// ✗ WRONG
QQuickView view;
view.show();  // Already shown!
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Too late

// ✅ CORRECT
QQuickView view;
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Configure first
view.show();  // Then show
```

---

### Pitfall 2: Not Using Data Binding

```qml
// ✗ BAD: Manual updates
Text {
    id: myText
}

Connections {
    target: model
    onValueChanged: {
        myText.text = model.value  // Manual!
    }
}

// ✅ GOOD: Automatic binding
Text {
    text: model.value  // Auto-updates!
}
```

---

### Pitfall 3: Heavy Computation in QML

```qml
// ✗ BAD: Calculate in QML binding
Text {
    text: {
        var sum = 0
        for (var i = 0; i < model.items.length; i++) {
            sum += model.items[i].value
        }
        return sum
    }  // Recalculates on every frame!
}

// ✅ GOOD: Pre-calculate in model
Text {
    text: model.totalValue  // Computed in C++
}
```

---

### Pitfall 4: Memory Leaks in Manager

```cpp
// ✗ BAD: Never delete views
void createView() {
    QQuickView *view = new QQuickView();
    // Never stored, never deleted
    view->show();
}

// ✅ GOOD: Track and delete
class Manager {
    QHash<QString, QQuickView*> m_views;
    
    void createView(const QString &id) {
        QQuickView *view = new QQuickView();
        m_views.insert(id, view);
        view->show();
    }
    
    void removeView(const QString &id) {
        QQuickView *view = m_views.take(id);
        if (view) view->deleteLater();
    }
    
    ~Manager() {
        qDeleteAll(m_views);
    }
};
```

---

### Pitfall 5: Wrong Layer for Use Case

```cpp
// ✗ BAD: Panel as OVERLAY (too aggressive)
layerWindow->setLayer(LayerOverlay);  // Blocks everything!

// ✅ GOOD: Panel as TOP
layerWindow->setLayer(LayerTop);  // Appropriate
```

---

### Pitfall 6: Forgetting NOTIFY Signal

```cpp
// ✗ BAD: No NOTIFY signal
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value READ value)  // No NOTIFY!
    // QML won't update when value changes
};

// ✅ GOOD: With NOTIFY
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value READ value NOTIFY valueChanged)
signals:
    void valueChanged();
};
```

---

### Pitfall 7: Not Checking Layer Shell Availability

```cpp
// ✗ BAD: Assume it's available
int main() {
    // No check!
    QQuickView view;
    auto layerWindow = LayerShellQt::Window::get(&view);
    // Might crash on unsupported compositor
}

// ✅ GOOD: Check first
int main() {
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    // Proceed safely
}
```

---

## Checklist: Production Ready

### Model
- [ ] Q_PROPERTY for all QML-accessible properties
- [ ] NOTIFY signals for all properties
- [ ] Q_INVOKABLE for methods called from QML
- [ ] Error handling with error states
- [ ] Unit tests for business logic
- [ ] No UI code in model

### View (QML)
- [ ] Use data binding (not manual updates)
- [ ] Proper layout (not absolute positioning)
- [ ] Responsive to different sizes
- [ ] Visual feedback for actions
- [ ] Loading indicators for async operations
- [ ] Error messages displayed to user

### LayerShell Configuration
- [ ] Check isAvailable() before use
- [ ] Configure before show()
- [ ] Appropriate layer for use case
- [ ] Correct exclusive zone
- [ ] Proper keyboard interactivity
- [ ] Handle multi-monitor if needed

### Performance
- [ ] No heavy computation in QML bindings
- [ ] Use Loader for complex/optional UI
- [ ] Async loading for images
- [ ] Batch model updates
- [ ] ListView for long lists (not Repeater)

### Error Handling
- [ ] Check layer shell availability
- [ ] Model error states
- [ ] Graceful fallbacks
- [ ] User-friendly error messages

### Testing
- [ ] Unit tests for models
- [ ] QML tests for UI components
- [ ] Integration tests
- [ ] Tested on real compositor

---

## Summary: QML + Model-View Best Practices

### Golden Rules

1. **Separate concerns**: Model (C++) = logic, View (QML) = UI
2. **Use data binding**: Let Qt handle updates automatically
3. **Configure before show**: Set LayerShellQt properties first
4. **Choose appropriate layer**: Match to use case
5. **Pre-compute in model**: Keep QML simple
6. **Handle errors**: Check availability, provide feedback
7. **Test independently**: Unit test models, QML test views
8. **Optimize smartly**: Use Loaders, ListView, async loading

### Architecture Summary

```
Clean Architecture:
  Model (C++) → Business logic, data, system integration
     ↓ Q_PROPERTY, signals, Q_INVOKABLE
  View (QML) → UI presentation, user interaction
     ↓ Automatic binding
  LayerShellQt (C++) → Wayland integration
```

---

## Next Steps

After mastering these best practices:
1. Build real projects with clean architecture
2. Contribute to KDE/Plasma
3. Share your patterns with community
4. Write tests for everything
5. Optimize based on profiling (not guessing)

**Happy coding with QML + Model-View! 🎨**

---

*Updated for QML + Model-View architecture*
*December 2024*
