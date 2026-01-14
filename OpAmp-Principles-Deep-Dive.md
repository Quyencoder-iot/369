# Nguyên Lý Khuếch Đại Op-Amp: Từ Nguyên Tử Đến Mạch Tích Hợp

## Mục Lục
1. [Giới Thiệu](#1-giới-thiệu)
2. [Cấp Độ Nguyên Tử - Vật Lý Bán Dẫn](#2-cấp-độ-nguyên-tử---vật-lý-bán-dẫn)
3. [Cấp Độ Transistor - Nguyên Lý Khuếch Đại](#3-cấp-độ-transistor---nguyên-lý-khuếch-đại)
4. [Cấp Độ CMOS - Ghép Nối Transistor](#4-cấp-độ-cmos---ghép-nối-transistor)
5. [Kiến Trúc Op-Amp Hoàn Chỉnh](#5-kiến-trúc-op-amp-hoàn-chỉnh)
6. [Các Tầng Khuếch Đại Trong Op-Amp](#6-các-tầng-khuếch-đại-trong-op-amp)

---

## 1. Giới Thiệu

**Op-Amp (Operational Amplifier)** là bộ khuếch đại vi sai có hệ số khuếch đại rất cao (10⁵ - 10⁶), được sử dụng rộng rãi trong xử lý tín hiệu analog. Để hiểu sâu nguyên lý hoạt động, ta cần phân tích từ cấp độ nhỏ nhất.

```
                    ┌─────────────────┐
    V+ (non-inv) ──►│                 │
                    │     Op-Amp      ├──► Vout = A(V+ - V-)
    V- (inv)     ──►│                 │
                    └─────────────────┘
```

---

## 2. Cấp Độ Nguyên Tử - Vật Lý Bán Dẫn

### 2.1 Cấu Trúc Tinh Thể Silicon

**Silicon (Si)** là nguyên tố thuộc nhóm IV với **4 electron hóa trị**:

```
Cấu hình electron của Si: 1s² 2s² 2p⁶ 3s² 3p²

         Lớp hóa trị (n=3)
              │
    Si: [Ne] 3s² 3p²
              │    │
              └────┴── 4 electron tham gia liên kết cộng hóa trị
```

Trong tinh thể Silicon tinh khiết:
- Mỗi nguyên tử Si liên kết với 4 nguyên tử lân cận qua **liên kết cộng hóa trị**
- Tạo thành cấu trúc **kim cương lập phương (diamond cubic)**

```
Cấu trúc mạng tinh thể Silicon (2D đơn giản hóa):

    Si ─── Si ─── Si ─── Si
    │      │      │      │
    Si ─── Si ─── Si ─── Si
    │      │      │      │
    Si ─── Si ─── Si ─── Si

    ─── : Liên kết cộng hóa trị (2 electron chia sẻ)
```

### 2.2 Lý Thuyết Vùng Năng Lượng (Band Theory)

Khi các nguyên tử Si kết hợp thành tinh thể, các mức năng lượng rời rạc mở rộng thành **vùng năng lượng**:

```
Năng lượng (E)
    ▲
    │   ╔════════════════════════╗ ← Vùng dẫn (Conduction Band)
    │   ║    Electron tự do      ║
    │   ╚════════════════════════╝
    │         │
    │         │ Eg ≈ 1.12 eV (vùng cấm - Band Gap)
    │         │
    │   ╔════════════════════════╗ ← Vùng hóa trị (Valence Band)
    │   ║  Electron liên kết     ║
    │   ╚════════════════════════╝
    │
    └──────────────────────────────►
```

**Ý nghĩa vật lý:**
- **Eg (Band Gap)**: Năng lượng tối thiểu để electron "nhảy" từ vùng hóa trị lên vùng dẫn
- Silicon có Eg ≈ 1.12 eV ở 300K → **bán dẫn nội tại** (intrinsic semiconductor)

### 2.3 Doping - Pha Tạp Bán Dẫn

#### 2.3.1 Bán Dẫn Loại N (N-type)

**Pha tạp với nguyên tố nhóm V** (P, As, Sb - có 5 electron hóa trị):

```
Nguyên tử Phosphorus (P) trong mạng Si:

    Si ─── Si ─── Si
    │      │      │
    Si ─── P ─── Si    ← P có 5e⁻, chỉ cần 4 để liên kết
    │      │•     │       • = electron dư (electron donor)
    Si ─── Si ─── Si

Mức năng lượng:
    ▲
    │   ╔═══════════════╗  Vùng dẫn
    │   ╚═══════════════╝
    │         ↑ 
    │   ─ ─ ─●─ ─ ─ ─   ← Mức donor (Ed) - rất gần vùng dẫn
    │         │            Ed ≈ 0.045 eV dưới Ec
    │         │ Eg
    │   ╔═══════════════╗  Vùng hóa trị
    │   ╚═══════════════╝
    └──────────────────►
```

**Cơ chế dẫn điện:**
- Electron thứ 5 của P chỉ cần **~0.045 eV** để ion hóa
- Ở nhiệt độ phòng (kT ≈ 0.026 eV), hầu hết đã ion hóa
- **Electron là hạt tải điện đa số (majority carrier)**
- Lỗ trống là hạt tải điện thiểu số

#### 2.3.2 Bán Dẫn Loại P (P-type)

**Pha tạp với nguyên tố nhóm III** (B, Al, Ga - có 3 electron hóa trị):

```
Nguyên tử Boron (B) trong mạng Si:

    Si ─── Si ─── Si
    │      │      │
    Si ─── B ─── Si    ← B có 3e⁻, thiếu 1 electron
    │      ○      │       ○ = lỗ trống (hole)
    Si ─── Si ─── Si

Mức năng lượng:
    ▲
    │   ╔═══════════════╗  Vùng dẫn
    │   ╚═══════════════╝
    │         │
    │         │ Eg
    │   ─ ─ ─○─ ─ ─ ─   ← Mức acceptor (Ea) - rất gần vùng hóa trị
    │         ↓            Ea ≈ 0.045 eV trên Ev
    │   ╔═══════════════╗  Vùng hóa trị
    │   ╚═══════════════╝
    └──────────────────►
```

**Cơ chế dẫn điện:**
- Electron từ vùng hóa trị dễ dàng "lấp" vào mức acceptor
- Để lại **lỗ trống** trong vùng hóa trị
- **Lỗ trống là hạt tải điện đa số**
- Lỗ trống di chuyển = electron di chuyển theo hướng ngược lại

### 2.4 Tiếp Giáp P-N (P-N Junction) - Nền Tảng Của Transistor

```
         Vùng nghèo (Depletion Region)
              ←───────────►
    ┌─────────┬───────────┬─────────┐
    │    P    │     │     │    N    │
    │   ○○○   │  ⊖  │  ⊕  │   •••   │
    │   ○○○   │  ⊖  │  ⊕  │   •••   │
    │   ○○○   │     │     │   •••   │
    └─────────┴───────────┴─────────┘
         ○ = lỗ trống (mobile)
         • = electron (mobile)
         ⊖ = ion acceptor (cố định, mang điện âm)
         ⊕ = ion donor (cố định, mang điện dương)
```

**Quá trình hình thành tiếp giáp:**

1. **Khuếch tán**: Electron từ N khuếch tán sang P, lỗ trống từ P sang N
2. **Tạo vùng nghèo**: Các ion cố định tạo điện trường nội tại
3. **Cân bằng**: Điện trường cản trở khuếch tán tiếp → trạng thái cân bằng
4. **Điện thế tiếp xúc (Built-in Voltage)**: Vbi ≈ 0.7V cho Si

```
Điện trường và điện thế trong vùng nghèo:

Điện trường E(x):
    │
    │     ╱╲
    │    ╱  ╲
────┼───╱────╲─────────► x
    │  P  │  N
    │     │
    
Điện thế V(x):
    │         ┌──────
    │        ╱
────┼───────╱─────────► x
    │  P   │   N
    │──────┘
         Vbi ≈ 0.7V
```

---

## 3. Cấp Độ Transistor - Nguyên Lý Khuếch Đại

### 3.1 Transistor BJT (Bipolar Junction Transistor)

#### 3.1.1 Cấu Trúc NPN

```
                      Collector (N)
                           │
    ┌──────────────────────┼──────────────────────┐
    │                      │                      │
    │    N (Collector)     │    Nồng độ pha tạp   │
    │    ├────────────────────────────────────►   │
    │    │ Nc (thấp nhất)                         │
    ├────┼────────────────────────────────────────┤
    │    P (Base) - rất mỏng (~1μm)               │
    │    │ Nb (trung bình)                        │
    ├────┼────────────────────────────────────────┤
    │    N (Emitter)                              │
    │    │ Ne (cao nhất) → phát xạ electron mạnh  │
    └────┼────────────────────────────────────────┘
         │
      Emitter (N)
```

#### 3.1.2 Nguyên Lý Hoạt Động - Chế Độ Khuếch Đại

```
Sơ đồ hoạt động NPN (chế độ active):

    Vcc (+)
      │
      RL (tải)
      │
      ├───────────► Vout
      │
      C ◄─────┐
              │
    ──►─┤     │ ← Tiếp giáp B-C: phân cực ngược
    Ic  │     │    (hút electron từ base)
        │  B  │
    ──►─┤     │ ← Tiếp giáp B-E: phân cực thuận
    Ib  │     │    (tiêm electron vào base)
        │     │
      E ◄─────┘
      │
     ─┴─
     GND

Dòng điện:
    Ic = β × Ib    (β = 100-300, hệ số khuếch đại dòng)
    Ie = Ic + Ib ≈ Ic
```

**Giải thích ở cấp độ nguyên tử:**

```
Bước 1: Phân cực thuận B-E
────────────────────────────
        │ Emitter (N⁺)│ Base (P) │
        │  •••••••    │   ○○○    │
        │  •••••••  ──►──►──►    │  ← Electron được "tiêm" vào Base
        │  •••••••    │   ○○○    │
        └─────────────┴──────────┘
        Vbe ≈ 0.7V

Bước 2: Electron di chuyển qua Base mỏng
────────────────────────────────────────
        │ Base (P) - rất mỏng │
        │    ←───────────     │
        │    Độ dày < 1μm     │
        │    •••••••••••••    │  ← Phần lớn electron (99%) không kịp
        │    ─────────────►   │     tái hợp với lỗ trống
        └─────────────────────┘

Bước 3: Collector hút electron
──────────────────────────────
        │ Base │ Collector (N) │
        │      │      ┌────────│
        │ •••• │ ──►──│ Vùng   │  ← Điện trường mạnh của tiếp giáp
        │ •••• │ ──►──│ nghèo  │     B-C phân cực ngược HÚT electron
        │      │      └────────│
        └──────┴───────────────┘
        Điện trường E hướng từ N sang P
```

**Kết quả:**
- 1% electron tái hợp trong Base → tạo Ib
- 99% electron được hút sang Collector → tạo Ic
- **Ic ≈ 100 × Ib** (hệ số khuếch đại β)

### 3.2 Transistor MOSFET

#### 3.2.1 Cấu Trúc NMOS

```
                   Gate (G)
                     │
              ┌──────┴──────┐
              │   Metal     │ ← Cực cổng (kim loại hoặc polysilicon)
              ├─────────────┤
              │    SiO₂     │ ← Lớp oxide cách điện (~2-10nm)
    ┌─────────┴─────────────┴─────────┐
    │         │   Channel   │         │
    │   N⁺    │      ↓      │   N⁺    │ ← Substrate loại P
    │ Source  │  ○ ○ ○ ○ ○  │  Drain  │
    │   (S)   │    P-sub    │   (D)   │
    └─────────┴─────────────┴─────────┘
                    │
                   Body (B)
```

#### 3.2.2 Nguyên Lý Hoạt Động - Hiệu Ứng Trường

```
Trạng thái OFF (Vgs < Vth):
──────────────────────────────
    Gate ─────────────────────
          ║ SiO₂ ║
    ──────║──────║──────
    │  N⁺ │  P   │  N⁺  │    ← Không có kênh dẫn
    │ S   │ ○○○○ │  D   │      Hai tiếp giáp N-P ngược cản dòng
    └─────┴──────┴──────┘

Trạng thái ON (Vgs > Vth):
──────────────────────────────
    Gate ───(+)───────────────
          ║     ║
          ║ E↓  ║              ← Điện trường hướng xuống
    ──────║─────║──────
    │  N⁺ │•••••│  N⁺  │    ← Kênh đảo nghịch (inversion layer)
    │ S   │•••••│  D   │      Electron bị hút lên bề mặt
    └─────┴─────┴──────┘       tạo thành "cầu" nối S và D
              │
         Kênh dẫn N
```

**Giải thích chi tiết ở cấp độ nguyên tử:**

```
Bước 1: Tích tụ điện tích (Vgs > 0)
───────────────────────────────────
    Gate (+)
    ──┬──┬──┬──┬──┬──
      │  │  │  │  │   ← Điện tích dương trên Gate
    ──────────────────
    │     SiO₂      │   ← Lớp cách điện ngăn dòng điện
    ──────────────────      nhưng cho phép điện trường
    ↓  ↓  ↓  ↓  ↓  ↓   ← Điện trường E xuyên qua oxide
    ──────────────────
    │○ ○ ○ ○ ○ ○ ○  │   ← Lỗ trống trong P-sub bị đẩy ra
    │      P-sub    │
    
Bước 2: Hình thành vùng nghèo (Vgs tăng)
─────────────────────────────────────────
    Gate (+)
    ──────────────────
    │     SiO₂      │
    ──────────────────
    │ ⊖ ⊖ ⊖ ⊖ ⊖ ⊖  │   ← Ion acceptor cố định (điện tích âm)
    │───────────────│      Vùng nghèo mở rộng
    │○ ○ ○ ○ ○ ○ ○  │   ← Lỗ trống bị đẩy sâu hơn
    │      P-sub    │

Bước 3: Đảo nghịch (Vgs > Vth)
───────────────────────────────
    Gate (+++)
    ──────────────────
    │     SiO₂      │
    ──────────────────
    │• • • • • • • •│   ← KÊNH ĐẢO NGHỊCH (inversion channel)
    │───────────────│      Electron thiểu số bị hút lên bề mặt
    │ ⊖ ⊖ ⊖ ⊖ ⊖ ⊖  │      Mật độ electron > mật độ lỗ trống
    │      P-sub    │      → Bề mặt chuyển thành loại N!
```

**Điều kiện ngưỡng Vth:**

```
Vth = Vfb + 2φf + (Qd/Cox)

Trong đó:
- Vfb: Điện áp flat-band (bù sai lệch công thoát)
- φf: Fermi potential = (kT/q)ln(Na/ni)
- Qd: Điện tích vùng nghèo
- Cox: Điện dung oxide = εox/tox
```

### 3.3 So Sánh Khuếch Đại BJT vs MOSFET

```
┌───────────────────┬────────────────────┬────────────────────┐
│     Đặc tính      │        BJT         │       MOSFET       │
├───────────────────┼────────────────────┼────────────────────┤
│ Điều khiển bởi    │   Dòng điện (Ib)   │   Điện áp (Vgs)    │
│ Hạt tải           │   Cả 2 loại        │   1 loại (unipolar)│
│ Trở kháng vào     │   Thấp (~kΩ)       │   Rất cao (~TΩ)    │
│ Tốc độ đóng ngắt  │   Nhanh            │   Rất nhanh        │
│ Tiêu thụ công suất│   Cao hơn          │   Thấp hơn         │
│ Tuyến tính        │   Tốt hơn          │   Kém hơn          │
│ Nhiễu             │   Thấp hơn         │   Cao hơn (1/f)    │
└───────────────────┴────────────────────┴────────────────────┘
```

---

## 4. Cấp Độ CMOS - Ghép Nối Transistor

### 4.1 Cấu Trúc CMOS Cơ Bản

**CMOS = Complementary MOS** = Kết hợp NMOS và PMOS

```
                    Vdd
                     │
              ┌──────┴──────┐
              │    PMOS     │
    Vin ──────┤             ├────────┬───── Vout
              │    NMOS     │        │
              └──────┬──────┘        │
                     │               │
                    GND             CL
                                    │
                                   GND

Bảng trạng thái:
┌───────┬───────┬───────┬────────┐
│  Vin  │ PMOS  │ NMOS  │  Vout  │
├───────┼───────┼───────┼────────┤
│  LOW  │  ON   │  OFF  │  HIGH  │
│ HIGH  │  OFF  │  ON   │  LOW   │
└───────┴───────┴───────┴────────┘
```

### 4.2 PMOS - Transistor Kênh P

```
Cấu trúc PMOS:
                    Source (S)
                       │
    ┌──────────────────┼──────────────────┐
    │                  │                  │
    │      P⁺         │        P⁺        │
    │    Source    ───┼───    Drain      │
    │              │Channel│              │
    │      ←──────┼──•••──┼──────→       │
    │              │      │              │
    │             N-well (đế loại N)      │
    └──────────────────┴──────────────────┘
                       │
                     Gate

Hoạt động:
- Vgs < Vth (Vth < 0 cho PMOS): Kênh P được hình thành
- Dòng lỗ trống chảy từ Source → Drain
- PMOS dẫn khi Gate = LOW
```

**So sánh NMOS và PMOS:**

```
                NMOS                           PMOS
    ┌────────────────────┐         ┌────────────────────┐
    │   D               │         │   S               │
    │   │               │         │   │               │
    │   │/──┤           │         │───│\──┤           │
    │   │   │           │         │   │   │           │
    │   │\──┤           │         │   │/──┤           │
    │   │               │         │   │  ○            │
    │   S               │         │   D               │
    └────────────────────┘         └────────────────────┘
    
    Vth > 0                        Vth < 0
    Dẫn khi Vgs > Vth              Dẫn khi Vgs < Vth
    Kênh electron                  Kênh lỗ trống
    Nằm trong P-substrate          Nằm trong N-well
```

### 4.3 CMOS Inverter - Phân Tích Chi Tiết

```
Sơ đồ mạch:
                    Vdd
                     │
                     │
              ┌──────┴──────┐
              │             │
              │    PMOS     ├──┐
              │    (Mp)     │  │
              │             │  │
              └──────┬──────┘  │
                     │         │
    Vin ─────────────┼─────────┼───── Vout
                     │         │
              ┌──────┴──────┐  │
              │             │  │
              │    NMOS     ├──┘
              │    (Mn)     │
              │             │
              └──────┬──────┘
                     │
                    GND
```

**Đặc tuyến truyền đạt (Voltage Transfer Characteristic):**

```
Vout
  │
Vdd├──────╮
  │       │╲
  │       │ ╲
  │       │  ╲         Vùng chuyển tiếp
Vdd├───────│───●───────(cả 2 MOS dẫn)
/2 │       │   │╲
  │       │   │ ╲
  │       │   │  ╲
  0├───────┴───┴───╲────────────────
  0      Vdd/2    Vdd              Vin
         
    Vùng 1   │ Vùng 2 │   Vùng 3
    PMOS ON  │ Cả 2   │  NMOS ON
    NMOS OFF │  dẫn   │  PMOS OFF
```

### 4.4 CMOS Differential Pair - Cặp Vi Sai

Đây là **khối xây dựng cơ bản** của Op-Amp!

```
                    Vdd
                     │
         ┌───────────┼───────────┐
         │           │           │
         RL1         │          RL2
         │           │           │
         ├───────────┼───────────┤
         │                       │
    Vout1●───────────────────────●Vout2
         │                       │
    ┌────┴────┐           ┌────┴────┐
    │   PMOS  │           │   PMOS  │
    │   (M1)  │           │   (M2)  │
    └────┬────┘           └────┬────┘
         │                     │
    V+ ──┴──────────┬──────────┴── V-
                    │
                    │
              ┌─────┴─────┐
              │   NMOS    │
              │   (M3)    │  ← Nguồn dòng đuôi (tail current)
              │   Ibias   │
              └─────┬─────┘
                    │
                   GND
```

**Nguyên lý hoạt động:**

```
Trường hợp 1: V+ = V- (tín hiệu chung - common mode)
────────────────────────────────────────────────────
    Itail chia đều cho M1 và M2:
    
    I1 = I2 = Itail/2
    
    Vout1 = Vout2 = Vdd - (Itail/2) × RL
    
    → Tín hiệu ra vi sai = 0

Trường hợp 2: V+ > V- (tín hiệu vi sai)
──────────────────────────────────────────
    M1 dẫn nhiều hơn, M2 dẫn ít hơn:
    
    I1 > Itail/2,  I2 < Itail/2
    
    Nhưng: I1 + I2 = Itail (không đổi!)
    
    → Vout1 giảm (I1 tăng → điện áp rơi trên RL1 tăng)
    → Vout2 tăng (I2 giảm → điện áp rơi trên RL2 giảm)
    
    Vout = Vout2 - Vout1 > 0
```

**Phương trình khuếch đại:**

```
Đối với MOSFET trong vùng bão hòa:
    Id = (μnCox/2)(W/L)(Vgs - Vth)²

Độ dẫn tương hỗ (transconductance):
    gm = ∂Id/∂Vgs = μnCox(W/L)(Vgs - Vth) = √(2μnCox(W/L)Id)

Hệ số khuếch đại vi sai:
    Ad = gm × (RL1 || ro1)
    
    Với ro = 1/(λId) là trở kháng ra của MOSFET
```

---

## 5. Kiến Trúc Op-Amp Hoàn Chỉnh

### 5.1 Kiến Trúc Two-Stage Op-Amp (Phổ biến nhất)

```
                                    Vdd
                                     │
        ┌────────────────────────────┼────────────────────────────┐
        │                            │                            │
        │    Tầng 1                  │         Tầng 2             │
        │    (Differential          │        (Common-Source      │
        │     Amplifier)            │         Amplifier)         │
        │                            │                            │
        │         M3        M4       │            M6              │
        │        ─┴─      ─┴─       │           ─┴─              │
        │        |||      |||       │           |||              │
        │         │        │         │            │               │
        │         ├────────┼─────────┼────────────┼───────────────┼──► Vout
        │         │        │         │            │               │
        │      ┌──┴──┐  ┌──┴──┐     │         ┌──┴──┐            │
        │      │ M1  │  │ M2  │     │         │ M7  │            │
        │      └──┬──┘  └──┬──┘     │         └──┬──┘            │
        │         │        │         │            │               │
        │    V+ ──┘        └── V-   │            │               │
        │              │             │            │               │
        │           ┌──┴──┐         │            │               │
        │           │ M5  │         │            │               │
        │           └──┬──┘         │            │               │
        │              │             │            │               │
        └──────────────┼─────────────┴────────────┼───────────────┘
                       │                          │
                      GND                        GND
```

### 5.2 Phân Tích Từng Khối

#### Khối 1: Cặp Vi Sai Đầu Vào (M1, M2)

```
                    │
        ┌───────────┼───────────┐
        │           │           │
        │          │           │
        M3 ◄───────┤           ├───────► M4
        │           │           │         (Active Load)
        │           │           │
        ├───────────●───────────┤
        │           │           │
     ┌──┴──┐     Vbias      ┌──┴──┐
     │ M1  │        │        │ M2  │
     └──┬──┘        │        └──┬──┘
        │           │           │
   V+ ──┘     ┌─────┴─────┐    └── V-
              │    M5     │
              │  (Tail)   │
              └─────┬─────┘
                    │
                   GND

Chức năng:
- M1, M2: Khuếch đại tín hiệu vi sai
- M3, M4: Tải tích cực (active load) - tăng hệ số khuếch đại
- M5: Nguồn dòng đuôi - ổn định điểm làm việc

Hệ số khuếch đại tầng 1:
    A1 = gm1 × (ro2 || ro4)
    
    Với tải tích cực: A1 ≈ gm1 × (ro2 || ro4) >> gm1 × RL
    (cao hơn nhiều so với tải điện trở thụ động)
```

#### Khối 2: Current Mirror (Gương Dòng - M3, M4)

```
                    Vdd
                     │
         ┌───────────┼───────────┐
         │           │           │
      ┌──┴──┐     ┌──┴──┐     ┌──┴──┐
      │ M3  │     │ M4  │     │ M6  │
      │     │◄────┤     │◄────┤     │
      └──┬──┘     └──┬──┘     └──┬──┘
         │           │           │
         │←──────────│           │
         │   Sao chép dòng      │
         │                       ▼
        GND                   I6 = I3 × (W6/L6)/(W3/L3)

Nguyên lý:
1. M3 được nối diode (Gate nối Drain)
2. Vgs3 = Vgs4 (Gate chung)
3. Nếu W/L giống nhau: I3 = I4
4. Có thể scale dòng bằng tỷ lệ W/L
```

#### Khối 3: Common-Source Amplifier (Tầng khuếch đại áp - M6, M7)

```
                    Vdd
                     │
              ┌──────┴──────┐
              │     M6      │
              │   (PMOS)    │
              └──────┬──────┘
                     │
    Từ tầng 1 ──────●───────────► Vout
                     │
              ┌──────┴──────┐
              │     M7      │
              │   (NMOS)    │  ← Nguồn dòng bias
              └──────┬──────┘
                     │
                    GND

Hệ số khuếch đại tầng 2:
    A2 = gm6 × (ro6 || ro7)

Tổng hệ số khuếch đại:
    A_total = A1 × A2 = [gm1(ro2||ro4)] × [gm6(ro6||ro7)]
    
    Thường đạt: A_total ≈ 10⁴ - 10⁶
```

### 5.3 Mạch Bù Tần Số (Frequency Compensation)

```
                    Vdd
                     │
              ┌──────┴──────┐
              │     M6      │
              └──────┬──────┘
                     │
    Từ tầng 1 ──────●─────┬─────► Vout
                     │     │
                    ─┴─   │
                Cc  ───    │    ← Tụ Miller (compensation capacitor)
                    ─┬─   │       Cc ≈ vài pF
                     │     │
              ┌──────┴─────┘
              │     M7      │
              └──────┬──────┘
                     │
                    GND

Hiệu ứng Miller:
    C_miller = Cc × (1 + |A2|)
    
    Tạo cực dominant pole tại:
    fp1 = 1/(2π × Rout1 × C_miller)
    
    Đẩy cực non-dominant xa hơn:
    fp2 = gm6/(2π × CL)
```

---

## 6. Các Tầng Khuếch Đại Trong Op-Amp

### 6.1 Tóm Tắt Chuỗi Khuếch Đại

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   INPUT     │    │   TẦNG 1    │    │   TẦNG 2    │    │   OUTPUT    │
│   STAGE     │───►│ Differential│───►│   Voltage   │───►│   BUFFER    │
│             │    │    Amp      │    │    Amp      │    │  (optional) │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
       │                  │                  │                  │
       ▼                  ▼                  ▼                  ▼
   Trở kháng         Khuếch đại        Khuếch đại         Trở kháng
   vào cao         dòng → áp           áp lớn           ra thấp
   (MΩ - TΩ)       A1 ≈ 100-1000      A2 ≈ 100-1000        (<100Ω)
```

### 6.2 Đặc Tính Khuếch Đại Của Từng Cấu Hình

#### Common-Source (CS) - Khuếch đại điện áp

```
        Vdd
         │
         RL
         │
    ●────┼────► Vout
    │    │
    │ ┌──┴──┐
    │ │ M1  │
    │ └──┬──┘
Vin─┘    │
        ─┴─
        GND

Đặc tính:
- Av = -gm × RL (đảo pha)
- Rin = ∞ (MOSFET)
- Rout = RL || ro
- Băng thông bị giới hạn bởi hiệu ứng Miller
```

#### Common-Drain (CD) / Source Follower - Đệm điện áp

```
        Vdd
         │
    ●────┼─── Vin
    │    │
    │ ┌──┴──┐
    │ │ M1  │
    │ └──┬──┘
    │    │
    └────┼────► Vout
         │
         RL
         │
        ─┴─
        GND

Đặc tính:
- Av ≈ 1 (không đảo pha)
- Rin = ∞
- Rout ≈ 1/gm (thấp!)
- Dùng làm buffer đầu ra
```

#### Common-Gate (CG) - Khuếch đại dòng

```
        Vdd
         │
         RL
         │
    ●────┼────► Vout
    │    │
    │ ┌──┴──┐
    │ │ M1  │◄── Vbias
    │ └──┬──┘
Iin─►    │
        ─┴─
        GND

Đặc tính:
- Ai ≈ 1
- Rin = 1/gm (thấp)
- Rout = RL || ro (cao)
- Dùng trong cascode để tăng bandwidth
```

### 6.3 Cấu Hình Cascode - Tăng Hệ Số Khuếch Đại

```
        Vdd
         │
         RL
         │
    ●────┼────► Vout
    │    │
    │ ┌──┴──┐
    │ │ M2  │◄── Vb2 (cascode transistor)
    │ └──┬──┘
    │    │
    │ ┌──┴──┐
    │ │ M1  │
    │ └──┬──┘
Vin─┘    │
        ─┴─
        GND

Lợi ích:
- Rout_cascode = gm2 × ro2 × ro1 >> ro1
- Av_cascode = gm1 × Rout_cascode >> gm1 × ro1
- Giảm hiệu ứng Miller
- Tăng bandwidth
```

### 6.4 Folded-Cascode Op-Amp

```
                                Vdd
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        │    M7          M8      │                        │
        │   ─┴─        ─┴─      │                        │
        │   |||        |||      │                        │
        │    │          │        │                        │
        │    ├──────────┤        │                        │
        │    │          │        │                        │
        │ ┌──┴──┐    ┌──┴──┐    │                        │
        │ │ M5  │    │ M6  │    │                        │
        │ └──┬──┘    └──┬──┘◄───┼── Vout                 │
        │    │          │        │                        │
        │    │          │        │                        │
    ┌───┼────┼──────────┼────────┼────────┐              │
    │   │    │          │        │        │              │
    │ ┌─┴──┐ │        │ ┌──┴─┐ │        │              │
    │ │ M3 │◄┤        ├►│ M4  │ │        │  ← Folded    │
    │ └─┬──┘ │        │ └──┬─┘ │        │    cascode   │
    │   │    │        │    │   │        │              │
    │   │ ┌──┴──┐  ┌──┴──┐ │   │        │              │
    │   │ │ M1  │  │ M2  │ │   │        │              │
    │   │ └──┬──┘  └──┬──┘ │   │        │              │
    │   │    │        │    │   │        │              │
    │   │   V+       V-    │   │        │              │
    │   │         │        │   │        │              │
    │   │    ┌────┴────┐   │   │        │              │
    │   │    │   M9    │   │   │        │              │
    │   │    └────┬────┘   │   │        │              │
    │   │         │        │   │        │              │
    └───┼─────────┼────────┼───┼────────┘              │
        │         │        │   │                        │
       GND       GND      GND GND                      │
                                                        │
        └────────────────────────────────────────────────┘

Ưu điểm:
- Single-stage design → không cần bù Miller
- Hệ số khuếch đại cao: A = gm1 × [gm5×ro5×(ro1||ro3)] || [gm6×ro6×(ro2||ro4)]
- Tốc độ cao (high slew rate)
- Output swing rộng
```

---

## 7. Tổng Kết

### 7.1 Từ Nguyên Tử Đến Op-Amp

```
┌──────────────────────────────────────────────────────────────────────┐
│                         TẦNG NGUYÊN TỬ                               │
│  Si + Dopants (P, B) → Electron/Hole carriers → Dòng điện            │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                         TẦNG TIẾP GIÁP                               │
│  P-N Junction → Vùng nghèo → Điều khiển dòng theo điện áp            │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                        TẦNG TRANSISTOR                               │
│  MOSFET: Điện trường → Kênh đảo nghịch → Khuếch đại dòng/áp          │
│  BJT: Tiêm hạt tải → Khuếch đại dòng                                 │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                          TẦNG CMOS                                   │
│  NMOS + PMOS → Inverter, Differential Pair, Current Mirror           │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                          TẦNG OP-AMP                                 │
│  Input Stage + Gain Stage + Output Stage → Khuếch đại vi sai 10⁵-10⁶│
└──────────────────────────────────────────────────────────────────────┘
```

### 7.2 Các Thông Số Quan Trọng Của Op-Amp

```
┌────────────────────────────────────────────────────────────────────┐
│                    THÔNG SỐ OP-AMP ĐIỂN HÌNH                       │
├────────────────────┬───────────────────────────────────────────────┤
│ Open-loop gain     │ 10⁵ - 10⁶ (100-120 dB)                        │
│ Input offset       │ < 1mV                                         │
│ Input bias current │ < 1nA (CMOS), < 1μA (BJT)                     │
│ Input impedance    │ > 1MΩ (BJT), > 10¹²Ω (CMOS)                   │
│ Output impedance   │ < 100Ω                                        │
│ CMRR               │ > 80dB                                        │
│ PSRR               │ > 80dB                                        │
│ Slew rate          │ 1-100 V/μs                                    │
│ GBW                │ 1-100 MHz                                     │
└────────────────────┴───────────────────────────────────────────────┘
```

---

## Tài Liệu Tham Khảo

1. **Razavi, B.** - "Design of Analog CMOS Integrated Circuits"
2. **Gray, P.R., et al.** - "Analysis and Design of Analog Integrated Circuits"
3. **Allen, P.E. & Holberg, D.R.** - "CMOS Analog Circuit Design"
4. **Sedra, A.S. & Smith, K.C.** - "Microelectronic Circuits"
5. **Streetman, B.G.** - "Solid State Electronic Devices"

---

*Tài liệu được biên soạn nhằm giải thích chi tiết nguyên lý khuếch đại từ cấp độ nguyên tử đến mạch tích hợp.*
