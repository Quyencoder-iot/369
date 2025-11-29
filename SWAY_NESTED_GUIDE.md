# 🖥️ Hướng Dẫn Cài Đặt và Cấu Hình Sway Nested trên Ubuntu

## 📋 Tổng Quan

Sway là một Wayland compositor tương thích với i3, hỗ trợ chạy ở chế độ **nested** (lồng nhau) bên trong một display server khác (X11 hoặc Wayland). Điều này rất hữu ích cho việc:

- 🧪 **Testing**: Thử nghiệm cấu hình Sway mà không cần logout
- 💻 **Development**: Phát triển ứng dụng Wayland trong môi trường an toàn
- 📚 **Learning**: Học cách sử dụng tiling window manager

## ✅ Đã Cài Đặt

### Các gói chính:
| Gói | Mô tả |
|-----|-------|
| `sway` | Wayland compositor chính |
| `swaybg` | Quản lý wallpaper |
| `swayidle` | Quản lý idle/lock |
| `swaylock` | Lock screen |

### Công cụ hỗ trợ:
| Gói | Mô tả |
|-----|-------|
| `foot` | Terminal emulator nhẹ cho Wayland |
| `waybar` | Status bar |
| `wofi` | Application launcher |
| `mako-notifier` | Notification daemon |
| `grim` | Screenshot tool |
| `slurp` | Screen area selection |
| `wl-clipboard` | Clipboard manager |
| `brightnessctl` | Brightness control |
| `xwayland` | Hỗ trợ ứng dụng X11 |

## 📁 Cấu Trúc File Cấu Hình

```
~/.config/
├── sway/
│   └── config           # Cấu hình chính của Sway
├── waybar/
│   ├── config           # Cấu hình Waybar
│   └── style.css        # Style cho Waybar
├── wofi/
│   ├── config           # Cấu hình Wofi
│   └── style.css        # Style cho Wofi
└── mako/
    └── config           # Cấu hình notification

~/.local/bin/
└── sway-nested          # Script khởi chạy Sway nested
```

## 🚀 Cách Chạy

### Cách 1: Sử dụng script (Khuyến nghị)

```bash
# Thêm ~/.local/bin vào PATH (nếu chưa có)
export PATH="$HOME/.local/bin:$PATH"

# Chạy với cài đặt mặc định
sway-nested

# Chạy với độ phân giải tùy chỉnh
sway-nested -w 1920 -h 1080

# Chạy ở chế độ debug
sway-nested -d

# Xem trợ giúp
sway-nested --help
```

### Cách 2: Chạy trực tiếp

```bash
# Trong X11
WLR_BACKENDS=x11 sway

# Trong Wayland compositor khác
WLR_BACKENDS=wayland sway

# Headless mode (không có display)
WLR_BACKENDS=headless WLR_RENDERER=pixman sway
```

## ⌨️ Phím Tắt (Keybindings)

> **Lưu ý**: Sử dụng phím `Alt` (Mod1) thay vì `Super` để tránh xung đột với host WM.

### Cơ bản
| Phím | Chức năng |
|------|-----------|
| `Alt + Return` | Mở terminal (foot) |
| `Alt + d` hoặc `Alt + Space` | Mở launcher (wofi) |
| `Alt + Shift + q` | Đóng cửa sổ đang focus |
| `Alt + Shift + c` | Reload cấu hình |
| `Alt + Shift + e` | Thoát Sway |
| `Alt + Ctrl + l` | Lock screen |

### Điều hướng
| Phím | Chức năng |
|------|-----------|
| `Alt + ←/↓/↑/→` | Di chuyển focus |
| `Alt + Shift + ←/↓/↑/→` | Di chuyển cửa sổ |
| `Alt + 1-9, 0` | Chuyển workspace |
| `Alt + Shift + 1-9, 0` | Di chuyển cửa sổ đến workspace |

### Layout
| Phím | Chức năng |
|------|-----------|
| `Alt + b` | Split ngang |
| `Alt + v` | Split dọc |
| `Alt + s` | Layout stacking |
| `Alt + w` | Layout tabbed |
| `Alt + e` | Toggle split |
| `Alt + f` | Toggle fullscreen |
| `Alt + Shift + Space` | Toggle floating |
| `Alt + Tab` | Toggle focus (tiling/floating) |

### Resize Mode
| Phím | Chức năng |
|------|-----------|
| `Alt + r` | Vào resize mode |
| `←/↓/↑/→` | Resize cửa sổ |
| `Enter / Escape` | Thoát resize mode |

### Scratchpad
| Phím | Chức năng |
|------|-----------|
| `Alt + Shift + -` | Đưa cửa sổ vào scratchpad |
| `Alt + -` | Hiện/ẩn scratchpad |

### Screenshot
| Phím | Chức năng |
|------|-----------|
| `Print` | Chụp toàn màn hình |
| `Alt + Print` | Chụp vùng chọn (lưu file) |
| `Alt + Shift + s` | Chụp vùng chọn (clipboard) |

## 🎨 Tùy Chỉnh

### Thay đổi độ phân giải
Chỉnh sửa file `~/.config/sway/config`:
```bash
output WL-1 resolution 1920x1080 position 0,0 scale 1
```

### Thay đổi wallpaper
```bash
output * bg /path/to/your/wallpaper.png fill
```

### Thay đổi theme màu
File cấu hình sử dụng theme **Tokyo Night**. Bạn có thể thay đổi các biến màu trong:
- `~/.config/sway/config` - Window borders
- `~/.config/waybar/style.css` - Status bar
- `~/.config/wofi/style.css` - Launcher
- `~/.config/mako/config` - Notifications

### Thay đổi modifier key
Nếu muốn dùng Super key thay vì Alt:
```bash
# Trong ~/.config/sway/config
set $mod Mod4  # Super key
# Thay vì
set $mod Mod1  # Alt key
```

## 🔧 Troubleshooting

### Lỗi "WLR_BACKENDS not found"
```bash
# Cài đặt thêm backend
sudo apt install libwlroots-dev
```

### Waybar không hiển thị
```bash
# Kiểm tra waybar có chạy không
pgrep waybar

# Restart waybar
pkill waybar && waybar &
```

### Wofi không mở
```bash
# Kiểm tra wofi có được cài không
which wofi

# Thử chạy thủ công
wofi --show drun
```

### Không có âm thanh
```bash
# Cài đặt PipeWire hoặc PulseAudio
sudo apt install pipewire pipewire-pulse wireplumber
```

### Ứng dụng X11 không chạy
```bash
# Kiểm tra XWayland
which Xwayland

# Đảm bảo XWayland được bật trong config
# (Mặc định đã được bật)
```

## 📚 Tài Liệu Tham Khảo

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [Sway Man Page](https://man.archlinux.org/man/sway.5)
- [Waybar Documentation](https://github.com/Alexays/Waybar/wiki)
- [i3 User's Guide](https://i3wm.org/docs/userguide.html) (Sway tương thích phần lớn)

## 🎯 Tips & Tricks

### 1. Tự động khởi động ứng dụng
Thêm vào `~/.config/sway/config`:
```bash
exec firefox
exec_always nm-applet --indicator
```

### 2. Workspace rules
```bash
# Luôn mở Firefox ở workspace 2
assign [app_id="firefox"] workspace 2
```

### 3. Window rules
```bash
# Tất cả cửa sổ của app "calculator" sẽ float
for_window [app_id="gnome-calculator"] floating enable
```

### 4. Multiple monitors (trong nested mode)
```bash
# Thêm output ảo
output HEADLESS-1 resolution 1920x1080
output HEADLESS-2 resolution 1920x1080 position 1920,0
```

---

📅 **Được tạo vào**: November 29, 2025  
🖥️ **Hệ thống**: Ubuntu 24.04.3 LTS  
🔧 **Sway version**: 1.9
