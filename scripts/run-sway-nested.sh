#!/bin/bash

# Script chạy Sway ở chế độ nested
# Sử dụng: ./scripts/run-sway-nested.sh

set -e

# Màu sắc cho output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Đang khởi động Sway ở chế độ nested...${NC}"
echo -e "${YELLOW}Nhấn Mod+Shift+E để thoát${NC}"
echo ""

# Kiểm tra xem Sway đã được cài đặt chưa
if ! command -v sway &> /dev/null; then
    echo -e "${RED}Lỗi: Sway chưa được cài đặt!${NC}"
    echo -e "${YELLOW}Chạy ./scripts/install-sway.sh để cài đặt${NC}"
    exit 1
fi

# Tạo thư mục cấu hình nếu chưa có
mkdir -p ~/.config/sway

# Kiểm tra file cấu hình
if [ ! -f ~/.config/sway/config ]; then
    echo -e "${YELLOW}Tạo file cấu hình mặc định...${NC}"
    if [ -f /etc/sway/config ]; then
        cp /etc/sway/config ~/.config/sway/config
    else
        echo -e "${YELLOW}Không tìm thấy config mặc định, tạo file cơ bản...${NC}"
        # Tạo config cơ bản
        cat > ~/.config/sway/config << 'EOF'
# Default config for Sway
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

# Exit Sway
bindsym $mod+Shift+e exec swaynag -t warning -m 'Bạn có muốn thoát Sway?' -b 'Có' 'swaymsg exit'

# Reload config
bindsym $mod+Shift+c reload

# Lock screen
bindsym $mod+x exec swaylock

# Output configuration for nested mode
output * bg #000000 solid_color
EOF
    fi
fi

# Chạy Sway nested
# WLR_BACKENDS=headless: Sử dụng headless backend (không cần GPU thật)
# WLR_LIBINPUT_NO_DEVICES=1: Không sử dụng thiết bị input thật
# XWAYLAND=1: Bật Xwayland để chạy ứng dụng X11
export WLR_BACKENDS=headless
export WLR_LIBINPUT_NO_DEVICES=1
export XWAYLAND=1

echo -e "${GREEN}Khởi động Sway...${NC}"
sway
