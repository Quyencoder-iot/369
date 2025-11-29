#!/bin/bash

# Script khởi chạy Sway nested trên Ubuntu
# Sử dụng: chmod +x run-sway-nested.sh && ./run-sway-nested.sh

set -e

# Màu sắc cho output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=========================================="
echo "Khởi chạy Sway Nested"
echo "==========================================${NC}"
echo ""

# Kiểm tra xem Sway đã được cài đặt chưa
if ! command -v sway &> /dev/null; then
    echo -e "${RED}Lỗi: Sway chưa được cài đặt.${NC}"
    echo "Vui lòng cài đặt Sway trước:"
    echo "  sudo apt install -y sway"
    echo "Hoặc chạy script cài đặt dependencies:"
    echo "  sudo ./install-dependencies.sh"
    exit 1
fi

# Kiểm tra file cấu hình
CONFIG_FILE="$HOME/.config/sway/config"
if [ ! -f "$CONFIG_FILE" ]; then
    echo -e "${YELLOW}Cảnh báo: File cấu hình không tồn tại tại $CONFIG_FILE${NC}"
    echo "Tạo thư mục cấu hình và copy file mẫu..."
    mkdir -p "$HOME/.config/sway"
    
    if [ -f "sway-config-nested" ]; then
        cp sway-config-nested "$CONFIG_FILE"
        echo -e "${GREEN}Đã copy file cấu hình mẫu vào $CONFIG_FILE${NC}"
    else
        echo -e "${RED}Lỗi: Không tìm thấy file sway-config-nested${NC}"
        echo "Vui lòng đảm bảo file sway-config-nested tồn tại trong thư mục hiện tại."
        exit 1
    fi
fi

# Kiểm tra xem có process Sway nào đang chạy không
if pgrep -x sway > /dev/null; then
    echo -e "${YELLOW}Cảnh báo: Có process Sway đang chạy.${NC}"
    read -p "Bạn có muốn kill các process Sway cũ không? (y/n): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        pkill sway
        sleep 1
        echo -e "${GREEN}Đã kill các process Sway cũ.${NC}"
    fi
fi

# Chọn phương thức chạy
echo ""
echo "Chọn phương thức chạy Sway nested:"
echo "1) Chạy trực tiếp với WAYLAND_DISPLAY (mặc định)"
echo "2) Chạy với headless backend"
echo "3) Chạy trong Xephyr window (yêu cầu Xephyr)"
echo ""
read -p "Lựa chọn (1-3, mặc định: 1): " choice
choice=${choice:-1}

case $choice in
    1)
        echo -e "${GREEN}Khởi chạy Sway nested với WAYLAND_DISPLAY...${NC}"
        WAYLAND_DISPLAY=wayland-1 sway -C "$CONFIG_FILE"
        ;;
    2)
        echo -e "${GREEN}Khởi chạy Sway nested với headless backend...${NC}"
        WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 sway -C "$CONFIG_FILE"
        ;;
    3)
        if ! command -v Xephyr &> /dev/null; then
            echo -e "${RED}Lỗi: Xephyr chưa được cài đặt.${NC}"
            echo "Cài đặt Xephyr: sudo apt install -y xserver-xephyr"
            exit 1
        fi
        echo -e "${GREEN}Khởi chạy Xephyr và Sway nested...${NC}"
        Xephyr -br -ac -noreset -screen 1280x720 :1 &
        sleep 2
        DISPLAY=:1 WAYLAND_DISPLAY=wayland-1 sway -C "$CONFIG_FILE"
        ;;
    *)
        echo -e "${RED}Lựa chọn không hợp lệ.${NC}"
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}Sway nested đã được khởi chạy!${NC}"
echo ""
echo "Để thoát Sway, nhấn: Mod+Shift+E"
echo "Hoặc trong terminal: swaymsg exit"
