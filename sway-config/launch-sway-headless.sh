#!/bin/bash
# Launch Sway in headless mode (for testing)
# This creates a virtual output without needing a physical display

# Set up configuration
CONFIG_DIR="$(dirname "$0")"
export SWAY_CONFIG="${CONFIG_DIR}/config"

echo "Launching Sway in headless mode..."
echo "Note: This creates a virtual display for testing purposes"
echo ""

# Set environment variables for headless mode
export WLR_BACKENDS=headless
export WLR_LIBINPUT_NO_DEVICES=1

# Launch Sway with headless backend
sway -c "$SWAY_CONFIG"

echo "Sway headless session ended."
