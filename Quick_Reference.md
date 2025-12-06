# Tài Liệu Tham Khảo Nhanh - Thiết Kế OPAMP

## Công Thức Quan Trọng Nhất

### 1. Transconductance (gm)
```
gm = √(2μCox(W/L)ID) = √(2μCoxID·W/L)
```
- Càng lớn → gain càng cao, noise càng thấp
- Tăng bằng: tăng W/L hoặc tăng ID

### 2. Output Resistance (ro)
```
ro = 1/(λID)
```
- Cascode: rout = gm·ro²
- Càng lớn → gain càng cao

### 3. Gain
```
Differential Stage: A₁ = gm₁(ro₂ || ro₄)
Output Stage: A₂ = gm₇(ro₇ || ro₈)
Total: A₀ = A₁ × A₂
```
- Mục tiêu: > 60 dB (1000x)

### 4. Unity-Gain Bandwidth
```
GBW = gm₁/(2πCc)
```
- Cc: Miller compensation capacitor
- Độc lập với gain!

### 5. Phase Margin
```
PM = 90° - arctan(GBW/p₂)
```
- Cần: PM > 60° cho ổn định
- p₂ > 2.2×GBW → PM ≈ 60°

### 6. Slew Rate
```
SR = Itail/Cc
```
- Giới hạn large-signal performance
- Check: SR > 2π·GBW·Vout,max

### 7. Input Offset
```
σ(VOS) = AVT/√(W×L)
```
- AVT ≈ 5-20 mV·μm
- Giảm offset → tăng W×L

### 8. Common-Mode Rejection Ratio
```
CMRR = gm·ro,tail
```
- Tăng bằng cascode tail current
- Mục tiêu: > 80 dB

### 9. Power Dissipation
```
Power = Itotal × VDD
```
- Two-stage: Itotal ≈ 2×Itail (2 nhánh)
- Folded cascode: Itotal ≈ 3-4×Itail

### 10. Figure of Merit
```
FOM = GBW × CL / Power
```
- Đơn vị: MHz·pF/mW
- Cao hơn = tốt hơn
- Điển hình: 10-100

---

## Bảng Tra Cứu Nhanh

### Thông Số Transistor Điển Hình

| Thông số | NMOS | PMOS |
|----------|------|------|
| μCox | 200 μA/V² | 80 μA/V² |
| VT | 0.4-0.7 V | -0.4 to -0.7 V |
| λ (L=1μm) | 0.05-0.1 V⁻¹ | 0.1-0.15 V⁻¹ |
| AVT | 10-15 mV·μm | 15-25 mV·μm |

### VGS - VT Khuyến Nghị

| Mục đích | VGS - VT | Lý do |
|----------|----------|-------|
| Low power | 100-150 mV | gm/ID cao |
| General | 200-300 mV | Cân bằng |
| High swing | 400-600 mV | Headroom tốt |

### Chiều Dài Kênh (L)

| Transistor | L | Lý do |
|-----------|---|-------|
| Input pair | 2-5×Lmin | Matching, noise |
| Current mirror | 2-4×Lmin | Gain, matching |
| Tail current | 1-2×Lmin | CMRR |
| Output | Lmin-2×Lmin | Speed, drive |

### Dòng Bias Điển Hình

| Ứng dụng | Itail | GBW |
|----------|-------|-----|
| Ultra low power | 1-10 μA | 0.1-1 MHz |
| Low power | 10-50 μA | 1-10 MHz |
| Medium speed | 50-200 μA | 10-100 MHz |
| High speed | > 200 μA | > 100 MHz |

---

## Decision Tree: Chọn Kiến Trúc

```
Cần Gain > 80 dB?
├─ Yes → Cần Swing lớn?
│  ├─ Yes → Folded Cascode hoặc Gain-Boosted
│  └─ No → Telescopic Cascode
│
└─ No (60-80 dB) → Cần very high speed?
   ├─ Yes → Folded Cascode (1 stage)
   └─ No → Two-Stage Miller (đơn giản nhất)
```

---

## Quy Trình Thiết Kế 5 Bước

### Bước 1: Xác định Spec
- [ ] Gain, GBW, SR, CL, Power, VDD

### Bước 2: Chọn Cc
```
Cc = CL/3 đến CL/2
```

### Bước 3: Tính Itail
```
Từ SR: Itail = SR × Cc
Từ GBW: Itail = 2π·GBW·Cc·(VGS-VT)
→ Chọn giá trị lớn hơn
```

### Bước 4: Tính Kích Thước
```
(W/L)₁₂ = 2ID/(μnCox(VGS-VT)²)
Chọn L = 2-5×Lmin
→ Tính W

(W/L)₃₄ = (μn/μp)×(W/L)₁₂
```

### Bước 5: Verify PM
```
p₂ = gm₂/CL
p₂/GBW > 2.2? → OK
Không → tăng Cc hoặc giảm gm₂
```

---

## Checklist Nhanh

### DC
- [ ] Tất cả MOS ở saturation
- [ ] VDS > VGS - VT
- [ ] ID đúng như tính toán

### AC
- [ ] Gain > spec
- [ ] GBW > spec
- [ ] PM > 60°

### Transient
- [ ] SR > spec
- [ ] Overshoot < 10%
- [ ] Settling time < yêu cầu

### Layout
- [ ] Common-centroid cho M1-M2, M3-M4
- [ ] Dummy transistors
- [ ] Guard rings
- [ ] Bypass capacitors

---

## Troubleshooting

### Vấn đề: Gain thấp
**Nguyên nhân:**
- ro nhỏ (L ngắn, λ lớn)
- gm nhỏ

**Giải pháp:**
- Dùng cascode → tăng ro
- Tăng L → giảm λ
- Tăng W/L → tăng gm

---

### Vấn đề: PM thấp (<60°)
**Nguyên nhân:**
- p₂ quá gần GBW
- Cc quá nhỏ

**Giải pháp:**
- Tăng Cc
- Giảm gm₂ (output stage)
- Thêm nulling resistor Rz

---

### Vấn đề: SR không đủ
**Nguyên nhân:**
- Itail quá nhỏ
- Cc quá lớn

**Giải pháp:**
- Tăng Itail
- Giảm Cc (nhưng check PM!)

---

### Vấn đề: Offset lớn
**Nguyên nhân:**
- W×L nhỏ
- Layout không matching

**Giải pháp:**
- Tăng kích thước input pair
- Common-centroid layout
- Same orientation
- Dummy transistors

---

### Vấn đề: CMRR thấp
**Nguyên nhân:**
- ro,tail nhỏ
- Mismatch load

**Giải pháp:**
- Cascode tail current
- Matching tốt M3-M4
- Tăng L của tail

---

### Vấn đề: Swing nhỏ
**Nguyên nhân:**
- Quá nhiều transistor xếp chồng
- VDS,sat quá lớn

**Giải pháp:**
- Dùng folded cascode thay vì telescopic
- Tăng W/L → giảm VDS,sat
- Rail-to-rail output stage

---

## Ví Dụ Thiết Kế Điển Hình

### Case 1: General Purpose OPAMP
**Spec:**
- VDD = 1.8V
- Gain = 60 dB
- GBW = 10 MHz
- CL = 10 pF
- Power < 500 μW

**Thiết kế:**
- Architecture: Two-stage Miller
- Itail = 30 μA
- Cc = 3 pF
- Power = 2×30μA×1.8V = 108 μW ✓

---

### Case 2: High-Gain OPAMP
**Spec:**
- VDD = 2.5V
- Gain = 90 dB
- GBW = 5 MHz
- CL = 20 pF

**Thiết kế:**
- Architecture: Telescopic Cascode
- Itail = 50 μA
- No Cc needed (1 stage)
- Gain = gm(gm·ro²) ≈ 90 dB ✓

---

### Case 3: Low-Power OPAMP
**Spec:**
- VDD = 1.2V
- Gain = 60 dB
- GBW = 100 kHz
- Power < 10 μW

**Thiết kế:**
- Architecture: Two-stage
- Itail = 1 μA (ultra low!)
- Cc = 5 pF
- W/L rất lớn (weak inversion)
- Power = 2×1μA×1.2V = 2.4 μW ✓

---

## Tips & Tricks

### Tip #1: Bắt đầu với Itail
- Tính từ SR (worst case)
- Verify với GBW
- Chọn giá trị lớn hơn + margin 20%

### Tip #2: Luôn dùng Cascode
- Cho current mirror load → tăng gain
- Cho tail current → tăng CMRR
- Chỉ cần thêm 1 transistor, lợi ích rất lớn!

### Tip #3: Cc không quá nhỏ
- Quá nhỏ → PM thấp, không ổn định
- Quá lớn → SR thấp, tốn area
- Cc = CL/3 là điểm khởi đầu tốt

### Tip #4: Matching quan trọng hơn size
- Không cần transistor quá lớn
- Nhưng phải matching tốt
- Common-centroid > kích thước lớn

### Tip #5: Simulate nhiều corners
- TT: typical
- FF: fast (PM critical)
- SS: slow (GBW critical)
- FS, SF: mismatch
- Ít nhất 5 corners!

### Tip #6: Nulling Resistor là "free lunch"
- Thêm Rz = 1/gm₂
- PM tăng 10-20°
- Không tốn power
- Chỉ tốn area (nhưng ít)

### Tip #7: Check large signal!
- Không chỉ AC analysis
- Transient với step input
- Measure SR thực tế
- Check overshoot, ringing

### Tip #8: Post-Layout quan trọng!
- Parasitic cap làm PM giảm 5-15°
- GBW giảm 10-20%
- Luôn để margin trong hand-calc

---

## Các Giá Trị "Magic Numbers"

- **PM ≥ 60°** → Ổn định tốt
- **p₂/GBW > 2.2** → PM ≈ 60°
- **VGS-VT = 200-300 mV** → Tối ưu
- **L = 2-3×Lmin** → Cân bằng gain/speed
- **Cc = CL/3** → Điểm khởi đầu
- **Rz = 1/gm** → Nulling resistor
- **W×L > 50 μm²** → Matching tốt
- **(W/L)p = 2.5×(W/L)n** → Same gm
- **Itail margin: +20%** → Compensate variations
- **Power margin: +30%** → Headroom cho corners

---

## Tài Nguyên Thêm

- **OPAMP_CMOS_Atomic_Molecular_Design.md**: Chi tiết về vật lý và chế tạo
- **OPAMP_Core_Design_Principles.md**: Nguyên tắc thiết kế sâu

---

**Lưu ý**: Đây là tài liệu tham khảo nhanh. Để hiểu sâu, vui lòng đọc tài liệu chi tiết.
