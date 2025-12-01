# LayerShellQt Learning Resources

Tài liệu học tập về LayerShellQt - từ cơ bản đến nâng cao.

## Tài Liệu

- **[LAYERSHELLQT_GUIDE.md](./LAYERSHELLQT_GUIDE.md)** - Hướng dẫn đầy đủ về LayerShellQt
- **[QUICK_START.md](./QUICK_START.md)** - Hướng dẫn nhanh để bắt đầu

## Ví Dụ Code

Xem thư mục `examples/` để có các ví dụ thực tế:
- `top_panel.cpp` - Tạo top panel với clock
- `notification.cpp` - Tạo notification overlay
- `bottom_dock.cpp` - Tạo bottom dock/launcher

## Bắt Đầu Nhanh

1. Đọc [QUICK_START.md](./QUICK_START.md) để hiểu nhanh
2. Xem các ví dụ trong `examples/`
3. Đọc [LAYERSHELLQT_GUIDE.md](./LAYERSHELLQT_GUIDE.md) để hiểu sâu

## Yêu Cầu

- Qt 5.15+ hoặc Qt 6.x
- Wayland compositor hỗ trợ wlr-layer-shell (Sway, Hyprland, etc.)
- LayerShellQt library đã được cài đặt

## Build Examples

```bash
cd examples
mkdir build && cd build
cmake ..
make
```

## Tài Liệu Tham Khảo

- [LayerShellQt GitHub](https://github.com/sedwards2009/layershellqt)
- [wlr-layer-shell Protocol](https://github.com/swaywm/wlr-protocols)
- [Qt Wayland Documentation](https://doc.qt.io/qt-6/qtwayland-index.html)

