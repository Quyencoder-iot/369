#!/bin/bash
# Launch Sway in nested mode using Weston
# This script starts Weston as a nested Wayland compositor,
# then runs Sway inside it

# Set up configuration
CONFIG_DIR="$(dirname "$0")"
export SWAY_CONFIG="${CONFIG_DIR}/config"

# Check if we're already in a Wayland session
if [ -z "$WAYLAND_DISPLAY" ]; then
    echo "Error: Not running in a Wayland session."
    echo "Please run this from within a Wayland compositor."
    exit 1
fi

# Start Weston in nested mode (windowed)
echo "Starting Weston in nested mode..."
weston --width=1280 --height=720 &
WESTON_PID=$!

# Wait for Weston to start
sleep 2

# Get the Wayland display socket created by Weston
# Weston typically creates wayland-1, wayland-2, etc.
for socket in /run/user/$(id -u)/wayland-*; do
    if [ -S "$socket" ] && [ "$socket" != "/run/user/$(id -u)/$WAYLAND_DISPLAY" ]; then
        NESTED_DISPLAY=$(basename "$socket")
        break
    fi
done

if [ -z "$NESTED_DISPLAY" ]; then
    echo "Error: Could not find nested Wayland display"
    kill $WESTON_PID 2>/dev/null
    exit 1
fi

echo "Found nested display: $NESTED_DISPLAY"

# Launch Sway in the nested Weston session
echo "Launching Sway in nested mode..."
WAYLAND_DISPLAY=$NESTED_DISPLAY sway -c "$SWAY_CONFIG"

# Clean up: kill Weston when Sway exits
kill $WESTON_PID 2>/dev/null

echo "Sway nested session ended."
