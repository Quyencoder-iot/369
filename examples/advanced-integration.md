# Tích Hợp Nâng Cao: Sway Nested với Plasma

## Tổng Quan

Tài liệu này hướng dẫn cách tích hợp Sway nested vào Plasma một cách mượt mà, tạo ra một môi trường hybrid mạnh mẽ.

## Use Cases

1. **Development Environment**: Chạy Sway nested trong Plasma để test config
2. **Hybrid Workflow**: Sử dụng cả hai môi trường song song
3. **Learning Tool**: Học Sway mà không rời khỏi Plasma
4. **Testing**: Test ứng dụng trên cả hai compositor

## Cấu Hình Nâng Cao

### 1. Tạo Desktop Entry cho Sway Nested

Tạo file `~/.local/share/applications/sway-nested.desktop`:

```ini
[Desktop Entry]
Name=Sway Nested
Comment=Run Sway in nested mode
Exec=/path/to/sway-nested.sh
Icon=sway
Terminal=false
Type=Application
Categories=System;WindowManager;
Keywords=sway;wayland;tiling;
```

### 2. Plasma Service Menu

Tạo file `~/.local/share/kservices5/sway-nested.desktop`:

```ini
[Desktop Entry]
Type=Service
ServiceTypes=KonqPopupMenu/Plugin
Actions=RunSwayNested;
MimeType=application/x-executable;

[Desktop Action RunSwayNested]
Name=Run Sway Nested
Exec=/path/to/sway-nested.sh
Icon=sway
```

### 3. KWin Script để Toggle Sway Nested

Tạo script KWin để tự động quản lý Sway nested window:

```javascript
// ~/.local/share/kwin/scripts/sway-nested-toggle/main.js

registerShortcut("ToggleSwayNested", "Toggle Sway Nested", "Meta+S", function() {
    var clients = workspace.clientList();
    var swayClient = null;
    
    for (var i = 0; i < clients.length; i++) {
        if (clients[i].caption.indexOf("Sway") !== -1) {
            swayClient = clients[i];
            break;
        }
    }
    
    if (swayClient) {
        if (swayClient.minimized) {
            workspace.sendClientToScreen(swayClient, workspace.currentScreen);
            swayClient.minimized = false;
        } else {
            swayClient.minimized = true;
        }
    } else {
        callDBus("org.kde.klauncher5", "/KLauncher", 
                 "org.kde.KLauncher.exec_blind", 
                 "sway-nested", []);
    }
});
```

### 4. Script Khởi Động Thông Minh

Script cải tiến để tích hợp tốt hơn với Plasma:

```bash
#!/bin/bash
# ~/.local/bin/sway-nested-smart.sh

# Kiểm tra xem Sway nested đã chạy chưa
if pgrep -f "sway.*headless" > /dev/null; then
    echo "Sway nested is already running!"
    # Focus vào cửa sổ Sway
    qdbus org.kde.KWin /KWin org.kde.KWin.activateWindow $(xdotool search --class "sway" | head -1)
    exit 0
fi

# Set environment
export WLR_BACKENDS=headless
export WLR_LIBINPUT_NO_DEVICES=1
export WLR_HEADLESS_OUTPUTS=1

# Tạo window title riêng để dễ nhận biết
export SWAY_NESTED_TITLE="Sway Nested - $(date +%H:%M:%S)"

# Chạy Sway với window class đặc biệt
sway -c ~/.config/sway/config-nested &
SWAY_PID=$!

# Đợi một chút để Sway khởi động
sleep 2

# Set window properties trong KWin
if command -v xdotool &> /dev/null; then
    WINDOW_ID=$(xdotool search --pid $SWAY_PID --class "sway" | head -1)
    if [ ! -z "$WINDOW_ID" ]; then
        xdotool set_window --name "$SWAY_NESTED_TITLE" $WINDOW_ID
        xdotool set_window --classname "SwayNested" $WINDOW_ID
    fi
fi

# Wait for Sway to exit
wait $SWAY_PID
```

### 5. Plasma Widget để Monitor Sway

Tạo một Plasma widget đơn giản để hiển thị trạng thái Sway:

```qml
// ~/.local/share/plasma/plasmoids/com.github.swaymonitor/contents/ui/main.qml

import QtQuick 2.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

PlasmoidItem {
    id: root
    
    Plasmoid.title: "Sway Monitor"
    Plasmoid.icon: "sway"
    
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            var result = Qt.createQmlObject('import QtQuick 2.0; QtObject {}', root);
            var process = Qt.createQmlObject('
                import QtQuick 2.0;
                Process {
                    command: ["pgrep", "-f", "sway.*headless"]
                    onFinished: {
                        root.swayRunning = exitCode === 0;
                    }
                }
            ', root);
            process.start();
        }
    }
    
    property bool swayRunning: false
    
    fullRepresentation: Item {
        PlasmaCore.IconItem {
            source: root.swayRunning ? "sway" : "sway-gray"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Toggle Sway nested
                Qt.createQmlObject('
                    import QtQuick 2.0;
                    Process {
                        command: ["/path/to/sway-nested.sh"]
                    }
                ', root).start();
            }
        }
    }
}
```

## Workflow Tích Hợp

### Scenario 1: Development Workflow

```bash
# 1. Khởi động Plasma
# 2. Mở terminal trong Plasma
# 3. Chạy Sway nested
sway-nested-smart.sh

# 4. Làm việc trong Sway nested
# 5. Test config changes
# 6. Khi xong, đóng Sway nested (Mod+Shift+E)
# 7. Quay lại Plasma để tiếp tục công việc khác
```

### Scenario 2: Hybrid Desktop

1. **Plasma làm desktop chính**: Email, browser, file manager
2. **Sway nested cho coding**: Terminal, editor, development tools
3. **Chuyển đổi dễ dàng**: Alt+Tab giữa hai môi trường

### Scenario 3: Testing Environment

```bash
# Test ứng dụng trên cả hai compositor
# 1. Chạy trong Plasma (native)
app-to-test &

# 2. Chạy trong Sway nested
WLR_BACKENDS=headless sway
# Trong Sway: app-to-test &
```

## Scripts Tiện Ích

### Sync Clipboard giữa Sway và Plasma

```bash
#!/bin/bash
# ~/.local/bin/clipboard-sync.sh

# Monitor clipboard changes và sync
while true; do
    # Plasma -> Sway
    PLASMA_CLIP=$(qdbus org.kde.klipper /klipper org.kde.klipper.klipper.getClipboardContents)
    if [ ! -z "$PLASMA_CLIP" ]; then
        echo -n "$PLASMA_CLIP" | wl-copy
    fi
    
    # Sway -> Plasma (nếu có wl-paste)
    if command -v wl-paste &> /dev/null; then
        SWAY_CLIP=$(wl-paste -n)
        if [ ! -z "$SWAY_CLIP" ]; then
            qdbus org.kde.klipper /klipper org.kde.klipper.klipper.setClipboardContents "$SWAY_CLIP"
        fi
    fi
    
    sleep 1
done
```

### Share Workspace State

```bash
#!/bin/bash
# ~/.local/bin/sway-workspace-notify.sh

# Monitor Sway workspace changes và notify Plasma
swaymsg -m -t SUBSCRIBE '["workspace"]' | while read line; do
    WORKSPACE=$(echo $line | jq -r '.current.name // empty')
    if [ ! -z "$WORKSPACE" ]; then
        # Gửi notification qua Plasma
        notify-send "Sway Workspace" "Switched to: $WORKSPACE"
        
        # Hoặc update Plasma widget
        qdbus org.kde.plasmashell /PlasmaShell \
              org.kde.PlasmaShell.evaluateScript \
              "console.log('Sway workspace: $WORKSPACE')"
    fi
done
```

## Performance Optimization

### Giảm Overhead của Nested Mode

1. **Giảm Resolution**:
```bash
export WLR_HEADLESS_OUTPUT_WIDTH=1280
export WLR_HEADLESS_OUTPUT_HEIGHT=720
```

2. **Disable Compositing Effects trong Plasma**:
- System Settings → Hardware → Display and Monitor → Compositor
- Set "Rendering backend" to "XRender" (nếu có thể)
- Disable animations

3. **Tối Ưu Sway Config**:
```bash
# Trong ~/.config/sway/config-nested
# Disable một số tính năng không cần thiết
# Giảm gaps, disable blur effects, etc.
```

## Troubleshooting Tích Hợp

### Vấn đề: Sway nested window không focus được

**Giải pháp**:
```bash
# Trong KWin, set window rules
# System Settings → Window Management → Window Rules
# Add rule: Window class = "SwayNested"
# Force: Accept focus = Yes
```

### Vấn đề: Input lag trong nested mode

**Giải pháp**:
- Giảm resolution của nested output
- Tắt compositing effects
- Sử dụng GPU acceleration nếu có thể

### Vấn đề: Clipboard không sync

**Giải pháp**:
- Sử dụng script clipboard-sync.sh ở trên
- Hoặc cài đặt `wl-clipboard` và `xclip` để bridge

## Best Practices

1. **Sử dụng separate config cho nested**:
   - `~/.config/sway/config` - Standalone
   - `~/.config/sway/config-nested` - Nested mode

2. **Set window properties rõ ràng**:
   - Dùng window class/name đặc biệt
   - Dễ quản lý trong KWin

3. **Monitor resources**:
   - Theo dõi CPU/RAM usage
   - Nested mode tốn nhiều tài nguyên hơn

4. **Backup config**:
   - Luôn backup config trước khi test
   - Sử dụng git để version control

## Kết Luận

Tích hợp Sway nested với Plasma mở ra nhiều khả năng:
- Development workflow linh hoạt
- Testing environment đa dạng
- Learning tool mạnh mẽ
- Hybrid desktop experience

Experiment và tùy chỉnh theo nhu cầu của bạn!
