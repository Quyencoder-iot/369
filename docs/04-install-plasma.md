# Hướng Dẫn Cài Đặt KDE Plasma

## Mục Lục
1. [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
2. [Các phiên bản Plasma](#các-phiên-bản-plasma)
3. [Cài đặt trên Ubuntu](#cài-đặt-trên-ubuntu)
4. [Chuyển đổi sang Plasma](#chuyển-đổi-sang-plasma)
5. [Cài đặt Plasma Wayland](#cài-đặt-plasma-wayland)
6. [Troubleshooting](#troubleshooting)

---

## Yêu Cầu Hệ Thống

### Phần Cứng Tối Thiểu

```
CPU: x86_64 dual-core 2GHz+
RAM: 2GB (khuyến nghị 4GB+)
GPU: 
  - Bất kỳ GPU với OpenGL 2.0+
  - Intel HD Graphics 
  - AMD Radeon
  - NVIDIA (với driver hỗ trợ)
Disk: 5GB cho full installation
Display: 1024x768 minimum (khuyến nghị 1920x1080+)
```

### Phần Cứng Đề Xuất cho Trải Nghiệm Tốt

```
CPU: Quad-core 2.5GHz+
RAM: 8GB+
GPU: 
  - Intel HD Graphics 530+ (Gen 9+)
  - AMD RX 460+ hoặc equivalent
  - NVIDIA GTX 750+ hoặc equivalent
  - Với driver cập nhật
Disk: 10GB+ (SSD preferred for performance)
Display: 1920x1080 hoặc cao hơn
```

### Phần Mềm Yêu Cầu

```
OS: Ubuntu 20.04+ (hoặc Debian-based distro tương tự)
Kernel: Linux 5.4+
Display Server: X11 hoặc Wayland support
```

---

## Các Phiên Bản Plasma

### Plasma Version History

```
KDE Plasma 5 (2014-2024):
├── Plasma 5.0 - 5.27 (Qt 5)
├── Stable, mature
└── Recommended for production

KDE Plasma 6 (2024+):
├── Qt 6 based
├── Wayland-first
├── Better performance
├── HDR support
└── Cutting edge features
```

### Chọn Version Nào?

**Plasma 5.27 (Stable):**
- ✅ Ổn định, tested thoroughly
- ✅ Đầy đủ documentation
- ✅ Hỗ trợ tốt từ community
- ✅ Available trên most distros
- ❌ Older features

**Plasma 6.x (Latest):**
- ✅ Latest features
- ✅ Better Wayland support
- ✅ Better performance
- ✅ HDR, VRR support
- ❌ Có thể có bugs
- ❌ Ít documentation hơn

---

## Cài Đặt Trên Ubuntu

### Ubuntu 22.04 LTS (Plasma 5.24)

#### Option 1: Kubuntu Packages (Recommended)

```bash
# Update system
sudo apt update
sudo apt upgrade -y

# Install KDE Plasma Desktop (minimal)
sudo apt install kde-plasma-desktop

# Hoặc install full Kubuntu desktop
sudo apt install kubuntu-desktop

# Hoặc install standard KDE (medium)
sudo apt install kde-standard
```

**Package differences:**

| Package | Size | Components | Recommended For |
|---------|------|------------|-----------------|
| `kde-plasma-desktop` | ~1GB | Core Plasma only | Minimal setup, experts |
| `kde-standard` | ~2GB | Plasma + common apps | Most users |
| `kubuntu-desktop` | ~3GB | Full Kubuntu experience | New users, complete setup |

#### Option 2: Backports (Newer version)

```bash
# Add Kubuntu backports PPA
sudo add-apt-repository ppa:kubuntu-ppa/backports
sudo apt update

# Install/upgrade Plasma
sudo apt install kubuntu-desktop

# Check version
plasmashell --version
# Should show 5.27 or newer
```

### Ubuntu 24.04 LTS (Plasma 5.27+)

```bash
# Ubuntu 24.04 comes with newer Plasma
sudo apt update
sudo apt install kubuntu-desktop

# Check version
plasmashell --version
# Output: plasmashell 5.27.x
```

### Install Plasma 6 (Experimental)

#### From Neon Repository

```bash
# Warning: This is experimental and may break your system
# Only do this on test machine or VM

# Add KDE Neon repository
echo "deb http://archive.neon.kde.org/user jammy main" | \
    sudo tee /etc/apt/sources.list.d/neon.list

# Add key
wget -qO - http://archive.neon.kde.org/public.key | \
    sudo apt-key add -

# Update and install
sudo apt update
sudo apt install kde-plasma-desktop plasma-workspace-wayland

# Reboot
sudo reboot
```

#### Better: Use KDE Neon ISO

Instead of upgrading Ubuntu, consider:
- Download [KDE Neon](https://neon.kde.org/)
- Based on Ubuntu LTS
- Always latest Plasma
- Official KDE distribution

### Package Details

#### Core Plasma Packages

```bash
# Essential Plasma components
plasma-desktop           # Desktop shell
plasma-workspace         # Core workspace
plasma-workspace-wayland # Wayland session
kwin-x11                # Window manager (X11)
kwin-wayland            # Window manager (Wayland)
systemsettings          # System settings
plasma-nm               # Network manager
plasma-pa               # PulseAudio integration
powerdevil              # Power management
bluedevil              # Bluetooth
kscreen                # Display management
```

#### KDE Applications

```bash
# File management
dolphin                 # File manager
kio-extras             # Extra KIO slaves

# Internet
firefox-esr            # Or falkon (KDE browser)

# Multimedia
gwenview               # Image viewer
okular                 # Document viewer
elisa                  # Music player
dragon                 # Video player

# Utilities
konsole                # Terminal
kate                   # Text editor
kcalc                  # Calculator
spectacle              # Screenshot tool
kwrite                 # Simple text editor

# System
partitionmanager       # Partition editor
ksysguard             # System monitor (Plasma 5)
plasma-systemmonitor  # System monitor (Plasma 6)
```

### Complete Installation Script

```bash
#!/bin/bash
# complete-plasma-install.sh

set -e

echo "=== KDE Plasma Installation Script ==="
echo "This will install KDE Plasma on Ubuntu"
echo ""

# Update system
echo "[1/5] Updating system..."
sudo apt update
sudo apt upgrade -y

# Install Plasma
echo "[2/5] Installing Plasma Desktop..."
sudo apt install -y \
    kde-plasma-desktop \
    plasma-workspace-wayland \
    sddm \
    kde-config-sddm

# Install essential KDE apps
echo "[3/5] Installing KDE Applications..."
sudo apt install -y \
    dolphin \
    konsole \
    kate \
    gwenview \
    okular \
    spectacle \
    ark \
    kio-extras \
    plasma-nm \
    plasma-pa \
    powerdevil \
    bluedevil

# Install useful utilities
echo "[4/5] Installing utilities..."
sudo apt install -y \
    firefox \
    libreoffice-kde \
    vlc \
    gimp

# Setup SDDM
echo "[5/5] Setting up display manager..."
sudo systemctl enable sddm
sudo systemctl set-default graphical.target

echo ""
echo "=== Installation Complete! ==="
echo "Please reboot and select 'Plasma (X11)' or 'Plasma (Wayland)' from login screen"
echo ""
echo "Reboot now? (y/n)"
read -r response
if [[ "$response" =~ ^[Yy]$ ]]; then
    sudo reboot
fi
```

Save as `install-plasma.sh` and run:
```bash
chmod +x install-plasma.sh
./install-plasma.sh
```

---

## Chuyển Đổi Sang Plasma

### Từ GNOME

```bash
# Install Plasma
sudo apt install kubuntu-desktop

# Logout
# At login screen, click gear icon ⚙️
# Select "Plasma (X11)" or "Plasma (Wayland)"
# Login
```

**Cleanup GNOME (optional):**
```bash
# Remove GNOME Shell (careful!)
sudo apt remove gnome-shell

# Remove GNOME packages but keep libs
sudo apt remove --purge ubuntu-desktop
sudo apt autoremove

# Keep: Don't remove gnome-* libraries
# KDE apps may depend on them
```

### Từ XFCE

```bash
# Install Plasma
sudo apt install kde-plasma-desktop

# Logout and select Plasma session
```

### Từ Other DEs

Same process:
1. Install Plasma packages
2. Logout
3. Select Plasma session from login screen

### Dual DE Setup (Keep Both)

You can keep multiple DEs:

```bash
# Both GNOME and Plasma installed
# Choose at login which one to use

# Advantages:
# - Fallback if one breaks
# - Compare features
# - Use apps from both

# Disadvantages:
# - Disk space (~5GB extra)
# - Potential conflicts in settings
# - Duplicate functionality
```

---

## Cài Đặt Plasma Wayland

### Enable Wayland Session

```bash
# Install Wayland session package
sudo apt install plasma-workspace-wayland

# Install required dependencies
sudo apt install \
    xwayland \
    kwayland-integration \
    plasma-wayland-protocols

# Reboot
sudo reboot

# At login screen:
# Click user → Click gear icon → Select "Plasma (Wayland)"
```

### Check if Running Wayland

```bash
# After login, open Konsole and run:
echo $XDG_SESSION_TYPE
# Output should be: wayland

# Check KWin compositor:
qdbus org.kde.KWin /KWin org.kde.KWin.currentDesktop
# If works, you're on Wayland

# Alternative check:
loginctl show-session $(loginctl | grep $(whoami) | awk '{print $1}') -p Type
# Output: Type=wayland
```

### Wayland vs X11 Comparison

```
Feature              │ X11           │ Wayland
─────────────────────┼───────────────┼─────────────────
Stability            │ ✓✓✓           │ ✓✓
Performance          │ ✓✓            │ ✓✓✓
Security             │ ✓             │ ✓✓✓
Screen sharing       │ ✓✓✓           │ ✓✓
Remote desktop       │ ✓✓✓           │ ✓✓
NVIDIA proprietary   │ ✓✓✓           │ ✓ (limited)
Fractional scaling   │ ✓ (blurry)    │ ✓✓✓
VRR/FreeSync         │ Limited       │ ✓✓✓
HDR                  │ ✗             │ ✓ (Plasma 6)
Legacy apps          │ ✓✓✓           │ ✓✓ (XWayland)
```

### Force X11 if Wayland Issues

```bash
# Edit SDDM config
sudo vim /etc/sddm.conf.d/wayland.conf

# Add:
[General]
DisplayServer=x11

# Restart SDDM
sudo systemctl restart sddm
```

---

## Troubleshooting

### SDDM không khởi động

```bash
# Check SDDM status
sudo systemctl status sddm

# If failed, check logs
sudo journalctl -u sddm -e

# Common fix: Reinstall
sudo apt install --reinstall sddm

# Enable and start
sudo systemctl enable sddm
sudo systemctl start sddm
```

### Black screen sau khi login

```bash
# Press Ctrl+Alt+F3 (switch to TTY3)
# Login with username and password

# Check plasmashell
systemctl --user status plasma-plasmashell

# Restart it
systemctl --user restart plasma-plasmashell

# Check for errors
journalctl --user -u plasma-plasmashell -e

# If still fails, reset config:
mv ~/.config/plasma-org.kde.plasma.desktop-appletsrc \
   ~/.config/plasma-org.kde.plasma.desktop-appletsrc.bak
```

### Plasma is very slow

```bash
# Check GPU driver
lspci -k | grep -A 3 VGA

# Install proper drivers:

# Intel:
sudo apt install intel-media-va-driver xserver-xorg-video-intel

# AMD:
sudo apt install mesa-vulkan-drivers firmware-amd-graphics

# NVIDIA:
# Check available drivers:
ubuntu-drivers devices

# Install recommended:
sudo ubuntu-drivers autoinstall

# Or specific version:
sudo apt install nvidia-driver-535

# Reboot after driver install
sudo reboot
```

### Compositor issues (tearing, artifacts)

```bash
# System Settings → Display and Monitor → Compositor
# Try changing:
# - Rendering backend: OpenGL 3.1 / OpenGL 2.0
# - Vsync: Automatic / Never / Always
# - Latency: Balance / Prefer latency / Prefer smoothness

# Or via command line:
kwriteconfig5 --file kwinrc --group Compositing --key Backend OpenGL
kwriteconfig5 --file kwinrc --group Compositing --key GLCore true

# Restart KWin:
kwin_x11 --replace &  # For X11
# Or
kwin_wayland --replace &  # For Wayland
```

### High memory usage

```bash
# Check what's using memory:
systemctl --user status | grep plasma

# Common memory hogs:
# - akonadi (PIM services) - Disable if not using KMail
systemctl --user stop akonadi.service
systemctl --user disable akonadi.service

# - baloo (file indexing) - Disable if not needed
balooctl disable

# - kdeconnect - Disable if not using
systemctl --user stop kdeconnect.service

# Restart Plasma:
killall plasmashell
kstart5 plasmashell
```

### Wayland issues

#### App doesn't start on Wayland

```bash
# Some apps need XWayland:
# Check if XWayland is running:
pgrep -a Xwayland

# If not running:
sudo apt install xwayland

# Force app to use XWayland:
env WAYLAND_DISPLAY= app-name
```

#### Screen sharing not working on Wayland

```bash
# Install xdg-desktop-portal:
sudo apt install \
    xdg-desktop-portal \
    xdg-desktop-portal-kde

# Restart session
```

#### NVIDIA Wayland issues

```bash
# NVIDIA proprietary driver on Wayland requires:
# 1. Driver 495+
nvidia-smi

# 2. Enable DRM kernel mode setting
sudo vim /etc/default/grub
# Add to GRUB_CMDLINE_LINUX:
# nvidia-drm.modeset=1

sudo update-grub
sudo reboot

# 3. Check if working:
cat /sys/module/nvidia_drm/parameters/modeset
# Should output: Y
```

### Settings not saving

```bash
# Config files might be corrupted
# Backup and reset:

cd ~/.config
mkdir config-backup
cp -r plasma* kwin* kde* config-backup/

# Delete plasma configs:
rm plasma*rc kwin*rc kde*

# Logout and login
# Plasma will recreate default configs
```

### Crash on startup

```bash
# Boot to recovery mode:
# At GRUB menu, select "Advanced options" → "Recovery mode"
# Select "root" (Drop to shell)

# Mount filesystem read-write:
mount -o remount,rw /

# Create new user to test:
adduser testuser
adduser testuser sudo

# Reboot:
reboot

# Login as testuser
# If Plasma works, your old user profile is corrupted

# Fix: Move old configs:
# Login as original user from TTY (Ctrl+Alt+F3)
cd ~
mv .config .config.old
mv .local .local.old
mv .cache .cache.old

# Reboot and login
```

---

## Post-Installation Setup

### First Boot Checklist

```bash
# 1. Update system
sudo apt update && sudo apt upgrade

# 2. Configure displays
# System Settings → Display and Monitor

# 3. Configure keyboard
# System Settings → Input Devices → Keyboard

# 4. Configure mouse/touchpad
# System Settings → Input Devices → Mouse/Touchpad

# 5. Set up networking
# System Settings → Connections

# 6. Enable file indexing (optional)
balooctl enable
balooctl start

# 7. Install additional software
# Discover (GUI) or apt
```

### Recommended Applications

```bash
# Development:
sudo apt install \
    kate \
    kdevelop \
    kompare \
    kdesdk-thumbnailers

# Productivity:
sudo apt install \
    okular \
    libreoffice-kde \
    korganizer \
    kontact

# Multimedia:
sudo apt install \
    vlc \
    kdenlive \
    krita \
    elisa

# Internet:
sudo apt install \
    firefox \
    qbittorrent \
    konversation

# Utilities:
sudo apt install \
    filelight \
    partitionmanager \
    krename \
    kfind
```

### Performance Tuning

```bash
# Disable animations (faster):
# System Settings → Workspace Behavior → Desktop Effects
# Uncheck effects you don't need

# Disable compositing:
kwriteconfig5 --file kwinrc --group Compositing --key Enabled false
qdbus org.kde.KWin /Compositor suspend

# Re-enable:
kwriteconfig5 --file kwinrc --group Compositing --key Enabled true
qdbus org.kde.KWin /Compositor resume

# Disable file indexing (Baloo):
balooctl disable

# Disable Akonadi (if not using KMail):
akonadictl stop
```

---

## Next Steps

1. ✅ Plasma installed and running
2. 📖 Read [06-operate-plasma.md](./06-operate-plasma.md) for usage guide
3. 🎨 Customize appearance (themes, widgets, etc.)
4. 🔧 Configure shortcuts and behaviors
5. 🚀 Install your favorite applications

---

## Tài Nguyên Bổ Sung

- [KDE UserBase Wiki](https://userbase.kde.org/)
- [Kubuntu Documentation](https://docs.kubuntu.org/)
- [KDE Forums](https://forum.kde.org/)
- [r/kde](https://reddit.com/r/kde)
- [Arch Wiki - KDE](https://wiki.archlinux.org/title/KDE) (applies to all distros)
