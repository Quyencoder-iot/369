# Sway Nested Mode - Hướng dẫn Cài đặt và Cấu hình trên Ubuntu

[English](#english-version) | [Tiếng Việt](#phiên-bản-tiếng-việt)

---

## Phiên bản Tiếng Việt

### 📋 Giới thiệu

Sway là một Wayland compositor tương thích với i3, cho phép bạn quản lý cửa sổ theo kiểu tiling (chia ô). Hướng dẫn này sẽ giúp bạn cài đặt và chạy Sway ở chế độ nested (lồng nhau) trên Ubuntu 24.04 LTS.

**Chế độ Nested** cho phép bạn chạy Sway bên trong một session đồ họa hiện có (X11 hoặc Wayland), rất hữu ích cho:
- Thử nghiệm Sway mà không cần đăng xuất
- Phát triển và debug cấu hình Sway
- Chạy nhiều compositor cùng lúc
- Demo và testing

### 🚀 Cài đặt Nhanh

#### 1. Cài đặt Sway và các công cụ cần thiết

Dự án này đã được cài đặt sẵn với tất cả các gói cần thiết:

```bash
# Sway và các công cụ cơ bản
- sway: Wayland compositor chính
- swayidle: Quản lý idle/lock screen
- swaylock: Lock screen
- swaybg: Quản lý background
- waybar: Status bar
- foot: Terminal emulator
- wl-clipboard: Clipboard manager
- grim, slurp: Screenshot tools
- mako-notifier: Notification daemon
- wmenu: Application launcher

# Công cụ chạy nested
- weston: Wayland compositor để chạy nested
- cage: Kiosk compositor cho nested mode
- xwayland: Hỗ trợ chạy ứng dụng X11
```

#### 2. Cài đặt cấu hình

```bash
cd /workspace/sway-config
./install-config.sh
```

Script này sẽ:
- Tạo thư mục `~/.config/sway/`
- Backup cấu hình cũ (nếu có)
- Copy cấu hình mới vào thư mục config

### 🎯 Các cách chạy Sway Nested

Có 4 phương pháp để chạy Sway ở chế độ nested:

#### 1️⃣ Nested trong Wayland (qua Weston)

**Yêu cầu:** Đang chạy trong session Wayland

```bash
cd /workspace/sway-config
./launch-sway-nested-weston.sh
```

**Mô tả:**
- Khởi động Weston làm compositor lồng nhau
- Chạy Sway bên trong cửa sổ Weston (1280x720)
- Phù hợp khi bạn đang dùng GNOME/Wayland, KDE/Wayland, v.v.

#### 2️⃣ Nested với Cage

**Yêu cầu:** Đang chạy trong session Wayland

```bash
cd /workspace/sway-config
./launch-sway-nested-cage.sh
```

**Mô tả:**
- Sử dụng Cage (kiosk compositor) để chạy nested
- Đơn giản và nhẹ hơn Weston
- Tốt cho testing và development

#### 3️⃣ Nested trong X11

**Yêu cầu:** Đang chạy trong session X11 hoặc Wayland

```bash
cd /workspace/sway-config
./launch-sway-x11.sh
```

**Mô tả:**
- Chạy Sway trong cửa sổ X11
- Sử dụng backend X11 của wlroots
- Hoạt động tốt trên các desktop environment truyền thống (GNOME/X11, XFCE, etc.)

#### 4️⃣ Headless Mode (Virtual Display)

**Yêu cầu:** Không cần session đồ họa

```bash
cd /workspace/sway-config
./launch-sway-headless.sh
```

**Mô tả:**
- Chạy Sway với virtual display
- Không có output đồ họa thực
- Dùng cho testing, scripting, hoặc remote access
- Có thể kết nối qua VNC/RDP sau

### ⌨️ Phím tắt Sway cơ bản

| Phím tắt | Chức năng |
|----------|-----------|
| `Super + Enter` | Mở terminal |
| `Super + D` | Mở launcher (wmenu) |
| `Super + Shift + Q` | Đóng cửa sổ hiện tại |
| `Super + Shift + E` | Thoát Sway |
| `Super + Shift + C` | Reload config |
| `Super + 1-9` | Chuyển workspace |
| `Super + Shift + 1-9` | Di chuyển cửa sổ sang workspace |
| `Super + H/J/K/L` | Di chuyển focus (vim-style) |
| `Super + Shift + H/J/K/L` | Di chuyển cửa sổ |
| `Super + F` | Fullscreen |
| `Super + Space` | Toggle floating |
| `Super + R` | Resize mode |
| `Super + V` | Split vertical |
| `Super + B` | Split horizontal |

**Lưu ý:** `Super` là phím Windows/Command

### 📁 Cấu trúc Thư mục

```
/workspace/
├── README.md                          # File này
└── sway-config/
    ├── config                         # Cấu hình Sway
    ├── install-config.sh              # Script cài đặt config
    ├── launch-sway-nested-weston.sh   # Chạy nested qua Weston
    ├── launch-sway-nested-cage.sh     # Chạy nested qua Cage
    ├── launch-sway-x11.sh             # Chạy nested trong X11
    └── launch-sway-headless.sh        # Chạy headless mode
```

### 🔧 Tùy chỉnh

#### Thay đổi terminal mặc định

Chỉnh sửa file `~/.config/sway/config`:

```
set $term alacritty  # hoặc gnome-terminal, kitty, etc.
```

#### Thay đổi launcher

```
set $menu rofi -show run  # hoặc launcher khác
```

#### Thêm autostart applications

Thêm vào cuối file config:

```
exec firefox
exec spotify
```

#### Cấu hình nhiều màn hình

```
output HDMI-A-1 resolution 1920x1080 position 0,0
output DP-1 resolution 2560x1440 position 1920,0
```

### 🐛 Troubleshooting

#### Vấn đề: "Error: Not running in a Wayland session"

**Giải pháp:**
- Sử dụng `launch-sway-x11.sh` nếu bạn đang dùng X11
- Hoặc đăng xuất và đăng nhập vào session Wayland

#### Vấn đề: Không thể khởi động Weston

**Giải pháp:**
```bash
# Kiểm tra Wayland display
echo $WAYLAND_DISPLAY

# Nếu rỗng, bạn không ở trong session Wayland
# Thử dùng X11 backend thay thế
./launch-sway-x11.sh
```

#### Vấn đề: Phím tắt không hoạt động

**Giải pháp:**
- Đảm bảo bạn đang focus vào cửa sổ Sway
- Kiểm tra file config có syntax error không: `sway -C -c ~/.config/sway/config`

#### Vấn đề: Font/Icon bị lỗi

**Giải pháp:**
```bash
sudo apt install fonts-font-awesome fonts-noto fonts-roboto
```

### 📚 Tài nguyên Tham khảo

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [Sway Manual](https://man.archlinux.org/man/sway.5)
- [i3 User Guide](https://i3wm.org/docs/userguide.html) (Sway tương thích với i3)
- [Awesome Sway](https://github.com/swaywm/awesome-sway) - Danh sách tools và configs

### 💡 Tips và Tricks

1. **Debug config:** `sway -d 2>&1 | tee ~/sway.log`
2. **Kiểm tra config:** `sway -C -c ~/.config/sway/config`
3. **Reload config:** `Super + Shift + C` (trong Sway)
4. **Screenshot:** `grim screenshot.png` hoặc `grim -g "$(slurp)" screenshot.png`
5. **Clipboard:** Sử dụng `wl-copy` và `wl-paste`

---

## English Version

### 📋 Introduction

Sway is an i3-compatible Wayland compositor that provides tiling window management. This guide helps you install and run Sway in nested mode on Ubuntu 24.04 LTS.

**Nested mode** allows running Sway inside an existing graphical session (X11 or Wayland), useful for:
- Testing Sway without logging out
- Developing and debugging Sway configurations
- Running multiple compositors simultaneously
- Demonstrations and testing

### 🚀 Quick Installation

#### 1. Install Sway and Required Tools

This project comes pre-installed with all necessary packages:

```bash
# Sway and basic tools
- sway: Main Wayland compositor
- swayidle: Idle/lock screen manager
- swaylock: Screen locker
- swaybg: Background manager
- waybar: Status bar
- foot: Terminal emulator
- wl-clipboard: Clipboard manager
- grim, slurp: Screenshot tools
- mako-notifier: Notification daemon
- wmenu: Application launcher

# Nested mode tools
- weston: Wayland compositor for nested mode
- cage: Kiosk compositor for nested mode
- xwayland: X11 application support
```

#### 2. Install Configuration

```bash
cd /workspace/sway-config
./install-config.sh
```

This script will:
- Create `~/.config/sway/` directory
- Backup existing config (if any)
- Copy new configuration to config directory

### 🎯 Running Sway Nested

There are 4 methods to run Sway in nested mode:

#### 1️⃣ Nested in Wayland (via Weston)

**Requires:** Running in a Wayland session

```bash
cd /workspace/sway-config
./launch-sway-nested-weston.sh
```

**Description:**
- Starts Weston as a nested compositor
- Runs Sway inside Weston window (1280x720)
- Suitable when using GNOME/Wayland, KDE/Wayland, etc.

#### 2️⃣ Nested with Cage

**Requires:** Running in a Wayland session

```bash
cd /workspace/sway-config
./launch-sway-nested-cage.sh
```

**Description:**
- Uses Cage (kiosk compositor) for nested mode
- Simpler and lighter than Weston
- Good for testing and development

#### 3️⃣ Nested in X11

**Requires:** Running in an X11 or Wayland session

```bash
cd /workspace/sway-config
./launch-sway-x11.sh
```

**Description:**
- Runs Sway in an X11 window
- Uses wlroots' X11 backend
- Works well on traditional desktop environments (GNOME/X11, XFCE, etc.)

#### 4️⃣ Headless Mode (Virtual Display)

**Requires:** No graphical session needed

```bash
cd /workspace/sway-config
./launch-sway-headless.sh
```

**Description:**
- Runs Sway with virtual display
- No actual graphical output
- Used for testing, scripting, or remote access
- Can connect via VNC/RDP later

### ⌨️ Basic Sway Keybindings

| Keybinding | Function |
|------------|----------|
| `Super + Enter` | Open terminal |
| `Super + D` | Open launcher (wmenu) |
| `Super + Shift + Q` | Close current window |
| `Super + Shift + E` | Exit Sway |
| `Super + Shift + C` | Reload config |
| `Super + 1-9` | Switch workspace |
| `Super + Shift + 1-9` | Move window to workspace |
| `Super + H/J/K/L` | Move focus (vim-style) |
| `Super + Shift + H/J/K/L` | Move window |
| `Super + F` | Fullscreen |
| `Super + Space` | Toggle floating |
| `Super + R` | Resize mode |
| `Super + V` | Split vertical |
| `Super + B` | Split horizontal |

**Note:** `Super` is the Windows/Command key

### 📁 Directory Structure

```
/workspace/
├── README.md                          # This file
└── sway-config/
    ├── config                         # Sway configuration
    ├── install-config.sh              # Config installation script
    ├── launch-sway-nested-weston.sh   # Run nested via Weston
    ├── launch-sway-nested-cage.sh     # Run nested via Cage
    ├── launch-sway-x11.sh             # Run nested in X11
    └── launch-sway-headless.sh        # Run headless mode
```

### 🔧 Customization

#### Change default terminal

Edit `~/.config/sway/config`:

```
set $term alacritty  # or gnome-terminal, kitty, etc.
```

#### Change launcher

```
set $menu rofi -show run  # or other launcher
```

#### Add autostart applications

Add to end of config file:

```
exec firefox
exec spotify
```

#### Configure multiple monitors

```
output HDMI-A-1 resolution 1920x1080 position 0,0
output DP-1 resolution 2560x1440 position 1920,0
```

### 🐛 Troubleshooting

#### Issue: "Error: Not running in a Wayland session"

**Solution:**
- Use `launch-sway-x11.sh` if you're on X11
- Or log out and log in to a Wayland session

#### Issue: Cannot start Weston

**Solution:**
```bash
# Check Wayland display
echo $WAYLAND_DISPLAY

# If empty, you're not in a Wayland session
# Try X11 backend instead
./launch-sway-x11.sh
```

#### Issue: Keybindings not working

**Solution:**
- Make sure Sway window is focused
- Check config for syntax errors: `sway -C -c ~/.config/sway/config`

#### Issue: Font/Icon problems

**Solution:**
```bash
sudo apt install fonts-font-awesome fonts-noto fonts-roboto
```

### 📚 References

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [Sway Manual](https://man.archlinux.org/man/sway.5)
- [i3 User Guide](https://i3wm.org/docs/userguide.html) (Sway is i3-compatible)
- [Awesome Sway](https://github.com/swaywm/awesome-sway) - List of tools and configs

### 💡 Tips and Tricks

1. **Debug config:** `sway -d 2>&1 | tee ~/sway.log`
2. **Check config:** `sway -C -c ~/.config/sway/config`
3. **Reload config:** `Super + Shift + C` (in Sway)
4. **Screenshot:** `grim screenshot.png` or `grim -g "$(slurp)" screenshot.png`
5. **Clipboard:** Use `wl-copy` and `wl-paste`

---

### 📝 License

This configuration and scripts are provided as-is for educational and testing purposes.

### 🤝 Contributing

Feel free to customize and improve these configurations for your needs!
