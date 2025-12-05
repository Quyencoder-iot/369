# Hướng Dẫn Toàn Diện: Sway Nested và Plasma

## Mục Lục
1. [Giới Thiệu](#giới-thiệu)
2. [Sway và Sway Nested](#sway-và-sway-nested)
3. [KDE Plasma](#kde-plasma)
4. [Cài Đặt](#cài-đặt)
5. [Cấu Hình](#cấu-hình)
6. [Vận Hành](#vận-hành)
7. [Hiểu Sâu Về Kiến Trúc](#hiểu-sâu-về-kiến-trúc)
8. [Troubleshooting](#troubleshooting)
9. [Tài Liệu Tham Khảo](#tài-liệu-tham-khảo)

---

## Giới Thiệu

### Sway là gì?
**Sway** là một compositor Wayland tương thích với i3 window manager. Nó được viết bằng Rust và cung cấp:
- Quản lý cửa sổ theo kiểu tiling (xếp gạch)
- Cấu hình bằng file text đơn giản
- Hiệu suất cao và tiết kiệm tài nguyên
- Tương thích với các script/config của i3

### Sway Nested là gì?
**Sway Nested** cho phép chạy Sway bên trong một môi trường hiển thị khác (như X11 hoặc Wayland compositor khác). Điều này hữu ích cho:
- Testing và development
- Chạy Sway trên desktop environment hiện có
- Học tập và thử nghiệm mà không cần logout

### KDE Plasma là gì?
**KDE Plasma** là một desktop environment hiện đại, mạnh mẽ với:
- Giao diện đẹp và có thể tùy biến cao
- Nhiều ứng dụng tích hợp
- Hỗ trợ Wayland và X11
- Cộng đồng lớn và tài liệu phong phú

---

## Sway và Sway Nested

### Kiến Trúc Cơ Bản

#### Wayland Protocol
Wayland là một protocol giao tiếp giữa:
- **Compositor**: Quản lý hiển thị và input
- **Clients**: Các ứng dụng cần hiển thị

```
┌─────────────┐
│   Client    │
│  (App)      │
└──────┬──────┘
       │ Wayland Protocol
       │
┌──────▼──────┐
│  Compositor │
│   (Sway)    │
└──────┬──────┘
       │
┌──────▼──────┐
│   Kernel    │
│  (DRM/KMS)  │
└─────────────┘
```

#### Sway Nested Architecture
Khi chạy nested, Sway trở thành một client của compositor cha:

```
┌─────────────┐
│   Client    │
│  (App)      │
└──────┬──────┘
       │
┌──────▼──────┐
│  Sway       │ ← Compositor con
│  (Nested)   │
└──────┬──────┘
       │
┌──────▼──────┐
│  Plasma/    │ ← Compositor cha
│  X11/Wayland│
└──────┬──────┘
       │
┌──────▼──────┐
│   Kernel    │
└─────────────┘
```

### Ưu và Nhược Điểm

#### Sway Nested
**Ưu điểm:**
- Không cần logout để test
- Dễ debug và development
- Có thể chạy song song với desktop hiện tại
- An toàn để thử nghiệm

**Nhược điểm:**
- Hiệu suất thấp hơn (nested rendering)
- Một số tính năng có thể không hoạt động đầy đủ
- Phức tạp hơn về kiến trúc

#### Sway Standalone
**Ưu điểm:**
- Hiệu suất tối ưu
- Toàn quyền kiểm soát display
- Trải nghiệm native

**Nhược điểm:**
- Cần logout/login
- Khó debug khi có vấn đề
- Cần cấu hình đầy đủ từ đầu

---

## KDE Plasma

### Kiến Trúc Plasma

Plasma được xây dựng trên:
- **Qt**: Framework UI
- **KWin**: Window manager/compositor
- **Plasma Shell**: Desktop shell
- **KDE Frameworks**: Thư viện hệ thống

### Plasma trên Wayland

Plasma 5.27+ hỗ trợ Wayland tốt với:
- KWin làm Wayland compositor
- Tích hợp tốt với các ứng dụng Qt/KDE
- Hỗ trợ các protocol Wayland mới

---

## Cài Đặt

### Yêu Cầu Hệ Thống

- Linux distribution (Ubuntu, Debian, Arch, Fedora, etc.)
- GPU hỗ trợ Wayland (hầu hết GPU hiện đại)
- Ít nhất 2GB RAM
- Quyền sudo/root

### Cài Đặt Sway

#### Ubuntu/Debian
```bash
# Cập nhật package list
sudo apt update

# Cài đặt Sway và dependencies
sudo apt install sway swaybg swayidle swaylock

# Cài đặt thêm tools hữu ích
sudo apt install wl-clipboard waybar rofi alacritty
```

#### Arch Linux
```bash
# Cài đặt từ AUR hoặc official repo
sudo pacman -S sway swaybg swayidle swaylock
sudo pacman -S waybar rofi alacritty
```

#### Fedora
```bash
sudo dnf install sway swaybg swayidle swaylock
sudo dnf install waybar rofi alacritty
```

### Cài Đặt Sway Nested

Sway nested được cài cùng với Sway, nhưng cần cấu hình đặc biệt:

```bash
# Kiểm tra xem sway có hỗ trợ nested không
sway --version

# Chạy nested mode
WLR_BACKENDS=headless sway
# hoặc
sway -c /path/to/config --my-next-gpu-wont-be-nvidia
```

### Cài Đặt KDE Plasma

#### Ubuntu/Debian
```bash
# Cài đặt Plasma Desktop đầy đủ
sudo apt install kde-plasma-desktop

# Hoặc cài đặt tất cả KDE applications
sudo apt install kde-standard

# Cài đặt Plasma Wayland session
sudo apt install plasma-workspace-wayland
```

#### Arch Linux
```bash
sudo pacman -S plasma-meta
sudo pacman -S plasma-wayland-session
```

#### Fedora
```bash
sudo dnf groupinstall "KDE Plasma Workspaces"
sudo dnf install plasma-workspace-wayland
```

### Kiểm Tra Cài Đặt

```bash
# Kiểm tra Sway
sway --version

# Kiểm tra Wayland
echo $XDG_SESSION_TYPE
# Nên hiển thị: wayland

# Kiểm tra Plasma
plasmashell --version
```

---

## Cấu Hình

### Cấu Hình Sway Cơ Bản

File cấu hình mặc định: `~/.config/sway/config`

#### Tạo File Cấu Hình

```bash
# Tạo thư mục config nếu chưa có
mkdir -p ~/.config/sway

# Copy file mẫu
cp /etc/sway/config ~/.config/sway/config
```

#### Cấu Hình Cơ Bản

```bash
# ~/.config/sway/config

# Modifier key (thường dùng $mod = Super/Windows key)
set $mod Mod4

# Terminal
set $term alacritty
# hoặc
set $term foot

# Browser
set $browser firefox

# Menu launcher
set $menu rofi -show drun

# Keybindings cơ bản
# Mở terminal
bindsym $mod+Return exec $term

# Mở menu
bindsym $mod+d exec $menu

# Đóng cửa sổ
bindsym $mod+Shift+q kill

# Chuyển focus
bindsym $mod+h focus left
bindsym $mod+j focus down
bindsym $mod+k focus up
bindsym $mod+l focus right

# Di chuyển cửa sổ
bindsym $mod+Shift+h move left
bindsym $mod+Shift+j move down
bindsym $mod+Shift+k move up
bindsym $mod+Shift+l move right

# Chuyển workspace
bindsym $mod+1 workspace number 1
bindsym $mod+2 workspace number 2
# ... tiếp tục đến 9, 0

# Di chuyển cửa sổ sang workspace
bindsym $mod+Shift+1 move container to workspace number 1

# Layout modes
bindsym $mod+s layout stacking
bindsym $mod+w layout tabbed
bindsym $mod+e layout toggle split

# Fullscreen
bindsym $mod+f fullscreen toggle

# Reload config
bindsym $mod+Shift+c reload

# Exit Sway
bindsym $mod+Shift+e exec swaynag -t warning -m 'Exit Sway?' -b 'Yes' 'swaymsg exit'

# Status bar (waybar)
bar {
    position top
    status_command waybar
}

# Background
output * bg /path/to/wallpaper.jpg fill

# Input settings
input * {
    xkb_layout us
    xkb_variant intl
}

# Window decorations
default_border pixel 2
default_floating_border pixel 2
```

### Cấu Hình Sway Nested

Để chạy Sway nested, bạn cần:

#### 1. Tạo Script Khởi Động

```bash
#!/bin/bash
# ~/sway-nested.sh

# Set environment variables cho nested mode
export WLR_BACKENDS=headless
export WLR_LIBINPUT_NO_DEVICES=1

# Chạy Sway nested
sway -c ~/.config/sway/config-nested
```

#### 2. File Config Nested

```bash
# ~/.config/sway/config-nested

# Include config chính
include ~/.config/sway/config

# Override một số settings cho nested
# Disable một số tính năng không cần thiết trong nested
# ...

# Set output cho nested window
output HEADLESS-1 {
    mode 1920x1080
}
```

#### 3. Chạy Sway Nested

```bash
# Cách 1: Từ terminal trong X11/Wayland session
chmod +x ~/sway-nested.sh
~/sway-nested.sh

# Cách 2: Trực tiếp
WLR_BACKENDS=headless sway

# Cách 3: Với kích thước cửa sổ cụ thể
WLR_BACKENDS=headless WLR_HEADLESS_OUTPUTS=1 sway
```

### Cấu Hình Plasma

#### Cấu Hình Cơ Bản

Plasma được cấu hình chủ yếu qua GUI:

1. **System Settings**: `systemsettings5` hoặc Settings trong menu
2. **Workspace Behavior**: Cấu hình window management
3. **Display and Monitor**: Cấu hình màn hình
4. **Input Devices**: Cấu hình bàn phím, chuột

#### Cấu Hình Plasma Wayland

File cấu hình: `~/.config/kwinrc`

```ini
[Compositing]
OpenGLIsUnsafe=false
GLCore=true
```

#### Tích Hợp Sway với Plasma

Bạn có thể chạy Sway nested trong Plasma:

```bash
# Từ Plasma, mở terminal và chạy:
WLR_BACKENDS=headless sway
```

---

## Vận Hành

### Khởi Động Sway

#### Standalone Mode
```bash
# Từ display manager (GDM, SDDM, etc.)
# Chọn "Sway" từ session menu

# Hoặc từ TTY
sway
```

#### Nested Mode
```bash
# Từ terminal trong session hiện tại
WLR_BACKENDS=headless sway
```

### Các Lệnh Quan Trọng

#### Sway Commands
```bash
# Xem cấu hình hiện tại
swaymsg -t get_config

# Xem cây cửa sổ
swaymsg -t get_tree

# Xem outputs
swaymsg -t get_outputs

# Xem inputs
swaymsg -t get_inputs

# Xem workspaces
swaymsg -t get_workspaces

# Thực thi lệnh
swaymsg exec firefox

# Chuyển workspace
swaymsg workspace 2

# Di chuyển cửa sổ
swaymsg move container to workspace 3

# Focus cửa sổ
swaymsg [app_id="firefox"] focus
```

#### Plasma Commands
```bash
# Khởi động lại Plasma shell
killall plasmashell && kstart plasmashell

# Xem thông tin session
qdbus org.kde.KWin /KWin org.kde.KWin.compositingActive

# Tắt/bật compositor
qdbus org.kde.KWin /Compositor org.kde.kwin.Compositing.suspend
qdbus org.kde.KWin /Compositor org.kde.kwin.Compositing.resume
```

### Workflow Hàng Ngày

#### Sway Workflow
1. **Mở ứng dụng**: `$mod+Return` (terminal), `$mod+d` (menu)
2. **Chuyển cửa sổ**: `$mod+h/j/k/l`
3. **Chuyển workspace**: `$mod+1-9`
4. **Tiling**: Tự động, hoặc `$mod+e` để toggle
5. **Floating**: `$mod+Shift+Space`

#### Plasma Workflow
1. **Mở ứng dụng**: `Alt+F2` hoặc menu
2. **Chuyển cửa sổ**: `Alt+Tab`
3. **Activities**: `Meta` key
4. **KRunner**: `Alt+Space`

### Scripts Hữu Ích

#### Script Khởi Động Sway
```bash
#!/bin/bash
# ~/.config/sway/autostart.sh

# Set wallpaper
swaybg -i ~/wallpapers/bg.jpg &

# Start waybar
waybar &

# Start notification daemon
mako &

# Set keyboard layout
swaymsg input "*" xkb_layout "us,vi"
swaymsg input "*" xkb_options "grp:alt_shift_toggle"
```

Thêm vào config:
```bash
exec ~/.config/sway/autostart.sh
```

---

## Hiểu Sâu Về Kiến Trúc

### Wayland Protocol Chi Tiết

#### Core Protocols

1. **wl_display**: Kết nối chính giữa client và compositor
2. **wl_compositor**: Tạo surfaces và regions
3. **wl_surface**: Vùng hiển thị của client
4. **wl_shell/xdg_shell**: Quản lý windows
5. **wl_seat**: Input devices (keyboard, pointer, touch)

#### Protocol Flow

```
Client                    Compositor
  │                          │
  ├─ wl_display.connect() ──►│
  │                          │
  ├─ wl_compositor.create_surface() ──►│
  │                          │
  ├─ wl_surface.attach(buffer) ──►│
  │                          │
  ├─ wl_surface.commit() ────►│
  │                          │
  │◄─ frame callback ────────┤
  │                          │
```

### Sway Architecture

#### Core Components

1. **wlroots**: Library cung cấp Wayland compositor functionality
2. **Sway IPC**: Inter-process communication
3. **Input Handling**: XKB, libinput
4. **Output Management**: DRM/KMS
5. **Window Management**: Tiling logic

#### Data Structures

```rust
// Simplified structure
Container {
    layout: Layout,
    children: Vec<Container>,
    window: Option<Window>,
    workspace: Workspace,
}

Workspace {
    name: String,
    output: Output,
    containers: Vec<Container>,
}

Output {
    name: String,
    modes: Vec<Mode>,
    current_mode: Mode,
}
```

### Plasma Architecture

#### Components

1. **KWin**: Window manager và compositor
2. **Plasma Shell**: Desktop shell
3. **KDE Frameworks**: Qt-based libraries
4. **QML**: UI description language

#### KWin trên Wayland

KWin sử dụng:
- **KWayland**: Qt bindings cho Wayland
- **KDecoration**: Window decorations
- **KScreen**: Screen management

### Nested Compositor Deep Dive

#### Headless Backend

Khi chạy nested với `WLR_BACKENDS=headless`:
- Sway không truy cập trực tiếp DRM
- Tạo virtual outputs
- Render vào buffers được chia sẻ với compositor cha

#### Performance Considerations

1. **Double Compositing**: Render 2 lần (Sway → Parent)
2. **Memory**: Cần nhiều RAM hơn
3. **CPU**: Overhead từ nested rendering
4. **Latency**: Tăng độ trễ input

### Debugging và Monitoring

#### Sway Debugging

```bash
# Enable debug logging
sway -d 2>&1 | tee sway.log

# Monitor IPC messages
swaymsg -m -t SUBSCRIBE '["window"]'

# Check wlroots debug
WLR_DEBUG=1 sway
```

#### Plasma Debugging

```bash
# KWin debug
QT_LOGGING_RULES="kwin*=true" kwin_wayland --replace

# Plasma shell debug
QT_LOGGING_RULES="org.kde.plasmashell*=true" plasmashell
```

#### Tools

```bash
# Wayland info
wayland-info

# Check Wayland clients
wayland-scanner

# Monitor Wayland protocol
wayland-monitor

# Sway specific
swaymsg -t get_tree | jq
```

---

## Troubleshooting

### Vấn Đề Thường Gặp

#### Sway Không Khởi Động

**Vấn đề**: Black screen hoặc crash
```bash
# Kiểm tra logs
journalctl -b | grep sway

# Kiểm tra Wayland session
echo $XDG_SESSION_TYPE

# Thử chạy với debug
sway -d
```

**Giải pháp**:
- Kiểm tra GPU drivers
- Đảm bảo có quyền truy cập /dev/dri/*
- Thử chạy nested mode trước

#### Sway Nested Không Hiển Thị

**Vấn đề**: Không thấy cửa sổ Sway
```bash
# Kiểm tra environment
echo $WLR_BACKENDS

# Thử với explicit output
WLR_BACKENDS=headless WLR_HEADLESS_OUTPUTS=1 sway
```

**Giải pháp**:
- Đảm bảo đang trong Wayland/X11 session
- Kiểm tra compositor cha có hoạt động không
- Thử với các backends khác nhau

#### Plasma Wayland Issues

**Vấn đề**: Ứng dụng không hiển thị đúng
```bash
# Kiểm tra compositor
qdbus org.kde.KWin /Compositor org.kde.kwin.Compositing.active

# Restart compositor
qdbus org.kde.KWin /Compositor org.kde.kwin.Compositing.suspend
qdbus org.kde.KWin /Compositor org.kde.kwin.Compositing.resume
```

**Giải pháp**:
- Đảm bảo đang dùng Wayland session
- Kiểm tra GPU drivers
- Cập nhật Plasma và Qt

#### Input Issues

**Vấn đề**: Bàn phím/chuột không hoạt động
```bash
# Kiểm tra input devices
swaymsg -t get_inputs

# Kiểm tra libinput
libinput list-devices
```

**Giải pháp**:
- Kiểm tra permissions (input group)
- Cấu hình lại input trong config
- Restart Sway/Plasma

### Performance Issues

#### Sway Nested Chậm

**Giải pháp**:
- Giảm resolution của nested output
- Tắt compositing effects trong compositor cha
- Sử dụng GPU acceleration nếu có thể

#### Plasma Lag

**Giải pháp**:
- Tắt effects không cần thiết
- Kiểm tra compositor backend (OpenGL vs XRender)
- Cập nhật drivers

---

## Tài Liệu Tham Khảo

### Official Documentation

- **Sway**: https://swaywm.org/
- **wlroots**: https://gitlab.freedesktop.org/wlroots/wlroots
- **KDE Plasma**: https://kde.org/plasma-desktop/
- **Wayland**: https://wayland.freedesktop.org/

### Hữu Ích

- **Sway Wiki**: https://github.com/swaywm/sway/wiki
- **Arch Wiki Sway**: https://wiki.archlinux.org/title/Sway
- **KDE Wiki**: https://wiki.kde.org/
- **Wayland Book**: https://wayland-book.com/

### Community

- **Sway IRC**: #sway on irc.libera.chat
- **KDE Forums**: https://forum.kde.org/
- **Reddit**: r/swaywm, r/kde

### Learning Resources

- **i3 User's Guide**: https://i3wm.org/docs/userguide.html (tương thích với Sway)
- **Wayland Protocol Specs**: https://wayland.freedesktop.org/docs/html/
- **Qt Wayland**: https://doc.qt.io/qt-6/wayland.html

---

## Kết Luận

Tài liệu này cung cấp kiến thức cơ bản đến nâng cao về Sway, Sway Nested, và Plasma. Để thành thạo:

1. **Thực hành**: Cài đặt và cấu hình nhiều lần
2. **Đọc code**: Xem source code của Sway và wlroots
3. **Tham gia community**: Hỏi và chia sẻ kinh nghiệm
4. **Experiment**: Thử các cấu hình khác nhau

Chúc bạn học tập thành công!
