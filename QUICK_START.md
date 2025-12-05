# Hướng Dẫn Nhanh - Sway Nested và Plasma

## Bắt Đầu Trong 5 Phút

### Bước 1: Cài Đặt

```bash
# Chạy script cài đặt tự động
cd /workspace
./scripts/install-sway-nested.sh

# Hoặc cài thủ công (Ubuntu/Debian)
sudo apt update
sudo apt install -y sway swaybg swayidle swaylock waybar wl-clipboard xwayland
```

### Bước 2: Kiểm Tra Hệ Thống

```bash
# Kiểm tra hỗ trợ Wayland
./scripts/check-wayland-support.sh
```

### Bước 3: Chạy Sway Nested

```bash
# Copy script launcher
cp scripts/sway-nested.sh ~/bin/sway-nested
chmod +x ~/bin/sway-nested

# Chạy Sway nested
sway-nested
```

### Bước 4: Sử Dụng Cơ Bản

Trong Sway nested:
- `Super + Enter`: Mở terminal
- `Super + D`: Mở application launcher
- `Super + J/K/L/;`: Di chuyển focus
- `Super + Shift + E`: Thoát Sway nested

## Tài Liệu Chi Tiết

Xem file `HUONG_DAN_SWAY_NESTED_PLASMA.md` để biết thêm chi tiết về:
- Cài đặt chi tiết
- Cấu hình nâng cao
- Hiểu sâu về kiến trúc
- Troubleshooting

## Ví Dụ và Scripts

- `examples/sway-config-advanced.md`: Cấu hình Sway nâng cao
- `examples/layer-shell-example.cpp`: Ví dụ Layer Shell
- `examples/plasma-integration.md`: Tích hợp với Plasma
- `scripts/sway-nested.sh`: Script launcher
- `scripts/install-sway-nested.sh`: Script cài đặt
- `scripts/check-wayland-support.sh`: Kiểm tra hệ thống

## Cần Giúp Đỡ?

1. Đọc `HUONG_DAN_SWAY_NESTED_PLASMA.md`
2. Chạy `./scripts/check-wayland-support.sh` để kiểm tra
3. Xem log: `swaymsg -t get_tree`
