# LayerShellQt - Core Concepts Deep Dive

## Mục Lục
1. [Wayland Architecture](#wayland-architecture)
2. [wlr-layer-shell Protocol](#wlr-layer-shell-protocol)
3. [Layer System Chi Tiết](#layer-system-chi-tiết)
4. [Anchor System Chi Tiết](#anchor-system-chi-tiết)
5. [Exclusive Zone Deep Dive](#exclusive-zone-deep-dive)
6. [Surface Lifetime & Scope](#surface-lifetime--scope)

---

## Wayland Architecture

### Wayland vs X11

**X11 Model (Old):**
```
Application → X Server (central authority) → Display
              ↑ manages everything:
              - Window positioning
              - Compositing
              - Input routing
```

**Wayland Model (Modern):**
```
Application → Compositor (direct) → Display
              ↑ compositor decides:
              - Window layout
              - Rendering
              - Input
```

### Vấn Đề với Normal Wayland Windows

Một **normal Wayland window** (`xdg-toplevel`) không cho phép:
- ✗ Chỉ định vị trí chính xác (compositor quyết định)
- ✗ Luôn ở trên cùng (no z-order control)
- ✗ Không cho phép các app khác che (no always-on-top)
- ✗ Reserve screen space

→ **Không thể tạo panels, docks, wallpapers!**

### Giải Pháp: Layer Shell Protocol

**wlr-layer-shell** là một Wayland protocol extension cho phép:
- ✓ Chỉ định layer (background → overlay)
- ✓ Anchor vào screen edges
- ✓ Reserve exclusive zones
- ✓ Control keyboard interactivity

---

## wlr-layer-shell Protocol

### Protocol Basics

**Protocol file:** `wlr-layer-shell-unstable-v1.xml`

Định nghĩa 2 interfaces chính:
1. **`zwlr_layer_shell_v1`**: Global object, tạo layer surfaces
2. **`zwlr_layer_surface_v1`**: Per-surface object, configure properties

### Lifecycle của Layer Surface

```
1. Get zwlr_layer_shell_v1 global
   ↓
2. Create wl_surface (Qt QWindow)
   ↓
3. Call get_layer_surface(wl_surface, layer, namespace, output)
   → Returns zwlr_layer_surface_v1
   ↓
4. Configure surface:
   - set_size(width, height)
   - set_anchor(edges)
   - set_exclusive_zone(pixels)
   - set_margin(top, right, bottom, left)
   - set_keyboard_interactivity(mode)
   ↓
5. Commit wl_surface
   ↓
6. Compositor sends configure event
   ↓
7. Ack configure
   ↓
8. Surface is displayed
```

**LayerShellQt abstracts this!** Bạn chỉ cần:
```cpp
auto window = LayerShellQt::Window::get(qwindow);
window->setLayer(...);
window->setAnchors(...);
// Qt handles commit, configure, ack
```

---

## Layer System Chi Tiết

### 4 Layers

```
Stack Order (bottom → top):
┌─────────────────────────────────────────┐
│ 4. OVERLAY (zwlr_layer_shell_v1_layer_overlay)
│    - Screen keyboards
│    - Lock screens  
│    - Critical system UI
│    - Always above everything
├─────────────────────────────────────────┤
│ 3. TOP (zwlr_layer_shell_v1_layer_top)
│    - Panels (taskbar, menu bar)
│    - Docks
│    - Notifications
│    - Above normal apps
├─────────────────────────────────────────┤
│ 2. NORMAL WINDOWS (not layer shell)
│    - Regular applications
│    - xdg-toplevel surfaces
├─────────────────────────────────────────┤
│ 1. BOTTOM (zwlr_layer_shell_v1_layer_bottom)
│    - Desktop widgets
│    - Conky-style monitors
│    - Below normal apps
├─────────────────────────────────────────┤
│ 0. BACKGROUND (zwlr_layer_shell_v1_layer_background)
│    - Wallpapers
│    - Desktop background
└─────────────────────────────────────────┘
```

### Layer Behavior

**BACKGROUND:**
- Luôn dưới cùng
- Normal windows cover nó
- Dùng cho: Wallpaper, background effects
- Exclusive zone: Thường không dùng

**BOTTOM:**
- Dưới normal windows, trên background
- Normal windows có thể che
- Dùng cho: Desktop widgets, info displays
- Exclusive zone: Thường không dùng

**TOP:**
- Trên normal windows
- Normal windows KHÔNG thể che
- Dùng cho: Panels, docks, tray
- Exclusive zone: Thường dùng (reserve space)

**OVERLAY:**
- Trên tất cả (kể cả TOP)
- Dùng cho: Lock screens, critical UI, OSK
- Exclusive zone: Thường không dùng

### Ví Dụ Stack

```
Screen state with multiple layer surfaces:

┌───────────────────────────────────────┐
│ [Lock Screen] (OVERLAY)              │ ← User bị lock
├───────────────────────────────────────┤
│ [Top Panel: Menu, Clock] (TOP)       │ ← Panel
├───────────────────────────────────────┤
│                                       │
│  [Firefox]  [Terminal]  (NORMAL)     │ ← Apps
│                                       │
├───────────────────────────────────────┤
│ [CPU Widget] (BOTTOM)                 │ ← Widget
├───────────────────────────────────────┤
│ [Animated Wallpaper] (BACKGROUND)     │ ← Background
└───────────────────────────────────────┘
```

---

## Anchor System Chi Tiết

### Anchor Bits

Anchors là **bitmask**:

```cpp
enum Anchor {
    AnchorTop    = 1,  // 0b0001
    AnchorBottom = 2,  // 0b0010
    AnchorLeft   = 4,  // 0b0100
    AnchorRight  = 8   // 0b1000
};
```

Combine bằng `|` operator:
```cpp
window->setAnchors(AnchorTop | AnchorLeft);  // 0b0101
```

### Single Edge Anchors

**AnchorTop:**
```
┌─────────────────────────────────┐
│ ████████████████████████████████│ ← Window kéo dài toàn bộ width
│                                 │
│                                 │
│                                 │
└─────────────────────────────────┘
```
- Window gắn vào top edge
- Chiều ngang = screen width (trừ margins)
- Chiều dọc = `setSize()` height

**AnchorLeft:**
```
┌─────────────────────────────────┐
│█                                │
│█                                │
│█                                │ ← Window kéo dài toàn bộ height
│█                                │
└─────────────────────────────────┘
```

### Corner Anchors

**AnchorTop | AnchorLeft:**
```
┌─────────────────────────────────┐
│ ███████                         │ ← Fixed size window at corner
│ ███████                         │
│                                 │
│                                 │
└─────────────────────────────────┘
```
- Window ở góc top-left
- Size = `setSize(width, height)`

**AnchorTop | AnchorRight:**
```
┌─────────────────────────────────┐
│                         ███████ │
│                         ███████ │
│                                 │
└─────────────────────────────────┘
```

### Opposite Edge Anchors

**AnchorTop | AnchorBottom:**
```
┌─────────────────────────────────┐
│ █                               │
│ █                               │ ← Window kéo dài toàn bộ height
│ █                               │
│ █                               │
└─────────────────────────────────┘
```
- Window kéo dài từ top → bottom
- Chiều ngang = `setSize()` width
- Chiều dọc = screen height

**AnchorLeft | AnchorRight:**
```
┌─────────────────────────────────┐
│ ████████████████████████████████│
│                                 │
│                                 │
└─────────────────────────────────┘
```
- Window kéo dài từ left → right
- Chiều ngang = screen width
- Chiều dọc = `setSize()` height

### Full Screen Anchor

**AnchorTop | AnchorBottom | AnchorLeft | AnchorRight:**
```
┌─────────────────────────────────┐
│ ████████████████████████████████│
│ ████████████████████████████████│
│ ████████████████████████████████│
│ ████████████████████████████████│
└─────────────────────────────────┘
```
- Window phủ toàn màn hình
- Ignore `setSize()`

### No Anchor (0)

```
┌─────────────────────────────────┐
│                                 │
│         ████████                │ ← Centered
│         ████████                │
│                                 │
└─────────────────────────────────┘
```
- Window centered
- Size = `setSize(width, height)`

---

## Exclusive Zone Deep Dive

### Khái Niệm

**Exclusive zone** = vùng screen space mà compositor **reserve** cho layer surface và **không cho normal windows vào**.

### Cách Tính

**Giá trị:**
- `exclusiveZone > 0`: Kích thước pixels (tính từ anchor edge)
- `exclusiveZone = 0`: Không reserve (normal windows có thể overlap)
- `exclusiveZone = -1`: Auto (compositor tính từ window size)

### Ví Dụ: Top Panel

```cpp
window->setLayer(LayerTop);
window->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
window->setSize(QSize(0, 30));  // Height = 30px
window->setExclusiveZone(30);   // Reserve 30px from top
```

**Kết quả:**
```
┌─────────────────────────────────┐
│ Top Panel (30px)                │ ← Exclusive zone
├─────────────────────────────────┤ ← Normal windows start here
│                                 │
│  Firefox (auto-positioned       │
│   by compositor below panel)    │
│                                 │
└─────────────────────────────────┘
```

### Ví Dụ: Bottom Dock

```cpp
window->setLayer(LayerTop);
window->setAnchors(AnchorBottom | AnchorLeft | AnchorRight);
window->setSize(QSize(0, 60));
window->setExclusiveZone(60);
```

**Kết quả:**
```
┌─────────────────────────────────┐
│                                 │
│  Normal apps (avoid bottom 60px)│
│                                 │
├─────────────────────────────────┤
│ Bottom Dock (60px)              │ ← Exclusive zone
└─────────────────────────────────┘
```

### Exclusive Zone = 0 (Overlay Mode)

```cpp
window->setLayer(LayerTop);
window->setAnchors(AnchorTop | AnchorRight);
window->setSize(QSize(300, 200));
window->setExclusiveZone(0);  // No reservation
```

**Kết quả:**
```
┌─────────────────────────────────┐
│                   ┌─────────────┤
│  Normal window    │ Notification│ ← Overlap OK
│  (can go under    │ (Exclusive  │
│   notification)   │  zone = 0)  │
│                   └─────────────┤
└─────────────────────────────────┘
```

### Multiple Exclusive Zones

```
Screen with top panel + bottom dock:

┌─────────────────────────────────┐
│ Top Panel (exclusive: 30px)     │ ← Reserve from top
├─────────────────────────────────┤
│                                 │
│  Normal Windows                 │ ← Between exclusions
│  (workarea = screen - 30 - 60)  │
│                                 │
├─────────────────────────────────┤
│ Bottom Dock (exclusive: 60px)   │ ← Reserve from bottom
└─────────────────────────────────┘
```

**Compositor automatically calculates workarea!**

---

## Surface Lifetime & Scope

### Scope Concept

**Scope** = màn hình mà layer surface hiển thị.

```cpp
window->setScope(const QString &scope);
```

**Giá trị:**
- `""` (empty): Tất cả outputs (multi-monitor)
- `"output-name"`: Specific output (e.g., "HDMI-A-1")

### Multi-Monitor Behavior

**Scope = "" (default):**
```
Monitor 1          Monitor 2
┌──────────────┐   ┌──────────────┐
│ Panel        │   │ Panel        │ ← Same surface spans both
├──────────────┤   ├──────────────┤
│              │   │              │
└──────────────┘   └──────────────┘
```

**Scope = "eDP-1":**
```
Monitor 1 (eDP-1)  Monitor 2 (HDMI-A-1)
┌──────────────┐   ┌──────────────┐
│ Panel        │   │ (no panel)   │ ← Only on eDP-1
├──────────────┤   │              │
│              │   │              │
└──────────────┘   └──────────────┘
```

### Per-Output Instances

Để tạo panel riêng cho mỗi màn hình:

```cpp
// Detect outputs
QList<QScreen*> screens = QGuiApplication::screens();

for (QScreen *screen : screens) {
    QWidget *panel = createPanel();
    
    auto window = LayerShellQt::Window::get(panel->windowHandle());
    window->setScope(screen->name());  // Per-screen instance
    
    panel->show();
}
```

**Kết quả:**
```
Monitor 1          Monitor 2
┌──────────────┐   ┌──────────────┐
│ Panel 1      │   │ Panel 2      │ ← Separate instances
├──────────────┤   ├──────────────┤
│              │   │              │
└──────────────┘   └──────────────┘
```

---

## Keyboard Interactivity

### 3 Modes

**KeyboardInteractivityNone:**
- Surface không nhận keyboard input
- Dùng cho: Wallpapers, passive indicators
```cpp
window->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
```

**KeyboardInteractivityExclusive:**
- Surface nhận keyboard khi focus
- Compositor cho layer surface focus
- Dùng cho: Panels với search box, docks với text input
```cpp
window->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityExclusive);
```

**KeyboardInteractivityOnDemand (Qt 5.24+):**
- Surface yêu cầu focus khi cần (on-demand)
- Compositor quyết định
- Best practice cho modern apps

### Ví Dụ

**Panel với Search Box:**
```cpp
// Panel with text input
window->setLayer(LayerTop);
window->setKeyboardInteractivity(KeyboardInteractivityExclusive);

// Khi user click vào search box → keyboard input works
// Khi user click app → panel loses focus
```

**Wallpaper:**
```cpp
// No keyboard needed
window->setLayer(LayerBackground);
window->setKeyboardInteractivity(KeyboardInteractivityNone);
```

---

## Size & Sizing Rules

### Sizing với Anchors

**Rule 1:** Nếu anchor 2 cạnh đối diện → ignore size trục đó

```cpp
// Anchor top + bottom → ignore height
window->setAnchors(AnchorTop | AnchorBottom);
window->setSize(100, 500);  // Width = 100, height = screen height (500 ignored)
```

**Rule 2:** Nếu anchor 1 cạnh → kéo dài trục vuông góc

```cpp
// Anchor top → stretch horizontally
window->setAnchors(AnchorTop);
window->setSize(123, 30);  // Width = screen width (123 ignored), height = 30
```

**Rule 3:** Anchor corner → use explicit size

```cpp
// Corner → fixed size
window->setAnchors(AnchorTop | AnchorLeft);
window->setSize(300, 200);  // Width = 300, height = 200
```

**Rule 4:** No anchor → centered, use explicit size

```cpp
window->setAnchors(0);
window->setSize(400, 300);  // Centered, 400x300
```

### Dynamic Resizing

Layer surfaces có thể resize:

```cpp
// Resize existing surface
window->setSize(QSize(newWidth, newHeight));
// Compositor reconfigures
```

**Use case:** Panel thay đổi chiều cao khi expand/collapse

---

## Margins

### Margin Values

```cpp
struct {
    int top;
    int right;
    int bottom;
    int left;
};

window->setMargins({10, 20, 10, 20});
```

### Effect của Margins

**Top Panel với Margin:**
```cpp
window->setAnchors(AnchorTop | AnchorLeft | AnchorRight);
window->setMargins({0, 10, 0, 10});  // right=10, left=10
```

**Kết quả:**
```
┌─────────────────────────────────┐
│ (10px) ███████████████ (10px)   │ ← Panel không chạm edges
│                                 │
└─────────────────────────────────┘
```

**Corner Widget với Margin:**
```cpp
window->setAnchors(AnchorTop | AnchorRight);
window->setMargins({10, 10, 0, 0});  // top=10, right=10
window->setSize(200, 100);
```

**Kết quả:**
```
┌─────────────────────────────────┐
│                    (10px)       │
│                  ┌──────────┐   │ ← 10px from top & right
│                  │ Widget   │   │
│                  └──────────┘   │
└─────────────────────────────────┘
```

---

## Summary: Property Interactions

```
Layer → Z-order trong compositor stack
  ↓
Anchors → Position & stretching behavior
  ↓
Size → Dimensions (respecting anchor rules)
  ↓
Margins → Offset from anchor edges
  ↓
Exclusive Zone → Reserve space from workarea
  ↓
Keyboard Interactivity → Input handling
  ↓
Scope → Output/monitor selection
```

**Order of Operations:**
1. Set layer (decide z-order)
2. Set anchors (decide layout)
3. Set size (decide dimensions)
4. Set margins (fine-tune position)
5. Set exclusive zone (reserve space)
6. Set keyboard mode (input handling)
7. Set scope (output selection)
8. Show window → Compositor configure

---

## Next Steps

Bây giờ bạn hiểu **core concepts**, tiếp theo:
- **[02-api-reference.md](./02-api-reference.md)** - API đầy đủ với function signatures
- **[03-examples.md](./03-examples.md)** - Code examples thực tế

Happy learning! 🚀
