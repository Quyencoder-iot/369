# Điểm Cốt Lõi Trong Thiết Kế OPAMP

## Mục Lục
1. [6 Thông Số Thiết Kế Quan Trọng Nhất](#6-thông-số-thiết-kế-quan-trọng-nhất)
2. [Các Trade-off Chính](#các-trade-off-chính)
3. [Nguyên Tắc Thiết Kế Từng Khối](#nguyên-tắc-thiết-kế-từng-khối)
4. [5 Quyết Định Thiết Kế Quan Trọng](#5-quyết-định-thiết-kế-quan-trọng)
5. [Kỹ Thuật Tối Ưu Hóa](#kỹ-thuật-tối-ưu-hóa)
6. [Checklist Thiết Kế](#checklist-thiết-kế)

---

## 6 Thông Số Thiết Kế Quan Trọng Nhất

### 1. Gain (Độ Khuếch Đại) - A₀

**Tầm quan trọng**: ⭐⭐⭐⭐⭐

**Định nghĩa**:
```
A₀ = Vout/Vin |DC = (tín hiệu đầu ra) / (tín hiệu đầu vào)
```

**Yêu cầu điển hình**: 
- Minimum: 60 dB (1000x)
- Tốt: 80 dB (10,000x)
- Rất tốt: 100 dB (100,000x)

**Cách tính**:
```
A₀ = gm₁ · Rout₁ · gm₂ · Rout₂
```

**Điểm cốt lõi #1: Tăng Gain**

Có 2 cách chính:

**Cách 1: Tăng gm (Transconductance)**
```
gm = √(2μCox(W/L)ID)
```

Làm sao?
- ✅ Tăng W/L (rộng hơn, ngắn hơn)
- ✅ Tăng ID (dòng bias lớn hơn)
- ❌ Nhưng: Tốn công suất, tốn diện tích!

**Cách 2: Tăng Rout (Điện trở đầu ra)**
```
Rout = ro₁ || ro₂  (ở differential stage)
ro = 1/(λID)
```

Làm sao?
- ✅ **Cascode**: Tăng Rout lên gm·ro² (hiệu quả nhất!)
- ✅ Tăng chiều dài L (giảm λ)
- ✅ Giảm ID (nhưng mâu thuẫn với tăng gm)

**Kỹ thuật tốt nhất: Telescopic Cascode**
```
VDD
 |
M9 (cascode PMOS)
 |
M7 (cascode PMOS)
 |
M3,M4 (load)
 |
M5,M6 (cascode NMOS)
 |
M1,M2 (diff pair)
 |
Itail
```

Gain: **80-100 dB** chỉ với 1 stage!

**Nhược điểm Cascode**:
- Cần điện áp cao (nhiều transistor xếp chồng)
- Output swing nhỏ
- Không phù hợp low-voltage (<1.8V)

---

### 2. Unity-Gain Bandwidth (GBW) - Băng Thông

**Tầm quan trọng**: ⭐⭐⭐⭐⭐

**Định nghĩa**: Tần số mà gain = 1 (0 dB)

**Yêu cầu điển hình**:
- Audio: 1-10 MHz
- Video: 100-500 MHz
- High-speed: 1-10 GHz

**Công thức cốt lõi**:
```
GBW = gm₁/(2πCc)
```

**Điểm cốt lõi #2: Trade-off Speed vs Stability**

**Tăng GBW**:
1. ✅ Tăng gm₁ (tăng ID, tăng W/L)
2. ✅ Giảm Cc (Miller capacitor)

**NHƯNG** giảm Cc → **mất ổn định!**

**Phase Margin** (cực kỳ quan trọng):
```
PM = 180° - ∠A(GBW)
```

**Quy tắc vàng**:
- PM ≥ 60° → ổn định, overshoot < 10%
- PM = 45° → overshoot ~25%, chấp nhận được
- PM < 45° → ringing, có thể oscillation

**Vị trí các cực**:

**Cực thứ nhất (dominant pole)**:
```
p₁ = -1/(Rout · CL)
```
- Cực này quyết định GBW
- Luôn đặt ở tần số thấp (~10 Hz - 1 kHz)

**Cực thứ hai (non-dominant pole)**:
```
p₂ ≈ -gm₂/CL
```
- Cực này quyết định phase margin
- **Phải đặt ở tần số > 2×GBW** để PM > 60°

**Điều kiện ổn định**:
```
p₂ > 2.2 × GBW  → PM ≥ 60°
```

**Miller Compensation (Kỹ thuật quan trọng nhất)**:
```
    Stage 1      Cc      Stage 2
Vin → [A₁] ──────┬────→ [A₂] → Vout
                 │
                 └───────────┘
              (feedback)
```

**Hiệu ứng Miller**:
```
Cin,effective = Cc(1 + A₂)
```
- Cc nhỏ (~1-5 pF) nhưng tạo ra điện dung lớn
- Kéo p₁ xuống thấp
- Tách p₁ và p₂ xa nhau → ổn định

**Nulling Resistor** (kỹ thuật nâng cao):
```
    ┌─── Rz ───┐
    │          │
   Cc
```
- Thêm Rz nối tiếp với Cc
- Rz ≈ 1/gm₂
- Tạo zero để bù cực p₂
- Cải thiện PM thêm 10-20°

---

### 3. Slew Rate (SR) - Tốc Độ Thay Đổi

**Tầm quan trọng**: ⭐⭐⭐⭐

**Định nghĩa**: Tốc độ thay đổi tối đa của đầu ra
```
SR = |dVout/dt|max
```

**Yêu cầu điển hình**:
- Audio: 1-10 V/μs
- Video: 50-500 V/μs
- High-speed: > 1000 V/μs

**Công thức cốt lõi**:
```
SR = Itail/Cc
```

**Điểm cốt lõi #3: Large Signal vs Small Signal**

**Small signal** (tín hiệu nhỏ):
- Hoạt động bình thường
- Bandwidth = GBW/A₀

**Large signal** (tín hiệu lớn):
- Một bên differential pair tắt hoàn toàn
- Toàn bộ Itail nạp/xả Cc
- Tốc độ giới hạn bởi SR, không phải GBW!

**Ví dụ quan trọng**:

Giả sử: Vout = A·sin(2πft)
```
dVout/dt = 2πfA·cos(2πft)
|dVout/dt|max = 2πfA
```

Để không bị slew-rate limited:
```
SR > 2πfA
```

**Ví dụ số**:
- f = 1 MHz
- A = 1V (peak)
- SR cần thiết = 2π × 10⁶ × 1 = 6.28 V/μs

Nếu SR chỉ có 1 V/μs → **Bị méo tín hiệu!**

**Trade-off SR vs GBW**:
```
GBW = gm₁/(2πCc) = Itail/(2πCc·(VGS-VT))
SR = Itail/Cc
```

Tỷ lệ:
```
SR/GBW = 2π(VGS - VT)
```

**Nguyên tắc vàng**:
```
SR ≥ 2π × GBW × Vout,max
```

Nếu không thỏa mãn → tăng Itail!

---

### 4. Input Offset Voltage (VOS) - Điện Áp Lệch

**Tầm quan trọng**: ⭐⭐⭐⭐

**Định nghĩa**: Điện áp cần đặt vào input để Vout = 0

**Yêu cầu điển hình**:
- General purpose: < 5 mV
- Precision: < 500 μV
- Ultra-precision: < 50 μV

**Nguyên nhân chính**:

**1. Mismatch VT (threshold voltage)**:
```
VOS = ΔVT
```

**2. Mismatch gm**:
```
VOS = (VGS - VT) × Δgm/gm
```

**3. Mismatch kích thước W/L**:
```
VOS ∝ ΔW/W hoặc ΔL/L
```

**Điểm cốt lõi #4: Matching là tất cả!**

**Mô hình Pelgrom**:
```
σ(ΔVT) = AVT/√(W×L)
```
- AVT ≈ 5-20 mV·μm (tùy công nghệ)
- W×L: diện tích transistor

**Ví dụ**:
- W/L = 10μm/1μm, AVT = 10 mV·μm
- σ(ΔVT) = 10/√10 ≈ 3.16 mV
- 3σ ≈ 9.5 mV offset

**Cách giảm offset**:

**1. Tăng kích thước** (quan trọng nhất!):
```
W × L lớn → offset nhỏ
```
- Nhân đôi diện tích → offset giảm √2 = 1.4 lần
- Tăng 10 lần diện tích → offset giảm 3.16 lần

**Quy tắc thực tế**:
- Input differential pair: W×L ≥ 50-100 μm²
- Thường dùng: W=20μm, L=2μm → W×L=40μm²

**2. Layout Matching**:

**Common-Centroid Layout** (bắt buộc!):
```
M1a | M2a | M2b | M1b
```
- Chia mỗi transistor thành 2 phần
- Xen kẽ nhau
- Gradient ảnh hưởng đều cả 2 bên

**Interdigitated Layout**:
```
┌────┬────┬────┬────┐
│ M1 │ M2 │ M1 │ M2 │
└────┴────┴────┴────┘
```
- Nhiều finger nhỏ xen kẽ
- Matching tốt hơn

**Dummy Transistors**:
```
[D] [M1] [M2] [D]
```
- Đặt transistor giả ở 2 đầu
- Môi trường đồng nhất cho M1, M2

**3. Orientation (Hướng)**:
- M1 và M2 **phải cùng hướng**
- Gate cùng hướng dòng chảy
- Giảm stress gradient effect

**4. Auto-Zeroing / Chopping** (kỹ thuật nâng cao):
- Lưu offset vào capacitor
- Trừ đi trong quá trình hoạt động
- Đạt offset < 10 μV

---

### 5. Common-Mode Rejection Ratio (CMRR)

**Tầm quan trọng**: ⭐⭐⭐⭐

**Định nghĩa**: Khả năng loại bỏ tín hiệu đồng pha
```
CMRR = A_differential / A_common-mode
```

**Yêu cầu**: > 80 dB (thường 80-120 dB)

**Công thức**:
```
CMRR = gm × ro,tail
```
- ro,tail: điện trở đầu ra của tail current source

**Điểm cốt lõi #5: Tail Current Source là chìa khóa**

**Tail Current đơn giản**:
```
M1   M2
 │   │
 └─┬─┘
   │
  M6 (Itail)
```
- ro,tail = ro6 = 1/(λID)
- CMRR ≈ 40-60 dB (không tốt!)

**Cascode Tail Current** (bắt buộc cho CMRR cao!):
```
M1   M2
 │   │
 └─┬─┘
   │
  M7 (cascode)
   │
  M6 (Itail)
```
- ro,tail = gm7 × ro6 × ro7
- CMRR ≈ 80-120 dB (rất tốt!)

**Regulated Cascode** (tốt nhất):
```
      ┌─── Amplifier
      │
M1   M2
 │   │
 └─┬─┘
   │
  M7
   │
  M6
```
- ro,tail = A_amp × gm7 × ro6 × ro7
- CMRR > 120 dB

**Nguyên lý**:
- Tín hiệu common-mode → dòng qua M1, M2 thay đổi
- Nếu ro,tail vô cùng lớn → dòng không đổi
- VCM không ảnh hưởng đến differential output

---

### 6. Power Supply Rejection Ratio (PSRR)

**Tầm quan trọng**: ⭐⭐⭐⭐

**Định nghĩa**: Khả năng loại bỏ nhiễu từ nguồn điện
```
PSRR = |A_signal / A_supply|
```

**Yêu cầu**: 
- DC: > 60 dB
- Low frequency (< 1 kHz): > 60 dB
- High frequency: > 40 dB

**Điểm cốt lõi #6: Current Mirror và Bypass**

**PSRR⁺ (từ VDD)**:
```
PSRR⁺ ≈ gm_load × ro_load
```
- Phụ thuộc vào PMOS current mirror load

**PSRR⁻ (từ VSS)**:
```
PSRR⁻ ≈ gm_tail × ro_tail
```
- Phụ thuộc vào tail current source

**Cách cải thiện**:

**1. Cascode Current Mirrors**:
```
VDD
 │
M9 (cascode)
 │
M7 (load)
```
- PSRR⁺ tăng 20-40 dB

**2. Regulated Bias**:
- Dùng bandgap reference
- Bias ổn định với VDD
- PSRR cải thiện 10-20 dB

**3. On-chip Bypass Capacitor**:
```
VDD ─┬─ OPAMP
     │
    ═══ Cbypass (10-100 pF)
     │
    VSS
```
- Shunt nhiễu AC đi ground
- PSRR cải thiện ở high frequency

**4. Differential Operation**:
- PSRR⁺ và PSRR⁻ cancel nhau
- Fully differential OPAMP: PSRR > 100 dB

---

## Các Trade-off Chính

### Trade-off #1: Gain vs Bandwidth

**Quan hệ**:
```
GBW = A₀ × BW₃dB = hằng số
```

**Ý nghĩa**:
- Tăng gain → giảm bandwidth
- Giảm gain → tăng bandwidth

**Ví dụ**:
- GBW = 10 MHz
- A₀ = 100 (40 dB) → BW = 100 kHz
- A₀ = 10 (20 dB) → BW = 1 MHz

**Giải pháp**:
- Dùng feedback để trade-off theo yêu cầu
- Thiết kế với GBW cao từ đầu

---

### Trade-off #2: Speed vs Power

**Quan hệ**:
```
GBW = gm₁/(2πCc)
gm₁ ∝ √ID
Power = ID × VDD
```

**Ý nghĩa**:
- Tăng speed → tăng gm → tăng ID → tăng power
- Giảm power → giảm speed

**Figure of Merit (FOM)**:
```
FOM = GBW × CL / Power
```
- Đơn vị: MHz·pF/mW
- FOM cao = hiệu quả năng lượng tốt
- FOM điển hình: 10-100 MHz·pF/mW

**Kỹ thuật tối ưu**:
1. Chọn công nghệ nhỏ (high fT transistors)
2. Tối thiểu hóa Cc
3. Dùng gain-boosting thay vì tăng ID

---

### Trade-off #3: Swing vs Gain

**Vấn đề**:
- Cascode: Gain cao, swing thấp
- Single transistor: Gain thấp, swing cao

**Headroom cho mỗi transistor**:
- Saturation: VDS ≥ VGS - VT ≈ 200-400 mV
- Cascode 2 tầng: cần 400-800 mV
- Output swing giảm nhiều!

**Ví dụ**:
- VDD = 1.8V, VSS = 0V
- Telescopic cascode: 4 transistor xếp chồng
- Mỗi cái cần 200 mV → 800 mV không dùng được
- Swing thực tế: 0.4V đến 1.4V (chỉ 1V!)

**Giải pháp**:

**1. Folded Cascode**:
```
VDD
 │
M7,M8 (PMOS cascode load)
 │
M5,M6 (PMOS cascode)
 │   ↓
 │  M1,M2 (NMOS diff pair)
 │   │
 └───┴─── (fold over)
     │
    Itail
```
- Chỉ 2 transistor trên đường signal (thay vì 4)
- Swing tốt hơn: 0.2V đến 1.6V
- Nhưng: Tốn gấp đôi power!

**2. Rail-to-Rail Output**:
- Dùng cả NMOS và PMOS output
- Swing: gần 0V đến VDD
- Phức tạp hơn

**3. Low-Voltage Design**:
- Bulk-driven
- Subthreshold operation
- VGS-VT nhỏ (~100mV)

---

### Trade-off #4: Precision vs Area

**Quan hệ**:
```
σ(VOS) ∝ 1/√(W×L)
```

**Ý nghĩa**:
- Offset nhỏ → kích thước lớn
- Tiết kiệm area → offset lớn

**Ví dụ**:
- Target: VOS < 1 mV (3σ)
- AVT = 10 mV·μm
- Cần: W×L > 900 μm²
- → W=30μm, L=30μm (rất lớn!)

**Phương án**:
1. Accept offset cao hơn
2. Dùng trimming/calibration
3. Auto-zeroing (phức tạp, tốn power)

---

### Trade-off #5: Stability vs Speed

**Vấn đề**:
- PM cao (>70°) → ổn định, nhưng chậm
- PM thấp (45°) → nhanh, nhưng ringing

**Quan hệ**:
```
Settling time ∝ 1/(GBW × PM/100)
```

**Ví dụ**:
- GBW = 10 MHz, PM = 70°
- Settling (0.1%): ~500 ns

- GBW = 10 MHz, PM = 45°
- Settling (0.1%): ~800 ns (chậm hơn do ringing!)

**Kết luận**:
- **PM = 60-65° là tối ưu**
- PM > 70° không cần thiết
- PM < 55° nguy hiểm

---

## Nguyên Tắc Thiết Kế Từng Khối

### Khối 1: Differential Pair (Cặp Vi Sai)

**Chức năng**: Chuyển đổi điện áp vi sai thành dòng điện

**Sơ đồ**:
```
     M3       M4
    PMOS     PMOS  (Active Load)
     │        │
Vout1        Vout2
     │        │
Vin+ │        │ Vin-
    M1       M2
   NMOS     NMOS
     │        │
     └───┬────┘
         │
        M5 (Itail)
```

**Thông số chính**:
```
gm = √(2μCox(W/L)ID)  với ID = Itail/2
Gain = gm(ro2 || ro4)
```

**Nguyên tắc thiết kế**:

**1. Chọn Itail**:
```
Itail = 10-100 μA  (low power)
Itail = 100μA-1mA  (medium speed)
Itail > 1mA        (high speed)
```

**2. Kích thước M1, M2**:
```
Chọn (W/L) sao cho: VGS - VT = 200-400 mV
```

**Tại sao?**
- VGS-VT nhỏ (100mV): gm cao, nhưng dễ vào triode
- VGS-VT lớn (600mV): headroom tốt, nhưng gm thấp
- **200-300mV là tối ưu**

**Công thức**:
```
ID = (μCox/2)(W/L)(VGS-VT)²
→ (W/L) = 2ID/(μCox(VGS-VT)²)
```

**Ví dụ**:
- Itail = 100 μA → ID = 50 μA
- VGS-VT = 250 mV
- μnCox = 200 μA/V²
- (W/L) = 2×50/(200×0.25²) = 8

**Nếu L = 1μm → W = 8μm**
**Nhưng để matching tốt, tăng cả W và L:**
- L = 2μm → W = 16μm
- Diện tích W×L = 32 μm² (tốt cho matching)

**3. Kích thước M3, M4 (PMOS load)**:
```
(W/L)₃₄ = (μn/μp) × (W/L)₁₂
```
- μp ≈ 0.4 × μn
- → (W/L)₃₄ ≈ 2.5 × (W/L)₁₂

**Ví dụ**: (W/L)₁₂ = 8 → (W/L)₃₄ = 20

**Tại sao?** 
- PMOS và NMOS có cùng gm
- Current mirror chính xác hơn

**4. M5 (Tail Current)**:
```
(W/L)₅ = 2 × (W/L)₁₂
```
- Chịu toàn bộ Itail (gấp đôi ID của M1)
- (W/L) tăng để VDS,sat nhỏ → headroom tốt

**5. Input Common-Mode Range (ICMR)**:
```
VCM,min = VSS + VDS5,sat + VGS1
VCM,max = VDD + VT3 - |VGS3| - VDS1,sat
```

**Để ICMR rộng**:
- VDS,sat nhỏ → (W/L) lớn
- NMOS input pair: ICMR gần VSS
- PMOS input pair: ICMR gần VDD
- **Rail-to-rail input**: dùng cả 2!

---

### Khối 2: Current Mirror Load

**Chức năng**: 
1. Chuyển đổi dòng sang điện áp
2. Cung cấp điện trở cao (ro)

**Loại 1: Simple Current Mirror**:
```
VDD
 │
 ├─── M3 ──┬── M4
 │         │
Vbias    Vout
```

**Gain**: A = gm(ro2 || ro4)
- Điển hình: 20-40 dB (10-100x)

**Loại 2: Cascode Current Mirror** (Tốt hơn!):
```
VDD
 │
 ├─── M7 ──┬── M8  (Cascode)
 │    │    │
 ├─── M3 ──┴── M4  (Mirror)
Vbias     Vout
```

**Gain**: A = gm(gm_cascode × ro²)
- Điển hình: 60-80 dB (1000-10000x)

**Thiết kế**:
```
(W/L)cascode ≈ (W/L)mirror
Cùng dòng ID → cùng gm và ro
```

**Loại 3: Wide-Swing Cascode**:
- Thêm bias độc lập cho cascode
- Giảm VDS,sat → tăng swing
- Phức tạp hơn

---

### Khối 3: Output Stage

**Chức năng**:
1. Tăng gain thêm
2. Drive tải nặng (buffer)
3. Tăng output swing

**Loại 1: Common-Source** (đơn giản nhất):
```
    M7
   PMOS (load)
     │
    Vout ──── CL
     │
    M6
   NMOS (driver)
```

**Gain**: A₂ = gm6(ro6 || ro7)
- Điển hình: 20-40 dB

**Loại 2: Source Follower** (buffer):
```
Vout ──── CL
  │
 M6 (follower)
  │
 Ibias
```

**Gain**: A₂ ≈ 1 (0 dB)
**Output impedance**: 1/gm (thấp, ~100Ω)
- Tốt cho drive tải nặng
- Nhưng không tăng gain

**Loại 3: Push-Pull (Class AB)**:
```
    M7 (PMOS) ──┐
                │
              Vout ──── CL
                │
    M6 (NMOS) ──┘
```

**Ưu điểm**:
- Rail-to-rail output swing
- Drive current lớn (pull-up và pull-down)
- Hiệu quả năng lượng (Class AB)

**Nhược điểm**:
- Cần bias phức tạp (tránh crossover distortion)
- Quiescent current control

---

### Khối 4: Compensation (Bù Tần Số)

**Mục đích**: Đảm bảo ổn định (PM > 60°)

**Kỹ thuật #1: Miller Compensation** (Quan trọng nhất!)

**Nguyên lý**:
```
Cc nối giữa Vout và node giữa 2 stage
→ Tạo dominant pole tại node giữa
→ Tách các cực xa nhau
```

**Chọn Cc**:
```
Cc ≥ CL × A₂/10
```

**Ví dụ**:
- CL = 10 pF
- A₂ = 20 (26 dB)
- Cc ≥ 10 × 20 / 10 = 20 pF

**Thường dùng**: Cc = 2-5 pF (tùy spec)

**GBW với Miller compensation**:
```
GBW ≈ gm₁/Cc
```

**Kỹ thuật #2: Nulling Resistor**:
```
Rz nối tiếp với Cc
Rz = 1/gm₂
```

**Hiệu quả**:
- Tạo zero để cancel cực p₂
- PM cải thiện 10-20°
- Nhưng: Tốn area (điện trở lớn)

**Kỹ thuật #3: Cascode Compensation**:
- Dùng cho cascode amplifier
- Cc nhỏ hơn Miller
- Phức tạp hơn

---

## 5 Quyết Định Thiết Kế Quan Trọng

### Quyết Định #1: Kiến Trúc OPAMP

**Lựa chọn**:

**1. Two-Stage Miller**:
```
[Diff Amp] → Cc → [Common-Source]
```
- ✅ Gain cao (60-100 dB)
- ✅ Đơn giản
- ✅ Rail-to-rail output dễ
- ❌ Cần compensation (Cc)
- ❌ Không tốt cho very high speed

**Khi nào dùng**: General purpose, gain cao, speed vừa

**2. Telescopic Cascode**:
```
[Diff] + [Cascode Load] + [Cascode Tail]
```
- ✅ Gain rất cao (80-120 dB)
- ✅ 1 stage → không cần Cc
- ✅ Nhanh, ổn định
- ✅ Ít power (1 nhánh dòng)
- ❌ Swing thấp
- ❌ Cần VDD cao (>1.8V)
- ❌ ICMR hẹp

**Khi nào dùng**: High gain, high VDD, swing không quan trọng

**3. Folded Cascode**:
```
[Diff] "gấp" qua [Cascode Load]
```
- ✅ Gain cao (70-100 dB)
- ✅ Swing tốt hơn Telescopic
- ✅ ICMR rộng
- ✅ 1 stage → không cần Cc
- ❌ Tốn gấp đôi power
- ❌ Nhiều transistor

**Khi nào dùng**: High gain + good swing, chấp nhận power cao

**4. Fully Differential**:
```
Vin+  Vin-
  │    │
[Differential OPAMP]
  │    │
Vout+ Vout-
```
- ✅ PSRR rất cao
- ✅ Even-order distortion cancel
- ✅ Gấp đôi swing (differential)
- ❌ Cần common-mode feedback (CMFB)
- ❌ Phức tạp

**Khi nào dùng**: High-performance ADC, DAC, filters

---

### Quyết Định #2: Loại Input Pair

**NMOS Input**:
```
Vin+ ─ M1(NMOS)
Vin- ─ M2(NMOS)
```
- ✅ gm cao (μn > μp)
- ✅ Noise thấp hơn
- ✅ Offset thấp hơn (AVT thấp)
- ❌ ICMR gần VSS
- ❌ Flicker noise cao hơn PMOS

**Khi nào dùng**: 
- Low noise
- Input gần VSS
- High speed (gm cao)

**PMOS Input**:
```
Vin+ ─ M1(PMOS)
Vin- ─ M2(PMOS)
```
- ✅ ICMR gần VDD
- ✅ Flicker noise thấp
- ❌ gm thấp hơn
- ❌ Area lớn hơn (cần W lớn)

**Khi nào dùng**:
- Low 1/f noise
- Input gần VDD
- Precision applications

**Rail-to-Rail Input**:
```
Cả NMOS và PMOS song song
```
- ✅ ICMR: VSS đến VDD
- ❌ gm thay đổi với VCM (cần linearization)
- ❌ Phức tạp

---

### Quyết Định #3: Bias Current

**Low Current** (< 10 μA):
- ✅ Ultra low power
- ❌ Slow (GBW thấp)
- ❌ Noise cao
- ❌ Cần W/L rất lớn

**Medium Current** (10-100 μA):
- ✅ Cân bằng power/performance
- ✅ Phổ biến nhất
- Size hợp lý

**High Current** (> 100 μA):
- ✅ High speed
- ✅ Low noise
- ❌ High power
- ❌ Reliability (EM, HCI)

**Quy tắc chọn**:
```
Itail = 2π × GBW × Cc × (VGS - VT)
```

---

### Quyết Định #4: Chiều Dài Kênh (L)

**Minimum Length** (Lmin = 0.18μm, 65nm, ...):
- ✅ gm/ID cao → power efficiency
- ✅ fT cao → speed
- ✅ Area nhỏ
- ❌ Gain thấp (λ lớn, ro nhỏ)
- ❌ Matching kém
- ❌ Flicker noise cao
- ❌ Short-channel effects

**Long Channel** (L = 2-10 × Lmin):
- ✅ Gain cao (ro lớn)
- ✅ Matching tốt
- ✅ Noise thấp
- ✅ Ổn định hơn
- ❌ Area lớn
- ❌ Speed thấp
- ❌ Parasitic cap cao

**Khuyến nghị**:

| Transistor | L | Lý do |
|-----------|---|-------|
| Input pair (M1,M2) | 2-5×Lmin | Matching, noise |
| Current mirror load | 2-4×Lmin | Gain, matching |
| Tail current | 1-2×Lmin | Headroom, CMRR |
| Output stage | Lmin-2×Lmin | Speed, drive |
| Cascode | 1-2×Lmin | Cân bằng |

---

### Quyết Định #5: Output Swing vs Gain

**Nếu cần Swing cao**:
- Two-stage Miller
- Folded cascode
- Class AB output

**Nếu cần Gain cao**:
- Telescopic cascode
- Gain boosting
- Multi-stage

**Nếu cần cả hai**:
- Folded cascode + gain boosting
- Three-stage (phức tạp)

---

## Kỹ Thuật Tối Ưu Hóa

### Tối Ưu #1: Gain Boosting

**Vấn đề**: ro giảm ở công nghệ nhỏ → gain thấp

**Giải pháp**: Tăng ro bằng feedback

```
     ┌──── Mini Amp
     │       ↓
    M1 ←── Vfb
```

**Hiệu quả**:
```
ro,effective = ro × A_boost
```
- A_boost = 10-20
- ro tăng 20-40 dB
- Tổng gain tăng 20-40 dB

**Nhược điểm**:
- Phức tạp
- Tốn power
- Cần compensation

---

### Tối Ưu #2: Adaptive Biasing

**Vấn đề**: Itail cố định → không tối ưu mọi điều kiện

**Giải pháp**: Thay đổi bias theo load/speed

```
CL lớn → tăng Itail → tăng SR
CL nhỏ → giảm Itail → tiết kiệm power
```

**Cài đặt**:
- Sense CL qua replica
- Điều chỉnh Ibias proportional

---

### Tối Ưu #3: Chopper Stabilization

**Vấn đề**: Offset và 1/f noise

**Giải pháp**: Modulate tín hiệu lên high freq

```
Input → [Chop] → OPAMP → [Chop] → Output
        fchop              fchop
```

**Hiệu quả**:
- Offset < 10 μV
- 1/f noise → high freq (filtered)

**Nhược điểm**:
- Residual offset (charge injection)
- Tốn power (switches)

---

### Tối Ưu #4: Correlated Double Sampling (CDS)

**Vấn đề**: Offset, 1/f noise trong SC circuits

**Nguyên lý**:
1. Sample offset → lưu vào C
2. Amplify signal → trừ offset

**Hiệu quả**:
- Offset gần 0
- 1/f noise giảm >20 dB

---

### Tối Ưu #5: Nested Miller Compensation

**Dùng cho**: 3-stage OPAMP

**Vấn đề**: 2 non-dominant poles → khó ổn định

**Giải pháp**:
```
Cc1 từ output → node 2
Cc2 từ node 2 → node 1
```

**Hiệu quả**:
- 3 stage vẫn ổn định
- Gain rất cao (>100 dB)

---

## Checklist Thiết Kế

### Giai Đoạn 1: Specification (Xác Định Yêu Cầu)

**Input:**
- [ ] Gain (A₀): ____ dB
- [ ] GBW: ____ MHz
- [ ] Phase Margin: ____ ° (min 60°)
- [ ] Slew Rate: ____ V/μs
- [ ] Load Capacitance (CL): ____ pF
- [ ] Input Offset (VOS): ____ mV
- [ ] CMRR: ____ dB
- [ ] PSRR: ____ dB
- [ ] Input Range (ICMR): ____ to ____ V
- [ ] Output Swing: ____ to ____ V
- [ ] Power Budget: ____ mW
- [ ] VDD: ____ V

**Derived:**
- [ ] SR ≥ 2π·GBW·Vout,swing? ____
- [ ] Power dissipation acceptable? ____

---

### Giai Đoạn 2: Architecture Selection (Chọn Kiến Trúc)

**Quyết định:**
- [ ] Two-stage Miller
- [ ] Telescopic cascode
- [ ] Folded cascode
- [ ] Fully differential
- [ ] Other: ____

**Input type:**
- [ ] NMOS
- [ ] PMOS
- [ ] Rail-to-rail

---

### Giai Đoạn 3: Hand Calculation (Tính Toán Sơ Bộ)

**Step 1: Chọn Cc**
```
Cc = CL/5 to CL/3
Cc = ____ pF
```

**Step 2: Tính Itail từ SR hoặc GBW**
```
Từ SR: Itail = SR × Cc = ____
Từ GBW: Itail = 2π × GBW × Cc × (VGS-VT) = ____
Chọn Itail lớn hơn: Itail = ____ μA
```

**Step 3: Kích thước M1, M2**
```
Chọn (VGS-VT) = 250 mV
ID = Itail/2 = ____ μA
(W/L)₁₂ = 2ID/(μnCox(VGS-VT)²) = ____
Chọn L = ____ μm → W = ____ μm
```

**Step 4: Kích thước M3, M4**
```
(W/L)₃₄ = 2.5 × (W/L)₁₂ = ____
L = ____ μm, W = ____ μm
```

**Step 5: Kích thước M5 (tail)**
```
(W/L)₅ = 2 × (W/L)₁₂ = ____
```

**Step 6: Kích thước output stage**
```
Tương tự, target gm₂ hoặc SR
```

**Step 7: Verify Phase Margin**
```
p₂ = gm₂/CL = ____
GBW = gm₁/(2πCc) = ____
p₂/GBW = ____ (cần > 2.2)
PM ≈ 90° - arctan(GBW/p₂) = ____ ° (cần > 60°)
```

---

### Giai Đoạn 4: Simulation (Mô Phỏng)

**DC Operating Point:**
- [ ] Tất cả transistor ở saturation?
- [ ] VDS > VGS - VT cho tất cả?
- [ ] ID đúng như tính toán?

**AC Analysis:**
- [ ] Gain = ____ dB (đạt spec?)
- [ ] GBW = ____ MHz (đạt spec?)
- [ ] Phase Margin = ____ ° (> 60°?)

**Transient:**
- [ ] Slew Rate = ____ V/μs (đạt spec?)
- [ ] Settling time = ____ ns
- [ ] Overshoot < 10%?

**Noise:**
- [ ] Input-referred noise = ____ nV/√Hz
- [ ] Integrated noise < spec?

**Corner Simulation:**
- [ ] TT (typical): PASS
- [ ] FF (fast): PM > 60°?
- [ ] SS (slow): GBW > spec?
- [ ] FS, SF: PASS?

**Monte Carlo:**
- [ ] Offset (3σ) = ____ mV
- [ ] Yield > 99%?

---

### Giai Đoạn 5: Layout

**Matching:**
- [ ] M1, M2: Common-centroid
- [ ] M3, M4: Common-centroid
- [ ] Dummy transistors added
- [ ] Same orientation
- [ ] Interdigitated fingers

**Symmetry:**
- [ ] Axis of symmetry
- [ ] Balanced routing

**Guard Rings:**
- [ ] N-well guard ring
- [ ] Substrate contacts

**Shielding:**
- [ ] VDD, VSS bypass caps
- [ ] Sensitive nodes shielded

---

### Giai Đoạn 6: Post-Layout Verification

**Extraction:**
- [ ] RC extraction done
- [ ] Parasitic caps check

**Post-Layout Sim:**
- [ ] Gain degradation < 10%?
- [ ] GBW degradation < 20%?
- [ ] PM still > 60°?

**DRC/LVS:**
- [ ] DRC clean
- [ ] LVS clean

---

## Ví Dụ Thiết Kế Hoàn Chỉnh

### Specification
- VDD = 1.8V
- Gain > 60 dB
- GBW = 10 MHz
- CL = 10 pF
- SR > 10 V/μs
- PM > 60°
- Power < 1 mW

### Architecture
→ **Two-Stage Miller** (đơn giản, đạt được spec)

### Hand Calculation

**Cc:**
```
Cc = CL/3 = 10/3 ≈ 3 pF
```

**Itail từ SR:**
```
SR = Itail/Cc
Itail = 10 V/μs × 3 pF = 30 μA
```

**Check từ GBW:**
```
GBW = Itail/(2πCc(VGS-VT))
10 MHz = Itail/(2π × 3pF × 0.25V)
Itail ≈ 47 μA
```

→ Chọn **Itail = 50 μA** (lớn hơn cả 2 yêu cầu)

**M1, M2:**
```
ID = 25 μA
(VGS-VT) = 250 mV
μnCox = 200 μA/V²
(W/L) = 2×25/(200×0.25²) = 4

Để matching tốt:
L = 2 μm → W = 8 μm
```

**M3, M4:**
```
μp/μn ≈ 0.4
(W/L)₃₄ = 4/0.4 = 10
L = 2 μm → W = 20 μm
```

**M5:**
```
ID = 50 μA
(W/L)₅ = 8
L = 1 μm → W = 8 μm
```

**Output Stage (M6, M7):**
```
Similar to input stage
Chọn (W/L) để gm₂ ≈ gm₁/3
```

**Verify PM:**
```
gm₁ = √(2×200×4×25) = 200 μS
gm₂ ≈ 70 μS (smaller)
p₂ = gm₂/CL = 70μS/10pF ≈ 11 MHz
GBW = gm₁/(2πCc) = 200μS/(2π×3pF) ≈ 10.6 MHz
p₂/GBW ≈ 1.04 → PM ≈ 45° (KHÔNG ĐỦ!)
```

**→ Cần giảm gm₂ hoặc tăng Cc**

**Solution: Tăng Cc = 5 pF**
```
GBW = 200μS/(2π×5pF) ≈ 6.4 MHz
p₂/GBW ≈ 1.7 → PM ≈ 60° (OK!)
```

**Nhưng GBW giảm!**

**Better solution: Tăng Itail**
```
Itail = 80 μA → ID = 40 μA
Rescale all transistors
gm₁ = √(2×200×4×40) = 250 μS
GBW = 250μS/(2π×3pF) ≈ 13 MHz (OK!)
p₂ = 11 MHz
PM ≈ 50° (acceptable)
```

**Add Nulling Resistor:**
```
Rz = 1/gm₂ ≈ 14 kΩ
PM increase to 65° (OK!)
```

### Final Design
- Itail = 80 μA
- Cc = 3 pF
- Rz = 14 kΩ
- Power = 80μA × 2 × 1.8V ≈ 0.3 mW (OK!)

---

## Tóm Tắt: 10 Điểm Cốt Lõi Không Thể Thiếu

1. **Gain = gm × rout** → Tăng bằng cascode
2. **GBW = gm/Cc** → Trade-off với stability
3. **SR = Itail/Cc** → Check large-signal performance
4. **PM > 60°** → p₂ > 2.2×GBW
5. **Matching** → W×L lớn, common-centroid layout
6. **Offset ∝ 1/√(W×L)** → Precision cần area lớn
7. **CMRR** → Dùng cascode tail current
8. **PSRR** → Cascode mirrors + regulated bias
9. **Trade-offs** → Speed/Power, Gain/Swing, Stability/Speed
10. **Simulation** → Verify corners, Monte Carlo, post-layout

---

**Kết luận**: Thiết kế OPAMP là nghệ thuật cân bằng các trade-off. Không có thiết kế "hoàn hảo", chỉ có thiết kế "tối ưu cho ứng dụng cụ thể". Hiểu rõ các điểm cốt lõi trên sẽ giúp bạn đưa ra quyết định thiết kế đúng đắn.
