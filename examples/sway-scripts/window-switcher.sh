#!/bin/bash
# Fuzzy window switcher for Sway using wofi

# Get list of windows with their IDs, app_id/class, and titles
windows=$(swaymsg -t get_tree | jq -r '
    .. | 
    select(.type?) | 
    select(.type == "con") | 
    select(.name != null) |
    select(.app_id != null or .window_properties.class != null) |
    "\(.id)\t\(.app_id // .window_properties.class)\t\(.name)"
')

# If no windows found
if [ -z "$windows" ]; then
    notify-send "Window Switcher" "No windows found"
    exit 0
fi

# Format for wofi display
display=$(echo "$windows" | awk -F'\t' '{printf "[%s] %s\n", $2, $3}')

# Show in wofi and get selection
selected=$(echo "$display" | wofi --dmenu --prompt "Switch to window:")

# If nothing selected, exit
if [ -z "$selected" ]; then
    exit 0
fi

# Extract window ID from original list based on selection
window_title=$(echo "$selected" | sed 's/^\[.*\] //')
window_id=$(echo "$windows" | grep -F "$window_title" | head -1 | cut -f1)

# Focus the window
if [ -n "$window_id" ]; then
    swaymsg "[con_id=$window_id] focus"
fi
