#!/bin/bash
# Launch Sway nested in X11 (using XWayland backend)
# This allows running Sway as a window inside an X11 session

# Set up configuration
CONFIG_DIR="$(dirname "$0")"
export SWAY_CONFIG="${CONFIG_DIR}/config"

# Check if we're in an X11 session
if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then
    echo "Error: Not running in a graphical session."
    echo "Please run this from within X11 or Wayland."
    exit 1
fi

echo "Launching Sway in nested X11 mode..."
echo "Window resolution: 1280x720"
echo ""

# Set environment variables for X11 backend
export WLR_BACKENDS=x11
export WLR_WL_OUTPUTS=1

# Launch Sway with X11 backend
sway -c "$SWAY_CONFIG"

echo "Sway nested session ended."
