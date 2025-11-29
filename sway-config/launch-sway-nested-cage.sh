#!/bin/bash
# Launch Sway in nested mode using Cage
# Cage is a Wayland kiosk compositor that can run nested

# Set up configuration
CONFIG_DIR="$(dirname "$0")"
export SWAY_CONFIG="${CONFIG_DIR}/config"

# Check if we're already in a Wayland session
if [ -z "$WAYLAND_DISPLAY" ]; then
    echo "Error: Not running in a Wayland session."
    echo "Please run this from within a Wayland compositor."
    exit 1
fi

# Launch Sway using Cage as a nested compositor
echo "Launching Sway in Cage (nested mode)..."
cage -d -- sway -c "$SWAY_CONFIG"

echo "Sway nested session ended."
