# Quick Reference: Sway và Plasma

## Sway Keybindings

### Cơ Bản
- `Mod+Return` - Mở terminal
- `Mod+D` - Mở application launcher
- `Mod+Shift+Q` - Đóng cửa sổ
- `Mod+Shift+C` - Reload config
- `Mod+Shift+E` - Exit Sway

### Di Chuyển Focus
- `Mod+H/J/K/L` - Focus left/down/up/right
- `Mod+Shift+H/J/K/L` - Move window left/down/up/right

### Workspaces
- `Mod+1-9,0` - Chuyển workspace
- `Mod+Shift+1-9,0` - Di chuyển cửa sổ sang workspace

### Layout
- `Mod+S` - Stacking layout
- `Mod+W` - Tabbed layout
- `Mod+E` - Toggle split
- `Mod+F` - Fullscreen toggle
- `Mod+Shift+Space` - Floating toggle

## Sway Commands

```bash
# IPC commands
swaymsg -t get_tree          # Xem cây cửa sổ
swaymsg -t get_outputs       # Xem outputs
swaymsg -t get_workspaces    # Xem workspaces
swaymsg workspace 2          # Chuyển workspace
swaymsg exec firefox         # Chạy ứng dụng
swaymsg reload               # Reload config
```

## Plasma Keybindings

- `Alt+F2` - KRunner
- `Alt+Space` - KRunner
- `Alt+Tab` - Switch windows
- `Meta` - Activities overview
- `Ctrl+Alt+L` - Lock screen
- `Ctrl+Alt+Del` - Logout menu

## Plasma Commands

```bash
# System
systemsettings5              # System Settings
krunner                      # KRunner
plasmashell                  # Plasma shell

# KWin
qdbus org.kde.KWin /KWin org.kde.KWin.compositingActive
```

## Wayland Environment Variables

```bash
# Session type
XDG_SESSION_TYPE=wayland

# Display
WAYLAND_DISPLAY=wayland-0

# Backend cho nested
WLR_BACKENDS=headless
WLR_HEADLESS_OUTPUTS=1
```

## File Locations

### Sway
- Config: `~/.config/sway/config`
- Logs: `~/.local/share/sway/sway.log`
- IPC socket: `/tmp/sway-ipc.*.sock`

### Plasma
- Config: `~/.config/kwinrc`
- Plasma config: `~/.config/plasma-org.kde.plasma.desktop-appletsrc`
- KDE config: `~/.config/kdeglobals`

## Debugging

```bash
# Sway debug
sway -d 2>&1 | tee sway.log
WLR_DEBUG=1 sway

# Plasma debug
QT_LOGGING_RULES="kwin*=true" kwin_wayland
QT_LOGGING_RULES="org.kde.plasmashell*=true" plasmashell

# Wayland info
wayland-info
```

## Useful Tools

```bash
# Clipboard
wl-clipboard                # Wayland clipboard
wl-paste                    # Paste
wl-copy                     # Copy

# Screenshot
grim                        # Screenshot tool
slurp                       # Region selector
grim -g "$(slurp)" - | wl-copy

# Screen recording
wf-recorder                 # Wayland recorder

# Monitor
waybar                      # Status bar
mako                        # Notification daemon
```

## Common Issues

### Sway không khởi động
```bash
# Kiểm tra logs
journalctl -b | grep sway
sway -d

# Kiểm tra permissions
ls -l /dev/dri/*
groups
```

### Nested không hiển thị
```bash
# Kiểm tra session
echo $WAYLAND_DISPLAY
echo $DISPLAY

# Thử với explicit backend
WLR_BACKENDS=headless WLR_HEADLESS_OUTPUTS=1 sway
```

### Input không hoạt động
```bash
# Kiểm tra input devices
swaymsg -t get_inputs
libinput list-devices

# Kiểm tra group
groups | grep input
```
