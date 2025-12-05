# Hướng Dẫn Toàn Diện: Sway Nested và Plasma

## Mục Lục
1. [Giới Thiệu](#giới-thiệu)
2. [Sway Nested](#sway-nested)
3. [Plasma Desktop](#plasma-desktop)
4. [Cài Đặt](#cài-đặt)
5. [Cấu Hình](#cấu-hình)
6. [Vận Hành](#vận-hành)
7. [Hiểu Sâu Về Kiến Trúc](#hiểu-sâu-về-kiến-trúc)
8. [Troubleshooting](#troubleshooting)
9. [Tài Liệu Tham Khảo](#tài-liệu-tham-khảo)

---

## Giới Thiệu

### Sway Nested là gì?
**Sway Nested** là một cách chạy Sway (window manager dựa trên Wayland) bên trong một cửa sổ của một compositor khác. Điều này cho phép bạn:
- Chạy Sway trong môi trường desktop hiện có (như Plasma, GNOME, X11)
- Test và phát triển cấu hình Sway mà không cần logout
- Học và thử nghiệm Wayland compositor mà không ảnh hưởng đến hệ thống chính

### Plasma Desktop là gì?
**KDE Plasma** là một desktop environment hiện đại, mạnh mẽ và có thể tùy biến cao:
- Dựa trên Qt framework
- Hỗ trợ cả X11 và Wayland
- Có thể tích hợp với các window manager khác như Sway
- Layer Shell Qt cho phép các ứng dụng tương tác với Wayland compositor

---

## Sway Nested

### Kiến Trúc Cơ Bản

Sway nested hoạt động như một **nested compositor**:
```
┌─────────────────────────────────┐
│  Host Compositor (Plasma/X11)   │
│  ┌───────────────────────────┐  │
│  │   Sway Nested (Wayland)    │  │
│  │   ┌─────────────────────┐ │  │
│  │   │  Ứng dụng Wayland   │ │  │
│  │   └─────────────────────┘ │  │
│  └───────────────────────────┘  │
└─────────────────────────────────┘
```

### Các Thành Phần Chính

1. **Wayland Client**: Ứng dụng chạy trong Sway nested
2. **Sway Compositor**: Quản lý cửa sổ và input
3. **Wayland Backend**: Kết nối với host compositor
4. **XWayland**: Bridge cho ứng dụng X11

---

## Plasma Desktop

### Kiến Trúc Plasma trên Wayland

Plasma trên Wayland sử dụng:
- **KWin**: Window manager và compositor
- **Layer Shell Protocol**: Cho phép ứng dụng overlay (panel, dock)
- **Qt Wayland**: Qt integration với Wayland
- **Plasma Shell**: Desktop shell components

### Layer Shell Qt

**Layer Shell Qt** là một thư viện cho phép ứng dụng Qt tương tác với Wayland Layer Shell protocol:
- Tạo các overlay surfaces (panel, notification)
- Định vị cửa sổ ở các layer khác nhau
- Tích hợp với Plasma và các compositor Wayland khác

---

## Cài Đặt

### Yêu Cầu Hệ Thống

- Linux distribution (Ubuntu, Arch, Fedora, etc.)
- GPU hỗ trợ OpenGL/Vulkan
- Ít nhất 2GB RAM
- 5GB dung lượng trống

### Cài Đặt Sway

#### Ubuntu/Debian
```bash
# Cài đặt dependencies
sudo apt update
sudo apt install -y \
    sway \
    swaybg \
    swayidle \
    swaylock \
    waybar \
    wl-clipboard \
    xdg-desktop-portal-wlr \
    xwayland

# Cài đặt Sway nested (thường đã có trong gói sway)
```

#### Arch Linux
```bash
sudo pacman -S sway swaybg swayidle swaylock waybar wl-clipboard xdg-desktop-portal-wlr xwayland
```

#### Fedora
```bash
sudo dnf install sway swaybg swayidle swaylock waybar wl-clipboard xdg-desktop-portal-wlr xwayland
```

### Cài Đặt Plasma

#### Ubuntu/Debian
```bash
sudo apt install -y \
    plasma-desktop \
    plasma-workspace-wayland \
    kde-standard \
    sddm \
    sddm-theme-breeze
```

#### Arch Linux
```bash
sudo pacman -S plasma plasma-wayland-session sddm sddm-kcm
```

#### Fedora
```bash
sudo dnf groupinstall "KDE Plasma Workspaces"
sudo dnf install sddm
```

### Cài Đặt Layer Shell Qt

```bash
# Ubuntu/Debian
sudo apt install -y libqt5waylandclient5-dev qtwayland5-dev-tools

# Arch Linux
sudo pacman -S qt5-wayland qt6-wayland

# Fedora
sudo dnf install qt5-qtwayland-devel qt6-qtwayland-devel
```

---

## Cấu Hình

### Cấu Hình Sway Nested

#### 1. Tạo thư mục cấu hình
```bash
mkdir -p ~/.config/sway
```

#### 2. File cấu hình cơ bản: `~/.config/sway/config`

```bash
# Cấu hình cho Sway Nested
# Chạy trong một cửa sổ X11 hoặc Wayland

# Thiết lập output cho nested session
output * bg #000000 solid_color

# Mod key (thường là Super/Windows key)
set $mod Mod4

# Font
font pango:DejaVu Sans Mono 10

# Terminal
set $term alacritty
set $menu rofi -show drun

# Keybindings cơ bản
bindsym $mod+Return exec $term
bindsym $mod+d exec $menu
bindsym $mod+Shift+q kill
bindsym $mod+j focus left
bindsym $mod+k focus down
bindsym $mod+l focus up
bindsym $mod+semicolon focus right

# Chuyển đổi giữa tiling và floating
bindsym $mod+Shift+space floating toggle

# Chuyển đổi layout
bindsym $mod+e layout toggle split

# Reload config
bindsym $mod+Shift+c reload

# Exit nested session
bindsym $mod+Shift+e exec swaymsg exit

# Gaps
gaps inner 10
gaps outer 5

# Colors
client.focused          #4c7899 #285577 #ffffff
client.focused_inactive #333333 #5f676a #ffffff
client.unfocused        #333333 #222222 #888888
client.urgent           #2f343a #900000 #ffffff

# Window decorations
default_border pixel 2
default_floating_border pixel 2

# Cấu hình cho nested mode
# Tự động phát hiện khi chạy nested
```

#### 3. Script khởi chạy Sway Nested: `~/bin/sway-nested`

```bash
#!/bin/bash
# Script để chạy Sway nested trong X11 hoặc Wayland

# Kiểm tra môi trường hiện tại
if [ -n "$WAYLAND_DISPLAY" ]; then
    echo "Đang chạy trong Wayland, sử dụng nested compositor..."
    WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 sway -c ~/.config/sway/config
elif [ -n "$DISPLAY" ]; then
    echo "Đang chạy trong X11, sử dụng X11 backend..."
    WLR_BACKENDS=x11 sway -c ~/.config/sway/config
else
    echo "Không tìm thấy display server!"
    exit 1
fi
```

Làm cho script có thể thực thi:
```bash
chmod +x ~/bin/sway-nested
mkdir -p ~/bin
```

### Cấu Hình Plasma với Sway

#### 1. Tạo session tùy chỉnh

Tạo file: `~/.local/share/wayland-sessions/sway-nested.desktop`

```ini
[Desktop Entry]
Name=Sway Nested
Comment=Sway nested trong Plasma
Exec=sway-nested
Type=Application
DesktopNames=sway
```

#### 2. Cấu hình Plasma để hỗ trợ Layer Shell

Plasma tự động hỗ trợ Layer Shell protocol. Để kiểm tra:

```bash
# Kiểm tra Wayland protocols được hỗ trợ
wayland-info | grep -i layer
```

---

## Vận Hành

### Khởi Chạy Sway Nested

#### Từ Terminal trong Plasma
```bash
# Chạy trực tiếp
sway-nested

# Hoặc với các tùy chọn
WLR_BACKENDS=x11 sway
```

#### Từ Menu Plasma
1. Mở Application Launcher
2. Tìm "Sway Nested"
3. Click để khởi chạy

### Các Lệnh Cơ Bản trong Sway

#### Di chuyển cửa sổ
- `$mod + j/k/l/;`: Di chuyển focus
- `$mod + Shift + j/k/l/;`: Di chuyển cửa sổ
- `$mod + h`: Chia cửa sổ ngang
- `$mod + v`: Chia cửa sổ dọc

#### Quản lý workspace
- `$mod + 1-9`: Chuyển workspace
- `$mod + Shift + 1-9`: Di chuyển cửa sổ đến workspace

#### Floating mode
- `$mod + Shift + Space`: Toggle floating
- `$mod + Mouse drag`: Di chuyển cửa sổ floating
- `$mod + Mouse scroll`: Resize cửa sổ floating

### Sử Dụng Layer Shell với Qt

#### Ví dụ ứng dụng Qt sử dụng Layer Shell

```cpp
#include <QtWaylandClient/QWaylandClientExtension>
#include <QtWaylandClient/QtWaylandClientVersion>

// Layer Shell interface
class LayerShell : public QWaylandClientExtensionTemplate<LayerShell>
{
    Q_OBJECT
public:
    LayerShell() : QWaylandClientExtensionTemplate(1) {}
    
    void createLayerSurface(QWaylandWindow *window, 
                           const QString &layer,
                           const QString &anchor) {
        // Tạo layer surface
    }
};
```

### Quản Lý Ứng Dụng

#### Chạy ứng dụng Wayland native
```bash
# Trong Sway nested
GDK_BACKEND=wayland firefox
QT_QPA_PLATFORM=wayland dolphin
```

#### Chạy ứng dụng X11 (qua XWayland)
```bash
# Tự động chuyển qua XWayland
xterm
```

#### Kiểm tra ứng dụng đang chạy
```bash
# Liệt kê Wayland clients
swaymsg -t get_tree

# Liệt kê XWayland windows
xlsclients
```

---

## Hiểu Sâu Về Kiến Trúc

### Wayland Protocol Stack

```
┌─────────────────────────────────────┐
│     Ứng dụng (Client)               │
│  ┌───────────────────────────────┐  │
│  │  Wayland Protocol (IPC)       │  │
│  └───────────────────────────────┘  │
│           ↕                          │
│  ┌───────────────────────────────┐  │
│  │  Compositor (Sway/KWin)       │  │
│  │  - Window Management          │  │
│  │  - Input Handling             │  │
│  │  - Rendering                  │  │
│  └───────────────────────────────┘  │
│           ↕                          │
│  ┌───────────────────────────────┐  │
│  │  Kernel (DRM/KMS)             │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

### Sway Nested Architecture

```
Host Environment (Plasma/X11)
    │
    ├─> X11 Window hoặc Wayland Surface
    │       │
    │       └─> Sway Nested Compositor
    │               │
    │               ├─> Wayland Clients
    │               │   ├─> Native Wayland apps
    │               │   └─> XWayland (X11 apps)
    │               │
    │               └─> Input Events
    │                   └─> Forwarded to clients
```

### Layer Shell Protocol

Layer Shell cho phép ứng dụng tạo các surface ở các layer khác nhau:

```
┌─────────────────────────────────┐
│  Overlay Layer (Notifications)  │
├─────────────────────────────────┤
│  Top Layer (Panels)            │
├─────────────────────────────────┤
│  Bottom Layer (Docks)          │
├─────────────────────────────────┤
│  Background Layer (Wallpapers) │
└─────────────────────────────────┘
```

#### Các Layer Types:
- `zwlr_layer_shell_v1_layer_background`: Background
- `zwlr_layer_shell_v1_layer_bottom`: Dưới cửa sổ thường
- `zwlr_layer_shell_v1_layer_top`: Trên cửa sổ thường
- `zwlr_layer_shell_v1_layer_overlay`: Trên cùng (notifications)

### Compositor Responsibilities

1. **Window Management**
   - Tạo và quản lý surfaces
   - Xử lý resize, move, focus
   - Quản lý workspaces

2. **Input Handling**
   - Keyboard events
   - Mouse/touchpad events
   - Touch events
   - Tablet input

3. **Rendering**
   - Compositing surfaces
   - OpenGL/Vulkan rendering
   - VSync và frame timing

4. **Protocol Implementation**
   - Wayland core protocol
   - Extensions (xdg-shell, layer-shell, etc.)
   - XWayland integration

### Memory Management

```
Client Application
    │
    ├─> wl_surface (shared memory)
    │       │
    │       └─> Compositor reads buffer
    │
    └─> wl_buffer (dmabuf/EGL)
            │
            └─> GPU memory (zero-copy)
```

### Input Event Flow

```
Hardware Input
    │
    ├─> Kernel (evdev)
    │       │
    │       └─> libinput
    │               │
    │               └─> Compositor
    │                       │
    │                       ├─> Focused window
    │                       └─> Global shortcuts
```

---

## Troubleshooting

### Vấn Đề Thường Gặp

#### 1. Sway nested không khởi động

**Triệu chứng**: Lỗi khi chạy `sway-nested`

**Giải pháp**:
```bash
# Kiểm tra log
sway -c ~/.config/sway/config 2>&1 | tee sway.log

# Kiểm tra backend
echo $WLR_BACKENDS

# Thử với backend cụ thể
WLR_BACKENDS=x11 sway
WLR_BACKENDS=headless sway
```

#### 2. Ứng dụng không hiển thị

**Triệu chứng**: Ứng dụng chạy nhưng không thấy cửa sổ

**Giải pháp**:
```bash
# Kiểm tra tree
swaymsg -t get_tree

# Kiểm tra output
swaymsg -t get_outputs

# Force reload
swaymsg reload
```

#### 3. Input không hoạt động

**Triệu chứng**: Bàn phím/chuột không phản hồi trong Sway nested

**Giải pháp**:
```bash
# Kiểm tra input devices
swaymsg -t get_inputs

# Thử với libinput debug
LIBINPUT_LOG_LEVEL=debug sway
```

#### 4. Performance kém

**Triệu chứng**: Lag, frame drops

**Giải pháp**:
```bash
# Kiểm tra GPU
glxinfo | grep "OpenGL renderer"

# Giảm effects
# Trong config: disable animations, reduce gaps

# Kiểm tra compositor của host
# Nếu đang dùng X11, thử tắt compositor
```

#### 5. Layer Shell không hoạt động

**Triệu chứng**: Ứng dụng Layer Shell không hiển thị đúng

**Giải pháp**:
```bash
# Kiểm tra protocol support
wayland-info

# Kiểm tra Sway version (cần >= 1.5)
sway --version

# Kiểm tra trong config
# Đảm bảo không có lệnh disable layer-shell
```

### Debug Commands

```bash
# Debug Sway
SWAY_DEBUG=1 sway

# Debug Wayland
WAYLAND_DEBUG=1 application

# Debug XWayland
XWAYLAND_DEBUG=1

# Monitor Wayland events
wayland-monitor

# Kiểm tra protocols
wayland-scanner
```

### Log Files

```bash
# Sway log
~/.local/share/sway/sway.log

# Wayland log
$XDG_RUNTIME_DIR/wayland-*.log

# XWayland log
~/.local/share/xorg/Xwayland.*.log
```

---

## Tài Liệu Tham Khảo

### Tài Liệu Chính Thức

1. **Sway**
   - Website: https://swaywm.org
   - Wiki: https://github.com/swaywm/sway/wiki
   - IRC: #sway on irc.libera.chat

2. **Wayland**
   - Protocol: https://wayland.freedesktop.org/docs/html/
   - Wiki: https://wayland.freedesktop.org/

3. **Plasma**
   - Website: https://kde.org/plasma-desktop/
   - Wiki: https://wiki.archlinux.org/title/KDE

4. **Layer Shell**
   - Protocol: https://wayland.app/protocols/wlr-layer-shell-unstable-v1
   - Qt Integration: https://doc.qt.io/qt-6/qtwayland-index.html

### Công Cụ Hữu Ích

```bash
# Wayland utilities
wayland-info          # Thông tin về Wayland
wayland-monitor       # Monitor Wayland events
wl-clipboard          # Clipboard manager
wl-paste              # Paste từ clipboard
wl-copy               # Copy vào clipboard

# Sway utilities
swaymsg               # Gửi commands đến Sway
swaybar               # Status bar
swaybg                # Background
swayidle              # Idle management
swaylock              # Screen locker

# Debug tools
wayland-scanner       # Wayland protocol scanner
weston-info           # Weston info tool
```

### Ví Dụ Cấu Hình Nâng Cao

Xem thêm các file ví dụ trong thư mục `examples/`:
- `sway-config-advanced`: Cấu hình nâng cao
- `layer-shell-example`: Ví dụ Layer Shell app
- `plasma-integration`: Tích hợp với Plasma

---

## Kết Luận

Sway nested và Plasma là những công cụ mạnh mẽ để làm việc với Wayland:
- **Sway nested** cho phép test và phát triển trong môi trường an toàn
- **Plasma** cung cấp desktop environment hoàn chỉnh với hỗ trợ Wayland tốt
- **Layer Shell** mở ra khả năng tạo các ứng dụng overlay và panel

Hãy thử nghiệm và tùy biến theo nhu cầu của bạn!
