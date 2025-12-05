#!/bin/bash
# Script cài đặt Sway nested và dependencies

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== Cài Đặt Sway Nested ===${NC}"

# Phát hiện distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
else
    echo -e "${RED}Không thể phát hiện distribution${NC}"
    exit 1
fi

echo -e "${YELLOW}Phát hiện: $DISTRO${NC}"

# Function để cài đặt trên Ubuntu/Debian
install_debian() {
    echo -e "${GREEN}Cài đặt cho Ubuntu/Debian...${NC}"
    sudo apt update
    sudo apt install -y \
        sway \
        swaybg \
        swayidle \
        swaylock \
        waybar \
        wl-clipboard \
        xdg-desktop-portal-wlr \
        xwayland \
        alacritty \
        rofi \
        fonts-dejavu-core
}

# Function để cài đặt trên Arch
install_arch() {
    echo -e "${GREEN}Cài đặt cho Arch Linux...${NC}"
    sudo pacman -S --noconfirm \
        sway \
        swaybg \
        swayidle \
        swaylock \
        waybar \
        wl-clipboard \
        xdg-desktop-portal-wlr \
        xwayland \
        alacritty \
        rofi \
        ttf-dejavu
}

# Function để cài đặt trên Fedora
install_fedora() {
    echo -e "${GREEN}Cài đặt cho Fedora...${NC}"
    sudo dnf install -y \
        sway \
        swaybg \
        swayidle \
        swaylock \
        waybar \
        wl-clipboard \
        xdg-desktop-portal-wlr \
        xwayland \
        alacritty \
        rofi \
        dejavu-sans-fonts
}

# Cài đặt theo distribution
case $DISTRO in
    ubuntu|debian)
        install_debian
        ;;
    arch|manjaro)
        install_arch
        ;;
    fedora)
        install_fedora
        ;;
    *)
        echo -e "${RED}Distribution không được hỗ trợ: $DISTRO${NC}"
        echo "Vui lòng cài đặt thủ công các gói:"
        echo "  - sway"
        echo "  - swaybg, swayidle, swaylock"
        echo "  - waybar"
        echo "  - wl-clipboard"
        echo "  - xdg-desktop-portal-wlr"
        echo "  - xwayland"
        exit 1
        ;;
esac

# Tạo thư mục config
echo -e "${YELLOW}Tạo thư mục cấu hình...${NC}"
mkdir -p ~/.config/sway
mkdir -p ~/bin

# Copy script launcher
if [ -f "scripts/sway-nested.sh" ]; then
    cp scripts/sway-nested.sh ~/bin/sway-nested
    chmod +x ~/bin/sway-nested
    echo -e "${GREEN}Đã cài đặt script launcher tại ~/bin/sway-nested${NC}"
fi

# Kiểm tra PATH
if [[ ":$PATH:" != *":$HOME/bin:"* ]]; then
    echo -e "${YELLOW}Thêm ~/bin vào PATH...${NC}"
    echo 'export PATH="$HOME/bin:$PATH"' >> ~/.bashrc
    echo -e "${GREEN}Đã thêm ~/bin vào PATH (cần reload shell)${NC}"
fi

echo -e "${GREEN}=== Cài Đặt Hoàn Tất ===${NC}"
echo ""
echo "Để chạy Sway nested:"
echo "  sway-nested"
echo ""
echo "Hoặc:"
echo "  ~/bin/sway-nested"
