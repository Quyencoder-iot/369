# Hướng Dẫn Sway Nested và KDE Plasma

Tài liệu hướng dẫn đầy đủ về **Sway** (Wayland compositor), **Nested Mode**, và tích hợp với **KDE Plasma**.

## 📚 Mục lục Tài liệu

| Tài liệu | Nội dung |
|----------|----------|
| [01. Tổng quan](docs/01-tong-quan.md) | Giới thiệu Wayland, Sway, Nested Mode, Plasma |
| [02. Cài đặt Sway](docs/02-cai-dat-sway.md) | Hướng dẫn cài đặt trên Ubuntu/Debian/Fedora/Arch |
| [03. Sway Nested Mode](docs/03-sway-nested-mode.md) | Chi tiết về nested compositing |
| [04. KDE Plasma](docs/04-kde-plasma.md) | Tích hợp Sway với Plasma |
| [05. Kiến trúc Wayland](docs/05-wayland-architecture.md) | Hiểu sâu về Wayland protocol |
| [06. Scripts & Thực hành](docs/06-scripts-thuc-hanh.md) | Scripts, configs, bài tập |

## 🚀 Quick Start

### Cài đặt nhanh trên Ubuntu

```bash
# Cài đặt Sway và các tools cần thiết
sudo apt install sway foot wofi waybar wl-clipboard grim slurp mako-notifier

# Copy configs
mkdir -p ~/.config/sway ~/.config/waybar
cp configs/sway-config-nested ~/.config/sway/config.nested
cp configs/waybar-config ~/.config/waybar/config
cp configs/waybar-style.css ~/.config/waybar/style.css

# Copy scripts
mkdir -p ~/bin
cp scripts/* ~/bin/
chmod +x ~/bin/sway-*

# Chạy Sway nested
~/bin/sway-nested
```

### Chạy Sway Nested

```bash
# Trong Wayland session (Plasma Wayland, GNOME Wayland)
./scripts/sway-nested

# Với debug
./scripts/sway-nested --debug

# Trong X11 session
WLR_BACKENDS=x11 sway -c configs/sway-config-nested
```

## 📁 Cấu trúc thư mục

```
.
├── docs/                          # Tài liệu hướng dẫn
│   ├── 01-tong-quan.md           # Tổng quan
│   ├── 02-cai-dat-sway.md        # Cài đặt
│   ├── 03-sway-nested-mode.md    # Nested mode
│   ├── 04-kde-plasma.md          # Plasma integration
│   ├── 05-wayland-architecture.md # Wayland deep dive
│   └── 06-scripts-thuc-hanh.md   # Scripts & practice
│
├── scripts/                       # Scripts tiện ích
│   ├── sway-nested               # Launcher cho nested mode
│   ├── sway-screenshot           # Screenshot tool
│   ├── sway-window-select        # Window switcher
│   └── sway-power                # Power menu
│
├── configs/                       # Config files mẫu
│   ├── sway-config-nested        # Sway config cho nested
│   ├── waybar-config             # Waybar config
│   └── waybar-style.css          # Waybar style
│
└── README.md                      # File này
```

## 🔑 Key Bindings (Nested Mode)

Sử dụng **Alt (Mod1)** thay vì Super để tránh xung đột với host compositor.

| Phím tắt | Chức năng |
|----------|-----------|
| `Alt + Enter` | Mở terminal |
| `Alt + d` | App launcher |
| `Alt + Shift + q` | Đóng cửa sổ |
| `Alt + Shift + e` | Thoát Sway |
| `Alt + h/j/k/l` | Di chuyển focus |
| `Alt + 1-9` | Chuyển workspace |
| `Alt + f` | Fullscreen |
| `Alt + b/v` | Split horizontal/vertical |
| `Alt + r` | Resize mode |

## 🎯 Use Cases

### 1. Development Environment
Chạy Sway nested trong Plasma để có tiling layout cho coding:
```bash
# Workspace 2: Development (Sway)
WLR_BACKENDS=wayland sway -c ~/.config/sway/config.nested
```

### 2. Learning Tiling WM
An toàn học Sway mà không cần logout:
```bash
./scripts/sway-nested --debug
```

### 3. Testing Wayland Apps
Test ứng dụng trong isolated environment:
```bash
WLR_BACKENDS=wayland sway
# Trong Sway, chạy app cần test
```

## 📖 Đọc thêm

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots)
- [Wayland Book](https://wayland-book.com/)
- [KDE Plasma](https://kde.org/plasma-desktop/)

## ⚡ Kiến trúc

```
┌─────────────────────────────────────────────────────────────────┐
│                    Host Compositor                               │
│                  (KDE Plasma / GNOME)                           │
│                                                                  │
│   ┌─────────────────────────────────────────────────────────┐   │
│   │              Sway (Nested Compositor)                    │   │
│   │                                                          │   │
│   │   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │   │
│   │   │   Terminal  │  │   Browser   │  │   Editor    │     │   │
│   │   │   (foot)    │  │  (firefox)  │  │   (nvim)    │     │   │
│   │   └─────────────┘  └─────────────┘  └─────────────┘     │   │
│   │                                                          │   │
│   │   wlroots Wayland Backend                                │   │
│   └─────────────────────────────────────────────────────────┘   │
│                                                                  │
│   Other Plasma windows (Dolphin, System Settings, etc.)         │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## 📝 License

Tài liệu này được tạo cho mục đích học tập.
