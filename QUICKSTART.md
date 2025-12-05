# Quick Start Guide - Sway Nested và KDE Plasma

Chào mừng! Đây là hướng dẫn nhanh để bắt đầu học về Sway nested và KDE Plasma.

## 📚 Tài Liệu Có Sẵn

### Tổng Quan
- **[README.md](README.md)** - Giới thiệu tổng quan, lộ trình học tập

### Deep Dive (Hiểu Sâu)
1. **[Sway Nested Deep Dive](docs/01-sway-nested-deep-dive.md)** - Kiến trúc và cách hoạt động của Sway
2. **[Plasma Deep Dive](docs/02-plasma-deep-dive.md)** - Kiến trúc và cách hoạt động của KDE Plasma
3. **[Wayland Protocol](docs/07-wayland-protocol.md)** - Hiểu sâu về Wayland protocol

### Cài Đặt
4. **[Install Sway Nested](docs/03-install-sway-nested.md)** - Hướng dẫn cài đặt Sway trên Ubuntu
5. **[Install Plasma](docs/04-install-plasma.md)** - Hướng dẫn cài đặt KDE Plasma trên Ubuntu

### Vận Hành
6. **[Operate Sway](docs/05-operate-sway.md)** - Hướng dẫn sử dụng và cấu hình Sway
7. **[Operate Plasma](docs/06-operate-plasma.md)** - Hướng dẫn sử dụng và cấu hình Plasma

### Thực Hành
8. **[Practical Examples](docs/08-practical-examples.md)** - Ví dụ và scripts thực tế

### Examples (Scripts & Configs)
- **[examples/](examples/)** - Config files và scripts có thể dùng ngay

## 🚀 Bắt Đầu Nhanh

### Tôi Muốn Học Sway Nested

```bash
# Bước 1: Đọc tổng quan
cat README.md

# Bước 2: Hiểu kiến trúc
cat docs/01-sway-nested-deep-dive.md

# Bước 3: Cài đặt
# Làm theo docs/03-install-sway-nested.md
sudo apt install sway swaybg swaylock swayidle

# Bước 4: Test nested mode
sway

# Bước 5: Học keybindings
# Đọc docs/05-operate-sway.md
# Mod+Return = Terminal
# Mod+d = Launcher
# Mod+Shift+q = Kill window
# Mod+1-9 = Workspaces

# Bước 6: Thử examples
cp examples/sway-configs/minimal-config ~/.config/sway/config
sway
```

**Timeline:**
- **30 phút**: Đọc docs, hiểu concept
- **15 phút**: Cài đặt
- **1 giờ**: Thử nghiệm và học keybindings
- **1 tuần**: Thành thạo workflow cơ bản

### Tôi Muốn Học KDE Plasma

```bash
# Bước 1: Đọc tổng quan
cat README.md

# Bước 2: Hiểu kiến trúc
cat docs/02-plasma-deep-dive.md

# Bước 3: Cài đặt
# Làm theo docs/04-install-plasma.md
sudo apt install kubuntu-desktop

# Bước 4: Reboot và chọn Plasma session
sudo reboot
# Tại login screen: chọn "Plasma (X11)" hoặc "Plasma (Wayland)"

# Bước 5: Explore System Settings
# System Settings → Appearance (themes)
# System Settings → Workspace Behavior (effects)
# System Settings → Shortcuts

# Bước 6: Customize
# Right-click desktop → Configure Desktop and Wallpaper
# Right-click panel → Edit Panel
```

**Timeline:**
- **30 phút**: Đọc docs
- **30 phút**: Cài đặt
- **1 giờ**: Explore và customize
- **Vài ngày**: Khám phá features

### Tôi Muốn Hiểu Wayland

```bash
# Đọc Wayland deep dive
cat docs/07-wayland-protocol.md

# Key concepts:
# - Wayland là protocol, không phải server
# - Compositor = Display Server + Window Manager + Compositor
# - Tốt hơn X11: performance, security, hiện đại
# - Sway và Plasma đều hỗ trợ Wayland
```

## 📖 Lộ Trình Học Tập Chi Tiết

### Người Mới Bắt Đầu (Beginner)

**Tuần 1: Tổng quan**
- [ ] Đọc README.md
- [ ] Đọc Wayland protocol basics (07)
- [ ] Quyết định học Sway hay Plasma trước

**Tuần 2: Cài đặt và cơ bản**
- [ ] Cài đặt môi trường đã chọn (03 hoặc 04)
- [ ] Học các thao tác cơ bản (05 hoặc 06)
- [ ] Thử các examples

**Tuần 3-4: Thực hành**
- [ ] Sử dụng hàng ngày
- [ ] Customize config/theme
- [ ] Thử scripts automation

### Người Trung Cấp (Intermediate)

**Đã biết Linux desktop basics**
- [ ] Đọc deep dive (01, 02, 07)
- [ ] So sánh architecture
- [ ] Thử cả Sway và Plasma
- [ ] Viết scripts riêng
- [ ] Tối ưu workflow

### Người Nâng Cao (Advanced)

**Đã dùng window manager/DE khác**
- [ ] Đọc toàn bộ documentation
- [ ] Phân tích so sánh với i3/GNOME/etc
- [ ] Contribute scripts/configs
- [ ] Tham gia community
- [ ] Giúp người khác

## 🎯 Mục Tiêu Học Tập

### Sau khi học Sway:
- ✅ Hiểu tiling window manager
- ✅ Thành thạo keyboard-driven workflow
- ✅ Config Sway qua text files
- ✅ Viết scripts automation
- ✅ Hiểu Wayland compositor

### Sau khi học Plasma:
- ✅ Hiểu desktop environment hoàn chỉnh
- ✅ Master KDE applications
- ✅ Customize GUI extensively
- ✅ Hiểu KDE frameworks
- ✅ Dùng KWin effects và scripts

### Sau khi học cả hai:
- ✅ Hiểu sâu Linux desktop stack
- ✅ So sánh pros/cons
- ✅ Chọn công cụ phù hợp cho từng task
- ✅ Troubleshoot issues
- ✅ Giúp đỡ community

## 💡 Tips

### Sway Tips
1. **Học keybindings dần dần** - Không cần nhớ hết lúc đầu
2. **Chạy nested để test** - Không cần logout
3. **Backup config trước khi edit** - `cp config config.bak`
4. **Dùng `swaymsg -t get_tree`** - Debug window hierarchy
5. **Join r/swaywm** - Community rất helpful

### Plasma Tips
1. **Explore System Settings** - Có rất nhiều options
2. **Don't over-customize ngay** - Dùng defaults trước
3. **Learn keyboard shortcuts** - Alt+Space cho KRunner
4. **Use Activities** - Powerful feature nhiều người bỏ qua
5. **Check KDE Store** - Themes, widgets, etc.

### General Tips
1. **Document your changes** - Ghi lại những gì bạn config
2. **Use git for dotfiles** - Version control configs
3. **Read man pages** - `man sway`, `man kwin`, etc.
4. **Ask questions** - Community rất sẵn lòng giúp
5. **Share your setup** - Post trên r/unixporn, r/swaywm, r/kde

## 🐛 Troubleshooting Quick Guide

### Sway không khởi động
```bash
# Check log
cat ~/.cache/sway/sway.log

# Validate config
sway -C

# Run with debug
SWAY_DEBUG=1 sway
```

### Plasma bị crash
```bash
# Restart plasmashell
killall plasmashell && kstart5 plasmashell

# Check logs
journalctl --user -u plasma-plasmashell -n 50
```

### Wayland issues
```bash
# Check if Wayland is running
echo $WAYLAND_DISPLAY  # Should output wayland-0 or similar

# Force X11 if needed (in ~/.profile)
export QT_QPA_PLATFORM=xcb
export GDK_BACKEND=x11
```

## 📞 Liên Hệ & Community

### Sway
- [GitHub](https://github.com/swaywm/sway)
- [IRC: #sway on Libera.Chat](https://web.libera.chat/#sway)
- [r/swaywm](https://reddit.com/r/swaywm)

### KDE Plasma
- [KDE Official](https://kde.org)
- [KDE Forums](https://forum.kde.org)
- [r/kde](https://reddit.com/r/kde)

### Wayland
- [Wayland Official](https://wayland.freedesktop.org)
- [Wayland IRC: #wayland on OFTC](https://webchat.oftc.net/?channels=wayland)

## 📝 Checklist

Để đảm bảo bạn đã nắm vững:

### Sway Checklist
- [ ] Chạy được Sway nested
- [ ] Mở terminal (Mod+Return)
- [ ] Kill window (Mod+Shift+q)
- [ ] Switch workspaces (Mod+1-9)
- [ ] Tile windows (Mod+h, Mod+v)
- [ ] Resize windows (Mod+r)
- [ ] Edit config file
- [ ] Reload config (Mod+Shift+c)
- [ ] Take screenshot
- [ ] Run script automation

### Plasma Checklist
- [ ] Login vào Plasma session
- [ ] Open System Settings
- [ ] Change theme
- [ ] Add widget to desktop
- [ ] Configure panel
- [ ] Set keyboard shortcut
- [ ] Use KRunner (Alt+Space)
- [ ] Configure multi-monitor
- [ ] Take screenshot (Spectacle)
- [ ] Customize window rules

## 🎓 Next Steps

Sau khi hoàn thành quick start:

1. **Deep dive vào topics bạn quan tâm**
   - Window management
   - Scripting & automation
   - Performance tuning
   - Custom themes

2. **Thực hành project**
   - Tạo dotfiles repository
   - Viết scripts riêng
   - Contribute to community

3. **Explore ecosystem**
   - Try other compositors (Hyprland, River)
   - Try other DEs (GNOME, XFCE)
   - Learn X11 for comparison

4. **Share knowledge**
   - Write blog posts
   - Create videos
   - Help others on forums

---

**Chúc bạn học tốt! 🚀**

_Nếu có thắc mắc, đọc lại docs hoặc hỏi community!_
