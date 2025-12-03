# 🎨 QML Multi-Monitor Panel - Model-View Example

## Overview

This example demonstrates **Model-View architecture** with **QML view** for LayerShellQt panels.

## Architecture

```
┌──────────────┐
│ PanelModel   │ (C++ - QAbstractListModel)
│   - Data     │ ← Manages screens, provides data
│   - Logic    │
└──────┬───────┘
       │ Exposed via QML Context
       ↓
┌──────────────────┐
│ PanelViewManager │ (C++)
│  - Creates views │ ← One QQuickView per screen
│  - LayerShell    │ ← Configure layer shell per view
└──────┬───────────┘
       │ Creates
       ↓
┌──────────────┐
│  Panel.qml   │ (QML - Declarative UI)
│  - UI only   │ ← Access model data, display UI
│  - Bindings  │
└──────────────┘
```

## Files

1. **06-qml-multi-monitor-panel.cpp** - Main C++ (Model + Manager)
2. **Panel.qml** - QML View (UI)
3. **resources.qrc** - Qt Resource file
4. **MODEL-VIEW-EXPLANATION.md** - Detailed explanation

## Key Components

### 1. PanelModel (C++)

```cpp
class PanelModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
    // Data roles for QML access
    enum Roles {
        ScreenNameRole,
        ScreenWidthRole,
        ScreenHeightRole,
        ScreenDpiRole,
        IsPrimaryRole
    };
    
signals:
    void countChanged();
    void screenAdded(const QString &name);
    void screenRemoved(const QString &name);
};
```

### 2. PanelViewManager (C++)

```cpp
class PanelViewManager : public QObject {
    // Creates one QQuickView per screen
    // Configures LayerShellQt for each view
    // Handles screen hotplug
    
private:
    QHash<QString, QQuickView*> m_views;
};
```

### 3. Panel.qml (QML)

```qml
Rectangle {
    // Access model
    Text { text: currentScreenName }
    Text { text: panelModel.count + " screens" }
    
    // Connections to model signals
    Connections {
        target: panelModel
        onCountChanged: { /* ... */ }
    }
}
```

## Build

### Option 1: Shell Script

```bash
./build-qml-example.sh
```

### Option 2: CMake

```bash
mkdir build && cd build
cmake ..
make
```

### Option 3: Manual

```bash
# Generate resources
rcc resources.qrc -o qrc_resources.cpp

# Compile
g++ 06-qml-multi-monitor-panel.cpp qrc_resources.cpp \
    -o 06-qml-multi-monitor-panel \
    $(pkg-config --cflags --libs Qt5Quick Qt5Qml LayerShellQtInterface) \
    -fPIC
```

## Run

```bash
./06-qml-multi-monitor-panel
```

## Features

✅ **Model-View separation**
- Business logic in C++ (PanelModel)
- UI in QML (Panel.qml)
- Clean separation of concerns

✅ **Per-screen panels**
- One panel per monitor
- Each with LayerShellQt scope

✅ **Dynamic updates**
- Hotplug support (add/remove monitors)
- Automatic panel creation/destruction
- Real-time UI updates

✅ **QML bindings**
- Automatic property updates
- Signal/slot connections
- Declarative UI

## What You Get

### Data Binding Example

QML automatically updates when model changes:

```qml
// This text updates automatically when panelModel.count changes
Text {
    text: panelModel.count + " screens"
}
```

### Signal Connection Example

React to model signals in QML:

```qml
Connections {
    target: panelModel
    
    function onScreenAdded(name) {
        console.log("New screen:", name);
        // Could show notification, animate, etc.
    }
}
```

### Data Access Example

Access model data in QML:

```qml
// Method 1: Context property
Text { text: currentScreenName }

// Method 2: Model property
Text { text: panelModel.count }

// Method 3: Invokable method
Text { text: panelModel.getScreenName(0) }

// Method 4: Data roles (for ListView, etc.)
ListView {
    model: panelModel
    delegate: Text { text: model.screenName }
}
```

## Advantages Over QWidget

### QWidget Approach

```cpp
// Everything in C++
for (QScreen *screen : screens) {
    QWidget *panel = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(panel);
    QLabel *label = new QLabel("Text", panel);
    layout->addWidget(label);
    // ... more C++ UI code
    panel->show();
}
```

**Issues:**
- ✗ UI code mixed with logic
- ✗ Verbose C++ UI code
- ✗ Hard to change styling
- ✗ Requires recompile for UI changes

### QML Approach

```cpp
// C++ - Data only
PanelModel model;
PanelViewManager manager(&model);
```

```qml
// QML - UI only (declarative)
Rectangle {
    Text { text: panelModel.count }
    Button { text: "Click" }
}
```

**Benefits:**
- ✅ Clear separation
- ✅ Declarative UI (easier)
- ✅ Hot reload (in development)
- ✅ Designer-friendly (Qt Designer)
- ✅ Modern architecture

## Testing

### Test Model Independently

```cpp
void testModel() {
    PanelModel model;
    
    QCOMPARE(model.count(), QGuiApplication::screens().count());
    
    // Test data access
    QModelIndex index = model.index(0, 0);
    QString name = model.data(index, PanelModel::ScreenNameRole).toString();
    QVERIFY(!name.isEmpty());
}
```

### Test View Separately

```qml
// Test.qml - Load Panel.qml with mock model
import QtQuick 2.15

Panel {
    // Override context properties for testing
}
```

## Customization

### Change UI (Panel.qml)

```qml
// Change colors
Rectangle {
    color: "#282c34"  // Dark theme
}

// Add widgets
Button {
    text: "New Feature"
    onClicked: { /* ... */ }
}

// Change layout
ColumnLayout { /* vertical instead */ }
```

**No recompile needed!** (in development with hot reload)

### Change Model (C++)

```cpp
// Add new property
class PanelModel {
    Q_PROPERTY(QString theme READ theme NOTIFY themeChanged)
    
signals:
    void themeChanged();
};
```

```qml
// Access in QML
Text { text: panelModel.theme }
```

## Multi-Monitor Behavior

```
Monitor 1 (eDP-1)           Monitor 2 (HDMI-A-1)
┌─────────────────────┐    ┌─────────────────────┐
│ Panel (scope=eDP-1) │    │ Panel (scope=HDMI-A)│
├─────────────────────┤    ├─────────────────────┤
│ Screen: eDP-1       │    │ Screen: HDMI-A-1    │
│ 1920×1080           │    │ 2560×1440           │
│ 2 screens     12:34 │    │ 2 screens     12:34 │
└─────────────────────┘    └─────────────────────┘
        ↑                           ↑
        └───────────┬───────────────┘
                    │
            Single PanelModel
            (shared data)
```

Each panel:
- Shows same system info (from shared model)
- Shows its own screen name
- Independent LayerShellQt scope

## Debugging

### Enable QML Debug Output

```bash
export QT_LOGGING_RULES="qml*=true"
./06-qml-multi-monitor-panel
```

### Check Model Updates

```qml
Connections {
    target: panelModel
    onCountChanged: {
        console.log("Count changed:", panelModel.count);
    }
}
```

### Inspect Properties

```qml
Component.onCompleted: {
    console.log("Screen name:", currentScreenName);
    console.log("Model count:", panelModel.count);
}
```

## Troubleshooting

### Issue: QML file not found

**Cause:** resources.qrc not compiled

**Fix:**
```bash
rcc resources.qrc -o qrc_resources.cpp
# Include qrc_resources.cpp in compilation
```

### Issue: Model not accessible in QML

**Cause:** Not exposed via context property

**Fix:**
```cpp
view->rootContext()->setContextProperty("panelModel", m_model);
```

### Issue: Properties not updating

**Cause:** Missing NOTIFY signal

**Fix:**
```cpp
Q_PROPERTY(int count READ count NOTIFY countChanged)
                                        ^^^^^^^^^^^
```

## Further Reading

- **MODEL-VIEW-EXPLANATION.md** - Detailed architecture explanation
- **Qt Model/View:** https://doc.qt.io/qt-5/model-view-programming.html
- **QML C++ Integration:** https://doc.qt.io/qt-5/qtqml-cppintegration-topic.html

## Comparison: Original vs Refactored

### Original (QWidget)

```cpp
QList<QScreen*> screens = QGuiApplication::screens();
for (QScreen *screen : screens) {
    QWidget *panel = createPanel();
    auto window = LayerShellQt::Window::get(panel->windowHandle());
    window->setScope(screen->name());
    panel->show();
}
```

### Refactored (Model-View + QML)

```cpp
// Model
PanelModel model;  // Manages screens

// View Manager
PanelViewManager viewManager(&model);  // Creates QML views

// Views (Panel.qml)
// Declarative UI with data binding
```

**Result:**
- ✅ Cleaner code
- ✅ Better architecture
- ✅ Easier to maintain
- ✅ Testable components
- ✅ Modern approach

---

**Enjoy clean architecture with QML! 🎨**
