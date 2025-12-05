# Hướng Dẫn Học Sway Nested và Plasma

Tài liệu toàn diện về cách cài đặt, cấu hình và hiểu sâu về Sway Nested và KDE Plasma.

## 📚 Tài Liệu

- **[HUONG_DAN_SWAY_PLASMA.md](./HUONG_DAN_SWAY_PLASMA.md)** - Tài liệu chính, hướng dẫn chi tiết về Sway, Sway Nested và Plasma

## 🚀 Bắt Đầu Nhanh

### Cài Đặt Sway

```bash
# Ubuntu/Debian
chmod +x examples/install-sway.sh
./examples/install-sway.sh ubuntu

# Arch Linux
./examples/install-sway.sh arch

# Fedora
./examples/install-sway.sh fedora
```

### Cài Đặt Plasma

```bash
chmod +x examples/install-plasma.sh
./examples/install-plasma.sh ubuntu
```

### Chạy Sway Nested

```bash
chmod +x examples/sway-nested.sh
./examples/sway-nested.sh
```

## 📁 Cấu Trúc Thư Mục

```
.
├── HUONG_DAN_SWAY_PLASMA.md    # Tài liệu chính
├── README.md                    # File này
└── examples/
    ├── sway-config-example      # File config Sway mẫu
    ├── sway-nested.sh           # Script chạy Sway nested
    ├── install-sway.sh          # Script cài đặt Sway
    ├── install-plasma.sh        # Script cài đặt Plasma
    └── quick-reference.md       # Quick reference guide
```

## 🎯 Nội Dung Chính

1. **Giới Thiệu** - Tổng quan về Sway và Plasma
2. **Cài Đặt** - Hướng dẫn cài đặt chi tiết
3. **Cấu Hình** - Cấu hình cơ bản và nâng cao
4. **Vận Hành** - Cách sử dụng hàng ngày
5. **Hiểu Sâu** - Kiến trúc và cách hoạt động
6. **Troubleshooting** - Giải quyết vấn đề

## 🔧 Quick Reference

Xem [quick-reference.md](./examples/quick-reference.md) để có danh sách đầy đủ các lệnh và keybindings.

## 📖 Đọc Thêm

- [Sway Official Docs](https://swaywm.org/)
- [KDE Plasma Docs](https://kde.org/plasma-desktop/)
- [Wayland Protocol](https://wayland.freedesktop.org/)

## 💡 Tips

1. Bắt đầu với Sway nested để học mà không cần logout
2. Đọc và chỉnh sửa file config để hiểu cách hoạt động
3. Sử dụng `swaymsg` để tương tác với Sway từ terminal
4. Tham gia cộng đồng để học hỏi thêm

## 🤝 Đóng Góp

Nếu bạn tìm thấy lỗi hoặc muốn cải thiện tài liệu, vui lòng tạo issue hoặc pull request.

---

**Chúc bạn học tập thành công!** 🎉
