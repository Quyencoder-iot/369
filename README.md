# Hướng Dẫn Học Sway Nested và KDE Plasma

## Giới Thiệu Tổng Quan

Đây là tài liệu hướng dẫn toàn diện về **Sway nested** và **KDE Plasma** - hai công nghệ quan trọng trong hệ sinh thái Linux desktop hiện đại.

### Sway là gì?

**Sway** là một window manager (trình quản lý cửa sổ) kiểu tiling cho Wayland, được thiết kế như một sự thay thế drop-in cho i3wm trên X11. Sway được viết bằng C và tận dụng sức mạnh của Wayland compositor.

**Đặc điểm chính:**
- Tiling window manager (cửa sổ được sắp xếp tự động không chồng lấn)
- Dựa trên Wayland thay vì X11
- Tương thích cấu hình với i3wm
- Hiệu năng cao, ít tiêu tốn tài nguyên
- Bảo mật tốt hơn X11

### Sway Nested là gì?

**Sway nested** là chế độ chạy Sway compositor bên trong một Wayland compositor khác hoặc X11 session. Điều này rất hữu ích cho:

- **Phát triển và testing**: Test cấu hình mà không cần logout
- **Debug**: Dễ dàng debug và xem logs
- **Học tập**: Thử nghiệm mà không ảnh hưởng session chính
- **Demo**: Trình diễn Sway trong môi trường desktop khác

### KDE Plasma là gì?

**KDE Plasma** là một desktop environment (môi trường desktop) đầy đủ tính năng, hiện đại và có thể tùy chỉnh cao cho Linux.

**Đặc điểm chính:**
- Desktop environment hoàn chỉnh với desktop shell, window manager, và widgets
- Hỗ trợ cả X11 và Wayland (qua KWin)
- Giao diện đẹp, hiện đại với hiệu ứng nâng cao
- Tùy chỉnh cao độ
- Tích hợp sâu với KDE frameworks và applications

### Sự Khác Biệt Giữa Sway và Plasma

| Khía Cạnh | Sway | KDE Plasma |
|-----------|------|------------|
| Loại | Window Manager | Desktop Environment |
| Phong cách | Tiling | Stacking/Floating |
| Giao diện | Minimalist | Full-featured |
| Tài nguyên | Rất nhẹ | Trung bình đến cao |
| Cấu hình | Text files | GUI + Files |
| Wayland | Native | Hỗ trợ qua KWin |
| Học tập | Dốc (cần học keybindings) | Dễ (familiar như Windows) |

## Cấu Trúc Tài Liệu

1. **[01-sway-nested-deep-dive.md](./docs/01-sway-nested-deep-dive.md)** - Hiểu sâu về Sway và Sway nested
2. **[02-plasma-deep-dive.md](./docs/02-plasma-deep-dive.md)** - Hiểu sâu về KDE Plasma
3. **[03-install-sway-nested.md](./docs/03-install-sway-nested.md)** - Hướng dẫn cài đặt Sway nested
4. **[04-install-plasma.md](./docs/04-install-plasma.md)** - Hướng dẫn cài đặt KDE Plasma
5. **[05-operate-sway.md](./docs/05-operate-sway.md)** - Vận hành và cấu hình Sway
6. **[06-operate-plasma.md](./docs/06-operate-plasma.md)** - Vận hành và cấu hình Plasma
7. **[07-wayland-protocol.md](./docs/07-wayland-protocol.md)** - Hiểu sâu về Wayland protocol
8. **[08-practical-examples.md](./docs/08-practical-examples.md)** - Ví dụ và thực hành

## Lộ Trình Học Tập

### Cho Người Mới Bắt Đầu

1. Đọc README này để có cái nhìn tổng quan
2. Đọc tài liệu về Wayland protocol (07) để hiểu nền tảng
3. Chọn học Sway hoặc Plasma trước:
   - **Sway**: Phù hợp nếu bạn muốn tối ưu hiệu năng, thích keyboard-driven workflow
   - **Plasma**: Phù hợp nếu bạn muốn desktop environment hoàn chỉnh, quen thuộc

### Cho Người Đã Biết Cơ Bản

1. Đọc deep dive về công nghệ bạn chọn (01 hoặc 02)
2. Thực hiện cài đặt (03 hoặc 04)
3. Học vận hành (05 hoặc 06)
4. Thực hành với examples (08)

### Cho Người Nâng Cao

1. Đọc toàn bộ tài liệu để hiểu sâu cả hai công nghệ
2. So sánh và phân tích kiến trúc
3. Tùy chỉnh và tối ưu cấu hình
4. Đóng góp cho community hoặc phát triển extensions

## Yêu Cầu Hệ Thống

### Tối Thiểu
- Ubuntu 20.04 hoặc mới hơn (hoặc distro Linux tương đương)
- 2GB RAM
- CPU dual-core
- GPU hỗ trợ OpenGL 2.0+

### Đề Xuất
- Ubuntu 22.04 hoặc 24.04
- 4GB RAM trở lên
- CPU quad-core
- GPU hiện đại với driver cập nhật

## Bắt Đầu Nhanh

### Cài Đặt Sway (Nested Mode)

```bash
# Cài đặt dependencies
sudo apt update
sudo apt install sway swaybg swaylock swayidle

# Chạy Sway nested từ session hiện tại
sway
```

### Cài Đặt KDE Plasma

```bash
# Cài đặt Plasma desktop
sudo apt update
sudo apt install kde-plasma-desktop

# Logout và chọn Plasma từ login screen
```

Chi tiết xem trong các tài liệu cài đặt tương ứng.

## Đóng Góp

Tài liệu này là mã nguồn mở. Bạn có thể:
- Báo cáo lỗi hoặc thiếu sót
- Đề xuất cải thiện
- Bổ sung ví dụ thực tế
- Dịch sang ngôn ngữ khác

## Tài Nguyên Bổ Sung

### Sway
- [Trang chủ Sway](https://swaywm.org/)
- [Sway Wiki](https://github.com/swaywm/sway/wiki)
- [man sway](https://man.archlinux.org/man/sway.5)

### KDE Plasma
- [Trang chủ KDE](https://kde.org/plasma-desktop/)
- [KDE UserBase Wiki](https://userbase.kde.org/)
- [KDE Developer Documentation](https://develop.kde.org/)

### Wayland
- [Wayland Official Site](https://wayland.freedesktop.org/)
- [Wayland Book](https://wayland-book.com/)
- [Wayland Protocols](https://gitlab.freedesktop.org/wayland/wayland-protocols)

## Giấy Phép

Tài liệu này được phát hành dưới giấy phép MIT License.

---

**Chúc bạn học tốt và khám phá thú vị về Sway nested và KDE Plasma!**
