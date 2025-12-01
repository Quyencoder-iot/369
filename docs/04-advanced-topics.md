# LayerShellQt - Advanced Topics

## Mục Lục
1. [Dynamic Configuration](#dynamic-configuration)
2. [Multi-Monitor Support](#multi-monitor-support)
3. [Window Lifetime Management](#window-lifetime-management)
4. [Performance Optimization](#performance-optimization)
5. [Integration với KDE/Plasma](#integration-với-kdeplasma)
6. [Custom Protocols](#custom-protocols)
7. [Debugging & Troubleshooting](#debugging--troubleshooting)

---

## Dynamic Configuration

### Runtime Property Changes

Có thể **thay đổi properties** của layer surface sau khi đã show:

```cpp
auto layerWindow = LayerShellQt::Window::get(window);

// Initial config
layerWindow->setLayer(LayerTop);
layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
window->show();

// Later: Change layer
layerWindow->setLayer(LayerOverlay);  // Compositor restacks

// Change anchors
layerWindow->setAnchors(AnchorBottom | AnchorLeft | AnchorRight);

// Change exclusive zone
layerWindow->setExclusiveZone(50);
```

**Compositor sẽ reconfigure surface automatically!**

### Expand/Collapse Panel

Panel có thể thay đổi chiều cao động:

```cpp
class DynamicPanel : public QWidget {
    Q_OBJECT
public:
    DynamicPanel() {
        collapsed = true;
        setFixedHeight(30);
        
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(LayerTop);
        layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
        layerWindow->setExclusiveZone(30);
        
        QPushButton *toggleBtn = new QPushButton("▼ Expand");
        connect(toggleBtn, &QPushButton::clicked, this, &DynamicPanel::toggle);
    }
    
private slots:
    void toggle() {
        collapsed = !collapsed;
        int newHeight = collapsed ? 30 : 200;
        
        setFixedHeight(newHeight);
        
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setExclusiveZone(newHeight);
        
        // Update button text
        findChild<QPushButton*>()->setText(collapsed ? "▼ Expand" : "▲ Collapse");
    }
    
    bool collapsed;
};
```

### Conditional Layer Switching

Switch layer dựa trên state:

```cpp
class AdaptiveWidget : public QWidget {
public:
    void setAlwaysOnTop(bool onTop) {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        if (onTop) {
            layerWindow->setLayer(LayerShellQt::Window::LayerOverlay);
            layerWindow->setExclusiveZone(0);
        } else {
            layerWindow->setLayer(LayerShellQt::Window::LayerBottom);
        }
    }
    
    void setFullAttention() {
        // Lock screen mode
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(LayerOverlay);
        layerWindow->setAnchors(AnchorTop | AnchorBottom | AnchorLeft | AnchorRight);
        layerWindow->setKeyboardInteractivity(KeyboardInteractivityExclusive);
    }
    
    void setNormalMode() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(LayerTop);
        layerWindow->setAnchors(AnchorTop | AnchorRight);
        layerWindow->setKeyboardInteractivity(KeyboardInteractivityNone);
    }
};
```

---

## Multi-Monitor Support

### Strategy 1: Single Surface Across All Monitors

**Default behavior:** `scope = ""` → surface spans all outputs

```cpp
// Panel on all monitors
auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
layerWindow->setScope("");  // Default
panel->show();
```

**Kết quả:**
```
Monitor 1                Monitor 2
┌──────────────────┐     ┌──────────────────┐
│ Panel spans both │─────│ monitors         │
├──────────────────┤     ├──────────────────┤
│                  │     │                  │
└──────────────────┘     └──────────────────┘
```

**Pros:**
- Simple
- Single instance
- Consistent appearance

**Cons:**
- Can't have per-monitor content
- Stretches across gap between monitors

---

### Strategy 2: Per-Monitor Instances

**Best practice:** Create separate window per monitor

```cpp
class MonitorManager : public QObject {
    Q_OBJECT
public:
    MonitorManager() {
        // Create panel for each existing screen
        for (QScreen *screen : QGuiApplication::screens()) {
            createPanelForScreen(screen);
        }
        
        // Handle screen hotplug
        connect(qApp, &QGuiApplication::screenAdded, 
                this, &MonitorManager::createPanelForScreen);
        connect(qApp, &QGuiApplication::screenRemoved, 
                this, &MonitorManager::removePanelForScreen);
    }
    
private:
    void createPanelForScreen(QScreen *screen) {
        qDebug() << "Creating panel for" << screen->name() 
                 << "geometry:" << screen->geometry();
        
        QWidget *panel = new QWidget();
        panel->setFixedHeight(30);
        panel->setProperty("screenName", screen->name());
        
        // Add screen-specific content
        QLabel *label = new QLabel("Panel on: " + screen->name());
        QHBoxLayout *layout = new QHBoxLayout(panel);
        layout->addWidget(label);
        
        auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
        layerWindow->setLayer(LayerTop);
        layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
        layerWindow->setExclusiveZone(30);
        layerWindow->setScope(screen->name());  // Key: per-screen scope
        
        panel->show();
        panels.insert(screen->name(), panel);
    }
    
    void removePanelForScreen(QScreen *screen) {
        qDebug() << "Removing panel for" << screen->name();
        
        QWidget *panel = panels.take(screen->name());
        if (panel) {
            panel->deleteLater();
        }
    }
    
    QHash<QString, QWidget*> panels;
};
```

**Usage:**
```cpp
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MonitorManager manager;
    
    return app.exec();
}
```

---

### Strategy 3: Primary Monitor Only

```cpp
QScreen *primaryScreen = QGuiApplication::primaryScreen();

auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
layerWindow->setScope(primaryScreen->name());
panel->show();
```

---

### Getting Screen Information

```cpp
for (QScreen *screen : QGuiApplication::screens()) {
    qDebug() << "Screen:" << screen->name();
    qDebug() << "  Geometry:" << screen->geometry();
    qDebug() << "  Available geometry:" << screen->availableGeometry();
    qDebug() << "  Physical size:" << screen->physicalSize();
    qDebug() << "  DPI:" << screen->logicalDotsPerInch();
    qDebug() << "  Scale factor:" << screen->devicePixelRatio();
}
```

**Example output:**
```
Screen: "eDP-1"
  Geometry: QRect(0,0 1920x1080)
  Available geometry: QRect(0,30 1920x1050)  # Minus exclusive zones
  Physical size: QSizeF(340, 190)
  DPI: 142.857
  Scale factor: 1

Screen: "HDMI-A-1"
  Geometry: QRect(1920,0 2560x1440)
  Available geometry: QRect(1920,30 2560x1410)
  Physical size: QSizeF(600, 340)
  DPI: 108.267
  Scale factor: 1.5
```

---

### Handling Screen Configuration Changes

```cpp
class ScreenAwarePanel : public QWidget {
    Q_OBJECT
public:
    ScreenAwarePanel(QScreen *screen) : targetScreen(screen) {
        setupLayerShell();
        
        // React to screen geometry changes
        connect(screen, &QScreen::geometryChanged, 
                this, &ScreenAwarePanel::onGeometryChanged);
        connect(screen, &QScreen::availableGeometryChanged,
                this, &ScreenAwarePanel::onAvailableGeometryChanged);
    }
    
private slots:
    void onGeometryChanged(const QRect &geometry) {
        qDebug() << "Screen geometry changed:" << geometry;
        // Adjust panel if needed
    }
    
    void onAvailableGeometryChanged(const QRect &geometry) {
        qDebug() << "Available geometry changed:" << geometry;
        // Other panels changed exclusive zones
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setScope(targetScreen->name());
        // ... other config
    }
    
    QScreen *targetScreen;
};
```

---

## Window Lifetime Management

### Proper Initialization Order

**CRITICAL:** Layer shell properties phải set **BEFORE** window is shown!

```cpp
// ✓ CORRECT
QWidget *widget = new QWidget();
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerTop);
widget->show();  // Surface created with layer shell

// ✗ WRONG
QWidget *widget = new QWidget();
widget->show();  // Surface created as xdg-toplevel
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerTop);  // Too late! Won't work
```

### With QWidget

```cpp
QWidget *widget = new QWidget();

// Force creation of window handle
widget->setAttribute(Qt::WA_NativeWindow);

// Now get layer window
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());
layerWindow->setLayer(LayerTop);

widget->show();
```

### With QQuickView

```cpp
QQuickView *view = new QQuickView();
view->setSource(QUrl("qrc:/main.qml"));

// Get layer window directly
auto layerWindow = LayerShellQt::Window::get(view);
layerWindow->setLayer(LayerTop);

view->show();
```

### With QMainWindow

```cpp
QMainWindow *mainWindow = new QMainWindow();

// Must get windowHandle()
auto layerWindow = LayerShellQt::Window::get(mainWindow->windowHandle());
layerWindow->setLayer(LayerTop);

mainWindow->show();
```

---

### Ownership & Cleanup

**LayerShellQt::Window** is **owned by QWindow**:

```cpp
QWidget *widget = new QWidget();
auto layerWindow = LayerShellQt::Window::get(widget->windowHandle());

// When widget is deleted, layerWindow is also cleaned up
delete widget;  // Automatic cleanup
```

**Manual cleanup not needed!**

---

## Performance Optimization

### 1. Reduce Repaints

```cpp
class OptimizedPanel : public QWidget {
public:
    OptimizedPanel() {
        // Only repaint when data actually changes
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground);
    }
    
protected:
    void paintEvent(QPaintEvent *event) override {
        // Only paint damaged region
        QPainter painter(this);
        painter.setClipRegion(event->region());
        
        // ... paint
    }
};
```

### 2. Use Hardware Acceleration

```cpp
// Enable OpenGL rendering
QQuickView *view = new QQuickView();
view->setSource(QUrl("qrc:/main.qml"));

// Enable multisampling
QSurfaceFormat format;
format.setSamples(4);
view->setFormat(format);
```

### 3. Minimize Layer Switches

```cpp
// BAD: Frequent layer switches
window->setLayer(LayerTop);
window->setLayer(LayerOverlay);
window->setLayer(LayerTop);  // Compositor restacks each time

// GOOD: Switch only when necessary
if (needsAttention && window->layer() != LayerOverlay) {
    window->setLayer(LayerOverlay);
}
```

### 4. Efficient Size Changes

```cpp
// BAD: Resize every frame
for (int i = 30; i <= 200; i++) {
    widget->setFixedHeight(i);
    layerWindow->setExclusiveZone(i);
    QThread::msleep(10);  // Animator
}

// GOOD: Use animation framework
QPropertyAnimation *anim = new QPropertyAnimation(widget, "maximumHeight");
anim->setDuration(300);
anim->setStartValue(30);
anim->setEndValue(200);
anim->start();
```

### 5. Reduce Exclusive Zone Updates

```cpp
// Only update exclusive zone when actually needed
void updateExclusiveZone(int newHeight) {
    if (m_lastExclusiveZone == newHeight) {
        return;  // No change
    }
    
    m_lastExclusiveZone = newHeight;
    layerWindow->setExclusiveZone(newHeight);
}
```

---

## Integration với KDE/Plasma

### Detect Plasma Environment

```cpp
bool isKDEPlasma() {
    return qEnvironmentVariable("XDG_CURRENT_DESKTOP") == "KDE";
}

bool isPlasmaWayland() {
    return isKDEPlasma() && 
           qEnvironmentVariable("XDG_SESSION_TYPE") == "wayland";
}
```

### Use Plasma Theme

```cpp
#include <KDecoration2/DecorationTheme>
#include <KConfigGroup>
#include <KSharedConfig>

QColor getPlasmaAccentColor() {
    auto config = KSharedConfig::openConfig("kdeglobals");
    KConfigGroup group(config, "Colors:Window");
    return group.readEntry("BackgroundNormal", QColor("#2d2d2d"));
}
```

### Integrate with Plasma Notifications

```cpp
#include <KNotification>

void showNotification(const QString &title, const QString &message) {
    KNotification *notif = new KNotification("myevent");
    notif->setTitle(title);
    notif->setText(message);
    notif->sendEvent();
}
```

### Follow Plasma Panel Conventions

```cpp
class PlasmaLikePanel : public QWidget {
public:
    PlasmaLikePanel() {
        // Standard Plasma panel height
        setFixedHeight(32);
        
        // Use Plasma color scheme
        auto config = KSharedConfig::openConfig("kdeglobals");
        KConfigGroup group(config, "Colors:Window");
        QColor bg = group.readEntry("BackgroundNormal", QColor("#2d2d2d"));
        QColor fg = group.readEntry("ForegroundNormal", QColor("#ffffff"));
        
        setStyleSheet(QString(
            "background-color: %1; color: %2;"
        ).arg(bg.name(), fg.name()));
    }
};
```

---

## Custom Protocols

### Using Other Wayland Protocols

LayerShellQt can coexist with other protocols:

```cpp
// Layer shell for positioning
auto layerWindow = LayerShellQt::Window::get(window);
layerWindow->setLayer(LayerTop);

// KWayland for blur effect
#include <KWayland/Client/blur.h>
#include <KWayland/Client/connection_thread.h>

KWayland::Client::ConnectionThread *connection = 
    KWayland::Client::ConnectionThread::fromApplication();

KWayland::Client::BlurManager *blurManager = 
    registry->createBlurManager(name, version);

KWayland::Client::Blur *blur = blurManager->createBlur(surface);
blur->setRegion(region);
blur->commit();
```

---

## Debugging & Troubleshooting

### Check Layer Shell Availability

```cpp
#include <LayerShellQt/Shell>

void checkSupport() {
    if (!LayerShellQt::Shell::isAvailable()) {
        qWarning() << "Layer shell NOT available";
        qWarning() << "Compositor:" << qEnvironmentVariable("XDG_SESSION_DESKTOP");
        qWarning() << "Session type:" << qEnvironmentVariable("XDG_SESSION_TYPE");
        
        // Try to detect why
        if (qEnvironmentVariable("XDG_SESSION_TYPE") != "wayland") {
            qWarning() << "Not running Wayland!";
        } else if (qEnvironmentVariable("XDG_SESSION_DESKTOP") == "gnome") {
            qWarning() << "GNOME doesn't support wlr-layer-shell";
        }
    } else {
        qDebug() << "Layer shell available!";
    }
}
```

### Enable Qt Wayland Debugging

```bash
# Environment variables
export QT_LOGGING_RULES="qt.qpa.wayland*=true"
export WAYLAND_DEBUG=1

# Run app
./myapp
```

**Output:**
```
qt.qpa.wayland: Registered global "zwlr_layer_shell_v1" version 4
qt.qpa.wayland: Layer surface created
qt.qpa.wayland: Configure: size=(1920, 30)
```

### Inspect Surface Properties

```cpp
void debugLayerWindow(LayerShellQt::Window *window) {
    qDebug() << "Layer:" << window->layer();
    qDebug() << "Anchors:" << window->anchors();
    qDebug() << "Exclusive zone:" << window->exclusiveZone();
    qDebug() << "Margins:" << window->margins();
    qDebug() << "Keyboard:" << window->keyboardInteractivity();
    qDebug() << "Scope:" << window->scope();
}
```

### Common Issues

#### Issue 1: Window appears as normal window

**Cause:** Layer properties set after window shown

**Fix:**
```cpp
// Set properties BEFORE show()
auto layerWindow = LayerShellQt::Window::get(window);
layerWindow->setLayer(LayerTop);
window->show();  // Now it works
```

#### Issue 2: Window not at correct position

**Cause:** Wrong anchors

**Debug:**
```cpp
qDebug() << "Anchors:" << layerWindow->anchors();
qDebug() << "Expected:" << (AnchorTop | AnchorLeft | AnchorRight);
```

#### Issue 3: Exclusive zone not working

**Cause:** Exclusive zone = 0 or wrong layer

**Fix:**
```cpp
// Must be Layer TOP or OVERLAY
layerWindow->setLayer(LayerTop);
layerWindow->setExclusiveZone(30);  // Must be > 0
```

#### Issue 4: Compositor crash

**Cause:** Invalid property combination

**Safe defaults:**
```cpp
layerWindow->setLayer(LayerTop);
layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
layerWindow->setExclusiveZone(30);
layerWindow->setMargins(QMargins());
layerWindow->setKeyboardInteractivity(KeyboardInteractivityNone);
layerWindow->setScope("");
```

---

### Logging Helper

```cpp
class LayerShellLogger : public QObject {
    Q_OBJECT
public:
    static void install(LayerShellQt::Window *window) {
        connect(window, &LayerShellQt::Window::layerChanged, []() {
            qDebug() << "[LayerShell] Layer changed";
        });
        connect(window, &LayerShellQt::Window::anchorsChanged, []() {
            qDebug() << "[LayerShell] Anchors changed";
        });
        connect(window, &LayerShellQt::Window::exclusiveZoneChanged, []() {
            qDebug() << "[LayerShell] Exclusive zone changed";
        });
    }
};

// Usage
auto layerWindow = LayerShellQt::Window::get(window);
LayerShellLogger::install(layerWindow);
```

---

## Wayland Compositor Compatibility

### Supported Compositors

| Compositor | Support | Notes |
|-----------|---------|-------|
| KDE Plasma (Wayland) | ✓ Full | Native support |
| Sway | ✓ Full | Reference implementation |
| Hyprland | ✓ Full | Full support |
| River | ✓ Full | Full support |
| Wayfire | ✓ Partial | Check version |
| Weston | ✗ No | Doesn't support wlr-layer-shell |
| GNOME | ✗ No | No plans to support |
| Mutter | ✗ No | GNOME's compositor |

### Runtime Detection

```cpp
QString detectCompositor() {
    QString desktop = qEnvironmentVariable("XDG_SESSION_DESKTOP");
    QString compositor = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    QString waylandDisplay = qEnvironmentVariable("WAYLAND_DISPLAY");
    
    if (compositor.contains("KDE")) return "KDE Plasma";
    if (compositor.contains("sway")) return "Sway";
    if (compositor.contains("Hyprland")) return "Hyprland";
    if (compositor.contains("GNOME")) return "GNOME (unsupported)";
    
    return "Unknown";
}
```

---

## Next Steps

Advanced topics covered! Next:
- **[05-best-practices.md](./05-best-practices.md)** - Best practices & patterns

You're becoming an expert! 🎓
