# LayerShellQt Learning Resources

Tài liệu học tập về LayerShellQt - thư viện Qt cho giao thức wlr-layer-shell của Wayland.

## 📚 Nội dung

### Tài liệu
- **[LayerShellQt-Guide.md](./LayerShellQt-Guide.md)** - Hướng dẫn toàn diện từ cơ bản đến nâng cao
- **[LayerShellQt-CheatSheet.md](./LayerShellQt-CheatSheet.md)** - Cheat sheet tra cứu nhanh

### Ví dụ code
Thư mục `examples/` chứa các ví dụ thực tế:
- `top_panel.cpp` - Panel cố định ở cạnh trên
- `floating_dock.cpp` - Dock nổi ở dưới
- `desktop_widget.cpp` - Widget đồng hồ trên desktop
- `notification.cpp` - Notification popup

## 🚀 Build ví dụ

```bash
# Cài đặt dependencies
# Arch: sudo pacman -S layer-shell-qt qt6-base
# Ubuntu: sudo apt install layer-shell-qt libqt6-dev

# Build
cd examples
mkdir build && cd build
cmake ..
make

# Chạy (phải trong Wayland session)
./top_panel
./floating_dock
```

## 📖 Mục lục nhanh

1. **Layers**: Background → Bottom → Top → Overlay
2. **Anchors**: Top, Bottom, Left, Right (combine với |)
3. **Exclusive Zone**: >0 chiếm, =0 nổi, =-1 ignore
4. **Keyboard**: None, OnDemand, Exclusive

## ⚠️ Yêu cầu

- Wayland session (không phải X11)
- Compositor hỗ trợ wlr-layer-shell (KWin, Sway, etc.)
- Qt 6 + LayerShellQt library
