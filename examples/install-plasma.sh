#!/bin/bash
# Script cài đặt KDE Plasma
# Usage: ./install-plasma.sh [ubuntu|debian|arch|fedora]

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
    echo -e "${GREEN}Installing KDE Plasma on Ubuntu/Debian...${NC}"
    
    sudo apt update
    sudo apt install -y kde-plasma-desktop plasma-workspace-wayland
    
    # Optional: Full KDE suite
    read -p "Install full KDE applications suite? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        sudo apt install -y kde-standard
    fi
    
    echo -e "${GREEN}Plasma installed successfully!${NC}"
    echo -e "${YELLOW}Logout and select 'Plasma (Wayland)' from session menu${NC}"
}

install_arch() {
    echo -e "${GREEN}Installing KDE Plasma on Arch Linux...${NC}"
    
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm plasma-meta plasma-wayland-session
    
    # Optional: Full KDE suite
    read -p "Install full KDE applications suite? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        sudo pacman -S --noconfirm kde-applications
    fi
    
    echo -e "${GREEN}Plasma installed successfully!${NC}"
    echo -e "${YELLOW}Logout and select 'Plasma (Wayland)' from session menu${NC}"
}

install_fedora() {
    echo -e "${GREEN}Installing KDE Plasma on Fedora...${NC}"
    
    sudo dnf update -y
    sudo dnf groupinstall -y "KDE Plasma Workspaces"
    sudo dnf install -y plasma-workspace-wayland
    
    echo -e "${GREEN}Plasma installed successfully!${NC}"
    echo -e "${YELLOW}Logout and select 'Plasma (Wayland)' from session menu${NC}"
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

echo -e "${GREEN}Installation complete!${NC}"
