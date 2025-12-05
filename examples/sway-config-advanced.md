# Cấu Hình Sway Nâng Cao

## Cấu hình đầy đủ cho Sway nested với nhiều tính năng

### File: ~/.config/sway/config

```bash
# ============================================
# SWAY CONFIG - NESTED MODE
# ============================================

# Mod key
set $mod Mod4

# Terminal và menu
set $term alacritty
set $menu rofi -show drun -modi drun,run,window

# Applications
set $browser firefox
set $filemanager dolphin
set $editor code

# ============================================
# OUTPUT CONFIGURATION
# ============================================

# Background (sẽ được override bởi swaybg nếu có)
output * bg #1e1e2e solid_color

# Scale cho high DPI (nếu cần)
# output * scale 1.5

# ============================================
# INPUT CONFIGURATION
# ============================================

# Keyboard layout
input * {
    xkb_layout us,us
    xkb_variant ,intl
    xkb_options grp:alt_shift_toggle
}

# Mouse acceleration
input type:pointer {
    accel_profile flat
    pointer_accel 0.5
}

# Touchpad (nếu có)
input type:touchpad {
    tap enabled
    natural_scroll enabled
}

# ============================================
# KEYBINDINGS - NAVIGATION
# ============================================

# Focus
bindsym $mod+j focus left
bindsym $mod+k focus down
bindsym $mod+l focus up
bindsym $mod+semicolon focus right

# Move window
bindsym $mod+Shift+j move left
bindsym $mod+Shift+k move down
bindsym $mod+Shift+l move up
bindsym $mod+Shift+semicolon move right

# ============================================
# KEYBINDINGS - WINDOW MANAGEMENT
# ============================================

# Launch applications
bindsym $mod+Return exec $term
bindsym $mod+d exec $menu
bindsym $mod+b exec $browser
bindsym $mod+f exec $filemanager
bindsym $mod+e exec $editor

# Kill window
bindsym $mod+Shift+q kill

# Fullscreen
bindsym $mod+f fullscreen toggle

# Floating toggle
bindsym $mod+Shift+space floating toggle

# Focus mode (tiling/floating)
bindsym $mod+Shift+f focus mode_toggle

# ============================================
# KEYBINDINGS - LAYOUT
# ============================================

# Layout modes
bindsym $mod+s layout stacking
bindsym $mod+w layout tabbed
bindsym $mod+e layout toggle split
bindsym $mod+r layout toggle split

# Split orientation
bindsym $mod+v split v
bindsym $mod+h split h

# ============================================
# KEYBINDINGS - WORKSPACES
# ============================================

# Switch workspace
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

# Move window to workspace
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

# Move to next/previous workspace
bindsym $mod+Tab workspace next
bindsym $mod+Shift+Tab workspace prev

# ============================================
# KEYBINDINGS - RESIZE
# ============================================

# Enter resize mode
mode "resize" {
    bindsym j resize shrink width 10px
    bindsym k resize grow height 10px
    bindsym l resize shrink height 10px
    bindsym semicolon resize grow width 10px
    
    bindsym Left resize shrink width 10px
    bindsym Down resize grow height 10px
    bindsym Up resize shrink height 10px
    bindsym Right resize grow width 10px
    
    bindsym Return mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+r mode "resize"

# ============================================
# KEYBINDINGS - SYSTEM
# ============================================

# Reload config
bindsym $mod+Shift+c reload

# Exit nested session
bindsym $mod+Shift+e exec swaymsg exit

# Lock screen (nếu có swaylock)
bindsym $mod+x exec swaylock

# ============================================
# APPEARANCE
# ============================================

# Font
font pango:DejaVu Sans Mono 10

# Gaps
gaps inner 10
gaps outer 5

# Border
default_border pixel 2
default_floating_border pixel 2
hide_edge_borders smart

# Colors - Catppuccin Mocha theme
set $bg #1e1e2e
set $fg #cdd6f4
set $accent #89b4fa
set $red #f38ba8
set $green #a6e3a1
set $yellow #f9e2af
set $blue #89b4fa
set $magenta #f5c2e7
set $cyan #94e2d5

# Client colors
client.focused          $accent $accent $fg
client.focused_inactive $bg $bg $fg
client.unfocused        $bg $bg $fg
client.urgent           $red $red $fg
client.placeholder      $bg $bg $fg

# ============================================
# WINDOW RULES
# ============================================

# Floating windows
for_window [class="Pavucontrol"] floating enable
for_window [class="Nm-connection-editor"] floating enable
for_window [class="Blueman-manager"] floating enable
for_window [class="Gcolor3"] floating enable
for_window [class="Gpick"] floating enable
for_window [class="Gnome-calculator"] floating enable
for_window [class="Gnome-screenshot"] floating enable
for_window [class="Gnome-system-monitor"] floating enable
for_window [class="Pavucontrol"] floating enable
for_window [class="Rofi"] floating enable
for_window [class="Wofi"] floating enable
for_window [class="Zathura"] floating enable

# Size rules
for_window [class="Pavucontrol"] resize set 800 600
for_window [class="Rofi"] resize set 800 600

# ============================================
# STARTUP APPLICATIONS
# ============================================

# Chỉ chạy khi không phải nested mode
# exec_always --no-startup-id waybar
# exec_always --no-startup-id swaybg -i ~/Pictures/wallpaper.jpg

# ============================================
# MISC
# ============================================

# Focus on window activation
focus_follows_mouse no
focus_wrapping yes

# Mouse warping
mouse_warping output

# Titlebar (nếu cần)
# default_border pixel 0
# titlebar_border_thickness 0
# titlebar_padding 0 0

# ============================================
# XWAYLAND
# ============================================

# XWayland được tự động enable
# Để disable: xwayland disable

# ============================================
# IPC
# ============================================

# Sway IPC socket tại $XDG_RUNTIME_DIR/sway-ipc.*.sock
# Sử dụng swaymsg để giao tiếp

# ============================================
# DEBUG
# ============================================

# Uncomment để debug
# debuglog on
