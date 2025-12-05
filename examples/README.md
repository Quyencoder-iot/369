# Examples và Scripts

Thư mục này chứa các config files và scripts thực tế để bạn có thể sử dụng ngay.

## Cấu Trúc

```
examples/
├── sway-configs/       # Sway configuration examples
│   └── minimal-config  # Minimal config for nested mode
│
├── sway-scripts/       # Sway automation scripts
│   ├── screenshot.sh   # Screenshot tool
│   ├── window-switcher.sh  # Window switcher
│   └── power-menu.sh   # Power menu
│
└── plasma-scripts/     # Plasma automation scripts
    └── theme-toggle.sh # Toggle light/dark theme
```

## Cách Sử Dụng

### Sway Configs

```bash
# Copy minimal config for testing
cp examples/sway-configs/minimal-config ~/.config/sway/config

# Test in nested mode
sway
```

### Sway Scripts

```bash
# Make scripts executable
chmod +x examples/sway-scripts/*.sh

# Copy to your config directory
cp examples/sway-scripts/*.sh ~/.config/sway/scripts/

# Add to sway config:
# bindsym Print exec ~/.config/sway/scripts/screenshot.sh area
# bindsym $mod+Tab exec ~/.config/sway/scripts/window-switcher.sh
# bindsym $mod+Shift+p exec ~/.config/sway/scripts/power-menu.sh
```

### Plasma Scripts

```bash
# Make executable
chmod +x examples/plasma-scripts/*.sh

# Copy to bin directory
cp examples/plasma-scripts/*.sh ~/.local/bin/

# Add keyboard shortcut in System Settings:
# System Settings → Shortcuts → Custom Shortcuts
# New → Global Shortcut → Command/URL
# Command: ~/.local/bin/theme-toggle.sh
# Trigger: Meta+T (or your choice)
```

## Dependencies

### Sway Scripts

```bash
sudo apt install -y \
    grim \
    slurp \
    wl-clipboard \
    wofi \
    mako-notifier \
    jq
```

### Plasma Scripts

```bash
# Usually already installed with Plasma
# No additional dependencies needed
```

## Customization

Feel free to modify these scripts to fit your needs:

- Change keybindings in configs
- Adjust screenshot directory
- Modify power menu options
- Add your own menu items

## Documentation

Xem tài liệu chi tiết tại:
- [Sway Operation Guide](../docs/05-operate-sway.md)
- [Plasma Operation Guide](../docs/06-operate-plasma.md)
- [Practical Examples](../docs/08-practical-examples.md)
