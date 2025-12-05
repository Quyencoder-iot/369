#!/bin/bash
# ═══════════════════════════════════════════════════════════════════
#                    SWAY NESTED INSTALLER
# ═══════════════════════════════════════════════════════════════════
# Script cài đặt Sway và các công cụ liên quan

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log() { echo -e "${GREEN}[✓]${NC} $1"; }
warn() { echo -e "${YELLOW}[!]${NC} $1"; }
error() { echo -e "${RED}[✗]${NC} $1"; exit 1; }
info() { echo -e "${BLUE}[i]${NC} $1"; }

# ─────────────────────────────────────────────────────────────────────
# DETECT DISTRO
# ─────────────────────────────────────────────────────────────────────

detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo "$ID"
    else
        echo "unknown"
    fi
}

# ─────────────────────────────────────────────────────────────────────
# INSTALL PACKAGES
# ─────────────────────────────────────────────────────────────────────

install_packages() {
    local distro=$(detect_distro)
    info "Detected distro: $distro"
    
    case $distro in
        ubuntu|debian|linuxmint|pop)
            info "Installing packages with apt..."
            sudo apt update
            sudo apt install -y \
                sway \
                swaylock \
                swayidle \
                swaybg \
                foot \
                wofi \
                waybar \
                wl-clipboard \
                mako-notifier \
                grim \
                slurp \
                jq \
                brightnessctl \
                playerctl
            ;;
        fedora)
            info "Installing packages with dnf..."
            sudo dnf install -y \
                sway \
                swaylock \
                swayidle \
                swaybg \
                foot \
                wofi \
                waybar \
                wl-clipboard \
                mako \
                grim \
                slurp \
                jq \
                brightnessctl \
                playerctl
            ;;
        arch|manjaro|endeavouros)
            info "Installing packages with pacman..."
            sudo pacman -S --noconfirm \
                sway \
                swaylock \
                swayidle \
                swaybg \
                foot \
                wofi \
                waybar \
                wl-clipboard \
                mako \
                grim \
                slurp \
                jq \
                brightnessctl \
                playerctl
            ;;
        *)
            error "Unsupported distribution: $distro"
            ;;
    esac
    
    log "Packages installed successfully"
}

# ─────────────────────────────────────────────────────────────────────
# INSTALL CONFIGS
# ─────────────────────────────────────────────────────────────────────

install_configs() {
    local script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    
    # Sway config
    info "Installing Sway config..."
    mkdir -p ~/.config/sway
    if [ -f ~/.config/sway/config.nested ]; then
        warn "Backup existing config.nested"
        cp ~/.config/sway/config.nested ~/.config/sway/config.nested.bak
    fi
    cp "$script_dir/configs/sway-config-nested" ~/.config/sway/config.nested
    log "Sway config installed"
    
    # Waybar config
    info "Installing Waybar config..."
    mkdir -p ~/.config/waybar
    if [ -f ~/.config/waybar/config ]; then
        warn "Backup existing waybar config"
        cp ~/.config/waybar/config ~/.config/waybar/config.bak
    fi
    cp "$script_dir/configs/waybar-config" ~/.config/waybar/config
    cp "$script_dir/configs/waybar-style.css" ~/.config/waybar/style.css
    log "Waybar config installed"
}

# ─────────────────────────────────────────────────────────────────────
# INSTALL SCRIPTS
# ─────────────────────────────────────────────────────────────────────

install_scripts() {
    local script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    
    info "Installing scripts..."
    mkdir -p ~/bin
    
    for script in "$script_dir/scripts/"*; do
        if [ -f "$script" ]; then
            cp "$script" ~/bin/
            chmod +x ~/bin/$(basename "$script")
            log "Installed $(basename "$script")"
        fi
    done
    
    # Add ~/bin to PATH if not already
    if [[ ":$PATH:" != *":$HOME/bin:"* ]]; then
        warn "Adding ~/bin to PATH in ~/.bashrc"
        echo 'export PATH="$HOME/bin:$PATH"' >> ~/.bashrc
    fi
    
    log "Scripts installed to ~/bin"
}

# ─────────────────────────────────────────────────────────────────────
# MAIN MENU
# ─────────────────────────────────────────────────────────────────────

show_menu() {
    echo ""
    echo "╔═══════════════════════════════════════════════════════════╗"
    echo "║            SWAY NESTED INSTALLER                          ║"
    echo "╠═══════════════════════════════════════════════════════════╣"
    echo "║  1. Install everything (packages + configs + scripts)     ║"
    echo "║  2. Install packages only                                 ║"
    echo "║  3. Install configs only                                  ║"
    echo "║  4. Install scripts only                                  ║"
    echo "║  5. Show post-install instructions                        ║"
    echo "║  0. Exit                                                  ║"
    echo "╚═══════════════════════════════════════════════════════════╝"
    echo ""
}

post_install() {
    echo ""
    echo "╔═══════════════════════════════════════════════════════════╗"
    echo "║                  POST-INSTALL INSTRUCTIONS                 ║"
    echo "╠═══════════════════════════════════════════════════════════╣"
    echo "║                                                           ║"
    echo "║  1. Reload your shell:                                    ║"
    echo "║     $ source ~/.bashrc                                    ║"
    echo "║                                                           ║"
    echo "║  2. Run Sway nested:                                      ║"
    echo "║     $ sway-nested                                         ║"
    echo "║                                                           ║"
    echo "║  3. Key bindings (Alt + ...):                             ║"
    echo "║     - Enter     : Terminal                                ║"
    echo "║     - d         : App launcher                            ║"
    echo "║     - Shift+q   : Close window                            ║"
    echo "║     - Shift+e   : Exit Sway                               ║"
    echo "║     - h/j/k/l   : Navigate                                ║"
    echo "║     - 1-9       : Workspaces                              ║"
    echo "║                                                           ║"
    echo "║  4. Documentation: ./docs/                                ║"
    echo "║                                                           ║"
    echo "╚═══════════════════════════════════════════════════════════╝"
    echo ""
}

# ─────────────────────────────────────────────────────────────────────
# MAIN
# ─────────────────────────────────────────────────────────────────────

main() {
    # Check if running interactively
    if [ -t 0 ]; then
        while true; do
            show_menu
            read -p "Select option [0-5]: " choice
            
            case $choice in
                1)
                    install_packages
                    install_configs
                    install_scripts
                    post_install
                    ;;
                2)
                    install_packages
                    ;;
                3)
                    install_configs
                    ;;
                4)
                    install_scripts
                    ;;
                5)
                    post_install
                    ;;
                0)
                    echo "Goodbye!"
                    exit 0
                    ;;
                *)
                    warn "Invalid option"
                    ;;
            esac
        done
    else
        # Non-interactive: install everything
        install_packages
        install_configs
        install_scripts
        post_install
    fi
}

main "$@"
