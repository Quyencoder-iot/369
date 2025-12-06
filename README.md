# CMOS OPAMP - Thiết Kế Ở Cấp Độ Nguyên Tử và Phân Tử

Dự án này giải thích cơ chế hoạt động và thiết kế OPAMP CMOS ở cấp độ nguyên tử và phân tử.

## Tài Liệu

- **[CMOS_OPAMP_ATOMIC_DESIGN.md](CMOS_OPAMP_ATOMIC_DESIGN.md)**: Tài liệu chi tiết giải thích:
  - Cấu trúc nguyên tử/phân tử của CMOS
  - Thiết kế OPAMP 2-stage
  - Quy trình chế tạo ở cấp độ nguyên tử
  - Các thông số vật lý và hiện tượng lượng tử

## Công Cụ Tính Toán

- **[cmos_opamp_calculator.py](cmos_opamp_calculator.py)**: Script Python để:
  - Tính toán các thông số OPAMP (độ lợi, bandwidth)
  - Phân tích cấu trúc nguyên tử
  - Tính threshold voltage từ các thông số vật lý
  - Vẽ các biểu đồ đặc tuyến

## Chạy Script

```bash
python3 cmos_opamp_calculator.py
```

## Yêu Cầu

- Python 3.x
- numpy
- matplotlib
