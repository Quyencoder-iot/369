# Scripts và Thực Hành

## Mục lục
1. [Scripts cơ bản](#scripts-cơ-bản)
2. [Sway IPC Scripts](#sway-ipc-scripts)
3. [Automation Scripts](#automation-scripts)
4. [Config Examples](#config-examples)
5. [Bài tập thực hành](#bài-tập-thực-hành)

---

## Scripts cơ bản

### Script khởi động Sway Nested

```bash
#!/bin/bash
# File: ~/bin/sway-nested
# Usage: sway-nested [--debug]

set -e

# ═══════════════════════════════════════════════════════════════════
#                    SWAY NESTED LAUNCHER
# ═══════════════════════════════════════════════════════════════════

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log() {
    echo -e "${GREEN}[SWAY]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

# ─────────────────────────────────────────────────────────────────────
# DETECT SESSION TYPE
# ─────────────────────────────────────────────────────────────────────

detect_session() {
    if [ -n "$WAYLAND_DISPLAY" ]; then
        echo "wayland"
    elif [ -n "$DISPLAY" ]; then
        echo "x11"
    else
        echo "tty"
    fi
}

# ─────────────────────────────────────────────────────────────────────
# CHECK DEPENDENCIES
# ─────────────────────────────────────────────────────────────────────

check_deps() {
    local deps=("sway" "foot" "wofi")
    local missing=()
    
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            missing+=("$dep")
        fi
    done
    
    if [ ${#missing[@]} -gt 0 ]; then
        error "Missing dependencies: ${missing[*]}"
    fi
}

# ─────────────────────────────────────────────────────────────────────
# MAIN
# ─────────────────────────────────────────────────────────────────────

main() {
    local debug=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -d|--debug)
                debug=true
                shift
                ;;
            -h|--help)
                echo "Usage: sway-nested [--debug]"
                exit 0
                ;;
            *)
                shift
                ;;
        esac
    done
    
    # Check dependencies
    check_deps
    
    # Detect session
    local session=$(detect_session)
    log "Detected session: $session"
    
    case $session in
        wayland)
            export WLR_BACKENDS=wayland
            log "Using Wayland backend"
            ;;
        x11)
            export WLR_BACKENDS=x11
            log "Using X11 backend"
            ;;
        tty)
            error "Cannot run nested from TTY. Use native mode."
            ;;
    esac
    
    # Set environment
    export XDG_CURRENT_DESKTOP=sway
    export MOZ_ENABLE_WAYLAND=1
    export QT_QPA_PLATFORM=wayland
    export SDL_VIDEODRIVER=wayland
    
    # Config file
    local config="$HOME/.config/sway/config.nested"
    if [ ! -f "$config" ]; then
        config="$HOME/.config/sway/config"
    fi
    log "Using config: $config"
    
    # Run Sway
    if [ "$debug" = true ]; then
        log "Running in debug mode..."
        export WAYLAND_DEBUG=1
        sway -d -c "$config" 2>&1 | tee ~/sway-nested.log
    else
        exec sway -c "$config"
    fi
}

main "$@"
```

### Script Screenshot

```bash
#!/bin/bash
# File: ~/bin/sway-screenshot
# Usage: sway-screenshot [full|region|window]

set -e

SCREENSHOT_DIR="$HOME/Pictures/Screenshots"
mkdir -p "$SCREENSHOT_DIR"

FILENAME="$SCREENSHOT_DIR/$(date +%Y%m%d-%H%M%S).png"

case "${1:-full}" in
    full)
        grim "$FILENAME"
        ;;
    region)
        grim -g "$(slurp)" "$FILENAME"
        ;;
    window)
        grim -g "$(swaymsg -t get_tree | jq -r '.. | select(.focused?) | .rect | "\(.x),\(.y) \(.width)x\(.height)"')" "$FILENAME"
        ;;
    *)
        echo "Usage: sway-screenshot [full|region|window]"
        exit 1
        ;;
esac

# Copy to clipboard
wl-copy < "$FILENAME"

# Notification
notify-send "Screenshot saved" "$FILENAME"

echo "$FILENAME"
```

### Script Recording

```bash
#!/bin/bash
# File: ~/bin/sway-record
# Usage: sway-record [start|stop|toggle]

PIDFILE="/tmp/sway-record.pid"
RECORDING_DIR="$HOME/Videos/Recordings"
mkdir -p "$RECORDING_DIR"

start_recording() {
    if [ -f "$PIDFILE" ]; then
        echo "Already recording!"
        exit 1
    fi
    
    FILENAME="$RECORDING_DIR/$(date +%Y%m%d-%H%M%S).mp4"
    
    # Select region or use full screen
    if [ "$1" = "region" ]; then
        REGION=$(slurp)
        wf-recorder -g "$REGION" -f "$FILENAME" &
    else
        wf-recorder -f "$FILENAME" &
    fi
    
    echo $! > "$PIDFILE"
    notify-send "Recording started" "$FILENAME"
}

stop_recording() {
    if [ ! -f "$PIDFILE" ]; then
        echo "Not recording!"
        exit 1
    fi
    
    kill $(cat "$PIDFILE")
    rm "$PIDFILE"
    notify-send "Recording stopped"
}

toggle_recording() {
    if [ -f "$PIDFILE" ]; then
        stop_recording
    else
        start_recording "$1"
    fi
}

case "${1:-toggle}" in
    start)
        start_recording "$2"
        ;;
    stop)
        stop_recording
        ;;
    toggle)
        toggle_recording "$2"
        ;;
    *)
        echo "Usage: sway-record [start|stop|toggle] [region]"
        exit 1
        ;;
esac
```

---

## Sway IPC Scripts

### Window Info Script

```bash
#!/bin/bash
# File: ~/bin/sway-window-info
# Show information about focused window

# Get focused window info
focused=$(swaymsg -t get_tree | jq -r '.. | select(.focused? == true)')

echo "═══════════════════════════════════════════════════════════"
echo "                    FOCUSED WINDOW INFO"
echo "═══════════════════════════════════════════════════════════"
echo ""
echo "Name:     $(echo $focused | jq -r '.name')"
echo "App ID:   $(echo $focused | jq -r '.app_id')"
echo "Class:    $(echo $focused | jq -r '.window_properties.class // "N/A"')"
echo ""
echo "Position: $(echo $focused | jq -r '.rect | "\(.x), \(.y)"')"
echo "Size:     $(echo $focused | jq -r '.rect | "\(.width) x \(.height)"')"
echo ""
echo "Focused:  $(echo $focused | jq -r '.focused')"
echo "Floating: $(echo $focused | jq -r '.type == "floating_con"')"
echo "Fullscreen: $(echo $focused | jq -r '.fullscreen_mode')"
echo ""
echo "PID:      $(echo $focused | jq -r '.pid')"
echo ""
```

### Workspace Manager

```bash
#!/bin/bash
# File: ~/bin/sway-workspace
# Advanced workspace management

case "$1" in
    list)
        swaymsg -t get_workspaces | jq -r '.[] | "\(.name) (\(.output)) - \(.num) windows, focused: \(.focused)"'
        ;;
    
    move-to)
        if [ -z "$2" ]; then
            # Interactive selection
            workspace=$(swaymsg -t get_workspaces | jq -r '.[].name' | wofi --dmenu -p "Move to workspace:")
        else
            workspace="$2"
        fi
        swaymsg move container to workspace "$workspace"
        ;;
    
    rename)
        current=$(swaymsg -t get_workspaces | jq -r '.[] | select(.focused) | .name')
        if [ -z "$2" ]; then
            newname=$(echo "" | wofi --dmenu -p "Rename '$current' to:")
        else
            newname="$2"
        fi
        swaymsg rename workspace "$current" to "$newname"
        ;;
    
    new)
        # Find next available number
        max=$(swaymsg -t get_workspaces | jq -r '[.[].num] | max')
        next=$((max + 1))
        swaymsg workspace number $next
        ;;
    
    swap)
        # Swap current workspace with another
        current=$(swaymsg -t get_workspaces | jq -r '.[] | select(.focused) | .name')
        other=$(swaymsg -t get_workspaces | jq -r '.[].name' | wofi --dmenu -p "Swap with:")
        # Note: Sway doesn't have native swap, this moves windows
        swaymsg "[workspace=$current]" move container to workspace temp_swap
        swaymsg "[workspace=$other]" move container to workspace "$current"
        swaymsg "[workspace=temp_swap]" move container to workspace "$other"
        swaymsg workspace "$current"
        ;;
    
    *)
        echo "Usage: sway-workspace [list|move-to|rename|new|swap] [arg]"
        ;;
esac
```

### Layout Switcher

```bash
#!/bin/bash
# File: ~/bin/sway-layout
# Quick layout switching

case "$1" in
    toggle)
        swaymsg layout toggle split tabbed stacking
        ;;
    
    split)
        swaymsg layout toggle split
        ;;
    
    tabbed)
        swaymsg layout tabbed
        ;;
    
    stacking)
        swaymsg layout stacking
        ;;
    
    float)
        swaymsg floating toggle
        ;;
    
    fullscreen)
        swaymsg fullscreen toggle
        ;;
    
    master)
        # Master-stack layout simulation
        swaymsg splith
        swaymsg resize set 60 ppt 0
        ;;
    
    monocle)
        # Show only focused, hide others
        swaymsg "[workspace=__focused__]" layout tabbed
        ;;
    
    tile)
        # Reset to default tiling
        swaymsg "[workspace=__focused__]" layout toggle split
        ;;
    
    *)
        echo "Usage: sway-layout [toggle|split|tabbed|stacking|float|fullscreen|master|monocle|tile]"
        ;;
esac
```

### Window Selector (like dmenu for windows)

```bash
#!/bin/bash
# File: ~/bin/sway-window-select
# dmenu-style window selector

# Get all windows
windows=$(swaymsg -t get_tree | jq -r '
    recurse(.nodes[]?, .floating_nodes[]?) |
    select(.type == "con" and .name != null) |
    "\(.id)|\(.name) (\(.app_id // .window_properties.class // "unknown"))"
')

# Show in wofi
selected=$(echo "$windows" | cut -d'|' -f2 | wofi --dmenu -p "Switch to window:")

if [ -n "$selected" ]; then
    # Find the window ID
    window_id=$(echo "$windows" | grep -F "$selected" | head -1 | cut -d'|' -f1)
    
    if [ -n "$window_id" ]; then
        swaymsg "[con_id=$window_id]" focus
    fi
fi
```

---

## Automation Scripts

### Auto-start Manager

```bash
#!/bin/bash
# File: ~/.config/sway/autostart.sh
# Manage autostart applications

# ═══════════════════════════════════════════════════════════════════
#                    SWAY AUTOSTART MANAGER
# ═══════════════════════════════════════════════════════════════════

log() {
    echo "[$(date '+%H:%M:%S')] $1"
}

# Kill existing instances
cleanup() {
    pkill -x waybar 2>/dev/null
    pkill -x mako 2>/dev/null
    pkill -x swayidle 2>/dev/null
}

# Start bar
start_bar() {
    if command -v waybar &>/dev/null; then
        waybar &
        log "Started waybar"
    else
        log "waybar not found"
    fi
}

# Start notification daemon
start_notifications() {
    if command -v mako &>/dev/null; then
        mako &
        log "Started mako"
    elif command -v dunst &>/dev/null; then
        dunst &
        log "Started dunst"
    fi
}

# Start idle manager (only for native mode)
start_idle() {
    # Skip in nested mode
    if [ "$WLR_BACKENDS" = "wayland" ] || [ "$WLR_BACKENDS" = "x11" ]; then
        log "Skipping swayidle in nested mode"
        return
    fi
    
    if command -v swayidle &>/dev/null; then
        swayidle -w \
            timeout 300 'swaylock -f -c 000000' \
            timeout 600 'swaymsg "output * dpms off"' \
            resume 'swaymsg "output * dpms on"' \
            before-sleep 'swaylock -f -c 000000' &
        log "Started swayidle"
    fi
}

# Start polkit agent
start_polkit() {
    if command -v /usr/lib/polkit-kde-authentication-agent-1 &>/dev/null; then
        /usr/lib/polkit-kde-authentication-agent-1 &
        log "Started KDE polkit agent"
    elif command -v /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 &>/dev/null; then
        /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 &
        log "Started GNOME polkit agent"
    fi
}

# Start XDG desktop portal
start_portal() {
    dbus-update-activation-environment --systemd WAYLAND_DISPLAY XDG_CURRENT_DESKTOP
    systemctl --user start xdg-desktop-portal-wlr.service 2>/dev/null
    log "Started XDG portal"
}

# Main
main() {
    log "Starting autostart..."
    
    # Cleanup old processes
    cleanup
    
    # Wait for Sway to be ready
    sleep 1
    
    # Start services
    start_bar
    start_notifications
    start_idle
    start_polkit
    start_portal
    
    log "Autostart complete"
}

main
```

### Session Save/Restore

```bash
#!/bin/bash
# File: ~/bin/sway-session
# Save and restore Sway session layout

SESSION_DIR="$HOME/.local/share/sway-sessions"
mkdir -p "$SESSION_DIR"

save_session() {
    local name="${1:-default}"
    local file="$SESSION_DIR/$name.json"
    
    # Get full tree
    swaymsg -t get_tree > "$file"
    
    # Also save workspace names and assignments
    swaymsg -t get_workspaces | jq -r '.[] | "workspace \(.name)"' > "$SESSION_DIR/$name.workspaces"
    
    echo "Session saved: $file"
}

restore_session() {
    local name="${1:-default}"
    local file="$SESSION_DIR/$name.json"
    
    if [ ! -f "$file" ]; then
        echo "Session not found: $name"
        exit 1
    fi
    
    # Read saved tree and try to recreate layout
    # This is a simplified version - full restore is complex
    
    workspaces=$(cat "$SESSION_DIR/$name.workspaces" 2>/dev/null)
    if [ -n "$workspaces" ]; then
        echo "$workspaces" | while read cmd; do
            swaymsg "$cmd"
        done
    fi
    
    echo "Session restored: $name"
}

list_sessions() {
    echo "Available sessions:"
    ls -1 "$SESSION_DIR"/*.json 2>/dev/null | xargs -I {} basename {} .json
}

case "$1" in
    save)
        save_session "$2"
        ;;
    restore)
        restore_session "$2"
        ;;
    list)
        list_sessions
        ;;
    *)
        echo "Usage: sway-session [save|restore|list] [name]"
        ;;
esac
```

### Power Menu

```bash
#!/bin/bash
# File: ~/bin/sway-power
# Power menu for Sway

options="Lock\nLogout\nSuspend\nHibernate\nReboot\nShutdown"

selected=$(echo -e "$options" | wofi --dmenu -p "Power Menu:")

case "$selected" in
    "Lock")
        swaylock -f -c 000000
        ;;
    "Logout")
        swaymsg exit
        ;;
    "Suspend")
        swaylock -f -c 000000 && systemctl suspend
        ;;
    "Hibernate")
        swaylock -f -c 000000 && systemctl hibernate
        ;;
    "Reboot")
        systemctl reboot
        ;;
    "Shutdown")
        systemctl poweroff
        ;;
esac
```

---

## Config Examples

### Waybar Config

```json
// File: ~/.config/waybar/config
{
    "layer": "top",
    "position": "top",
    "height": 30,
    "spacing": 4,
    
    "modules-left": ["sway/workspaces", "sway/mode", "sway/window"],
    "modules-center": ["clock"],
    "modules-right": ["pulseaudio", "network", "cpu", "memory", "battery", "tray"],
    
    "sway/workspaces": {
        "disable-scroll": false,
        "all-outputs": true,
        "format": "{name}",
        "format-icons": {
            "1": "",
            "2": "",
            "3": "",
            "4": "",
            "5": "",
            "urgent": "",
            "focused": "",
            "default": ""
        }
    },
    
    "sway/mode": {
        "format": "<span style=\"italic\">{}</span>"
    },
    
    "sway/window": {
        "format": "{}",
        "max-length": 50
    },
    
    "clock": {
        "tooltip-format": "<big>{:%Y %B}</big>\n<tt><small>{calendar}</small></tt>",
        "format": "{:%Y-%m-%d %H:%M}",
        "format-alt": "{:%H:%M}"
    },
    
    "cpu": {
        "format": " {usage}%",
        "tooltip": true
    },
    
    "memory": {
        "format": " {}%"
    },
    
    "battery": {
        "states": {
            "warning": 30,
            "critical": 15
        },
        "format": "{icon} {capacity}%",
        "format-charging": " {capacity}%",
        "format-plugged": " {capacity}%",
        "format-icons": ["", "", "", "", ""]
    },
    
    "network": {
        "format-wifi": " {signalStrength}%",
        "format-ethernet": "",
        "format-disconnected": "⚠",
        "tooltip-format": "{ifname}: {ipaddr}"
    },
    
    "pulseaudio": {
        "format": "{icon} {volume}%",
        "format-muted": "",
        "format-icons": {
            "default": ["", "", ""]
        },
        "on-click": "pavucontrol"
    },
    
    "tray": {
        "icon-size": 21,
        "spacing": 10
    }
}
```

### Waybar Style

```css
/* File: ~/.config/waybar/style.css */

* {
    font-family: "JetBrains Mono", "Font Awesome 6 Free";
    font-size: 13px;
}

window#waybar {
    background-color: rgba(43, 48, 59, 0.95);
    color: #c0caf5;
    transition-property: background-color;
    transition-duration: .5s;
}

window#waybar.hidden {
    opacity: 0.2;
}

#workspaces button {
    padding: 0 10px;
    background-color: transparent;
    color: #c0caf5;
    border-bottom: 3px solid transparent;
}

#workspaces button:hover {
    background: rgba(0, 0, 0, 0.2);
}

#workspaces button.focused {
    background-color: #64727D;
    border-bottom: 3px solid #7aa2f7;
}

#workspaces button.urgent {
    background-color: #f7768e;
}

#mode {
    background-color: #64727D;
    border-bottom: 3px solid #c0caf5;
}

#clock,
#battery,
#cpu,
#memory,
#network,
#pulseaudio,
#tray,
#mode,
#window {
    padding: 0 10px;
    color: #c0caf5;
}

#window {
    border-bottom: 3px solid #7aa2f7;
}

#battery.charging {
    color: #9ece6a;
}

#battery.warning:not(.charging) {
    background-color: #e0af68;
    color: #1a1b26;
}

#battery.critical:not(.charging) {
    background-color: #f7768e;
    color: #1a1b26;
    animation-name: blink;
    animation-duration: 0.5s;
    animation-timing-function: linear;
    animation-iteration-count: infinite;
    animation-direction: alternate;
}

@keyframes blink {
    to {
        background-color: #1a1b26;
        color: #f7768e;
    }
}

#pulseaudio.muted {
    color: #565f89;
}
```

### Complete Sway Config (Nested-friendly)

```bash
# File: ~/.config/sway/config.complete
# ═══════════════════════════════════════════════════════════════════
#                    COMPLETE SWAY CONFIGURATION
# ═══════════════════════════════════════════════════════════════════

# ─────────────────────────────────────────────────────────────────────
# VARIABLES
# ─────────────────────────────────────────────────────────────────────

# Mod key (Mod4 = Super, Mod1 = Alt)
# Use Alt for nested to avoid conflict with host WM
set $mod Mod1

# Direction keys
set $left h
set $down j
set $up k
set $right l

# Terminal
set $term foot

# Application launcher
set $menu wofi --show drun --allow-images

# Lock screen
set $lock swaylock -f -c 1a1b26

# Scripts
set $screenshot ~/bin/sway-screenshot
set $power ~/bin/sway-power

# ─────────────────────────────────────────────────────────────────────
# OUTPUT CONFIGURATION
# ─────────────────────────────────────────────────────────────────────

# Wallpaper
output * bg #1a1b26 solid_color

# For nested mode, output is usually WL-1 or X11-1
# No need to set resolution - it follows window size

# ─────────────────────────────────────────────────────────────────────
# INPUT CONFIGURATION
# ─────────────────────────────────────────────────────────────────────

input type:keyboard {
    xkb_layout us
    xkb_options caps:escape
    repeat_delay 250
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

# ─────────────────────────────────────────────────────────────────────
# GENERAL SETTINGS
# ─────────────────────────────────────────────────────────────────────

# Focus follows mouse
focus_follows_mouse yes

# Mouse warping
mouse_warping output

# Workspace auto back and forth
workspace_auto_back_and_forth yes

# Popup behavior
popup_during_fullscreen smart

# ─────────────────────────────────────────────────────────────────────
# APPEARANCE
# ─────────────────────────────────────────────────────────────────────

# Gaps
gaps inner 8
gaps outer 4

# Smart gaps - disable gaps when only one window
smart_gaps on

# Borders
default_border pixel 2
default_floating_border pixel 2
smart_borders on

# Hide edge borders
hide_edge_borders smart

# Font
font pango:JetBrains Mono 10

# Colors (Tokyo Night theme)
# class                 border  backgr. text    indicator child_border
client.focused          #7aa2f7 #1a1b26 #c0caf5 #7dcfff   #7aa2f7
client.focused_inactive #414868 #1a1b26 #c0caf5 #414868   #414868
client.unfocused        #24283b #1a1b26 #565f89 #24283b   #24283b
client.urgent           #f7768e #1a1b26 #c0caf5 #f7768e   #f7768e
client.placeholder      #1a1b26 #1a1b26 #c0caf5 #1a1b26   #1a1b26
client.background       #1a1b26

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - BASIC
# ─────────────────────────────────────────────────────────────────────

# Terminal
bindsym $mod+Return exec $term

# Kill focused
bindsym $mod+Shift+q kill

# Launcher
bindsym $mod+d exec $menu
bindsym $mod+space exec $menu

# Reload config
bindsym $mod+Shift+c reload

# Exit sway
bindsym $mod+Shift+e exec $power

# Lock screen
bindsym $mod+Escape exec $lock

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - NAVIGATION
# ─────────────────────────────────────────────────────────────────────

# Move focus
bindsym $mod+$left focus left
bindsym $mod+$down focus down
bindsym $mod+$up focus up
bindsym $mod+$right focus right

bindsym $mod+Left focus left
bindsym $mod+Down focus down
bindsym $mod+Up focus up
bindsym $mod+Right focus right

# Move focused window
bindsym $mod+Shift+$left move left
bindsym $mod+Shift+$down move down
bindsym $mod+Shift+$up move up
bindsym $mod+Shift+$right move right

bindsym $mod+Shift+Left move left
bindsym $mod+Shift+Down move down
bindsym $mod+Shift+Up move up
bindsym $mod+Shift+Right move right

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - WORKSPACES
# ─────────────────────────────────────────────────────────────────────

# Define workspaces with names
set $ws1 "1:term"
set $ws2 "2:web"
set $ws3 "3:code"
set $ws4 "4:files"
set $ws5 "5:misc"
set $ws6 "6"
set $ws7 "7"
set $ws8 "8"
set $ws9 "9"
set $ws10 "10"

# Switch to workspace
bindsym $mod+1 workspace $ws1
bindsym $mod+2 workspace $ws2
bindsym $mod+3 workspace $ws3
bindsym $mod+4 workspace $ws4
bindsym $mod+5 workspace $ws5
bindsym $mod+6 workspace $ws6
bindsym $mod+7 workspace $ws7
bindsym $mod+8 workspace $ws8
bindsym $mod+9 workspace $ws9
bindsym $mod+0 workspace $ws10

# Move container to workspace
bindsym $mod+Shift+1 move container to workspace $ws1
bindsym $mod+Shift+2 move container to workspace $ws2
bindsym $mod+Shift+3 move container to workspace $ws3
bindsym $mod+Shift+4 move container to workspace $ws4
bindsym $mod+Shift+5 move container to workspace $ws5
bindsym $mod+Shift+6 move container to workspace $ws6
bindsym $mod+Shift+7 move container to workspace $ws7
bindsym $mod+Shift+8 move container to workspace $ws8
bindsym $mod+Shift+9 move container to workspace $ws9
bindsym $mod+Shift+0 move container to workspace $ws10

# Move workspace to output
bindsym $mod+Ctrl+Left move workspace to output left
bindsym $mod+Ctrl+Right move workspace to output right

# ─────────────────────────────────────────────────────────────────────
# KEY BINDINGS - LAYOUT
# ─────────────────────────────────────────────────────────────────────

# Split direction
bindsym $mod+b splith
bindsym $mod+v splitv

# Layout switching
bindsym $mod+s layout stacking
bindsym $mod+w layout tabbed
bindsym $mod+e layout toggle split

# Fullscreen
bindsym $mod+f fullscreen toggle

# Floating
bindsym $mod+Shift+space floating toggle
bindsym $mod+Shift+f focus mode_toggle

# Sticky window
bindsym $mod+Shift+s sticky toggle

# Focus parent/child
bindsym $mod+a focus parent
bindsym $mod+Shift+a focus child

# Scratchpad
bindsym $mod+Shift+minus move scratchpad
bindsym $mod+minus scratchpad show

# ─────────────────────────────────────────────────────────────────────
# RESIZE MODE
# ─────────────────────────────────────────────────────────────────────

mode "resize" {
    bindsym $left resize shrink width 10px
    bindsym $down resize grow height 10px
    bindsym $up resize shrink height 10px
    bindsym $right resize grow width 10px

    bindsym Left resize shrink width 10px
    bindsym Down resize grow height 10px
    bindsym Up resize shrink height 10px
    bindsym Right resize grow width 10px

    # Fine adjustments
    bindsym Shift+$left resize shrink width 2px
    bindsym Shift+$down resize grow height 2px
    bindsym Shift+$up resize shrink height 2px
    bindsym Shift+$right resize grow width 2px

    bindsym Return mode "default"
    bindsym Escape mode "default"
    bindsym $mod+r mode "default"
}
bindsym $mod+r mode "resize"

# ─────────────────────────────────────────────────────────────────────
# MEDIA KEYS
# ─────────────────────────────────────────────────────────────────────

# Volume
bindsym XF86AudioRaiseVolume exec pactl set-sink-volume @DEFAULT_SINK@ +5%
bindsym XF86AudioLowerVolume exec pactl set-sink-volume @DEFAULT_SINK@ -5%
bindsym XF86AudioMute exec pactl set-sink-mute @DEFAULT_SINK@ toggle
bindsym XF86AudioMicMute exec pactl set-source-mute @DEFAULT_SOURCE@ toggle

# Brightness
bindsym XF86MonBrightnessDown exec brightnessctl set 5%-
bindsym XF86MonBrightnessUp exec brightnessctl set 5%+

# Media
bindsym XF86AudioPlay exec playerctl play-pause
bindsym XF86AudioNext exec playerctl next
bindsym XF86AudioPrev exec playerctl previous

# ─────────────────────────────────────────────────────────────────────
# SCREENSHOTS
# ─────────────────────────────────────────────────────────────────────

bindsym Print exec $screenshot full
bindsym Shift+Print exec $screenshot region
bindsym $mod+Print exec $screenshot window

# ─────────────────────────────────────────────────────────────────────
# WINDOW RULES
# ─────────────────────────────────────────────────────────────────────

# Floating windows
for_window [window_role="pop-up"] floating enable
for_window [window_role="task_dialog"] floating enable
for_window [window_type="dialog"] floating enable
for_window [window_type="menu"] floating enable

for_window [app_id="pavucontrol"] floating enable
for_window [app_id="nm-connection-editor"] floating enable
for_window [app_id="blueman-manager"] floating enable
for_window [app_id="imv"] floating enable
for_window [app_id="mpv"] floating enable

for_window [class="Pinentry"] floating enable
for_window [class="Lxappearance"] floating enable

# Assign apps to workspaces
assign [app_id="firefox"] $ws2
assign [app_id="code"] $ws3
assign [app_id="thunar"] $ws4
assign [class="Spotify"] $ws5

# ─────────────────────────────────────────────────────────────────────
# STATUS BAR
# ─────────────────────────────────────────────────────────────────────

bar {
    swaybar_command waybar
}

# ─────────────────────────────────────────────────────────────────────
# AUTOSTART
# ─────────────────────────────────────────────────────────────────────

# Run autostart script
exec ~/.config/sway/autostart.sh

# ─────────────────────────────────────────────────────────────────────
# INCLUDE ADDITIONAL CONFIGS
# ─────────────────────────────────────────────────────────────────────

include ~/.config/sway/config.d/*
```

---

## Bài tập thực hành

### Bài 1: Cài đặt Sway cơ bản

```bash
# Mục tiêu: Cài đặt và chạy Sway nested trong session hiện tại

# Bước 1: Cài đặt packages
sudo apt install sway foot wofi waybar

# Bước 2: Tạo config cơ bản
mkdir -p ~/.config/sway
cat > ~/.config/sway/config.practice << 'EOF'
set $mod Mod1
set $term foot
bindsym $mod+Return exec $term
bindsym $mod+Shift+q kill
bindsym $mod+Shift+e exec swaymsg exit
bindsym $mod+h focus left
bindsym $mod+l focus right
bindsym $mod+j focus down
bindsym $mod+k focus up
EOF

# Bước 3: Chạy Sway nested
WLR_BACKENDS=wayland sway -c ~/.config/sway/config.practice

# Bài tập:
# 1. Mở 3 terminal (Mod1+Enter)
# 2. Di chuyển focus giữa các terminal
# 3. Thoát Sway (Mod1+Shift+e)
```

### Bài 2: Customize layout

```bash
# Mục tiêu: Tạo layout tùy chỉnh

# Thêm vào config:
cat >> ~/.config/sway/config.practice << 'EOF'

# Layout keybindings
bindsym $mod+b splith
bindsym $mod+v splitv
bindsym $mod+f fullscreen
bindsym $mod+s layout stacking
bindsym $mod+w layout tabbed
bindsym $mod+e layout toggle split

# Workspaces
bindsym $mod+1 workspace 1
bindsym $mod+2 workspace 2
bindsym $mod+3 workspace 3
bindsym $mod+Shift+1 move container to workspace 1
bindsym $mod+Shift+2 move container to workspace 2
bindsym $mod+Shift+3 move container to workspace 3

# Gaps and borders
gaps inner 10
default_border pixel 2
EOF

# Bài tập:
# 1. Mở terminal, split horizontal (Mod1+b)
# 2. Mở thêm terminal, split vertical (Mod1+v)
# 3. Thử các layout: tabbed, stacking
# 4. Di chuyển window sang workspace 2
```

### Bài 3: Thêm Waybar

```bash
# Mục tiêu: Cấu hình status bar

# Tạo waybar config
mkdir -p ~/.config/waybar
cat > ~/.config/waybar/config << 'EOF'
{
    "layer": "top",
    "position": "top",
    "height": 24,
    "modules-left": ["sway/workspaces"],
    "modules-center": ["sway/window"],
    "modules-right": ["clock"],
    "clock": {
        "format": "{:%H:%M}"
    }
}
EOF

cat > ~/.config/waybar/style.css << 'EOF'
* {
    font-family: monospace;
    font-size: 12px;
}
window#waybar {
    background: #1a1b26;
    color: #c0caf5;
}
#workspaces button.focused {
    background: #7aa2f7;
}
EOF

# Thêm vào sway config:
echo 'bar { swaybar_command waybar }' >> ~/.config/sway/config.practice

# Reload sway
swaymsg reload

# Bài tập:
# 1. Thêm module cpu và memory vào waybar
# 2. Customize colors
# 3. Thêm module network
```

### Bài 4: Viết IPC script

```bash
# Mục tiêu: Sử dụng Sway IPC

# Script: Liệt kê tất cả windows
cat > ~/bin/list-windows << 'EOF'
#!/bin/bash
swaymsg -t get_tree | jq -r '
    recurse(.nodes[]?, .floating_nodes[]?) |
    select(.type == "con" and .name != null) |
    "\(.app_id // "unknown"): \(.name)"
'
EOF
chmod +x ~/bin/list-windows

# Bài tập:
# 1. Viết script để đếm số windows đang mở
# 2. Viết script để focus vào window theo tên
# 3. Viết script để di chuyển tất cả floating windows sang workspace 10
```

### Bài 5: Integration với Plasma

```bash
# Mục tiêu: Chạy Sway nested trong KDE Plasma

# 1. Đảm bảo đang trong Plasma Wayland session
echo $XDG_SESSION_TYPE  # Phải là "wayland"
echo $XDG_CURRENT_DESKTOP  # Phải là "KDE"

# 2. Tạo launcher
cat > ~/.local/share/applications/sway-dev.desktop << 'EOF'
[Desktop Entry]
Type=Application
Name=Sway Development
Comment=Sway tiling WM for coding
Exec=env WLR_BACKENDS=wayland sway -c ~/.config/sway/config.practice
Icon=utilities-terminal
Categories=Development;
EOF

# 3. Chạy từ KRunner hoặc menu

# Bài tập:
# 1. Tạo keybinding trong Plasma để launch Sway
# 2. Cấu hình Sway để dùng konsole thay vì foot
# 3. Share clipboard giữa Plasma và Sway apps
```

---

## Tài nguyên thêm

### Official Documentation
- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [wlroots Documentation](https://gitlab.freedesktop.org/wlroots/wlroots/-/wikis/home)
- [Wayland Book](https://wayland-book.com/)

### Communities
- [Sway Discussions](https://github.com/swaywm/sway/discussions)
- [r/swaywm](https://reddit.com/r/swaywm)
- [#sway on Libera.Chat](ircs://irc.libera.chat/#sway)

### Example Configs
- [Sway Default Config](https://github.com/swaywm/sway/blob/master/config.in)
- [Awesome Sway](https://github.com/swaywm/sway/wiki/Useful-add-ons-for-sway)
