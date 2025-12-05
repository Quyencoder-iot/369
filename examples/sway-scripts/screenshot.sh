#!/bin/bash
# Screenshot tool for Sway
# Usage: ./screenshot.sh [fullscreen|area|window]

SCREENSHOT_DIR="$HOME/Pictures"
FILENAME="screenshot-$(date +%Y%m%d-%H%M%S).png"
FILEPATH="$SCREENSHOT_DIR/$FILENAME"

# Ensure directory exists
mkdir -p "$SCREENSHOT_DIR"

case "${1:-area}" in
    fullscreen|full)
        grim "$FILEPATH"
        notify-send "Screenshot" "Fullscreen saved to $FILENAME"
        ;;
    area|region)
        grim -g "$(slurp)" "$FILEPATH"
        notify-send "Screenshot" "Region saved to $FILENAME"
        ;;
    window)
        # Get focused window geometry
        window_rect=$(swaymsg -t get_tree | jq -r '
            .. | select(.focused?) | .rect | 
            "\(.x),\(.y) \(.width)x\(.height)"
        ')
        grim -g "$window_rect" "$FILEPATH"
        notify-send "Screenshot" "Window saved to $FILENAME"
        ;;
    clipboard|clip)
        grim -g "$(slurp)" - | wl-copy
        notify-send "Screenshot" "Copied to clipboard"
        ;;
    *)
        echo "Usage: $0 [fullscreen|area|window|clipboard]"
        exit 1
        ;;
esac
