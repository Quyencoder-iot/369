# 🚀 START HERE - LayerShellQt Learning Journey

Chào mừng! Đây là điểm bắt đầu để học LayerShellQt.

## ⚡ Quick Links

- 📖 **[README.md](./README.md)** - Overview & concepts tổng quan
- 🎓 **[LEARNING-GUIDE.md](./LEARNING-GUIDE.md)** - Lộ trình học chi tiết
- 💻 **[examples/](./examples/)** - Code examples thực tế
- 📚 **[docs/](./docs/)** - Documentation đầy đủ

---

## 🎯 Bạn Muốn Gì?

### 🚀 "Tôi muốn bắt đầu nhanh!"
1. Đọc [README.md](./README.md) - 15 phút
2. Build example: `cd examples && ./build.sh`
3. Run: `./01-simple-panel`
4. Modify code và experiment!

---

### 📚 "Tôi muốn học từ đầu đến cuối"
Follow [LEARNING-GUIDE.md](./LEARNING-GUIDE.md):
- Ngày 1-2: Fundamentals
- Ngày 3-4: Intermediate
- Ngày 5-7: Advanced

---

### 🔍 "Tôi cần tìm thông tin cụ thể"
Go to [docs/06-quick-reference.md](./docs/06-quick-reference.md) - Cheatsheet

---

### 💡 "Tôi muốn xem code examples"
Browse [examples/](./examples/):
- `01-simple-panel.cpp` - Top panel
- `02-notification.cpp` - Notifications
- `03-desktop-widget.cpp` - Desktop widget
- `04-animated-wallpaper.cpp` - Wallpaper
- `05-multi-monitor-panel.cpp` - Multi-monitor

---

### 📖 "Tôi muốn hiểu concepts sâu"
Đọc theo thứ tự:
1. [docs/01-core-concepts.md](./docs/01-core-concepts.md)
2. [docs/02-api-reference.md](./docs/02-api-reference.md)
3. [docs/03-examples.md](./docs/03-examples.md)
4. [docs/04-advanced-topics.md](./docs/04-advanced-topics.md)
5. [docs/05-best-practices.md](./docs/05-best-practices.md)

---

### 🎯 "Tôi có project cụ thể"

**Tôi muốn tạo:**
- **Panel/Taskbar** → Đọc [03-examples.md](./docs/03-examples.md) #1
- **Notification** → Đọc [03-examples.md](./docs/03-examples.md) #5
- **Desktop Widget** → Đọc [03-examples.md](./docs/03-examples.md) #3
- **Wallpaper** → Đọc [03-examples.md](./docs/03-examples.md) #4
- **Lock Screen** → Đọc [03-examples.md](./docs/03-examples.md) #9
- **Multi-Monitor** → Đọc [03-examples.md](./docs/03-examples.md) #7

---

## 📊 Prerequisites

### Required Knowledge
- ✅ C++ basics
- ✅ Qt fundamentals (QWidget, signals/slots)
- ✅ Basic Wayland concepts (helpful but not required)

### System Requirements
- ✅ Wayland compositor với wlr-layer-shell support
  - KDE Plasma (Wayland) ✓
  - Sway ✓
  - Hyprland ✓
  - River ✓
  - GNOME ✗ (not supported)
- ✅ Qt 5.15+
- ✅ LayerShellQt library

### Check Your System
```bash
# Check session type
echo $XDG_SESSION_TYPE  # Should be: wayland

# Check compositor
echo $XDG_SESSION_DESKTOP

# Check layer shell support
wayland-info | grep layer_shell
# Should show: zwlr_layer_shell_v1
```

---

## 🏗️ Build & Run Examples

### Quick Build
```bash
cd examples
./build.sh
./01-simple-panel
```

### CMake Build
```bash
cd examples
mkdir build && cd build
cmake ..
make
./01-simple-panel
```

---

## 📚 Documentation Structure

### Overview Documents
- `README.md` - Project overview, quick start
- `LEARNING-GUIDE.md` - Step-by-step learning path
- `START-HERE.md` - This file (navigation)

### Core Documentation (`docs/`)
1. **01-core-concepts.md** (45-60 min)
   - Wayland architecture
   - wlr-layer-shell protocol
   - Layers, anchors, exclusive zones

2. **02-api-reference.md** (30-45 min)
   - Complete API documentation
   - All classes, methods, properties
   - CMake & QML integration

3. **03-examples.md** (1-2 hours)
   - 10 complete, working examples
   - Panel, dock, widget, wallpaper, etc.
   - Copy-paste ready code

4. **04-advanced-topics.md** (45-60 min)
   - Dynamic configuration
   - Multi-monitor strategies
   - Performance optimization
   - Debugging

5. **05-best-practices.md** (30-45 min)
   - Design principles
   - Coding patterns
   - UI/UX guidelines
   - Testing strategies

6. **06-quick-reference.md** (reference)
   - Cheatsheet
   - Quick lookup
   - Common patterns
   - Troubleshooting

### Examples (`examples/`)
- Working C++ examples
- CMakeLists.txt for building
- Build script (build.sh)
- README with instructions

---

## 🎓 Learning Paths

### Path 1: Fast Track (1 day)
**For experienced Qt developers**
1. README.md (15 min)
2. 01-core-concepts.md (30 min, skim)
3. 06-quick-reference.md (15 min)
4. Build all examples (30 min)
5. Start your project!

### Path 2: Comprehensive (1 week)
**For thorough understanding**
- Follow [LEARNING-GUIDE.md](./LEARNING-GUIDE.md)
- Read all documentation
- Build and modify all examples
- Complete practice projects

### Path 3: Project-Based (variable)
**Learn by doing**
1. Pick a project (panel, widget, etc.)
2. Find relevant example
3. Read related docs sections
4. Build, modify, extend
5. Refer to docs as needed

---

## 🆘 Help & Support

### When Stuck
1. Check [06-quick-reference.md](./docs/06-quick-reference.md)
2. Search this repository
3. Re-read relevant doc section
4. Check LayerShellQt GitHub issues
5. Ask on KDE forums/IRC

### Common Issues
- "Layer shell not available" → Check compositor support
- "Compilation error" → Verify Qt & LayerShellQt installed
- "Window wrong position" → Review anchor documentation
- "Not appearing on screen" → Configure before show()

All covered in [06-quick-reference.md](./docs/06-quick-reference.md) Troubleshooting section!

---

## 🎯 Success Checklist

After learning, you should be able to:
- [ ] Explain 4 layers and their use cases
- [ ] Use anchors to position windows
- [ ] Set appropriate exclusive zones
- [ ] Handle multi-monitor setups
- [ ] Build a functional panel
- [ ] Debug layer shell issues
- [ ] Apply best practices
- [ ] Read LayerShellQt API docs

---

## 🌟 What's Next After Learning?

1. **Build Real Projects**
   - Solve your own needs
   - Contribute to KDE
   - Share with community

2. **Explore Advanced Topics**
   - Wayland protocol development
   - Qt Wayland integration
   - Compositor development

3. **Give Back**
   - Answer questions
   - Write tutorials
   - Contribute code

---

## 📞 Contact & Community

- **KDE Invent:** https://invent.kde.org/plasma/layer-shell-qt
- **KDE Forums:** https://discuss.kde.org/
- **IRC:** #kde on Libera.Chat

---

## 🎉 Ready to Start?

Choose your path above and dive in!

**Recommended first step for most people:**
→ **Read [README.md](./README.md)** to understand what LayerShellQt is

Then follow the learning path that fits your style!

**Happy Learning! 🚀**

---

*Last updated: December 2024*
*Tạo với ❤️ cho cộng đồng KDE/Qt*
