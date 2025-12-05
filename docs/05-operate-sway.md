# Hướng Dẫn Vận Hành và Cấu Hình Sway

## Mục Lục
1. [Khởi động Sway](#khởi-động-sway)
2. [Keybindings cơ bản](#keybindings-cơ-bản)
3. [Window Management](#window-management)
4. [Workspace Management](#workspace-management)
5. [Configuration Deep Dive](#configuration-deep-dive)
6. [Customization](#customization)
7. [IPC và Scripting](#ipc-và-scripting)

---

## Khởi Động Sway

### Nested Mode (Testing)

```bash
# Simple launch từ desktop environment hiện tại
sway

# With custom window size
WLR_WL_OUTPUT_WIDTH=1920 WLR_WL_OUTPUT_HEIGHT=1080 sway

# With debug output
SWAY_DEBUG=1 sway 2>&1 | tee ~/sway-debug.log

# With specific config
sway -c ~/.config/sway/config-test
```

### Full Mode (từ TTY)

```bash
# Login to TTY (Ctrl+Alt+F3)
# Login với username/password

# Launch Sway
sway

# Or with systemd
systemctl --user start sway.service
```

### Auto-start từ Login

```bash
# Add to ~/.bash_profile or ~/.profile
if [ -z "$DISPLAY" ] && [ "$XDG_VTNR" -eq 1 ]; then
    exec sway
fi

# This will auto-start Sway when login to TTY1
```

---

## Keybindings Cơ Bản

### Notation

```
Mod = Super/Windows key (Mod4)
Alt = Alt key (Mod1)
Shift = Shift key
Ctrl = Control key

Example: Mod+Return = Windows key + Enter
```

### Essential Keybindings

#### Launching Programs

```
Mod+Return          → Open terminal
Mod+d               → Open application launcher (dmenu/wofi/rofi)
Mod+Shift+q         → Kill focused window
```

#### Window Focus

```
Mod+Left            → Focus window to the left
Mod+Down            → Focus window below
Mod+Up              → Focus window above  
Mod+Right           → Focus window to the right

Mod+j               → Focus next window
Mod+k               → Focus previous window

Mod+a               → Focus parent container
```

#### Moving Windows

```
Mod+Shift+Left      → Move window left
Mod+Shift+Down      → Move window down
Mod+Shift+Up        → Move window up
Mod+Shift+Right     → Move window right

Mod+Shift+Space     → Toggle floating
Mod+Left_mouse      → Drag floating window
Mod+Right_mouse     → Resize floating window
```

#### Window Layout

```
Mod+h               → Split horizontal (next window opens to the right)
Mod+v               → Split vertical (next window opens below)

Mod+s               → Stacking layout
Mod+w               → Tabbed layout
Mod+e               → Split layout (toggle)

Mod+f               → Fullscreen toggle
Mod+Shift+Space     → Toggle floating
Mod+Space           → Toggle focus: tiling/floating
```

#### Workspaces

```
Mod+1 to 9          → Switch to workspace 1-9
Mod+0               → Switch to workspace 10

Mod+Shift+1 to 9    → Move window to workspace 1-9
Mod+Shift+0         → Move window to workspace 10

Mod+Tab             → Switch to recent workspace (if configured)
```

#### Sway Control

```
Mod+Shift+c         → Reload configuration
Mod+Shift+e         → Exit Sway (shows confirmation)
Mod+Shift+r         → Restart Sway (X11 compatibility, not needed)

Mod+r               → Enter resize mode
```

### Resize Mode

Khi trong resize mode:

```
Left/Right          → Resize width
Up/Down             → Resize height

Return/Escape       → Exit resize mode
```

---

## Window Management

### Tiling Concepts

```
┌─────────────────────────────────────┐
│  Sway Tiling Layout                 │
├─────────────────────────────────────┤
│                                     │
│  ┌────────────┐  ┌───────────────┐ │
│  │            │  │               │ │
│  │  Window 1  │  │   Window 2    │ │
│  │            │  │               │ │
│  │            │  ├───────────────┤ │
│  │            │  │               │ │
│  │            │  │   Window 3    │ │
│  │            │  │               │ │
│  └────────────┘  └───────────────┘ │
│                                     │
└─────────────────────────────────────┘

Window 1: Root container, full height
Window 2,3: In vertical split container
```

### Container Hierarchy

```
Workspace
  └── Container (Horizontal)
      ├── Window A
      └── Container (Vertical)
          ├── Window B
          └── Window C
```

### Layout Modes

#### Split Layout

Default tiling behavior:

```
Horizontal split (Mod+h):
┌────────┬────────┐
│   A    │   B    │
│        │        │
└────────┴────────┘

Vertical split (Mod+v):
┌────────────────┐
│       A        │
├────────────────┤
│       B        │
└────────────────┘
```

#### Stacking Layout

Windows stack vertically with tabs:

```
┌────────────────┐
│ [A] [B] [C]    │  ← Tabs
├────────────────┤
│                │
│   Active       │
│   Window       │
│                │
└────────────────┘
```

#### Tabbed Layout

Similar to stacking but emphasizes tabs:

```
┌─[A]─[B]─[C]───┐
│                │
│   Active Tab   │
│                │
└────────────────┘
```

### Floating Windows

```bash
# Toggle floating
Mod+Shift+Space

# Move floating window
Mod+Left_mouse_drag

# Resize floating window
Mod+Right_mouse_drag

# Center floating window (via config):
bindsym Mod+c move position center
```

### Scratchpad

Hidden workspace for temporary windows:

```bash
# Move window to scratchpad
Mod+Shift+Minus

# Toggle scratchpad visibility
Mod+Minus

# Use case: Keep terminal/notes/music player hidden but accessible
```

---

## Workspace Management

### Basic Workspace Operations

```bash
# Switch to workspace
bindsym $mod+1 workspace number 1
bindsym $mod+2 workspace number 2
# ... etc

# Move window to workspace
bindsym $mod+Shift+1 move container to workspace number 1
```

### Named Workspaces

```bash
# Define workspace names
set $ws1 "1: Web"
set $ws2 "2: Code"
set $ws3 "3: Term"
set $ws4 "4: Chat"

# Use names in bindings
bindsym $mod+1 workspace $ws1
bindsym $mod+2 workspace $ws2
bindsym $mod+Shift+1 move container to workspace $ws1
```

### Multi-Monitor Workspaces

```bash
# Assign workspaces to specific outputs
workspace 1 output HDMI-A-1
workspace 2 output HDMI-A-1
workspace 3 output eDP-1
workspace 4 output eDP-1

# Move workspace to different output
bindsym $mod+Ctrl+Left move workspace to output left
bindsym $mod+Ctrl+Right move workspace to output right
```

### Workspace Back-and-Forth

```bash
# Toggle between current and previous workspace
bindsym $mod+Tab workspace back_and_forth

# Move window and follow
bindsym $mod+Shift+Tab move container to workspace back_and_forth; workspace back_and_forth
```

---

## Configuration Deep Dive

### Config File Structure

```bash
# ~/.config/sway/config

# Variables
set $var value

# Outputs (monitors)
output <name> <properties>

# Inputs (keyboard, mouse, touchpad)
input <identifier> { <properties> }

# Key bindings
bindsym <key> <command>

# Window rules
for_window [criteria] <command>

# Autostart programs
exec <command>
exec_always <command>

# Appearance
gaps inner|outer <amount>
default_border pixel|normal <amount>
client.<colorclass> <border> <background> <text> <indicator> <child_border>

# Status bar
bar { <properties> }

# Include additional configs
include /path/to/config
```

### Variables

```bash
# Modifier key
set $mod Mod4              # Windows/Super key
# set $mod Mod1            # Alt key (for nested mode)

# Terminal
set $term foot
# set $term alacritty
# set $term kitty

# Launcher
set $menu wofi --show drun
# set $menu rofi -show drun
# set $menu dmenu_path | dmenu | xargs swaymsg exec --

# Directions (vi-style)
set $left h
set $down j
set $up k
set $right l
```

### Output Configuration

```bash
# List available outputs
swaymsg -t get_outputs

# Configure output
output HDMI-A-1 resolution 1920x1080 position 0,0
output eDP-1 resolution 1920x1080 position 1920,0 scale 1.5

# Rotation
output eDP-1 transform 90  # 90, 180, 270

# Disable output
output VGA-1 disable

# Wallpaper
output * bg ~/Pictures/wallpaper.jpg fill
output HDMI-A-1 bg ~/Pictures/monitor2.png fit

# Adaptive sync (VRR/FreeSync)
output * adaptive_sync on

# Max render time (for VRR)
output * max_render_time 4
```

### Input Configuration

```bash
# List input devices
swaymsg -t get_inputs

# Configure all keyboards
input type:keyboard {
    xkb_layout us,ru
    xkb_options grp:alt_shift_toggle,caps:escape
    repeat_delay 300
    repeat_rate 30
}

# Configure specific keyboard
input "1234:5678:My_Keyboard" {
    xkb_layout dvorak
}

# Configure touchpad
input type:touchpad {
    tap enabled
    natural_scroll enabled
    dwt enabled                    # Disable while typing
    accel_profile adaptive
    pointer_accel 0.3
    scroll_factor 0.5
    middle_emulation enabled
}

# Configure mouse
input type:pointer {
    accel_profile flat
    pointer_accel 0.0
}

# Disable laptop keyboard when lid closed (requires special setup)
bindswitch --reload --locked lid:on input type:keyboard events disabled
bindswitch --reload --locked lid:off input type:keyboard events enabled
```

### Window Rules

```bash
# Floating windows
for_window [app_id="pavucontrol"] floating enable
for_window [app_id="thunar"] floating enable, resize set 800 600
for_window [title="Picture-in-Picture"] floating enable, sticky enable

# Position windows
for_window [app_id="telegram"] move to workspace 4

# Borders
for_window [class="^.*"] border pixel 2
for_window [app_id="firefox"] border none

# Inhibit idle
for_window [app_id="firefox"] inhibit_idle fullscreen
for_window [class="mpv"] inhibit_idle visible

# Criteria matching
# [class="..."]          X11 window class
# [app_id="..."]         Wayland app_id
# [title="..."]          Window title
# [workspace="..."]      Workspace
# [con_mark="..."]       Container mark
# [floating]             Floating windows
# [tiling]               Tiled windows
```

**Finding app_id:**

```bash
# Method 1: swaymsg
swaymsg -t get_tree | grep app_id

# Method 2: Check in real-time
swaymsg -t subscribe -m '["window"]' | jq '.container | .app_id, .name'
# Then launch the app

# For X11 apps, use class instead
```

### Keybinding Advanced

```bash
# Multiple commands
bindsym $mod+o exec term1; exec term2

# Conditions
bindsym $mod+p exec "if pgrep firefox; then swaymsg [app_id=firefox] focus; else firefox; fi"

# Modes
mode "launcher" {
    bindsym f exec firefox; mode "default"
    bindsym t exec telegram; mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+o mode "launcher"

# Repeat delay and rate
set $mode_repeat repeat_rate 50 repeat_delay 200

# Bind to specific location
# --no-repeat: Don't trigger on key repeat
# --locked: Work even when screen is locked
# --release: Trigger on key release
# --inhibited: Work even when shortcuts are inhibited

bindsym --locked XF86AudioMute exec pactl set-sink-mute @DEFAULT_SINK@ toggle
```

### Appearance

```bash
# Gaps
gaps inner 5
gaps outer 3

# Smart gaps (hide when one window)
smart_gaps on

# Borders
default_border pixel 2
default_floating_border normal

# Hide borders when one window
hide_edge_borders smart

# Title bar
titlebar_border_thickness 2
titlebar_padding 5 1

# Font
font pango:JetBrains Mono 10

# Colors
# class                 border  bg      text    indicator child_border
client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577
client.focused_inactive #333333 #5f676a #ffffff #484e50   #5f676a
client.unfocused        #333333 #222222 #888888 #292d2e   #222222
client.urgent           #2f343a #900000 #ffffff #900000   #900000
client.placeholder      #000000 #0c0c0c #ffffff #000000   #0c0c0c
client.background       #ffffff
```

### Status Bar

```bash
# Simple built-in bar
bar {
    position top
    status_command while date +'%Y-%m-%d %H:%M:%S'; do sleep 1; done
    
    font pango:JetBrains Mono 10
    
    colors {
        statusline #ffffff
        background #323232
        inactive_workspace #32323200 #32323200 #5c5c5c
        focused_workspace #4c7899 #285577 #ffffff
    }
}

# Using waybar (better)
bar {
    swaybar_command waybar
}

# No bar
# (comment out bar section)
```

### Autostart Programs

```bash
# Run once on start
exec firefox
exec telegram-desktop

# Run on each reload
exec_always pgrep mako || mako

# Background services
exec wl-paste -t text --watch clipman store
exec gammastep -l 40.7:-74.0

# Idle management
exec swayidle -w \
    timeout 300 'swaylock -f' \
    timeout 600 'swaymsg "output * dpms off"' \
    resume 'swaymsg "output * dpms on"' \
    before-sleep 'swaylock -f'
```

---

## Customization

### Themes & Colors

#### Base16 Theme Example

```bash
# Base16 Nord
set $base00 #2E3440
set $base01 #3B4252
set $base02 #434C5E
set $base03 #4C566A
set $base04 #D8DEE9
set $base05 #E5E9F0
set $base06 #ECEFF4
set $base07 #8FBCBB
set $base08 #BF616A
set $base09 #D08770
set $base0A #EBCB8B
set $base0B #A3BE8C
set $base0C #88C0D0
set $base0D #81A1C1
set $base0E #B48EAD
set $base0F #5E81AC

# Apply to windows
client.focused $base0D $base0D $base00 $base0D $base0D
client.focused_inactive $base03 $base03 $base05 $base03 $base03
client.unfocused $base01 $base01 $base05 $base01 $base01
client.urgent $base08 $base08 $base00 $base08 $base08
```

### Wallpaper Management

```bash
# Static wallpaper
output * bg ~/Pictures/wallpaper.jpg fill

# Random wallpaper
exec_always swaybg -i $(find ~/Pictures/Wallpapers -type f | shuf -n 1) -m fill

# Animated wallpaper (with mpvpaper)
exec_always mpvpaper -o "no-audio loop" '*' ~/Videos/animated-bg.mp4
```

### Waybar Configuration

```bash
# ~/.config/waybar/config
{
    "layer": "top",
    "position": "top",
    "height": 30,
    "modules-left": ["sway/workspaces", "sway/mode"],
    "modules-center": ["clock"],
    "modules-right": ["network", "pulseaudio", "battery", "tray"],
    
    "sway/workspaces": {
        "format": "{name}",
        "disable-scroll": true
    },
    
    "clock": {
        "format": "{:%H:%M %Y-%m-%d}"
    },
    
    "battery": {
        "format": "{capacity}% {icon}",
        "format-icons": ["", "", "", "", ""]
    },
    
    "network": {
        "format-wifi": "{essid} ",
        "format-ethernet": "{ifname} ",
        "format-disconnected": "Disconnected ⚠"
    },
    
    "pulseaudio": {
        "format": "{volume}% {icon}",
        "format-muted": "",
        "format-icons": ["", "", ""]
    }
}
```

```css
/* ~/.config/waybar/style.css */
* {
    font-family: JetBrains Mono;
    font-size: 13px;
}

window#waybar {
    background-color: #282828;
    color: #ebdbb2;
}

#workspaces button {
    padding: 0 10px;
    background-color: transparent;
    color: #ebdbb2;
}

#workspaces button.focused {
    background-color: #504945;
}

#clock, #battery, #network, #pulseaudio {
    padding: 0 10px;
}
```

### Application Launcher (wofi)

```bash
# ~/.config/wofi/config
show=drun
width=600
height=400
prompt=Search...
allow_images=true
image_size=32
```

```css
/* ~/.config/wofi/style.css */
window {
    background-color: #282828;
    color: #ebdbb2;
}

#input {
    background-color: #3c3836;
    color: #ebdbb2;
    border: 2px solid #504945;
}

#entry:selected {
    background-color: #504945;
}
```

---

## IPC và Scripting

### swaymsg Command

```bash
# Get information
swaymsg -t get_outputs      # List monitors
swaymsg -t get_inputs       # List input devices
swaymsg -t get_workspaces   # List workspaces
swaymsg -t get_tree         # Full tree (windows, etc.)

# Execute commands
swaymsg exec firefox
swaymsg reload
swaymsg "output HDMI-A-1 transform 90"

# Focus windows
swaymsg "[app_id=firefox] focus"

# Move windows
swaymsg "[app_id=firefox] move to workspace 2"
```

### JSON Output

```bash
# Get tree in JSON
swaymsg -t get_tree | jq

# Find focused window
swaymsg -t get_tree | jq '.. | select(.focused? == true)'

# List all windows
swaymsg -t get_tree | jq '.. | select(.type?) | select(.type=="con") | {name, app_id}'

# Get window count
swaymsg -t get_tree | jq '[.. | select(.type?) | select(.type=="con")] | length'
```

### Subscribe to Events

```bash
# Watch window events
swaymsg -t subscribe -m '["window"]' | jq

# Watch workspace changes
swaymsg -t subscribe -m '["workspace"]' | jq

# Multiple event types
swaymsg -t subscribe -m '["workspace", "window"]' | jq
```

### Scripting Examples

#### Auto-tile Script

```bash
#!/bin/bash
# auto-tile.sh - Automatically set split direction

swaymsg -t subscribe -m '["window"]' | while read -r event; do
    window_id=$(echo "$event" | jq -r '.container.id')
    width=$(echo "$event" | jq -r '.container.rect.width')
    height=$(echo "$event" | jq -r '.container.rect.height')
    
    if [ "$width" -gt "$height" ]; then
        swaymsg "[con_id=$window_id] splith"
    else
        swaymsg "[con_id=$window_id] splitv"
    fi
done
```

#### Focus Last Urgent Window

```bash
#!/bin/bash
# focus-urgent.sh

urgent=$(swaymsg -t get_tree | jq -r '.. | select(.urgent? == true) | .id' | head -1)
if [ -n "$urgent" ]; then
    swaymsg "[con_id=$urgent] focus"
fi
```

#### Cycle Through Windows

```bash
#!/bin/bash
# cycle-windows.sh

direction=$1  # next or prev

swaymsg focus "$direction"
```

Bind in config:
```bash
bindsym $mod+Tab exec ~/.config/sway/scripts/cycle-windows.sh next
bindsym $mod+Shift+Tab exec ~/.config/sway/scripts/cycle-windows.sh prev
```

---

## Tips & Tricks

### Multi-Monitor Setup

```bash
# Check connected monitors
swaymsg -t get_outputs | jq -r '.[] | "\(.name): \(.current_mode.width)x\(.current_mode.height)"'

# Configure in config file
output eDP-1 pos 0 0 res 1920x1080
output HDMI-A-1 pos 1920 0 res 1920x1080
output DP-1 pos 3840 0 res 2560x1440 scale 1.5

# GUI tool
wdisplays
```

### Power Management

```bash
# swayidle configuration
exec swayidle -w \
    timeout 300 'swaylock -f -c 000000' \
    timeout 600 'swaymsg "output * dpms off"' resume 'swaymsg "output * dpms on"' \
    before-sleep 'swaylock -f -c 000000'

# Manual lock
bindsym $mod+l exec swaylock -f -c 000000

# Suspend
bindsym $mod+Shift+s exec systemctl suspend
```

### Screenshots

```bash
# Full screen
bindsym Print exec grim ~/Pictures/screenshot-$(date +%Y%m%d-%H%M%S).png

# Select area
bindsym Shift+Print exec grim -g "$(slurp)" ~/Pictures/screenshot-$(date +%Y%m%d-%H%M%S).png

# Copy to clipboard
bindsym Ctrl+Print exec grim -g "$(slurp)" - | wl-copy

# Current window
bindsym $mod+Print exec grim -g "$(swaymsg -t get_tree | jq -r '.. | select(.focused?) | .rect | "\(.x),\(.y) \(.width)x\(.height)"')" ~/Pictures/screenshot.png
```

### Clipboard Management

```bash
# Install clipman
sudo apt install clipman

# Auto-start in config
exec wl-paste -t text --watch clipman store

# Show history
bindsym $mod+c exec clipman pick -t wofi

# Clear history
clipman clear
```

---

## Kết Luận

Sway là powerful tiling window manager với learning curve. Key points:

1. **Keyboard-driven**: Mọi thứ qua keybindings
2. **Tiling**: Windows tự động arrange
3. **Configurable**: Text-based config, infinitely customizable
4. **Efficient**: Fast, lightweight, productive

### Next Steps

1. Practice keybindings daily
2. Customize config dần dần
3. Explore tools: waybar, wofi, etc.
4. Join community: r/swaywm, IRC
5. Read [08-practical-examples.md](./08-practical-examples.md)

### Tài Nguyên

- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [man sway(5)](https://man.archlinux.org/man/sway.5)
- [Dotfiles inspiration](https://github.com/search?q=sway+config)
