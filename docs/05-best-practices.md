# LayerShellQt - Best Practices & Design Patterns

## Mục Lục
1. [Design Principles](#design-principles)
2. [Coding Patterns](#coding-patterns)
3. [UI/UX Guidelines](#uiux-guidelines)
4. [Performance Best Practices](#performance-best-practices)
5. [Error Handling](#error-handling)
6. [Testing Strategies](#testing-strategies)
7. [Common Pitfalls](#common-pitfalls)

---

## Design Principles

### Principle 1: Configure Before Show

**Always set layer shell properties BEFORE showing window!**

```cpp
// ✓ GOOD
QWidget *panel = new QWidget();
auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
layerWindow->setLayer(LayerTop);
layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
panel->show();

// ✗ BAD
QWidget *panel = new QWidget();
panel->show();  // Too late!
auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
layerWindow->setLayer(LayerTop);  // Won't work
```

---

### Principle 2: Use Appropriate Layer

**Choose layer theo use case:**

| Use Case | Layer | Reasoning |
|----------|-------|-----------|
| Panel | TOP | Above apps, exclusive zone |
| Dock | TOP | Above apps, exclusive zone |
| Notification | TOP | Above apps, no exclusive zone |
| Widget | BOTTOM | Below apps, decorative |
| Wallpaper | BACKGROUND | Lowest layer |
| Lock screen | OVERLAY | Block everything |
| Screen keyboard | OVERLAY | Must be accessible always |

**Anti-pattern:**
```cpp
// ✗ DON'T use OVERLAY for normal UI
layerWindow->setLayer(LayerOverlay);  // Too aggressive for a panel!

// ✓ DO use TOP for panels
layerWindow->setLayer(LayerTop);
```

---

### Principle 3: Match Exclusive Zone to Purpose

**Reserve space only when needed:**

```cpp
// ✓ Panel - reserve space
layerWindow->setExclusiveZone(30);

// ✓ Notification - overlay mode, no reservation
layerWindow->setExclusiveZone(0);

// ✓ Auto-calculate
layerWindow->setExclusiveZone(-1);
```

**Rule:** If users should not interact under your window → use exclusive zone.

---

### Principle 4: Minimize Keyboard Grab

**Only use keyboard interactivity when necessary:**

```cpp
// ✓ Wallpaper - no keyboard needed
layerWindow->setKeyboardInteractivity(KeyboardInteractivityNone);

// ✓ Panel with search - needs keyboard
layerWindow->setKeyboardInteractivity(KeyboardInteractivityExclusive);

// ✗ DON'T grab keyboard unnecessarily
// Makes other apps unresponsive!
```

---

## Coding Patterns

### Pattern 1: Base Layer Window Class

**Create reusable base class:**

```cpp
class LayerWindow : public QWidget {
    Q_OBJECT
public:
    LayerWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setupLayerShell();
    }
    
    virtual ~LayerWindow() = default;
    
protected:
    virtual void setupLayerShell() {
        // Default config - override in subclass
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(defaultLayer());
        layerWindow->setAnchors(defaultAnchors());
        layerWindow->setExclusiveZone(defaultExclusiveZone());
        layerWindow->setKeyboardInteractivity(defaultKeyboardMode());
    }
    
    virtual LayerShellQt::Window::Layer defaultLayer() const {
        return LayerShellQt::Window::LayerTop;
    }
    
    virtual LayerShellQt::Window::Anchors defaultAnchors() const {
        return LayerShellQt::Window::AnchorTop | 
               LayerShellQt::Window::AnchorLeft | 
               LayerShellQt::Window::AnchorRight;
    }
    
    virtual int defaultExclusiveZone() const {
        return 0;
    }
    
    virtual LayerShellQt::Window::KeyboardInteractivity defaultKeyboardMode() const {
        return LayerShellQt::Window::KeyboardInteractivityNone;
    }
    
    LayerShellQt::Window* layerWindow() {
        return LayerShellQt::Window::get(windowHandle());
    }
};
```

**Usage:**
```cpp
class MyPanel : public LayerWindow {
protected:
    LayerShellQt::Window::Layer defaultLayer() const override {
        return LayerShellQt::Window::LayerTop;
    }
    
    int defaultExclusiveZone() const override {
        return 35;
    }
};
```

---

### Pattern 2: Configuration Builder

**Fluent interface for configuration:**

```cpp
class LayerWindowBuilder {
public:
    explicit LayerWindowBuilder(QWindow *window) : m_window(window) {
        m_layerWindow = LayerShellQt::Window::get(window);
    }
    
    LayerWindowBuilder& layer(LayerShellQt::Window::Layer layer) {
        m_layerWindow->setLayer(layer);
        return *this;
    }
    
    LayerWindowBuilder& anchors(LayerShellQt::Window::Anchors anchors) {
        m_layerWindow->setAnchors(anchors);
        return *this;
    }
    
    LayerWindowBuilder& exclusiveZone(int zone) {
        m_layerWindow->setExclusiveZone(zone);
        return *this;
    }
    
    LayerWindowBuilder& margins(const QMargins &margins) {
        m_layerWindow->setMargins(margins);
        return *this;
    }
    
    LayerWindowBuilder& keyboard(LayerShellQt::Window::KeyboardInteractivity mode) {
        m_layerWindow->setKeyboardInteractivity(mode);
        return *this;
    }
    
    LayerWindowBuilder& scope(const QString &scope) {
        m_layerWindow->setScope(scope);
        return *this;
    }
    
    LayerShellQt::Window* build() {
        return m_layerWindow;
    }
    
private:
    QWindow *m_window;
    LayerShellQt::Window *m_layerWindow;
};
```

**Usage:**
```cpp
QWidget *panel = new QWidget();

LayerWindowBuilder(panel->windowHandle())
    .layer(LayerShellQt::Window::LayerTop)
    .anchors(LayerShellQt::Window::AnchorTop | 
             LayerShellQt::Window::AnchorLeft | 
             LayerShellQt::Window::AnchorRight)
    .exclusiveZone(30)
    .margins(QMargins(5, 0, 5, 0))
    .keyboard(LayerShellQt::Window::KeyboardInteractivityExclusive)
    .build();

panel->show();
```

---

### Pattern 3: Screen Manager

**Centralized multi-monitor management:**

```cpp
class ScreenManager : public QObject {
    Q_OBJECT
public:
    using WindowFactory = std::function<QWidget*(QScreen*)>;
    
    ScreenManager(WindowFactory factory) : m_factory(factory) {
        // Create windows for existing screens
        for (QScreen *screen : QGuiApplication::screens()) {
            addScreen(screen);
        }
        
        // Watch for screen changes
        connect(qApp, &QGuiApplication::screenAdded, 
                this, &ScreenManager::addScreen);
        connect(qApp, &QGuiApplication::screenRemoved, 
                this, &ScreenManager::removeScreen);
    }
    
    QWidget* windowForScreen(QScreen *screen) const {
        return m_windows.value(screen);
    }
    
private slots:
    void addScreen(QScreen *screen) {
        if (m_windows.contains(screen)) return;
        
        QWidget *window = m_factory(screen);
        m_windows.insert(screen, window);
        window->show();
        
        emit screenAdded(screen, window);
    }
    
    void removeScreen(QScreen *screen) {
        QWidget *window = m_windows.take(screen);
        if (window) {
            window->deleteLater();
            emit screenRemoved(screen);
        }
    }
    
signals:
    void screenAdded(QScreen *screen, QWidget *window);
    void screenRemoved(QScreen *screen);
    
private:
    WindowFactory m_factory;
    QHash<QScreen*, QWidget*> m_windows;
};
```

**Usage:**
```cpp
ScreenManager manager([](QScreen *screen) -> QWidget* {
    QWidget *panel = new QWidget();
    panel->setFixedHeight(30);
    
    auto layerWindow = LayerShellQt::Window::get(panel->windowHandle());
    layerWindow->setLayer(LayerShellQt::Window::LayerTop);
    layerWindow->setAnchors(LayerShellQt::Window::AnchorTop | 
                           LayerShellQt::Window::AnchorLeft | 
                           LayerShellQt::Window::AnchorRight);
    layerWindow->setExclusiveZone(30);
    layerWindow->setScope(screen->name());
    
    return panel;
});
```

---

### Pattern 4: State-Based Layer Switching

**Use state machine for mode changes:**

```cpp
class AdaptivePanel : public QWidget {
    Q_OBJECT
public:
    enum Mode {
        NormalMode,
        ExpandedMode,
        FullscreenMode
    };
    
    void setMode(Mode mode) {
        if (m_currentMode == mode) return;
        
        m_currentMode = mode;
        applyMode();
    }
    
private:
    void applyMode() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        switch (m_currentMode) {
        case NormalMode:
            setFixedHeight(30);
            layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
            layerWindow->setExclusiveZone(30);
            break;
            
        case ExpandedMode:
            setFixedHeight(200);
            layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
            layerWindow->setExclusiveZone(200);
            break;
            
        case FullscreenMode:
            layerWindow->setAnchors(AnchorTop | AnchorBottom | 
                                   AnchorLeft | AnchorRight);
            layerWindow->setExclusiveZone(-1);
            break;
        }
    }
    
    Mode m_currentMode = NormalMode;
};
```

---

## UI/UX Guidelines

### Guideline 1: Visual Hierarchy

**Use layers to establish visual order:**

```
OVERLAY: Critical UI (lock screen, emergency alerts)
    ↓
TOP: Interactive UI (panels, docks, notifications)
    ↓
NORMAL APPS: User applications
    ↓
BOTTOM: Passive UI (widgets, monitors)
    ↓
BACKGROUND: Wallpapers, effects
```

---

### Guideline 2: Respect User Space

**Don't be intrusive:**

```cpp
// ✓ GOOD: Small notification, auto-hide
class PoliteNotification : public QWidget {
public:
    PoliteNotification() {
        setFixedSize(300, 80);  // Reasonable size
        
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(LayerTop);
        layerWindow->setAnchors(AnchorTop | AnchorRight);
        layerWindow->setExclusiveZone(0);  // Don't push apps
        
        // Auto-hide after 5s
        QTimer::singleShot(5000, this, &QWidget::close);
    }
};

// ✗ BAD: Huge, blocks everything
class AnnoyingNotification : public QWidget {
public:
    AnnoyingNotification() {
        // Takes entire screen!
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(LayerOverlay);
        layerWindow->setAnchors(AnchorTop | AnchorBottom | AnchorLeft | AnchorRight);
        layerWindow->setExclusiveZone(-1);
        
        // Never closes!
    }
};
```

---

### Guideline 3: Consistent Sizing

**Panel sizes (industry standard):**
- Small: 24-28px
- Medium: 30-40px
- Large: 44-56px

```cpp
// Follow conventions
static const int PANEL_HEIGHT_SMALL = 28;
static const int PANEL_HEIGHT_MEDIUM = 35;
static const int PANEL_HEIGHT_LARGE = 48;
```

---

### Guideline 4: Responsive to Screen Size

**Adapt to screen dimensions:**

```cpp
class ResponsivePanel : public QWidget {
public:
    ResponsivePanel(QScreen *screen) {
        // Smaller panel on smaller screens
        int screenHeight = screen->geometry().height();
        int panelHeight = screenHeight > 1080 ? 40 : 30;
        
        setFixedHeight(panelHeight);
        
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setExclusiveZone(panelHeight);
    }
};
```

---

### Guideline 5: Accessibility

**Support high DPI, scaling:**

```cpp
class AccessiblePanel : public QWidget {
public:
    AccessiblePanel() {
        // Use logical pixels, not physical
        int logicalHeight = 30;  // DPI-independent
        setFixedHeight(logicalHeight);
        
        // Font sizes scale automatically
        setStyleSheet("font-size: 12pt;");  // Use pt, not px
        
        // Handle DPI changes
        connect(qApp, &QGuiApplication::primaryScreenChanged, 
                this, &AccessiblePanel::onDPIChanged);
    }
};
```

---

## Performance Best Practices

### Practice 1: Minimize Repaints

**Only repaint when necessary:**

```cpp
class EfficientWidget : public QWidget {
protected:
    void updateContent(const QString &newContent) {
        if (m_content == newContent) return;  // No change
        
        m_content = newContent;
        update();  // Only repaint when changed
    }
    
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setClipRegion(event->region());  // Clip to damaged region
        
        // Paint only what's needed
    }
    
private:
    QString m_content;
};
```

---

### Practice 2: Use Double Buffering

**Prevent flicker:**

```cpp
class SmoothWidget : public QWidget {
public:
    SmoothWidget() {
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground);
    }
    
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Qt automatically double-buffers
    }
};
```

---

### Practice 3: Lazy Initialization

**Don't create everything upfront:**

```cpp
class LazyPanel : public QWidget {
public:
    LazyPanel() {
        // Only create visible elements
        setupBasicUI();
    }
    
private:
    void setupBasicUI() {
        // Create only essentials
        m_menuButton = new QPushButton("Menu");
    }
    
    QMenu* getMenu() {
        if (!m_menu) {
            // Create on-demand
            m_menu = new QMenu(this);
            populateMenu();
        }
        return m_menu;
    }
    
    QPushButton *m_menuButton;
    QMenu *m_menu = nullptr;  // Created lazily
};
```

---

### Practice 4: Batch Updates

**Group property changes:**

```cpp
// ✗ BAD: Multiple updates
layerWindow->setLayer(LayerTop);       // Compositor update
layerWindow->setAnchors(anchors);      // Compositor update
layerWindow->setExclusiveZone(30);     // Compositor update

// ✓ GOOD: Qt batches these automatically, but minimize calls
void configure() {
    auto layerWindow = LayerShellQt::Window::get(windowHandle());
    layerWindow->setLayer(LayerTop);
    layerWindow->setAnchors(anchors);
    layerWindow->setExclusiveZone(30);
    // Committed together
}
```

---

## Error Handling

### Pattern 1: Graceful Fallback

**Handle missing layer shell support:**

```cpp
class RobustPanel : public QWidget {
public:
    RobustPanel() {
        if (LayerShellQt::Shell::isAvailable()) {
            setupLayerShell();
        } else {
            setupFallback();
        }
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        layerWindow->setLayer(LayerTop);
        layerWindow->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
    }
    
    void setupFallback() {
        // X11-style window flags
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | 
                      Qt::WindowStaysOnTopHint);
        
        qWarning() << "Layer shell not available, using fallback";
    }
};
```

---

### Pattern 2: Validation

**Validate configuration:**

```cpp
bool validateConfig(LayerShellQt::Window *window) {
    // Check layer
    if (window->layer() < LayerBackground || window->layer() > LayerOverlay) {
        qWarning() << "Invalid layer:" << window->layer();
        return false;
    }
    
    // Check exclusive zone
    if (window->exclusiveZone() < -1) {
        qWarning() << "Invalid exclusive zone:" << window->exclusiveZone();
        return false;
    }
    
    return true;
}
```

---

## Testing Strategies

### Strategy 1: Unit Tests

**Test layer configuration:**

```cpp
class LayerWindowTest : public QObject {
    Q_OBJECT
private slots:
    void testLayerConfiguration() {
        QWidget widget;
        auto layerWindow = LayerShellQt::Window::get(widget.windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        QCOMPARE(layerWindow->layer(), LayerShellQt::Window::LayerTop);
        
        layerWindow->setAnchors(LayerShellQt::Window::AnchorTop);
        QVERIFY(layerWindow->anchors() & LayerShellQt::Window::AnchorTop);
    }
    
    void testExclusiveZone() {
        QWidget widget;
        auto layerWindow = LayerShellQt::Window::get(widget.windowHandle());
        
        layerWindow->setExclusiveZone(30);
        QCOMPARE(layerWindow->exclusiveZone(), 30);
    }
};

QTEST_MAIN(LayerWindowTest)
```

---

### Strategy 2: Integration Tests

**Test with actual compositor:**

```cpp
class IntegrationTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        if (!LayerShellQt::Shell::isAvailable()) {
            QSKIP("Layer shell not available");
        }
    }
    
    void testPanelDisplay() {
        QWidget panel;
        panel.setFixedHeight(30);
        
        auto layerWindow = LayerShellQt::Window::get(panel.windowHandle());
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        panel.show();
        QVERIFY(QTest::qWaitForWindowExposed(&panel));
        
        // Check window is at correct position
        QCOMPARE(panel.y(), 0);  // At top
    }
};
```

---

### Strategy 3: Visual Testing

**Manual visual checks:**

```bash
#!/bin/bash
# visual_test.sh

echo "Testing panel..."
./panel &
PANEL_PID=$!

echo "Check: Panel at top? (y/n)"
read -n 1 response

if [ "$response" != "y" ]; then
    echo "FAIL: Panel not at top"
fi

kill $PANEL_PID
```

---

## Common Pitfalls

### Pitfall 1: Setting Properties After Show

```cpp
// ✗ WRONG
panel->show();
layerWindow->setLayer(LayerTop);  // Too late!

// ✓ CORRECT
layerWindow->setLayer(LayerTop);
panel->show();
```

---

### Pitfall 2: Wrong Layer for Use Case

```cpp
// ✗ WRONG: Wallpaper on TOP layer
layerWindow->setLayer(LayerTop);  // Blocks apps!

// ✓ CORRECT: Wallpaper on BACKGROUND
layerWindow->setLayer(LayerBackground);
```

---

### Pitfall 3: Forgetting Exclusive Zone

```cpp
// ✗ WRONG: Panel without exclusive zone
layerWindow->setLayer(LayerTop);
layerWindow->setExclusiveZone(0);  // Apps overlap panel!

// ✓ CORRECT
layerWindow->setExclusiveZone(30);  // Reserve space
```

---

### Pitfall 4: Not Handling Screen Changes

```cpp
// ✗ WRONG: Static panel, doesn't adapt
class StaticPanel : public QWidget {
    // No screen change handling
};

// ✓ CORRECT: React to changes
class DynamicPanel : public QWidget {
    void init() {
        connect(screen(), &QScreen::geometryChanged, 
                this, &DynamicPanel::onScreenChanged);
    }
};
```

---

### Pitfall 5: Memory Leaks

```cpp
// ✗ WRONG: Leak when screen removed
void createPanel(QScreen *screen) {
    QWidget *panel = new QWidget();  // Never deleted!
    // ...
}

// ✓ CORRECT: Proper cleanup
QHash<QScreen*, QWidget*> panels;

void createPanel(QScreen *screen) {
    QWidget *panel = new QWidget();
    panels.insert(screen, panel);
}

void removePanel(QScreen *screen) {
    QWidget *panel = panels.take(screen);
    if (panel) panel->deleteLater();
}
```

---

## Checklist: Before Production

- [ ] Check `LayerShellQt::Shell::isAvailable()` at startup
- [ ] Set all properties before `show()`
- [ ] Use appropriate layer for use case
- [ ] Match exclusive zone to purpose
- [ ] Handle multi-monitor scenarios
- [ ] Test on different compositors (Plasma, Sway)
- [ ] Provide fallback for unsupported compositors
- [ ] Validate all property values
- [ ] Handle screen add/remove
- [ ] Clean up resources properly
- [ ] Test with different DPI settings
- [ ] Check performance (no excessive repaints)
- [ ] Follow accessibility guidelines
- [ ] Document compositor requirements

---

## Summary: Golden Rules

1. **Configure before show**
2. **Use appropriate layer**
3. **Reserve space wisely**
4. **Minimize keyboard grabs**
5. **Support multi-monitor**
6. **Handle errors gracefully**
7. **Optimize performance**
8. **Test thoroughly**

---

## Next Steps

You're now a LayerShellQt expert! Check:
- **[06-quick-reference.md](./06-quick-reference.md)** - Cheatsheet

Happy coding! 🎉
