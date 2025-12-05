# Hướng Dẫn Cài Đặt Sway Nested

## Mục Lục
1. [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
2. [Cài đặt trên Ubuntu](#cài-đặt-trên-ubuntu)
3. [Cài đặt từ source](#cài-đặt-từ-source)
4. [Cấu hình ban đầu](#cấu-hình-ban-đầu)
5. [Troubleshooting](#troubleshooting)

---

## Yêu Cầu Hệ Thống

### Phần Cứng Tối Thiểu

```
CPU: x86_64 dual-core (hoặc ARM64 cho Pi, etc.)
RAM: 1GB (khuyến nghị 2GB+)
GPU: Bất kỳ GPU hỗ trợ OpenGL ES 2.0+
     - Intel HD Graphics (Gen 4+)
     - AMD (với driver amdgpu hoặc radeon)
     - NVIDIA (với driver nouveau hoặc proprietary)
Disk: 500MB cho cài đặt cơ bản
```

### Phần Mềm Yêu Cầu

```
OS: Linux kernel 4.10+
Display: 
  - Wayland compositor đang chạy (cho nested mode)
  - Hoặc X11 server (sẽ dùng X11 backend)
  - Hoặc chạy từ TTY (full mode, không nested)
```

---

## Cài Đặt Trên Ubuntu

### Ubuntu 22.04 / 24.04

#### Method 1: Cài từ Repository (Đơn giản nhất)

```bash
# Update package lists
sudo apt update

# Install Sway and essential tools
sudo apt install -y \
    sway \
    swaybg \
    swaylock \
    swayidle \
    waybar \
    wdisplays \
    foot \
    wofi \
    mako-notifier \
    grim \
    slurp \
    wl-clipboard

# Optional but recommended packages
sudo apt install -y \
    alacritty \
    kitty \
    thunar \
    pavucontrol \
    brightnessctl \
    playerctl \
    network-manager-gnome
```

**Package chi tiết:**

| Package | Mô tả | Thay thế |
|---------|-------|----------|
| `sway` | Sway compositor chính | Bắt buộc |
| `swaybg` | Wallpaper manager | `mpvpaper`, `oguri` |
| `swaylock` | Screen locker | `swaylock-effects` |
| `swayidle` | Idle management daemon | - |
| `waybar` | Status bar | `swaybar`, `i3status` |
| `wdisplays` | Display configuration GUI | `kanshi` (auto) |
| `foot` | Terminal emulator | `alacritty`, `kitty` |
| `wofi` | Application launcher | `rofi`, `dmenu` |
| `mako-notifier` | Notification daemon | `dunst` |
| `grim` | Screenshot utility | - |
| `slurp` | Screen area selector | - |
| `wl-clipboard` | Clipboard utilities | - |

#### Method 2: Cài PPA mới nhất (Latest version)

```bash
# Add Sway PPA (cho version mới hơn)
# Note: PPA có thể không có cho Sway, nhưng có cho wlroots
sudo add-apt-repository ppa:wlroots-dev/wlroots-git
sudo apt update

# Install Sway
sudo apt install sway
```

#### Method 3: Snap (Universal)

```bash
# Install via Snap (isolated, may have permission issues)
sudo snap install sway --classic

# Note: Snap version có thể có vấn đề với nested mode
# do sandboxing restrictions
```

### Kiểm tra cài đặt

```bash
# Check Sway version
sway --version
# Output: sway version 1.8.1

# Check wlroots version
pkg-config --modversion wlroots
# Output: 0.16.2

# List Sway runtime files
dpkg -L sway | grep bin
# /usr/bin/sway
# /usr/bin/swaymsg
# /usr/bin/swaynag
```

---

## Cài Đặt Từ Source

### Khi nào cần build từ source?

- Bạn muốn version mới nhất (bleeding edge)
- Distro của bạn có version cũ
- Bạn muốn customize build
- Bạn muốn đóng góp code

### Dependencies

```bash
# Build dependencies
sudo apt install -y \
    meson \
    ninja-build \
    cmake \
    gcc \
    g++ \
    pkg-config \
    git

# Sway dependencies
sudo apt install -y \
    libwayland-dev \
    wayland-protocols \
    libegl1-mesa-dev \
    libgles2-mesa-dev \
    libdrm-dev \
    libgbm-dev \
    libinput-dev \
    libxkbcommon-dev \
    libudev-dev \
    libpixman-1-dev \
    libsystemd-dev \
    libseat-dev \
    libpcre2-dev \
    libjson-c-dev \
    libpango1.0-dev \
    libcairo2-dev \
    libgdk-pixbuf-2.0-dev

# Optional dependencies
sudo apt install -y \
    scdoc \
    libavutil-dev \
    libavcodec-dev \
    libavformat-dev
```

### Build wlroots (nếu cần)

```bash
# Clone wlroots
cd ~/build
git clone https://gitlab.freedesktop.org/wlroots/wlroots.git
cd wlroots

# Checkout stable version
git checkout 0.17.0

# Build
meson setup build/ --prefix=/usr/local
ninja -C build/

# Install
sudo ninja -C build/ install

# Update library cache
sudo ldconfig
```

### Build Sway

```bash
# Clone Sway
cd ~/build
git clone https://github.com/swaywm/sway.git
cd sway

# Checkout stable version (hoặc dùng main cho latest)
git checkout 1.9

# Configure build
meson setup build/ \
    --prefix=/usr/local \
    --buildtype=release \
    -Dsd-bus-provider=libsystemd \
    -Dwerror=false

# Build
ninja -C build/

# Install
sudo ninja -C build/ install

# Verify installation
/usr/local/bin/sway --version
```

### Build supporting tools

#### swaybg
```bash
cd ~/build
git clone https://github.com/swaywm/swaybg.git
cd swaybg
meson setup build/ --prefix=/usr/local
ninja -C build/
sudo ninja -C build/ install
```

#### swaylock
```bash
cd ~/build
git clone https://github.com/swaywm/swaylock.git
cd swaylock
meson setup build/ --prefix=/usr/local
ninja -C build/
sudo ninja -C build/ install
```

#### swayidle
```bash
cd ~/build
git clone https://github.com/swaywm/swayidle.git
cd swayidle
meson setup build/ --prefix=/usr/local
ninja -C build/
sudo ninja -C build/ install
```

---

## Cấu Hình Ban Đầu

### Tạo config file

```bash
# Create config directory
mkdir -p ~/.config/sway

# Copy default config
cp /etc/sway/config ~/.config/sway/config

# Hoặc nếu build from source:
cp /usr/local/etc/sway/config ~/.config/sway/config
```

### Config cơ bản cho nested mode

```bash
# Edit config
vim ~/.config/sway/config
```

**Nội dung config mẫu:**

```bash
# ~/.config/sway/config - Minimal nested config

### Variables
# Logo key (Mod4 = Super/Windows key)
set $mod Mod4

# Alternative: Use Alt for nested to avoid conflicts
# set $mod Mod1

# Terminal emulator
set $term foot
# Alternatives: alacritty, kitty, gnome-terminal

# Application launcher
set $menu wofi --show drun
# Alternatives: rofi -show drun, dmenu_path | dmenu | xargs swaymsg exec --

### Output configuration
# For nested mode, this is usually not needed
# Sway will auto-detect the nested window

### Input configuration
# Configure keyboard
input type:keyboard {
    xkb_layout us
    xkb_variant ,
    xkb_options grp:alt_shift_toggle
}

# Configure touchpad
input type:touchpad {
    tap enabled
    natural_scroll enabled
    dwt enabled
    accel_profile adaptive
}

### Key bindings
#
# Basics:
#
    # Start a terminal
    bindsym $mod+Return exec $term

    # Kill focused window
    bindsym $mod+Shift+q kill

    # Start your launcher
    bindsym $mod+d exec $menu

    # Reload the configuration file
    bindsym $mod+Shift+c reload

    # Exit sway (logs you out of your Wayland session)
    bindsym $mod+Shift+e exec swaynag -t warning -m 'Exit Sway?' -B 'Yes' 'swaymsg exit'

#
# Moving around:
#
    # Move your focus around
    bindsym $mod+Left focus left
    bindsym $mod+Down focus down
    bindsym $mod+Up focus up
    bindsym $mod+Right focus right

    # Move the focused window
    bindsym $mod+Shift+Left move left
    bindsym $mod+Shift+Down move down
    bindsym $mod+Shift+Up move up
    bindsym $mod+Shift+Right move right

#
# Workspaces:
#
    # Switch to workspace
    bindsym $mod+1 workspace number 1
    bindsym $mod+2 workspace number 2
    bindsym $mod+3 workspace number 3
    bindsym $mod+4 workspace number 4

    # Move focused container to workspace
    bindsym $mod+Shift+1 move container to workspace number 1
    bindsym $mod+Shift+2 move container to workspace number 2
    bindsym $mod+Shift+3 move container to workspace number 3
    bindsym $mod+Shift+4 move container to workspace number 4

#
# Layout stuff:
#
    # Split horizontal/vertical
    bindsym $mod+h splith
    bindsym $mod+v splitv

    # Switch layout style
    bindsym $mod+s layout stacking
    bindsym $mod+w layout tabbed
    bindsym $mod+e layout toggle split

    # Fullscreen
    bindsym $mod+f fullscreen

    # Toggle floating
    bindsym $mod+Shift+space floating toggle

    # Swap focus between tiling/floating
    bindsym $mod+space focus mode_toggle

#
# Resizing containers:
#
mode "resize" {
    bindsym Left resize shrink width 10px
    bindsym Down resize grow height 10px
    bindsym Up resize shrink height 10px
    bindsym Right resize grow width 10px

    # Return to default mode
    bindsym Return mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+r mode "resize"

### Status Bar
bar {
    position top
    status_command while date +'%Y-%m-%d %H:%M:%S'; do sleep 1; done
    
    colors {
        statusline #ffffff
        background #323232
        inactive_workspace #32323200 #32323200 #5c5c5c
    }
}

### Appearance
# Window borders
default_border pixel 2
default_floating_border pixel 2

# Gaps
gaps inner 5
gaps outer 3

# Colors
client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577
client.focused_inactive #333333 #5f676a #ffffff #484e50   #5f676a
client.unfocused        #333333 #222222 #888888 #292d2e   #222222
client.urgent           #2f343a #900000 #ffffff #900000   #900000

### Autostart (for nested, minimal)
# Background
exec_always swaybg -i ~/Pictures/wallpaper.jpg -m fill

# Notification daemon
exec mako
```

### Tạo script để launch Sway nested

```bash
# Create launch script
cat > ~/launch-sway-nested.sh << 'EOF'
#!/bin/bash

# Launch Sway in nested mode

# Set environment variables
export XDG_CURRENT_DESKTOP=sway
export XDG_SESSION_TYPE=wayland

# Optional: Set specific display size for nested window
# export WLR_WL_OUTPUT_WIDTH=1920
# export WLR_WL_OUTPUT_HEIGHT=1080

# Debug mode (optional)
# export WAYLAND_DEBUG=1
# export SWAY_DEBUG=1

# Launch Sway
# If you're in a Wayland session, it will use Wayland backend
# If you're in X11, it will use X11 backend
sway 2>&1 | tee ~/sway-nested.log
EOF

chmod +x ~/launch-sway-nested.sh
```

### Test Sway nested

```bash
# Simple test (from current desktop session)
sway

# With custom window size
WLR_WL_OUTPUT_WIDTH=1600 WLR_WL_OUTPUT_HEIGHT=900 sway

# With debug output
WAYLAND_DEBUG=1 sway 2>&1 | tee ~/sway-debug.log
```

**Nếu thành công, bạn sẽ thấy:**
- Một cửa sổ mới mở ra
- Desktop của Sway bên trong
- Status bar ở trên
- Có thể mở terminal bằng `$mod+Return`

---

## Troubleshooting

### Lỗi: "Unable to create backend"

```bash
# Error message:
00:00:00.001 [ERROR] [wlr] [backend/backend.c:217] Unable to create backend

# Nguyên nhân:
- Không có Wayland compositor hoặc X11 server đang chạy
- Bạn đang ở TTY thuần (không phải nested mode)

# Giải pháp:
# 1. Check nếu bạn đang trong desktop environment
echo $WAYLAND_DISPLAY   # Should output something like "wayland-0"
echo $DISPLAY           # Or ":0" for X11

# 2. Nếu ở TTY, bạn cần run full mode (không nested):
# Đăng nhập từ TTY, then:
sway
# Nhưng document này là về nested mode
```

### Lỗi: Keybindings không hoạt động

```bash
# Nguyên nhân:
# Host compositor đang capture keybindings trước Sway

# Giải pháp:
# Change modifier key in config
set $mod Mod1  # Use Alt instead of Super

# Hoặc disable conflicting keybindings in host
```

### Lỗi: "Permission denied" khi access devices

```bash
# Nguyên nhân:
# User không có quyền truy cập /dev/input/* hoặc /dev/dri/*

# Giải pháp (chỉ cần cho full mode, không cần cho nested):
sudo usermod -a -G input,video,render $USER
# Logout and login again
```

### Lỗi: Black screen hoặc no rendering

```bash
# Check GPU driver
lspci -k | grep -A 3 VGA

# Check EGL/OpenGL
eglinfo
glxinfo | grep "OpenGL version"

# Install proper drivers:
# For Intel:
sudo apt install intel-media-va-driver

# For AMD:
sudo apt install mesa-vulkan-drivers libgl1-mesa-dri

# For NVIDIA (proprietary):
sudo apt install nvidia-driver-535  # or latest version
```

### Lỗi: Sway crashes immediately

```bash
# Run with debug:
sway -d 2>&1 | tee ~/sway-crash.log

# Check log file
cat ~/sway-crash.log

# Common issues:
# 1. Config syntax error
sway -C  # Validate config without running

# 2. Missing dependencies
ldd $(which sway)  # Check missing libraries

# 3. Wayland socket permission
ls -l $XDG_RUNTIME_DIR/wayland-*
```

### Performance issues

```bash
# Check if running with GPU acceleration
# In Sway, open terminal and run:
glxinfo | grep "renderer"
# Should show your GPU, not "llvmpipe" (software rendering)

# If software rendering:
# 1. Install proper GPU drivers (see above)
# 2. Check if GPU is detected:
lspci | grep VGA
# 3. For nested, check host compositor's GPU usage
```

### Config không được load

```bash
# Sway looks for config in this order:
# 1. ~/.config/sway/config
# 2. ~/.config/sway/config.d/*.conf
# 3. /etc/sway/config
# 4. /etc/sway/config.d/*.conf

# Check which config is loaded:
swaymsg -t get_config

# Validate config:
sway -C -c ~/.config/sway/config
```

---

## Testing Nested Setup

### Quick test checklist

```bash
# 1. Launch Sway nested
sway

# 2. In Sway nested window:
# Press $mod+Return (open terminal)
# Should open terminal inside Sway

# 3. Check if windows tile correctly
# Open another terminal
# Should split screen automatically

# 4. Test window focus
# Press $mod+Left/Right
# Should move focus between windows

# 5. Test workspaces
# Press $mod+2
# Should switch to workspace 2

# 6. Test applications
# Press $mod+d (launcher)
# Launch firefox or any app

# 7. Exit
# Press $mod+Shift+e
# Should show exit confirmation
```

### Performance test

```bash
# Inside Sway nested, run:
glxgears -info
# Should show your GPU and high FPS

# Check compositor overhead:
# In host, run:
top -p $(pgrep sway)
# Sway should use minimal CPU when idle

# Memory usage:
ps aux | grep sway
# Should be under 100MB for basic setup
```

---

## Advanced: Multiple Nested Instances

Bạn có thể chạy nhiều Sway instances:

```bash
# Terminal 1:
sway
# Will use WAYLAND_DISPLAY=wayland-1 (or first available)

# Terminal 2 (in host, not in Sway):
sway
# Will use WAYLAND_DISPLAY=wayland-2

# Check running Sway instances:
ps aux | grep sway
ls -l $XDG_RUNTIME_DIR/wayland-*
```

---

## Next Steps

1. ✅ Sway đã cài đặt và chạy nested
2. 📖 Đọc [05-operate-sway.md](./05-operate-sway.md) để học cách vận hành
3. 🎨 Tùy chỉnh config, theme, status bar
4. 🔧 Explore các tools trong ecosystem (waybar, rofi, etc.)
5. 💪 Thực hành với [08-practical-examples.md](./08-practical-examples.md)

---

## Tài Nguyên Bổ Sung

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [Sway Man Pages](https://man.archlinux.org/man/sway.5)
- [Sway GitHub Issues](https://github.com/swaywm/sway/issues)
- [r/swaywm](https://reddit.com/r/swaywm)
- [Arch Wiki - Sway](https://wiki.archlinux.org/title/Sway)
