# Thuật Ngữ - Sway Nested và Plasma

## A

**Application Launcher**: Trình khởi chạy ứng dụng, cho phép tìm kiếm và mở các ứng dụng.

## B

**Backend**: Phần backend của compositor xử lý việc render và input. Sway nested hỗ trợ các backend: `x11`, `headless`, `wayland`.

**Buffer**: Vùng nhớ chứa dữ liệu hình ảnh của cửa sổ. Trong Wayland, clients gửi buffers đến compositor để hiển thị.

## C

**Client**: Ứng dụng chạy trên Wayland compositor. Client giao tiếp với compositor qua Wayland protocol.

**Compositor**: Chương trình quản lý việc hiển thị các cửa sổ, xử lý input, và render. Sway và KWin là các compositor.

**Compositing**: Quá trình kết hợp nhiều cửa sổ thành một hình ảnh cuối cùng để hiển thị.

## D

**Display Server**: Hệ thống quản lý hiển thị và input. X11 và Wayland là các display server.

**DRM (Direct Rendering Manager)**: Hệ thống kernel quản lý GPU và framebuffer, được Wayland sử dụng.

## E

**EGL**: API để tạo OpenGL context trên Wayland/X11. Cho phép ứng dụng render bằng GPU.

## F

**Floating Window**: Cửa sổ có thể di chuyển tự do, không tuân theo layout tiling.

**Focus**: Cửa sổ đang nhận input từ bàn phím/chuột.

**Framebuffer**: Vùng nhớ chứa dữ liệu pixel để hiển thị trên màn hình.

## G

**GPU**: Graphics Processing Unit, xử lý đồ họa và rendering.

## H

**Headless Backend**: Backend cho phép chạy compositor mà không cần màn hình vật lý, hữu ích cho nested compositor.

## I

**Input**: Dữ liệu từ bàn phím, chuột, touchpad, v.v.

**IPC (Inter-Process Communication)**: Giao tiếp giữa các process. Sway sử dụng Unix socket cho IPC.

## K

**KWin**: Window manager và compositor của KDE Plasma.

**KMS (Kernel Mode Setting)**: Hệ thống kernel quản lý display mode và resolution.

## L

**Layer Shell**: Wayland protocol extension cho phép ứng dụng tạo overlay surfaces ở các layer khác nhau (panel, notification, dock).

**Layout**: Cách sắp xếp cửa sổ trên màn hình. Sway hỗ trợ tiling layout (split, stacking, tabbed).

## M

**Mod Key**: Phím modifier được dùng trong keybindings. Thường là Super/Windows key (`Mod4`).

## N

**Nested Compositor**: Compositor chạy bên trong một cửa sổ của compositor khác. Sway nested là ví dụ.

## O

**Output**: Màn hình hoặc display device. Sway quản lý các output riêng biệt.

**Overlay**: Surface hiển thị trên các cửa sổ thường, như panel hoặc notification.

## P

**Plasma**: Desktop environment của KDE, có thể chạy trên cả X11 và Wayland.

**Protocol**: Bộ quy tắc giao tiếp giữa client và compositor. Wayland có core protocol và nhiều extensions.

## Q

**Qt**: Framework ứng dụng, được Plasma sử dụng. Qt có hỗ trợ Wayland tốt.

## R

**Rendering**: Quá trình vẽ các cửa sổ lên màn hình.

**Resize**: Thay đổi kích thước cửa sổ.

## S

**Session**: Phiên làm việc của người dùng. Có thể là X11 session hoặc Wayland session.

**Surface**: Đối tượng cơ bản trong Wayland, đại diện cho một vùng có thể hiển thị (cửa sổ, panel, v.v.).

**Sway**: Tiling window manager dựa trên Wayland, fork của i3.

**Swaymsg**: Công cụ dòng lệnh để gửi commands đến Sway qua IPC.

## T

**Tiling**: Layout tự động sắp xếp cửa sổ không chồng lấp, chia màn hình thành các phần.

**Touchpad**: Bàn di chuột trên laptop.

## U

**Unfocused**: Cửa sổ không nhận focus, không nhận input từ bàn phím.

## V

**VSync**: Vertical synchronization, đồng bộ frame rate với refresh rate của màn hình để tránh tearing.

## W

**Wayland**: Display server protocol hiện đại, thay thế X11. Tập trung vào security và performance.

**Wayland Client**: Ứng dụng sử dụng Wayland protocol để giao tiếp với compositor.

**Wayland Protocol**: Bộ giao thức định nghĩa cách client và compositor giao tiếp.

**Window Manager**: Chương trình quản lý cửa sổ, xử lý layout, focus, resize, move.

**Workspace**: Không gian làm việc riêng biệt, có thể chứa nhiều cửa sổ. Còn gọi là "virtual desktop".

## X

**X11**: Display server protocol cũ, được Wayland thay thế.

**XWayland**: Compatibility layer cho phép ứng dụng X11 chạy trên Wayland compositor.

**xdg-shell**: Wayland protocol extension định nghĩa cách tạo và quản lý cửa sổ ứng dụng.

## Z

**zwlr_layer_shell_v1**: Wayland protocol extension cho Layer Shell, được định nghĩa bởi wlroots.

---

## Thuật Ngữ Liên Quan

### Keybindings
- **Mod**: Modifier key (thường là Super/Windows)
- **Bindsym**: Binding symbol, định nghĩa keybinding trong Sway config
- **Exec**: Execute command

### Window States
- **Focused**: Cửa sổ đang nhận focus
- **Floating**: Cửa sổ ở chế độ floating
- **Tiled**: Cửa sổ ở chế độ tiling
- **Fullscreen**: Cửa sổ toàn màn hình

### Layout Types
- **Split**: Chia màn hình theo chiều ngang hoặc dọc
- **Stacking**: Xếp chồng cửa sổ
- **Tabbed**: Hiển thị như tabs

### Colors (trong Sway config)
- **client.focused**: Màu cửa sổ đang focus
- **client.unfocused**: Màu cửa sổ không focus
- **client.urgent**: Màu cửa sổ urgent (cần chú ý)

---

## Tài Liệu Tham Khảo

Để hiểu sâu hơn về các thuật ngữ này, xem:
- [Hướng Dẫn Toàn Diện](HUONG_DAN_SWAY_NESTED_PLASMA.md)
- [Wayland Protocol Documentation](https://wayland.freedesktop.org/docs/html/)
- [Sway Wiki](https://github.com/swaywm/sway/wiki)
