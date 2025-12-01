# LayerShellQt - Quick Reference Cheatsheet

## 📋 Table of Contents
- [Basic Setup](#basic-setup)
- [Layers](#layers)
- [Anchors](#anchors)
- [Common Patterns](#common-patterns)
- [API Quick Lookup](#api-quick-lookup)
- [Troubleshooting](#troubleshooting)

---

## Basic Setup

### Minimal Example

```cpp
#include <QApplication>
#include <QWidget>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Check support
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
    }
    
    // Create widget
    QWidget *widget = new QWidget();
    
    // Configure layer shell
    auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(LayerShellQt::Window::AnchorTop);
    
    // Show
    widget->show();
    return app.exec();
}
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(LayerShellQt REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp 
    Qt5::Widgets 
    LayerShellQt::Interface
)
```

### Compile Command

```bash
g++ main.cpp -o myapp \
    $(pkg-config --cflags --libs Qt5Widgets LayerShellQtInterface) \
    -fPIC
```

---

## Layers

### Layer Values

```cpp
LayerShellQt::Window::LayerBackground  // 0 - Wallpapers
LayerShellQt::Window::LayerBottom      // 1 - Widgets
LayerShellQt::Window::LayerTop         // 2 - Panels, docks
LayerShellQt::Window::LayerOverlay     // 3 - Lock screens
```

### Layer Usage Matrix

| Layer | Z-Order | Use Cases | Exclusive Zone |
|-------|---------|-----------|----------------|
| BACKGROUND | Lowest | Wallpapers, backgrounds | Usually 0 |
| BOTTOM | Below apps | Desktop widgets | Usually 0 |
| TOP | Above apps | Panels, docks | Usually > 0 |
| OVERLAY | Highest | Lock screens, OSK | Usually 0 or -1 |

### Set Layer

```cpp
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
```

---

## Anchors

### Anchor Values

```cpp
LayerShellQt::Window::AnchorTop     // 1
LayerShellQt::Window::AnchorBottom  // 2
LayerShellQt::Window::AnchorLeft    // 4
LayerShellQt::Window::AnchorRight   // 8
```

### Common Anchor Combinations

```cpp
// Top panel (full width)
AnchorTop | AnchorLeft | AnchorRight

// Bottom dock (full width)
AnchorBottom | AnchorLeft | AnchorRight

// Left sidebar (full height)
AnchorLeft | AnchorTop | AnchorBottom

// Top-right corner
AnchorTop | AnchorRight

// Bottom-left corner
AnchorBottom | AnchorLeft

// Full screen
AnchorTop | AnchorBottom | AnchorLeft | AnchorRight

// Centered
0  // No anchors
```

### Anchor Behavior

| Anchors | Width | Height | Position |
|---------|-------|--------|----------|
| Top | Screen width | setSize() | Top edge |
| Bottom | Screen width | setSize() | Bottom edge |
| Left | setSize() | Screen height | Left edge |
| Right | setSize() | Screen height | Right edge |
| Top+Left | setSize() | setSize() | Top-left corner |
| Top+Bottom | setSize() | Screen height | Vertical stretch |
| Left+Right | Screen width | setSize() | Horizontal stretch |
| All 4 | Screen width | Screen height | Full screen |
| None | setSize() | setSize() | Centered |

### Set Anchors

```cpp
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorLeft | 
    LayerShellQt::Window::AnchorRight
);
```

---

## Common Patterns

### Pattern: Top Panel

```cpp
widget->setFixedHeight(30);

auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorLeft | 
    LayerShellQt::Window::AnchorRight
);
layerWindow->setExclusiveZone(30);
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityExclusive
);

widget->show();
```

---

### Pattern: Bottom Dock

```cpp
widget->setFixedHeight(60);

auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorBottom | 
    LayerShellQt::Window::AnchorLeft | 
    LayerShellQt::Window::AnchorRight
);
layerWindow->setExclusiveZone(60);

widget->show();
```

---

### Pattern: Notification (Top-Right)

```cpp
widget->setFixedSize(350, 100);

auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerShellQt::Window::LayerTop);
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorRight
);
layerWindow->setExclusiveZone(0);  // No space reservation
layerWindow->setMargins(QMargins(0, 10, 10, 0));
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityNone
);

widget->show();
```

---

### Pattern: Desktop Widget (Top-Right)

```cpp
widget->setFixedSize(200, 150);

auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerShellQt::Window::LayerBottom);
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorRight
);
layerWindow->setExclusiveZone(0);
layerWindow->setMargins(QMargins(0, 50, 20, 0));
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityNone
);

widget->show();
```

---

### Pattern: Wallpaper

```cpp
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerShellQt::Window::LayerBackground);
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorBottom | 
    LayerShellQt::Window::AnchorLeft | 
    LayerShellQt::Window::AnchorRight
);
layerWindow->setExclusiveZone(0);
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityNone
);

widget->showFullScreen();
```

---

### Pattern: Lock Screen

```cpp
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
layerWindow->setAnchors(
    LayerShellQt::Window::AnchorTop | 
    LayerShellQt::Window::AnchorBottom | 
    LayerShellQt::Window::AnchorLeft | 
    LayerShellQt::Window::AnchorRight
);
layerWindow->setExclusiveZone(-1);  // Auto
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityExclusive
);

widget->showFullScreen();
```

---

### Pattern: Per-Monitor Panel

```cpp
for (QScreen *screen : QGuiApplication::screens()) {
    QWidget *panel = new QWidget();
    panel->setFixedHeight(30);
    
    auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(
        LayerShellQt::Window::AnchorTop | 
        LayerShellQt::Window::AnchorLeft | 
        LayerShellQt::Window::AnchorRight
    );
    layerWindow->setExclusiveZone(30);
    layerWindow->setScope(screen->name());  // Key!
    
    panel->show();
}
```

---

## API Quick Lookup

### LayerShellQt::Shell

```cpp
// Check availability
static bool LayerShellQt::Shell::isAvailable();
```

### LayerShellQt::Window

```cpp
// Factory
static LayerShellQt::Window* get(QWindow *window);

// Getters
Layer layer() const;
Anchors anchors() const;
int exclusiveZone() const;
QMargins margins() const;
KeyboardInteractivity keyboardInteractivity() const;
QString scope() const;

// Setters
void setLayer(Layer layer);
void setAnchors(Anchors anchors);
void setExclusiveZone(int zone);
void setMargins(const QMargins &margins);
void setKeyboardInteractivity(KeyboardInteractivity mode);
void setScope(const QString &scope);

// Signals
void layerChanged();
void anchorsChanged();
void exclusiveZoneChanged();
void marginsChanged();
void keyboardInteractivityChanged();
void scopeChanged();
```

### Enums

```cpp
enum Layer {
    LayerBackground = 0,
    LayerBottom = 1,
    LayerTop = 2,
    LayerOverlay = 3
};

enum Anchor {
    AnchorTop = 1,
    AnchorBottom = 2,
    AnchorLeft = 4,
    AnchorRight = 8
};

enum KeyboardInteractivity {
    KeyboardInteractivityNone = 0,
    KeyboardInteractivityExclusive = 1,
    KeyboardInteractivityOnDemand = 2
};
```

---

## Property Defaults

| Property | Default Value | Notes |
|----------|--------------|-------|
| layer | LayerTop | Most common use case |
| anchors | 0 (none) | Centered window |
| exclusiveZone | 0 | No space reservation |
| margins | (0,0,0,0) | No margins |
| keyboardInteractivity | None | No keyboard input |
| scope | "" (empty) | All outputs |

---

## Exclusive Zone Guide

| Value | Meaning | Use Case |
|-------|---------|----------|
| `> 0` | Reserve N pixels | Panels, docks |
| `0` | No reservation | Notifications, widgets |
| `-1` | Auto-calculate | Full-screen overlays |

**Examples:**
```cpp
layerWindow->setExclusiveZone(30);   // Panel reserves 30px
layerWindow->setExclusiveZone(0);    // Notification, no reservation
layerWindow->setExclusiveZone(-1);   // Lock screen, auto
```

---

## Keyboard Interactivity Guide

| Mode | Behavior | Use Case |
|------|----------|----------|
| None | No keyboard input | Wallpapers, passive widgets |
| Exclusive | Grab keyboard when focused | Panels with input, lock screens |
| OnDemand | Request keyboard on-demand | Modern apps (Qt 5.24+) |

**Examples:**
```cpp
// Wallpaper
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityNone
);

// Panel with search
layerWindow->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityExclusive
);
```

---

## Screen/Monitor Scope

### All Monitors (Default)
```cpp
layerWindow->setScope("");  // Empty string
```

### Specific Monitor
```cpp
layerWindow->setScope("HDMI-A-1");
layerWindow->setScope("eDP-1");
```

### Get Monitor Names
```cpp
for (QScreen *screen : QGuiApplication::screens()) {
    qDebug() << screen->name();  // "eDP-1", "HDMI-A-1", etc.
}
```

---

## Margins

### Set Margins
```cpp
// Uniform
layerWindow->setMargins(QMargins(10, 10, 10, 10));

// Custom (left, top, right, bottom)
layerWindow->setMargins(QMargins(5, 0, 5, 0));
```

### Margin Effect
```
Screen
┌─────────────────────┐
│ (margin top)        │
│  ┌───────────────┐  │
│  │ Window        │  │ (margin left/right)
│  └───────────────┘  │
│ (margin bottom)     │
└─────────────────────┘
```

---

## Size Behavior

### With Anchors

| Anchors | setSize() Width | setSize() Height | Actual Size |
|---------|-----------------|------------------|-------------|
| Top | Ignored | Used | Screen width × height |
| Bottom | Ignored | Used | Screen width × height |
| Left | Used | Ignored | Width × screen height |
| Right | Used | Ignored | Width × screen height |
| Top+Left | Used | Used | Width × height |
| Top+Bottom | Used | Ignored | Width × screen height |
| Left+Right | Ignored | Used | Screen width × height |
| All 4 | Ignored | Ignored | Full screen |
| None | Used | Used | Width × height (centered) |

---

## Troubleshooting

### Issue: Window appears as normal window
**Cause:** Properties set after show()
**Fix:**
```cpp
// Set properties BEFORE show()
layerWindow->setLayer(LayerTop);
widget->show();
```

---

### Issue: Layer shell not available
**Check:**
```cpp
if (!LayerShellQt::Shell::isAvailable()) {
    qDebug() << "Compositor:" << qEnvironmentVariable("XDG_SESSION_DESKTOP");
    qDebug() << "Session type:" << qEnvironmentVariable("XDG_SESSION_TYPE");
}
```

**Supported compositors:**
- KDE Plasma Wayland ✓
- Sway ✓
- Hyprland ✓
- River ✓
- GNOME ✗ (not supported)

---

### Issue: Exclusive zone not working
**Cause:** Wrong layer or zone = 0
**Fix:**
```cpp
layerWindow->setLayer(LayerTop);  // Must be TOP or OVERLAY
layerWindow->setExclusiveZone(30);  // Must be > 0
```

---

### Issue: Window not at correct position
**Debug:**
```cpp
qDebug() << "Layer:" << layerWindow->layer();
qDebug() << "Anchors:" << layerWindow->anchors();
qDebug() << "Exclusive zone:" << layerWindow->exclusiveZone();
qDebug() << "Margins:" << layerWindow->margins();
```

---

## Debugging Commands

### Enable Qt Wayland Debug
```bash
export QT_LOGGING_RULES="qt.qpa.wayland*=true"
export WAYLAND_DEBUG=1
./myapp
```

### Check Wayland Protocols
```bash
# List available protocols
wayland-info | grep layer_shell

# Expected output:
# interface: 'zwlr_layer_shell_v1', version: 4
```

---

## Environment Variables

```bash
# Session info
echo $XDG_SESSION_TYPE        # "wayland"
echo $XDG_SESSION_DESKTOP     # "KDE", "sway", etc.
echo $XDG_CURRENT_DESKTOP     # "KDE"
echo $WAYLAND_DISPLAY         # "wayland-0"

# Qt platform
echo $QT_QPA_PLATFORM         # "wayland"
```

---

## Code Snippets

### Check Support at Runtime
```cpp
if (!LayerShellQt::Shell::isAvailable()) {
    qWarning() << "Layer shell not available";
    // Fallback or exit
}
```

### Get All Screens
```cpp
for (QScreen *screen : QGuiApplication::screens()) {
    qDebug() << screen->name() << screen->geometry();
}
```

### Connect to Signals
```cpp
connect(layerWindow, &LayerShellQt::Window::layerChanged, []() {
    qDebug() << "Layer changed";
});
```

### Dynamic Reconfiguration
```cpp
// Can change anytime after show()
layerWindow->setLayer(LayerOverlay);
layerWindow->setAnchors(AnchorTop | AnchorBottom | AnchorLeft | AnchorRight);
layerWindow->setExclusiveZone(-1);
```

---

## One-Liners

```cpp
// Check support
LayerShellQt::Shell::isAvailable()

// Get layer window
LayerShellQt::Window::get(widget->windowHandle())

// Top panel
layerWindow->setLayer(LayerTop); layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight); layerWindow->setExclusiveZone(30);

// Full screen
layerWindow->setAnchors(AnchorTop | AnchorBottom | AnchorLeft | AnchorRight);

// Notification (top-right, no reserve)
layerWindow->setLayer(LayerTop); layerWindow->setAnchors(AnchorTop | AnchorRight); layerWindow->setExclusiveZone(0);

// Wallpaper
layerWindow->setLayer(LayerBackground); layerWindow->setAnchors(AnchorTop | AnchorBottom | AnchorLeft | AnchorRight);
```

---

## Performance Tips

1. **Minimize repaints:** Use `update()` only when needed
2. **Batch updates:** Change multiple properties before show
3. **Use hardware acceleration:** Enable OpenGL for QML
4. **Lazy initialization:** Create widgets on-demand
5. **Efficient painting:** Use `QPainter` clipping

---

## Best Practices Summary

✓ **DO:**
- Check `isAvailable()` before using
- Set properties before `show()`
- Use appropriate layer for use case
- Match exclusive zone to height/width
- Support multi-monitor
- Handle screen add/remove
- Clean up resources

✗ **DON'T:**
- Set properties after `show()`
- Use OVERLAY for normal UI
- Grab keyboard unnecessarily
- Forget exclusive zone for panels
- Hardcode screen dimensions
- Leak widgets on screen removal

---

## Resources

- **Official Repo:** https://invent.kde.org/plasma/layer-shell-qt
- **API Docs:** https://api.kde.org/plasma/layer-shell-qt/html/
- **Protocol Spec:** https://wayland.app/protocols/wlr-layer-shell-unstable-v1
- **Examples:** Check `examples/` folder in this repo

---

## Quick Decision Tree

```
What are you building?
│
├─ Panel/Taskbar
│  └─ Layer: TOP, Anchors: Top+Left+Right, Exclusive: height
│
├─ Dock
│  └─ Layer: TOP, Anchors: Bottom+Left+Right, Exclusive: height
│
├─ Notification
│  └─ Layer: TOP, Anchors: Top+Right, Exclusive: 0
│
├─ Desktop Widget
│  └─ Layer: BOTTOM, Anchors: corner, Exclusive: 0
│
├─ Wallpaper
│  └─ Layer: BACKGROUND, Anchors: All, Exclusive: 0
│
└─ Lock Screen
   └─ Layer: OVERLAY, Anchors: All, Exclusive: -1
```

---

## Complete Minimal Examples

### Top Panel (30px)
```cpp
QWidget *panel = new QWidget();
panel->setFixedHeight(30);
auto w = LayerShellQt::Window::get(panel->windowHandle());
w->setLayer(LayerShellQt::Window::LayerTop);
w->setAnchors(LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight);
w->setExclusiveZone(30);
panel->show();
```

### Corner Widget (200×150)
```cpp
QWidget *widget = new QWidget();
widget->setFixedSize(200, 150);
auto w = LayerShellQt::Window::get(widget->windowHandle());
w->setLayer(LayerShellQt::Window::LayerBottom);
w->setAnchors(LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorRight);
w->setMargins(QMargins(0, 10, 10, 0));
widget->show();
```

### Wallpaper
```cpp
QWidget *wallpaper = new QWidget();
auto w = LayerShellQt::Window::get(wallpaper->windowHandle());
w->setLayer(LayerShellQt::Window::LayerBackground);
w->setAnchors(LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorBottom | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight);
wallpaper->showFullScreen();
```

---

**That's it! You're ready to build with LayerShellQt! 🚀**

Print this page and keep it handy while coding!
