#!/bin/bash
# Script khởi động Sway Nested
# Usage: ./sway-nested.sh

# Màu sắc cho output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Starting Sway Nested...${NC}"

# Kiểm tra xem đã có session Wayland/X11 chưa
if [ -z "$WAYLAND_DISPLAY" ] && [ -z "$DISPLAY" ]; then
    echo -e "${RED}Error: No display session found!${NC}"
    echo "Please run this script from within a Wayland or X11 session."
    exit 1
fi

# Kiểm tra Sway đã được cài đặt chưa
if ! command -v sway &> /dev/null; then
    echo -e "${RED}Error: Sway is not installed!${NC}"
    echo "Please install Sway first."
    exit 1
fi

# Set environment variables cho nested mode
export WLR_BACKENDS=headless
export WLR_LIBINPUT_NO_DEVICES=1

# Optional: Set output size
export WLR_HEADLESS_OUTPUTS=1

# Optional: Set resolution
# export WLR_HEADLESS_OUTPUT_WIDTH=1920
# export WLR_HEADLESS_OUTPUT_HEIGHT=1080

# Config file (có thể override)
CONFIG_FILE="${SWAY_CONFIG:-$HOME/.config/sway/config}"

echo -e "${YELLOW}Using config: $CONFIG_FILE${NC}"
echo -e "${YELLOW}Press Mod+Shift+E to exit Sway nested${NC}"

# Chạy Sway nested
if [ -f "$CONFIG_FILE" ]; then
    sway -c "$CONFIG_FILE"
else
    echo -e "${YELLOW}Config file not found, using default${NC}"
    sway
fi
