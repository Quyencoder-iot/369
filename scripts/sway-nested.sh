#!/bin/bash
# Script để chạy Sway nested trong môi trường hiện tại

set -e

# Màu sắc cho output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Sway Nested Launcher ===${NC}"

# Kiểm tra sway đã được cài đặt chưa
if ! command -v sway &> /dev/null; then
    echo -e "${RED}Lỗi: Sway chưa được cài đặt!${NC}"
    echo "Cài đặt với: sudo apt install sway (Ubuntu/Debian)"
    exit 1
fi

# Xác định môi trường hiện tại
if [ -n "$WAYLAND_DISPLAY" ]; then
    echo -e "${YELLOW}Phát hiện: Đang chạy trong Wayland${NC}"
    BACKEND="headless"
    echo "Sử dụng headless backend cho nested compositor"
elif [ -n "$DISPLAY" ]; then
    echo -e "${YELLOW}Phát hiện: Đang chạy trong X11${NC}"
    BACKEND="x11"
    echo "Sử dụng X11 backend"
else
    echo -e "${RED}Lỗi: Không tìm thấy display server!${NC}"
    exit 1
fi

# Đường dẫn config
CONFIG_FILE="${HOME}/.config/sway/config"

# Kiểm tra config file
if [ ! -f "$CONFIG_FILE" ]; then
    echo -e "${YELLOW}Cảnh báo: Không tìm thấy config tại $CONFIG_FILE${NC}"
    echo "Tạo config mặc định..."
    mkdir -p "${HOME}/.config/sway"
    cat > "$CONFIG_FILE" << 'EOF'
# Sway config cơ bản cho nested
set $mod Mod4

# Terminal
set $term alacritty
set $menu rofi -show drun

# Keybindings
bindsym $mod+Return exec $term
bindsym $mod+d exec $menu
bindsym $mod+Shift+q kill
bindsym $mod+Shift+e exec swaymsg exit

# Focus
bindsym $mod+j focus left
bindsym $mod+k focus down
bindsym $mod+l focus up
bindsym $mod+semicolon focus right

# Layout
bindsym $mod+e layout toggle split
bindsym $mod+Shift+space floating toggle

# Gaps
gaps inner 10
gaps outer 5

# Colors
client.focused          #4c7899 #285577 #ffffff
client.focused_inactive #333333 #5f676a #ffffff
client.unfocused        #333333 #222222 #888888
EOF
    echo -e "${GREEN}Đã tạo config tại $CONFIG_FILE${NC}"
fi

# Thiết lập biến môi trường
export WLR_BACKENDS="$BACKEND"
export WLR_LIBINPUT_NO_DEVICES=1

# Thông báo
echo -e "${GREEN}Bắt đầu Sway nested...${NC}"
echo "Nhấn Mod+Shift+E để thoát"
echo ""

# Chạy Sway
exec sway -c "$CONFIG_FILE" "$@"
