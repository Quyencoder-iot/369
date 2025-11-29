# Sway Nested Mode - Quick Start Guide

## ⚡ Quick Start (5 minutes)

### 1. Install Configuration

```bash
cd /workspace/sway-config
./install-config.sh
```

### 2. Choose Your Launch Method

Based on your current environment:

#### If you're on **Wayland** (GNOME Wayland, KDE Plasma Wayland, etc.):

```bash
# Option A: Using Weston (recommended)
./launch-sway-nested-weston.sh

# Option B: Using Cage (lighter)
./launch-sway-nested-cage.sh
```

#### If you're on **X11** (GNOME X11, XFCE, etc.):

```bash
./launch-sway-x11.sh
```

#### For **Testing/Development** (no display needed):

```bash
./launch-sway-headless.sh
```

### 3. First Steps in Sway

Once Sway is running:

1. **Open terminal**: `Super + Enter`
2. **Open launcher**: `Super + D`
3. **Close window**: `Super + Shift + Q`
4. **Exit Sway**: `Super + Shift + E`

**Note:** `Super` = Windows key / Command key

---

## 📊 System Status

✅ **Installation Complete!**

All components installed:
- ✓ Sway 1.9
- ✓ Weston 13.0.0
- ✓ Cage, XWayland, Waybar
- ✓ Terminal, launcher, utilities
- ✓ Configuration files
- ✓ Launch scripts

Run test: `./test-installation.sh`

---

## 🎯 Essential Keybindings

| Action | Keybinding |
|--------|-----------|
| Open terminal | `Super + Enter` |
| Open launcher | `Super + D` |
| Close window | `Super + Shift + Q` |
| Switch workspace 1-10 | `Super + 1-9, 0` |
| Move window to workspace | `Super + Shift + 1-9, 0` |
| Fullscreen | `Super + F` |
| Resize mode | `Super + R` |
| Reload config | `Super + Shift + C` |
| Exit Sway | `Super + Shift + E` |

---

## 🔧 Troubleshooting

### "Error: Not running in a Wayland session"

→ Use `./launch-sway-x11.sh` instead

### "Could not start Weston"

→ Check: `echo $WAYLAND_DISPLAY` - if empty, use X11 method

### Keybindings not working

→ Make sure Sway window is focused

---

## 📚 More Information

- Full documentation: See `README.md`
- Test installation: Run `./test-installation.sh`
- Customize config: Edit `~/.config/sway/config`

---

## 🚀 Launch Scripts Summary

| Script | Environment | Use Case |
|--------|-------------|----------|
| `launch-sway-nested-weston.sh` | Wayland | Full featured, windowed |
| `launch-sway-nested-cage.sh` | Wayland | Lightweight, simple |
| `launch-sway-x11.sh` | X11/Wayland | Universal compatibility |
| `launch-sway-headless.sh` | None | Testing, development |

Choose the script that matches your current session!
