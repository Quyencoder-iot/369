# Sway Nested Mode - Hướng Dẫn Chi Tiết

## Mục lục
1. [Nested Mode là gì?](#nested-mode-là-gì)
2. [Cách hoạt động kỹ thuật](#cách-hoạt-động-kỹ-thuật)
3. [Chạy Sway Nested](#chạy-sway-nested)
4. [Cấu hình cho Nested Mode](#cấu-hình-cho-nested-mode)
5. [Các Backend Options](#các-backend-options)
6. [Troubleshooting](#troubleshooting)
7. [Use Cases thực tế](#use-cases-thực-tế)

---

## Nested Mode là gì?

### Định nghĩa
**Nested Mode** là khả năng chạy một Wayland compositor như một **client** của compositor khác, thay vì chạy trực tiếp trên hardware (DRM/KMS).

### So sánh Native vs Nested

```
┌─────────────────────────────────────────────────────────────────────┐
│                    NATIVE MODE (Normal)                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────────────────────────────────────────────┐           │
│  │                      Sway                             │           │
│  │              (Wayland Compositor)                     │           │
│  └──────────────────────────┬───────────────────────────┘           │
│                             │                                        │
│                             ▼                                        │
│  ┌──────────────────────────────────────────────────────┐           │
│  │                    wlroots                            │           │
│  │               DRM/KMS Backend                         │           │
│  └──────────────────────────┬───────────────────────────┘           │
│                             │                                        │
│                             ▼                                        │
│  ┌──────────────────────────────────────────────────────┐           │
│  │               Linux Kernel (DRM)                      │           │
│  │                    GPU Driver                         │           │
│  └──────────────────────────────────────────────────────┘           │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│                    NESTED MODE                                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────────────────────────────────────────────┐           │
│  │            Host Compositor (KDE Plasma/GNOME)         │           │
│  │                                                       │           │
│  │   ┌─────────────────────────────────────────────┐    │           │
│  │   │           Sway (Nested)                      │    │           │
│  │   │      Chạy như một Wayland client             │    │           │
│  │   │                                              │    │           │
│  │   │   ┌─────────┐  ┌─────────┐  ┌─────────┐     │    │           │
│  │   │   │  App 1  │  │  App 2  │  │  App 3  │     │    │           │
│  │   │   └─────────┘  └─────────┘  └─────────┘     │    │           │
│  │   │                                              │    │           │
│  │   └─────────────────────────────────────────────┘    │           │
│  │                                                       │           │
│  │   wlroots Wayland Backend ◄────── thay vì DRM        │           │
│  │                                                       │
│  └──────────────────────────────────────────────────────┘           │
│                             │                                        │
│                             ▼                                        │
│  ┌──────────────────────────────────────────────────────┐           │
│  │               Linux Kernel (DRM)                      │           │
│  └──────────────────────────────────────────────────────┘           │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Cách hoạt động kỹ thuật

### wlroots Backend System

wlroots (library mà Sway sử dụng) hỗ trợ nhiều backends:

```
┌─────────────────────────────────────────────────────────────────────┐
│                      wlroots Backends                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌─────────────────┐                                                │
│  │  DRM Backend    │ ◄─── Native, direct hardware access            │
│  │  (Default)      │      GPU → KMS → Display                       │
│  └─────────────────┘                                                │
│                                                                      │
│  ┌─────────────────┐                                                │
│  │ Wayland Backend │ ◄─── Nested trong Wayland compositor khác      │
│  │ (WLR_BACKENDS=  │      Sway là Wayland client                    │
│  │   wayland)      │                                                │
│  └─────────────────┘                                                │
│                                                                      │
│  ┌─────────────────┐                                                │
│  │   X11 Backend   │ ◄─── Nested trong X11 session                  │
│  │ (WLR_BACKENDS=  │      Sway chạy trong X11 window                │
│  │   x11)          │                                                │
│  └─────────────────┘                                                │
│                                                                      │
│  ┌─────────────────┐                                                │
│  │ Headless Backend│ ◄─── Không có display (testing/CI)             │
│  │ (WLR_BACKENDS=  │                                                │
│  │   headless)     │                                                │
│  └─────────────────┘                                                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Luồng dữ liệu trong Nested Mode

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Data Flow in Nested Mode                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  User Input (keyboard/mouse)                                        │
│       │                                                              │
│       ▼                                                              │
│  ┌─────────────────────────────────────┐                            │
│  │    Host Compositor (e.g., KWin)     │                            │
│  │    - Nhận input từ kernel           │                            │
│  │    - Forward đến focused client     │                            │
│  └───────────────────┬─────────────────┘                            │
│                      │                                               │
│       Wayland Protocol (wl_keyboard, wl_pointer)                    │
│                      │                                               │
│                      ▼                                               │
│  ┌─────────────────────────────────────┐                            │
│  │    Sway (Nested Compositor)         │                            │
│  │    - Nhận input như Wayland client  │                            │
│  │    - Xử lý và forward đến apps      │                            │
│  └───────────────────┬─────────────────┘                            │
│                      │                                               │
│       Wayland Protocol (nested)                                      │
│                      │                                               │
│                      ▼                                               │
│  ┌─────────────────────────────────────┐                            │
│  │    Applications trong Sway          │                            │
│  │    (foot, firefox, etc.)            │                            │
│  └─────────────────────────────────────┘                            │
│                                                                      │
│  ═══════════════════════════════════════                            │
│                                                                      │
│  Rendering (ngược lại):                                             │
│  Apps → Sway (compose) → Host (display)                             │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Chạy Sway Nested

### Cách 1: Trong Wayland Session (KDE Plasma Wayland, GNOME Wayland)

```bash
# Đơn giản nhất - tự động detect
sway

# Hoặc explicit set backend
WLR_BACKENDS=wayland sway

# Với custom size
WLR_WL_OUTPUTS=1 sway  # 1 output
```

### Cách 2: Trong X11 Session

```bash
# Set X11 backend
WLR_BACKENDS=x11 sway

# Với custom window size (không fullscreen)
WLR_X11_OUTPUTS=1 sway
```

### Cách 3: Với Full Control

```bash
# Script khởi động nested sway
#!/bin/bash

# Detect host session type
if [ "$XDG_SESSION_TYPE" = "wayland" ]; then
    export WLR_BACKENDS=wayland
elif [ "$XDG_SESSION_TYPE" = "x11" ]; then
    export WLR_BACKENDS=x11
else
    echo "Unknown session type: $XDG_SESSION_TYPE"
    exit 1
fi

# Optional: Set renderer
# export WLR_RENDERER=vulkan  # hoặc gles2, pixman

# Optional: Debug output
# export WAYLAND_DEBUG=1

# Run Sway
exec sway "$@"
```

### Cách 4: Với cage hoặc window wrapper

```bash
# Dùng cage để wrap Sway trong một window
cage -- sway

# Hoặc dùng weston-terminal
weston-terminal -e sway
```

---

## Cấu hình cho Nested Mode

### Config riêng cho Nested Mode

```bash
# Tạo config riêng cho nested
mkdir -p ~/.config/sway

# File: ~/.config/sway/config.nested
```

```bash
# ╔═══════════════════════════════════════════════════════════════════╗
# ║              SWAY NESTED MODE CONFIGURATION                        ║
# ╚═══════════════════════════════════════════════════════════════════╝

# ─────────────────────────────────────────────────────────────────────
# NESTED-SPECIFIC SETTINGS
# ─────────────────────────────────────────────────────────────────────

# Trong nested mode, thường dùng Mod1 (Alt) để tránh conflict với host
set $mod Mod1

# Terminal
set $term foot

# Menu
set $menu wofi --show drun

# ─────────────────────────────────────────────────────────────────────
# OUTPUT CONFIGURATION FOR NESTED
# ─────────────────────────────────────────────────────────────────────

# Trong nested mode, output thường là WL-1 hoặc X11-1
# Không cần cấu hình resolution vì phụ thuộc window size

# Đơn giản set background
output * bg #1a1a2e solid_color

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS (tránh conflict với host)
# ─────────────────────────────────────────────────────────────────────

# Dùng Alt thay vì Super
bindsym $mod+Return exec $term
bindsym $mod+Shift+q kill
bindsym $mod+d exec $menu
bindsym $mod+Shift+c reload

# EXIT nested sway - quan trọng!
bindsym $mod+Shift+e exec swaymsg exit

# Navigation (vim-style với Alt)
bindsym $mod+h focus left
bindsym $mod+j focus down
bindsym $mod+k focus up
bindsym $mod+l focus right

bindsym $mod+Shift+h move left
bindsym $mod+Shift+j move down
bindsym $mod+Shift+k move up
bindsym $mod+Shift+l move right

# Workspaces
bindsym $mod+1 workspace number 1
bindsym $mod+2 workspace number 2
bindsym $mod+3 workspace number 3
bindsym $mod+4 workspace number 4
bindsym $mod+5 workspace number 5

bindsym $mod+Shift+1 move container to workspace number 1
bindsym $mod+Shift+2 move container to workspace number 2
bindsym $mod+Shift+3 move container to workspace number 3
bindsym $mod+Shift+4 move container to workspace number 4
bindsym $mod+Shift+5 move container to workspace number 5

# Layout
bindsym $mod+b splith
bindsym $mod+v splitv
bindsym $mod+f fullscreen
bindsym $mod+Shift+space floating toggle
bindsym $mod+space focus mode_toggle

# Resize mode
mode "resize" {
    bindsym h resize shrink width 10px
    bindsym j resize grow height 10px
    bindsym k resize shrink height 10px
    bindsym l resize grow width 10px
    bindsym Return mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+r mode "resize"

# ─────────────────────────────────────────────────────────────────────
# APPEARANCE (compact cho nested window)
# ─────────────────────────────────────────────────────────────────────

# Giảm gaps cho không gian nhỏ hơn
gaps inner 5
gaps outer 2

# Border mỏng hơn
default_border pixel 1
default_floating_border pixel 1

# Colors (dark theme)
client.focused          #6272a4 #44475a #f8f8f2 #6272a4 #6272a4
client.focused_inactive #44475a #282a36 #f8f8f2 #44475a #44475a
client.unfocused        #282a36 #282a36 #bfbfbf #282a36 #282a36
client.urgent           #ff5555 #ff5555 #f8f8f2 #ff5555 #ff5555

# ─────────────────────────────────────────────────────────────────────
# STATUS BAR (minimal cho nested)
# ─────────────────────────────────────────────────────────────────────

bar {
    position top
    height 20
    
    status_command while echo "Sway Nested | $(date +'%H:%M')"; do sleep 30; done
    
    colors {
        statusline #f8f8f2
        background #282a36
        focused_workspace #6272a4 #44475a #f8f8f2
        inactive_workspace #282a36 #282a36 #bfbfbf
    }
}

# ─────────────────────────────────────────────────────────────────────
# NO AUTOSTART (tránh duplicate services)
# ─────────────────────────────────────────────────────────────────────
# Không cần mako, swayidle trong nested mode
# Host compositor đã xử lý notifications và idle
```

### Chạy với config riêng

```bash
# Chạy Sway với config nested
sway -c ~/.config/sway/config.nested
```

---

## Các Backend Options

### Environment Variables quan trọng

```bash
# ═══════════════════════════════════════════════════════════════════
#                      WLR ENVIRONMENT VARIABLES
# ═══════════════════════════════════════════════════════════════════

# Backend selection
WLR_BACKENDS=wayland    # Chạy trong Wayland session
WLR_BACKENDS=x11        # Chạy trong X11 session
WLR_BACKENDS=headless   # Không có display
WLR_BACKENDS=drm        # Native (default)

# Renderer selection
WLR_RENDERER=gles2      # OpenGL ES 2 (default)
WLR_RENDERER=vulkan     # Vulkan renderer
WLR_RENDERER=pixman     # Software renderer (fallback)

# Output configuration
WLR_WL_OUTPUTS=2        # Số virtual outputs trong Wayland backend
WLR_X11_OUTPUTS=1       # Số windows trong X11 backend

# Debugging
WLR_DRM_NO_ATOMIC=1     # Disable atomic modesetting
WLR_DRM_NO_MODIFIERS=1  # Disable format modifiers

# ═══════════════════════════════════════════════════════════════════
#                    WAYLAND ENVIRONMENT VARIABLES
# ═══════════════════════════════════════════════════════════════════

WAYLAND_DEBUG=1         # Enable Wayland protocol debugging
WAYLAND_DISPLAY=wayland-1   # Specify Wayland display

# ═══════════════════════════════════════════════════════════════════
#                     SWAY SPECIFIC VARIABLES
# ═══════════════════════════════════════════════════════════════════

SWAYSOCK=/run/user/1000/sway-ipc.*.sock  # IPC socket path
SWAY_CURSOR_SIZE=24     # Cursor size
SWAY_CURSOR_THEME=Adwaita  # Cursor theme
```

### Script tổng hợp

```bash
#!/bin/bash
# File: ~/bin/sway-nested

# ═══════════════════════════════════════════════════════════════════
# Sway Nested Launcher
# ═══════════════════════════════════════════════════════════════════

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect session type
detect_session() {
    if [ -n "$WAYLAND_DISPLAY" ]; then
        echo "wayland"
    elif [ -n "$DISPLAY" ]; then
        echo "x11"
    else
        echo "unknown"
    fi
}

# Main
main() {
    local session_type=$(detect_session)
    
    log_info "Detected session type: $session_type"
    
    case $session_type in
        wayland)
            export WLR_BACKENDS=wayland
            log_info "Using Wayland backend"
            ;;
        x11)
            export WLR_BACKENDS=x11
            log_info "Using X11 backend"
            ;;
        *)
            log_error "Cannot detect session type"
            exit 1
            ;;
    esac
    
    # Optional: Use nested config if exists
    local config="$HOME/.config/sway/config.nested"
    if [ -f "$config" ]; then
        log_info "Using nested config: $config"
        exec sway -c "$config" "$@"
    else
        log_warn "No nested config found, using default"
        exec sway "$@"
    fi
}

main "$@"
```

---

## Troubleshooting

### Vấn đề thường gặp

#### 1. Sway không khởi động trong nested mode

```bash
# Kiểm tra WAYLAND_DISPLAY
echo $WAYLAND_DISPLAY
# Nên có giá trị như: wayland-0 hoặc wayland-1

# Kiểm tra socket file
ls -la $XDG_RUNTIME_DIR/wayland-*

# Nếu trong X11, kiểm tra DISPLAY
echo $DISPLAY
# Nên có giá trị như: :0 hoặc :1
```

#### 2. Keyboard không hoạt động

```bash
# Trong host compositor, Sway window phải được focus
# Một số host compositors cần cấu hình đặc biệt

# Kiểm tra input trong Sway
swaymsg -t get_inputs

# Thử force grab keyboard (trong nested Sway)
# Thêm vào config:
# input * {
#     xkb_layout us
# }
```

#### 3. GPU acceleration không hoạt động

```bash
# Kiểm tra renderer
WLR_RENDERER=gles2 sway

# Fallback to software rendering
WLR_RENDERER=pixman sway

# Kiểm tra OpenGL
glxinfo | grep "OpenGL renderer"
```

#### 4. Cursor không hiển thị

```bash
# Set cursor theme và size
export XCURSOR_THEME=Adwaita
export XCURSOR_SIZE=24
sway
```

#### 5. Screen tearing / Performance issues

```bash
# Thử Vulkan renderer (nếu được hỗ trợ)
WLR_RENDERER=vulkan sway

# Hoặc force VSync
export vblank_mode=1
sway
```

### Debug Mode

```bash
# Chạy Sway với debug output
sway -d 2>&1 | tee ~/sway-debug.log

# Wayland protocol debug
WAYLAND_DEBUG=1 sway 2>&1 | tee ~/wayland-debug.log

# Xem logs trong real-time
journalctl --user -f -u sway
```

---

## Use Cases thực tế

### 1. Development & Testing

```bash
# Test sway config changes mà không cần logout
WLR_BACKENDS=wayland sway -c ~/.config/sway/config.test

# Test wlroots-based app
WLR_BACKENDS=wayland cage -- my-wayland-app
```

### 2. Screen Recording

```bash
# Record chỉ Sway window (dễ hơn full desktop)
# Trong Plasma, dùng OBS hoặc wf-recorder để record Sway window

# Record từ bên trong Sway nested
wf-recorder -o WL-1 -f output.mp4
```

### 3. Multi-WM Workflow

```bash
# Plasma cho GUI apps, Sway cho terminal workflow
# Khởi động từ Plasma với một keybinding

# ~/.local/share/applications/sway-nested.desktop
[Desktop Entry]
Name=Sway Nested
Comment=Run Sway in a window
Exec=env WLR_BACKENDS=wayland sway -c ~/.config/sway/config.nested
Icon=sway
Type=Application
Categories=System;
```

### 4. Learning Tiling WM

```bash
# Script để học Sway an toàn
#!/bin/bash

echo "Welcome to Sway Learning Mode!"
echo "================================"
echo ""
echo "Key bindings:"
echo "  Alt+Enter    - Open terminal"
echo "  Alt+d        - Open app launcher"
echo "  Alt+Shift+e  - Exit Sway"
echo "  Alt+h/j/k/l  - Navigate windows"
echo "  Alt+1-5      - Switch workspaces"
echo ""
echo "Press Enter to start..."
read

WLR_BACKENDS=wayland sway -c ~/.config/sway/config.nested
```

---

## So sánh Performance

| Metric | Native Mode | Nested Mode |
|--------|-------------|-------------|
| Latency | ~1-2ms | ~5-10ms |
| GPU Utilization | Direct | Composited |
| Memory | Lower | Higher (double buffer) |
| Multi-monitor | Full support | Limited |
| VRR/FreeSync | Supported | Usually not |
| HDR | Supported | Depends on host |

### Khi nào dùng Native vs Nested?

| Use Case | Recommendation |
|----------|----------------|
| Daily driver | Native |
| Development/Testing | Nested |
| Learning | Nested |
| Screen recording | Nested |
| Gaming | Native |
| Professional work | Native |

---

## Tiếp theo

- [KDE Plasma & Tích hợp](04-kde-plasma.md) - Sử dụng Sway với Plasma
- [Kiến trúc Wayland](05-wayland-architecture.md) - Hiểu sâu protocol
