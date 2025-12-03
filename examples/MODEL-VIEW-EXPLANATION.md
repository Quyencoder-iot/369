# Model-View Architecture với QML

## Tổng Quan

Example này minh họa **Model-View pattern** với LayerShellQt và QML.

## Architecture

```
┌─────────────────────────────────────────────────┐
│                   APPLICATION                   │
├─────────────────────────────────────────────────┤
│                                                 │
│  ┌──────────────┐         ┌─────────────────┐  │
│  │    MODEL     │────────▶│  VIEW MANAGER   │  │
│  │  (C++ Class) │         │   (C++ Class)   │  │
│  └──────────────┘         └─────────────────┘  │
│         │                          │            │
│         │                          │            │
│         │ Expose via               │ Creates    │
│         │ QML Context              │ multiple   │
│         │                          ▼            │
│         │                  ┌──────────────┐    │
│         └─────────────────▶│ QML VIEWS    │    │
│                            │ (Panel.qml)  │    │
│                            │   × N        │    │
│                            └──────────────┘    │
│                                    │            │
│                            ┌───────▼────────┐  │
│                            │  LayerShellQt  │  │
│                            │   per screen   │  │
│                            └────────────────┘  │
└─────────────────────────────────────────────────┘
```

---

## Components

### 1. MODEL: PanelModel (C++)

**Trách nhiệm:**
- Quản lý danh sách screens
- Cung cấp data cho views
- Emit signals khi screens thay đổi

**Kế thừa:** `QAbstractListModel`

**Properties:**
- `count` - Số lượng screens
- Data roles: screenName, screenWidth, screenHeight, screenDpi, isPrimary

**Signals:**
- `countChanged()` - Khi số screens thay đổi
- `screenAdded(name)` - Khi thêm screen
- `screenRemoved(name)` - Khi remove screen

**Code:**
```cpp
class PanelModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    enum Roles {
        ScreenNameRole = Qt::UserRole + 1,
        ScreenWidthRole,
        ScreenHeightRole,
        ScreenDpiRole,
        IsPrimaryRole
    };
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    
signals:
    void countChanged();
    void screenAdded(const QString &name);
    void screenRemoved(const QString &name);
};
```

---

### 2. VIEW MANAGER: PanelViewManager (C++)

**Trách nhiệm:**
- Tạo QML view cho mỗi screen
- Configure LayerShellQt cho mỗi view
- Quản lý lifecycle của views
- React to screen hotplug

**Code:**
```cpp
class PanelViewManager : public QObject {
    Q_OBJECT
    
public:
    PanelViewManager(PanelModel *model);
    
private slots:
    void createPanelForScreen(const QString &screenName);
    void removePanelForScreen(const QString &screenName);
    
private:
    PanelModel *m_model;
    QHash<QString, QQuickView*> m_views;
};
```

**Workflow:**
```cpp
void createPanelForScreen(const QString &screenName) {
    // 1. Create QQuickView
    QQuickView *view = new QQuickView();
    
    // 2. Expose model to QML
    view->rootContext()->setContextProperty("panelModel", m_model);
    view->rootContext()->setContextProperty("currentScreenName", screenName);
    
    // 3. Load QML
    view->setSource(QUrl("qrc:/Panel.qml"));
    
    // 4. Configure LayerShellQt
    auto layerWindow = LayerShellQt::Window::get(view);
    layerWindow->setScope(screenName);  // KEY: Per-screen
    
    // 5. Show
    view->show();
}
```

---

### 3. VIEW: Panel.qml (QML)

**Trách nhiệm:**
- Hiển thị UI
- Bind to model data
- Handle user interactions

**Structure:**
```qml
Rectangle {
    // Panel UI
    
    RowLayout {
        // Left: Screen info (from model)
        Text { text: currentScreenName }
        Text { text: panelModel.data(...) }
        
        // Center: Spacer
        Item { Layout.fillWidth: true }
        
        // Right: System info
        Text { text: panelModel.count + " screens" }
        Text { id: clock }
        Button { text: "⚙️" }
    }
    
    // Connections to model signals
    Connections {
        target: panelModel
        onCountChanged: { /* update UI */ }
        onScreenAdded: { /* notification */ }
    }
}
```

**Accessing Model Data:**
```qml
// Via context property
panelModel.count

// Via data() method with roles
panelModel.data(panelModel.index(i, 0), 257)  // ScreenWidthRole

// Via invokable method
panelModel.getScreenName(i)
```

---

## Data Flow

### Screen Added

```
1. QGuiApplication::screenAdded signal
   ↓
2. PanelModel::onScreenAdded()
   ├─ Update internal list
   ├─ beginInsertRows() / endInsertRows()
   └─ emit screenAdded(name)
   ↓
3. PanelViewManager receives signal
   ├─ createPanelForScreen(name)
   ├─ Create QQuickView
   ├─ Configure LayerShellQt
   └─ Show view
   ↓
4. QML View created
   ├─ Access model via context property
   ├─ Display screen info
   └─ Update UI
```

### Screen Removed

```
1. QGuiApplication::screenRemoved signal
   ↓
2. PanelModel::onScreenRemoved()
   ├─ Find screen index
   ├─ beginRemoveRows() / endRemoveRows()
   └─ emit screenRemoved(name)
   ↓
3. PanelViewManager receives signal
   ├─ removePanelForScreen(name)
   ├─ Find and delete QQuickView
   └─ Cleanup
   ↓
4. QML View destroyed
```

---

## Key Concepts

### 1. Model-View Separation

**Benefits:**
- ✅ Business logic (C++) tách biệt khỏi UI (QML)
- ✅ Model reusable với different views
- ✅ Testing dễ dàng (test model separately)
- ✅ Performance (data processing in C++)

**Example:**
```cpp
// Model: Business logic
class PanelModel {
    QList<QScreen*> m_screens;  // Data
    void refreshScreens();       // Logic
};

// View: UI only
Panel.qml:
    Text { text: panelModel.count }  // Display data
    Button { onClicked: /* UI interaction */ }
```

---

### 2. QAbstractListModel

**Why use it?**
- Chuẩn Qt model interface
- Automatic QML integration
- Support for ListView, Repeater
- Efficient updates (beginInsertRows, etc.)

**Roles:**
```cpp
enum Roles {
    ScreenNameRole = Qt::UserRole + 1,  // Custom role
    ScreenWidthRole,
    // ...
};

QHash<int, QByteArray> roleNames() const override {
    roles[ScreenNameRole] = "screenName";  // QML property name
    // ...
}
```

**QML Access:**
```qml
// Direct property access
model.screenName
model.screenWidth

// Or via data()
model.data(model.index(i, 0), ScreenNameRole)
```

---

### 3. Context Properties

**Expose C++ to QML:**
```cpp
view->rootContext()->setContextProperty("panelModel", m_model);
view->rootContext()->setContextProperty("currentScreenName", screenName);
```

**Access in QML:**
```qml
Text { text: panelModel.count }
Text { text: currentScreenName }
```

**Alternative: Q_PROPERTY:**
```cpp
class PanelModel {
    Q_PROPERTY(int count READ count NOTIFY countChanged)
};
```

```qml
Text { text: panelModel.count }  // Automatic binding
```

---

### 4. Signals & Slots

**C++ Model signals:**
```cpp
signals:
    void countChanged();
    void screenAdded(const QString &name);
```

**QML Connections:**
```qml
Connections {
    target: panelModel
    
    function onCountChanged() {
        console.log("Count:", panelModel.count);
    }
    
    function onScreenAdded(name) {
        console.log("Added:", name);
    }
}
```

**Automatic property bindings:**
```qml
Text {
    text: panelModel.count  // Automatically updates when countChanged emitted
}
```

---

## Build & Run

### CMake Configuration

```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Quick Qml)

qt5_add_resources(QML_RESOURCES resources.qrc)

add_executable(app main.cpp ${QML_RESOURCES})
target_link_libraries(app Qt5::Quick Qt5::Qml LayerShellQt::Interface)
```

### Build

```bash
cd examples
mkdir build && cd build
cmake ..
make
./06-qml-multi-monitor-panel
```

---

## Advantages of This Architecture

### 1. Separation of Concerns
```
Model (C++)    → Data & logic
View (QML)     → UI & presentation
Manager (C++)  → Coordination
```

### 2. Scalability
- Add new screens → Automatic panel creation
- Change UI → Edit QML only
- Change logic → Edit Model only

### 3. Testability
```cpp
// Test model independently
PanelModel model;
QCOMPARE(model.count(), expectedCount);
```

### 4. Maintainability
- Clear responsibilities
- Easy to understand
- Easy to modify

---

## Comparison: QWidget vs QML

### Original (QWidget)
```cpp
for (QScreen *screen : screens) {
    QWidget *panel = createPanel();  // C++ UI
    auto window = LayerShellQt::Window::get(panel->windowHandle());
    window->setScope(screen->name());
    panel->show();
}
```

**Issues:**
- ✗ UI logic mixed với business logic
- ✗ Hard to change UI
- ✗ C++ UI code verbose
- ✗ No separation

### New (Model-View + QML)
```cpp
// Model: Data only
class PanelModel : public QAbstractListModel { /* ... */ };

// Manager: Coordination
PanelViewManager viewManager(&model);

// View: QML (declarative)
Panel.qml: Rectangle { RowLayout { /* ... */ } }
```

**Benefits:**
- ✅ Clear separation
- ✅ Easy UI changes (edit QML)
- ✅ Reusable model
- ✅ Modern declarative UI

---

## Advanced: Using ListView

**Alternative: Use QML ListView:**

```qml
// ListModel for dynamic items
ListView {
    model: panelModel
    delegate: Rectangle {
        width: parent.width
        height: 30
        
        Text {
            text: model.screenName + " - " + 
                  model.screenWidth + "×" + model.screenHeight
        }
    }
}
```

**But:** For layer shell panels, **individual QQuickView per screen** is better (separate windows with separate scopes).

---

## Summary

### Architecture Benefits

| Aspect | QWidget Approach | Model-View + QML |
|--------|-----------------|------------------|
| Separation | ✗ Mixed | ✅ Clear |
| UI Changes | ✗ Recompile | ✅ Edit QML |
| Testability | ✗ Coupled | ✅ Independent |
| Modern | ✗ Older API | ✅ Declarative |
| Performance | ✓ Good | ✓ Good |

### When to Use

**QWidget:**
- Simple UIs
- C++ only projects
- Legacy codebases

**Model-View + QML:**
- Complex UIs
- Frequent UI changes
- Modern applications
- Team với QML designers

---

## Further Reading

- Qt Model/View Programming: https://doc.qt.io/qt-5/model-view-programming.html
- QAbstractListModel: https://doc.qt.io/qt-5/qabstractlistmodel.html
- QML Integration: https://doc.qt.io/qt-5/qtqml-cppintegration-topic.html
- Context Properties: https://doc.qt.io/qt-5/qtqml-cppintegration-contextproperties.html

---

**Happy coding with clean architecture! 🎉**
