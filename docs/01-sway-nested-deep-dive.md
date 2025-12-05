# Hiểu Sâu Về Sway Nested

## Mục Lục
1. [Sway Compositor là gì?](#sway-compositor-là-gì)
2. [Kiến trúc của Sway](#kiến-trúc-của-sway)
3. [Nested Mode là gì?](#nested-mode-là-gì)
4. [Cách hoạt động của Sway Nested](#cách-hoạt-động-của-sway-nested)
5. [wlroots - Trái tim của Sway](#wlroots---trái-tim-của-sway)
6. [So sánh với X11 Nested](#so-sánh-với-x11-nested)

---

## Sway Compositor là gì?

### Định nghĩa

**Sway** là một **Wayland compositor** và **tiling window manager** được thiết kế để thay thế i3wm trên môi trường Wayland.

### Điểm khác biệt quan trọng

Trong X11:
- **Window Manager** (WM): Chỉ quản lý cửa sổ (vị trí, kích thước, decorations)
- **Display Server** (X.Org): Quản lý input/output, rendering

Trong Wayland:
- **Compositor**: Kết hợp cả hai vai trò trên - vừa là display server vừa là window manager
- Sway đóng vai trò là **compositor** đầy đủ

### Triết lý thiết kế

```
┌─────────────────────────────────────────┐
│         Sway Design Philosophy          │
├─────────────────────────────────────────┤
│ 1. Tiling first                         │
│    - Cửa sổ tự động sắp xếp             │
│    - Không overlap                      │
│                                         │
│ 2. Keyboard-driven                      │
│    - Mọi thao tác qua phím tắt          │
│    - Tốc độ và hiệu quả                 │
│                                         │
│ 3. Minimalist                           │
│    - Chỉ cung cấp core features         │
│    - Nhẹ và nhanh                       │
│                                         │
│ 4. i3-compatible                        │
│    - Config tương thích i3              │
│    - Migration dễ dàng                  │
└─────────────────────────────────────────┘
```

---

## Kiến Trúc Của Sway

### Kiến trúc tổng quan

```
┌─────────────────────────────────────────────────────────────┐
│                      SWAY ARCHITECTURE                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Sway (Main Process)                    │   │
│  │                                                     │   │
│  │  ┌─────────────┐  ┌──────────────┐  ┌───────────┐ │   │
│  │  │   Config    │  │   IPC Server │  │  Command  │ │   │
│  │  │   Parser    │  │   (Socket)   │  │  Handler  │ │   │
│  │  └─────────────┘  └──────────────┘  └───────────┘ │   │
│  │                                                     │   │
│  │  ┌──────────────────────────────────────────────┐  │   │
│  │  │         Tree Management (Container)          │  │   │
│  │  │  - Workspace                                 │  │   │
│  │  │  - Container (split)                         │  │   │
│  │  │  - Window (view)                             │  │   │
│  │  └──────────────────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│                            │                                │
│                            ▼                                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    wlroots                          │   │
│  │  ┌──────────┐  ┌──────────┐  ┌─────────────────┐  │   │
│  │  │  Input   │  │  Output  │  │    Rendering    │  │   │
│  │  │ Handling │  │  Mgmt    │  │    (OpenGL)     │  │   │
│  │  └──────────┘  └──────────┘  └─────────────────┘  │   │
│  │                                                     │   │
│  │  ┌──────────────────────────────────────────────┐  │   │
│  │  │        Wayland Protocol Implementation       │  │   │
│  │  └──────────────────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│                            │                                │
│                            ▼                                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Linux Kernel (DRM/KMS)                 │   │
│  │  ┌────────────┐  ┌────────────┐  ┌──────────────┐  │   │
│  │  │  Graphics  │  │   Input    │  │    Event     │  │   │
│  │  │   (GPU)    │  │  Devices   │  │   System     │  │   │
│  │  └────────────┘  └────────────┘  └──────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│                            │                                │
│                            ▼                                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                Hardware Layer                       │   │
│  │     GPU  │  Monitor  │  Keyboard  │  Mouse          │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### Các thành phần chính

#### 1. Tree Management (Quản lý cây cấu trúc)

Sway sử dụng cấu trúc cây để tổ chức windows:

```
Output (Monitor)
  └── Workspace
      ├── Container (Horizontal Split)
      │   ├── Window A
      │   └── Window B
      └── Container (Vertical Split)
          ├── Window C
          └── Container (Horizontal Split)
              ├── Window D
              └── Window E
```

**Container types:**
- `root`: Gốc của cây
- `output`: Màn hình vật lý
- `workspace`: Không gian làm việc ảo
- `container`: Container chứa split (h/v)
- `view`: Cửa sổ ứng dụng thực tế

#### 2. IPC (Inter-Process Communication)

Sway cung cấp IPC socket để giao tiếp với các tool khác:

```json
// Ví dụ: Query tree
{
  "type": "get_tree"
}

// Response
{
  "id": 1,
  "type": "root",
  "nodes": [
    {
      "id": 2,
      "type": "output",
      "name": "eDP-1",
      "nodes": [
        {
          "id": 3,
          "type": "workspace",
          "name": "1",
          "nodes": [...]
        }
      ]
    }
  ]
}
```

**IPC được sử dụng bởi:**
- `swaymsg`: Command-line tool
- `swaybar`: Status bar
- `waybar`: Alternative status bar
- Custom scripts và tools

#### 3. Config System

Cấu hình Sway bằng file text thuần:

```bash
# ~/.config/sway/config

# Variables
set $mod Mod4
set $term alacritty

# Key bindings
bindsym $mod+Return exec $term
bindsym $mod+d exec dmenu

# Window rules
for_window [app_id="firefox"] move to workspace 2
for_window [title="Picture-in-Picture"] floating enable

# Outputs (monitors)
output HDMI-A-1 resolution 1920x1080 position 0,0
output eDP-1 resolution 1920x1080 position 1920,0

# Input devices
input "2:14:ETPS/2_Elantech_Touchpad" {
    tap enabled
    natural_scroll enabled
}
```

---

## Nested Mode là gì?

### Khái niệm

**Nested mode** là chế độ chạy một compositor/display server bên trong compositor/display server khác.

```
┌────────────────────────────────────────────┐
│     Host Compositor (GNOME/Plasma/X11)     │
│                                            │
│   ┌────────────────────────────────────┐   │
│   │   Nested Sway Instance             │   │
│   │                                    │   │
│   │  ┌──────────┐     ┌──────────┐   │   │
│   │  │  App A   │     │  App B   │   │   │
│   │  └──────────┘     └──────────┘   │   │
│   │                                    │   │
│   └────────────────────────────────────┘   │
│                                            │
└────────────────────────────────────────────┘
```

### Use cases

#### 1. Development & Testing
```bash
# Edit config
vim ~/.config/sway/config

# Test immediately without logout
sway

# If something breaks, just close the window
# Your main session is unaffected
```

#### 2. Debugging
```bash
# Run with debug output
sway -d 2>&1 | tee sway-debug.log

# View all protocol messages
WAYLAND_DEBUG=1 sway
```

#### 3. Demonstrations
- Show Sway features trong presentations
- Record tutorials mà không ảnh hưởng workflow
- So sánh trực tiếp với desktop environment khác

#### 4. Learning
- Học Sway mà không cần commit hoàn toàn
- Thử nghiệm cấu hình mới
- Làm quen với tiling workflow

---

## Cách Hoạt Động Của Sway Nested

### Quy trình khởi động Sway nested

```
┌──────────────────────────────────────────────────────┐
│        Sway Nested Startup Sequence                  │
├──────────────────────────────────────────────────────┤
│                                                      │
│  1. Check Environment                                │
│     ├─ DISPLAY set? → Running under X11             │
│     ├─ WAYLAND_DISPLAY set? → Running under Wayland │
│     └─ Neither? → Error (need nested environment)   │
│                                                      │
│  2. Initialize wlroots                               │
│     ├─ Create Wayland backend                        │
│     ├─ Setup rendering context (EGL/OpenGL)         │
│     └─ Initialize virtual outputs                   │
│                                                      │
│  3. Load Configuration                               │
│     ├─ Parse ~/.config/sway/config                  │
│     ├─ Execute commands                             │
│     └─ Setup keybindings                            │
│                                                      │
│  4. Create Wayland Socket                            │
│     ├─ $XDG_RUNTIME_DIR/wayland-1 (hoặc -2, -3...)  │
│     └─ Set WAYLAND_DISPLAY environment variable     │
│                                                      │
│  5. Start IPC Server                                 │
│     ├─ Create Unix socket                           │
│     └─ $SWAYSOCK=/run/user/1000/sway-ipc.sock       │
│                                                      │
│  6. Initialize Input/Output                          │
│     ├─ Setup keyboard layouts                       │
│     ├─ Configure mouse/touchpad                     │
│     └─ Create virtual outputs for nested windows    │
│                                                      │
│  7. Start Event Loop                                 │
│     └─ Process events from host and clients         │
│                                                      │
└──────────────────────────────────────────────────────┘
```

### Communication flow

```
┌─────────────────────────────────────────────────────────┐
│                Communication in Nested Sway             │
└─────────────────────────────────────────────────────────┘

Host Compositor (GNOME Wayland)
        │
        │ Wayland Protocol
        ▼
┌─────────────────────┐
│  Nested Sway        │
│  (Wayland client    │
│   to host)          │
└─────────────────────┘
        │
        │ Wayland Protocol
        ▼
┌─────────────────────┐
│  Applications       │
│  (firefox, term)    │
│  (Wayland clients   │
│   to Sway)          │
└─────────────────────┘
```

**Hai vai trò của Sway nested:**
1. **Wayland Client** → Nhận input từ host, request rendering buffer
2. **Wayland Server** → Cung cấp display server cho apps bên trong

### Backend system

Sway sử dụng wlroots backends khác nhau:

```c
// Backends trong wlroots

1. DRM Backend (Normal mode - Direct Rendering)
   - Direct access to GPU qua DRM/KMS
   - Used khi chạy từ TTY
   - Hiệu năng cao nhất

2. Wayland Backend (Nested in Wayland)
   - Nested trong Wayland compositor khác
   - Sway window = wl_surface in host
   
3. X11 Backend (Nested in X11)
   - Nested trong X11 server
   - Sway window = X11 window in host

4. Headless Backend (Testing)
   - No actual output
   - Used for CI/testing
```

### Input handling trong nested mode

```
Physical Input (Keyboard press)
        │
        ▼
Host Compositor (captures raw input)
        │
        │ (forwards to focused window)
        ▼
Nested Sway Window (receives Wayland event)
        │
        ▼
Sway Input Handler
        │
        ├─ Check keybindings
        │  └─ Match? Execute command
        │
        └─ No match? Forward to focused app
               │
               ▼
        Application inside Sway
```

**Vấn đề và giải pháp:**

1. **Keybinding conflicts:**
```bash
# Host uses Mod4+Return
# Sway also uses Mod4+Return
# → Host captures first, Sway never sees it

# Solution: Use different modifier in nested
set $mod Mod1  # Alt instead of Super
```

2. **Fullscreen issues:**
```bash
# Nested Sway can't go "real" fullscreen
# → Only fullscreen within its window

# Solution: Maximize nested window in host
```

---

## wlroots - Trái Tim Của Sway

### Giới thiệu wlroots

**wlroots** là một thư viện để xây dựng Wayland compositors. Nó cung cấp:
- Backend abstraction (DRM, Wayland, X11)
- Input device handling
- Output management
- Rendering infrastructure
- Protocol implementations

### Kiến trúc wlroots

```
┌─────────────────────────────────────────────┐
│               wlroots Library               │
├─────────────────────────────────────────────┤
│                                             │
│  High-level Abstractions                   │
│  ┌─────────────────────────────────────┐   │
│  │  wlr_output, wlr_input_device,      │   │
│  │  wlr_surface, wlr_scene             │   │
│  └─────────────────────────────────────┘   │
│                                             │
│  Protocol Support                           │
│  ┌─────────────────────────────────────┐   │
│  │  xdg-shell, layer-shell,            │   │
│  │  xwayland, input-method, etc.       │   │
│  └─────────────────────────────────────┘   │
│                                             │
│  Rendering                                  │
│  ┌─────────────────────────────────────┐   │
│  │  wlr_renderer (OpenGL/Vulkan)       │   │
│  │  wlr_allocator (buffer allocation)  │   │
│  └─────────────────────────────────────┘   │
│                                             │
│  Backends                                   │
│  ┌─────────────────────────────────────┐   │
│  │  DRM, Wayland, X11, Headless        │   │
│  └─────────────────────────────────────┘   │
│                                             │
└─────────────────────────────────────────────┘
```

### Các compositor sử dụng wlroots

```
wlroots ecosystem:
├── Sway (Tiling WM)
├── Wayfire (3D compositing)
├── River (Dynamic tiling)
├── Hyprland (Eye candy + tiling)
├── dwl (dwm for Wayland)
└── nhiều compositors khác...
```

### Ví dụ code với wlroots

```c
// Minimal compositor với wlroots
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>

struct my_server {
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_compositor *compositor;
};

int main(void) {
    struct my_server server;
    
    // Create Wayland display
    server.wl_display = wl_display_create();
    
    // Create backend (auto-detects: DRM, Wayland, or X11)
    server.backend = wlr_backend_autocreate(server.wl_display);
    
    // Create renderer
    server.renderer = wlr_renderer_autocreate(server.backend);
    
    // Create compositor
    server.compositor = wlr_compositor_create(
        server.wl_display,
        server.renderer
    );
    
    // Add Wayland socket
    const char *socket = wl_display_add_socket_auto(server.wl_display);
    setenv("WAYLAND_DISPLAY", socket, 1);
    
    // Start backend
    wlr_backend_start(server.backend);
    
    // Run event loop
    wl_display_run(server.wl_display);
    
    // Cleanup
    wl_display_destroy(server.wl_display);
    return 0;
}
```

---

## So Sánh Với X11 Nested

### X11 Nested (Xephyr/Xnest)

```
┌────────────────────────────────┐
│     Host X11 Server            │
│                                │
│   ┌────────────────────────┐   │
│   │   Xephyr               │   │
│   │   (Nested X Server)    │   │
│   │                        │   │
│   │  ┌──────┐  ┌────────┐ │   │
│   │  │ App  │  │  App   │ │   │
│   │  └──────┘  └────────┘ │   │
│   └────────────────────────┘   │
└────────────────────────────────┘

Example:
Xephyr -br -ac -noreset -screen 1920x1080 :1 &
DISPLAY=:1 i3
```

### Sway Nested

```
┌────────────────────────────────┐
│  Host Wayland Compositor       │
│                                │
│   ┌────────────────────────┐   │
│   │   Sway                 │   │
│   │   (Nested Compositor)  │   │
│   │                        │   │
│   │  ┌──────┐  ┌────────┐ │   │
│   │  │ App  │  │  App   │ │   │
│   │  └──────┘  └────────┘ │   │
│   └────────────────────────┘   │
└────────────────────────────────┘

Example:
sway  # Just run it!
```

### So sánh chi tiết

| Aspect | X11 Nested (Xephyr) | Sway Nested |
|--------|---------------------|-------------|
| **Setup** | Phức tạp, cần specify display | Đơn giản, tự động detect |
| **Performance** | Kém (X11 overhead + network stack) | Tốt (shared memory) |
| **Security** | Yếu (X11 security model) | Mạnh (Wayland isolation) |
| **Integration** | Cần X forwarding | Native integration |
| **Resource** | Cao (separate X server) | Thấp (shared resources) |
| **Display** | Fixed resolution | Resizable window |
| **Input** | Có thể bị lag | Smooth |

### Ưu điểm của Sway Nested

1. **Tự động backend selection:**
```bash
# Không cần chỉ định gì
sway  # Tự động dùng Wayland backend nếu trong Wayland
      # Hoặc X11 backend nếu trong X11
```

2. **Better resource sharing:**
```
Xephyr: Separate GPU context, separate memory
Sway:   Shared GPU context, shared buffers
```

3. **Modern protocols:**
```
X11: Legacy protocols from 1980s
Wayland: Modern, designed for today's hardware
```

4. **Security:**
```
X11: Any app can keylog any other app
Wayland: Apps are isolated
```

---

## Kết Luận

### Tại sao học Sway nested?

1. **Practical**: Development và testing workflow
2. **Educational**: Hiểu compositor architecture
3. **Safe**: Thử nghiệm mà không risk
4. **Modern**: Học công nghệ tương lai (Wayland)

### Next steps

1. Đọc [03-install-sway-nested.md](./03-install-sway-nested.md) để cài đặt
2. Đọc [05-operate-sway.md](./05-operate-sway.md) để học vận hành
3. Thực hành với [08-practical-examples.md](./08-practical-examples.md)
4. Đọc [wlroots documentation](https://gitlab.freedesktop.org/wlroots/wlroots) để hiểu sâu hơn

### Tài nguyên tham khảo

- [Sway GitHub](https://github.com/swaywm/sway)
- [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots)
- [Wayland Book](https://wayland-book.com/)
- [Sway Wiki](https://github.com/swaywm/sway/wiki)
