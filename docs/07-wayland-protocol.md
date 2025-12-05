# Hiểu Sâu Về Wayland Protocol

## Mục Lục
1. [Wayland là gì?](#wayland-là-gì)
2. [Wayland vs X11](#wayland-vs-x11)
3. [Wayland Architecture](#wayland-architecture)
4. [Wayland Protocol](#wayland-protocol)
5. [Wayland Protocols Extension](#wayland-protocols-extension)
6. [Compositors và Clients](#compositors-và-clients)

---

## Wayland là gì?

### Định Nghĩa

**Wayland** là một protocol (giao thức) cho việc giao tiếp giữa:
- **Wayland compositor** (display server)
- **Wayland clients** (applications)

### Triết Lý Thiết Kế

```
X11 Philosophy (1984):
"Mechanism, not policy"
→ X server provides primitives
→ Window manager adds policy
→ Separation of concerns
→ But: complex, legacy baggage

Wayland Philosophy (2008):
"Every frame is perfect"
→ Compositor is the display server
→ Simple, modern protocol
→ Security by design
→ Clean slate
```

### Lịch Sử

```
Timeline:
2008 - Kristian Høgsberg bắt đầu Wayland project
2010 - First public demo
2012 - Wayland 1.0 protocol
2013 - Weston 1.0 (reference compositor)
2015 - GNOME 3.20 adds Wayland session
2015 - KDE Plasma 5.5 adds Wayland support
2017 - Sway 1.0 (i3 replacement for Wayland)
2024 - Wayland becoming default on major distros
```

---

## Wayland vs X11

### Architectural Differences

#### X11 Architecture

```
┌─────────────────────────────────────────────────┐
│              X11 Architecture                   │
└─────────────────────────────────────────────────┘

┌──────────────┐                    ┌──────────────┐
│              │                    │              │
│  Application │◄───────────────────┤  Application │
│              │                    │              │
└──────┬───────┘                    └──────┬───────┘
       │ X Protocol                        │
       │ (network transparent)             │
       ▼                                   ▼
┌────────────────────────────────────────────────┐
│           X Server (Xorg)                      │
│  - Rendering                                   │
│  - Input handling                              │
│  - Window positioning (basic)                  │
└───────────┬────────────────────────────────────┘
            │
            ▼
┌────────────────────────────────────────────────┐
│         Window Manager (separate process)      │
│  - Window decorations                          │
│  - Window placement                            │
│  - Effects                                     │
└───────────┬────────────────────────────────────┘
            │
            ▼
┌────────────────────────────────────────────────┐
│         Compositor (optional)                  │
│  - Compositing effects                         │
│  - Transparency                                │
└───────────┬────────────────────────────────────┘
            │
            ▼
┌────────────────────────────────────────────────┐
│              Hardware (GPU)                    │
└────────────────────────────────────────────────┘
```

**Issues with X11:**
- Too many components
- Multiple copies of frame data
- No security model (any client can keylog any other)
- Complex, 40+ years of legacy code
- Network transparency rarely used today

#### Wayland Architecture

```
┌─────────────────────────────────────────────────┐
│            Wayland Architecture                 │
└─────────────────────────────────────────────────┘

┌──────────────┐                    ┌──────────────┐
│              │                    │              │
│  Application │◄───────────────────┤  Application │
│  (Wayland    │                    │  (Wayland    │
│   Client)    │                    │   Client)    │
└──────┬───────┘                    └──────┬───────┘
       │ Wayland Protocol                  │
       │ (local socket)                    │
       ▼                                   ▼
┌────────────────────────────────────────────────┐
│      Wayland Compositor                        │
│  (Combines: Display Server + Window Manager    │
│             + Compositor)                      │
│                                                │
│  - Window management                           │
│  - Input handling                              │
│  - Rendering                                   │
│  - Effects                                     │
│  - Everything                                  │
└───────────┬────────────────────────────────────┘
            │
            ▼
┌────────────────────────────────────────────────┐
│              Hardware (GPU)                    │
│  - Direct access via DRM/KMS                   │
└────────────────────────────────────────────────┘
```

**Benefits:**
- Simpler architecture
- Better performance (fewer copies)
- Better security (isolation)
- Modern design
- Less legacy code

### Feature Comparison

| Feature | X11 | Wayland |
|---------|-----|---------|
| **Architecture** | Complex, modular | Simple, unified |
| **Performance** | Good | Better (fewer copies) |
| **Security** | Poor (no isolation) | Good (clients isolated) |
| **Tearing** | Common | Rare (by design) |
| **VRR/FreeSync** | Limited support | Native support |
| **HDR** | No | Yes (in progress) |
| **Mixed DPI** | Problematic | Good |
| **Network transparency** | Yes (built-in) | No (by design, use RDP/VNC) |
| **Screen recording** | Easy | Needs portal/permission |
| **Remote desktop** | Easy (X forwarding) | Needs portal/permission |
| **Maturity** | Very mature (40+ years) | Maturing (10+ years) |
| **Legacy app support** | Native | Via XWayland |

---

## Wayland Architecture

### Core Components

```
┌────────────────────────────────────────────────────┐
│           Wayland System Components                │
├────────────────────────────────────────────────────┤
│                                                    │
│  ┌──────────────────────────────────────────────┐  │
│  │         Wayland Protocol (libwayland)        │  │
│  │  - Core protocol specification               │  │
│  │  - Client library (libwayland-client)        │  │
│  │  - Server library (libwayland-server)        │  │
│  └──────────────────────────────────────────────┘  │
│                                                    │
│  ┌──────────────────────────────────────────────┐  │
│  │         Protocol Extensions                  │  │
│  │  - wayland-protocols (official)              │  │
│  │  - Vendor-specific protocols                 │  │
│  └──────────────────────────────────────────────┘  │
│                                                    │
│  ┌──────────────────────────────────────────────┐  │
│  │         Compositor Implementation            │  │
│  │  - Display server                            │  │
│  │  - Window manager                            │  │
│  │  - Compositor                                │  │
│  │  Examples: Sway, KWin, Mutter, weston       │  │
│  └──────────────────────────────────────────────┘  │
│                                                    │
│  ┌──────────────────────────────────────────────┐  │
│  │         Helper Libraries                     │  │
│  │  - wlroots (compositor library)              │  │
│  │  - libwayland-cursor                         │  │
│  │  - libwayland-egl                            │  │
│  └──────────────────────────────────────────────┘  │
│                                                    │
│  ┌──────────────────────────────────────────────┐  │
│  │         Backend (Kernel)                     │  │
│  │  - DRM/KMS (graphics)                        │  │
│  │  - libinput (input devices)                  │  │
│  │  - evdev (event devices)                     │  │
│  └──────────────────────────────────────────────┘  │
│                                                    │
└────────────────────────────────────────────────────┘
```

### Communication Flow

```
Application (Firefox) wants to display a window:

1. Application creates buffer with window contents
   ├─ Using EGL/OpenGL
   ├─ Or Cairo/software rendering
   └─ Creates wl_buffer object

2. Application sends Wayland protocol messages:
   ├─ wl_surface.attach (buffer)
   ├─ wl_surface.damage (region to update)
   └─ wl_surface.commit (apply changes)

3. Compositor receives messages:
   ├─ Validates request
   ├─ Composites all windows
   └─ Presents to display

4. User input (mouse click):
   ├─ Kernel captures event
   ├─ Compositor receives event
   ├─ Compositor determines which window
   └─ Sends wl_pointer.button to client

5. Application receives input
   └─ Processes and updates buffer
```

### Shared Memory Buffers

```
Buffer Sharing (Zero-Copy):

┌─────────────┐
│ Application │
│ (Client)    │
└─────┬───────┘
      │
      │ 1. Creates shared memory region
      │    shm_open() + ftruncate()
      ▼
┌──────────────────┐
│  Shared Memory   │
│  /dev/shm/...    │
└──────────────────┘
      ▲
      │ 2. Compositor maps same region
      │    No copy needed!
┌─────┴───────┐
│ Compositor  │
└─────────────┘
```

---

## Wayland Protocol

### Protocol Basics

**Wayland protocol** là XML-defined interface:

```xml
<!-- Example: wl_surface -->
<interface name="wl_surface" version="4">
  <description summary="an onscreen surface">
    A surface is a rectangular area that is displayed on the screen.
  </description>

  <request name="attach">
    <description summary="set the surface contents">
      Set a buffer as the content of this surface.
    </description>
    <arg name="buffer" type="object" interface="wl_buffer" allow-null="true"/>
    <arg name="x" type="int"/>
    <arg name="y" type="int"/>
  </request>

  <request name="commit">
    <description summary="commit pending surface state">
      Make pending state effective.
    </description>
  </request>
</interface>
```

### Core Objects

#### 1. wl_display

Root object, represents connection:

```c
// Client side
struct wl_display *display = wl_display_connect(NULL);

// Get registry
struct wl_registry *registry = wl_display_get_registry(display);

// Event loop
wl_display_roundtrip(display);  // Sync
wl_display_dispatch(display);   // Process events
```

#### 2. wl_registry

Global object registry:

```c
// Registry listener
static void registry_handler(void *data,
                             struct wl_registry *registry,
                             uint32_t name,
                             const char *interface,
                             uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = wl_registry_bind(registry, name,
                                     &wl_compositor_interface, 1);
    }
}

static const struct wl_registry_listener registry_listener = {
    registry_handler,
    NULL
};

wl_registry_add_listener(registry, &registry_listener, NULL);
```

#### 3. wl_compositor

Creates surfaces:

```c
struct wl_compositor *compositor;
struct wl_surface *surface;

surface = wl_compositor_create_surface(compositor);
```

#### 4. wl_surface

Represents window content area:

```c
// Attach buffer to surface
wl_surface_attach(surface, buffer, 0, 0);

// Mark damaged region
wl_surface_damage(surface, 0, 0, width, height);

// Commit changes (atomic update)
wl_surface_commit(surface);
```

#### 5. wl_buffer

Pixel data:

```c
// Create shared memory buffer
struct wl_shm_pool *pool;
struct wl_buffer *buffer;

int fd = create_shm_file(size);
void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

pool = wl_shm_create_pool(shm, fd, size);
buffer = wl_shm_pool_create_buffer(pool, 0, width, height,
                                   stride, WL_SHM_FORMAT_XRGB8888);
```

#### 6. wl_seat

Input device group:

```c
// Seat groups keyboard, pointer, touch
struct wl_seat *seat;
struct wl_pointer *pointer;
struct wl_keyboard *keyboard;

pointer = wl_seat_get_pointer(seat);
keyboard = wl_seat_get_keyboard(seat);
```

#### 7. wl_output

Display/monitor:

```c
// Output events
static void output_geometry(void *data,
                           struct wl_output *output,
                           int x, int y,
                           int physical_width,
                           int physical_height,
                           ...) {
    printf("Output at %d,%d size %dx%d\n", x, y, 
           physical_width, physical_height);
}
```

### Example: Minimal Wayland Client

```c
#include <wayland-client.h>
#include <stdio.h>

struct wl_display *display;
struct wl_compositor *compositor;
struct wl_surface *surface;

static void registry_global(void *data,
                           struct wl_registry *registry,
                           uint32_t name,
                           const char *interface,
                           uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = wl_registry_bind(registry, name,
                                     &wl_compositor_interface, 1);
    }
}

static const struct wl_registry_listener registry_listener = {
    registry_global,
    NULL
};

int main() {
    // Connect to display
    display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display\n");
        return 1;
    }
    
    // Get registry
    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    
    // Sync
    wl_display_roundtrip(display);
    
    if (!compositor) {
        fprintf(stderr, "No compositor available\n");
        return 1;
    }
    
    // Create surface
    surface = wl_compositor_create_surface(compositor);
    
    printf("Successfully created Wayland surface!\n");
    
    // Cleanup
    wl_surface_destroy(surface);
    wl_compositor_destroy(compositor);
    wl_display_disconnect(display);
    
    return 0;
}
```

Compile:
```bash
gcc -o wayland-test wayland-test.c $(pkg-config --cflags --libs wayland-client)
```

---

## Wayland Protocols Extension

Core Wayland protocol is minimal. Extensions add functionality:

### Protocol Categories

```
┌────────────────────────────────────────────────┐
│         Wayland Protocol Landscape             │
├────────────────────────────────────────────────┤
│                                                │
│  Core Protocol (libwayland)                    │
│  ├─ wl_display                                 │
│  ├─ wl_compositor                              │
│  ├─ wl_surface                                 │
│  └─ Basic building blocks                      │
│                                                │
│  Stable Extensions (wayland-protocols)         │
│  ├─ xdg-shell (window management)              │
│  ├─ xdg-decoration (server-side decorations)   │
│  ├─ linux-dmabuf (zero-copy buffers)           │
│  └─ presentation-time                          │
│                                                │
│  Staging Extensions                            │
│  ├─ xdg-activation (focus stealing prevention) │
│  ├─ content-type (hint for VRR)                │
│  └─ tearing-control (allow tearing)            │
│                                                │
│  Unstable Extensions                           │
│  ├─ input-method                               │
│  ├─ text-input                                 │
│  └─ tablet support                             │
│                                                │
│  Desktop-Specific                              │
│  ├─ wlr-protocols (wlroots)                    │
│  │  ├─ wlr-layer-shell (panels, notifications) │
│  │  ├─ wlr-screencopy (screen recording)       │
│  │  └─ wlr-output-management                   │
│  │                                              │
│  ├─ KDE protocols                              │
│  │  ├─ org_kde_plasma_shell                    │
│  │  ├─ org_kde_kwin_blur                       │
│  │  └─ org_kde_kwin_server_decoration          │
│  │                                              │
│  └─ GNOME protocols                            │
│     └─ Various GNOME-specific extensions       │
│                                                │
└────────────────────────────────────────────────┘
```

### XDG Shell

Most important extension - window management:

```xml
<!-- xdg_wm_base: Shell interface -->
<interface name="xdg_wm_base" version="3">
  <request name="create_positioner">
    <!-- Create popup positioner -->
  </request>
  
  <request name="get_xdg_surface">
    <!-- Create xdg surface from wl_surface -->
  </request>
</interface>

<!-- xdg_surface: Shell surface -->
<interface name="xdg_surface" version="3">
  <request name="get_toplevel">
    <!-- Regular window -->
  </request>
  
  <request name="get_popup">
    <!-- Popup window -->
  </request>
</interface>

<!-- xdg_toplevel: Top-level window -->
<interface name="xdg_toplevel" version="3">
  <request name="set_title">
    <arg name="title" type="string"/>
  </request>
  
  <request name="set_fullscreen"/>
  <request name="set_maximized"/>
  <request name="set_minimized"/>
  
  <event name="configure">
    <arg name="width" type="int"/>
    <arg name="height" type="int"/>
    <arg name="states" type="array"/>
  </event>
</interface>
```

**Usage:**

```c
// Create XDG surface
struct xdg_wm_base *xdg_wm_base;
struct xdg_surface *xdg_surface;
struct xdg_toplevel *xdg_toplevel;

xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);

// Set properties
xdg_toplevel_set_title(xdg_toplevel, "My Application");
xdg_toplevel_set_app_id(xdg_toplevel, "org.example.myapp");

// Handle configure events
static void xdg_toplevel_configure(void *data,
                                   struct xdg_toplevel *toplevel,
                                   int32_t width,
                                   int32_t height,
                                   struct wl_array *states) {
    // Resize application
}
```

### Layer Shell (wlroots)

For panels, overlays, backgrounds:

```c
// Used by waybar, swaybar, etc.
struct zwlr_layer_shell_v1 *layer_shell;
struct zwlr_layer_surface_v1 *layer_surface;

layer_surface = zwlr_layer_shell_v1_get_layer_surface(
    layer_shell,
    surface,
    output,
    ZWLR_LAYER_SHELL_V1_LAYER_TOP,  // or BOTTOM, BACKGROUND, OVERLAY
    "panel"
);

// Configure layer surface
zwlr_layer_surface_v1_set_size(layer_surface, width, height);
zwlr_layer_surface_v1_set_anchor(layer_surface,
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
    ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
    ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
);
```

### Input Method

For virtual keyboards, IME:

```xml
<interface name="zwp_input_method_v2">
  <request name="commit_string">
    <arg name="text" type="string"/>
  </request>
  
  <event name="activate"/>
  <event name="deactivate"/>
</interface>
```

---

## Compositors và Clients

### Popular Wayland Compositors

```
Wayland Compositors:

Desktop Environments:
├─ Mutter (GNOME)
│  └─ Most polished, feature-complete
├─ KWin (KDE Plasma)
│  └─ Excellent Wayland support, effects
└─ wlroots-based
   ├─ Sway (tiling)
   ├─ River (dynamic tiling)
   ├─ Hyprland (eye candy)
   ├─ Wayfire (3D effects)
   └─ many more...

Specialized:
├─ Weston (reference implementation)
├─ labwc (openbox-like)
├─ hikari (stacking)
└─ cage (kiosk mode)
```

### Toolkit Support

```
GUI Toolkits:

✅ Native Wayland:
├─ GTK 3/4 (GNOME)
├─ Qt 5/6 (KDE)
├─ SDL2
├─ GLFW
└─ EFL

🟡 Via XWayland:
├─ Electron (old versions)
├─ Java/Swing
└─ Old GTK2 apps

❌ No support:
└─ Very old apps (need XWayland)
```

### Finding Wayland Support

```bash
# Check if app uses Wayland
# Method 1: Check protocol
lsof -p $(pgrep firefox) | grep wayland

# Method 2: Check environment
cat /proc/$(pgrep firefox)/environ | tr '\0' '\n' | grep WAYLAND

# Method 3: Force Wayland (if supported)
export MOZ_ENABLE_WAYLAND=1  # Firefox
export QT_QPA_PLATFORM=wayland  # Qt apps
export GDK_BACKEND=wayland  # GTK apps

# Method 4: Check libraries
ldd $(which firefox) | grep wayland
```

---

## Debugging Wayland

### Environment Variables

```bash
# Enable Wayland debug logging
export WAYLAND_DEBUG=1

# Log all protocol messages
# Output: very verbose!

# Run app with debug
WAYLAND_DEBUG=1 gtk4-demo 2>&1 | grep wl_surface

# Filter specific interface
WAYLAND_DEBUG=1 app 2>&1 | grep xdg_toplevel
```

### wayland-scanner

Generate code from protocol XML:

```bash
# Generate client header
wayland-scanner client-header protocol.xml protocol-client.h

# Generate server header
wayland-scanner server-header protocol.xml protocol-server.h

# Generate code
wayland-scanner private-code protocol.xml protocol.c
```

### weston-info

Display Wayland information:

```bash
# Install weston
sudo apt install weston

# Run weston-info
weston-info

# Output:
interface: 'wl_compositor', version: 4, name: 1
interface: 'wl_subcompositor', version: 1, name: 2
interface: 'wl_data_device_manager', version: 3, name: 3
...
```

---

## Kết Luận

### Wayland Benefits

✅ **Modern Architecture**
- Clean design
- No legacy baggage
- Security by default

✅ **Performance**
- Fewer copies
- Better frame pacing
- Native VRR/FreeSync

✅ **Features**
- HDR (coming)
- Better multi-monitor
- Mixed DPI

### Wayland Challenges

❌ **Maturity**
- Still evolving
- Some apps need work
- NVIDIA support improving

❌ **Network Transparency**
- No X11-style forwarding
- Need RDP/VNC/etc.

❌ **Screen Recording**
- Needs permissions
- Not as simple as X11

### Future

Wayland is the future:
- Default on Fedora, Ubuntu (soon)
- Better support every release
- Most apps work great
- Ecosystem maturing rapidly

### Tài Nguyên

- [Wayland Official Site](https://wayland.freedesktop.org/)
- [Wayland Book](https://wayland-book.com/)
- [Wayland Protocols](https://gitlab.freedesktop.org/wayland/wayland-protocols)
- [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots)
