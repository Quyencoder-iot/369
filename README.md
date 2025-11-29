# CÀI ĐẶT VÀ CẤU HÌNH SWAY NESTED TRÊN UBUNTU

Hướng dẫn chi tiết để cài đặt và cấu hình Sway nested trên Ubuntu. Sway là một Wayland compositor, được thiết kế như một thay thế tương thích với i3 window manager cho X11. Chế độ nested cho phép chạy Sway bên trong một môi trường display server hiện có (như X11 hoặc Wayland compositor khác), rất hữu ích cho việc testing và phát triển.

## Mục Lục

1. [Yêu Cầu Hệ Thống](#yêu-cầu-hệ-thống)
2. [Cài Đặt Dependencies](#cài-đặt-dependencies)
3. [Cài Đặt Sway](#cài-đặt-sway)
4. [Cấu Hình Sway Nested](#cấu-hình-sway-nested)
5. [Chạy Sway Nested](#chạy-sway-nested)
6. [Cấu Hình Cơ Bản](#cấu-hình-cơ-bản)
7. [Troubleshooting](#troubleshooting)

## Yêu Cầu Hệ Thống

- Ubuntu 20.04 LTS trở lên (khuyến nghị Ubuntu 22.04 hoặc mới hơn)
- Đã cài đặt X11 hoặc Wayland compositor khác
- Quyền sudo để cài đặt packages
- Ít nhất 2GB RAM (khuyến nghị 4GB trở lên)

## Cài Đặt Dependencies

Sway yêu cầu một số dependencies cần thiết. Bạn có thể sử dụng script tự động hoặc cài đặt thủ công:

### Cách 1: Sử dụng Script Tự Động

```bash
chmod +x install-dependencies.sh
sudo ./install-dependencies.sh
```

### Cách 2: Cài Đặt Thủ Công

```bash
# Cập nhật package list
sudo apt update

# Cài đặt các dependencies cần thiết
sudo apt install -y \
    build-essential \
    git \
    meson \
    ninja-build \
    cmake \
    libwayland-dev \
    libxkbcommon-dev \
    libpango1.0-dev \
    libcairo2-dev \
    libgdk-pixbuf2.0-dev \
    libwlroots-dev \
    wayland-protocols \
    libinput-dev \
    libxcb1-dev \
    libxcb-render0-dev \
    libxcb-render-util0-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-shape0-dev \
    libxcb-util-dev \
    libxcb-xinput-dev \
    libxcb-dri3-dev \
    libgbm-dev \
    libsystemd-dev \
    libpixman-1-dev \
    scdoc \
    pkg-config
```

## Cài Đặt Sway

### Cách 1: Cài Đặt từ Ubuntu Repository (Khuyến Nghị)

```bash
sudo apt install -y sway
```

### Cách 2: Cài Đặt từ Source (Cho phiên bản mới nhất)

```bash
# Clone repository
git clone https://github.com/swaywm/sway.git
cd sway

# Build và cài đặt
meson setup build
ninja -C build
sudo ninja -C build install
```

## Cấu Hình Sway Nested

### Tạo Thư Mục Cấu Hình

```bash
mkdir -p ~/.config/sway
```

### File Cấu Hình Mẫu

File cấu hình mẫu đã được tạo tại `sway-config-nested`. Bạn có thể copy vào thư mục cấu hình:

```bash
cp sway-config-nested ~/.config/sway/config
```

Hoặc chỉnh sửa file cấu hình theo nhu cầu của bạn. File cấu hình mẫu bao gồm:

- Cấu hình cho nested mode
- Keybindings cơ bản (giống i3)
- Cấu hình output cho nested window
- Cấu hình workspace và window management

## Chạy Sway Nested

### Sử dụng Script Khởi Chạy

Script `run-sway-nested.sh` đã được tạo để khởi chạy Sway nested một cách dễ dàng:

```bash
chmod +x run-sway-nested.sh
./run-sway-nested.sh
```

### Chạy Thủ Công

Để chạy Sway nested, bạn cần chỉ định `WAYLAND_DISPLAY` và chạy với flag `-C`:

```bash
WAYLAND_DISPLAY=wayland-1 sway -C ~/.config/sway/config
```

Hoặc nếu bạn muốn chỉ định display cụ thể:

```bash
WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 sway -C ~/.config/sway/config
```

### Chạy trong X11 Window

Để chạy Sway nested trong một X11 window (ví dụ: Xephyr):

```bash
# Cài đặt Xephyr
sudo apt install -y xserver-xephyr

# Chạy Xephyr
Xephyr -br -ac -noreset -screen 1280x720 :1 &
DISPLAY=:1 WAYLAND_DISPLAY=wayland-1 sway
```

## Cấu Hình Cơ Bản

### Keybindings Mặc Định

- `Mod` (thường là `Super` hoặc `Alt`) + `Enter`: Mở terminal
- `Mod` + `d`: Mở application launcher
- `Mod` + `Shift` + `q`: Đóng window
- `Mod` + `j/k`: Chuyển focus giữa các windows
- `Mod` + `Shift` + `j/k`: Di chuyển window
- `Mod` + `h/l`: Thay đổi kích thước window
- `Mod` + `1-9`: Chuyển workspace
- `Mod` + `Shift` + `1-9`: Di chuyển window đến workspace

### Cấu Hình Output cho Nested Mode

Trong file cấu hình, bạn có thể chỉnh sửa phần output:

```
output * {
    mode 1920x1080
    scale 1
}
```

### Cấu Hình Terminal

Mặc định Sway sử dụng `foot` terminal. Nếu bạn muốn sử dụng terminal khác (như `gnome-terminal` hoặc `alacritty`), chỉnh sửa trong file config:

```
set $term gnome-terminal
# hoặc
set $term alacritty
```

## Troubleshooting

### Lỗi: "Cannot connect to wayland compositor"

- Đảm bảo bạn đã chỉ định `WAYLAND_DISPLAY` đúng cách
- Kiểm tra xem có process Sway nào đang chạy không: `ps aux | grep sway`
- Thử kill các process Sway cũ: `pkill sway`

### Lỗi: "Permission denied"

- Đảm bảo bạn có quyền truy cập vào `/dev/dri/` (thường cần thêm user vào group `video`):
  ```bash
  sudo usermod -aG video $USER
  ```
- Logout và login lại sau khi thêm vào group

### Lỗi: "wlroots version mismatch"

- Cài đặt lại `libwlroots-dev` với phiên bản tương thích
- Hoặc build Sway từ source với cùng version wlroots

### Sway không khởi động được

- Kiểm tra log: `sway -C ~/.config/sway/config -d 2>&1 | tee sway.log`
- Kiểm tra cú pháp file config: `swaymsg -t get_config`
- Đảm bảo tất cả dependencies đã được cài đặt đầy đủ

### Window không hiển thị đúng

- Kiểm tra cấu hình output trong file config
- Thử điều chỉnh scale factor: `output * scale 1.5`
- Kiểm tra resolution của nested window

## Tài Liệu Tham Khảo

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [Sway Configuration](https://github.com/swaywm/sway/wiki/Configuration)
- [Wayland Documentation](https://wayland.freedesktop.org/)
- [wlroots Documentation](https://gitlab.freedesktop.org/wlroots/wlroots)

## Ghi Chú

- Sway nested mode chủ yếu dùng cho testing và development
- Hiệu năng có thể không tốt bằng chạy Sway trực tiếp trên hardware
- Một số tính năng có thể không hoạt động đầy đủ trong nested mode
- Để sử dụng Sway như window manager chính, bạn nên chạy nó trực tiếp trên TTY thay vì nested mode

## License

Hướng dẫn này được cung cấp miễn phí để sử dụng và chỉnh sửa theo nhu cầu của bạn.
