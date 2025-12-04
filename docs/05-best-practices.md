# LayerShellQt - Best Practices & Design Patterns (QML + Model-View)

## Mục Lục
1. [Design Principles](#design-principles)
2. [Model-View Patterns](#model-view-patterns)
3. [QML Best Practices](#qml-best-practices)
4. [C++ Model Patterns](#c-model-patterns)
5. [UI/UX Guidelines](#uiux-guidelines)
6. [Performance Best Practices](#performance-best-practices)
7. [Error Handling](#error-handling)
8. [Testing Strategies](#testing-strategies)
9. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Separate Model from View

**Always separate business logic (C++) from UI (QML)!**

```cpp
// ✓ GOOD: Model - Business logic only
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
    
public:
    QString currentTime() const { return m_currentTime; }
    
signals:
    void timeChanged();
    
private:
    void updateTime(); // Logic here
    QString m_currentTime;
};
```

```qml
// ✓ GOOD: View - UI only
Rectangle {
    Text {
        text: panelModel.currentTime  // Data binding
        color: "white"
    }
}
```

```cpp
// ✗ BAD: Mixed logic and UI
class PanelWidget : public QWidget {
    void paintEvent(QPaintEvent*) {
        // Drawing code
        QString time = getCurrentTime(); // Logic mixed in!
        painter.drawText(rect(), time);
    }
};
```

---

### Principle 2: Configure Before Show

**Set LayerShellQt properties BEFORE showing window!**

```cpp
// ✓ GOOD
QQuickView view;
view.rootContext()->setContextProperty("model", &model);
view.setSource(QUrl("qrc:/Panel.qml"));

// Configure layer shell BEFORE show
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);

view.show();  // Now window appears correctly
```

```cpp
// ✗ BAD
view.show();  // Too early!
auto layerWindow = LayerShellQt::Window::get(&view);
layerWindow->setLayer(LayerTop);  // Won't work properly
```

---

### Principle 3: Use Appropriate Layer

**Choose layer theo use case:**

```qml
// ✓ GOOD: Panel uses TOP layer
// Model.h
Q_PROPERTY(int recommendedLayer READ recommendedLayer CONSTANT)

int recommendedLayer() const {
    return LayerShellQt::Window::LayerTop;
}
```

```cpp
// ✓ GOOD: Set appropriate layer
layerWindow->setLayer(model.recommendedLayer());
```

**Layer Decision Tree:**

```
Need user interaction?
├─ Yes → TOP or OVERLAY
│  ├─ Critical/blocking? → OVERLAY
│  └─ Normal panel/dock? → TOP
└─ No → BOTTOM or BACKGROUND
   ├─ Below windows? → BOTTOM
   └─ Wallpaper? → BACKGROUND
```

---

### Principle 4: Expose Only Necessary Data to QML

**Keep Model interface minimal:**

```cpp
// ✓ GOOD: Minimal interface
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString displayText READ displayText NOTIFY textChanged)
    
public:
    QString displayText() const { return m_displayText; }
    
signals:
    void textChanged();
    
private:
    // Internal complexity hidden
    void fetchData();
    void processData();
    QString m_displayText;
    QList<InternalData> m_internalList;  // Not exposed to QML
};
```

```cpp
// ✗ BAD: Exposing everything
class BadModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<InternalData> rawData READ rawData)  // Too much!
    Q_PROPERTY(DatabaseConnection* db READ db)  // Dangerous!
    // QML shouldn't access these directly
};
```

---

## Model-View Patterns

### Pattern 1: Simple Property Model

**For single values:**

```cpp
// Model (C++)
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value READ value NOTIFY valueChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY readyChanged)
    
public:
    QString value() const { return m_value; }
    bool isReady() const { return m_ready; }
    
    Q_INVOKABLE void refresh() {
        // Update data
        m_value = fetchNewValue();
        emit valueChanged();
    }
    
signals:
    void valueChanged();
    void readyChanged();
    
private:
    QString m_value;
    bool m_ready = false;
};
```

```qml
// View (QML)
Rectangle {
    Text {
        text: model.value
        visible: model.isReady
    }
    
    Button {
        text: "Refresh"
        onClicked: model.refresh()
    }
}
```

---

### Pattern 2: List Model

**For collections:**

```cpp
// Model (C++)
class AppListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
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
    
    int count() const { return m_apps.count(); }
    
    Q_INVOKABLE void addApp(const QString &name, const QString &icon, 
                           const QString &command) {
        beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());
        m_apps.append({name, icon, command});
        endInsertRows();
        emit countChanged();
    }
    
signals:
    void countChanged();
    
private:
    QList<App> m_apps;
};
```

```qml
// View (QML)
ListView {
    model: appListModel
    
    delegate: Rectangle {
        width: parent.width
        height: 50
        
        Row {
            spacing: 10
            
            Text {
                text: model.icon
                font.pixelSize: 24
            }
            
            Text {
                text: model.name
                color: "white"
            }
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Launch app via model
                Qt.openUrlExternally(model.command)
            }
        }
    }
}
```

---

### Pattern 3: Manager Pattern

**For managing multiple views:**

```cpp
// ViewManager (C++)
class PanelViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit PanelViewManager(QAbstractListModel *model, QObject *parent = nullptr)
        : QObject(parent), m_model(model) {
        
        // Watch model changes
        connect(model, &QAbstractListModel::rowsInserted,
                this, &PanelViewManager::onRowsInserted);
        connect(model, &QAbstractListModel::rowsRemoved,
                this, &PanelViewManager::onRowsRemoved);
        
        // Create initial views
        createViews();
    }
    
private slots:
    void onRowsInserted(const QModelIndex &parent, int first, int last) {
        for (int i = first; i <= last; ++i) {
            createViewForRow(i);
        }
    }
    
    void onRowsRemoved(const QModelIndex &parent, int first, int last) {
        for (int i = first; i <= last; ++i) {
            removeViewForRow(i);
        }
    }
    
private:
    void createViewForRow(int row) {
        // Get data from model
        QString screenName = m_model->data(
            m_model->index(row, 0), 
            ScreenModel::NameRole
        ).toString();
        
        // Create view
        QQuickView *view = new QQuickView();
        view->rootContext()->setContextProperty("model", m_model);
        view->rootContext()->setContextProperty("screenName", screenName);
        view->setSource(QUrl("qrc:/Panel.qml"));
        
        // Configure layer shell
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setScope(screenName);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        view->show();
        m_views.insert(screenName, view);
    }
    
    void removeViewForRow(int row) { /* ... */ }
    
    QAbstractListModel *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

**Usage:**
```cpp
ScreenListModel model;
PanelViewManager manager(&model);
// Manager automatically creates/destroys views based on model
```

---

### Pattern 4: Command Pattern (Actions)

**For user actions:**

```cpp
// Model with actions
class ActionModel : public QObject {
    Q_OBJECT
    
public:
    Q_INVOKABLE void executeAction(const QString &actionName) {
        if (actionName == "volume") {
            openVolumeControl();
        } else if (actionName == "screenshot") {
            takeScreenshot();
        } else if (actionName == "lock") {
            lockScreen();
        }
    }
    
    Q_INVOKABLE bool canExecute(const QString &actionName) const {
        // Check if action is available
        return checkActionAvailability(actionName);
    }
    
signals:
    void actionExecuted(const QString &actionName);
    void actionFailed(const QString &actionName, const QString &error);
    
private:
    void openVolumeControl();
    void takeScreenshot();
    void lockScreen();
};
```

```qml
// View
Column {
    Repeater {
        model: [
            {name: "volume", icon: "🔊", text: "Volume"},
            {name: "screenshot", icon: "📸", text: "Screenshot"},
            {name: "lock", icon: "🔒", text: "Lock Screen"}
        ]
        
        delegate: Button {
            text: modelData.icon + " " + modelData.text
            enabled: actionModel.canExecute(modelData.name)
            
            onClicked: {
                actionModel.executeAction(modelData.name)
            }
        }
    }
    
    Connections {
        target: actionModel
        
        function onActionFailed(actionName, error) {
            errorText.text = "Failed: " + error
        }
    }
}
```

---

## QML Best Practices

### Practice 1: Use Property Bindings

```qml
// ✓ GOOD: Property binding (automatic updates)
Rectangle {
    width: parent.width
    height: model.panelHeight
    color: model.isActive ? "#4fc3f7" : "#2d2d2d"
    
    Text {
        text: model.currentTime  // Auto-updates
        visible: model.showClock  // Auto-updates
    }
}
```

```qml
// ✗ BAD: Manual updates
Rectangle {
    id: rect
    
    Connections {
        target: model
        onPanelHeightChanged: {
            rect.height = model.panelHeight  // Manual!
        }
        onIsActiveChanged: {
            rect.color = model.isActive ? "#4fc3f7" : "#2d2d2d"  // Manual!
        }
    }
}
```

---

### Practice 2: Component Composition

```qml
// ✓ GOOD: Reusable components
// StatusIcon.qml
import QtQuick 2.15

Item {
    property string icon: ""
    property string text: ""
    property string status: ""
    
    width: 60
    height: 40
    
    Column {
        anchors.centerIn: parent
        spacing: 2
        
        Text {
            text: icon
            font.pixelSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        Text {
            text: status
            font.pixelSize: 10
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
```

```qml
// Usage
Row {
    StatusIcon {
        icon: "🔊"
        status: model.volumeLevel + "%"
    }
    
    StatusIcon {
        icon: "🔋"
        status: model.batteryLevel + "%"
    }
    
    StatusIcon {
        icon: "📶"
        status: model.networkStatus
    }
}
```

---

### Practice 3: Avoid JavaScript Logic in QML

```qml
// ✗ BAD: Complex logic in QML
Rectangle {
    property var items: model.items
    
    Text {
        text: {
            var result = "";
            for (var i = 0; i < items.length; i++) {
                if (items[i].active) {
                    result += items[i].name;
                    if (i < items.length - 1) result += ", ";
                }
            }
            return result;
        }
    }
}
```

```cpp
// ✓ GOOD: Logic in C++ model
class Model : public QObject {
    Q_PROPERTY(QString activeItemsText READ activeItemsText NOTIFY itemsChanged)
    
    QString activeItemsText() const {
        QStringList active;
        for (const Item &item : m_items) {
            if (item.active) active << item.name;
        }
        return active.join(", ");
    }
};
```

```qml
// ✓ GOOD: Simple binding in QML
Text {
    text: model.activeItemsText
}
```

---

### Practice 4: Use Loaders for Dynamic Content

```qml
// ✓ GOOD: Lazy loading
Loader {
    id: settingsLoader
    active: false
    sourceComponent: SettingsPanel {}
}

Button {
    text: "Settings"
    onClicked: settingsLoader.active = true
}
```

```qml
// ✗ BAD: Always loaded
SettingsPanel {
    visible: false  // Still loaded, just hidden!
}

Button {
    text: "Settings"
    onClicked: settingsPanel.visible = true
}
```

---

### Practice 5: State Management

```qml
// ✓ GOOD: QML States
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
    
    // Control from model
    state: model.isExpanded ? "expanded" : "collapsed"
}
```

---

## C++ Model Patterns

### Pattern 1: Read-Only Properties

```cpp
// ✓ GOOD: Read-only properties with NOTIFY
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    
public:
    QString text() const { return m_text; }
    
signals:
    void textChanged();
    
private:
    void updateText(const QString &newText) {
        if (m_text != newText) {
            m_text = newText;
            emit textChanged();  // QML auto-updates
        }
    }
    
    QString m_text;
};
```

```cpp
// ✗ BAD: Writable properties from QML
class BadModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    
    // QML can modify internal state directly - dangerous!
    void setText(const QString &text) { m_text = text; }
};
```

---

### Pattern 2: Use Q_INVOKABLE for Actions

```cpp
// ✓ GOOD: Actions via Q_INVOKABLE
class Model : public QObject {
    Q_OBJECT
    
public:
    Q_INVOKABLE void performAction() {
        // Validate, execute, emit signals
        if (!canPerformAction()) {
            emit actionFailed("Cannot perform action");
            return;
        }
        
        doAction();
        emit actionCompleted();
    }
    
signals:
    void actionCompleted();
    void actionFailed(const QString &error);
    
private:
    bool canPerformAction() const;
    void doAction();
};
```

---

### Pattern 3: Async Operations

```cpp
// ✓ GOOD: Async with signals
class DataModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString data READ data NOTIFY dataChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    
public:
    Q_INVOKABLE void fetchData() {
        setLoading(true);
        
        // Async operation
        QFuture<QString> future = QtConcurrent::run([this]() {
            return downloadData();
        });
        
        auto watcher = new QFutureWatcher<QString>(this);
        connect(watcher, &QFutureWatcher<QString>::finished, [this, watcher]() {
            try {
                m_data = watcher->result();
                emit dataChanged();
            } catch (const std::exception &e) {
                m_error = e.what();
                emit errorChanged();
            }
            setLoading(false);
            watcher->deleteLater();
        });
        watcher->setFuture(future);
    }
    
    bool loading() const { return m_loading; }
    QString data() const { return m_data; }
    QString error() const { return m_error; }
    
signals:
    void loadingChanged();
    void dataChanged();
    void errorChanged();
    
private:
    QString downloadData();
    
    void setLoading(bool loading) {
        if (m_loading != loading) {
            m_loading = loading;
            emit loadingChanged();
        }
    }
    
    bool m_loading = false;
    QString m_data;
    QString m_error;
};
```

```qml
// View with loading state
Column {
    BusyIndicator {
        running: model.loading
        visible: model.loading
    }
    
    Text {
        text: model.data
        visible: !model.loading && !model.error
    }
    
    Text {
        text: "Error: " + model.error
        color: "red"
        visible: model.error
    }
    
    Button {
        text: "Refresh"
        enabled: !model.loading
        onClicked: model.fetchData()
    }
}
```

---

### Pattern 4: Model Validation

```cpp
// ✓ GOOD: Validate in model
class FormModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString passwordError READ passwordError NOTIFY passwordErrorChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    
public:
    QString password() const { return m_password; }
    QString passwordError() const { return m_passwordError; }
    bool isValid() const { return m_isValid; }
    
    void setPassword(const QString &password) {
        m_password = password;
        validatePassword();
        emit passwordChanged();
    }
    
signals:
    void passwordChanged();
    void passwordErrorChanged();
    void isValidChanged();
    
private:
    void validatePassword() {
        QString oldError = m_passwordError;
        bool oldValid = m_isValid;
        
        if (m_password.length() < 8) {
            m_passwordError = "Password too short";
            m_isValid = false;
        } else if (!containsNumber(m_password)) {
            m_passwordError = "Password must contain a number";
            m_isValid = false;
        } else {
            m_passwordError.clear();
            m_isValid = true;
        }
        
        if (oldError != m_passwordError) emit passwordErrorChanged();
        if (oldValid != m_isValid) emit isValidChanged();
    }
    
    QString m_password;
    QString m_passwordError;
    bool m_isValid = false;
};
```

```qml
// View
Column {
    TextField {
        text: model.password
        onTextChanged: model.password = text
        echoMode: TextInput.Password
    }
    
    Text {
        text: model.passwordError
        color: "red"
        visible: model.passwordError
    }
    
    Button {
        text: "Submit"
        enabled: model.isValid
    }
}
```

---

## UI/UX Guidelines

### Guideline 1: Visual Hierarchy

**Use QML properties for styling:**

```qml
// ✓ GOOD: Clear visual hierarchy
Rectangle {
    id: panel
    
    // Theme properties
    readonly property color primaryColor: "#4fc3f7"
    readonly property color backgroundColor: "#2d2d2d"
    readonly property color textColor: "white"
    readonly property int titleSize: 16
    readonly property int normalSize: 12
    
    color: backgroundColor
    
    Text {
        text: "Title"
        color: primaryColor
        font.pixelSize: titleSize
        font.bold: true
    }
    
    Text {
        text: "Content"
        color: textColor
        font.pixelSize: normalSize
    }
}
```

---

### Guideline 2: Responsive Sizing

```qml
// ✓ GOOD: Responsive to screen size
Rectangle {
    // Adapt to screen dimensions
    property real scaleFactor: Math.min(
        Screen.width / 1920,
        Screen.height / 1080
    )
    
    height: 35 * scaleFactor
    
    Text {
        font.pixelSize: 14 * scaleFactor
    }
}
```

---

### Guideline 3: Consistent Spacing

```qml
// ✓ GOOD: Use spacing properties
Item {
    readonly property int spacingSmall: 5
    readonly property int spacingMedium: 10
    readonly property int spacingLarge: 20
    
    ColumnLayout {
        spacing: spacingMedium
        
        Rectangle {
            Layout.margins: spacingSmall
        }
        
        Rectangle {
            Layout.topMargin: spacingLarge
        }
    }
}
```

---

### Guideline 4: Animations

```qml
// ✓ GOOD: Smooth animations
Rectangle {
    id: button
    color: "#3d3d3d"
    
    Behavior on color {
        ColorAnimation { duration: 150 }
    }
    
    scale: 1.0
    
    Behavior on scale {
        NumberAnimation { 
            duration: 100
            easing.type: Easing.OutCubic
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            button.color = "#4d4d4d"
            button.scale = 1.05
        }
        
        onExited: {
            button.color = "#3d3d3d"
            button.scale = 1.0
        }
    }
}
```

---

## Performance Best Practices

### Practice 1: Minimize Property Bindings

```qml
// ✗ BAD: Complex binding recalculated often
Text {
    text: model.firstName + " " + model.lastName + 
          " (" + model.age + " years old)"
}
```

```cpp
// ✓ GOOD: Pre-computed in model
class Model : public QObject {
    Q_PROPERTY(QString displayName READ displayName NOTIFY dataChanged)
    
    QString displayName() const {
        return QString("%1 %2 (%3 years old)")
            .arg(m_firstName, m_lastName)
            .arg(m_age);
    }
};
```

```qml
// ✓ GOOD: Simple binding
Text {
    text: model.displayName
}
```

---

### Practice 2: Use Loaders for Heavy Content

```qml
// ✓ GOOD: Load on demand
Loader {
    id: heavyContentLoader
    active: false
    asynchronous: true
    
    sourceComponent: Component {
        HeavyContent {
            // Complex UI here
        }
    }
}

Button {
    onClicked: heavyContentLoader.active = true
}
```

---

### Practice 3: Optimize List Views

```qml
// ✓ GOOD: Optimized ListView
ListView {
    model: appListModel
    
    // Performance optimizations
    cacheBuffer: 100  // Cache off-screen items
    reuseItems: true  // Reuse delegate instances
    
    delegate: Rectangle {
        // Keep delegate simple!
        width: parent.width
        height: 50
        
        Text {
            text: model.name
            anchors.centerIn: parent
        }
    }
}
```

---

### Practice 4: Debounce Frequent Updates

```cpp
// ✓ GOOD: Debounce in model
class Model : public QObject {
    Q_OBJECT
    
public:
    void onDataChanged() {
        // Debounce updates
        m_updateTimer.stop();
        m_updateTimer.start();
    }
    
private:
    void scheduleUpdate() {
        m_updateTimer = new QTimer(this);
        m_updateTimer->setInterval(100);
        m_updateTimer->setSingleShot(true);
        connect(m_updateTimer, &QTimer::timeout, [this]() {
            emit dataChanged();
        });
    }
    
    QTimer *m_updateTimer;
};
```

---

## Error Handling

### Pattern 1: Graceful Degradation

```cpp
// ✓ GOOD: Handle missing features
class Model : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool featureAvailable READ featureAvailable CONSTANT)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    
public:
    bool featureAvailable() const {
        return checkFeatureAvailability();
    }
    
    Q_INVOKABLE void useFeature() {
        if (!featureAvailable()) {
            m_errorMessage = "Feature not available on this system";
            emit errorChanged();
            return;
        }
        
        try {
            performFeature();
        } catch (const std::exception &e) {
            m_errorMessage = e.what();
            emit errorChanged();
        }
    }
    
signals:
    void errorChanged();
    
private:
    QString m_errorMessage;
};
```

```qml
// View with fallback
Column {
    Button {
        text: "Use Feature"
        enabled: model.featureAvailable
        onClicked: model.useFeature()
    }
    
    Text {
        text: model.featureAvailable 
            ? "Feature ready" 
            : "Feature not available"
        color: model.featureAvailable ? "green" : "orange"
    }
    
    Text {
        text: model.errorMessage
        color: "red"
        visible: model.errorMessage
    }
}
```

---

### Pattern 2: Validation Before Action

```cpp
// ✓ GOOD: Validate before executing
class ActionModel : public QObject {
    Q_OBJECT
    
public:
    Q_INVOKABLE bool canExecute(const QString &action) const {
        if (action == "screenshot") {
            return hasScreenshotTool();
        } else if (action == "lock") {
            return hasLockScreen();
        }
        return false;
    }
    
    Q_INVOKABLE void execute(const QString &action) {
        if (!canExecute(action)) {
            emit executionFailed(action, "Action not available");
            return;
        }
        
        // Execute
        doAction(action);
        emit executionSucceeded(action);
    }
    
signals:
    void executionSucceeded(const QString &action);
    void executionFailed(const QString &action, const QString &reason);
};
```

```qml
// View
Button {
    text: "Take Screenshot"
    enabled: actionModel.canExecute("screenshot")
    
    onClicked: actionModel.execute("screenshot")
    
    ToolTip.visible: !enabled
    ToolTip.text: "Screenshot tool not available"
}
```

---

## Testing Strategies

### Strategy 1: Test Models Independently

```cpp
// Test C++ model
class ModelTest : public QObject {
    Q_OBJECT
    
private slots:
    void testPropertyBinding() {
        PanelModel model;
        QSignalSpy spy(&model, &PanelModel::timeChanged);
        
        model.updateTime();
        
        QCOMPARE(spy.count(), 1);
        QVERIFY(!model.currentTime().isEmpty());
    }
    
    void testAction() {
        ActionModel model;
        QSignalSpy successSpy(&model, &ActionModel::actionCompleted);
        
        model.executeAction("test");
        
        QCOMPARE(successSpy.count(), 1);
    }
};

QTEST_MAIN(ModelTest)
```

---

### Strategy 2: QML Tests

```qml
// TestPanel.qml
import QtQuick 2.15
import QtTest 1.15

TestCase {
    name: "PanelTests"
    
    Panel {
        id: panel
    }
    
    function test_initialization() {
        verify(panel.width > 0)
        verify(panel.height > 0)
    }
    
    function test_button_click() {
        var button = findChild(panel, "myButton")
        verify(button !== null)
        
        mouseClick(button)
        // Verify action occurred
    }
}
```

---

### Strategy 3: Mock Models for UI Testing

```cpp
// MockModel for UI testing
class MockPanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime CONSTANT)
    
public:
    QString currentTime() const { return "12:34:56"; }
};
```

```cpp
// Test with mock
void testUI() {
    MockPanelModel mockModel;
    
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &mockModel);
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    // Test UI behavior with known data
}
```

---

## Common Pitfalls

### Pitfall 1: Forgetting NOTIFY Signals

```cpp
// ✗ BAD: No NOTIFY signal
Q_PROPERTY(QString text READ text)

// QML won't update when text changes!
```

```cpp
// ✓ GOOD: With NOTIFY
Q_PROPERTY(QString text READ text NOTIFY textChanged)

signals:
    void textChanged();
```

---

### Pitfall 2: Complex Logic in QML

```qml
// ✗ BAD: Business logic in QML
Button {
    onClicked: {
        // Complex calculations
        var result = 0;
        for (var i = 0; i < data.length; i++) {
            result += calculateValue(data[i]);
        }
        // Database operations
        saveToDatabase(result);
        // Network requests
        sendToServer(result);
    }
}
```

```cpp
// ✓ GOOD: Logic in C++
class Model : public QObject {
    Q_INVOKABLE void processData() {
        int result = 0;
        for (const Data &d : m_data) {
            result += calculateValue(d);
        }
        saveToDatabase(result);
        sendToServer(result);
        emit dataProcessed();
    }
};
```

```qml
// ✓ GOOD: Simple action in QML
Button {
    onClicked: model.processData()
}
```

---

### Pitfall 3: Not Using LayerShellQt Correctly

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

### Pitfall 4: Memory Leaks with QML Objects

```qml
// ✗ BAD: Creating objects without parent
Button {
    onClicked: {
        var obj = Qt.createQmlObject('import QtQuick 2.15; Rectangle {}', root)
        // obj has no parent, will leak!
    }
}
```

```qml
// ✓ GOOD: Proper ownership
Button {
    onClicked: {
        var obj = Qt.createQmlObject(
            'import QtQuick 2.15; Rectangle {}', 
            root  // parent
        )
    }
}
```

---

### Pitfall 5: Blocking UI Thread

```cpp
// ✗ BAD: Long operation on UI thread
Q_INVOKABLE void loadData() {
    QThread::sleep(5);  // BLOCKS UI!
    m_data = fetchData();
    emit dataChanged();
}
```

```cpp
// ✓ GOOD: Async operation
Q_INVOKABLE void loadData() {
    QtConcurrent::run([this]() {
        QString data = fetchData();  // Background thread
        
        QMetaObject::invokeMethod(this, [this, data]() {
            m_data = data;
            emit dataChanged();
        }, Qt::QueuedConnection);
    });
}
```

---

## Checklist: Before Production

### Architecture
- [ ] Models separate from views
- [ ] Business logic in C++
- [ ] UI in QML
- [ ] Clear data flow

### LayerShellQt
- [ ] Check `isAvailable()` at startup
- [ ] Configure before `show()`
- [ ] Appropriate layer for use case
- [ ] Correct anchor configuration
- [ ] Proper exclusive zone

### Models
- [ ] All properties have NOTIFY signals
- [ ] Q_INVOKABLE for actions
- [ ] Input validation
- [ ] Error handling
- [ ] Async for long operations

### QML
- [ ] Use property bindings
- [ ] No complex JavaScript
- [ ] Component composition
- [ ] Loaders for heavy content
- [ ] Proper ownership

### Performance
- [ ] No blocking operations
- [ ] Optimized list views
- [ ] Minimal property bindings
- [ ] Debounced updates

### Testing
- [ ] Unit tests for models
- [ ] QML tests for UI
- [ ] Mock models for testing
- [ ] Integration tests

### Error Handling
- [ ] Graceful degradation
- [ ] User-friendly errors
- [ ] Logging
- [ ] Recovery mechanisms

---

## Summary: Golden Rules

### Model-View
1. **Separate concerns**: Model (C++) vs View (QML)
2. **Data flows down**: Model → QML via properties
3. **Events flow up**: QML → Model via Q_INVOKABLE

### LayerShellQt
4. **Configure early**: Before show()
5. **Choose wisely**: Right layer for use case
6. **Reserve space**: Exclusive zones for panels

### QML
7. **Bind, don't set**: Use property bindings
8. **Compose**: Break into components
9. **Keep simple**: No complex logic

### C++
10. **Expose minimally**: Only what QML needs
11. **Signal changes**: Always emit NOTIFY
12. **Validate input**: Before executing

### Performance
13. **Don't block**: Async for long ops
14. **Lazy load**: Use Loaders
15. **Optimize lists**: cacheBuffer, reuseItems

---

**Happy coding với clean Model-View architecture! 🎨**
