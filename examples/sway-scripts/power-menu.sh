#!/bin/bash
# Power menu for Sway

options="🔒 Lock\n🚪 Logout\n⏾ Suspend\n🔄 Reboot\n⏻ Shutdown"

choice=$(echo -e "$options" | wofi --dmenu --prompt "Power Menu:" --width 250 --height 200)

case "$choice" in
    "🔒 Lock")
        swaylock -f -c 000000
        ;;
    "🚪 Logout")
        swaymsg exit
        ;;
    "⏾ Suspend")
        systemctl suspend
        ;;
    "🔄 Reboot")
        systemctl reboot
        ;;
    "⏻ Shutdown")
        systemctl poweroff
        ;;
    *)
        exit 0
        ;;
esac
