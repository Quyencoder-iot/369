#!/bin/bash

# Script cài đặt dependencies cho Sway nested trên Ubuntu
# Sử dụng: chmod +x install-dependencies.sh && sudo ./install-dependencies.sh

set -e

echo "=========================================="
echo "Cài đặt Dependencies cho Sway Nested"
echo "=========================================="
echo ""

# Kiểm tra quyền sudo
if [ "$EUID" -ne 0 ]; then 
    echo "Lỗi: Script này cần quyền sudo. Vui lòng chạy: sudo $0"
    exit 1
fi

# Cập nhật package list
echo "[1/3] Cập nhật package list..."
apt update

# Cài đặt dependencies cơ bản
echo ""
echo "[2/3] Cài đặt build tools và dependencies cơ bản..."
apt install -y \
    build-essential \
    git \
    meson \
    ninja-build \
    cmake \
    pkg-config \
    scdoc

# Cài đặt Wayland và wlroots dependencies
echo ""
echo "[3/3] Cài đặt Wayland và wlroots dependencies..."
apt install -y \
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
    libpixman-1-dev

# Cài đặt Sway (tùy chọn)
echo ""
read -p "Bạn có muốn cài đặt Sway từ repository không? (y/n): " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Cài đặt Sway..."
    apt install -y sway
    echo "Sway đã được cài đặt!"
else
    echo "Bỏ qua cài đặt Sway. Bạn có thể cài đặt sau bằng: sudo apt install -y sway"
fi

# Cài đặt Xephyr (tùy chọn, để chạy nested trong X11 window)
echo ""
read -p "Bạn có muốn cài đặt Xephyr để chạy Sway nested trong X11 window không? (y/n): " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Cài đặt Xephyr..."
    apt install -y xserver-xephyr
    echo "Xephyr đã được cài đặt!"
fi

echo ""
echo "=========================================="
echo "Hoàn thành! Tất cả dependencies đã được cài đặt."
echo "=========================================="
echo ""
echo "Bước tiếp theo:"
echo "1. Tạo thư mục cấu hình: mkdir -p ~/.config/sway"
echo "2. Copy file cấu hình: cp sway-config-nested ~/.config/sway/config"
echo "3. Chạy Sway nested: ./run-sway-nested.sh"
echo ""
