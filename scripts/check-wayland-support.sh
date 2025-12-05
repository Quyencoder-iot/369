#!/bin/bash
# Script kiểm tra hỗ trợ Wayland và các protocols

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== Kiểm Tra Hỗ Trợ Wayland ===${NC}\n"

# Kiểm tra session type
echo -e "${YELLOW}1. Session Type:${NC}"
if [ -n "$XDG_SESSION_TYPE" ]; then
    echo -e "   ${GREEN}✓${NC} XDG_SESSION_TYPE=$XDG_SESSION_TYPE"
else
    echo -e "   ${RED}✗${NC} XDG_SESSION_TYPE không được set"
fi

if [ -n "$WAYLAND_DISPLAY" ]; then
    echo -e "   ${GREEN}✓${NC} WAYLAND_DISPLAY=$WAYLAND_DISPLAY"
elif [ -n "$DISPLAY" ]; then
    echo -e "   ${YELLOW}○${NC} DISPLAY=$DISPLAY (X11)"
else
    echo -e "   ${RED}✗${NC} Không có display server"
fi

# Kiểm tra các commands
echo -e "\n${YELLOW}2. Wayland Tools:${NC}"

check_command() {
    if command -v $1 &> /dev/null; then
        VERSION=$($1 --version 2>&1 | head -n1)
        echo -e "   ${GREEN}✓${NC} $1: $VERSION"
        return 0
    else
        echo -e "   ${RED}✗${NC} $1: Không tìm thấy"
        return 1
    fi
}

check_command sway
check_command wayland-info
check_command wl-clipboard
check_command wayland-scanner

# Kiểm tra Sway
echo -e "\n${YELLOW}3. Sway:${NC}"
if command -v sway &> /dev/null; then
    SWAY_VERSION=$(sway --version 2>&1 | head -n1)
    echo -e "   ${GREEN}✓${NC} $SWAY_VERSION"
    
    # Kiểm tra config
    if [ -f ~/.config/sway/config ]; then
        echo -e "   ${GREEN}✓${NC} Config file tồn tại: ~/.config/sway/config"
    else
        echo -e "   ${YELLOW}○${NC} Config file không tồn tại"
    fi
else
    echo -e "   ${RED}✗${NC} Sway chưa được cài đặt"
fi

# Kiểm tra Plasma
echo -e "\n${YELLOW}4. Plasma:${NC}"
if command -v plasmashell &> /dev/null; then
    echo -e "   ${GREEN}✓${NC} Plasma shell được cài đặt"
    
    # Kiểm tra Wayland session
    if [ -d /usr/share/wayland-sessions ] || [ -d ~/.local/share/wayland-sessions ]; then
        echo -e "   ${GREEN}✓${NC} Wayland sessions có sẵn"
    fi
else
    echo -e "   ${YELLOW}○${NC} Plasma shell không được tìm thấy"
fi

# Kiểm tra Layer Shell support
echo -e "\n${YELLOW}5. Layer Shell Support:${NC}"
if command -v wayland-info &> /dev/null; then
    if wayland-info 2>&1 | grep -qi "layer"; then
        echo -e "   ${GREEN}✓${NC} Layer Shell protocol được hỗ trợ"
    else
        echo -e "   ${YELLOW}○${NC} Không thể xác định Layer Shell support"
    fi
else
    echo -e "   ${YELLOW}○${NC} Cần wayland-info để kiểm tra"
fi

# Kiểm tra GPU
echo -e "\n${YELLOW}6. GPU/OpenGL:${NC}"
if command -v glxinfo &> /dev/null; then
    GPU=$(glxinfo | grep "OpenGL renderer" | cut -d: -f2 | xargs)
    echo -e "   ${GREEN}✓${NC} GPU: $GPU"
elif command -v vulkaninfo &> /dev/null; then
    echo -e "   ${GREEN}✓${NC} Vulkan có sẵn"
else
    echo -e "   ${YELLOW}○${NC} Không thể kiểm tra GPU (cài mesa-utils để kiểm tra)"
fi

# Kiểm tra XWayland
echo -e "\n${YELLOW}7. XWayland:${NC}"
if command -v Xwayland &> /dev/null; then
    XWAYLAND_VERSION=$(Xwayland -version 2>&1 | head -n1)
    echo -e "   ${GREEN}✓${NC} $XWAYLAND_VERSION"
else
    echo -e "   ${YELLOW}○${NC} XWayland không được tìm thấy"
fi

# Tổng kết
echo -e "\n${BLUE}=== Tổng Kết ===${NC}"
echo "Để chạy Sway nested, bạn cần:"
echo "  1. Sway đã được cài đặt"
echo "  2. Một display server (X11 hoặc Wayland)"
echo "  3. GPU hỗ trợ OpenGL/Vulkan"
echo ""
echo "Chạy: sway-nested hoặc WLR_BACKENDS=x11 sway"
