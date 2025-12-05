# Hiểu Sâu Về KDE Plasma

## Mục Lục
1. [KDE Plasma là gì?](#kde-plasma-là-gì)
2. [Kiến trúc của KDE Plasma](#kiến-trúc-của-kde-plasma)
3. [KDE Frameworks](#kde-frameworks)
4. [KWin - Window Manager của Plasma](#kwin---window-manager-của-plasma)
5. [Plasma Desktop vs Plasma Workspaces](#plasma-desktop-vs-plasma-workspaces)
6. [Wayland Support trong Plasma](#wayland-support-trong-plasma)

---

## KDE Plasma là gì?

### Định nghĩa

**KDE Plasma** là một desktop environment (môi trường desktop) hoàn chỉnh, hiện đại và có thể tùy chỉnh cao cho Linux và Unix-like systems.

### Lịch sử và Evolution

```
KDE Timeline:
1996 - KDE 1.0 (K Desktop Environment)
1998 - KDE 1.0 released
2000 - KDE 2.0 (Qt 2, DCOP)
2002 - KDE 3.0 (peak of KDE 3 series)
2008 - KDE 4.0 (Qt 4, complete rewrite, Plasma introduced)
2014 - KDE Plasma 5 (Qt 5, modularization)
       ├─ KDE Frameworks 5 (libraries)
       ├─ KDE Plasma 5 (desktop)
       └─ KDE Applications (apps)
2024 - KDE Plasma 6 (Qt 6, Wayland first)
```

### KDE Project Structure

```
┌─────────────────────────────────────────┐
│           KDE Project                   │
├─────────────────────────────────────────┤
│                                         │
│  ┌───────────────────────────────────┐  │
│  │    KDE Frameworks (KF6)           │  │
│  │    - 80+ libraries                │  │
│  │    - Core functionality           │  │
│  └───────────────────────────────────┘  │
│                 │                       │
│                 │ used by               │
│                 ▼                       │
│  ┌───────────────────────────────────┐  │
│  │    KDE Plasma                     │  │
│  │    - Desktop environment          │  │
│  │    - Widgets, panels, themes      │  │
│  └───────────────────────────────────┘  │
│                 │                       │
│                 │ provides platform     │
│                 ▼                       │
│  ┌───────────────────────────────────┐  │
│  │    KDE Applications               │  │
│  │    - Dolphin, Kate, Konsole,      │  │
│  │      Okular, Gwenview, etc.       │  │
│  └───────────────────────────────────┘  │
│                                         │
└─────────────────────────────────────────┘
```

---

## Kiến Trúc Của KDE Plasma

### Architecture Overview

```
┌──────────────────────────────────────────────────────────────┐
│                  KDE PLASMA ARCHITECTURE                     │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌────────────────────────────────────────────────────────┐  │
│  │              Plasma Shell (plasmashell)                │  │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────┐  │  │
│  │  │   Desktop    │  │    Panel     │  │   Widgets   │  │  │
│  │  │  (Containment)│ │ (Containment)│ │  (Applets)  │  │  │
│  │  └──────────────┘  └──────────────┘  └─────────────┘  │  │
│  │                                                        │  │
│  │  ┌──────────────────────────────────────────────────┐  │  │
│  │  │         Plasma Framework (LibPlasma)             │  │  │
│  │  │  - Corona, Containment, Applet system            │  │  │
│  │  └──────────────────────────────────────────────────┘  │  │
│  └────────────────────────────────────────────────────────┘  │
│                            │                                 │
│  ┌────────────────────────────────────────────────────────┐  │
│  │              KWin (Window Manager)                     │  │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────┐  │  │
│  │  │  Compositing │  │    Window    │  │   Effects   │  │  │
│  │  │   (OpenGL)   │  │  Management  │  │   System    │  │  │
│  │  └──────────────┘  └──────────────┘  └─────────────┘  │  │
│  │                                                        │  │
│  │  ┌──────────────────────────────────────────────────┐  │  │
│  │  │    X11 / Wayland Compositor Support              │  │  │
│  │  └──────────────────────────────────────────────────┘  │  │
│  └────────────────────────────────────────────────────────┘  │
│                            │                                 │
│  ┌────────────────────────────────────────────────────────┐  │
│  │         KDE Frameworks (KF6) - 80+ libraries          │  │
│  │  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐        │  │
│  │  │KIO   │ │KConfig│ │KCoreAdd│ │Solid│ │KNotify│  ...  │  │
│  │  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘        │  │
│  └────────────────────────────────────────────────────────┘  │
│                            │                                 │
│  ┌────────────────────────────────────────────────────────┐  │
│  │                   Qt Framework                         │  │
│  │  - GUI, Network, DBus, QML, etc.                       │  │
│  └────────────────────────────────────────────────────────┘  │
│                            │                                 │
│  ┌────────────────────────────────────────────────────────┐  │
│  │        System Layer (X11/Wayland, systemd, etc.)       │  │
│  └────────────────────────────────────────────────────────┘  │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### Core Components Chi Tiết

#### 1. Plasma Shell (plasmashell)

**Plasma Shell** là process chính chạy desktop environment:

```
plasmashell responsibilities:
├── Desktop rendering
│   ├── Wallpaper
│   ├── Desktop icons (if enabled)
│   └── Desktop widgets
│
├── Panel management
│   ├── Application launcher
│   ├── Task manager
│   ├── System tray
│   └── Clock
│
├── Widget system
│   ├── Load widgets (applets)
│   ├── Manage widget lifecycle
│   └── Handle widget configuration
│
└── Communication
    ├── D-Bus for IPC
    ├── KWin for window management
    └── System services
```

**Process hierarchy:**
```bash
systemd --user
  └── plasma-plasmashell.service
       ├── plasmashell (main GUI)
       ├── Multiple QML engines (for widgets)
       └── Plugin processes
```

#### 2. KWin (Window Manager & Compositor)

**KWin** là window manager và compositor cho Plasma:

```
KWin Architecture:
┌─────────────────────────────────────┐
│           KWin Process              │
├─────────────────────────────────────┤
│                                     │
│  Window Management                  │
│  ┌──────────────────────────────┐   │
│  │ - Window placement           │   │
│  │ - Focus handling             │   │
│  │ - Virtual desktops           │   │
│  │ - Window rules               │   │
│  └──────────────────────────────┘   │
│                                     │
│  Compositing                        │
│  ┌──────────────────────────────┐   │
│  │ - OpenGL rendering           │   │
│  │ - Window textures            │   │
│  │ - Scene graph                │   │
│  │ - VSync handling             │   │
│  └──────────────────────────────┘   │
│                                     │
│  Effects System                     │
│  ┌──────────────────────────────┐   │
│  │ - Desktop effects            │   │
│  │ - Animations                 │   │
│  │ - Blur, shadow, etc.         │   │
│  │ - JavaScript plugins         │   │
│  └──────────────────────────────┘   │
│                                     │
│  Backend (X11 or Wayland)           │
│  ┌──────────────────────────────┐   │
│  │ - Protocol implementation    │   │
│  │ - Input handling             │   │
│  │ - Output management          │   │
│  └──────────────────────────────┘   │
│                                     │
└─────────────────────────────────────┘
```

**KWin as Wayland Compositor:**
```
KWin_wayland acts as:
1. Display server (like Xorg role)
2. Window manager (traditional WM role)
3. Compositor (rendering and effects)

KWin creates Wayland socket:
/run/user/1000/wayland-0
```

#### 3. System Settings (systemsettings)

Central configuration hub:

```
systemsettings
├── KCModule plugins (each setting page)
│   ├── kcm_kwin (Window management)
│   ├── kcm_kscreen (Display configuration)
│   ├── kcm_keyboard (Keyboard settings)
│   └── 50+ other modules
│
└── Configuration storage
    ├── ~/.config/kdeglobals (global KDE settings)
    ├── ~/.config/kwinrc (KWin settings)
    ├── ~/.config/plasmarc (Plasma settings)
    └── Many other config files
```

---

## KDE Frameworks

### Giới thiệu KDE Frameworks

**KDE Frameworks (KF6)** là tập hợp 80+ libraries cung cấp functionality cho KDE applications.

### Phân loại Frameworks

```
KDE Frameworks Categories:
┌─────────────────────────────────────────────────┐
│                                                 │
│  Tier 1: Functional                             │
│  - No dependencies on other KF6 libs            │
│  - Examples: KArchive, KCodecs, KCoreAddons     │
│                                                 │
│  Tier 2: Integration                            │
│  - Depend on Tier 1 and Qt                      │
│  - Examples: KAuth, KCompletion, KCrash         │
│                                                 │
│  Tier 3: Solutions                              │
│  - Can depend on Tier 1 and 2                   │
│  - Examples: KIO, KParts, KTextEditor           │
│                                                 │
│  Tier 4: Porting Aids                           │
│  - Deprecated, for porting from KDE4            │
│                                                 │
└─────────────────────────────────────────────────┘
```

### Frameworks Quan Trọng

#### 1. KIO (KDE Input/Output)

Network transparent I/O framework:

```cpp
// Example: Copy file with KIO
#include <KIO/CopyJob>
#include <KIO/Job>

// Works with local files
KIO::copy(QUrl("file:///home/user/file.txt"),
          QUrl("file:///home/user/backup/file.txt"));

// Also works with remote files!
KIO::copy(QUrl("file:///home/user/file.txt"),
          QUrl("ftp://server.com/file.txt"));

// Or even specialized protocols
KIO::copy(QUrl("file:///home/user/file.txt"),
          QUrl("smb://server/share/file.txt"));
```

**KIO Slaves (Workers):**
```
KIO architecture:
Application
    │
    ▼
KIO Framework
    │
    ├─── file:// → Local filesystem
    ├─── http:// → HTTP protocol
    ├─── ftp:// → FTP protocol
    ├─── smb:// → Samba shares
    ├─── fish:// → SSH file transfer
    ├─── sftp:// → SFTP protocol
    ├─── trash:// → Trash can
    ├─── man:// → Man pages
    └─── 50+ other protocols
```

#### 2. Solid

Hardware discovery and management:

```cpp
#include <Solid/Device>
#include <Solid/StorageAccess>

// List all storage devices
QList<Solid::Device> devices = 
    Solid::Device::listFromType(Solid::DeviceInterface::StorageAccess);

for (const Solid::Device &device : devices) {
    qDebug() << "Found device:" << device.description();
    
    auto storage = device.as<Solid::StorageAccess>();
    if (storage && storage->isAccessible()) {
        qDebug() << "  Mounted at:" << storage->filePath();
    }
}
```

#### 3. KConfig

Configuration management:

```cpp
#include <KConfig>
#include <KConfigGroup>

// Read configuration
KConfig config("myapprc");
KConfigGroup group = config.group("General");
QString username = group.readEntry("Username", "default");
int fontSize = group.readEntry("FontSize", 12);

// Write configuration
group.writeEntry("Username", "john");
group.writeEntry("FontSize", 14);
config.sync();  // Save to disk
```

**Config file structure:**
```ini
# ~/.config/myapprc
[General]
Username=john
FontSize=14

[Colors]
Background=#ffffff
Foreground=#000000
```

#### 4. KNotifications

System notifications:

```cpp
#include <KNotification>

KNotification *notification = new KNotification("downloadComplete");
notification->setTitle("Download Finished");
notification->setText("Your file has been downloaded successfully");
notification->setPixmap(QPixmap(":/icons/download.png"));
notification->sendEvent();
```

---

## KWin - Window Manager của Plasma

### KWin Deep Dive

**KWin** là một trong những window managers phức tạp và feature-rich nhất.

### KWin Rendering Pipeline

```
┌────────────────────────────────────────────────┐
│         KWin Rendering Pipeline                │
├────────────────────────────────────────────────┤
│                                                │
│  1. Input Events                               │
│     │                                          │
│     ▼                                          │
│  2. Window State Updates                       │
│     ├─ Position changes                        │
│     ├─ Size changes                            │
│     ├─ Focus changes                           │
│     └─ Visibility changes                      │
│     │                                          │
│     ▼                                          │
│  3. Scene Graph Update                         │
│     ├─ Build/update scene graph                │
│     ├─ Apply effects                           │
│     └─ Determine what needs repainting         │
│     │                                          │
│     ▼                                          │
│  4. OpenGL Rendering                           │
│     ├─ Setup OpenGL context                    │
│     ├─ Render window textures                  │
│     ├─ Apply shaders (blur, etc.)              │
│     ├─ Composite all layers                    │
│     └─ Apply effects (fade, slide, etc.)       │
│     │                                          │
│     ▼                                          │
│  5. Buffer Swap                                │
│     ├─ VSync wait (if enabled)                 │
│     └─ Present to screen                       │
│                                                │
└────────────────────────────────────────────────┘
```

### KWin Effects System

KWin có powerful effects system:

```javascript
// Example: Simple KWin effect in JavaScript
// ~/.local/share/kwin/effects/myeffect/contents/code/main.js

effects.windowAdded.connect(function(window) {
    // Animate new windows
    animate({
        window: window,
        duration: 300,
        type: Effect.Opacity,
        from: 0.0,
        to: 1.0
    });
    
    animate({
        window: window,
        duration: 300,
        type: Effect.Scale,
        from: 0.8,
        to: 1.0
    });
});

effects.windowClosed.connect(function(window) {
    // Animate closing windows
    animate({
        window: window,
        duration: 200,
        type: Effect.Opacity,
        to: 0.0
    });
});
```

**Built-in effects:**
```
KWin Effects Categories:
├── Accessibility
│   └── Zoom, Magnifier
│
├── Appearance
│   ├── Blur
│   ├── Background contrast
│   └── Dim inactive
│
├── Focus
│   └── Dim inactive windows
│
├── Window Management
│   ├── Desktop grid
│   ├── Present windows
│   └── Slide
│
└── Window Open/Close
    ├── Fade
    ├── Glide
    ├── Scale
    └── Magic lamp
```

### KWin Scripts

KWin supports powerful scripting:

```javascript
// Example: Tile windows script
// ~/.local/share/kwin/scripts/tiling/contents/code/main.js

workspace.clientAdded.connect(function(client) {
    if (client.normalWindow && !client.dialog) {
        // Tile the window
        var screen = workspace.activeScreen;
        var area = workspace.clientArea(
            KWin.PlacementArea, 
            screen, 
            workspace.currentDesktop
        );
        
        // Place window in left half
        client.geometry = {
            x: area.x,
            y: area.y,
            width: area.width / 2,
            height: area.height
        };
    }
});
```

---

## Plasma Desktop vs Plasma Workspaces

### Plasma Desktop

**Plasma Desktop** là traditional desktop layout:

```
┌──────────────────────────────────────────────────┐
│  Plasma Desktop                                  │ ← Panel (top)
├──────────────────────────────────────────────────┤
│                                                  │
│                                                  │
│           Desktop Area                           │
│           - Wallpaper                            │
│           - Desktop widgets (plasmoids)          │
│           - Desktop icons (optional)             │
│                                                  │
│                                                  │
│                                                  │
├──────────────────────────────────────────────────┤
│ [Menu] [Apps] [Files]            [Tray] [Clock] │ ← Panel (bottom)
└──────────────────────────────────────────────────┘
```

### Other Plasma Workspaces

#### Plasma Mobile
```
For mobile devices:
├── Touch-optimized UI
├── Virtual keyboard
├── Convergence with desktop
└── Works on phones (PinePhone, etc.)
```

#### Plasma Bigscreen
```
For TV/media center:
├── 10-foot UI
├── Remote control support
├── Voice control
└── Media focused
```

---

## Wayland Support trong Plasma

### KWin_wayland

**KWin** has excellent Wayland support since Plasma 5.5 (2015):

```
KWin Wayland Session:
┌─────────────────────────────────────────┐
│  kwin_wayland (compositor + WM)         │
│                                         │
│  Creates: wayland-0 socket              │
│  Sets: WAYLAND_DISPLAY=wayland-0        │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  Native Wayland Clients           │  │
│  │  - GTK4 apps                      │  │
│  │  - Qt6 apps                       │  │
│  │  - Native Wayland apps            │  │
│  └───────────────────────────────────┘  │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  XWayland (for X11 apps)          │  │
│  │  - Legacy X11 applications        │  │
│  │  - Seamlessly integrated          │  │
│  └───────────────────────────────────┘  │
│                                         │
└─────────────────────────────────────────┘
```

### Wayland Protocols Supported

```
KWin implements many Wayland protocols:
├── Core protocols
│   ├── wl_compositor
│   ├── wl_seat (input)
│   └── wl_output
│
├── XDG protocols
│   ├── xdg_wm_base (window management)
│   ├── xdg_decoration (server-side decorations)
│   └── xdg_output
│
├── KDE protocols
│   ├── org_kde_plasma_shell (Plasma integration)
│   ├── org_kde_kwin_blur (blur effect)
│   └── org_kde_kwin_server_decoration
│
└── Other protocols
    ├── zwp_linux_dmabuf (zero-copy buffers)
    ├── wp_presentation_time (timing)
    └── 20+ more protocols
```

### Starting Plasma Wayland Session

```bash
# Method 1: From login screen
# Select "Plasma (Wayland)" session

# Method 2: From terminal
dbus-run-session startplasma-wayland

# Method 3: From systemd
systemctl --user start plasma-plasmashell.service
```

### X11 vs Wayland Comparison in Plasma

```
Feature Comparison:
┌─────────────────────┬──────────────┬──────────────┐
│ Feature             │ X11          │ Wayland      │
├─────────────────────┼──────────────┼──────────────┤
│ Maturity            │ ✓✓✓          │ ✓✓           │
│ Performance         │ ✓✓           │ ✓✓✓          │
│ Security            │ ✓            │ ✓✓✓          │
│ HDR support         │ ✗            │ ✓ (Plasma 6) │
│ VRR (FreeSync)      │ Limited      │ ✓✓           │
│ Fractional scaling  │ Problematic  │ ✓✓           │
│ Screen recording    │ ✓✓✓          │ ✓✓ (portal)  │
│ Remote desktop      │ ✓✓✓          │ ✓✓ (portal)  │
│ Legacy app support  │ ✓✓✓          │ ✓✓ (XWayland)│
│ Multi-monitor       │ ✓✓           │ ✓✓✓          │
└─────────────────────┴──────────────┴──────────────┘
```

---

## Plasma Configuration System

### Configuration Files

Plasma configuration stored in `~/.config/`:

```
~/.config/
├── kdeglobals                    # Global KDE settings
├── kwinrc                        # KWin configuration
├── plasmarc                      # Plasma shell settings
├── kscreenlockerrc              # Screen locker
├── kglobalshortcutsrc           # Global shortcuts
├── plasma-org.kde.plasma.desktop-appletsrc  # Desktop layout
└── many more...
```

### Example Configuration

```ini
# ~/.config/kwinrc
[Compositing]
Backend=OpenGL
Enabled=true
GLCore=true
GLPlatformInterface=glx

[Desktops]
Number=4
Rows=1

[Effect-PresentWindows]
BorderActivateAll=9

[Windows]
BorderlessMaximizedWindows=true
FocusPolicy=FocusFollowsMouse
```

### Plasma Look and Feel Packages

```
Look and Feel Package Structure:
~/.local/share/plasma/look-and-feel/
└── my-theme/
    ├── metadata.desktop          # Theme metadata
    └── contents/
        ├── splash/               # Splash screen
        ├── defaults              # Default settings
        ├── layouts/              # Plasma layouts
        ├── previews/            # Theme preview images
        └── components/          # QML components
```

---

## Plasma Widgets (Plasmoids)

### Widget Architecture

```
Plasmoid Structure:
~/.local/share/plasma/plasmoids/
└── org.kde.mywidget/
    ├── metadata.desktop          # Widget metadata
    └── contents/
        ├── ui/
        │   ├── main.qml          # Main UI (QML)
        │   └── config.qml        # Configuration UI
        ├── code/
        │   └── main.js           # Logic (JavaScript)
        └── images/               # Resources
```

### Example Plasmoid

```qml
// main.qml - Simple clock widget
import QtQuick 2.15
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    width: 200
    height: 100
    
    PlasmaComponents.Label {
        id: timeLabel
        anchors.centerIn: parent
        font.pixelSize: 32
        text: Qt.formatTime(new Date(), "hh:mm:ss")
    }
    
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            timeLabel.text = Qt.formatTime(new Date(), "hh:mm:ss")
        }
    }
}
```

```desktop
# metadata.desktop
[Desktop Entry]
Name=My Clock Widget
Comment=A simple clock widget
Type=Service
Icon=preferences-system-time

X-KDE-ServiceTypes=Plasma/Applet
X-KDE-PluginInfo-Name=org.kde.myclock
X-KDE-PluginInfo-Category=Date and Time
X-KDE-PluginInfo-Version=1.0
X-KDE-PluginInfo-License=GPL
```

---

## Kết Luận

### Điểm Mạnh của Plasma

1. **Customization**: Tùy chỉnh gần như mọi thứ
2. **Performance**: Hiệu năng tốt hơn nhiều so với Plasma 4
3. **Features**: Feature-rich nhất trong các DE
4. **Wayland**: Excellent Wayland support
5. **Integration**: Tight integration với KDE apps

### Điểm Yếu

1. **Complexity**: Có thể overwhelming cho người mới
2. **Bugs**: Đôi khi có bugs do tính năng quá nhiều
3. **Resource usage**: Nặng hơn lightweight WM như Sway
4. **Learning curve**: Nhiều options có thể confusing

### Khi Nào Nên Dùng Plasma?

✅ Dùng Plasma nếu:
- Bạn muốn full-featured desktop environment
- Bạn cần GUI configuration tools
- Bạn thích giao diện đẹp, hiện đại
- Bạn muốn ecosystem hoàn chỉnh (apps, themes, etc.)

❌ Không dùng Plasma nếu:
- Bạn cần system cực kỳ nhẹ
- Bạn thích minimal, keyboard-driven workflow
- Hardware của bạn quá yếu
- Bạn thích full control qua text configs

### Next Steps

1. Đọc [04-install-plasma.md](./04-install-plasma.md) để cài đặt
2. Đọc [06-operate-plasma.md](./06-operate-plasma.md) để học vận hành
3. Khám phá [KDE Documentation](https://docs.kde.org/)
4. Tham gia [KDE Community](https://community.kde.org/)
