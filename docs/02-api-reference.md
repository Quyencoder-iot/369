# LayerShellQt - Complete API Reference

## Mục Lục
1. [LayerShellQt::Shell](#layershellqtshell)
2. [LayerShellQt::Window](#layershellqtwindow)
3. [Enums & Constants](#enums--constants)
4. [Signals](#signals)
5. [Properties](#properties)

---

## LayerShellQt::Shell

**Singleton** object quản lý connection với Wayland layer shell protocol.

### Header
```cpp
#include <LayerShellQt/Shell>
```

### Static Methods

#### `static bool isAvailable()`

Kiểm tra compositor có support layer shell không.

**Returns:** `true` nếu wlr-layer-shell available, `false` otherwise

**Example:**
```cpp
if (!LayerShellQt::Shell::isAvailable()) {
    qWarning() << "Layer shell not supported on this compositor!";
    // Fallback to normal windows or exit
    return;
}
```

**Use case:**
- Check trước khi khởi tạo layer shell windows
- Provide fallback cho X11 hoặc non-supporting compositors
- Runtime detection

---

## LayerShellQt::Window

**Main class** để configure layer shell properties cho một QWindow.

### Header
```cpp
#include <LayerShellQt/Window>
```

### Factory Method

#### `static Window* get(QWindow *window)`

Lấy hoặc tạo `LayerShellQt::Window` wrapper cho một `QWindow`.

**Parameters:**
- `window`: Pointer to QWindow (hoặc windowHandle của QWidget/QQuickView)

**Returns:** Pointer to LayerShellQt::Window

**Important:**
- Phải gọi **TRƯỚC KHI** window được show lần đầu
- Gọi nhiều lần với cùng QWindow → trả về cùng instance
- QWindow phải chưa được realized (chưa tạo native surface)

**Example:**
```cpp
QWindow *window = new QWindow();
auto layerWindow = LayerShellQt::Window::get(window);
layerWindow->setLayer(LayerTop);
window->show();  // Now surface is created with layer shell
```

**With QWidget:**
```cpp
QWidget *widget = new QWidget();
widget->setAttribute(Qt::WA_NativeWindow);  // Force window handle creation
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerTop);
widget->show();
```

**With QQuickView:**
```cpp
QQuickView *view = new QQuickView();
auto layerWindow = LayerShellQt::Window::get(view);
layerWindow->setLayer(LayerTop);
view->show();
```

---

### Properties & Setters

#### `void setLayer(Layer layer)`

Set layer (z-order) của surface.

**Parameters:**
- `layer`: Enum LayerShellQt::Window::Layer

**Values:**
- `LayerBackground` (0)
- `LayerBottom` (1)
- `LayerTop` (2)
- `LayerOverlay` (3)

**Default:** `LayerTop`

**Example:**
```cpp
// Wallpaper
window->setLayer(LayerShellQt::Window::LayerBackground);

// Panel
window->setLayer(LayerShellQt::Window::LayerTop);

// Lock screen
window->setLayer(LayerShellQt::Window::LayerOverlay);
```

**Note:** Có thể change runtime, compositor sẽ restacking.

---

#### `Layer layer() const`

Get current layer.

**Returns:** LayerShellQt::Window::Layer

**Example:**
```cpp
if (window->layer() == LayerShellQt::Window::LayerOverlay) {
    qDebug() << "This is an overlay window";
}
```

---

#### `void setAnchors(Anchors anchors)`

Set anchor edges.

**Parameters:**
- `anchors`: Bitmask của LayerShellQt::Window::Anchor

**Values:**
- `AnchorTop` (1)
- `AnchorBottom` (2)
- `AnchorLeft` (4)
- `AnchorRight` (8)

**Combinations:**
```cpp
// Single edge
AnchorTop
AnchorBottom
AnchorLeft
AnchorRight

// Corners
AnchorTop | AnchorLeft
AnchorTop | AnchorRight
AnchorBottom | AnchorLeft
AnchorBottom | AnchorRight

// Opposite edges (stretch)
AnchorTop | AnchorBottom
AnchorLeft | AnchorRight

// Full screen
AnchorTop | AnchorBottom | AnchorLeft | AnchorRight

// No anchor (centered)
0
```

**Default:** `0` (no anchor, centered)

**Example:**
```cpp
// Top panel spanning full width
window->setAnchors(LayerShellQt::Window::AnchorTop | 
                   LayerShellQt::Window::AnchorLeft | 
                   LayerShellQt::Window::AnchorRight);

// Bottom-right corner widget
window->setAnchors(LayerShellQt::Window::AnchorBottom | 
                   LayerShellQt::Window::AnchorRight);

// Centered notification
window->setAnchors(0);
```

---

#### `Anchors anchors() const`

Get current anchors.

**Returns:** Bitmask của anchors

**Example:**
```cpp
if (window->anchors() & LayerShellQt::Window::AnchorTop) {
    qDebug() << "Window is anchored to top";
}
```

---

#### `void setExclusiveZone(int zone)`

Set exclusive zone size.

**Parameters:**
- `zone`: Pixels to reserve, 0 for none, -1 for auto

**Values:**
- `> 0`: Reserve N pixels from anchor edge
- `0`: No reservation (normal windows can overlap)
- `-1`: Auto-calculate from window size

**Default:** `0`

**Example:**
```cpp
// Panel reserves 30px from top
window->setExclusiveZone(30);

// Notification doesn't reserve space
window->setExclusiveZone(0);

// Auto-calculate from window height
window->setExclusiveZone(-1);
```

**Best Practice:**
```cpp
// Top panel 30px tall
window->setLayer(LayerTop);
window->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
window->setSize(QSize(0, 30));
window->setExclusiveZone(30);  // Match height
```

---

#### `int exclusiveZone() const`

Get current exclusive zone.

**Returns:** Exclusive zone value

---

#### `void setMargins(const QMargins &margins)`

Set margins from anchor edges.

**Parameters:**
- `margins`: QMargins(left, top, right, bottom)

**Default:** `QMargins(0, 0, 0, 0)`

**Example:**
```cpp
// 10px margin on all sides
window->setMargins(QMargins(10, 10, 10, 10));

// Top panel with side margins
window->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
window->setMargins(QMargins(10, 0, 10, 0));  // 10px left/right
```

**Constructor variants:**
```cpp
QMargins()              // 0,0,0,0
QMargins(10, 20, 10, 20)  // left, top, right, bottom
```

---

#### `QMargins margins() const`

Get current margins.

**Returns:** QMargins object

---

#### `void setKeyboardInteractivity(KeyboardInteractivity interactivity)`

Set keyboard input mode.

**Parameters:**
- `interactivity`: Enum LayerShellQt::Window::KeyboardInteractivity

**Values:**
- `KeyboardInteractivityNone` (0) - No keyboard input
- `KeyboardInteractivityExclusive` (1) - Exclusive keyboard when focused
- `KeyboardInteractivityOnDemand` (2) - On-demand keyboard (Qt 5.24+)

**Default:** `KeyboardInteractivityNone`

**Example:**
```cpp
// Wallpaper - no keyboard
window->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityNone);

// Panel with search box - exclusive keyboard
window->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityExclusive);

// Modern on-demand (preferred)
window->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityOnDemand);
```

---

#### `KeyboardInteractivity keyboardInteractivity() const`

Get current keyboard mode.

**Returns:** KeyboardInteractivity enum

---

#### `void setScope(const QString &scope)`

Set output scope (which monitor to display on).

**Parameters:**
- `scope`: Output name, or empty string for all outputs

**Values:**
- `""` (empty) - Display on all outputs (default)
- `"eDP-1"` - Specific output name
- `"HDMI-A-1"`, `"DP-1"`, etc.

**Default:** `""` (all outputs)

**Example:**
```cpp
// All monitors
window->setScope("");

// Specific monitor
window->setScope("HDMI-A-1");

// Per-screen panels
for (QScreen *screen : QGuiApplication::screens()) {
    auto panel = createPanel();
    auto layerWin = LayerShellQt::Window::get(panel->windowHandle());
    layerWin->setScope(screen->name());
    panel->show();
}
```

**Get screen names:**
```cpp
for (QScreen *screen : QGuiApplication::screens()) {
    qDebug() << "Screen:" << screen->name();
}
```

---

#### `QString scope() const`

Get current scope.

**Returns:** Output name or empty string

---

### Signals

Layer shell windows emit Qt signals when compositor changes properties.

#### `void layerChanged()`

Emitted khi layer thay đổi.

**Example:**
```cpp
connect(layerWindow, &LayerShellQt::Window::layerChanged, [=]() {
    qDebug() << "Layer changed to:" << layerWindow->layer();
});
```

---

#### `void anchorsChanged()`

Emitted khi anchors thay đổi.

---

#### `void exclusiveZoneChanged()`

Emitted khi exclusive zone thay đổi.

---

#### `void marginsChanged()`

Emitted khi margins thay đổi.

---

#### `void keyboardInteractivityChanged()`

Emitted khi keyboard interactivity thay đổi.

---

#### `void scopeChanged()`

Emitted khi scope thay đổi.

---

## Enums & Constants

### LayerShellQt::Window::Layer

```cpp
enum Layer {
    LayerBackground = 0,
    LayerBottom = 1,
    LayerTop = 2,
    LayerOverlay = 3
};
```

**Usage:**
```cpp
LayerShellQt::Window::LayerTop
```

---

### LayerShellQt::Window::Anchor

```cpp
enum Anchor {
    AnchorTop = 1,     // 0b0001
    AnchorBottom = 2,  // 0b0010
    AnchorLeft = 4,    // 0b0100
    AnchorRight = 8    // 0b1000
};
Q_DECLARE_FLAGS(Anchors, Anchor)
```

**Bitmask type:** `LayerShellQt::Window::Anchors`

**Usage:**
```cpp
LayerShellQt::Window::Anchors anchors = 
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorLeft;
```

---

### LayerShellQt::Window::KeyboardInteractivity

```cpp
enum KeyboardInteractivity {
    KeyboardInteractivityNone = 0,
    KeyboardInteractivityExclusive = 1,
    KeyboardInteractivityOnDemand = 2  // Qt 5.24+
};
```

**Usage:**
```cpp
LayerShellQt::Window::KeyboardInteractivityExclusive
```

---

## Complete Example: All Properties

```cpp
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Check availability
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    
    // Create widget
    QWidget *panel = new QWidget();
    panel->setFixedHeight(40);
    
    QLabel *label = new QLabel("My Panel", panel);
    label->setStyleSheet("color: white;");
    
    // Get layer window
    auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
    
    // Configure all properties
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop | 
        LayerShellQt::Window::AnchorLeft | 
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(40);
    layerWindow->setMargins(QMargins(5, 0, 5, 0));
    layerWindow->setKeyboardInteractivity(
        LayerShellQt::Window::KeyboardInteractivityExclusive
    );
    layerWindow->setScope("");  // All outputs
    
    // Connect signals
    QObject::connect(layerWindow, &LayerShellQt::Window::anchorsChanged, [=]() {
        qDebug() << "Anchors changed!";
    });
    
    // Show
    panel->show();
    
    return app.exec();
}
```

---

## CMake Integration

### Find Package

```cmake
find_package(LayerShellQt REQUIRED)
```

**Variables set:**
- `LayerShellQt_FOUND` - TRUE if found
- `LayerShellQt_VERSION` - Version string
- `LayerShellQt_INCLUDE_DIRS` - Include directories
- `LayerShellQt_LIBRARIES` - Libraries to link

### Link Libraries

```cmake
target_link_libraries(myapp 
    LayerShellQt::Interface
)
```

**Full CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyLayerShellApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(LayerShellQt REQUIRED)

add_executable(myapp main.cpp)

target_link_libraries(myapp
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    LayerShellQt::Interface
)
```

---

## QML Integration (Qt Quick)

LayerShellQt works với QML/Qt Quick!

### Example

```cpp
// main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <LayerShellQt/Window>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    QQuickView view;
    view.setSource(QUrl("qrc:/main.qml"));
    
    auto layerWindow = LayerShellQt::Window::get(&view);
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(LayerShellQt::Window::AnchorTop | 
                           LayerShellQt::Window::AnchorLeft | 
                           LayerShellQt::Window::AnchorRight);
    layerWindow->setExclusiveZone(50);
    
    view.show();
    return app.exec();
}
```

```qml
// main.qml
import QtQuick 2.15

Rectangle {
    width: 1920
    height: 50
    color: "#2d2d2d"
    
    Text {
        anchors.centerIn: parent
        text: "QML Panel"
        color: "white"
        font.pixelSize: 20
    }
}
```

**CMakeLists.txt additions:**
```cmake
find_package(Qt5 REQUIRED COMPONENTS Quick)
target_link_libraries(myapp Qt5::Quick)
```

---

## Property Summary Table

| Property | Type | Default | Purpose |
|----------|------|---------|---------|
| `layer` | Layer | LayerTop | Z-order position |
| `anchors` | Anchors | 0 (none) | Edge attachment |
| `exclusiveZone` | int | 0 | Space reservation |
| `margins` | QMargins | (0,0,0,0) | Edge offsets |
| `keyboardInteractivity` | KeyboardInteractivity | None | Keyboard input mode |
| `scope` | QString | "" (all) | Output selection |

---

## Error Handling

### Layer Shell Not Available

```cpp
if (!LayerShellQt::Shell::isAvailable()) {
    // Fallback strategies:
    
    // 1. Exit with error
    qFatal("Layer shell required but not available");
    
    // 2. Use normal windows (X11-style)
    QWidget *widget = new QWidget();
    widget->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | 
                          Qt::WindowStaysOnTopHint);
    widget->show();
    
    // 3. Inform user
    QMessageBox::warning(nullptr, "Unsupported Compositor",
        "This compositor doesn't support layer shell.\n"
        "Please use KDE Plasma Wayland, Sway, or Hyprland.");
    return 1;
}
```

### Window Already Realized

```cpp
// BAD: Window already shown
QWidget *widget = new QWidget();
widget->show();
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
// Too late! Surface already created as xdg-toplevel

// GOOD: Configure before show
QWidget *widget = new QWidget();
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerTop);
widget->show();  // Now creates layer surface
```

---

## Version Compatibility

### Qt 5.15+

LayerShellQt requires **Qt 5.15** or later.

```cmake
find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets)
```

### LayerShellQt Versions

- **5.24+**: Adds `KeyboardInteractivityOnDemand`
- **5.27+**: Improved multi-monitor support

**Check version:**
```cpp
#include <LayerShellQt/Version>
qDebug() << "LayerShellQt version:" << LAYERSHELLQT_VERSION_STRING;
```

---

## Next Steps

Bây giờ bạn biết toàn bộ API! Tiếp theo:
- **[03-examples.md](./03-examples.md)** - Code examples thực tế
- **[04-advanced-topics.md](./04-advanced-topics.md)** - Advanced patterns

Happy coding! 🚀
