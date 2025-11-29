# Hướng Dẫn Nhanh - Sway Nested trên Ubuntu

## Cài Đặt Nhanh (3 bước)

### Bước 1: Cài đặt Sway và dependencies

```bash
./scripts/install-sway.sh
```

### Bước 2: Copy cấu hình (tùy chọn)

```bash
# Copy cấu hình Sway
cp config/sway/config ~/.config/sway/config

# Copy cấu hình Waybar
mkdir -p ~/.config/waybar
cp config/waybar/config ~/.config/waybar/config
cp config/waybar/style.css ~/.config/waybar/style.css
```

### Bước 3: Chạy Sway nested

```bash
./scripts/run-sway-nested.sh
```

## Phím Tắt Cơ Bản

- `Mod + Enter`: Mở terminal (Alacritty)
- `Mod + D`: Mở application launcher (Rofi)
- `Mod + H/J/K/L`: Di chuyển focus giữa các cửa sổ
- `Mod + Shift + H/J/K/L`: Di chuyển cửa sổ
- `Mod + 1-9`: Chuyển workspace
- `Mod + Shift + E`: Thoát Sway
- `Mod + Shift + C`: Reload cấu hình

*Mod = Windows/Super key*

## Troubleshooting Nhanh

**Sway không chạy được?**
```bash
# Kiểm tra log
sway -d 2> ~/sway.log
cat ~/sway.log
```

**Thiếu dependencies?**
```bash
sudo apt install --fix-missing
./scripts/install-sway.sh
```

**Cần thêm quyền?**
```bash
sudo usermod -aG input,video $USER
# Đăng xuất và đăng nhập lại
```

## Tài Liệu Đầy Đủ

Xem [README.md](README.md) để biết thêm chi tiết.
