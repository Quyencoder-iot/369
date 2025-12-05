#!/bin/bash
# Toggle between light and dark theme in KDE Plasma

current_scheme=$(kreadconfig5 --file kdeglobals --group General --key ColorScheme)

if [ "$current_scheme" = "Breeze" ] || [ "$current_scheme" = "BreezeLight" ]; then
    # Switch to dark
    echo "Switching to dark theme..."
    plasma-apply-colorscheme BreezeDark
    plasma-apply-lookandfeel -a org.kde.breezedark.desktop
    notify-send "Theme" "Switched to Dark Mode"
else
    # Switch to light
    echo "Switching to light theme..."
    plasma-apply-colorscheme Breeze
    plasma-apply-lookandfeel -a org.kde.breeze.desktop
    notify-send "Theme" "Switched to Light Mode"
fi

# Optional: Restart plasmashell for immediate effect
# killall plasmashell && kstart5 plasmashell &
