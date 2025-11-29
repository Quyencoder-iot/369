#!/bin/bash

# Script cài đặt Sway và các dependencies trên Ubuntu
# Sử dụng: ./scripts/install-sway.sh

set -e

echo "========================================="
echo "Cài đặt Sway và Dependencies"
echo "========================================="

# Màu sắc cho output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Kiểm tra quyền sudo
if [ "$EUID" -eq 0 ]; then 
    echo -e "${RED}Vui lòng không chạy script này với sudo${NC}"
    exit 1
fi

echo -e "${YELLOW}Bước 1: Cập nhật hệ thống...${NC}"
sudo apt update && sudo apt upgrade -y

echo -e "${YELLOW}Bước 2: Cài đặt dependencies cơ bản...${NC}"
sudo apt install -y \
    build-essential \
    git \
    meson \
    ninja-build \
    cmake \
    pkg-config \
    software-properties-common

echo -e "${YELLOW}Bước 3: Cài đặt Wayland dependencies...${NC}"
sudo apt install -y \
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

echo -e "${YELLOW}Bước 4: Cài đặt wlroots...${NC}"
sudo apt install -y wlroots-dev wayland-protocols

echo -e "${YELLOW}Bước 5: Thêm repository Sway...${NC}"
sudo add-apt-repository ppa:debian-sway/backports -y
sudo apt update

echo -e "${YELLOW}Bước 6: Cài đặt Sway...${NC}"
sudo apt install -y sway

echo -e "${YELLOW}Bước 7: Cài đặt các công cụ hỗ trợ...${NC}"
sudo apt install -y \
    swaybg \
    swayidle \
    swaylock \
    waybar \
    rofi \
    alacritty \
    firefox \
    fonts-dejavu \
    fonts-noto

echo -e "${YELLOW}Bước 8: Cập nhật font cache...${NC}"
fc-cache -fv

echo -e "${YELLOW}Bước 9: Tạo thư mục cấu hình...${NC}"
mkdir -p ~/.config/sway
mkdir -p ~/.config/waybar

echo -e "${GREEN}=========================================${NC}"
echo -e "${GREEN}Cài đặt hoàn tất!${NC}"
echo -e "${GREEN}=========================================${NC}"
echo ""
echo "Để chạy Sway nested, sử dụng:"
echo "  ./scripts/run-sway-nested.sh"
echo ""
echo "Hoặc chạy thủ công:"
echo "  WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 sway"
