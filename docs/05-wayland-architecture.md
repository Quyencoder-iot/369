# Kiến Trúc Wayland - Hiểu Sâu về Protocol

## Mục lục
1. [Wayland là gì?](#wayland-là-gì)
2. [So sánh với X11](#so-sánh-với-x11)
3. [Kiến trúc Wayland](#kiến-trúc-wayland)
4. [Wayland Protocol](#wayland-protocol)
5. [wlroots Library](#wlroots-library)
6. [Nested Compositing Technical](#nested-compositing-technical)
7. [Extensions và Protocols](#extensions-và-protocols)
8. [Debugging Wayland](#debugging-wayland)

---

## Wayland là gì?

### Định nghĩa
**Wayland** là một display server protocol và reference implementation thay thế X Window System (X11). Nó đơn giản hóa kiến trúc bằng cách gộp display server, window manager, và compositor thành một.

### Lịch sử

```
Timeline
─────────────────────────────────────────────────────────────────────────
1984        2008         2012          2020           2024
 │           │            │             │              │
 ▼           ▼            ▼             ▼              ▼
X11       Wayland      Wayland      Wayland       Plasma 6
Release   Started       1.0        mainstream     Wayland
                                    adoption      default
─────────────────────────────────────────────────────────────────────────
```

---

## So sánh với X11

### Architectural Differences

```
┌─────────────────────────────────────────────────────────────────────┐
│                         X11 Architecture                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌─────────────────────────────────────────────────────────┐       │
│   │                     X Clients (Apps)                     │       │
│   │   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐   │       │
│   │   │  App 1  │  │  App 2  │  │  App 3  │  │  App 4  │   │       │
│   │   └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘   │       │
│   └────────┼────────────┼────────────┼────────────┼────────┘       │
│            │            │            │            │                  │
│            └────────────┴─────┬──────┴────────────┘                  │
│                               │                                      │
│                               ▼                                      │
│   ┌──────────────────────────────────────────────────────────┐      │
│   │                      X Server                             │      │
│   │  • Input handling     • Window management (basic)         │      │
│   │  • Drawing primitives • Font rendering                    │      │
│   │  • Event routing      • Extensions (Xrandr, Xinerama...)  │      │
│   └─────────────────────────────┬────────────────────────────┘      │
│                                 │                                    │
│                    ┌────────────┼────────────┐                      │
│                    │            │            │                      │
│                    ▼            ▼            ▼                      │
│   ┌────────────────────┐ ┌───────────┐ ┌────────────────────┐      │
│   │   Window Manager   │ │ Compositor│ │    Input Driver    │      │
│   │  (i3, openbox...)  │ │  (picom)  │ │  (libinput/evdev)  │      │
│   └────────────────────┘ └───────────┘ └────────────────────┘      │
│                                                                      │
│   Problems:                                                          │
│   ❌ Everything goes through X Server (bottleneck)                   │
│   ❌ Apps can snoop on each other (security)                         │
│   ❌ Complex protocol with legacy cruft                              │
│   ❌ Compositor is separate → potential tearing                      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                       Wayland Architecture                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌─────────────────────────────────────────────────────────┐       │
│   │                  Wayland Clients (Apps)                  │       │
│   │   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐   │       │
│   │   │  App 1  │  │  App 2  │  │  App 3  │  │ XWayland│   │       │
│   │   │(native) │  │(native) │  │(native) │  │ (X11)   │   │       │
│   │   └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘   │       │
│   └────────┼────────────┼────────────┼────────────┼────────┘       │
│            │            │            │            │                  │
│            │            │            │    ┌───────┘                  │
│            │            │            │    │                          │
│            └────────────┴─────┬──────┴────┤                          │
│                               │           │                          │
│                               ▼           ▼                          │
│   ┌───────────────────────────────────────────────────────────┐     │
│   │              Wayland Compositor (All-in-One)               │     │
│   │                                                            │     │
│   │   ┌────────────────────────────────────────────────────┐  │     │
│   │   │  • Display Server  • Window Manager  • Compositor  │  │     │
│   │   │  • Input Handler   • Output Manager                │  │     │
│   │   └────────────────────────────────────────────────────┘  │     │
│   │                                                            │     │
│   │   Examples: Sway, KWin, Mutter, Weston                    │     │
│   └───────────────────────────────────────────────────────────┘     │
│                               │                                      │
│                               ▼                                      │
│   ┌───────────────────────────────────────────────────────────┐     │
│   │                     Linux Kernel                           │     │
│   │   • DRM/KMS (Display)  • evdev/libinput (Input)           │     │
│   │   • GPU Driver         • Buffer management                 │     │
│   └───────────────────────────────────────────────────────────┘     │
│                                                                      │
│   Benefits:                                                          │
│   ✅ Direct client-compositor communication                          │
│   ✅ Apps isolated from each other (security)                        │
│   ✅ Simpler, modern protocol                                        │
│   ✅ Built-in compositing → no tearing                               │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Kiến trúc Wayland

### Core Concepts

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Wayland Core Concepts                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                        OBJECTS                               │    │
│  │  Wayland uses object-oriented protocol                       │    │
│  │                                                              │    │
│  │  wl_display ─► Root object, represents connection            │    │
│  │       │                                                      │    │
│  │       ├── wl_registry ─► Lists available interfaces          │    │
│  │       │       │                                              │    │
│  │       │       ├── wl_compositor ─► Creates surfaces          │    │
│  │       │       ├── wl_shm ─► Shared memory buffers            │    │
│  │       │       ├── wl_seat ─► Input devices                   │    │
│  │       │       ├── wl_output ─► Display outputs               │    │
│  │       │       └── xdg_wm_base ─► Window management           │    │
│  │       │                                                      │    │
│  │       └── wl_surface ─► A rectangular area                   │    │
│  │               │                                              │    │
│  │               ├── wl_buffer ─► Pixel data                    │    │
│  │               ├── wl_callback ─► Frame timing                │    │
│  │               └── wl_subsurface ─► Child surfaces            │    │
│  │                                                              │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                       REQUESTS & EVENTS                      │    │
│  │                                                              │    │
│  │  Client ──── Requests ────► Compositor                       │    │
│  │         (create surface, attach buffer, commit)              │    │
│  │                                                              │    │
│  │  Client ◄──── Events ───── Compositor                        │    │
│  │         (configure, enter/leave, key/pointer events)         │    │
│  │                                                              │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                         SURFACES                             │    │
│  │                                                              │    │
│  │  wl_surface:                                                 │    │
│  │  ┌─────────────────────────────────────────────────┐        │    │
│  │  │                                                  │        │    │
│  │  │   ┌──────────────────────────────────────────┐  │        │    │
│  │  │   │             wl_buffer                     │  │        │    │
│  │  │   │        (pixel data: ARGB, etc)            │  │        │    │
│  │  │   └──────────────────────────────────────────┘  │        │    │
│  │  │                                                  │        │    │
│  │  │   + damage region                               │        │    │
│  │  │   + input region                                │        │    │
│  │  │   + opaque region                               │        │    │
│  │  │   + scale factor                                │        │    │
│  │  │   + transform                                   │        │    │
│  │  │                                                  │        │    │
│  │  └─────────────────────────────────────────────────┘        │    │
│  │                                                              │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Buffer Sharing

```
┌─────────────────────────────────────────────────────────────────────┐
│                      Buffer Sharing Methods                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  1. wl_shm (Shared Memory)                                          │
│  ─────────────────────────                                          │
│     Client                    Compositor                             │
│     ┌─────────┐              ┌─────────┐                            │
│     │  App    │              │  Sway   │                            │
│     │         │              │         │                            │
│     │  ┌────┐ │ ──────────── │ ┌────┐  │                            │
│     │  │shm │ │  shared fd   │ │shm │  │                            │
│     │  │buf │ │ ◄──────────► │ │buf │  │                            │
│     │  └────┘ │              │ └────┘  │                            │
│     └─────────┘              └─────────┘                            │
│     ✅ Simple, works everywhere                                      │
│     ❌ Requires copy to GPU                                          │
│                                                                      │
│  2. linux-dmabuf (DMA Buffer)                                       │
│  ────────────────────────────                                       │
│     Client                    Compositor                             │
│     ┌─────────┐              ┌─────────┐                            │
│     │  App    │              │  Sway   │                            │
│     │         │              │         │                            │
│     │  GPU ───┼──────────────┼─── GPU  │                            │
│     │  buf    │   dmabuf fd  │   (same)│                            │
│     │         │              │         │                            │
│     └─────────┘              └─────────┘                            │
│     ✅ Zero-copy, GPU native                                         │
│     ✅ Best performance                                              │
│     ❌ Requires GPU support                                          │
│                                                                      │
│  3. EGL Wayland (OpenGL)                                            │
│  ───────────────────────                                            │
│     Client renders with OpenGL → EGL creates buffer → shared        │
│     ✅ Good for 3D apps                                              │
│     ✅ Widely supported                                              │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Wayland Protocol

### Protocol Definition (XML)

```xml
<!-- Ví dụ protocol definition từ wayland.xml -->
<interface name="wl_surface" version="6">
  <description summary="an onscreen surface">
    A surface is a rectangular area...
  </description>

  <request name="destroy" type="destructor">
    <description summary="delete surface"/>
  </request>

  <request name="attach">
    <description summary="set the surface contents"/>
    <arg name="buffer" type="object" interface="wl_buffer" allow-null="true"/>
    <arg name="x" type="int"/>
    <arg name="y" type="int"/>
  </request>

  <request name="commit">
    <description summary="commit pending surface state"/>
  </request>

  <event name="enter">
    <description summary="surface enters an output"/>
    <arg name="output" type="object" interface="wl_output"/>
  </event>

  <event name="leave">
    <description summary="surface leaves an output"/>
    <arg name="output" type="object" interface="wl_output"/>
  </event>
</interface>
```

### Protocol Flow Example

```
┌─────────────────────────────────────────────────────────────────────┐
│                 Typical Client-Compositor Flow                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  Client                                          Compositor          │
│     │                                                 │              │
│     │  1. Connect to Wayland socket                   │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  2. wl_display.get_registry()                   │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  3. registry.global() events                    │              │
│     │ ◄───────────────────────────────────────────── │              │
│     │     (wl_compositor, wl_shm, xdg_wm_base, ...)  │              │
│     │                                                 │              │
│     │  4. registry.bind(wl_compositor, ...)           │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  5. wl_compositor.create_surface()              │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  6. xdg_wm_base.get_xdg_surface(surface)        │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  7. xdg_surface.get_toplevel()                  │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  8. xdg_toplevel.set_title("My App")            │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  9. surface.commit()                            │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  10. xdg_surface.configure(serial)              │              │
│     │ ◄───────────────────────────────────────────── │              │
│     │                                                 │              │
│     │  11. Create buffer, attach, commit               │              │
│     │ ─────────────────────────────────────────────► │              │
│     │                                                 │              │
│     │  12. Frame callback                             │              │
│     │ ◄───────────────────────────────────────────── │              │
│     │                                                 │              │
│     ▼                                                 ▼              │
│   Render loop: attach buffer → commit → wait callback → repeat      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## wlroots Library

### Overview

**wlroots** là một modular Wayland compositor library được phát triển bởi Sway team. Nó cung cấp các building blocks để tạo Wayland compositor.

### Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                      wlroots Architecture                            │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                 Compositor (Sway, Wayfire, etc.)              │   │
│  └─────────────────────────────────┬────────────────────────────┘   │
│                                    │                                 │
│                                    ▼                                 │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                         wlroots                               │   │
│  │                                                               │   │
│  │   ┌─────────────────┐    ┌─────────────────────────────┐     │   │
│  │   │   wlr_backend   │    │      wlr_renderer           │     │   │
│  │   │                 │    │                             │     │   │
│  │   │  ┌───────────┐  │    │  ┌───────────┐  ┌────────┐ │     │   │
│  │   │  │    DRM    │  │    │  │   GLES2   │  │ Vulkan │ │     │   │
│  │   │  │ (native)  │  │    │  └───────────┘  └────────┘ │     │   │
│  │   │  ├───────────┤  │    │  ┌───────────┐             │     │   │
│  │   │  │  Wayland  │  │    │  │  Pixman   │             │     │   │
│  │   │  │ (nested)  │  │    │  │(software) │             │     │   │
│  │   │  ├───────────┤  │    │  └───────────┘             │     │   │
│  │   │  │    X11    │  │    │                             │     │   │
│  │   │  │ (nested)  │  │    │                             │     │   │
│  │   │  ├───────────┤  │    └─────────────────────────────┘     │   │
│  │   │  │ Headless  │  │                                        │   │
│  │   │  └───────────┘  │    ┌─────────────────────────────┐     │   │
│  │   └─────────────────┘    │      wlr_output             │     │   │
│  │                          │  (monitor management)       │     │   │
│  │   ┌─────────────────┐    └─────────────────────────────┘     │   │
│  │   │  wlr_input      │                                        │   │
│  │   │                 │    ┌─────────────────────────────┐     │   │
│  │   │  ┌───────────┐  │    │   Protocol Implementations  │     │   │
│  │   │  │ Keyboard  │  │    │                             │     │   │
│  │   │  ├───────────┤  │    │  • xdg-shell               │     │   │
│  │   │  │  Pointer  │  │    │  • layer-shell             │     │   │
│  │   │  ├───────────┤  │    │  • xdg-decoration          │     │   │
│  │   │  │   Touch   │  │    │  • viewporter              │     │   │
│  │   │  ├───────────┤  │    │  • presentation-time       │     │   │
│  │   │  │  Tablet   │  │    │  • linux-dmabuf            │     │   │
│  │   │  └───────────┘  │    │  • idle-inhibit            │     │   │
│  │   └─────────────────┘    └─────────────────────────────┘     │   │
│  │                                                               │   │
│  │   ┌─────────────────────────────────────────────────────┐    │   │
│  │   │                    XWayland                          │    │   │
│  │   │         (Run X11 apps in Wayland compositor)         │    │   │
│  │   └─────────────────────────────────────────────────────┘    │   │
│  │                                                               │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Backend Selection Flow

```
┌─────────────────────────────────────────────────────────────────────┐
│                    wlroots Backend Selection                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│    Start                                                             │
│      │                                                               │
│      ▼                                                               │
│  ┌───────────────────────────┐                                      │
│  │ Check WLR_BACKENDS env    │                                      │
│  └─────────────┬─────────────┘                                      │
│                │                                                     │
│        ┌───────┴───────┐                                            │
│        │               │                                            │
│        ▼               ▼                                            │
│   Set explicitly    Not set                                         │
│   (user override)   (auto-detect)                                   │
│        │               │                                            │
│        │               ▼                                            │
│        │    ┌─────────────────────┐                                 │
│        │    │ Check WAYLAND_DISPLAY│                                │
│        │    └──────────┬──────────┘                                 │
│        │               │                                            │
│        │        ┌──────┴──────┐                                     │
│        │        │             │                                     │
│        │        ▼             ▼                                     │
│        │     Present        Not present                             │
│        │        │             │                                     │
│        │        ▼             ▼                                     │
│        │   Use Wayland    ┌────────────────┐                        │
│        │     Backend      │ Check $DISPLAY │                        │
│        │                  └───────┬────────┘                        │
│        │                          │                                  │
│        │                   ┌──────┴──────┐                          │
│        │                   │             │                          │
│        │                   ▼             ▼                          │
│        │               Present        Not present                    │
│        │                   │             │                          │
│        │                   ▼             ▼                          │
│        │              Use X11       Use DRM                          │
│        │              Backend       Backend                          │
│        │                            (native)                         │
│        │                                                             │
│        └──────────────────────────────────────────┘                 │
│                                 │                                    │
│                                 ▼                                    │
│                          Initialize                                  │
│                           Backend                                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Nested Compositing Technical

### Wayland Backend Deep Dive

```
┌─────────────────────────────────────────────────────────────────────┐
│               Nested Compositor Technical Details                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  Host Compositor (KWin)                                              │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                                                               │   │
│  │  Wayland Server                                               │   │
│  │  ┌────────────────────────────────────────────────────────┐  │   │
│  │  │  wl_display                                             │  │   │
│  │  │       │                                                 │  │   │
│  │  │       ├── wl_compositor                                 │  │   │
│  │  │       ├── wl_shm / linux-dmabuf                         │  │   │
│  │  │       ├── wl_seat                                       │  │   │
│  │  │       ├── wl_output                                     │  │   │
│  │  │       └── xdg_wm_base                                   │  │   │
│  │  │                                                         │  │   │
│  │  │                           ▲                             │  │   │
│  │  │                           │ Wayland socket              │  │   │
│  │  │                           │ (wayland-0)                 │  │   │
│  │  └───────────────────────────┼────────────────────────────┘  │   │
│  │                              │                                │   │
│  └──────────────────────────────┼────────────────────────────────┘   │
│                                 │                                     │
│                                 ▼                                     │
│  Nested Compositor (Sway)                                            │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                                                               │   │
│  │  wlroots Wayland Backend                                      │   │
│  │  ┌────────────────────────────────────────────────────────┐  │   │
│  │  │                                                         │  │   │
│  │  │  Acts as Wayland CLIENT to host:                        │  │   │
│  │  │  • Creates wl_surface for its "window"                  │  │   │
│  │  │  • Receives input events                                │  │   │
│  │  │  • Renders to buffer, commits                           │  │   │
│  │  │                                                         │  │   │
│  │  │  wl_surface ◄──── Sway's entire viewport                │  │   │
│  │  │      │                                                  │  │   │
│  │  │      └─── xdg_toplevel (window with decorations)        │  │   │
│  │  │                                                         │  │   │
│  │  └────────────────────────────────────────────────────────┘  │   │
│  │                              │                                │   │
│  │                              ▼                                │   │
│  │  Internal Wayland Server (Sway)                               │   │
│  │  ┌────────────────────────────────────────────────────────┐  │   │
│  │  │                                                         │  │   │
│  │  │  Acts as Wayland SERVER to its clients:                 │  │   │
│  │  │  • Has its own wl_display (wayland-1)                   │  │   │
│  │  │  • Clients connect to this socket                       │  │   │
│  │  │  • Composites client buffers                            │  │   │
│  │  │  • Renders final image to parent's buffer               │  │   │
│  │  │                                                         │  │   │
│  │  │  ┌─────────┐  ┌─────────┐  ┌─────────┐                 │  │   │
│  │  │  │  foot   │  │ firefox │  │  nvim   │                 │  │   │
│  │  │  │(client) │  │(client) │  │(client) │                 │  │   │
│  │  │  └─────────┘  └─────────┘  └─────────┘                 │  │   │
│  │  │                                                         │  │   │
│  │  └────────────────────────────────────────────────────────┘  │   │
│  │                                                               │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Data Flow in Nested Mode

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Rendering Pipeline (Nested)                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  1. App Rendering                                                    │
│     ┌──────────────────────────────────────────────────────────┐    │
│     │  foot terminal                                            │    │
│     │  ┌────────────────────────────────────────────────────┐  │    │
│     │  │  GPU renders text/graphics                          │  │    │
│     │  │  → Creates wl_buffer                                 │  │    │
│     │  │  → wl_surface.attach(buffer)                         │  │    │
│     │  │  → wl_surface.commit()                               │  │    │
│     │  └────────────────────────────────────────────────────┘  │    │
│     └──────────────────────────────────────────────────────────┘    │
│                              │                                       │
│                              ▼                                       │
│  2. Sway Compositing                                                 │
│     ┌──────────────────────────────────────────────────────────┐    │
│     │  Sway receives buffer from foot                          │    │
│     │  ┌────────────────────────────────────────────────────┐  │    │
│     │  │  • Place buffer according to tiling layout          │  │    │
│     │  │  • Composite all client buffers                      │  │    │
│     │  │  • Add decorations, borders                          │  │    │
│     │  │  • Render to own wl_buffer                           │  │    │
│     │  │  • Commit to parent compositor (KWin)                │  │    │
│     │  └────────────────────────────────────────────────────┘  │    │
│     └──────────────────────────────────────────────────────────┘    │
│                              │                                       │
│                              ▼                                       │
│  3. KWin Compositing                                                 │
│     ┌──────────────────────────────────────────────────────────┐    │
│     │  KWin receives Sway's buffer as window content           │    │
│     │  ┌────────────────────────────────────────────────────┐  │    │
│     │  │  • Sway appears as just another window              │  │    │
│     │  │  • Apply KWin effects (blur, shadows)               │  │    │
│     │  │  • Composite with other KDE windows                 │  │    │
│     │  │  • Final render to screen                           │  │    │
│     │  └────────────────────────────────────────────────────┘  │    │
│     └──────────────────────────────────────────────────────────┘    │
│                              │                                       │
│                              ▼                                       │
│  4. Display                                                          │
│     ┌──────────────────────────────────────────────────────────┐    │
│     │  DRM/KMS scanout to physical display                     │    │
│     └──────────────────────────────────────────────────────────┘    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Extensions và Protocols

### Common Wayland Protocols

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Important Wayland Protocols                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  CORE PROTOCOLS (wayland.xml)                                        │
│  ─────────────────────────────                                       │
│  • wl_compositor     - Create surfaces                               │
│  • wl_surface        - Rectangular area for content                  │
│  • wl_buffer         - Pixel data container                          │
│  • wl_shm            - Shared memory for buffers                     │
│  • wl_seat           - Input device group (keyboard, pointer)        │
│  • wl_output         - Display output                                │
│                                                                      │
│  WAYLAND-PROTOCOLS (extended functionality)                          │
│  ──────────────────────────────────────────                          │
│  STABLE:                                                             │
│  • xdg-shell         - Window management (toplevel, popup)           │
│  • presentation-time - Frame timing for video playback               │
│  • viewporter        - Cropping and scaling                          │
│  • linux-dmabuf      - Zero-copy GPU buffer sharing                  │
│                                                                      │
│  STAGING:                                                            │
│  • xdg-activation    - Window activation/focus requests              │
│  • xdg-decoration    - Server-side decorations                       │
│  • drm-lease         - VR headset support                            │
│  • single-pixel-buffer - Solid color surfaces                        │
│                                                                      │
│  UNSTABLE:                                                           │
│  • xdg-foreign       - Share surface to another process              │
│  • idle-inhibit      - Prevent screen lock                           │
│  • text-input        - Input method support                          │
│  • input-method      - IME support                                   │
│                                                                      │
│  WLROOTS-SPECIFIC:                                                   │
│  ──────────────────                                                  │
│  • wlr-layer-shell   - Panels, wallpapers, overlays                  │
│  • wlr-screencopy    - Screenshot/recording                          │
│  • wlr-output-management - Monitor configuration                     │
│  • wlr-foreign-toplevel - Task managers                              │
│                                                                      │
│  KDE-SPECIFIC:                                                       │
│  ──────────────                                                      │
│  • plasma-shell      - Plasma desktop integration                    │
│  • plasma-window-management - KWin window control                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Layer Shell Protocol

```
┌─────────────────────────────────────────────────────────────────────┐
│                      wlr-layer-shell                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  Layers (từ dưới lên trên):                                         │
│                                                                      │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                     OVERLAY LAYER                              │  │
│  │        (notifications, OSD, critical alerts)                   │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │                      TOP LAYER                                 │  │
│  │           (panels, docks, status bars)                         │  │
│  │                                                                │  │
│  │   ┌─────────────────────────────────────────────────────┐     │  │
│  │   │                    waybar                            │     │  │
│  │   └─────────────────────────────────────────────────────┘     │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │                     NORMAL LAYER                               │  │
│  │            (regular application windows)                       │  │
│  │                                                                │  │
│  │   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │  │
│  │   │   Firefox   │  │    foot     │  │    nvim     │           │  │
│  │   └─────────────┘  └─────────────┘  └─────────────┘           │  │
│  ├───────────────────────────────────────────────────────────────┤  │
│  │                   BACKGROUND LAYER                             │  │
│  │              (wallpapers, desktop)                             │  │
│  │                                                                │  │
│  │   ┌─────────────────────────────────────────────────────┐     │  │
│  │   │                    swaybg                            │     │  │
│  │   └─────────────────────────────────────────────────────┘     │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                                                                      │
│  Anchors:                                                            │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  TOP     - anchor to top edge                                 │   │
│  │  BOTTOM  - anchor to bottom edge                              │   │
│  │  LEFT    - anchor to left edge                                │   │
│  │  RIGHT   - anchor to right edge                               │   │
│  │                                                               │   │
│  │  Example: waybar với anchor TOP+LEFT+RIGHT                    │   │
│  │           → spans toàn bộ top edge                            │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Debugging Wayland

### Debug Tools

```bash
# ═══════════════════════════════════════════════════════════════════
#                    WAYLAND DEBUG TOOLS
# ═══════════════════════════════════════════════════════════════════

# 1. Protocol trace
export WAYLAND_DEBUG=1
sway  # Xem tất cả messages

# 2. Client-side only
export WAYLAND_DEBUG=client
firefox

# 3. Server-side only
export WAYLAND_DEBUG=server
sway

# 4. wlhax - Wayland hacking tool
# https://github.com/swaywm/wlhax
wlhax dump-protocol

# 5. wayland-info - Show compositor capabilities
wayland-info

# 6. wlr-randr - Output configuration (wlroots)
wlr-randr
# Output:
# WL-1 "Nested Wayland output"
#   Enabled: yes
#   Modes:
#     1920x1080 px, 60.000000 Hz (preferred, current)

# 7. swaymsg - Sway IPC
swaymsg -t get_tree      # Window tree
swaymsg -t get_outputs   # Output info
swaymsg -t get_inputs    # Input devices
swaymsg -t get_seats     # Seat info

# 8. wev - Wayland event viewer
wev  # Like xev for Wayland
```

### Debug Environment Variables

```bash
# ═══════════════════════════════════════════════════════════════════
#                  DEBUG ENVIRONMENT VARIABLES
# ═══════════════════════════════════════════════════════════════════

# Wayland core
WAYLAND_DEBUG=1              # All protocol messages
WAYLAND_DISPLAY=wayland-0    # Which display to connect

# wlroots
WLR_BACKENDS=wayland,drm,x11 # Force specific backends
WLR_RENDERER=gles2           # gles2, vulkan, pixman
WLR_DRM_NO_ATOMIC=1          # Disable atomic modesetting
WLR_DRM_NO_MODIFIERS=1       # Disable format modifiers
WLR_SCENE_DISABLE_DIRECT_SCANOUT=1  # Disable scanout
WLR_NO_HARDWARE_CURSORS=1    # Software cursor

# Mesa/OpenGL
MESA_DEBUG=1                 # Mesa debug
LIBGL_DEBUG=verbose          # OpenGL debug
EGL_LOG_LEVEL=debug          # EGL debug

# XWayland
XWAYLAND_DEBUG=1             # XWayland messages

# Qt
QT_QPA_PLATFORM=wayland      # Force Wayland
QT_WAYLAND_DEBUG=1           # Qt Wayland debug

# GTK
GDK_BACKEND=wayland          # Force Wayland
GDK_DEBUG=all                # GTK debug
```

### Profiling Performance

```bash
# Frame timing
export WLR_DRM_FRAME_TIMING=1
sway

# Perf trace
sudo perf record -g sway
sudo perf report

# GPU profiling (for Intel)
sudo intel_gpu_top

# Memory usage
valgrind --leak-check=full sway
```

---

## Tài nguyên học thêm

### Documentation
- [Wayland Book](https://wayland-book.com/) - Comprehensive guide
- [wlroots Wiki](https://github.com/swaywm/wlroots/wiki)
- [Wayland Protocol Docs](https://wayland.freedesktop.org/docs/html/)

### Source Code
- [wayland](https://gitlab.freedesktop.org/wayland/wayland)
- [wayland-protocols](https://gitlab.freedesktop.org/wayland/wayland-protocols)
- [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots)
- [sway](https://github.com/swaywm/sway)

### Tools
- [wlhax](https://github.com/swaywm/wlhax)
- [wev](https://git.sr.ht/~sircmpwn/wev)
- [wayland-info](https://gitlab.freedesktop.org/wayland/wayland-utils)

---

## Tiếp theo

- [Scripts và Thực hành](06-scripts-thuc-hanh.md) - Hands-on practice
