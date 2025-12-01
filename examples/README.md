# LayerShellQt Examples

This folder contains practical, working examples demonstrating LayerShellQt usage.

## Examples

### 01-simple-panel.cpp
**A basic top panel with clock**
- Layer: TOP
- Demonstrates: Basic setup, anchors, exclusive zone
- Use case: Taskbar, menu bar

**Run:**
```bash
./01-simple-panel
```

---

### 02-notification.cpp
**Notification overlay in top-right corner**
- Layer: TOP
- Demonstrates: Corner positioning, auto-hide, no exclusive zone
- Use case: Toast notifications, alerts

**Run:**
```bash
./02-notification
```

---

### 03-desktop-widget.cpp
**System monitor widget in bottom-right**
- Layer: BOTTOM
- Demonstrates: Below-window layer, system info display
- Use case: Desktop widgets, Conky-style monitors

**Run:**
```bash
./03-desktop-widget
```

---

### 04-animated-wallpaper.cpp
**Animated gradient wallpaper**
- Layer: BACKGROUND
- Demonstrates: Full-screen, animation, custom painting
- Use case: Live wallpapers, background effects

**Run:**
```bash
./04-animated-wallpaper
```

---

### 05-multi-monitor-panel.cpp
**Per-monitor panels with hotplug support**
- Layer: TOP
- Demonstrates: Multi-monitor, scope, screen add/remove
- Use case: Multi-monitor panel systems

**Run:**
```bash
./05-multi-monitor-panel
```

---

## Building

### Using CMake (Recommended)

```bash
cd examples
mkdir build && cd build
cmake ..
make
```

**Run examples:**
```bash
./01-simple-panel
./02-notification
# etc.
```

---

### Manual Compilation

**Single example:**
```bash
g++ 01-simple-panel.cpp -o 01-simple-panel \
    $(pkg-config --cflags --libs Qt5Widgets LayerShellQtInterface) \
    -fPIC
```

**All examples:**
```bash
for file in *.cpp; do
    name="${file%.cpp}"
    echo "Building $name..."
    g++ "$file" -o "$name" \
        $(pkg-config --cflags --libs Qt5Widgets LayerShellQtInterface) \
        -fPIC
done
```

---

## Requirements

- Qt 5.15+
- LayerShellQt library
- Wayland compositor with wlr-layer-shell support:
  - KDE Plasma (Wayland)
  - Sway
  - Hyprland
  - River

**Check support:**
```bash
# List available protocols
wayland-info | grep layer_shell
```

---

## Running

**Make sure you're on Wayland:**
```bash
echo $XDG_SESSION_TYPE
# Should output: wayland
```

**If on X11, switch to Wayland session**

---

## Troubleshooting

### "Layer shell not available"

**Cause:** Not on Wayland or compositor doesn't support wlr-layer-shell

**Solutions:**
1. Switch to Wayland session
2. Use supported compositor (KDE Plasma, Sway, Hyprland)
3. Check with: `wayland-info | grep layer_shell`

---

### Compilation errors

**Missing headers:**
```bash
# Install development packages
sudo apt install libkf5waylandclient-dev qt5-wayland qtbase5-dev

# Or
sudo dnf install kf5-kwayland-devel qt5-qtwayland-devel

# Or
sudo pacman -S layer-shell-qt qt5-wayland
```

**Missing pkg-config:**
```bash
# Check if LayerShellQt is installed
pkg-config --modversion LayerShellQtInterface
```

---

## Modification Ideas

Try modifying the examples:

1. **Change colors/styles** in the stylesheets
2. **Add buttons** to panels
3. **Change positions** (try different anchors)
4. **Adjust sizes** and margins
5. **Add animations** to widgets
6. **Connect to D-Bus** for system integration

---

## Next Steps

After trying these examples:
1. Read the full documentation in `../docs/`
2. Combine examples (e.g., panel + notifications)
3. Build your own desktop shell!

Happy coding! 🚀
