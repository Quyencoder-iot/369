# KDE Plasma - Hướng Dẫn Chi Tiết & Tích Hợp với Sway

## Mục lục
1. [KDE Plasma là gì?](#kde-plasma-là-gì)
2. [Kiến trúc KDE Plasma](#kiến-trúc-kde-plasma)
3. [Cài đặt KDE Plasma](#cài-đặt-kde-plasma)
4. [Plasma Wayland vs X11](#plasma-wayland-vs-x11)
5. [KWin - Window Manager](#kwin---window-manager)
6. [Tích hợp Sway với Plasma](#tích-hợp-sway-với-plasma)
7. [Cấu hình nâng cao](#cấu-hình-nâng-cao)

---

## KDE Plasma là gì?

### Định nghĩa
**KDE Plasma** là một desktop environment (DE) miễn phí và mã nguồn mở, được phát triển bởi cộng đồng KDE. Đây là một trong những DE phổ biến và mạnh mẽ nhất cho Linux.

### Đặc điểm chính

```
┌─────────────────────────────────────────────────────────────────────┐
│                    KDE Plasma Features                               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  🎨 CUSTOMIZATION                    🚀 PERFORMANCE                  │
│  ├── Themes & Colors                 ├── Lightweight core           │
│  ├── Widgets & Panels                ├── Hardware acceleration      │
│  ├── Desktop Effects                 ├── Efficient memory usage     │
│  └── Activities & Workspaces         └── Wayland support            │
│                                                                      │
│  🔧 INTEGRATION                      📱 MODERN UX                    │
│  ├── KDE Connect (phone sync)        ├── Touch-friendly             │
│  ├── Dolphin File Manager            ├── HiDPI support              │
│  ├── System Settings                 ├── Night Color                │
│  └── Discover Software Center        └── Multi-monitor              │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Kiến trúc KDE Plasma

### Component Stack

```
┌─────────────────────────────────────────────────────────────────────┐
│                    KDE Plasma Architecture                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌───────────────────────────────────────────────────────────┐      │
│  │                    Plasma Desktop Shell                    │      │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │      │
│  │  │  Panels  │  │ Widgets  │  │Activities│  │  Desktop │  │      │
│  │  │ (taskbar)│  │(plasmoid)│  │          │  │  Shell   │  │      │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘  │      │
│  └───────────────────────────────────────────────────────────┘      │
│                              │                                       │
│                              ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐      │
│  │                         KWin                               │      │
│  │              (Window Manager / Compositor)                 │      │
│  │  ┌─────────────────┐     ┌─────────────────────────┐      │      │
│  │  │   kwin_x11      │     │     kwin_wayland        │      │      │
│  │  │ (X11 backend)   │     │   (Wayland backend)     │      │      │
│  │  └─────────────────┘     └─────────────────────────┘      │      │
│  └───────────────────────────────────────────────────────────┘      │
│                              │                                       │
│                              ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐      │
│  │                  KDE Frameworks (KF5/KF6)                  │      │
│  │                                                            │      │
│  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐   │      │
│  │  │  KIO   │ │Kirigami│ │KConfig │ │ Solid  │ │Plasma  │   │      │
│  │  │(I/O)   │ │  (UI)  │ │        │ │(HW)    │ │Framework│  │      │
│  │  └────────┘ └────────┘ └────────┘ └────────┘ └────────┘   │      │
│  └───────────────────────────────────────────────────────────┘      │
│                              │                                       │
│                              ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐      │
│  │                      Qt Framework                          │      │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────────────┐  │      │
│  │  │Qt Core  │ │Qt GUI   │ │Qt Quick │ │Qt Wayland       │  │      │
│  │  │         │ │         │ │(QML)    │ │Compositor       │  │      │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────────────┘  │      │
│  └───────────────────────────────────────────────────────────┘      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Thành phần chính

| Component | Mô tả |
|-----------|-------|
| **Plasma Shell** | Desktop shell, panels, widgets |
| **KWin** | Window manager và compositor |
| **System Settings** | Cấu hình hệ thống tập trung |
| **Dolphin** | File manager |
| **Konsole** | Terminal emulator |
| **KRunner** | Application launcher |
| **KDE Connect** | Kết nối với điện thoại |

---

## Cài đặt KDE Plasma

### Ubuntu/Debian

```bash
# Cài đặt Plasma Desktop đầy đủ
sudo apt install kde-plasma-desktop

# Hoặc cài đặt full KDE (với tất cả ứng dụng)
sudo apt install kde-full

# Cài đặt minimal (chỉ desktop)
sudo apt install plasma-desktop

# Cài thêm Plasma Wayland session
sudo apt install plasma-workspace-wayland

# Cài các ứng dụng KDE cần thiết
sudo apt install \
    konsole \
    dolphin \
    kate \
    ark \
    gwenview \
    spectacle \
    krunner
```

### Fedora

```bash
# Cài đặt Plasma Desktop
sudo dnf install @kde-desktop

# Hoặc từng package
sudo dnf install \
    plasma-workspace \
    plasma-desktop \
    kwin \
    plasma-workspace-wayland \
    konsole \
    dolphin
```

### Arch Linux

```bash
# Cài đặt Plasma group
sudo pacman -S plasma

# Cài đặt minimal
sudo pacman -S \
    plasma-desktop \
    plasma-wayland-session \
    konsole \
    dolphin

# Cài thêm KDE applications
sudo pacman -S kde-applications
```

### Xác minh cài đặt

```bash
# Kiểm tra Plasma version
plasmashell --version

# Kiểm tra KWin
kwin_wayland --version

# Kiểm tra Qt
qmake --version
```

---

## Plasma Wayland vs X11

### So sánh

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Plasma X11 vs Wayland                             │
├───────────────────────────────┬─────────────────────────────────────┤
│         Plasma X11            │           Plasma Wayland            │
├───────────────────────────────┼─────────────────────────────────────┤
│                               │                                      │
│  ✅ Mature, stable            │  ✅ Modern architecture              │
│  ✅ Wide app compatibility    │  ✅ Better security                  │
│  ✅ Screen sharing works      │  ✅ Smoother performance             │
│  ✅ Remote desktop easy       │  ✅ Per-monitor scaling              │
│                               │  ✅ Touchpad gestures                │
│  ❌ Security issues           │                                      │
│  ❌ Tearing possible          │  ⚠️ Some apps need XWayland         │
│  ❌ No per-monitor scale      │  ⚠️ Screen sharing needs portals    │
│                               │  ⚠️ Some features still developing  │
│                               │                                      │
└───────────────────────────────┴─────────────────────────────────────┘
```

### Chọn session khi login

```bash
# Kiểm tra available sessions
ls /usr/share/wayland-sessions/
ls /usr/share/xsessions/

# Thường thấy:
# /usr/share/wayland-sessions/plasma.desktop    # Plasma Wayland
# /usr/share/xsessions/plasma.desktop           # Plasma X11
```

### Kiểm tra session hiện tại

```bash
# Kiểm tra session type
echo $XDG_SESSION_TYPE
# Output: wayland hoặc x11

# Kiểm tra compositor
echo $XDG_CURRENT_DESKTOP
# Output: KDE

# Trong Plasma Wayland, kiểm tra KWin
pgrep -a kwin
# Thấy: kwin_wayland (Wayland) hoặc kwin_x11 (X11)
```

---

## KWin - Window Manager

### Giới thiệu
**KWin** là window manager và compositor của KDE Plasma. Nó có hai backends:
- **kwin_x11**: Cho X11 sessions
- **kwin_wayland**: Cho Wayland sessions (cũng là Wayland compositor)

### Kiến trúc KWin Wayland

```
┌─────────────────────────────────────────────────────────────────────┐
│                      KWin Wayland Architecture                       │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌───────────────────────────────────────────────────────────┐      │
│  │                     KWin Core                              │      │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │      │
│  │  │   Window    │  │  Compositing│  │   Effects   │        │      │
│  │  │  Management │  │   Engine    │  │   System    │        │      │
│  │  └─────────────┘  └─────────────┘  └─────────────┘        │      │
│  │                                                            │      │
│  │  ┌─────────────────────────────────────────────────┐      │      │
│  │  │              Wayland Server                      │      │      │
│  │  │  - wl_compositor  - xdg_shell  - plasma_shell    │      │      │
│  │  │  - wl_output      - wl_seat    - layer_shell     │      │      │
│  │  └─────────────────────────────────────────────────┘      │      │
│  │                                                            │      │
│  │  ┌─────────────────────────────────────────────────┐      │      │
│  │  │              XWayland Support                    │      │      │
│  │  │       (Chạy X11 apps trong Wayland)              │      │      │
│  │  └─────────────────────────────────────────────────┘      │      │
│  └───────────────────────────────────────────────────────────┘      │
│                              │                                       │
│          ┌───────────────────┼───────────────────┐                  │
│          │                   │                   │                  │
│          ▼                   ▼                   ▼                  │
│   ┌────────────┐      ┌────────────┐      ┌────────────┐            │
│   │  Wayland   │      │  XWayland  │      │   Native   │            │
│   │   Clients  │      │   (X11)    │      │    Apps    │            │
│   │ (Qt, GTK)  │      │   Clients  │      │  (Plasma)  │            │
│   └────────────┘      └────────────┘      └────────────┘            │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### KWin Scripting & Effects

```javascript
// Ví dụ KWin script: ~/kwin-script-example.js
// Tự động tile windows theo pattern

function tileWindow(client) {
    if (client.normalWindow && !client.fullScreen) {
        client.geometry = {
            x: 0,
            y: 0,
            width: workspace.displayWidth / 2,
            height: workspace.displayHeight
        };
    }
}

workspace.clientAdded.connect(tileWindow);
```

---

## Tích hợp Sway với Plasma

### Phương pháp 1: Chạy Sway Nested trong Plasma Wayland

```bash
# Script: ~/.local/bin/sway-in-plasma
#!/bin/bash

# Đảm bảo đang chạy trong Plasma Wayland
if [ "$XDG_CURRENT_DESKTOP" != "KDE" ]; then
    echo "Error: Not running in KDE Plasma"
    exit 1
fi

if [ "$XDG_SESSION_TYPE" != "wayland" ]; then
    echo "Error: Not in Wayland session"
    echo "Please use Plasma Wayland"
    exit 1
fi

# Set backend
export WLR_BACKENDS=wayland

# Use nested config nếu có
if [ -f "$HOME/.config/sway/config.nested" ]; then
    exec sway -c "$HOME/.config/sway/config.nested"
else
    exec sway
fi
```

### Phương pháp 2: Sway window với KDE integration

```bash
# File: ~/.config/sway/config.plasma-nested

# ─────────────────────────────────────────────────────────────────────
# PLASMA-FRIENDLY SWAY CONFIG
# ─────────────────────────────────────────────────────────────────────

# Dùng Alt để tránh xung đột với Plasma (Super)
set $mod Mod1

# Variables
set $term konsole
set $menu krunner

# Basic keybindings
bindsym $mod+Return exec $term
bindsym $mod+Shift+q kill
bindsym $mod+d exec $menu

# EXIT - Quan trọng!
bindsym $mod+Shift+e exec swaymsg exit

# Navigation
bindsym $mod+h focus left
bindsym $mod+j focus down
bindsym $mod+k focus up
bindsym $mod+l focus right

bindsym $mod+Shift+h move left
bindsym $mod+Shift+j move down
bindsym $mod+Shift+k move up
bindsym $mod+Shift+l move right

# Workspaces
bindsym $mod+1 workspace 1
bindsym $mod+2 workspace 2
bindsym $mod+3 workspace 3
bindsym $mod+4 workspace 4
bindsym $mod+5 workspace 5

bindsym $mod+Shift+1 move container to workspace 1
bindsym $mod+Shift+2 move container to workspace 2
bindsym $mod+Shift+3 move container to workspace 3
bindsym $mod+Shift+4 move container to workspace 4
bindsym $mod+Shift+5 move container to workspace 5

# Layout
bindsym $mod+b splith
bindsym $mod+v splitv
bindsym $mod+f fullscreen
bindsym $mod+Shift+space floating toggle
bindsym $mod+space focus mode_toggle

# Resize mode
mode "resize" {
    bindsym h resize shrink width 10px
    bindsym j resize grow height 10px
    bindsym k resize shrink height 10px
    bindsym l resize grow width 10px
    bindsym Return mode "default"
    bindsym Escape mode "default"
}
bindsym $mod+r mode "resize"

# ─────────────────────────────────────────────────────────────────────
# APPEARANCE
# ─────────────────────────────────────────────────────────────────────

# Match Breeze Dark theme
output * bg #1b1e20 solid_color

gaps inner 5
gaps outer 2
default_border pixel 2

# Breeze-inspired colors
client.focused          #3daee9 #31363b #eff0f1 #3daee9 #3daee9
client.focused_inactive #4d4d4d #31363b #eff0f1 #4d4d4d #4d4d4d
client.unfocused        #31363b #232629 #7f8c8d #31363b #31363b
client.urgent           #da4453 #da4453 #eff0f1 #da4453 #da4453

# ─────────────────────────────────────────────────────────────────────
# MINIMAL STATUS BAR
# ─────────────────────────────────────────────────────────────────────

bar {
    position top
    height 24
    
    status_command while echo "$(date +'%H:%M') | Sway"; do sleep 30; done
    
    colors {
        background #31363b
        statusline #eff0f1
        separator #4d4d4d
        
        focused_workspace  #3daee9 #3daee9 #31363b
        active_workspace   #4d4d4d #4d4d4d #eff0f1
        inactive_workspace #31363b #31363b #eff0f1
        urgent_workspace   #da4453 #da4453 #eff0f1
    }
}

# ─────────────────────────────────────────────────────────────────────
# NO EXTRA SERVICES (dùng của Plasma)
# ─────────────────────────────────────────────────────────────────────
# Không cần:
# - Notification daemon (dùng của Plasma)
# - Idle management (Plasma handles)
# - Screen locker (Plasma handles)
```

### Phương pháp 3: Desktop Entry để launch từ Plasma

```bash
# File: ~/.local/share/applications/sway-nested.desktop

[Desktop Entry]
Type=Application
Name=Sway (Nested)
Comment=Run Sway tiling WM in a window
Icon=sway
Exec=env WLR_BACKENDS=wayland sway -c ~/.config/sway/config.plasma-nested
Terminal=false
Categories=System;
Keywords=tiling;window;manager;sway;
```

### Phương pháp 4: KDE Activity Integration

```bash
# Concept: Một Activity riêng cho Sway workflow
# 1. Tạo Activity mới trong Plasma: "Development"
# 2. Trong Activity đó, có một panel với icon launch Sway
# 3. Khi cần tiling workflow, switch sang Activity và launch Sway

# Autostart script cho Development activity
# File: ~/.config/autostart-scripts/sway-activity.sh
#!/bin/bash

# Detect current activity
current_activity=$(qdbus org.kde.ActivityManager /ActivityManager/Activities CurrentActivity)

# Development activity ID (lấy từ: qdbus org.kde.ActivityManager /ActivityManager/Activities ListActivities)
dev_activity_id="your-activity-uuid-here"

if [ "$current_activity" = "$dev_activity_id" ]; then
    # In development activity, offer to start Sway
    kdialog --yesno "Start Sway tiling session?" && \
        WLR_BACKENDS=wayland sway -c ~/.config/sway/config.plasma-nested
fi
```

---

## Cấu hình nâng cao

### Sử dụng KDE services từ Sway

```bash
# Trong Sway nested, bạn vẫn có thể dùng KDE apps và services

# File: ~/.config/sway/config.plasma-nested (thêm vào)

# Use KDE applications
set $filemanager dolphin
set $browser firefox
set $editor kate
set $terminal konsole

# KDE app keybindings
bindsym $mod+e exec $filemanager
bindsym $mod+w exec $browser
bindsym $mod+n exec $editor

# KDE system shortcuts (nên match với Plasma)
bindsym $mod+Shift+s exec spectacle --region  # Screenshot region
bindsym Print exec spectacle                   # Full screenshot

# Volume control (sử dụng Plasma's audio)
bindsym XF86AudioRaiseVolume exec pactl set-sink-volume @DEFAULT_SINK@ +5%
bindsym XF86AudioLowerVolume exec pactl set-sink-volume @DEFAULT_SINK@ -5%
bindsym XF86AudioMute exec pactl set-sink-mute @DEFAULT_SINK@ toggle

# Media keys (playerctl works với Plasma media)
bindsym XF86AudioPlay exec playerctl play-pause
bindsym XF86AudioNext exec playerctl next
bindsym XF86AudioPrev exec playerctl previous

# Brightness (if laptop)
bindsym XF86MonBrightnessUp exec brightnessctl set +10%
bindsym XF86MonBrightnessDown exec brightnessctl set 10%-
```

### Dùng KRunner từ Sway

```bash
# KRunner có thể được gọi từ Sway
bindsym $mod+space exec krunner

# Hoặc dùng cả hai
bindsym $mod+d exec wofi --show drun
bindsym $mod+Shift+d exec krunner
```

### Clipboard sharing giữa Plasma và Sway

```bash
# Clipboard tự động được share vì cùng Wayland session
# Nhưng có thể cần wl-clipboard tools

# Copy từ Sway app
wl-copy < file.txt

# Paste vào Plasma app
wl-paste > output.txt

# Clipboard history (dùng của Plasma: Klipper)
```

---

## Workflows thực tế

### Workflow 1: Developer Environment

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Developer Workflow                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  PLASMA (Main Desktop)                                               │
│  ├── Panel (bottom): Task manager, System tray, Clock                │
│  │                                                                   │
│  ├── Virtual Desktop 1: "Communication"                              │
│  │   ├── Slack                                                       │
│  │   ├── Thunderbird                                                 │
│  │   └── Teams                                                       │
│  │                                                                   │
│  ├── Virtual Desktop 2: "Development" ◄── SWAY NESTED                │
│  │   └── [Sway Window - Fullscreen]                                  │
│  │       ├── Workspace 1: Editor (nvim)                              │
│  │       │   ┌─────────────────┬─────────────────┐                   │
│  │       │   │    nvim         │   Terminal      │                   │
│  │       │   │    (code)       │   (compile)     │                   │
│  │       │   ├─────────────────┴─────────────────┤                   │
│  │       │   │          Terminal (logs)          │                   │
│  │       │   └───────────────────────────────────┘                   │
│  │       │                                                           │
│  │       ├── Workspace 2: Testing                                    │
│  │       │   ┌───────────────────────────────────┐                   │
│  │       │   │         Browser (testing)         │                   │
│  │       │   ├───────────────────────────────────┤                   │
│  │       │   │      Terminal (test runner)       │                   │
│  │       │   └───────────────────────────────────┘                   │
│  │       │                                                           │
│  │       └── Workspace 3: Git/Database                               │
│  │                                                                   │
│  └── Virtual Desktop 3: "Media"                                      │
│      ├── Spotify                                                     │
│      └── VLC                                                         │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Workflow 2: Content Creation

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Content Creator Workflow                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  PLASMA                                                              │
│  ├── Activity 1: "Video Editing" (Plasma-native apps)                │
│  │   ├── Kdenlive (video editor)                                    │
│  │   ├── Dolphin (file management)                                  │
│  │   └── Spectacle (screenshots)                                    │
│  │                                                                   │
│  ├── Activity 2: "Writing" (Sway for focused work)                  │
│  │   └── [Sway Nested - Distraction-free]                           │
│  │       ├── neovim/markdown                                        │
│  │       └── Preview                                                │
│  │                                                                   │
│  └── Activity 3: "Research" (Plasma for flexibility)                │
│      ├── Firefox (multiple tabs/windows)                            │
│      ├── Okular (PDF reader)                                        │
│      └── Obsidian (notes)                                           │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Tips & Tricks

### 1. Quick switch giữa Plasma và Sway

```bash
# Đặt Sway fullscreen và dùng như một "virtual desktop"
# Trong Sway config:
bindsym $mod+Escape exec swaymsg exit  # Quick exit

# Trong Plasma, set shortcut để focus Sway window
# System Settings → Shortcuts → KWin → "Window to Desktop X"
```

### 2. Consistent theming

```bash
# Match Sway colors với Plasma theme
# Breeze Dark colors:
# Background: #1b1e20 hoặc #31363b
# Foreground: #eff0f1
# Accent: #3daee9
# Urgent: #da4453

# GTK apps trong Sway:
export GTK_THEME=Breeze-Dark
export QT_QPA_PLATFORMTHEME=kde
```

### 3. Shared configurations

```bash
# Dùng chung một số settings
# File: ~/.config/environment.d/shared.conf

EDITOR=nvim
BROWSER=firefox
TERMINAL=konsole
```

---

## Tiếp theo

- [Kiến trúc Wayland](05-wayland-architecture.md) - Hiểu sâu về protocol
- [Scripts và Thực hành](06-scripts-thuc-hanh.md) - Scripts automation
