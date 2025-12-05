#!/bin/bash
# Script cài đặt Sway và dependencies
# Usage: ./install-sway.sh [ubuntu|debian|arch|fedora]

set -e

DISTRO=${1:-auto}

# Màu sắc
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo $ID
    else
        echo "unknown"
    fi
}

install_ubuntu_debian() {
    echo -e "${GREEN}Installing Sway on Ubuntu/Debian...${NC}"
    
    sudo apt update
    sudo apt install -y \
        sway \
        swaybg \
        swayidle \
        swaylock \
        wl-clipboard \
        waybar \
        rofi \
        alacritty \
        mako \
        firefox \
        neovim
    
    echo -e "${GREEN}Sway installed successfully!${NC}"
}

install_arch() {
    echo -e "${GREEN}Installing Sway on Arch Linux...${NC}"
    
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm \
        sway \
        swaybg \
        swayidle \
        swaylock \
        wl-clipboard \
        waybar \
        rofi \
        alacritty \
        mako \
        firefox \
        neovim
    
    echo -e "${GREEN}Sway installed successfully!${NC}"
}

install_fedora() {
    echo -e "${GREEN}Installing Sway on Fedora...${NC}"
    
    sudo dnf update -y
    sudo dnf install -y \
        sway \
        swaybg \
        swayidle \
        swaylock \
        wl-clipboard \
        waybar \
        rofi \
        alacritty \
        mako \
        firefox \
        neovim
    
    echo -e "${GREEN}Sway installed successfully!${NC}"
}

setup_config() {
    echo -e "${YELLOW}Setting up configuration...${NC}"
    
    mkdir -p ~/.config/sway
    
    if [ ! -f ~/.config/sway/config ]; then
        echo -e "${YELLOW}Creating default config...${NC}"
        cp /etc/sway/config ~/.config/sway/config 2>/dev/null || \
        echo "# Sway config" > ~/.config/sway/config
    else
        echo -e "${YELLOW}Config already exists at ~/.config/sway/config${NC}"
    fi
    
    # Copy example config nếu có
    if [ -f "$(dirname $0)/sway-config-example" ]; then
        echo -e "${YELLOW}Example config available at: $(dirname $0)/sway-config-example${NC}"
    fi
}

# Main
if [ "$DISTRO" = "auto" ]; then
    DISTRO=$(detect_distro)
fi

echo -e "${GREEN}Detected distribution: $DISTRO${NC}"

case $DISTRO in
    ubuntu|debian)
        install_ubuntu_debian
        ;;
    arch|manjaro)
        install_arch
        ;;
    fedora)
        install_fedora
        ;;
    *)
        echo -e "${RED}Unknown distribution: $DISTRO${NC}"
        echo "Please specify: ubuntu, debian, arch, or fedora"
        exit 1
        ;;
esac

setup_config

echo -e "${GREEN}Installation complete!${NC}"
echo -e "${YELLOW}Next steps:${NC}"
echo "1. Copy example config: cp examples/sway-config-example ~/.config/sway/config"
echo "2. Edit config: nvim ~/.config/sway/config"
echo "3. Test nested: ./examples/sway-nested.sh"
echo "4. Or login to Sway from display manager"
