# Tổng Quan về Sway, Nested Mode và KDE Plasma

## Mục lục
1. [Giới thiệu](#giới-thiệu)
2. [Wayland vs X11](#wayland-vs-x11)
3. [Sway là gì?](#sway-là-gì)
4. [Nested Mode là gì?](#nested-mode-là-gì)
5. [KDE Plasma là gì?](#kde-plasma-là-gì)
6. [Tại sao kết hợp chúng?](#tại-sao-kết-hợp-chúng)

---

## Giới thiệu

Tài liệu này hướng dẫn bạn hiểu sâu về ba khái niệm quan trọng trong Linux desktop:
- **Sway**: Một Wayland compositor tương thích với i3
- **Nested Mode**: Chạy một compositor bên trong compositor khác
- **KDE Plasma**: Desktop environment mạnh mẽ với Wayland support

---

## Wayland vs X11

### X11 (X Window System)
- Ra đời năm 1984
- Kiến trúc client-server
- Window manager và display server tách biệt
- Có nhiều vấn đề bảo mật (keylogging, screen capture)

```
┌─────────────────────────────────────────────────────────┐
│                    X11 Architecture                      │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐               │
│  │  App 1   │  │  App 2   │  │  App 3   │   Applications │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘               │
│       │             │             │                      │
│       └─────────────┼─────────────┘                      │
│                     ▼                                    │
│            ┌────────────────┐                            │
│            │   X Server     │  Display Server            │
│            └────────┬───────┘                            │
│                     │                                    │
│                     ▼                                    │
│            ┌────────────────┐                            │
│            │ Window Manager │  (i3, openbox, etc.)       │
│            └────────┬───────┘                            │
│                     │                                    │
│                     ▼                                    │
│            ┌────────────────┐                            │
│            │   Compositor   │  (picom, compton)          │
│            └────────────────┘                            │
└─────────────────────────────────────────────────────────┘
```

### Wayland
- Ra đời năm 2008, stable từ 2012
- Compositor = Display Server + Window Manager + Compositor
- Bảo mật hơn (sandbox isolation)
- Hiệu năng tốt hơn (no context switching)

```
┌─────────────────────────────────────────────────────────┐
│                  Wayland Architecture                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐               │
│  │  App 1   │  │  App 2   │  │  App 3   │   Applications │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘               │
│       │             │             │                      │
│       └─────────────┼─────────────┘                      │
│                     │                                    │
│                     ▼                                    │
│   ┌─────────────────────────────────────────┐           │
│   │           Wayland Compositor             │           │
│   │  ┌─────────────────────────────────┐    │           │
│   │  │  Display Server + WM + Compositor│    │           │
│   │  │        (All-in-One)              │    │           │
│   │  └─────────────────────────────────┘    │           │
│   │  Examples: Sway, KWin, Mutter, wlroots  │           │
│   └─────────────────────────────────────────┘           │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

---

## Sway là gì?

### Định nghĩa
**Sway** (SirCmpwn's Wayland compositor) là một Wayland compositor được thiết kế để tương thích với i3 window manager.

### Đặc điểm chính
- **Tiling Window Manager**: Tự động sắp xếp cửa sổ
- **i3-compatible**: Sử dụng cùng cú pháp cấu hình với i3
- **Dựa trên wlroots**: Library compositor mạnh mẽ
- **Nhẹ và nhanh**: Tiêu thụ ít tài nguyên

### Kiến trúc Sway

```
┌─────────────────────────────────────────────────────────┐
│                     Sway Architecture                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌─────────────────────────────────────────────────┐    │
│  │                    Sway                          │    │
│  ├─────────────────────────────────────────────────┤    │
│  │  ┌───────────────┐  ┌───────────────────────┐   │    │
│  │  │  Config Parser │  │  Input Handler        │   │    │
│  │  │  (i3 compat)   │  │  (keyboard, mouse)    │   │    │
│  │  └───────────────┘  └───────────────────────┘   │    │
│  │                                                  │    │
│  │  ┌───────────────┐  ┌───────────────────────┐   │    │
│  │  │ Layout Engine │  │  Output Manager       │   │    │
│  │  │ (tiling logic)│  │  (multi-monitor)      │   │    │
│  │  └───────────────┘  └───────────────────────┘   │    │
│  └────────────────────────┬────────────────────────┘    │
│                           │                              │
│                           ▼                              │
│  ┌─────────────────────────────────────────────────┐    │
│  │                   wlroots                        │    │
│  │  (Modular Wayland compositor library)            │    │
│  │  - Rendering     - Input handling                │    │
│  │  - DRM/KMS       - XWayland support              │    │
│  └────────────────────────┬────────────────────────┘    │
│                           │                              │
│                           ▼                              │
│  ┌─────────────────────────────────────────────────┐    │
│  │                Wayland Protocol                  │    │
│  └─────────────────────────────────────────────────┘    │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

---

## Nested Mode là gì?

### Định nghĩa
**Nested Mode** là khả năng chạy một Wayland compositor bên trong một compositor hoặc desktop environment khác.

### Cách hoạt động

```
┌─────────────────────────────────────────────────────────┐
│              Nested Compositor Architecture              │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌─────────────────────────────────────────────────┐    │
│  │          Host Compositor (Parent)                │    │
│  │          Example: KDE Plasma (KWin)              │    │
│  │                                                  │    │
│  │    ┌─────────────────────────────────────┐      │    │
│  │    │     Nested Compositor (Child)        │      │    │
│  │    │     Example: Sway                    │      │    │
│  │    │                                      │      │    │
│  │    │   ┌──────────┐  ┌──────────┐        │      │    │
│  │    │   │  App 1   │  │  App 2   │        │      │    │
│  │    │   │ (in Sway)│  │ (in Sway)│        │      │    │
│  │    │   └──────────┘  └──────────┘        │      │    │
│  │    │                                      │      │    │
│  │    └─────────────────────────────────────┘      │    │
│  │                                                  │    │
│  │    ┌──────────┐  ┌──────────┐                   │    │
│  │    │  App 3   │  │  App 4   │  (in Plasma)      │    │
│  │    └──────────┘  └──────────┘                   │    │
│  │                                                  │    │
│  └─────────────────────────────────────────────────┘    │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### Tại sao cần Nested Mode?
1. **Development & Testing**: Test compositor mà không cần logout
2. **Learning**: Học cách sử dụng WM mới an toàn
3. **Hybrid Workflow**: Kết hợp ưu điểm của nhiều DE/WM
4. **Screen Recording**: Dễ dàng record một session cụ thể

---

## KDE Plasma là gì?

### Định nghĩa
**KDE Plasma** là một desktop environment đầy đủ tính năng, hiện đại và có thể tùy biến cao.

### Thành phần chính

```
┌─────────────────────────────────────────────────────────┐
│                  KDE Plasma Components                   │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌─────────────────────────────────────────────────┐    │
│  │              Plasma Desktop Shell                │    │
│  │  - Panels      - Widgets    - Desktop           │    │
│  │  - Applets     - Themes     - Activities        │    │
│  └─────────────────────────────────────────────────┘    │
│                          │                               │
│                          ▼                               │
│  ┌─────────────────────────────────────────────────┐    │
│  │                    KWin                          │    │
│  │         (Window Manager / Compositor)            │    │
│  │  - X11 backend (kwin_x11)                        │    │
│  │  - Wayland backend (kwin_wayland)                │    │
│  └─────────────────────────────────────────────────┘    │
│                          │                               │
│                          ▼                               │
│  ┌─────────────────────────────────────────────────┐    │
│  │              KDE Frameworks (KF5/KF6)            │    │
│  │  - KIO        - Kirigami   - KConfig            │    │
│  │  - Solid      - KWindowSystem                    │    │
│  └─────────────────────────────────────────────────┘    │
│                          │                               │
│                          ▼                               │
│  ┌─────────────────────────────────────────────────┐    │
│  │                  Qt Framework                    │    │
│  │         (Cross-platform application framework)   │    │
│  └─────────────────────────────────────────────────┘    │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

---

## Tại sao kết hợp chúng?

### Use Cases

| Scenario | Giải pháp |
|----------|-----------|
| Học tiling WM mà không rời Plasma | Sway nested trong Plasma |
| Phát triển ứng dụng Wayland | Test trong nested compositor |
| Workflow coding cần tiling | Sway cho coding, Plasma cho đa phương tiện |
| Debug compositor issues | Chạy nested để isolate vấn đề |

### Workflow ví dụ

```
┌─────────────────────────────────────────────────────────┐
│              Developer Workflow Example                  │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  KDE Plasma Desktop (Main Environment)                   │
│  ├── Virtual Desktop 1: Communication                    │
│  │   ├── Slack                                          │
│  │   ├── Email (KMail)                                  │
│  │   └── Browser                                        │
│  │                                                       │
│  ├── Virtual Desktop 2: Development (Sway Nested)       │
│  │   └── [Sway Window]                                  │
│  │       ├── Terminal (neovim) ─────┬─ Tiling Layout    │
│  │       ├── Terminal (htop)  ──────┤                   │
│  │       └── Terminal (logs)  ──────┘                   │
│  │                                                       │
│  └── Virtual Desktop 3: Documentation                   │
│      ├── Obsidian                                       │
│      └── PDF Reader                                     │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

---

## Tiếp theo

Đọc các tài liệu tiếp theo để học chi tiết:
1. [Cài đặt Sway](02-cai-dat-sway.md)
2. [Sway Nested Mode](03-sway-nested-mode.md)
3. [KDE Plasma & Tích hợp](04-kde-plasma.md)
4. [Kiến trúc sâu về Wayland](05-wayland-architecture.md)
5. [Scripts và Thực hành](06-scripts-thuc-hanh.md)
