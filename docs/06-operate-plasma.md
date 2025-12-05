# Hướng Dẫn Vận Hành và Cấu Hình KDE Plasma

## Mục Lục
1. [Giao diện cơ bản](#giao-diện-cơ-bản)
2. [System Settings](#system-settings)
3. [Desktop và Widgets](#desktop-và-widgets)
4. [Panel Configuration](#panel-configuration)
5. [Keyboard Shortcuts](#keyboard-shortcuts)
6. [Window Management](#window-management)
7. [Customization](#customization)
8. [Advanced Features](#advanced-features)

---

## Giao Diện Cơ Bản

### Desktop Layout

```
┌──────────────────────────────────────────────────┐
│  Application Menu  │  System Tray  │   Clock     │ ← Top Panel (optional)
├──────────────────────────────────────────────────┤
│                                                  │
│                                                  │
│                                                  │
│              Desktop Workspace                   │
│              - Wallpaper                         │
│              - Desktop Widgets                   │
│              - Desktop Icons (optional)          │
│                                                  │
│                                                  │
├──────────────────────────────────────────────────┤
│ [App Launcher] [Apps] [Files]  [Tray] [Clock]   │ ← Bottom Panel (default)
└──────────────────────────────────────────────────┘
```

### Components Chính

1. **Panel (Taskbar)**
   - Application Launcher
   - Task Manager (running apps)
   - System Tray
   - Clock

2. **Desktop**
   - Wallpaper
   - Widgets (Plasmoids)
   - Desktop Icons (nếu enabled)

3. **Application Menu**
   - Application Launcher (menu)
   - Search functionality
   - Recent files/apps

4. **System Tray**
   - Network
   - Volume
   - Notifications
   - Battery (laptops)

---

## System Settings

### Accessing System Settings

```bash
# Method 1: Application Launcher
# Click Launcher → type "System Settings"

# Method 2: Right-click desktop
# Right-click desktop → Configure Desktop and Wallpaper

# Method 3: Command line
systemsettings5   # or systemsettings6 for Plasma 6

# Method 4: Keyboard shortcut (if configured)
# Usually: Meta/Super/Win key → type "settings"
```

### System Settings Categories

#### Appearance

```
System Settings → Appearance
├── Global Theme
│   └── Breeze, Breeze Dark, etc.
├── Plasma Style
│   └── Widget style
├── Application Style
│   └── Window decorations
├── Colors
│   └── Color schemes
├── Icons
│   └── Icon themes
├── Fonts
│   └── Font configuration
└── Cursors
    └── Mouse cursor themes
```

**Quick theme change:**

```bash
# Via GUI:
System Settings → Appearance → Global Theme

# Via command line:
# List themes
plasma-apply-colorscheme --list-schemes

# Apply theme
plasma-apply-colorscheme BreezeDark

# Look and Feel
plasma-apply-lookandfeel -l  # list
plasma-apply-lookandfeel org.kde.breezedark.desktop  # apply
```

#### Workspace Behavior

```
System Settings → Workspace Behavior
├── General Behavior
│   ├── Animation speed
│   └── Clicking files/folders
├── Desktop Effects
│   ├── Blur
│   ├── Fade
│   ├── Slide
│   └── Many more...
├── Screen Edges
│   └── Hot corners/edges
├── Screen Locking
│   └── Lock screen settings
├── Virtual Desktops
│   └── Number and layout
├── Activities
│   └── Manage activities
└── KWin Scripts
    └── Tiling scripts, etc.
```

**Enable Desktop Effects:**

```bash
# Via GUI:
System Settings → Workspace Behavior → Desktop Effects

# Popular effects:
- Blur: Blur background behind windows
- Fade: Fade in/out animations
- Magic Lamp: Minimize animation
- Slide: Slide windows when switching workspaces
- Wobbly Windows: Fun jelly effect

# Via command line:
qdbus org.kde.KWin /Effects isEffectLoaded blur
qdbus org.kde.KWin /Effects loadEffect blur
```

#### Shortcuts

```bash
# System Settings → Shortcuts
├── Shortcuts (Global)
│   └── System-wide shortcuts
├── Custom Shortcuts
│   └── Your custom commands
└── KWin
    └── Window management shortcuts

# Important default shortcuts:
Alt+F1          → Application Launcher
Alt+F2          → Run Command (KRunner)
Ctrl+Esc        → System Activity (task manager)
Ctrl+Alt+Esc    → Kill window
Alt+Tab         → Switch windows
Alt+Shift+Tab   → Switch windows (reverse)
Meta/Win+Tab    → Overview effect
```

#### Input Devices

```bash
# System Settings → Input Devices
├── Keyboard
│   ├── Hardware
│   ├── Layouts
│   └── Advanced
├── Mouse
│   ├── General
│   ├── Cursor
│   └── Advanced
└── Touchpad
    ├── Tapping
    ├── Scrolling
    └── Pointer speed
```

**Configure keyboard layouts:**

```bash
# GUI:
System Settings → Input Devices → Keyboard → Layouts
# Add layouts, set switching shortcut

# Command line:
setxkbmap -layout us,ru -option grp:alt_shift_toggle
```

#### Display and Monitor

```bash
# System Settings → Display and Monitor
├── Display Configuration
│   ├── Resolution
│   ├── Refresh rate
│   ├── Position
│   └── Rotation
├── Compositor
│   ├── Rendering backend
│   ├── VSync
│   └── Animation speed
├── Gamma
│   └── Color temperature (night mode)
└── Night Color
    └── Blue light filter
```

**Multiple monitors:**

```bash
# GUI:
System Settings → Display and Monitor → Display Configuration

# Arrange monitors by dragging
# Set primary monitor
# Enable/disable monitors
# Set individual resolution/refresh rate

# Command line (X11):
xrandr --output HDMI-1 --mode 1920x1080 --pos 1920x0 --rotate normal
```

---

## Desktop và Widgets

### Desktop Right-Click Menu

```
Right-click Desktop
├── Add Widgets...
├── Add Panel
│   ├── Empty Panel
│   └── Default Panel
├── Configure Desktop and Wallpaper...
│   ├── Wallpaper
│   ├── Mouse Actions
│   └── Filter
├── Lock Widgets
└── Leave...
    ├── Lock Session
    ├── Switch User
    ├── Suspend
    ├── Hibernate
    ├── Restart
    └── Shut Down
```

### Adding Widgets

```bash
# Method 1: Right-click desktop
Right-click → Add Widgets

# Method 2: Panel toolbox
Right-click panel → Add Widgets

# Method 3: Alt+D, W
Alt+D, W (keyboard shortcut)

# Popular widgets:
- Analog Clock
- Digital Clock
- System Monitor (CPU, RAM, Network)
- Weather
- Notes
- Calculator
- Sticky Notes
- Media Player
- System Tray
```

### Widget Configuration

```bash
# Hover over widget
# Click wrench icon (⚙️) for settings
# Or right-click widget → Configure <Widget Name>

# Common settings:
- Appearance
- Update interval
- Data source
- Size/position
```

### Creating Widget Layout

**Example: Desktop monitoring setup**

1. Right-click desktop → Add Widgets
2. Add "System Monitor Sensor"
3. Configure → Select CPU usage
4. Position it
5. Repeat for RAM, Network
6. Right-click desktop → Lock Widgets

### Removing Widgets

```bash
# Method 1: Right-click widget → Remove
# Method 2: Hover → Click X icon
# Method 3: Unlock widgets → Drag to trash

# Lock/Unlock widgets:
Right-click desktop → Lock Widgets (toggle)
```

---

## Panel Configuration

### Accessing Panel Configuration

```bash
# Right-click panel
├── Edit Panel...
├── Add Widgets...
├── Add Spacer
├── Panel Options
│   ├── Panel Alignment
│   ├── Visibility
│   └── Height
└── Remove Panel
```

### Panel Editing Mode

```bash
# Enter edit mode:
Right-click panel → Edit Panel

# In edit mode:
- Drag panel items to reorder
- Click X on items to remove
- Adjust panel height (drag edges)
- Move panel (drag anywhere)
- Add widgets (click "Add Widgets")
- Add spacers (click "Add Spacer")

# Exit edit mode:
Click anywhere outside panel, or press Escape
```

### Panel Options

```bash
# Panel Alignment:
- Left
- Center
- Right

# Panel Visibility:
- Always visible
- Auto hide
- Windows can cover
- Windows go below

# Panel Height:
- Drag to adjust
- Custom value in pixels

# Floating Panel (Plasma 6):
# Modern floating panel style
Enable: Panel Options → Floating
```

### Multiple Panels

```bash
# Add new panel:
Right-click desktop → Add Panel → Empty Panel

# Common setups:
1. Top panel: System tray, clock
   Bottom panel: App launcher, task manager

2. Single top panel: Everything in one

3. Vertical panel: For ultrawide monitors
   Right-click panel → Panel Options → Screen Edge
```

### Customizing Panel Contents

**Default panel layout:**
```
[App Launcher] [Task Manager] [System Tray] [Clock]
```

**Customized example:**
```
[App Launcher] [Spacer] [Focused Window Title] [Spacer] [System Tray] [Clock]
```

**Another example (top):**
```
[Global Menu] [Spacer] [Clock] [Spacer] [System Tray]
```

### Panel Widget Recommendations

| Widget | Purpose | Position |
|--------|---------|----------|
| Application Menu | Launch apps | Left |
| Application Launcher | Launch apps (tiles) | Left |
| Task Manager | Running apps | Center |
| Global Menu | In-panel menu bar | Top-left |
| Spacer | Push items apart | Between items |
| System Tray | System icons | Right |
| Digital Clock | Time/date | Right |
| Pager | Virtual desktops | Center/Right |
| System Monitor | CPU/RAM/Net | Right |

---

## Keyboard Shortcuts

### Default KDE Shortcuts

#### Application Launching

```
Alt+F1              → Application Launcher
Alt+F2              → Run Command (KRunner)
Alt+Space           → KRunner (alternative)
Meta+E              → Dolphin (file manager)
```

#### Window Management

```
Alt+Tab             → Switch windows
Alt+Shift+Tab       → Switch windows (reverse)
Alt+F3              → Window operations menu
Alt+F4              → Close window
Alt+F7              → Move window
Alt+F8              → Resize window
Alt+F9              → Minimize window
Alt+F10             → Maximize window
Alt+F11             → Fullscreen

# Tiling
Meta+Left           → Tile window left
Meta+Right          → Tile window right
Meta+Up             → Maximize window
Meta+Down           → Restore/minimize window
```

#### Virtual Desktops

```
Ctrl+F1 to F4       → Switch to desktop 1-4
Ctrl+Alt+Left       → Previous desktop
Ctrl+Alt+Right      → Next desktop
Meta+Tab            → Overview (show all desktops)
```

#### System

```
Ctrl+Alt+L          → Lock screen
Ctrl+Alt+Del        → Leave (shutdown dialog)
Ctrl+Esc            → System Monitor
```

### Custom Shortcuts

```bash
# System Settings → Shortcuts → Custom Shortcuts

# Example: Open terminal
1. Right-click "Custom Shortcuts" → New → Global Shortcut → Command/URL
2. Name: "Launch Terminal"
3. Trigger tab: Click "None" → Press desired keys (e.g., Meta+Return)
4. Action tab: Command: konsole

# Example: Screenshot area
Trigger: Print
Command: spectacle -r
```

### KRunner (Alt+F2)

Powerful run dialog:

```bash
# Launch applications
firefox

# Calculator
= 2 + 2

# Unit conversion
100 usd to eur

# Search files
file:myfile

# Web search
gg: kde plasma  # Google search
wp: wayland     # Wikipedia

# System commands
lock            # Lock screen
sleep           # Suspend
logout          # Log out

# Window switching
<window name>   # Switch to window
```

---

## Window Management

### Window Operations

```bash
# Window menu (Alt+F3):
├── Move
├── Resize
├── Minimize
├── Maximize
├── Fullscreen
├── Shade (roll up)
├── Keep Above Others
├── Keep Below Others
├── On All Desktops
├── Move to Desktop
├── Close
└── More Actions
    ├── No Border
    ├── Window Rules
    └── Special Application Settings
```

### Window Tiling

```bash
# Basic tiling (KWin default):
Meta+Left           → Tile left (50%)
Meta+Right          → Tile right (50%)
Meta+Up             → Maximize
Meta+Down           → Restore

# Quarter tiling:
Meta+Left, then Meta+Up    → Top-left quarter
Meta+Left, then Meta+Down  → Bottom-left quarter
```

### KWin Tiling Scripts

For advanced tiling like i3/Sway:

```bash
# Install Bismuth (popular tiling script)
# Method 1: System Settings
System Settings → Window Management → KWin Scripts
→ Get New Scripts → Search "Bismuth" → Install

# Method 2: From GitHub
git clone https://github.com/Bismuth-Forge/bismuth.git
cd bismuth
bash install.sh

# Configure:
System Settings → Window Management → KWin Scripts
→ Check "Bismuth" → Configure

# Bismuth shortcuts:
Meta+T              → Cycle layouts
Meta+Shift+T        → Toggle floating
Meta+[/]            → Resize
Meta+R              → Rotate windows
```

### Virtual Desktops

```bash
# Configure:
System Settings → Workspace Behavior → Virtual Desktops

# Add desktops:
Click "+" to add, up to 20 desktops

# Configure rows:
Set number of rows (for grid layout)

# Navigation:
Ctrl+F1-F4          → Direct access
Ctrl+Alt+Arrow      → Navigate

# Pager widget:
# Add to panel for visual overview
Right-click panel → Add Widgets → Pager
```

### Activities

More than virtual desktops:

```bash
# Different activities for different contexts:
- Work: Work-related apps, files
- Personal: Personal apps, different layout
- Gaming: Full-screen, different settings

# Manage:
System Settings → Workspace Behavior → Activities

# Switch:
Meta+Q              → Show activities (if configured)

# Per-activity settings:
- Different wallpapers
- Different widgets
- Different panels
- Different shortcuts
```

---

## Customization

### Themes (Look and Feel)

```bash
# System Settings → Appearance → Global Theme

# Popular themes:
- Breeze (default, light)
- Breeze Dark (default, dark)
- Arc KDE
- Materia KDE
- Sweet
- Layan

# Install new themes:
Click "Get New Global Themes"
```

### Plasma Styles

```bash
# System Settings → Appearance → Plasma Style

# Affects:
- Panel appearance
- Widget appearance
- Tooltip style

# Independent from Global Theme
```

### Window Decorations

```bash
# System Settings → Application Style → Window Decorations

# Styles:
- Breeze
- Oxygen
- SierraBreezeEnhanced (popular)

# Customization:
- Button order (minimize, maximize, close)
- Titlebar buttons
- Titlebar colors
```

### Colors

```bash
# System Settings → Appearance → Colors

# Color schemes:
- Breeze Light
- Breeze Dark
- Breeze High Contrast
- Custom...

# Create custom scheme:
1. Start from existing scheme
2. Modify colors
3. Save with new name
```

### Icons

```bash
# System Settings → Appearance → Icons

# Popular icon themes:
- Breeze (default)
- Papirus
- Tela
- Numix

# Install:
System Settings → Icons → Get New Icons
```

### Fonts

```bash
# System Settings → Appearance → Fonts

# Font roles:
- General: UI text
- Fixed width: Terminal, code editors
- Small: Small text
- Toolbar
- Menu
- Window title

# Recommended fonts:
- General: Noto Sans, Inter, Roboto
- Fixed width: JetBrains Mono, Fira Code, Cascadia Code
```

### Cursors

```bash
# System Settings → Appearance → Cursors

# Popular themes:
- Breeze
- Breeze Light
- Bibata
- Capitaine

# Size: Adjust for HiDPI displays
```

### Splash Screen

```bash
# System Settings → Appearance → Splash Screen

# Shown during login
# Can install custom splash screens
```

### Wallpaper

```bash
# Right-click desktop → Configure Desktop and Wallpaper

# Types:
- Image
- Slideshow (rotate images)
- Plain Color
- Pattern
- Get New Wallpapers (online)

# Wallpaper settings:
- Positioning: Scaled, Scaled and Cropped, Centered, Tiled
- Background color
- Blur (for slideshow)

# Per-monitor wallpapers:
# Configure each monitor separately
```

### Desktop Effects

```bash
# System Settings → Workspace Behavior → Desktop Effects

# Categories:
- Accessibility (Zoom, magnifier)
- Appearance (Blur, background contrast)
- Focus (Dim inactive)
- Show Desktop Animation
- Virtual Desktop Switching Animation
- Window Management (Present windows, desktop grid)
- Window Open/Close Animation

# Performance:
- Animation speed: System Settings → Workspace Behavior → General Behavior
```

---

## Advanced Features

### KDE Connect

Connect phone to desktop:

```bash
# Install:
sudo apt install kdeconnect

# On phone:
# Install "KDE Connect" from Play Store/F-Droid

# Pair:
# Both devices on same WiFi
# Open KDE Connect on both
# Pair from one device

# Features:
- Notifications sync
- Clipboard sync
- File transfer
- Remote input (phone as touchpad/keyboard)
- Media controls
- SMS from desktop
- Run commands
```

### Klipper (Clipboard Manager)

```bash
# System Tray → Clipboard icon

# Features:
- Clipboard history
- Search history
- Barcode scanning
- Actions (custom actions on clipboard content)

# Shortcuts:
Meta+Ctrl+V         → Show clipboard history

# Configure:
Right-click Clipboard icon → Configure Clipboard
```

### KRunner Plugins

Extend KRunner functionality:

```bash
# System Settings → Search → KRunner

# Enable/disable plugins:
- Applications
- Bookmarks
- Calculator
- Dictionary
- Files
- Unit Converter
- Web Search
- Windows

# Custom web shortcuts:
System Settings → Search → Web Search Shortcuts
# Add custom: e.g., "gh" for GitHub search
```

### File Indexing (Baloo)

```bash
# Enable:
System Settings → Search → File Search
# Check "Enable File Search"

# Configure:
- Folders to index
- Folders to exclude
- Index file content

# Use:
# KRunner will search file contents
# Dolphin has instant search

# Disable if causing performance issues:
balooctl disable
```

### System Monitor Widgets

```bash
# Add System Monitor widget:
Right-click desktop/panel → Add Widgets → System Monitor

# Sensors:
- CPU usage
- CPU frequency
- RAM usage
- Swap usage
- Network speed
- Disk usage
- GPU usage (if supported)

# Appearance:
- Line Chart
- Bar Chart
- Pie Chart
- Text only
```

### Global Menu

macOS-style menu in panel:

```bash
# Add to panel:
Right-click panel → Add Widgets → Global Menu (Plasma 6) / Application Menu Bar

# Configure applications:
System Settings → Application Style → Window Decorations
→ Titlebar Buttons → Check "Show window title on menubar"

# Note: Requires app support (mostly works with Qt apps)
```

### Window Rules

Automate window behavior:

```bash
# System Settings → Window Management → Window Rules

# Create rule:
1. Click "Add New..."
2. Detect Window Properties → Click target window
3. Configure rules:
   - Size & Position
   - Arrangement & Access
   - Appearance & Fixes
   - Workarounds

# Examples:
- Always open Firefox on desktop 2
- Telegram always floating
- Steam starts minimized
- Picture-in-picture always on top
```

---

## Troubleshooting Common Issues

### Plasma crashed

```bash
# Restart plasmashell:
killall plasmashell && kstart5 plasmashell

# From TTY (Ctrl+Alt+F3):
export DISPLAY=:0
killall plasmashell
kstart5 plasmashell &
```

### Corrupted configuration

```bash
# Backup current config:
cd ~/.config
tar czf plasma-config-backup-$(date +%Y%m%d).tar.gz plasma* kde*

# Reset Plasma:
mv ~/.config/plasma-org.kde.plasma.desktop-appletsrc{,.bak}
mv ~/.config/plasmarc{,.bak}

# Restart:
killall plasmashell && kstart5 plasmashell
```

### Performance issues

```bash
# Disable compositing:
System Settings → Display and Monitor → Compositor
→ Uncheck "Enable compositor on startup"

# Reduce effects:
System Settings → Workspace Behavior → Desktop Effects
→ Disable heavy effects (blur, etc.)

# Disable file indexing:
balooctl disable

# Check for resource hogs:
plasma-systemmonitor
```

---

## Kết Luận

KDE Plasma offers:
- **Full-featured**: Complete desktop environment
- **Customizable**: Endless customization options
- **User-friendly**: GUI for everything
- **Modern**: Beautiful, up-to-date

### Quick Start Guide

1. **Day 1**: Explore default layout, try applications
2. **Week 1**: Customize theme, wallpaper, panels
3. **Month 1**: Learn shortcuts, add widgets, configure shortcuts
4. **Month 3**: Advanced: Window rules, KWin scripts, activities

### Next Steps

1. Explore System Settings gradually
2. Try different themes and layouts
3. Install useful widgets
4. Learn keyboard shortcuts
5. Join KDE community

### Tài Nguyên

- [KDE UserBase](https://userbase.kde.org/)
- [KDE Forums](https://forum.kde.org/)
- [r/kde](https://reddit.com/r/kde)
- [KDE Documentation](https://docs.kde.org/)
