# Cài Đặt và Cấu Hình Sway Nested trên Ubuntu

Hướng dẫn chi tiết để cài đặt và cấu hình Sway (Wayland compositor) ở chế độ nested trên Ubuntu.

## Mục Lục

1. [Giới Thiệu](#giới-thiệu)
2. [Yêu Cầu Hệ Thống](#yêu-cầu-hệ-thống)
3. [Cài Đặt](#cài-đặt)
4. [Cấu Hình](#cấu-hình)
5. [Chạy Sway Nested](#chạy-sway-nested)
6. [Troubleshooting](#troubleshooting)

## Giới Thiệu

Sway là một Wayland compositor tương thích với i3 window manager. Chế độ "nested" cho phép chạy Sway bên trong một display server khác (như X11 hoặc Wayland compositor khác), rất hữu ích cho việc test và phát triển.

## Yêu Cầu Hệ Thống

- Ubuntu 20.04 trở lên (khuyến nghị Ubuntu 22.04 hoặc mới hơn)
- GPU hỗ trợ Wayland
- Ít nhất 2GB RAM
- Quyền sudo

## Cài Đặt

### Bước 1: Cập nhật hệ thống

```bash
sudo apt update && sudo apt upgrade -y
```

### Bước 2: Cài đặt các dependencies cần thiết

```bash
sudo apt install -y \
    build-essential \
    git \
    meson \
    ninja-build \
    cmake \
    pkg-config \
    libwayland-dev \
    libxkbcommon-dev \
    libpcre2-dev \
    libcairo2-dev \
    libpango1.0-dev \
    libgdk-pixbuf2.0-dev \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libswscale-dev \
    libswresample-dev \
    wlroots-dev \
    wayland-protocols \
    libdrm-dev \
    libgbm-dev \
    libinput-dev \
    libxcb1-dev \
    libxcb-composite0-dev \
    libxcb-xfixes0-dev \
    libxcb-render0-dev \
    libxcb-shape0-dev \
    libxcb-xinerama0-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-xkb-dev \
    libxkbcommon-x11-dev \
    libpixman-1-dev \
    scdoc
```

### Bước 3: Cài đặt Sway

#### Tùy chọn A: Cài đặt từ repository (Khuyến nghị)

```bash
# Thêm repository cho Sway
sudo apt install -y software-properties-common
sudo add-apt-repository ppa:debian-sway/backports -y
sudo apt update
sudo apt install -y sway
```

#### Tùy chọn B: Cài đặt từ source

Nếu bạn muốn cài đặt phiên bản mới nhất từ source:

```bash
# Clone repository
git clone https://github.com/swaywm/sway.git
cd sway

# Build và cài đặt
meson setup build
ninja -C build
sudo ninja -C build install
```

### Bước 4: Cài đặt các công cụ hỗ trợ

```bash
sudo apt install -y \
    swaybg \
    swayidle \
    swaylock \
    waybar \
    rofi \
    alacritty \
    firefox
```

## Cấu Hình

### Bước 1: Tạo thư mục cấu hình

```bash
mkdir -p ~/.config/sway
```

### Bước 2: Tạo file cấu hình cơ bản

File cấu hình mặc định sẽ được tạo tự động khi chạy Sway lần đầu. Bạn có thể copy từ template:

```bash
mkdir -p ~/.config/sway
cp /etc/sway/config ~/.config/sway/config
```

Hoặc tạo file cấu hình tùy chỉnh (xem file `config/sway/config` trong repo này).

### Bước 3: Cấu hình cho chế độ nested

Để chạy Sway ở chế độ nested, bạn cần chỉ định output ảo. Thêm vào file `~/.config/sway/config`:

```
output * bg ~/Pictures/wallpaper.jpg fill
```

## Chạy Sway Nested

### Cách 1: Chạy từ terminal X11

```bash
# Từ X11 session
WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 sway
```

### Cách 2: Chạy với Xwayland

```bash
# Chạy Sway nested với Xwayland support
WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 XWAYLAND=1 sway
```

### Cách 3: Sử dụng script (Khuyến nghị)

Sử dụng script `run-sway-nested.sh` có trong repo này:

```bash
chmod +x scripts/run-sway-nested.sh
./scripts/run-sway-nested.sh
```

## Cấu Hình Nâng Cao

### Thêm keybindings tùy chỉnh

Chỉnh sửa `~/.config/sway/config` để thêm các keybindings:

```
# Mod key (thường là Super/Windows key)
set $mod Mod4

# Terminal
bindsym $mod+Return exec alacritty

# Application launcher
bindsym $mod+d exec rofi -show drun

# Window management
bindsym $mod+h focus left
bindsym $mod+j focus down
bindsym $mod+k focus up
bindsym $mod+l focus right
```

### Cấu hình Waybar

Waybar là status bar cho Sway. Cấu hình tại `~/.config/waybar/config`.

## Troubleshooting

### Lỗi: "wlroots backend not found"

```bash
# Cài đặt lại wlroots
sudo apt install --reinstall wlroots-dev
```

### Lỗi: "Permission denied" khi chạy nested

Đảm bảo bạn có quyền truy cập vào các thiết bị cần thiết:

```bash
sudo usermod -aG input,video $USER
# Đăng xuất và đăng nhập lại
```

### Sway không khởi động được

Kiểm tra log:

```bash
sway -d 2> ~/sway.log
```

### Vấn đề với fonts

```bash
sudo apt install fonts-dejavu fonts-noto
fc-cache -fv
```

## Tài Liệu Tham Khảo

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [Sway Configuration](https://github.com/swaywm/sway/wiki/Configuration)
- [Wayland Documentation](https://wayland.freedesktop.org/)

## License

MIT
