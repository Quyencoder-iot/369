# Tích Hợp Sway Nested với Plasma

## Cách tích hợp Sway nested vào Plasma Desktop

### 1. Tạo Desktop Entry

Tạo file `~/.local/share/applications/sway-nested.desktop`:

```ini
[Desktop Entry]
Version=1.0
Type=Application
Name=Sway Nested
Comment=Chạy Sway nested trong Plasma
Exec=/home/YOUR_USERNAME/bin/sway-nested
Icon=sway
Terminal=false
Categories=System;WindowManager;
Keywords=sway;wayland;nested;
```

Thay `YOUR_USERNAME` bằng username của bạn.

### 2. Tạo Shortcut trong Plasma

#### Cách 1: Thông qua Application Launcher
1. Mở Application Launcher (Super key hoặc click icon)
2. Tìm "Sway Nested"
3. Right-click → Add to Desktop/Favorites

#### Cách 2: Tạo Shortcut tùy chỉnh
1. Right-click trên desktop → Add Widgets → Custom Script
2. Hoặc Settings → Shortcuts → Custom Shortcuts
3. Thêm shortcut mới với command: `sway-nested`

### 3. Sử dụng trong Plasma Wayland Session

Nếu bạn đang chạy Plasma trên Wayland:

```bash
# Kiểm tra session
echo $XDG_SESSION_TYPE
# Output: wayland

# Chạy Sway nested
WLR_BACKENDS=headless sway-nested
```

### 4. Tích hợp với Plasma Panels

Để hiển thị thông tin từ Sway trong Plasma panel:

#### Script lấy thông tin từ Sway

```bash
#!/bin/bash
# ~/bin/sway-info.sh
# Hiển thị workspace hiện tại từ Sway

if pgrep -x sway > /dev/null; then
    swaymsg -t get_workspaces | jq -r '.[] | select(.focused==true) | .name'
else
    echo "Sway không chạy"
fi
```

#### Thêm vào Plasma Panel
1. Right-click panel → Add Widgets → Custom Script
2. Chọn script `sway-info.sh`
3. Cấu hình refresh interval

### 5. Share Clipboard giữa Plasma và Sway

Sử dụng `wl-clipboard`:

```bash
# Trong Plasma, copy vào Wayland clipboard
wl-copy "Text từ Plasma"

# Trong Sway, paste từ clipboard
wl-paste
```

### 6. Share Environment Variables

Tạo file `~/.config/sway/env`:

```bash
#!/bin/bash
# Environment variables cho Sway nested

# Share với Plasma
export QT_QPA_PLATFORM=wayland
export GDK_BACKEND=wayland

# DPI scaling (nếu Plasma dùng scale)
export QT_WAYLAND_FORCE_DPI=96

# Theme (nếu muốn dùng cùng theme với Plasma)
export QT_QPA_PLATFORMTHEME=qt5ct
```

Load trong Sway config:
```bash
# Thêm vào đầu ~/.config/sway/config
include ~/.config/sway/env
```

### 7. Sử dụng cùng Font và Theme

#### Font
```bash
# Trong ~/.config/sway/config
font pango:Noto Sans 10
# Hoặc font mà Plasma đang dùng
```

#### Colors
Có thể extract colors từ Plasma theme và dùng trong Sway config.

### 8. Window Rules cho Plasma Apps trong Sway

Thêm vào Sway config:

```bash
# Cho phép Plasma apps chạy trong Sway
for_window [app_id="org.kde.*"] floating enable
for_window [class="plasmashell"] floating enable
for_window [class="krunner"] floating enable
```

### 9. Script khởi động tự động

Tạo `~/.config/plasma-workspace/env/sway-nested.sh`:

```bash
#!/bin/bash
# Tự động khởi động Sway nested khi login vào Plasma
# (Optional - chỉ nếu muốn)

# Uncomment để enable:
# sway-nested &
```

### 10. Troubleshooting Integration

#### Vấn đề: Sway nested không hiển thị trong Plasma
```bash
# Kiểm tra X11/Wayland
echo $XDG_SESSION_TYPE

# Thử với backend cụ thể
WLR_BACKENDS=x11 sway-nested
```

#### Vấn đề: Input không hoạt động
```bash
# Kiểm tra input devices
swaymsg -t get_inputs

# Thử với libinput
LIBINPUT_LOG_LEVEL=debug sway-nested
```

#### Vấn đề: Performance kém
```bash
# Tắt compositor của Plasma khi chạy Sway nested
# Hoặc giảm effects trong Plasma Settings
```

### 11. Best Practices

1. **Không chạy đồng thời**: Tránh chạy nhiều instance Sway nested
2. **Resource management**: Sway nested tiêu tốn RAM và GPU
3. **Testing**: Dùng Sway nested để test config trước khi switch sang full Sway
4. **Backup**: Luôn backup config trước khi thay đổi

### 12. Ví dụ Workflow

```bash
# 1. Mở Plasma
# 2. Chạy terminal
# 3. Chạy Sway nested
sway-nested

# 4. Test config và apps trong Sway nested
# 5. Thoát với Mod+Shift+E
# 6. Chỉnh sửa config nếu cần
# 7. Lặp lại từ bước 3
```
