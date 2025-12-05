# Hướng Dẫn Cài Đặt Sway

## Mục lục
1. [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
2. [Cài đặt trên Ubuntu/Debian](#cài-đặt-trên-ubuntudebian)
3. [Cài đặt trên Fedora](#cài-đặt-trên-fedora)
4. [Cài đặt trên Arch Linux](#cài-đặt-trên-arch-linux)
5. [Build từ source](#build-từ-source)
6. [Cấu hình cơ bản](#cấu-hình-cơ-bản)
7. [Xác minh cài đặt](#xác-minh-cài-đặt)

---

## Yêu cầu hệ thống

### Phần cứng
- GPU hỗ trợ OpenGL ES 2.0+
- Drivers đồ họa với KMS/DRM support

### Phần mềm
- Linux Kernel 4.12+ (khuyến nghị 5.x+)
- Wayland libraries
- wlroots (tự động cài với Sway)

### Kiểm tra khả năng tương thích

```bash
# Kiểm tra kernel version
uname -r

# Kiểm tra GPU driver
lspci -k | grep -A 3 VGA

# Kiểm tra Wayland session support
echo $XDG_SESSION_TYPE
# Nếu output là "wayland" hoặc "x11", bạn có thể chạy Sway

# Kiểm tra DRM support
ls -la /dev/dri/
# Cần thấy card0, renderD128, etc.
```

---

## Cài đặt trên Ubuntu/Debian

### Ubuntu 22.04+ / Debian 12+

```bash
# Cập nhật hệ thống
sudo apt update && sudo apt upgrade -y

# Cài đặt Sway và các dependencies cần thiết
sudo apt install -y \
    sway \
    swaylock \
    swayidle \
    swaybg \
    waybar \
    wofi \
    foot \
    wl-clipboard \
    mako-notifier \
    grim \
    slurp \
    xdg-desktop-portal-wlr

# Cài thêm các tiện ích hữu ích
sudo apt install -y \
    brightnessctl \
    playerctl \
    pavucontrol \
    network-manager-gnome
```

### Giải thích các packages

| Package | Mô tả |
|---------|-------|
| `sway` | Wayland compositor chính |
| `swaylock` | Screen locker cho Sway |
| `swayidle` | Idle management (tự động lock màn hình) |
| `swaybg` | Background/wallpaper setter |
| `waybar` | Status bar hiện đại |
| `wofi` | Application launcher (thay dmenu/rofi) |
| `foot` | Terminal emulator cho Wayland |
| `wl-clipboard` | Clipboard utilities (wl-copy, wl-paste) |
| `mako-notifier` | Notification daemon |
| `grim` | Screenshot tool |
| `slurp` | Region selection cho screenshot |
| `xdg-desktop-portal-wlr` | Portal cho screen sharing |

---

## Cài đặt trên Fedora

```bash
# Cài đặt Sway và ecosystem
sudo dnf install -y \
    sway \
    swaylock \
    swayidle \
    swaybg \
    waybar \
    wofi \
    foot \
    wl-clipboard \
    mako \
    grim \
    slurp \
    xdg-desktop-portal-wlr

# Thêm các tiện ích
sudo dnf install -y \
    brightnessctl \
    playerctl \
    pavucontrol
```

---

## Cài đặt trên Arch Linux

```bash
# Cài đặt Sway và ecosystem
sudo pacman -S \
    sway \
    swaylock \
    swayidle \
    swaybg \
    waybar \
    wofi \
    foot \
    wl-clipboard \
    mako \
    grim \
    slurp \
    xdg-desktop-portal-wlr

# Từ AUR (dùng yay hoặc paru)
yay -S \
    swaylock-effects \
    autotiling
```

---

## Build từ Source

### Lý do build từ source
- Cần features mới nhất
- Debug và development
- Custom patches

### Dependencies

```bash
# Ubuntu/Debian
sudo apt install -y \
    git \
    meson \
    ninja-build \
    build-essential \
    libwayland-dev \
    wayland-protocols \
    libwlroots-dev \
    libinput-dev \
    libxkbcommon-dev \
    libudev-dev \
    libpixman-1-dev \
    libcairo2-dev \
    libpango1.0-dev \
    libgdk-pixbuf2.0-dev \
    libjson-c-dev \
    libpcre2-dev \
    scdoc
```

### Build wlroots (nếu cần version mới hơn)

```bash
# Clone wlroots
git clone https://gitlab.freedesktop.org/wlroots/wlroots.git
cd wlroots

# Checkout version ổn định
git checkout 0.17.0  # hoặc version mới hơn

# Build
meson setup build/
ninja -C build/
sudo ninja -C build/ install

# Cập nhật library cache
sudo ldconfig
```

### Build Sway

```bash
# Clone Sway
git clone https://github.com/swaywm/sway.git
cd sway

# Checkout version ổn định
git checkout 1.9  # hoặc version mới hơn

# Build
meson setup build/
ninja -C build/

# Install
sudo ninja -C build/ install
```

---

## Cấu hình cơ bản

### Cấu trúc thư mục cấu hình

```
~/.config/sway/
├── config           # File cấu hình chính
├── config.d/        # Thư mục cho các config modules
│   ├── input.conf
│   ├── output.conf
│   ├── keybindings.conf
│   └── autostart.conf
└── scripts/         # Custom scripts
    ├── screenshot.sh
    └── volume.sh
```

### File config cơ bản

```bash
# Tạo thư mục config
mkdir -p ~/.config/sway

# Copy config mặc định
cp /etc/sway/config ~/.config/sway/config
```

### Nội dung config cơ bản (~/.config/sway/config)

```bash
# ╔═══════════════════════════════════════════════════════════════════╗
# ║                    SWAY CONFIGURATION FILE                        ║
# ╚═══════════════════════════════════════════════════════════════════╝

# ─────────────────────────────────────────────────────────────────────
# VARIABLES
# ─────────────────────────────────────────────────────────────────────

# Mod key: Mod4 = Super/Windows, Mod1 = Alt
set $mod Mod4

# Direction keys (vim-style)
set $left h
set $down j
set $up k
set $right l

# Terminal
set $term foot

# Application launcher
set $menu wofi --show drun

# ─────────────────────────────────────────────────────────────────────
# OUTPUT CONFIGURATION
# ─────────────────────────────────────────────────────────────────────

# Wallpaper
output * bg /usr/share/backgrounds/sway/Sway_Wallpaper_Blue_1920x1080.png fill

# Monitor configuration (ví dụ)
# output HDMI-A-1 resolution 1920x1080 position 0,0
# output eDP-1 resolution 1920x1080 position 1920,0

# ─────────────────────────────────────────────────────────────────────
# INPUT CONFIGURATION
# ─────────────────────────────────────────────────────────────────────

# Keyboard
input type:keyboard {
    xkb_layout us
    xkb_options caps:escape
    repeat_delay 300
    repeat_rate 50
}

# Touchpad
input type:touchpad {
    dwt enabled
    tap enabled
    natural_scroll enabled
    middle_emulation enabled
}

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - BASICS
# ─────────────────────────────────────────────────────────────────────

# Start terminal
bindsym $mod+Return exec $term

# Kill focused window
bindsym $mod+Shift+q kill

# Start launcher
bindsym $mod+d exec $menu

# Reload configuration
bindsym $mod+Shift+c reload

# Exit sway
bindsym $mod+Shift+e exec swaynag -t warning -m 'Exit Sway?' \
    -B 'Yes' 'swaymsg exit'

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - NAVIGATION
# ─────────────────────────────────────────────────────────────────────

# Move focus
bindsym $mod+$left focus left
bindsym $mod+$down focus down
bindsym $mod+$up focus up
bindsym $mod+$right focus right

# Alternative with arrow keys
bindsym $mod+Left focus left
bindsym $mod+Down focus down
bindsym $mod+Up focus up
bindsym $mod+Right focus right

# Move focused window
bindsym $mod+Shift+$left move left
bindsym $mod+Shift+$down move down
bindsym $mod+Shift+$up move up
bindsym $mod+Shift+$right move right

# Move with arrow keys
bindsym $mod+Shift+Left move left
bindsym $mod+Shift+Down move down
bindsym $mod+Shift+Up move up
bindsym $mod+Shift+Right move right

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - WORKSPACES
# ─────────────────────────────────────────────────────────────────────

# Switch to workspace
bindsym $mod+1 workspace number 1
bindsym $mod+2 workspace number 2
bindsym $mod+3 workspace number 3
bindsym $mod+4 workspace number 4
bindsym $mod+5 workspace number 5
bindsym $mod+6 workspace number 6
bindsym $mod+7 workspace number 7
bindsym $mod+8 workspace number 8
bindsym $mod+9 workspace number 9
bindsym $mod+0 workspace number 10

# Move focused container to workspace
bindsym $mod+Shift+1 move container to workspace number 1
bindsym $mod+Shift+2 move container to workspace number 2
bindsym $mod+Shift+3 move container to workspace number 3
bindsym $mod+Shift+4 move container to workspace number 4
bindsym $mod+Shift+5 move container to workspace number 5
bindsym $mod+Shift+6 move container to workspace number 6
bindsym $mod+Shift+7 move container to workspace number 7
bindsym $mod+Shift+8 move container to workspace number 8
bindsym $mod+Shift+9 move container to workspace number 9
bindsym $mod+Shift+0 move container to workspace number 10

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - LAYOUT
# ─────────────────────────────────────────────────────────────────────

# Split horizontal/vertical
bindsym $mod+b splith
bindsym $mod+v splitv

# Switch layout styles
bindsym $mod+s layout stacking
bindsym $mod+w layout tabbed
bindsym $mod+e layout toggle split

# Toggle fullscreen
bindsym $mod+f fullscreen

# Toggle floating
bindsym $mod+Shift+space floating toggle

# Swap focus between tiling/floating
bindsym $mod+space focus mode_toggle

# Focus parent container
bindsym $mod+a focus parent

# ─────────────────────────────────────────────────────────────────────
# RESIZE MODE
# ─────────────────────────────────────────────────────────────────────

mode "resize" {
    bindsym $left resize shrink width 10px
    bindsym $down resize grow height 10px
    bindsym $up resize shrink height 10px
    bindsym $right resize grow width 10px

    bindsym Left resize shrink width 10px
    bindsym Down resize grow height 10px
    bindsym Up resize shrink height 10px
    bindsym Right resize grow width 10px

    # Return to default mode
    bindsym Return mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+r mode "resize"

# ─────────────────────────────────────────────────────────────────────
# APPEARANCE
# ─────────────────────────────────────────────────────────────────────

# Gaps
gaps inner 10
gaps outer 5

# Border
default_border pixel 2
default_floating_border pixel 2

# Colors
# class                 border  backgr. text    indicator child_border
client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577
client.focused_inactive #333333 #5f676a #ffffff #484e50   #5f676a
client.unfocused        #333333 #222222 #888888 #292d2e   #222222
client.urgent           #2f343a #900000 #ffffff #900000   #900000

# ─────────────────────────────────────────────────────────────────────
# STATUS BAR
# ─────────────────────────────────────────────────────────────────────

bar {
    position top
    swaybar_command waybar
}

# Alternative: built-in bar
# bar {
#     position top
#     status_command while date +'%Y-%m-%d %H:%M:%S'; do sleep 1; done
#     colors {
#         statusline #ffffff
#         background #323232
#         inactive_workspace #32323200 #32323200 #5c5c5c
#     }
# }

# ─────────────────────────────────────────────────────────────────────
# AUTOSTART
# ─────────────────────────────────────────────────────────────────────

# Notification daemon
exec mako

# Idle configuration
exec swayidle -w \
    timeout 300 'swaylock -f -c 000000' \
    timeout 600 'swaymsg "output * dpms off"' \
    resume 'swaymsg "output * dpms on"' \
    before-sleep 'swaylock -f -c 000000'

# Include additional configs
include /etc/sway/config.d/*
```

---

## Xác minh cài đặt

### Kiểm tra version

```bash
# Kiểm tra Sway
sway --version
# Output: sway version 1.9

# Kiểm tra wlroots
pkg-config --modversion wlroots
# Output: 0.17.0
```

### Chạy Sway lần đầu

#### Từ TTY (khuyến nghị cho native session)

```bash
# Logout khỏi DE hiện tại
# Chuyển sang TTY (Ctrl+Alt+F3)
# Login và chạy:
sway
```

#### Nested Mode (trong X11 hoặc Wayland khác)

```bash
# Set biến môi trường cho nested
export WLR_BACKENDS=wayland
# hoặc cho X11
export WLR_BACKENDS=x11

# Chạy Sway nested
sway
```

### Troubleshooting

```bash
# Xem logs
journalctl --user -xe

# Chạy với debug output
sway -d 2>&1 | tee ~/sway.log

# Kiểm tra Wayland socket
ls -la $XDG_RUNTIME_DIR/wayland-*

# Kiểm tra GPU permissions
groups | grep -E "(video|render)"
# Nếu không có, thêm user vào group:
# sudo usermod -aG video,render $USER
```

---

## Tiếp theo

- [Sway Nested Mode](03-sway-nested-mode.md) - Chạy Sway bên trong DE khác
- [Cấu hình nâng cao](06-scripts-thuc-hanh.md) - Scripts và tips
