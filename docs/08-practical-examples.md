# Ví Dụ và Scripts Thực Hành

## Mục Lục
1. [Sway Configuration Examples](#sway-configuration-examples)
2. [Sway Scripts](#sway-scripts)
3. [Plasma Automation](#plasma-automation)
4. [Comparison Workflows](#comparison-workflows)
5. [Troubleshooting Scripts](#troubleshooting-scripts)

---

## Sway Configuration Examples

### Example 1: Complete Sway Config

```bash
# ~/.config/sway/config
# Comprehensive Sway configuration

### Variables
set $mod Mod4
set $alt Mod1

# Directions
set $left h
set $down j
set $up k
set $right l

# Terminal
set $term foot

# Launcher
set $menu wofi --show drun | xargs swaymsg exec --

### Output configuration
# Get output names: swaymsg -t get_outputs

# Laptop screen
output eDP-1 {
    resolution 1920x1080
    position 0,0
    scale 1
    bg ~/Pictures/wallpapers/laptop.jpg fill
}

# External monitor
output HDMI-A-1 {
    resolution 2560x1440
    position 1920,0
    scale 1
    bg ~/Pictures/wallpapers/external.jpg fill
    adaptive_sync on
}

# Disable unused outputs
output DP-1 disable

### Idle configuration
exec swayidle -w \
    timeout 300 'swaylock -f -c 000000' \
    timeout 600 'swaymsg "output * dpms off"' \
         resume 'swaymsg "output * dpms on"' \
    before-sleep 'swaylock -f -c 000000'

### Input configuration
input type:keyboard {
    xkb_layout us,ru
    xkb_options grp:alt_shift_toggle,caps:escape
    repeat_delay 300
    repeat_rate 50
}

input type:touchpad {
    dwt enabled
    tap enabled
    natural_scroll enabled
    middle_emulation enabled
    accel_profile adaptive
    pointer_accel 0.3
}

input type:pointer {
    accel_profile flat
}

### Key bindings

## Basics
bindsym $mod+Return exec $term
bindsym $mod+Shift+q kill
bindsym $mod+d exec $menu
bindsym $mod+Shift+c reload
bindsym $mod+Shift+e exec swaynag -t warning -m 'Exit?' \
    -B 'Yes' 'swaymsg exit'

## Movement
bindsym $mod+$left focus left
bindsym $mod+$down focus down
bindsym $mod+$up focus up
bindsym $mod+$right focus right

bindsym $mod+Left focus left
bindsym $mod+Down focus down
bindsym $mod+Up focus up
bindsym $mod+Right focus right

bindsym $mod+Shift+$left move left
bindsym $mod+Shift+$down move down
bindsym $mod+Shift+$up move up
bindsym $mod+Shift+$right move right

bindsym $mod+Shift+Left move left
bindsym $mod+Shift+Down move down
bindsym $mod+Shift+Up move up
bindsym $mod+Shift+Right move right

## Workspaces
bindsym $mod+1 workspace number 1
bindsym $mod+2 workspace number 2
bindsym $mod+3 workspace number 3
bindsym $mod+4 workspace number 4
bindsym $mod+5 workspace number 5
bindsym $mod+6 workspace number 6
bindsym $mod+7 workspace number 7
bindsym $mod+8 workspace number 8
bindsym $mod+9 workspace number 9
bindsym $mod+0 workspace number 10

bindsym $mod+Shift+1 move container to workspace number 1
bindsym $mod+Shift+2 move container to workspace number 2
bindsym $mod+Shift+3 move container to workspace number 3
bindsym $mod+Shift+4 move container to workspace number 4
bindsym $mod+Shift+5 move container to workspace number 5
bindsym $mod+Shift+6 move container to workspace number 6
bindsym $mod+Shift+7 move container to workspace number 7
bindsym $mod+Shift+8 move container to workspace number 8
bindsym $mod+Shift+9 move container to workspace number 9
bindsym $mod+Shift+0 move container to workspace number 10

## Layout
bindsym $mod+b splith
bindsym $mod+v splitv
bindsym $mod+s layout stacking
bindsym $mod+w layout tabbed
bindsym $mod+e layout toggle split
bindsym $mod+f fullscreen
bindsym $mod+Shift+space floating toggle
bindsym $mod+space focus mode_toggle
bindsym $mod+a focus parent

## Scratchpad
bindsym $mod+Shift+minus move scratchpad
bindsym $mod+minus scratchpad show

## Resize mode
mode "resize" {
    bindsym $left resize shrink width 10px
    bindsym $down resize grow height 10px
    bindsym $up resize shrink height 10px
    bindsym $right resize grow width 10px

    bindsym Left resize shrink width 10px
    bindsym Down resize grow height 10px
    bindsym Up resize shrink height 10px
    bindsym Right resize grow width 10px

    bindsym Return mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+r mode "resize"

## Multimedia keys
bindsym XF86AudioRaiseVolume exec pactl set-sink-volume @DEFAULT_SINK@ +5%
bindsym XF86AudioLowerVolume exec pactl set-sink-volume @DEFAULT_SINK@ -5%
bindsym XF86AudioMute exec pactl set-sink-mute @DEFAULT_SINK@ toggle
bindsym XF86AudioMicMute exec pactl set-source-mute @DEFAULT_SOURCE@ toggle
bindsym XF86MonBrightnessDown exec brightnessctl set 5%-
bindsym XF86MonBrightnessUp exec brightnessctl set 5%+
bindsym XF86AudioPlay exec playerctl play-pause
bindsym XF86AudioNext exec playerctl next
bindsym XF86AudioPrev exec playerctl previous

## Screenshots
bindsym Print exec grim ~/Pictures/screenshot-$(date +%Y%m%d-%H%M%S).png
bindsym Shift+Print exec grim -g "$(slurp)" ~/Pictures/screenshot-$(date +%Y%m%d-%H%M%S).png
bindsym Ctrl+Print exec grim -g "$(slurp)" - | wl-copy

## Custom shortcuts
bindsym $mod+n exec makoctl dismiss
bindsym $mod+Shift+n exec makoctl dismiss -a
bindsym $mod+c exec clipman pick -t wofi
bindsym $mod+period exec wofi-emoji

### Window rules
for_window [app_id="pavucontrol"] floating enable, resize set 800 600
for_window [app_id="blueman-manager"] floating enable
for_window [title="Picture-in-Picture"] floating enable, sticky enable
for_window [app_id="firefox" title="Firefox — Sharing Indicator"] kill

# Workspace assignments
assign [app_id="firefox"] 2
assign [app_id="telegram"] 3
assign [class="Spotify"] 4

# Inhibit idle for fullscreen windows
for_window [class="^.*"] inhibit_idle fullscreen
for_window [app_id="^.*"] inhibit_idle fullscreen

### Appearance
gaps inner 5
gaps outer 2
smart_gaps on
default_border pixel 2
default_floating_border normal
hide_edge_borders smart

# Font
font pango:JetBrains Mono 10

# Colors (Nord theme)
set $bg #2e3440
set $fg #d8dee9
set $cyan #8fbcbb
set $red #bf616a

client.focused $cyan $cyan $bg $cyan $cyan
client.focused_inactive $bg $bg $fg $bg $bg
client.unfocused $bg $bg $fg $bg $bg
client.urgent $red $red $bg $red $red

### Status Bar (using waybar)
bar {
    swaybar_command waybar
}

### Autostart
exec mako
exec nm-applet --indicator
exec blueman-applet
exec wl-paste -t text --watch clipman store
exec gammastep -l 40.7:-74.0
exec_always autotiling

include /etc/sway/config.d/*
```

### Example 2: Multi-Monitor Setup

```bash
# ~/.config/sway/config.d/monitors.conf

# Define monitors
set $laptop eDP-1
set $external HDMI-A-1

# Laptop only (unplugged)
output $laptop {
    pos 0 0
    res 1920x1080@60Hz
}

# External only (lid closed or external priority)
output $external {
    pos 0 0
    res 2560x1440@144Hz
    adaptive_sync on
}

# Both monitors (side by side)
# Laptop on left, external on right
output $laptop pos 0 0
output $external pos 1920 0

# Workspace assignments for multi-monitor
workspace 1 output $laptop
workspace 2 output $laptop
workspace 3 output $external
workspace 4 output $external
workspace 5 output $external

# Bind for moving workspaces between monitors
bindsym $mod+Ctrl+Left move workspace to output left
bindsym $mod+Ctrl+Right move workspace to output right
bindsym $mod+Ctrl+Up move workspace to output up
bindsym $mod+Ctrl+Down move workspace to output down

# Clamshell mode detection
bindswitch --reload --locked lid:on output $laptop disable
bindswitch --reload --locked lid:off output $laptop enable
```

---

## Sway Scripts

### Script 1: Auto-tiling

```bash
#!/usr/bin/env python3
# ~/.config/sway/scripts/autotiling.py
# Auto split horizontal/vertical based on window dimensions

import i3ipc

def on_window_focus(i3, e):
    """Set split direction based on window dimensions"""
    focused = i3.get_tree().find_focused()
    
    if focused.rect.width > focused.rect.height:
        # Landscape: split vertically (new window below)
        i3.command('splitv')
    else:
        # Portrait: split horizontally (new window to the right)
        i3.command('splith')

def main():
    i3 = i3ipc.Connection()
    i3.on('window::focus', on_window_focus)
    i3.main()

if __name__ == '__main__':
    main()
```

Install dependencies and run:
```bash
pip3 install i3ipc
# Add to sway config:
# exec_always ~/.config/sway/scripts/autotiling.py
```

### Script 2: Workspace Switcher

```bash
#!/bin/bash
# ~/.config/sway/scripts/workspace-switcher.sh
# Fuzzy workspace switcher using wofi

workspaces=$(swaymsg -t get_workspaces | jq -r '.[] | "\(.num): \(.name)"')
selected=$(echo "$workspaces" | wofi --dmenu --prompt "Switch to workspace:")

if [ -n "$selected" ]; then
    workspace_num=$(echo "$selected" | cut -d: -f1)
    swaymsg workspace number "$workspace_num"
fi
```

Bind in config:
```bash
bindsym $mod+o exec ~/.config/sway/scripts/workspace-switcher.sh
```

### Script 3: Window Switcher

```bash
#!/bin/bash
# ~/.config/sway/scripts/window-switcher.sh

windows=$(swaymsg -t get_tree | jq -r '
    .. | 
    select(.type?) | 
    select(.type == "con") | 
    select(.app_id != null or .window_properties.class != null) |
    "\(.id) \(.app_id // .window_properties.class): \(.name)"
')

selected=$(echo "$windows" | wofi --dmenu --prompt "Focus window:")

if [ -n "$selected" ]; then
    window_id=$(echo "$selected" | awk '{print $1}')
    swaymsg "[con_id=$window_id] focus"
fi
```

### Script 4: Screenshot Tool

```bash
#!/bin/bash
# ~/.config/sway/scripts/screenshot.sh

choice=$(echo -e "Fullscreen\nArea\nWindow\nOutput" | wofi --dmenu --prompt "Screenshot:")

filename="$HOME/Pictures/screenshot-$(date +%Y%m%d-%H%M%S).png"

case "$choice" in
    "Fullscreen")
        grim "$filename"
        notify-send "Screenshot" "Saved to $filename"
        ;;
    "Area")
        grim -g "$(slurp)" "$filename"
        notify-send "Screenshot" "Saved to $filename"
        ;;
    "Window")
        window_rect=$(swaymsg -t get_tree | jq -r '.. | select(.focused?) | .rect | "\(.x),\(.y) \(.width)x\(.height)"')
        grim -g "$window_rect" "$filename"
        notify-send "Screenshot" "Saved to $filename"
        ;;
    "Output")
        output=$(swaymsg -t get_outputs | jq -r '.[].name' | wofi --dmenu --prompt "Select output:")
        grim -o "$output" "$filename"
        notify-send "Screenshot" "Saved to $filename"
        ;;
esac

# Copy to clipboard option
if [ -f "$filename" ]; then
    if echo -e "Yes\nNo" | wofi --dmenu --prompt "Copy to clipboard?" | grep -q "Yes"; then
        wl-copy < "$filename"
    fi
fi
```

### Script 5: Power Menu

```bash
#!/bin/bash
# ~/.config/sway/scripts/power-menu.sh

options="Lock\nLogout\nSuspend\nReboot\nShutdown"
choice=$(echo -e "$options" | wofi --dmenu --prompt "Power:")

case "$choice" in
    "Lock")
        swaylock -f -c 000000
        ;;
    "Logout")
        swaymsg exit
        ;;
    "Suspend")
        systemctl suspend
        ;;
    "Reboot")
        systemctl reboot
        ;;
    "Shutdown")
        systemctl poweroff
        ;;
esac
```

Bind in config:
```bash
bindsym $mod+Shift+p exec ~/.config/sway/scripts/power-menu.sh
```

### Script 6: Dynamic Wallpaper

```bash
#!/bin/bash
# ~/.config/sway/scripts/wallpaper-cycle.sh
# Cycle wallpaper every 5 minutes

WALLPAPER_DIR="$HOME/Pictures/Wallpapers"

while true; do
    wallpaper=$(find "$WALLPAPER_DIR" -type f | shuf -n 1)
    swaymsg output '*' bg "$wallpaper" fill
    sleep 300  # 5 minutes
done
```

Start in config:
```bash
exec ~/.config/sway/scripts/wallpaper-cycle.sh
```

---

## Plasma Automation

### Script 1: KWin Script - Center Window

```javascript
// ~/.local/share/kwin/scripts/center-window/contents/code/main.js

workspace.clientAdded.connect(function(client) {
    if (client.dialog || client.splash) {
        // Center dialogs and splash screens
        client.geometry = {
            x: (workspace.clientArea(KWin.ScreenArea, client).width - client.width) / 2,
            y: (workspace.clientArea(KWin.ScreenArea, client).height - client.height) / 2,
            width: client.width,
            height: client.height
        };
    }
});
```

```desktop
# ~/.local/share/kwin/scripts/center-window/metadata.desktop
[Desktop Entry]
Name=Center Window
Description=Automatically center dialog windows
Type=Service

X-Plasma-API=javascript
X-Plasma-MainScript=code/main.js
X-KDE-PluginInfo-Name=center-window
X-KDE-PluginInfo-Version=1.0
X-KDE-PluginInfo-License=GPL
X-KDE-PluginInfo-EnabledByDefault=true
```

Enable:
```bash
kwriteconfig5 --file kwinrc --group Plugins --key center-windowEnabled true
qdbus org.kde.KWin /KWin reconfigure
```

### Script 2: Plasma Applet - Quick Notes

```qml
// ~/.local/share/plasma/plasmoids/org.kde.quicknotes/contents/ui/main.qml

import QtQuick 2.15
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PlasmaComponents

Item {
    id: root
    
    property string notes: plasmoid.configuration.notes
    
    Plasmoid.preferredRepresentation: Plasmoid.compactRepresentation
    
    Plasmoid.compactRepresentation: PlasmaComponents.Button {
        icon.name: "document-edit"
        text: "Notes"
        onClicked: plasmoid.expanded = !plasmoid.expanded
    }
    
    Plasmoid.fullRepresentation: PlasmaComponents.TextArea {
        id: notesArea
        text: root.notes
        onTextChanged: {
            root.notes = text
            plasmoid.configuration.notes = text
        }
        
        Layout.minimumWidth: 300
        Layout.minimumHeight: 200
    }
}
```

### Script 3: Plasma Theme Switcher

```bash
#!/bin/bash
# ~/.local/bin/plasma-theme-toggle.sh

current_theme=$(kreadconfig5 --file kdeglobals --group General --key ColorScheme)

if [ "$current_theme" = "Breeze" ]; then
    # Switch to dark
    plasma-apply-colorscheme BreezeDark
    plasma-apply-lookandfeel -a org.kde.breezedark.desktop
else
    # Switch to light
    plasma-apply-colorscheme Breeze
    plasma-apply-lookandfeel -a org.kde.breeze.desktop
fi
```

Add shortcut:
```bash
kwriteconfig5 --file kglobalshortcutsrc --group "org.kde.krunner.desktop" --key "toggle-theme" "Meta+T,none,Toggle Theme"
```

### Script 4: Automated Workspace Setup

```bash
#!/bin/bash
# ~/.local/bin/setup-workspaces.sh
# Setup development workspace layout

# Desktop 1: Browser
qdbus org.kde.kwin /KWin/desktops setDesktopName 1 "Browser"
firefox &
sleep 2
wmctrl -r firefox -t 0

# Desktop 2: Code
qdbus org.kde.kwin /KWin/desktops setDesktopName 2 "Code"
code &
sleep 2
wmctrl -r "Visual Studio Code" -t 1

# Desktop 3: Terminal
qdbus org.kde.kwin /KWin/desktops setDesktopName 3 "Terminal"
konsole &
sleep 1
wmctrl -r konsole -t 2

# Desktop 4: Communication
qdbus org.kde.kwin /KWin/desktops setDesktopName 4 "Chat"
telegram-desktop &
sleep 2
wmctrl -r telegram -t 3
```

---

## Comparison Workflows

### Workflow 1: Development Setup

**Sway:**
```bash
# ~/.config/sway/config
# Development workspace layout

# Workspace 1: Browser
assign [app_id="firefox"] 1

# Workspace 2: Code
assign [app_id="code"] 2

# Workspace 3: Terminal
assign [app_id="alacritty"] 3
for_window [app_id="alacritty"] layout tabbed

# Auto-start dev environment
exec firefox
exec code
exec alacritty
```

**Plasma:**
```bash
# System Settings → Window Management → Window Rules

# Rule 1: Firefox to Desktop 1
# Rule 2: VSCode to Desktop 2
# Rule 3: Konsole to Desktop 3, Tiled

# Or use KWin script for automatic layout
```

### Workflow 2: Window Tiling

**Sway (native):**
```bash
# Automatic tiling
Mod+Return # Open terminals
# They automatically tile

# Manual control
Mod+h      # Split horizontal
Mod+v      # Split vertical
Mod+s      # Stacking layout
Mod+e      # Toggle layout
```

**Plasma (via Bismuth):**
```bash
# Install Bismuth KWin script
# System Settings → KWin Scripts → Bismuth

# Shortcuts
Meta+T     # Cycle layouts
Meta+[/]   # Resize
Meta+Shift+Space # Toggle floating
```

### Workflow 3: Screenshot

**Sway:**
```bash
# Full screen
grim ~/Pictures/screenshot.png

# Select area
grim -g "$(slurp)" ~/Pictures/screenshot.png

# Copy to clipboard
grim -g "$(slurp)" - | wl-copy

# Script it (see above)
```

**Plasma (Spectacle):**
```bash
# Built-in GUI tool
Meta+Shift+Print # Opens Spectacle
# Or
Print            # Quick screenshot

# Command line
spectacle -r     # Region
spectacle -a     # Active window
spectacle -f     # Full screen
spectacle -b -n  # Background, no GUI
```

---

## Troubleshooting Scripts

### Script 1: Sway Debug

```bash
#!/bin/bash
# sway-debug.sh

echo "=== Sway Debug Information ==="
echo ""

echo "1. Sway Version:"
sway --version

echo ""
echo "2. Running Sway Processes:"
ps aux | grep sway | grep -v grep

echo ""
echo "3. Wayland Display:"
echo "WAYLAND_DISPLAY=$WAYLAND_DISPLAY"
ls -l "$XDG_RUNTIME_DIR"/wayland-*

echo ""
echo "4. Outputs:"
swaymsg -t get_outputs | jq -r '.[] | "\(.name): \(.current_mode.width)x\(.current_mode.height)@\(.current_mode.refresh)Hz"'

echo ""
echo "5. Inputs:"
swaymsg -t get_inputs | jq -r '.[] | "\(.identifier): \(.type)"'

echo ""
echo "6. Workspaces:"
swaymsg -t get_workspaces | jq -r '.[] | "Workspace \(.num): \(.name) (focused: \(.focused))"'

echo ""
echo "7. Config Validation:"
sway -C

echo ""
echo "8. Recent Logs (last 20 lines):"
journalctl --user -u sway -n 20 --no-pager
```

### Script 2: Plasma Debug

```bash
#!/bin/bash
# plasma-debug.sh

echo "=== Plasma Debug Information ==="
echo ""

echo "1. Plasma Version:"
plasmashell --version

echo ""
echo "2. KWin Version:"
kwin_x11 --version 2>/dev/null || kwin_wayland --version

echo ""
echo "3. Session Type:"
echo "XDG_SESSION_TYPE=$XDG_SESSION_TYPE"

echo ""
echo "4. Running Plasma Processes:"
ps aux | grep -E "plasma|kwin" | grep -v grep

echo ""
echo "5. Display Configuration:"
kscreen-doctor -o

echo ""
echo "6. KWin Compositing Status:"
qdbus org.kde.KWin /Compositor org.kde.kwin.Compositing.active

echo ""
echo "7. Plasma Configuration Files:"
ls -lh ~/.config/plasma* ~/.config/kwin*

echo ""
echo "8. Recent Plasma Logs:"
journalctl --user -u plasma-plasmashell -n 20 --no-pager

echo ""
echo "9. KWin Errors:"
journalctl --user -u plasma-kwin* -n 20 --no-pager | grep -i error
```

### Script 3: Wayland Check

```bash
#!/bin/bash
# wayland-check.sh

echo "=== Wayland Environment Check ==="
echo ""

echo "1. Environment Variables:"
echo "WAYLAND_DISPLAY=$WAYLAND_DISPLAY"
echo "XDG_SESSION_TYPE=$XDG_SESSION_TYPE"
echo "XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR"

echo ""
echo "2. Wayland Socket:"
if [ -S "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY" ]; then
    echo "✓ Wayland socket exists"
    ls -l "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY"
else
    echo "✗ Wayland socket not found!"
fi

echo ""
echo "3. Running Compositor:"
if pgrep -x sway > /dev/null; then
    echo "✓ Sway is running"
elif pgrep -x kwin_wayland > /dev/null; then
    echo "✓ KWin (Wayland) is running"
elif pgrep -x gnome-shell > /dev/null; then
    echo "✓ GNOME Shell is running"
else
    echo "✗ No Wayland compositor detected"
fi

echo ""
echo "4. Wayland Libraries:"
ldconfig -p | grep wayland

echo ""
echo "5. weston-info (if available):"
if command -v weston-info &> /dev/null; then
    weston-info | head -20
else
    echo "weston-info not installed"
fi
```

---

## Complete Setup Script

```bash
#!/bin/bash
# setup-sway-environment.sh
# Complete Sway environment setup

set -e

echo "=== Sway Environment Setup ==="

# 1. Install packages
echo "[1/6] Installing packages..."
sudo apt update
sudo apt install -y \
    sway swaybg swaylock swayidle \
    waybar wofi foot alacritty \
    mako-notifier grim slurp wl-clipboard \
    brightnessctl playerctl \
    network-manager-gnome blueman \
    pavucontrol \
    jq python3-i3ipc

# 2. Create directories
echo "[2/6] Creating directories..."
mkdir -p ~/.config/sway
mkdir -p ~/.config/waybar
mkdir -p ~/.config/wofi
mkdir -p ~/.config/mako
mkdir -p ~/.config/foot
mkdir -p ~/.config/sway/scripts
mkdir -p ~/Pictures/wallpapers

# 3. Copy configurations
echo "[3/6] Setting up configurations..."
cp /etc/sway/config ~/.config/sway/config

# 4. Download scripts
echo "[4/6] Setting up scripts..."
# (Would download from repository)

# 5. Setup wallpaper
echo "[5/6] Setting up wallpaper..."
if [ ! -f ~/Pictures/wallpapers/default.jpg ]; then
    # Download a default wallpaper
    wget -O ~/Pictures/wallpapers/default.jpg \
        "https://source.unsplash.com/random/1920x1080"
fi

# 6. Make scripts executable
echo "[6/6] Making scripts executable..."
chmod +x ~/.config/sway/scripts/*.sh

echo ""
echo "=== Setup Complete! ==="
echo "Launch Sway nested: sway"
echo "Or reboot and select Sway from login screen"
```

---

## Kết Luận

### Key Takeaways

1. **Sway**: Keyboard-driven, scriptable, efficient
2. **Plasma**: GUI-friendly, feature-rich, polished
3. **Both**: Support automation và customization

### Learning Path

1. Start with basics
2. Add scripts gradually
3. Customize to your workflow
4. Share with community

### Resources

- Save these scripts to your dotfiles
- Version control your configs (git)
- Backup before major changes
- Document your customizations

---

**Happy customizing! 🚀**
