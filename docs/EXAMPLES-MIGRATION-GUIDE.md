# 🔄 Examples Migration Guide: QWidget → QML + Model-View

## Tổng Quan

File `03-examples.md` đã được **hoàn toàn refactor** từ QWidget sang **QML với Model-View architecture**.

## Changes

### ❌ Old Version (QWidget)
- File: `03-examples-widget-backup.md` (backup)
- Architecture: Monolithic QWidget code
- UI: C++ QWidget/QLabel/QPushButton
- Lines: 1,185

### ✅ New Version (QML + Model-View)
- File: `03-examples.md` (current)
- Architecture: Separated Model (C++) + View (QML)
- UI: Declarative QML
- Lines: 2,084 (more detailed!)

---

## Migration Comparison

### Example: Top Panel

#### Old (QWidget)

```cpp
// panel.cpp - Everything in one file
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

class TopPanel : public QWidget {
    Q_OBJECT
public:
    TopPanel() {
        setFixedHeight(35);
        
        QHBoxLayout *layout = new QHBoxLayout(this);
        
        QLabel *title = new QLabel("My Panel");
        clockLabel = new QLabel();
        
        layout->addWidget(title);
        layout->addStretch();
        layout->addWidget(clockLabel);
        
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TopPanel::updateClock);
        timer->start(1000);
        updateClock();
        
        setupLayerShell();
    }
    
private:
    void setupLayerShell() { /* ... */ }
    void updateClock() {
        clockLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
    }
    
    QLabel *clockLabel;
};
```

**Issues:**
- ✗ UI code mixed với logic
- ✗ Hard-coded styling
- ✗ Difficult to modify UI
- ✗ No separation

#### New (Model-View + QML)

**Model (C++):**
```cpp
// PanelModel.h - Business logic only
class PanelModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
    
public:
    PanelModel() {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PanelModel::updateTime);
        timer->start(1000);
    }
    
    QString currentTime() const { return m_currentTime; }
    
signals:
    void timeChanged();
    
private:
    void updateTime() {
        m_currentTime = QTime::currentTime().toString("hh:mm:ss");
        emit timeChanged();
    }
    
    QString m_currentTime;
};
```

**View (QML):**
```qml
// Panel.qml - UI only
import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    color: "#2d2d2d"
    height: 35
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        
        Text {
            text: "My Panel"
            color: "white"
        }
        
        Item { Layout.fillWidth: true }
        
        Text {
            text: panelModel.currentTime  // Data binding!
            color: "white"
        }
    }
}
```

**Main (C++):**
```cpp
// main.cpp - Integration
int main() {
    QGuiApplication app(argc, argv);
    
    PanelModel model;
    
    QQuickView view;
    view.rootContext()->setContextProperty("panelModel", &model);
    view.setSource(QUrl("qrc:/Panel.qml"));
    
    // LayerShellQt configuration
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerTop);
    // ...
    
    view.show();
    return app.exec();
}
```

**Benefits:**
- ✅ Clear separation (logic vs UI)
- ✅ Easy styling changes (edit QML)
- ✅ Data binding automatic
- ✅ Testable components

---

## All 10 Examples Migrated

| # | Example | Old | New |
|---|---------|-----|-----|
| 1 | Top Panel | QWidget | PanelModel + Panel.qml |
| 2 | Bottom Dock | QWidget | DockModel + Dock.qml |
| 3 | Desktop Widget | QWidget | WidgetModel + Widget.qml |
| 4 | Wallpaper | QWidget (QPainter) | WallpaperModel + Wallpaper.qml |
| 5 | Notification | QWidget | NotificationModel + Notification.qml |
| 6 | Corner Widget | QWidget | CornerWidgetModel + CornerWidget.qml |
| 7 | Multi-Monitor | QWidget | MultiPanelModel + MultiPanel.qml |
| 8 | QML Panel | Already QML | Enhanced: DynamicPanelModel + DynamicPanel.qml |
| 9 | Lock Screen | QWidget | LockScreenModel + LockScreen.qml |
| 10 | System Tray | QWidget | SystemTrayModel + SystemTray.qml |

---

## Key Architectural Changes

### 1. Separation of Concerns

**Before:**
```
panel.cpp
├─ UI Code (QWidget, QLayout, QLabel)
├─ Business Logic (timer, data updates)
├─ Styling (setStyleSheet)
└─ LayerShell config
```

**After:**
```
Model (C++)
├─ Business logic only
├─ Data management
└─ System integration

View (QML)
├─ UI declaration
├─ Styling
└─ User interactions

Main (C++)
├─ Integration
└─ LayerShell config
```

### 2. Data Binding

**Before (Manual):**
```cpp
void updateClock() {
    clockLabel->setText(QTime::currentTime().toString());
}
```

**After (Automatic):**
```qml
Text {
    text: panelModel.currentTime  // Auto-updates!
}
```

### 3. Styling

**Before (C++ QString):**
```cpp
setStyleSheet(R"(
    QWidget { background-color: #2d2d2d; }
    QLabel { color: white; }
)");
```

**After (QML Native):**
```qml
Rectangle {
    color: "#2d2d2d"
    
    Text {
        color: "white"
    }
}
```

### 4. Testability

**Before:**
```cpp
// Can't test UI separately from logic
```

**After:**
```cpp
// Test model independently
void testModel() {
    PanelModel model;
    QString time = model.currentTime();
    QVERIFY(!time.isEmpty());
}
```

---

## Migration Benefits

### For Development

✅ **Faster UI Iteration**
- Edit QML → See changes
- No recompile for UI changes
- Hot reload in development

✅ **Cleaner Code**
- Model: Data & logic
- View: Presentation
- Clear responsibilities

✅ **Better Testing**
- Test models independently
- Mock data for UI testing
- Unit test business logic

### For Maintenance

✅ **Easier to Understand**
- Code organized by concern
- QML is declarative (readable)
- Less cognitive load

✅ **Easier to Modify**
- Change UI → Edit QML only
- Change logic → Edit Model only
- No mixing

✅ **Easier to Extend**
- Add properties to model
- Add UI elements to QML
- Connect via data binding

### For Designers

✅ **Designer-Friendly**
- Qt Designer support
- Visual QML editing
- No C++ knowledge needed for UI

---

## Learning Path

### If You're New to QML

1. **Read old examples** (`03-examples-widget-backup.md`)
   - Understand what each example does
   - See QWidget approach

2. **Compare with new examples** (`03-examples.md`)
   - See Model class
   - See QML view
   - Understand separation

3. **Try modifying QML**
   - Change colors
   - Add elements
   - See how easy it is!

4. **Learn QML basics**
   - Rectangle, Text, Button
   - RowLayout, ColumnLayout
   - Property bindings

5. **Learn Model-View pattern**
   - Q_PROPERTY
   - Q_INVOKABLE
   - Signals & slots

### If You Know QWidget

1. **See migration patterns** (this guide)
2. **Pick one example**
3. **Refactor your own code** from QWidget → QML
4. **Enjoy clean architecture!**

---

## Common Patterns

### Pattern 1: Simple Data Model

```cpp
class SimpleModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    
signals:
    void textChanged();
};
```

```qml
Text { text: model.text }  // Auto-binding
```

### Pattern 2: List Model

```cpp
class ListModel : public QAbstractListModel {
    // For Repeater, ListView
};
```

```qml
Repeater {
    model: listModel
    delegate: Text { text: model.name }
}
```

### Pattern 3: Action Model

```cpp
class ActionModel : public QObject {
    Q_INVOKABLE void doAction();
};
```

```qml
Button {
    onClicked: model.doAction()
}
```

---

## Build Changes

### Old (QWidget)

```cmake
find_package(Qt5 REQUIRED COMPONENTS Widgets)
add_executable(app main.cpp)
target_link_libraries(app Qt5::Widgets)
```

### New (QML)

```cmake
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Quick Qml)
qt5_add_resources(RESOURCES resources.qrc)
add_executable(app main.cpp ${RESOURCES})
target_link_libraries(app Qt5::Quick Qt5::Qml)
```

---

## File Structure

### Old (QWidget Example)

```
example/
├── main.cpp (everything)
└── CMakeLists.txt
```

### New (QML Example)

```
example/
├── main.cpp (integration)
├── Model.h (business logic)
├── View.qml (UI)
├── resources.qrc (QML resource)
└── CMakeLists.txt
```

---

## Quick Comparison Table

| Aspect | QWidget | QML + Model-View |
|--------|---------|------------------|
| **Architecture** | Monolithic | Separated |
| **UI Code** | C++ (verbose) | QML (declarative) |
| **Styling** | StyleSheet strings | Native QML |
| **Data Updates** | Manual setText() | Automatic binding |
| **Hot Reload** | ✗ No | ✅ Yes (dev) |
| **Designer Tools** | Limited | ✅ Qt Designer |
| **Testability** | Coupled | ✅ Independent |
| **Lines of Code** | Less | More (but cleaner) |
| **Learning Curve** | Lower | Higher (worth it!) |
| **Maintenance** | Harder | ✅ Easier |
| **Future** | Legacy | ✅ Modern Qt |

---

## Recommendation

### Use QWidget When:
- Simple utilities
- Legacy codebases
- C++ only team
- Very simple UI

### Use QML + Model-View When:
- Modern applications
- Complex UI
- Frequent UI changes
- Team collaboration (designers + developers)
- **Any serious desktop shell project** ✅

---

## Resources

### Learn QML
- **Qt QML Documentation:** https://doc.qt.io/qt-5/qmlapplications.html
- **QML Tutorial:** https://doc.qt.io/qt-5/qml-tutorial.html
- **Qt Quick Controls:** https://doc.qt.io/qt-5/qtquickcontrols-index.html

### Learn Model-View
- **Model/View Programming:** https://doc.qt.io/qt-5/model-view-programming.html
- **QML C++ Integration:** https://doc.qt.io/qt-5/qtqml-cppintegration-topic.html

### Examples
- **New examples:** `docs/03-examples.md`
- **Old examples (backup):** `docs/03-examples-widget-backup.md`
- **Working code:** `examples/06-qml-multi-monitor-panel.cpp`

---

## Conclusion

Migration từ QWidget sang QML + Model-View là một **upgrade lớn** với nhiều benefits:

✅ **Better architecture**
✅ **Cleaner code**
✅ **Easier maintenance**
✅ **Modern approach**
✅ **Future-proof**

**Recommendation:** Nếu bạn đang bắt đầu project mới, sử dụng QML approach ngay từ đầu!

**Happy coding với clean architecture! 🎨**

---

*Created: December 2024*
*Part of LayerShellQt Learning Resource*
