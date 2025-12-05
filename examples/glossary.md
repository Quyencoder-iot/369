# Thuật Ngữ và Khái Niệm

## Wayland

### Compositor
**Compositor** là một chương trình quản lý việc hiển thị và input trong Wayland. Nó:
- Nhận các buffer từ clients (ứng dụng)
- Kết hợp (composite) chúng thành một hình ảnh cuối cùng
- Gửi hình ảnh đó đến màn hình
- Xử lý input (bàn phím, chuột, touch)

**Ví dụ**: Sway, KWin, Mutter, Weston

### Client
**Client** là một ứng dụng chạy trên Wayland. Nó:
- Kết nối đến compositor qua Wayland protocol
- Gửi các buffer chứa nội dung cần hiển thị
- Nhận input events từ compositor

**Ví dụ**: Firefox, Terminal, Text Editor

### Protocol
**Protocol** là cách thức giao tiếp giữa client và compositor. Wayland protocol định nghĩa:
- Cách client yêu cầu tạo surface
- Cách gửi buffer để hiển thị
- Cách nhận input events
- Các extension protocols (như xdg-shell)

### Surface
**Surface** là một vùng hình chữ nhật mà client muốn hiển thị. Một surface có thể:
- Chứa một window
- Là một phần của window
- Được sử dụng cho popup, tooltip, etc.

### Buffer
**Buffer** chứa dữ liệu pixel thực tế mà client muốn hiển thị. Compositor nhận buffer và render nó lên màn hình.

### Seat
**Seat** đại diện cho một tập hợp các thiết bị input (keyboard, pointer, touch) được nhóm lại với nhau. Thường một seat = một người dùng.

### Output
**Output** là một màn hình hoặc thiết bị hiển thị. Một compositor có thể quản lý nhiều outputs.

## Sway

### Window Manager
**Window Manager** quản lý cách các cửa sổ được sắp xếp và hiển thị. Sway là một tiling window manager, nghĩa là nó tự động sắp xếp cửa sổ theo layout.

### Tiling
**Tiling** là cách sắp xếp cửa sổ không chồng lên nhau, mà chia màn hình thành các vùng. Các layout phổ biến:
- **Split**: Chia màn hình theo chiều ngang/dọc
- **Stacking**: Xếp chồng các cửa sổ
- **Tabbed**: Hiển thị như tabs

### Container
**Container** là một khái niệm trong Sway để tổ chức cửa sổ. Một container có thể:
- Chứa một window
- Chứa các containers con (tạo thành cây)
- Có layout (split, stacking, tabbed)

### Workspace
**Workspace** (không gian làm việc) là một nhóm các cửa sổ. Bạn có thể chuyển đổi giữa các workspaces để tổ chức công việc.

### Modifier Key
**Modifier Key** là phím được giữ khi nhấn các phím khác để tạo keybinding. Trong Sway, thường dùng `Mod4` (Super/Windows key).

### IPC (Inter-Process Communication)
**IPC** là cách Sway giao tiếp với các chương trình khác. Bạn có thể dùng `swaymsg` để gửi lệnh đến Sway.

### Nested
**Nested** nghĩa là chạy một compositor bên trong một compositor khác. Sway nested chạy Sway như một client của compositor cha (như Plasma).

## Plasma

### Desktop Environment
**Desktop Environment** là một bộ phần mềm hoàn chỉnh cung cấp:
- Desktop shell (giao diện người dùng)
- Window manager
- Applications
- System settings
- File manager
- etc.

### KWin
**KWin** là window manager và compositor của Plasma. Nó có thể chạy trên cả X11 và Wayland.

### Plasma Shell
**Plasma Shell** là phần tạo ra desktop, panel, widgets, v.v. Nó chạy trên KWin.

### QML
**QML** (Qt Modeling Language) là ngôn ngữ để mô tả giao diện người dùng trong Plasma. Nhiều Plasma components được viết bằng QML.

### KDE Frameworks
**KDE Frameworks** là tập hợp các thư viện Qt được KDE phát triển để xây dựng ứng dụng.

### Activities
**Activities** trong Plasma là các không gian làm việc khác nhau, mỗi activity có thể có:
- Desktop riêng
- Widgets riêng
- Applications riêng

## Kỹ Thuật

### DRM (Direct Rendering Manager)
**DRM** là subsystem của Linux kernel quản lý GPU và framebuffer. Compositor sử dụng DRM để hiển thị trực tiếp lên màn hình.

### KMS (Kernel Mode Setting)
**KMS** là phần của DRM cho phép kernel thiết lập resolution và refresh rate của màn hình.

### EGL
**EGL** là interface giữa rendering API (như OpenGL) và windowing system. Wayland compositors sử dụng EGL để render.

### libinput
**libinput** là thư viện xử lý input devices (keyboard, mouse, touchpad, touchscreen) trong Wayland.

### XKB
**XKB** (X Keyboard Extension) quản lý keyboard layout và key mapping. Sway sử dụng XKB để xử lý bàn phím.

### wlroots
**wlroots** là một library cung cấp các building blocks để xây dựng Wayland compositor. Sway được xây dựng trên wlroots.

### Headless Backend
**Headless Backend** là một backend của wlroots cho phép chạy compositor mà không cần truy cập trực tiếp vào hardware. Được dùng cho nested mode.

## Các Khái Niệm Khác

### Session
**Session** là một phiên làm việc của người dùng. Có thể là:
- X11 session
- Wayland session
- TTY session

### Display Manager
**Display Manager** (như GDM, SDDM) quản lý việc đăng nhập và khởi động session.

### TTY
**TTY** (Teletypewriter) là terminal text-based. Bạn có thể chuyển sang TTY bằng `Ctrl+Alt+F1-F6`.

### Floating Window
**Floating Window** là cửa sổ không tuân theo tiling layout, có thể di chuyển tự do như trong desktop environment truyền thống.

### Fullscreen
**Fullscreen** là chế độ cửa sổ chiếm toàn bộ màn hình, ẩn các cửa sổ khác.

### Focus
**Focus** là cửa sổ đang nhận input từ bàn phím. Chỉ có một cửa sổ có focus tại một thời điểm.

### Modifier
**Modifier** là các phím đặc biệt như Shift, Ctrl, Alt, Super được dùng kết hợp với phím khác.

### Keybinding
**Keybinding** là tổ hợp phím được gán cho một hành động cụ thể.

### Status Bar
**Status Bar** là thanh hiển thị thông tin hệ thống (workspace, time, battery, etc.). Trong Sway thường dùng Waybar.

### Launcher
**Launcher** là ứng dụng để khởi chạy các chương trình khác. Trong Sway thường dùng Rofi hoặc dmenu.

---

## So Sánh X11 và Wayland

| Khái niệm | X11 | Wayland |
|-----------|-----|---------|
| **Server** | X Server (tách biệt) | Compositor (tích hợp) |
| **Security** | Tất cả clients có thể thấy nhau | Clients bị cô lập |
| **Protocol** | X11 protocol (cũ, phức tạp) | Wayland protocol (hiện đại) |
| **Performance** | Có overhead từ X Server | Hiệu suất tốt hơn |
| **Input** | XInput | libinput (tích hợp) |

---

## Thuật Ngữ Tiếng Việt Thường Dùng

- **Compositor** → Trình tổng hợp
- **Window Manager** → Trình quản lý cửa sổ
- **Desktop Environment** → Môi trường desktop
- **Workspace** → Không gian làm việc
- **Keybinding** → Phím tắt
- **Nested** → Lồng nhau
- **Tiling** → Xếp gạch
- **Floating** → Nổi
- **Focus** → Tiêu điểm
- **Session** → Phiên làm việc

---

Hiểu rõ các thuật ngữ này sẽ giúp bạn học và làm việc với Sway và Plasma hiệu quả hơn!
