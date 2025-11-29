#!/bin/bash
# Install Sway configuration to user's home directory

CONFIG_DIR="$(dirname "$0")"
INSTALL_DIR="$HOME/.config/sway"

echo "Installing Sway configuration..."
echo "Source: $CONFIG_DIR/config"
echo "Destination: $INSTALL_DIR/config"
echo ""

# Create config directory if it doesn't exist
mkdir -p "$INSTALL_DIR"

# Backup existing config if present
if [ -f "$INSTALL_DIR/config" ]; then
    BACKUP="$INSTALL_DIR/config.backup.$(date +%Y%m%d_%H%M%S)"
    echo "Backing up existing config to: $BACKUP"
    cp "$INSTALL_DIR/config" "$BACKUP"
fi

# Copy new config
cp "$CONFIG_DIR/config" "$INSTALL_DIR/config"

echo ""
echo "✓ Configuration installed successfully!"
echo ""
echo "You can now:"
echo "  1. Run Sway normally: sway"
echo "  2. Run Sway nested (choose a method):"
echo "     - ./launch-sway-nested-weston.sh (requires Wayland session)"
echo "     - ./launch-sway-nested-cage.sh (requires Wayland session)"
echo "     - ./launch-sway-x11.sh (requires X11 or Wayland session)"
echo "     - ./launch-sway-headless.sh (virtual display, for testing)"
