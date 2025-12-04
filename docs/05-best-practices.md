# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model Patterns](#model-patterns)
3. [QML View Patterns](#qml-view-patterns)
4. [UI/UX Guidelines](#uiux-guidelines)
5. [Performance Best Practices](#performance-best-practices)
6. [Error Handling](#error-handling)
7. [Testing Strategies](#testing-strategies)
8. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Separate Model from View

**Always separate business logic (C++) from UI (QML)!**

✅ **GOOD:**
```cpp
// Model: PanelModel.h - Business logic
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    
public:
    QString time() const { return m_time; }
    
signals:
    void timeChanged();
    
private:
    void updateTime(); // Logic here
    QString m_time;
};
```

```qml
// View: Panel.qml - UI only
Rectangle {
    Text {
        text: panelModel.time  // Data binding
    }
}
```

✗ **BAD:**
```qml
// DON'T put logic in QML
Rectangle {
    property var timer: Timer {
        onTriggered: {
            // Complex calculations here - WRONG!
            var result = complexCalculation();
        }
    }
}
```

**Why:**
- Models testable independently
- QML focused on presentation
- Better performance (C++ faster than QML)
- Easier to maintain

---

### Principle 2: Use Appropriate Model Type

Choose the right model base class:

#### QObject (Simple Properties)

```cpp
// For simple data with properties
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(int value READ value NOTIFY valueChanged)
};
```

**Use when:**
- Simple properties (strings, numbers, bools)
- No list of items
- Few properties

#### QAbstractListModel (Lists)

```cpp
// For lists of items
class ListModel : public QAbstractListModel {
    Q_OBJECT
    
    enum Roles {
        NameRole = Qt::UserRole + 1,
        IconRole
    };
    
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};
```

**Use when:**
- List of items (apps, screens, notifications)
- Used with Repeater, ListView
- Dynamic add/remove items

---

### Principle 3: Configure LayerShell Before Show

**CRITICAL:** Always configure LayerShellQt **before** `view.show()`!

✅ **GOOD:**
```cpp
QQuickView view;

// 1. Setup context
view.rootContext()->setContextProperty("model", &model);

// 2. Load QML
view.setSource(QUrl("qrc:/View.qml"));

// 3. Configure LayerShell
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(/* ... */);

// 4. Show
view.show();
```

✗ **BAD:**
```cpp
view.show();  // Too early!
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Too late!
```

---

### Principle 4: Use Appropriate Layer

| Use Case | Layer | Exclusive Zone |
|----------|-------|----------------|
| Panel/Taskbar | TOP | > 0 (reserve space) |
| Dock | TOP | > 0 |
| Notification | TOP | 0 (overlay) |
| Widget | BOTTOM | 0 |
| Wallpaper | BACKGROUND | 0 |
| Lock screen | OVERLAY | -1 (auto) |

✅ **GOOD:**
```cpp
// Panel reserves space
layerWindow->setLayer(LayerTop);
layerWindow->setExclusiveZone(35);
```

✗ **BAD:**
```cpp
// Notification shouldn't use OVERLAY
layerWindow->setLayer(LayerOverlay);  // Too aggressive!
```

---

## Model Patterns

### Pattern 1: Base Model Class

Create reusable base with common functionality:

```cpp
// BaseModel.h
class BaseModel : public QObject {
    Q_OBJECT
    
public:
    explicit BaseModel(QObject *parent = nullptr) : QObject(parent) {
        if (!LayerShellQt::Shell::isAvailable()) {
            qWarning() << "Layer shell not available!";
        }
    }
    
protected:
    // Helper for creating layer windows
    void configureLayerWindow(QQuickView *view,
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
class PanelModel : public BaseModel {
    Q_OBJECT
    // Your specific logic
};
```

---

### Pattern 2: Property Model with Auto-Update

```cpp
// AutoUpdateModel.h
class AutoUpdateModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value READ value NOTIFY valueChanged)
    
public:
    AutoUpdateModel(int intervalMs = 1000, QObject *parent = nullptr)
        : QObject(parent) {
        
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &AutoUpdateModel::update);
        timer->start(intervalMs);
        update();
    }
    
    QString value() const { return m_value; }
    
signals:
    void valueChanged();
    
protected:
    virtual void update() = 0;  // Override in subclass
    
    void setValue(const QString &value) {
        if (m_value != value) {
            m_value = value;
            emit valueChanged();
        }
    }
    
private:
    QString m_value;
};
```

**Usage:**
```cpp
class ClockModel : public AutoUpdateModel {
    Q_OBJECT
    
protected:
    void update() override {
        setValue(QTime::currentTime().toString("hh:mm:ss"));
    }
};
```

---

### Pattern 3: List Model Template

```cpp
// ListModelTemplate.h
template<typename T>
class ListModelTemplate : public QAbstractListModel {
public:
    void append(const T &item) {
        beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
        m_items.append(item);
        endInsertRows();
        emit countChanged();
    }
    
    void remove(int index) {
        if (index >= 0 && index < m_items.count()) {
            beginRemoveRows(QModelIndex(), index, index);
            m_items.removeAt(index);
            endRemoveRows();
            emit countChanged();
        }
    }
    
    void clear() {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged();
    }
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_items.count();
    }
    
    int count() const { return m_items.count(); }
    
signals:
    void countChanged();
    
protected:
    QList<T> m_items;
};
```

**Usage:**
```cpp
struct App {
    QString name;
    QString icon;
    QString command;
};

class AppListModel : public ListModelTemplate<App> {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
    enum Roles {
        NameRole = Qt::UserRole + 1,
        IconRole,
        CommandRole
    };
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_items.count())
            return QVariant();
        
        const App &app = m_items.at(index.row());
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
};
```

---

### Pattern 4: Singleton Model

```cpp
// SingletonModel.h
class SingletonModel : public QObject {
    Q_OBJECT
    
public:
    static SingletonModel* instance() {
        static SingletonModel instance;
        return &instance;
    }
    
    // Prevent copying
    SingletonModel(const SingletonModel&) = delete;
    SingletonModel& operator=(const SingletonModel&) = delete;
    
private:
    SingletonModel() = default;
};
```

**Register as QML singleton:**
```cpp
// main.cpp
qmlRegisterSingletonType<SingletonModel>(
    "MyApp", 1, 0, "GlobalModel",
    [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return SingletonModel::instance();
    }
);
```

**Use in QML:**
```qml
import MyApp 1.0

Rectangle {
    Text { text: GlobalModel.value }
}
```

---

### Pattern 5: View Manager for Multi-Window

```cpp
// ViewManager.h
class ViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit ViewManager(QObject *parent = nullptr) : QObject(parent) {}
    
    QQuickView* createView(const QString &qmlFile, 
                          QObject *model,
                          const QString &modelName) {
        QQuickView *view = new QQuickView();
        view->rootContext()->setContextProperty(modelName, model);
        view->setSource(QUrl(qmlFile));
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        m_views.append(view);
        return view;
    }
    
    void destroyView(QQuickView *view) {
        m_views.removeOne(view);
        view->deleteLater();
    }
    
    void destroyAll() {
        qDeleteAll(m_views);
        m_views.clear();
    }
    
private:
    QList<QQuickView*> m_views;
};
```

**Usage:**
```cpp
ViewManager manager;

// Create multiple views
auto view1 = manager.createView("qrc:/Panel.qml", &panelModel, "panelModel");
auto view2 = manager.createView("qrc:/Dock.qml", &dockModel, "dockModel");

// Configure LayerShell for each
configureLayerShell(view1, LayerTop);
configureLayerShell(view2, LayerTop);

view1->show();
view2->show();
```

---

## QML View Patterns

### Pattern 1: Component-Based Architecture

Break UI into reusable components:

**Components/IconButton.qml:**
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root
    
    property string iconText: "⚙️"
    property color iconColor: "white"
    
    contentItem: Text {
        text: root.iconText
        color: root.iconColor
        font.pixelSize: 20
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    
    background: Rectangle {
        color: root.hovered ? "#4d4d4d" : "#3d3d3d"
        radius: 5
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
```

**Main view:**
```qml
import QtQuick 2.15
import "./Components"

Rectangle {
    IconButton {
        iconText: "🔊"
        onClicked: model.volumeAction()
    }
    
    IconButton {
        iconText: "📸"
        onClicked: model.screenshotAction()
    }
}
```

---

### Pattern 2: Data Binding Best Practices

✅ **GOOD - Direct binding:**
```qml
Rectangle {
    color: model.isActive ? "#4fc3f7" : "#2d2d2d"
    
    Text {
        text: model.title
        visible: model.showTitle
    }
}
```

✗ **BAD - Manual updates:**
```qml
Rectangle {
    id: root
    
    Connections {
        target: model
        onIsActiveChanged: {
            // DON'T do this!
            root.color = model.isActive ? "#4fc3f7" : "#2d2d2d"
        }
    }
}
```

**Why:** Direct binding is cleaner, automatic, and optimized.

---

### Pattern 3: State Management

Use QML States for UI states:

```qml
Rectangle {
    id: panel
    
    state: panelModel.expanded ? "expanded" : "collapsed"
    
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
    
    transitions: [
        Transition {
            NumberAnimation { 
                properties: "height" 
                duration: 300 
                easing.type: Easing.OutCubic
            }
        }
    ]
    
    Rectangle {
        id: expandedContent
        // ...
    }
}
```

---

### Pattern 4: Loader for Dynamic Content

Use Loader for lazy loading:

```qml
Rectangle {
    Loader {
        id: settingsLoader
        active: false  // Don't load until needed
        source: "qrc:/Settings.qml"
    }
    
    Button {
        text: "Settings"
        onClicked: {
            settingsLoader.active = true
        }
    }
}
```

---

### Pattern 5: Reusable Delegates

```qml
// AppDelegate.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    required property string name
    required property string icon
    required property string command
    
    contentItem: Column {
        spacing: 5
        
        Text {
            text: icon
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        Text {
            text: name
            color: "white"
            font.pixelSize: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
    
    background: Rectangle {
        color: parent.hovered ? "#4d4d4d" : "#3d3d3d"
        radius: 5
    }
    
    onClicked: model.launchApp(command)
}
```

**Usage:**
```qml
Repeater {
    model: appListModel
    
    delegate: AppDelegate {
        name: model.name
        icon: model.icon
        command: model.command
    }
}
```

---

## UI/UX Guidelines

### Guideline 1: Consistent Sizing

Define constants for consistency:

```qml
// Theme.qml - Singleton
pragma Singleton
import QtQuick 2.15

QtObject {
    // Panel sizes
    readonly property int panelHeightSmall: 28
    readonly property int panelHeightMedium: 35
    readonly property int panelHeightLarge: 48
    
    // Colors
    readonly property color backgroundDark: "#1e1e1e"
    readonly property color backgroundMid: "#2d2d2d"
    readonly property color backgroundLight: "#3d3d3d"
    readonly property color accentColor: "#4fc3f7"
    
    // Spacing
    readonly property int spacingSmall: 5
    readonly property int spacingMedium: 10
    readonly property int spacingLarge: 15
    
    // Animations
    readonly property int animationFast: 150
    readonly property int animationMedium: 300
    readonly property int animationSlow: 500
}
```

**Usage:**
```qml
import "."

Rectangle {
    height: Theme.panelHeightMedium
    color: Theme.backgroundMid
    
    RowLayout {
        spacing: Theme.spacingMedium
    }
}
```

---

### Guideline 2: Responsive Design

Adapt to screen size:

```qml
Rectangle {
    id: panel
    
    // Responsive height
    height: {
        if (Screen.height <= 768) return 28
        if (Screen.height <= 1080) return 35
        return 48
    }
    
    Text {
        // Responsive font
        font.pixelSize: panel.height * 0.4
    }
}
```

---

### Guideline 3: Accessibility

```qml
Button {
    text: "Volume"
    
    // Accessible properties
    Accessible.name: "Volume Control"
    Accessible.description: "Adjust system volume"
    Accessible.role: Accessible.Button
    
    // Keyboard support
    Keys.onReturnPressed: clicked()
    Keys.onEnterPressed: clicked()
}
```

---

### Guideline 4: Visual Feedback

Always provide feedback:

```qml
Button {
    id: btn
    
    background: Rectangle {
        color: {
            if (btn.pressed) return "#5d5d5d"
            if (btn.hovered) return "#4d4d4d"
            return "#3d3d3d"
        }
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
        
        // Ripple effect on click
        Rectangle {
            id: ripple
            anchors.centerIn: parent
            width: 0
            height: width
            radius: width / 2
            color: Qt.rgba(1, 1, 1, 0.3)
            
            states: State {
                name: "active"
                when: btn.pressed
                PropertyChanges { target: ripple; width: parent.width * 2 }
            }
            
            transitions: Transition {
                NumberAnimation { 
                    property: "width" 
                    duration: 300 
                    easing.type: Easing.OutCubic
                }
            }
        }
    }
}
```

---

## Performance Best Practices

### Practice 1: Minimize Property Bindings

✅ **GOOD:**
```qml
Text {
    text: model.value  // Simple binding - OK
}
```

✗ **BAD:**
```qml
Text {
    // Complex binding recalculates too often
    text: {
        var result = "";
        for (var i = 0; i < model.count; i++) {
            result += model.getData(i) + " ";
        }
        return result;
    }
}
```

**Fix:** Do complex calculations in C++ model:
```cpp
class Model : public QObject {
    Q_PROPERTY(QString formattedData READ formattedData NOTIFY dataChanged)
    
    QString formattedData() const {
        QString result;
        // Calculate once in C++
        for (const auto &item : m_data) {
            result += item + " ";
        }
        return result;
    }
};
```

```qml
Text {
    text: model.formattedData  // Simple binding
}
```

---

### Practice 2: Use Loaders for Heavy Content

```qml
Rectangle {
    Loader {
        id: heavyLoader
        active: false
        asynchronous: true  // Load in background
        source: "qrc:/HeavyComponent.qml"
        
        onLoaded: console.log("Heavy component loaded")
    }
    
    Button {
        text: "Show Heavy Content"
        onClicked: heavyLoader.active = true
    }
}
```

---

### Practice 3: Optimize Repeater/ListView

✅ **GOOD - Use delegate caching:**
```qml
ListView {
    model: largeModel
    
    // Cache delegates for reuse
    cacheBuffer: 200
    
    delegate: Rectangle {
        // Keep delegate simple
        height: 40
        
        Text {
            text: model.name
        }
    }
}
```

✗ **BAD - Heavy delegates:**
```qml
Repeater {
    model: largeModel
    
    delegate: Rectangle {
        // Complex delegate - creates lag
        Image { source: model.icon }  // Loads image each time
        ShaderEffect { /* ... */ }     // Expensive
        ParticleSystem { /* ... */ }   // Very expensive
    }
}
```

---

### Practice 4: Debounce Expensive Operations

```qml
Rectangle {
    property string searchText: ""
    
    Timer {
        id: searchDebounce
        interval: 300
        onTriggered: model.search(searchText)
    }
    
    TextField {
        onTextChanged: {
            searchText = text
            searchDebounce.restart()  // Debounce search
        }
    }
}
```

---

### Practice 5: Use Image Providers for Dynamic Images

```cpp
// C++ Image Provider
class IconProvider : public QQuickImageProvider {
public:
    IconProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}
    
    QPixmap requestPixmap(const QString &id, QSize *size, 
                         const QSize &requestedSize) override {
        // Generate/cache icons
        return generateIcon(id, requestedSize);
    }
};

// Register
engine.addImageProvider("icons", new IconProvider());
```

```qml
// Use in QML
Image {
    source: "image://icons/volume"  // Fast!
}
```

---

## Error Handling

### Pattern 1: Model Error Handling

```cpp
class RobustModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    
public:
    bool hasError() const { return m_hasError; }
    QString errorMessage() const { return m_errorMessage; }
    
    Q_INVOKABLE void performAction() {
        clearError();
        
        try {
            // Risky operation
            doSomethingRisky();
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
        if (m_hasError) {
            m_hasError = false;
            m_errorMessage.clear();
            emit errorChanged();
        }
    }
    
    bool m_hasError = false;
    QString m_errorMessage;
};
```

**QML Error Display:**
```qml
Rectangle {
    Column {
        spacing: 10
        
        Button {
            text: "Do Action"
            onClicked: model.performAction()
        }
        
        // Error banner
        Rectangle {
            visible: model.hasError
            color: "#ff5555"
            height: 40
            
            Text {
                anchors.centerIn: parent
                text: model.errorMessage
                color: "white"
            }
        }
    }
}
```

---

### Pattern 2: Graceful Fallback

```cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    // Check layer shell availability
    if (!LayerShellQt::Shell::isAvailable()) {
        qWarning() << "Layer shell not available, using fallback";
        
        // Fallback: Create normal window
        QQuickView view;
        view.setSource(QUrl("qrc:/FallbackView.qml"));
        view.setFlags(Qt::Tool | Qt::FramelessWindowHint | 
                     Qt::WindowStaysOnTopHint);
        view.show();
        
        return app.exec();
    }
    
    // Normal layer shell path
    // ...
}
```

---

### Pattern 3: Input Validation

```cpp
class ValidatedModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(QString validationError READ validationError NOTIFY isValidChanged)
    
public:
    void setPassword(const QString &password) {
        m_password = password;
        validate();
        emit passwordChanged();
    }
    
    bool isValid() const { return m_isValid; }
    QString validationError() const { return m_validationError; }
    
signals:
    void passwordChanged();
    void isValidChanged();
    
private:
    void validate() {
        if (m_password.length() < 6) {
            m_isValid = false;
            m_validationError = "Password must be at least 6 characters";
        } else if (!m_password.contains(QRegExp("[0-9]"))) {
            m_isValid = false;
            m_validationError = "Password must contain a number";
        } else {
            m_isValid = true;
            m_validationError.clear();
        }
        emit isValidChanged();
    }
    
    QString m_password;
    bool m_isValid = false;
    QString m_validationError;
};
```

**QML:**
```qml
Column {
    TextField {
        id: passwordField
        echoMode: TextInput.Password
        onTextChanged: model.password = text
        
        background: Rectangle {
            border.color: model.isValid ? "green" : "red"
        }
    }
    
    Text {
        text: model.validationError
        color: "red"
        visible: !model.isValid
    }
    
    Button {
        text: "Submit"
        enabled: model.isValid
    }
}
```

---

## Testing Strategies

### Strategy 1: Unit Test Models

```cpp
// test_model.cpp
#include <QtTest/QtTest>
#include "PanelModel.h"

class TestPanelModel : public QObject {
    Q_OBJECT
    
private slots:
    void testTimeUpdate() {
        PanelModel model;
        
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        // Wait for update
        QVERIFY(spy.wait(2000));
        
        // Check time format
        QString time = model.currentTime();
        QVERIFY(QTime::fromString(time, "hh:mm:ss").isValid());
    }
    
    void testPropertyBinding() {
        PanelModel model;
        
        // Test property read
        QString time = model.currentTime();
        QVERIFY(!time.isEmpty());
    }
};

QTEST_MAIN(TestPanelModel)
#include "test_model.moc"
```

---

### Strategy 2: Test QML with QQuickTest

```cpp
// test_qml.cpp
#include <QtQuickTest/quicktest.h>

QUICK_TEST_MAIN(qml_tests)
```

```qml
// tst_Panel.qml
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
    
    function test_panelSize() {
        var panel = createTemporaryObject(panelComponent, testCase)
        compare(panel.height, 35)
    }
    
    function test_clockUpdates() {
        var panel = createTemporaryObject(panelComponent, testCase)
        var initialTime = panel.clockText
        wait(1100)  // Wait > 1 second
        verify(panel.clockText !== initialTime)
    }
}
```

---

### Strategy 3: Mock Models for Testing

```cpp
// MockModel.h
class MockPanelModel : public PanelModel {
    Q_OBJECT
    
public:
    void setMockTime(const QString &time) {
        m_mockTime = time;
        emit timeChanged();
    }
    
    QString currentTime() const override {
        return m_mockTime.isEmpty() ? PanelModel::currentTime() : m_mockTime;
    }
    
private:
    QString m_mockTime;
};
```

**Test with mock:**
```cpp
MockPanelModel mockModel;
mockModel.setMockTime("12:34:56");

QQuickView view;
view.rootContext()->setContextProperty("panelModel", &mockModel);
// Test UI with known data
```

---

### Strategy 4: Integration Tests

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
        
        // Check layer configuration
        QCOMPARE(layerWindow->layer(), LayerShellQt::Window::LayerTop);
    }
};
```

---

## Common Pitfalls

### Pitfall 1: Setting Properties After Show

✗ **WRONG:**
```cpp
view.show();
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Too late!
```

✅ **CORRECT:**
```cpp
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);
view.show();  // Configure first, then show
```

---

### Pitfall 2: Complex Logic in QML

✗ **WRONG:**
```qml
Rectangle {
    property var data: []
    
    Text {
        text: {
            // Heavy calculation in QML - BAD!
            var sum = 0;
            for (var i = 0; i < data.length; i++) {
                sum += complexCalculation(data[i]);
            }
            return sum;
        }
    }
}
```

✅ **CORRECT:**
```cpp
// Do it in C++ model
class Model : public QObject {
    Q_PROPERTY(int sum READ sum NOTIFY sumChanged)
    
    int sum() const {
        int result = 0;
        for (const auto &item : m_data) {
            result += complexCalculation(item);
        }
        return result;
    }
};
```

```qml
Text {
    text: model.sum  // Simple binding
}
```

---

### Pitfall 3: Missing NOTIFY Signals

✗ **WRONG:**
```cpp
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text)  // Missing NOTIFY!
    
    QString text() const { return m_text; }
    void setText(const QString &text) {
        m_text = text;
        // No emit! QML won't update!
    }
};
```

✅ **CORRECT:**
```cpp
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    
signals:
    void textChanged();
    
private:
    void setText(const QString &text) {
        if (m_text != text) {
            m_text = text;
            emit textChanged();  // QML updates automatically
        }
    }
};
```

---

### Pitfall 4: Not Using roleNames()

✗ **WRONG:**
```cpp
class ListModel : public QAbstractListModel {
    // Missing roleNames()!
    QVariant data(const QModelIndex &index, int role) const override {
        // ...
    }
};
```

```qml
Repeater {
    model: listModel
    delegate: Text {
        text: ???  // Can't access data!
    }
}
```

✅ **CORRECT:**
```cpp
class ListModel : public QAbstractListModel {
    enum Roles {
        NameRole = Qt::UserRole + 1
    };
    
    QHash<int, QByteArray> roleNames() const override {
        return {{NameRole, "name"}};
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (role == NameRole) return m_items[index.row()].name;
        return QVariant();
    }
};
```

```qml
Repeater {
    model: listModel
    delegate: Text {
        text: model.name  // Works!
    }
}
```

---

### Pitfall 5: Memory Leaks with QML Objects

✗ **WRONG:**
```cpp
void createView() {
    QQuickView *view = new QQuickView();  // Never deleted!
    view->show();
}
```

✅ **CORRECT:**
```cpp
class Manager : public QObject {
    QList<QQuickView*> m_views;
    
public:
    ~Manager() {
        qDeleteAll(m_views);  // Cleanup
    }
    
    void createView() {
        QQuickView *view = new QQuickView();
        m_views.append(view);
        view->show();
    }
};
```

---

### Pitfall 6: Blocking Main Thread

✗ **WRONG:**
```cpp
Q_INVOKABLE void loadData() {
    // Blocking operation in main thread!
    QThread::sleep(5);  // UI freezes!
    m_data = loadHugeFile();
    emit dataChanged();
}
```

✅ **CORRECT:**
```cpp
Q_INVOKABLE void loadData() {
    // Use QFuture
    auto future = QtConcurrent::run([this]() {
        return loadHugeFile();
    });
    
    auto watcher = new QFutureWatcher<Data>(this);
    connect(watcher, &QFutureWatcher<Data>::finished, [this, watcher]() {
        m_data = watcher->result();
        emit dataChanged();
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}
```

---

## Checklist: Production Ready

### Model Checklist
- [ ] All properties have NOTIFY signals
- [ ] Q_INVOKABLE methods for QML actions
- [ ] Error handling in place
- [ ] No blocking operations in main thread
- [ ] Proper signal/slot connections
- [ ] Memory management (ownership clear)

### QML Checklist
- [ ] No complex logic in bindings
- [ ] Components broken down (reusable)
- [ ] Proper state management
- [ ] Animations for transitions
- [ ] Accessibility properties set
- [ ] Keyboard navigation works

### LayerShell Checklist
- [ ] Configuration before show()
- [ ] Appropriate layer chosen
- [ ] Exclusive zone set correctly
- [ ] Keyboard interactivity appropriate
- [ ] Tested on target compositor

### Performance Checklist
- [ ] Heavy components use Loader
- [ ] No unnecessary bindings
- [ ] Images optimized/cached
- [ ] Repeater/ListView optimized
- [ ] No blocking operations

### Testing Checklist
- [ ] Model unit tests written
- [ ] QML tests for UI
- [ ] Integration tests pass
- [ ] Tested on multiple compositors
- [ ] Error cases handled

---

## Summary: Golden Rules

1. **Separate Model from View** - Logic in C++, UI in QML
2. **Configure before show()** - LayerShell setup before display
3. **Use appropriate model type** - QObject vs QAbstractListModel
4. **Emit NOTIFY signals** - Required for QML bindings
5. **Keep QML simple** - Complex logic belongs in model
6. **Test models independently** - Unit tests for business logic
7. **Optimize performance** - Use Loader, cache, debounce
8. **Handle errors gracefully** - Fallbacks and validation
9. **Clean up resources** - Proper memory management
10. **Follow Qt best practices** - Modern Qt style

---

## Next Steps

You're now ready for production! Check:
- **[06-quick-reference.md](./06-quick-reference.md)** - Quick lookup reference

**Build amazing desktop UIs with clean architecture! 🎨**
