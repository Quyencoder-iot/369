# Op-Amp (Operational Amplifier) - Nguyên Lý Cốt Lõi và Ứng Dụng

## Mục Lục
1. [Giới thiệu tổng quan](#1-giới-thiệu-tổng-quan)
2. [Nguyên lý hoạt động ở cấp độ nguyên tử](#2-nguyên-lý-hoạt-động-ở-cấp-độ-nguyên-tử)
3. [Cấu trúc transistor và CMOS](#3-cấu-trúc-transistor-và-cmos)
4. [Các tầng khuếch đại trong Op-Amp](#4-các-tầng-khuếch-đại-trong-op-amp)
5. [Phân loại Op-Amp](#5-phân-loại-op-amp)
6. [Tiêu chí lựa chọn Op-Amp](#6-tiêu-chí-lựa-chọn-op-amp)
7. [Các ứng dụng của Op-Amp](#7-các-ứng-dụng-của-op-amp)
8. [Bảng tra cứu nhanh](#8-bảng-tra-cứu-nhanh)

---

## 1. Giới Thiệu Tổng Quan

### 1.1 Op-Amp là gì?

**Op-Amp (Operational Amplifier)** - Bộ khuếch đại thuật toán là một mạch tích hợp (IC) khuếch đại tín hiệu điện với:

- **Hệ số khuếch đại rất cao** (10⁵ - 10⁶ lần ở DC)
- **Trở kháng vào rất lớn** (lý tưởng: vô cực)
- **Trở kháng ra rất nhỏ** (lý tưởng: 0Ω)
- **Băng thông rộng**

### 1.2 Ký hiệu và chân cơ bản

```
                 +Vcc
                   │
                   │
        ┌──────────┴──────────┐
        │                     │
   V⁻ ──┤─                    │
        │         Op-Amp      ├── Vout
   V⁺ ──┤+                    │
        │                     │
        └──────────┬──────────┘
                   │
                   │
                 -Vcc
```

**Các chân chính:**
- **V⁺ (Non-inverting input)**: Đầu vào không đảo
- **V⁻ (Inverting input)**: Đầu vào đảo  
- **Vout**: Đầu ra
- **+Vcc, -Vcc**: Nguồn cấp (thường ±15V, ±12V, ±5V)

### 1.3 Phương trình cơ bản

```
Vout = A × (V⁺ - V⁻)

Trong đó:
- A: Hệ số khuếch đại vòng hở (Open-loop gain)
- V⁺: Điện áp đầu vào không đảo
- V⁻: Điện áp đầu vào đảo
```

---

## 2. Nguyên Lý Hoạt Động Ở Cấp Độ Nguyên Tử

### 2.1 Vật liệu bán dẫn Silicon (Si)

```
Cấu trúc tinh thể Silicon:

        Si ─── Si ─── Si
        │      │      │
       ═╪═    ═╪═    ═╪═    ← Liên kết cộng hóa trị
        │      │      │
        Si ─── Si ─── Si
        │      │      │
       ═╪═    ═╪═    ═╪═
        │      │      │
        Si ─── Si ─── Si

Mỗi Si có 4 electron hóa trị
Liên kết cộng hóa trị tạo cấu trúc tinh thể ổn định
```

### 2.2 Pha tạp (Doping) - Tạo bán dẫn loại N và P

#### Bán dẫn loại N (N-type)

```
Pha tạp Phosphorus (P) hoặc Arsenic (As) - có 5 electron hóa trị:

        Si ─── Si ─── Si
        │      │      │
       ═╪═    ═╪═    ═╪═
        │      │      │
        Si ─── P ──── Si
        │      │⊖     │      ← Electron tự do (e⁻)
       ═╪═    ═╪═    ═╪═        (Hạt tải đa số)
        │      │      │
        Si ─── Si ─── Si

→ Electron tự do: hạt tải điện đa số
→ Lỗ trống: hạt tải điện thiểu số
```

#### Bán dẫn loại P (P-type)

```
Pha tạp Boron (B) - có 3 electron hóa trị:

        Si ─── Si ─── Si
        │      │      │
       ═╪═    ═╪═    ═╪═
        │      │      │
        Si ─── B ──── Si
        │      ⊕      │      ← Lỗ trống (hole)
       ═╪═    ═╪═    ═╪═        (Hạt tải đa số)
        │      │      │
        Si ─── Si ─── Si

→ Lỗ trống: hạt tải điện đa số
→ Electron: hạt tải điện thiểu số
```

### 2.3 Tiếp giáp P-N (P-N Junction)

```
Khi P và N tiếp xúc:

    Vùng P          │         Vùng N
                    │
   ⊕ ⊕ ⊕ ⊕ ⊕      │      ⊖ ⊖ ⊖ ⊖ ⊖
   ⊕ ⊕ ⊕ ⊕ ⊕   ───┼───   ⊖ ⊖ ⊖ ⊖ ⊖
   ⊕ ⊕ ⊕ ⊕ ⊕      │      ⊖ ⊖ ⊖ ⊖ ⊖
                    │
          ←─────────┼─────────→
              Khuếch tán

Hình thành vùng nghèo (Depletion Region):

    Vùng P    │ Vùng nghèo │    Vùng N
              │            │
   ⊕ ⊕ ⊕ ⊕   │  ⊖│⊕      │   ⊖ ⊖ ⊖ ⊖
   ⊕ ⊕ ⊕ ⊕   │  ⊖│⊕      │   ⊖ ⊖ ⊖ ⊖
   ⊕ ⊕ ⊕ ⊕   │  ⊖│⊕      │   ⊖ ⊖ ⊖ ⊖
              │            │
              │←── E ───→│
              │  Điện trường nội tại
              │  (~0.7V cho Si)
```

### 2.4 Cơ chế khuếch đại ở cấp nguyên tử

```
NGUYÊN LÝ KHUẾCH ĐẠI:

1. Electron trong vùng dẫn có năng lượng cao
2. Một tín hiệu nhỏ điều khiển dòng electron lớn
3. Năng lượng từ nguồn DC được "điều biến" bởi tín hiệu vào

Biểu đồ năng lượng:

Năng lượng
    ↑
    │  ┌─────────────────────┐ Vùng dẫn (Conduction Band)
    │  │  ⊖  ⊖  ⊖  ⊖  ⊖    │ 
    │  └─────────────────────┘
    │         ↑↓ Eg (Band gap ~1.1eV cho Si)
    │  ┌─────────────────────┐ Vùng hóa trị (Valence Band)
    │  │  ⊕  ⊕  ⊕  ⊕  ⊕    │
    │  └─────────────────────┘
    └──────────────────────────→ Vị trí

Khuếch đại xảy ra khi:
- Tín hiệu nhỏ thay đổi điện trường
- Điện trường điều khiển mật độ hạt tải
- Dòng điện lớn từ nguồn bị điều chỉnh
```

---

## 3. Cấu Trúc Transistor và CMOS

### 3.1 Transistor lưỡng cực (BJT - Bipolar Junction Transistor)

#### Cấu trúc NPN

```
                 Collector (C)
                      │
                      │
            ┌─────────┴─────────┐
            │    N (nhẹ pha)    │
            │                   │
    Base ───┤──  P (mạnh pha)  ─┤
    (B)     │    (rất mỏng)     │
            │                   │
            │    N (nặng pha)   │
            └─────────┬─────────┘
                      │
                      │
                 Emitter (E)

Ký hiệu mạch:
        C
        │
        ↓
    B ──┤
        │
        ↓
        E
```

#### Nguyên lý hoạt động BJT

```
Chế độ khuếch đại (Active Mode):

                    Ic (dòng collector)
                      ↓
    ┌─────────────────┼─────────────────┐
    │                 │                 │
    │    N           ││            N    │
    │  Collector  ←──┼┼──→   Collector │
    │             e⁻ ││ e⁻              │
    │─────────────────┼─────────────────│
    │        P        │        P        │
    │      Base    ←──┴──→   Base      │
    │      (mỏng)    Ib      (mỏng)    │
    │─────────────────┬─────────────────│
    │    N           │                 │
    │  Emitter     e⁻↑      Emitter    │
    │                 │                 │
    └─────────────────┼─────────────────┘
                      │
                    Ie (dòng emitter)

Phương trình quan hệ:
┌──────────────────────────────────┐
│  Ic = β × Ib                     │
│  Ie = Ic + Ib ≈ Ic (vì Ib << Ic) │
│  β (hFE) = 50 - 500              │
└──────────────────────────────────┘

Quá trình khuếch đại:
1. Ib nhỏ → phun electron từ E vào B
2. Vùng B mỏng → phần lớn e⁻ đến C
3. Ic lớn = β × Ib → KHUẾCH ĐẠI DÒNG
```

#### Cấu trúc PNP

```
                 Emitter (E)
                      │
                      │
            ┌─────────┴─────────┐
            │    P (nặng pha)   │
            │                   │
    Base ───┤──  N (mạnh pha)  ─┤
    (B)     │    (rất mỏng)     │
            │                   │
            │    P (nhẹ pha)    │
            └─────────┬─────────┘
                      │
                      │
                 Collector (C)

Ký hiệu mạch:
        E
        │
        ↓
    B ──┤
        │
        ↓
        C
```

### 3.2 MOSFET (Metal-Oxide-Semiconductor FET)

#### Cấu trúc NMOS

```
Cấu trúc vật lý:

        Source (S)    Gate (G)    Drain (D)
            │            │            │
            │      ┌─────┴─────┐      │
            │      │   Metal   │      │
            │      ├───────────┤      │
            │      │   SiO₂    │      │ ← Lớp oxide cách điện
            │      └─────┬─────┘      │
    ┌───────┴────────────┼────────────┴───────┐
    │    N⁺              │              N⁺    │
    │  (Source)     ─────┴─────      (Drain)  │ ← Vùng pha tạp N⁺
    │                                         │
    │                P-substrate              │ ← Đế loại P
    │                                         │
    └─────────────────────────────────────────┘

Ký hiệu mạch NMOS:
        D
        │
        ↓
    G ──┤│
        │
        ↓
        S
```

#### Nguyên lý hoạt động NMOS

```
1. Khi Vgs = 0 (OFF):
   
   S          G          D
   │          │          │
   N⁺─────────┴──────────N⁺
   │                     │
   │   P-substrate       │  ← Không có kênh dẫn
   │   (không dẫn)       │
   └─────────────────────┘

2. Khi Vgs > Vth (ON):
   
   S          G (+)       D
   │          │          │
   N⁺─────────┴──────────N⁺
   │     ⊖⊖⊖⊖⊖⊖⊖⊖⊖      │  ← Kênh đảo (inversion channel)
   │   ⊕⊕⊕⊕⊕⊕⊕⊕⊕⊕⊕      │  ← Electron bị hút lên
   │   P-substrate       │
   └─────────────────────┘

   → Kênh N được tạo thành
   → Dòng Id chảy từ D đến S

Phương trình dòng điện:
┌─────────────────────────────────────────────┐
│ Vùng tuyến tính (Vds < Vgs - Vth):          │
│   Id = μₙCox(W/L)[(Vgs-Vth)Vds - Vds²/2]    │
│                                             │
│ Vùng bão hòa (Vds ≥ Vgs - Vth):             │
│   Id = (1/2)μₙCox(W/L)(Vgs-Vth)²            │
└─────────────────────────────────────────────┘

Trong đó:
- μₙ: độ linh động electron
- Cox: điện dung oxide
- W/L: tỷ lệ chiều rộng/chiều dài kênh
- Vth: điện áp ngưỡng
```

#### Cấu trúc PMOS

```
Cấu trúc vật lý:

        Source (S)    Gate (G)    Drain (D)
            │            │            │
            │      ┌─────┴─────┐      │
            │      │   Metal   │      │
            │      ├───────────┤      │
            │      │   SiO₂    │      │
            │      └─────┬─────┘      │
    ┌───────┴────────────┼────────────┴───────┐
    │    P⁺              │              P⁺    │
    │  (Source)     ─────┴─────      (Drain)  │
    │                                         │
    │                N-substrate              │
    │                   (N-well)              │
    └─────────────────────────────────────────┘

Ký hiệu mạch PMOS:
        S
        │
        ○
    G ──┤│
        │
        ↓
        D
```

### 3.3 Cặp CMOS (Complementary MOS)

```
Cấu trúc inverter CMOS cơ bản:

         Vdd (+)
          │
          │
    ┌─────┴─────┐
    │   PMOS    │
    │     │     │
    │     ○     │
Vin─┼─────┤├────┼───→ Vout
    │     │     │
    │   NMOS    │
    │     │     │
    └─────┴─────┘
          │
          │
         GND

Hoạt động:
┌────────────────────────────────────┐
│ Vin = LOW (0V):                    │
│   - PMOS: ON (Vgs = -Vdd)          │
│   - NMOS: OFF (Vgs = 0)            │
│   → Vout = Vdd (HIGH)              │
│                                    │
│ Vin = HIGH (Vdd):                  │
│   - PMOS: OFF (Vgs = 0)            │
│   - NMOS: ON (Vgs = Vdd)           │
│   → Vout = 0V (LOW)                │
└────────────────────────────────────┘
```

---

## 4. Các Tầng Khuếch Đại Trong Op-Amp

### 4.1 Sơ đồ khối tổng quan

```
┌─────────────────────────────────────────────────────────────────────┐
│                         OP-AMP ARCHITECTURE                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐    │
│   │  INPUT   │    │  GAIN    │    │  OUTPUT  │    │ PROTECTION│    │
│   │  STAGE   │───→│  STAGE   │───→│  STAGE   │───→│  STAGE   │    │
│   │          │    │          │    │          │    │          │    │
│   │Differential│  │High Gain │    │Push-Pull │    │Overcurrent│    │
│   │ Amplifier │  │Amplifier │    │ Driver   │    │ Thermal  │    │
│   └──────────┘    └──────────┘    └──────────┘    └──────────┘    │
│        ↑              ↑                                             │
│        │              │                                             │
│   ┌────┴────┐    ┌────┴────┐                                       │
│   │ BIAS    │    │FREQUENCY│                                       │
│   │ CIRCUIT │    │  COMP   │                                       │
│   └─────────┘    └─────────┘                                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 4.2 Tầng đầu vào (Input Stage) - Differential Amplifier

#### Sơ đồ cặp vi sai BJT

```
                        +Vcc
                          │
              ┌───────────┼───────────┐
              │           │           │
             Rc1         │          Rc2
              │           │           │
              ├───────────┴───────────┤
              │                       │
        Vout1 ●                       ● Vout2
              │                       │
              │                       │
        Q1  ──┤                       ├──  Q2
              │\                     /│
              │ \                   / │
    Vin1 ─────┤  ↘               ↙  ├───── Vin2
              │   \             /   │
              │    \           /    │
              │     \         /     │
              │      \       /      │
              │       ↘     ↙       │
              │         │           │
              │         │           │
              │        Iee          │
              │         │           │
              │        ═╧═          │
              │         │           │
                       -Vcc

Phương trình:
┌──────────────────────────────────────────┐
│ Vout = Vout1 - Vout2 = Av × (Vin1 - Vin2)│
│                                          │
│ Av (differential gain) = gm × Rc         │
│                                          │
│ gm = Ic / VT  (VT ≈ 26mV @ 27°C)        │
│                                          │
│ CMRR = Ad / Acm (thường > 80dB)         │
└──────────────────────────────────────────┘
```

#### Sơ đồ cặp vi sai CMOS

```
                        +Vdd
                          │
              ┌───────────┼───────────┐
              │           │           │
             M3          │          M4
           (PMOS)        │        (PMOS)
              │           │           │    ← Current Mirror Load
              ├───────────┴───────────┤
              │                       │
        Vout1 ●                       ● Vout2
              │                       │
              │                       │
             M1                      M2
           (NMOS)                  (NMOS)
              │                       │
    Vin1 ─────┤│                     ├│───── Vin2
              │                       │
              └───────────┬───────────┘
                          │
                         M5
                       (NMOS)     ← Tail Current Source
                          │
                          │
                        -Vss

Ưu điểm CMOS:
┌──────────────────────────────────────┐
│ • Trở kháng vào cực cao (>10¹²Ω)    │
│ • Dòng rò cực nhỏ (pA - fA)         │
│ • Công suất tiêu thụ thấp           │
│ • Phù hợp rail-to-rail input        │
└──────────────────────────────────────┘
```

### 4.3 Tầng khuếch đại (Gain Stage)

#### Common-Emitter với Active Load (BJT)

```
                    +Vcc
                      │
                     Q3
                   (PNP)  ← Active Load (Current Mirror)
                      │
                      ├──────→ Vout
                      │
                     Q2
                   (NPN)  ← Amplifier
                      │
            Vin ──────┤
                      │
                     Re
                      │
                    -Vcc

Gain: Av = -gm × ro  (có thể đạt 1000-10000)
```

#### Common-Source với Active Load (CMOS)

```
                    +Vdd
                      │
                     M2
                   (PMOS)  ← Active Load
                      │
                      ├──────→ Vout
                      │
                     M1
                   (NMOS)  ← Amplifier
                      │
            Vin ──────┤│
                      │
                    -Vss

Gain: Av = -gm1 × (ro1 || ro2)
      Av = -gm1 × (gm2 × ro1 × ro2) với cascode
```

### 4.4 Tầng đầu ra (Output Stage) - Push-Pull

```
Class AB Push-Pull Output Stage:

                    +Vcc
                      │
                     Q1
                   (NPN)
                      │\
    Vin(+) ───────────┤ \
                      │  \
                      ├───────→ Vout
                      │  /
    Vin(-) ───────────┤ /
                      │/
                     Q2
                   (PNP)
                      │
                    -Vcc

CMOS Push-Pull:

                    +Vdd
                      │
                     M1
                   (PMOS)
                      │
    Vin ──────────────┤├
                      │
                      ├──────→ Vout
                      │
                     M2
                   (NMOS)
                      │
                    -Vss

Đặc điểm:
┌────────────────────────────────────┐
│ • Trở kháng ra thấp                │
│ • Khả năng cấp dòng cao           │
│ • Hiệu suất cao (Class AB ~50-78%)│
│ • Méo crossover được giảm thiểu   │
└────────────────────────────────────┘
```

### 4.5 Mạch bù tần số (Frequency Compensation)

```
Vấn đề ổn định:

Gain (dB)
    │
100─┤          ╲
    │           ╲  Open-loop gain
 80─┤            ╲
    │             ╲
 60─┤              ╲
    │               ╲
 40─┤                ╲
    │    Dominant     ╲  Second
 20─┤    pole          ╲ pole
    │      ↓            ↓
  0─┤──────●────────────●───────→ f (Hz)
    │      fp1          fp2
    │     10Hz         1MHz

Bù tần số bằng tụ Miller:

                    ┌───Cc───┐
                    │        │
             ┌──────┴──┐  ┌──┴──────┐
    Vin ────→│ Stage 1 ├──┤ Stage 2 ├────→ Vout
             └─────────┘  └─────────┘

    Cc tạo ra:
    - Di chuyển fp1 xuống tần số thấp hơn
    - Di chuyển fp2 lên tần số cao hơn
    → Phase margin > 45° (thường 60°)
```

### 4.6 Sơ đồ hoàn chỉnh Op-Amp 741 đơn giản hóa

```
                              +Vcc
                                │
    ┌───────────────────────────┼───────────────────────────────┐
    │                           │                               │
    │  ┌──────────────┬─────────┼───────────┬──────────────┐   │
    │  │              │         │           │              │   │
    │  │             Q5        Q6          Q7              │   │
    │  │              │         │           │              │   │
    │  │              ├─────────┴───────────┤              │   │
    │  │              │                     │              │   │
    │  │              │                    Q14             │   │
    │  │              │        ┌────────────┤              │   │
    │  │              │        │            │              │   │
    │  │      Q1     Q3       Q4     Q2     │              │   │
    │  │       │╲    ╱│       │╲    ╱│      │              │   │
V⁺ ─┼──┼───────┤ \  / ├───────┤ \  / ├──────┼──────────────┼── V⁻
    │  │       │  \/  │       │  \/  │      │              │   │
    │  │       │  /\  │       │  /\  │      │      Cc      │   │
    │  │       │ /  \ │       │ /  \ │      │    ┌─┤├─┐    │   │
    │  │       │╱    ╲│       │╱    ╲│      │    │     │    │   │
    │  │              │               │      │    │    Q16   │   │
    │  │              └───────┬───────┘      │    │     │    │   │
    │  │                      │              │    │     ├────┼───→ Vout
    │  │                     Q8              │    │     │    │   │
    │  │                      │              │    └─────┤    │   │
    │  │                      │              │         Q20   │   │
    │  │                      │              │          │    │   │
    │  └──────────────────────┴──────────────┴──────────┴────┘   │
    │                         │                                   │
    └─────────────────────────┼───────────────────────────────────┘
                              │
                            -Vcc

Khối chức năng:
┌────────────────────────────────────────────┐
│ Q1-Q4: Differential input stage            │
│ Q5-Q7: Active load (current mirror)        │
│ Q8: Tail current source                    │
│ Q14, Q16: Gain stage (common-emitter)      │
│ Q20: Output stage (emitter follower)       │
│ Cc: Miller compensation capacitor (~30pF)  │
└────────────────────────────────────────────┘
```

---

## 5. Phân Loại Op-Amp

### 5.1 Phân loại theo công nghệ

```
┌─────────────────────────────────────────────────────────────────────┐
│                    PHÂN LOẠI THEO CÔNG NGHỆ                         │
├──────────────┬──────────────┬──────────────┬───────────────────────┤
│   Loại       │   Đặc điểm   │   Ưu điểm    │      Ứng dụng        │
├──────────────┼──────────────┼──────────────┼───────────────────────┤
│              │ • Tầng vào   │ • GBW cao    │ • Audio amplifier     │
│   BJT        │   dùng BJT   │ • Nhiễu thấp │ • Precision circuits  │
│ (Bipolar)    │ • Ib: nA-μA  │ • Offset nhỏ │ • Instrumentation     │
│              │              │              │                       │
├──────────────┼──────────────┼──────────────┼───────────────────────┤
│              │ • Tầng vào   │ • Zin cực cao│ • Sensor interface    │
│   JFET       │   dùng JFET  │ • Ib: pA     │ • pH meter            │
│              │ • Nhiễu 1/f  │              │ • Photodiode amp      │
│              │   thấp       │              │                       │
├──────────────┼──────────────┼──────────────┼───────────────────────┤
│              │ • Tầng vào   │ • Zin cao nhất│• Electrometer        │
│   CMOS       │   dùng CMOS  │ • Ib: fA     │ • Charge amplifier    │
│              │ • Rail-to-rail│ • Low power │ • Battery devices     │
│              │   khả thi    │              │                       │
├──────────────┼──────────────┼──────────────┼───────────────────────┤
│              │ • JFET input │ • Kết hợp ưu │ • General purpose     │
│   BiFET     │ • BJT output │   điểm 2 loại│ • High impedance src  │
│              │              │              │                       │
├──────────────┼──────────────┼──────────────┼───────────────────────┤
│              │ • BJT input  │ • GBW rất cao│ • Video amplifier     │
│   BiCMOS     │ • CMOS output│ • Low power  │ • High-speed DAC      │
│              │              │              │                       │
└──────────────┴──────────────┴──────────────┴───────────────────────┘
```

### 5.2 Phân loại theo đặc tính

```
┌─────────────────────────────────────────────────────────────────────┐
│                   PHÂN LOẠI THEO ĐẶC TÍNH                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  1. GENERAL PURPOSE (Đa năng)                                       │
│     ├── LM741, LM358, TL072, NE5532                                │
│     ├── GBW: 1-10 MHz                                              │
│     └── Giá rẻ, dễ tìm                                              │
│                                                                     │
│  2. PRECISION (Chính xác cao)                                       │
│     ├── OP07, OP177, AD797, OPA277                                 │
│     ├── Offset: < 100μV                                            │
│     ├── Drift: < 1μV/°C                                            │
│     └── CMRR, PSRR cao                                              │
│                                                                     │
│  3. HIGH-SPEED (Tốc độ cao)                                         │
│     ├── AD8065, OPA656, LMH6629                                    │
│     ├── GBW: > 100 MHz                                             │
│     ├── Slew Rate: > 100 V/μs                                      │
│     └── Settling time ngắn                                          │
│                                                                     │
│  4. LOW-NOISE (Nhiễu thấp)                                          │
│     ├── AD797, OPA227, LT1028                                      │
│     ├── Voltage noise: < 3 nV/√Hz                                  │
│     └── Audio, instrumentation                                      │
│                                                                     │
│  5. LOW-POWER (Công suất thấp)                                      │
│     ├── TLV2462, MCP6001, OPA344                                   │
│     ├── Iq: < 100 μA                                               │
│     └── Battery-powered devices                                     │
│                                                                     │
│  6. RAIL-TO-RAIL (I/O đến nguồn)                                    │
│     ├── OPA340, AD8605, MCP6002                                    │
│     ├── Input/Output swing: 0 to Vcc                               │
│     └── Single supply, low voltage                                  │
│                                                                     │
│  7. HIGH-VOLTAGE (Điện áp cao)                                      │
│     ├── OPA445, PA85, OPA547                                       │
│     ├── Supply: ±25V đến ±150V                                     │
│     └── Industrial, motor control                                   │
│                                                                     │
│  8. HIGH-OUTPUT CURRENT (Dòng ra cao)                               │
│     ├── OPA548, LM675, OPA549                                      │
│     ├── Iout: > 1A                                                 │
│     └── Motor driver, power amplifier                               │
│                                                                     │
│  9. INSTRUMENTATION AMPLIFIER (Khuếch đại đo lường)                 │
│     ├── INA128, AD620, INA333                                      │
│     ├── 3 op-amp in one package                                    │
│     ├── Programmable gain                                          │
│     └── High CMRR (> 100 dB)                                        │
│                                                                     │
│ 10. CHOPPER-STABILIZED (Ổn định chopper)                            │
│     ├── ADA4522, OPA378, LTC2057                                   │
│     ├── Near-zero offset drift                                     │
│     └── DC precision applications                                   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 5.3 Phân loại theo cấu hình nguồn

```
┌─────────────────────────────────────────────────────────────────────┐
│                 PHÂN LOẠI THEO NGUỒN CẤP                            │
├──────────────────────────────┬──────────────────────────────────────┤
│      DUAL SUPPLY             │         SINGLE SUPPLY               │
│      (±Vcc)                  │         (0 to Vcc)                  │
├──────────────────────────────┼──────────────────────────────────────┤
│                              │                                      │
│         +Vcc                 │              +Vcc                    │
│           │                  │                │                     │
│     ┌─────┴─────┐            │         ┌─────┴─────┐               │
│     │           │            │         │           │               │
│  ───┤─  Op-Amp ├───          │      ───┤─  Op-Amp ├───             │
│  ───┤+          │            │      ───┤+          │               │
│     │           │            │         │           │               │
│     └─────┬─────┘            │         └─────┬─────┘               │
│           │                  │               │                     │
│         -Vcc                 │              GND                    │
│                              │                                      │
│ Ví dụ:                       │ Ví dụ:                              │
│ • LM741: ±5V to ±18V         │ • LM358: 3V to 32V                  │
│ • OP07: ±3V to ±18V          │ • MCP6001: 1.8V to 6V               │
│ • TL072: ±3.5V to ±18V       │ • OPA344: 2.5V to 5.5V              │
│                              │                                      │
│ Ưu điểm:                     │ Ưu điểm:                            │
│ • Swing lớn                  │ • Đơn giản nguồn                    │
│ • Xử lý tín hiệu AC dễ       │ • Phù hợp pin                       │
│                              │                                      │
└──────────────────────────────┴──────────────────────────────────────┘
```

---

## 6. Tiêu Chí Lựa Chọn Op-Amp

### 6.1 Các thông số quan trọng

```
┌─────────────────────────────────────────────────────────────────────┐
│                    CÁC THÔNG SỐ QUAN TRỌNG                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 1. GAIN BANDWIDTH PRODUCT (GBW)                              │   │
│  │    - Tích số giữa hệ số khuếch đại và băng thông            │   │
│  │    - GBW = Av × BW = hằng số                                │   │
│  │    - Ví dụ: GBW = 1MHz → Av=10 thì BW=100kHz                │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 2. SLEW RATE (SR)                                            │   │
│  │    - Tốc độ thay đổi điện áp đầu ra tối đa                  │   │
│  │    - SR = dVout/dt (V/μs)                                   │   │
│  │    - Quan trọng cho tín hiệu lớn, tần số cao                │   │
│  │    - fmax = SR / (2π × Vpeak)                               │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 3. INPUT OFFSET VOLTAGE (Vos)                                │   │
│  │    - Điện áp cần đặt vào để Vout = 0                        │   │
│  │    - Vos nhỏ → chính xác cao                                │   │
│  │    - Drift: μV/°C - quan trọng cho DC precision             │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 4. INPUT BIAS CURRENT (Ib)                                   │   │
│  │    - Dòng điện chạy vào các ngõ vào                         │   │
│  │    - BJT: nA-μA, JFET: pA, CMOS: fA                         │   │
│  │    - Ảnh hưởng khi nguồn có trở kháng cao                   │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 5. INPUT OFFSET CURRENT (Ios)                                │   │
│  │    - Sự chênh lệch giữa 2 dòng bias                         │   │
│  │    - Ios = |Ib+ - Ib-|                                      │   │
│  │    - Gây sai số khi nguồn không cân bằng trở kháng          │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 6. CMRR (Common-Mode Rejection Ratio)                        │   │
│  │    - Khả năng loại bỏ tín hiệu đồng pha                     │   │
│  │    - CMRR = Ad / Acm (dB)                                   │   │
│  │    - Thường > 80 dB, precision > 120 dB                     │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 7. PSRR (Power Supply Rejection Ratio)                       │   │
│  │    - Khả năng loại bỏ nhiễu nguồn                           │   │
│  │    - Quan trọng trong môi trường nhiễu                      │   │
│  │    - Giảm theo tần số                                        │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 8. INPUT NOISE                                               │   │
│  │    - Voltage noise: nV/√Hz                                  │   │
│  │    - Current noise: pA/√Hz                                  │   │
│  │    - Quan trọng cho tín hiệu nhỏ                            │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 9. OUTPUT SWING                                              │   │
│  │    - Phạm vi điện áp đầu ra                                 │   │
│  │    - Rail-to-rail: gần bằng Vcc                             │   │
│  │    - Traditional: Vcc - 1V đến -Vcc + 1V                    │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ 10. QUIESCENT CURRENT (Iq)                                   │   │
│  │    - Dòng tiêu thụ khi không tải                            │   │
│  │    - Low-power: < 100μA                                     │   │
│  │    - High-speed: có thể > 10mA                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 6.2 Flowchart lựa chọn Op-Amp

```
                              ┌─────────────┐
                              │   BẮT ĐẦU   │
                              └──────┬──────┘
                                     │
                    ┌────────────────┼────────────────┐
                    ▼                ▼                ▼
            ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
            │   DC/Low     │ │   Audio      │ │  High-Speed  │
            │  Frequency   │ │  (20Hz-20kHz)│ │  (>1MHz)     │
            └──────┬───────┘ └──────┬───────┘ └──────┬───────┘
                   │                │                │
          ┌────────┴────────┐       │        ┌───────┴───────┐
          ▼                 ▼       ▼        ▼               ▼
    ┌──────────┐      ┌──────────┐ ┌──────┐ ┌──────┐   ┌──────────┐
    │ Precision│      │  General │ │ Low  │ │Video │   │ RF/High  │
    │ Required?│      │ Purpose  │ │Noise │ │ Amp  │   │ Frequency│
    └────┬─────┘      └────┬─────┘ └──┬───┘ └──┬───┘   └────┬─────┘
         │                 │          │        │            │
    ┌────┴────┐            │          │        │            │
    ▼         ▼            ▼          ▼        ▼            ▼
┌───────┐ ┌───────┐  ┌──────────┐ ┌───────┐ ┌───────┐ ┌──────────┐
│  YES  │ │   NO  │  │  LM358   │ │AD797  │ │AD8065 │ │ LMH6629  │
│       │ │       │  │  TL072   │ │OPA227 │ │OPA656 │ │ AD8000   │
└───┬───┘ └───┬───┘  │  NE5532  │ │LT1028 │ │       │ │          │
    │         │      └──────────┘ └───────┘ └───────┘ └──────────┘
    │         │
    ▼         ▼
┌───────────────────────────────────────┐
│          KIỂM TRA NGUỒN               │
├───────────────────────────────────────┤
│                                       │
│  ┌─────────────┐    ┌─────────────┐  │
│  │ Single      │    │ Dual Supply │  │
│  │ Supply?     │    │ Available?  │  │
│  └──────┬──────┘    └──────┬──────┘  │
│         │                  │         │
│    ┌────┴────┐        ┌────┴────┐    │
│    ▼         ▼        ▼         ▼    │
│ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ │
│ │Rail- │ │Vref  │ │ OP07 │ │OP177 │ │
│ │to-   │ │Biased│ │AD797 │ │ADA45│ │
│ │Rail  │ │      │ │      │ │22    │ │
│ │OPA34x│ │      │ │      │ │      │ │
│ └──────┘ └──────┘ └──────┘ └──────┘ │
│                                       │
└───────────────────────────────────────┘
```

### 6.3 Ma trận lựa chọn theo ứng dụng

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    MA TRẬN LỰA CHỌN OP-AMP THEO ỨNG DỤNG                    │
├────────────────┬─────────┬────────┬────────┬─────────┬─────────┬───────────┤
│    Ứng dụng    │  Vos    │   Ib   │  GBW   │   SR    │  Noise  │   Gợi ý   │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Thermocouple   │ ●●●●●   │ ●●●    │ ●      │ ●       │ ●●●     │ ADA4522   │
│ Amplifier      │ <10μV   │ <1nA   │        │         │         │ OPA378    │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Photodiode     │ ●●      │ ●●●●●  │ ●●●    │ ●●      │ ●●●●    │ OPA380    │
│ TIA            │         │ <10pA  │ >10MHz │         │         │ AD8615    │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Audio Pre-amp  │ ●●      │ ●●     │ ●●●    │ ●●●     │ ●●●●●   │ NE5532    │
│                │         │        │ >5MHz  │ >10V/μs │ <5nV/√Hz│ OPA1612   │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Video Buffer   │ ●       │ ●      │ ●●●●●  │ ●●●●●   │ ●●      │ AD8065    │
│                │         │        │ >100MHz│ >200V/μs│         │ OPA656    │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ DAC I/V        │ ●●●     │ ●●●●   │ ●●●●   │ ●●●●    │ ●●●     │ OPA627    │
│ Converter      │         │ <1nA   │ >40MHz │ >50V/μs │         │ AD8610    │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Strain Gauge   │ ●●●●●   │ ●●●    │ ●●     │ ●       │ ●●●●    │ INA128    │
│ (Inst. Amp)    │ <25μV   │        │        │         │         │ AD620     │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Active Filter  │ ●●●     │ ●●     │ ●●●    │ ●●●     │ ●●      │ TL072     │
│                │         │        │ >3MHz  │ >10V/μs │         │ OPA2134   │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Battery        │ ●●      │ ●●●    │ ●●     │ ●●      │ ●●      │ MCP6001   │
│ Powered        │         │        │        │         │         │ OPA344    │
│ (Low Power)    │         │ Iq<1mA │        │         │         │ TLV2462   │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Current Sense  │ ●●●●    │ ●●     │ ●●●    │ ●●●     │ ●●●     │ INA181    │
│                │ <100μV  │        │        │         │         │ AD8418    │
├────────────────┼─────────┼────────┼────────┼─────────┼─────────┼───────────┤
│ Oscillator     │ ●●      │ ●●     │ ●●●    │ ●●●●    │ ●●      │ TL072     │
│                │         │        │ >5MHz  │ >13V/μs │         │ OPA2134   │
└────────────────┴─────────┴────────┴────────┴─────────┴─────────┴───────────┘

Chú thích: ● = không quan trọng → ●●●●● = rất quan trọng
```

---

## 7. Các Ứng Dụng Của Op-Amp

### 7.1 Mạch khuếch đại cơ bản

#### Inverting Amplifier (Khuếch đại đảo)

```
                        Rf
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
                  │    │       ││
    Vin ──/\/\/───┼────┤-      ├┼──── Vout
            R1    │    │ Op-Amp│
                  │ ┌──┤+      │
                  │ │  └───────┘
                  │ │
                  │ ╧ GND
                  │
                  └───────────────

Công thức:
┌─────────────────────────────────┐
│  Vout = -(Rf/R1) × Vin          │
│  Av = -Rf/R1                    │
│  Zin = R1                       │
│  Zout ≈ 0 (lý tưởng)           │
└─────────────────────────────────┘

Ứng dụng:
• Audio amplifier
• Active filter
• Integrator/Differentiator
```

#### Non-Inverting Amplifier (Khuếch đại không đảo)

```
                        Rf
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
                  │    │       ││
                  ├────┤-      ├┼──── Vout
                  │    │ Op-Amp│
    Vin ──────────┼────┤+      │
                  │    └───────┘
                  │
                 R1
                  │
                  ╧ GND

Công thức:
┌─────────────────────────────────┐
│  Vout = (1 + Rf/R1) × Vin       │
│  Av = 1 + Rf/R1                 │
│  Zin = rất cao (≈ ∞)           │
│  Zout ≈ 0                       │
└─────────────────────────────────┘

Ứng dụng:
• Sensor amplifier
• Buffer with gain
• Instrumentation front-end
```

#### Voltage Follower (Buffer)

```
                       ┌───────┐
                       │       │
                  ┌────┤-      ├────┬──── Vout
                  │    │ Op-Amp│    │
    Vin ─────────┼────┤+      │    │
                  │    └───────┘    │
                  │                 │
                  └─────────────────┘

Công thức:
┌─────────────────────────────────┐
│  Vout = Vin                     │
│  Av = 1                         │
│  Zin = rất cao                  │
│  Zout = rất thấp                │
└─────────────────────────────────┘

Ứng dụng:
• Impedance matching
• Isolation amplifier
• Driving capacitive loads
```

### 7.2 Mạch toán học

#### Summing Amplifier (Mạch cộng)

```
                              Rf
                        ┌────/\/\/────┐
                        │             │
    V1 ──/\/\/──┐       │    ┌───────┐│
           R1   │       │    │       ││
    V2 ──/\/\/──┼───────┼────┤-      ├┼──── Vout
           R2   │       │    │ Op-Amp│
    V3 ──/\/\/──┘       │ ┌──┤+      │
           R3           │ │  └───────┘
                        │ ╧ GND
                        │

Công thức (R1=R2=R3=R):
┌─────────────────────────────────────────┐
│  Vout = -(Rf/R) × (V1 + V2 + V3)        │
│                                         │
│  Tổng quát:                             │
│  Vout = -Rf × (V1/R1 + V2/R2 + V3/R3)   │
└─────────────────────────────────────────┘

Ứng dụng:
• Audio mixer
• DAC (Digital-to-Analog Converter)
• Weighted sum calculator
```

#### Differential Amplifier (Mạch trừ)

```
                     R2
               ┌────/\/\/────┐
               │             │
    V1 ─/\/\/──┼─────────────┼──── Vout
          R1   │    ┌───────┐│
               └────┤-      ├┘
                    │ Op-Amp│
    V2 ─/\/\/──┬────┤+      │
          R1   │    └───────┘
              R2
               │
               ╧ GND

Công thức (R1=R3, R2=R4):
┌─────────────────────────────────────────┐
│  Vout = (R2/R1) × (V2 - V1)             │
│  Av = R2/R1                             │
└─────────────────────────────────────────┘

Ứng dụng:
• Differential signal amplification
• Common-mode noise rejection
• Current sensing
```

#### Integrator (Mạch tích phân)

```
                        C
                  ┌────┤├────┐
                  │          │
                  │    ┌─────┴─┐
                  │    │       │
    Vin ──/\/\/───┼────┤-      ├──── Vout
            R     │    │ Op-Amp│
                  │ ┌──┤+      │
                  │ │  └───────┘
                  │ ╧ GND

Công thức:
┌─────────────────────────────────────────┐
│  Vout = -(1/RC) × ∫Vin dt               │
│                                         │
│  Với sóng vuông → tam giác              │
│  Với DC → ramp (dốc tuyến tính)         │
└─────────────────────────────────────────┘

Ứng dụng:
• Waveform generator
• Analog computer
• Active low-pass filter
• PID controller (I component)
```

#### Differentiator (Mạch vi phân)

```
                        Rf
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
    Vin ────┤├────┼────┤-      ├┼──── Vout
            C     │    │ Op-Amp│
                  │ ┌──┤+      │
                  │ │  └───────┘
                  │ ╧ GND

Công thức:
┌─────────────────────────────────────────┐
│  Vout = -RC × (dVin/dt)                 │
│                                         │
│  Với tam giác → vuông                   │
│  Với sin → cos (đảo pha)                │
└─────────────────────────────────────────┘

Ứng dụng:
• Edge detection
• High-pass filter
• PID controller (D component)
• Rate of change measurement
```

### 7.3 Mạch so sánh và xử lý tín hiệu

#### Comparator (Mạch so sánh)

```
                    +Vcc
                      │
              ┌───────┴───────┐
              │               │
    Vin ──────┤+              │
              │    Op-Amp     ├──── Vout
    Vref ─────┤-              │
              │               │
              └───────┬───────┘
                      │
                    -Vcc

Hoạt động:
┌─────────────────────────────────────────┐
│  Nếu Vin > Vref → Vout = +Vsat          │
│  Nếu Vin < Vref → Vout = -Vsat          │
└─────────────────────────────────────────┘

Ứng dụng:
• Zero-crossing detector
• Level detector
• Square wave generator
• ADC (flash type)
```

#### Schmitt Trigger (Có trễ)

```
                        R1
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
    Vin ─────────┼────┤-      ├┼──┬── Vout
                  │    │ Op-Amp│  │
                  │ ┌──┤+      │  │
                  │ │  └───────┘  │
                  │ │             │
                  │ └──/\/\/──────┘
                  │      R2
                  │
                  ╧ GND

Ngưỡng:
┌─────────────────────────────────────────┐
│  Vth+ = (R1/(R1+R2)) × Vsat+            │
│  Vth- = (R1/(R1+R2)) × Vsat-            │
│  Hysteresis = Vth+ - Vth-               │
└─────────────────────────────────────────┘

Ứng dụng:
• Noise immunity
• Square wave from noisy signal
• Oscillator
• Debouncing
```

#### Precision Rectifier (Chỉnh lưu chính xác)

```
Half-Wave Rectifier:
                        D1
                  ┌──────►├──────┐
                  │              │
                  │    ┌───────┐ │
    Vin ──/\/\/───┼────┤-      ├─┼──── Vout
            R     │    │ Op-Amp│ │
                  │ ┌──┤+      │ │
                  │ │  └───────┘ │
                  │ ╧ GND        │
                  │              │
                  └──────◄├──────┘
                        D2

Ưu điểm:
┌─────────────────────────────────────────┐
│  • Khắc phục Vf của diode (~0.7V)       │
│  • Chính xác với tín hiệu nhỏ (mV)      │
│  • Dùng cho AC-DC converter chính xác   │
└─────────────────────────────────────────┘

Ứng dụng:
• AC voltage measurement
• Peak detector
• Absolute value circuit
```

### 7.4 Mạch lọc tích cực (Active Filters)

#### Sallen-Key Low-Pass Filter

```
                   R1          R2
    Vin ──/\/\/────┬──/\/\/────┬─────┐
                   │           │     │
                  C1           │     │
                   │           │     │
                   ╧          C2     │
                              │     │
                              │  ┌──┴──┐
                              │  │     │
                              └──┤+    ├─┬── Vout
                                 │Op-Amp│ │
                              ┌──┤-    │ │
                              │  └─────┘ │
                              └──────────┘

Công thức:
┌─────────────────────────────────────────┐
│  fc = 1 / (2π√(R1×R2×C1×C2))            │
│  Q = √(R1×R2×C1×C2) / (C2×(R1+R2))      │
│  Butterworth: Q = 0.707                 │
└─────────────────────────────────────────┘

Ứng dụng:
• Anti-aliasing filter (ADC)
• Audio tone control
• Noise filtering
```

#### State Variable Filter

```
                 ┌─────────────────────────────────────┐
                 │                                     │
    Vin ─/\/\/───┼──┬────┐                            │
                 │  │    │     ┌───────┐              │
                 │  │    └─────┤-      ├──────────────┼── HP (High Pass)
                 │  │     R    │ Op1   │   ┌─────┐    │
                 │  │    ┌─────┤+      │   │     │    │
                 │  │    │     └───────┘   │  C  │    │
                 │  │    │                 │     │    │
                 │  │    └──/\/\/──┬───────┤├────┼────┼── BP (Band Pass)
                 │  │         R   │        │     │    │
                 │  │             │    ┌───┴───┐ │    │
                 │  │             │    │       │ │    │
                 │  │             └────┤-      ├─┼────┼── LP (Low Pass)
                 │  │                  │ Op2   │ │    │
                 │  │             ┌────┤+      │ │    │
                 │  │             │    └───────┘ │    │
                 │  │             │              │    │
                 │  └──/\/\/──────┴──────────────┘    │
                 │        R                           │
                 │                                    │
                 └────────────────────────────────────┘

Đặc điểm:
• Cho đồng thời LP, HP, BP
• Dễ điều chỉnh Q và fc
• Stable performance
```

### 7.5 Mạch dao động (Oscillators)

#### Wien Bridge Oscillator

```
                        Rf
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
                  ├────┤-      ├┼──┬── Vout
                  │    │ Op-Amp│  │
               R1 │ ┌──┤+      │  │
                  │ │  └───────┘  │
                  │ │             │
                  │ │   ┌─R──C─┐  │
                  │ └───┤      ├──┘
                  │     │      │
                  │     C      │
                  │     │      │
                  │     ╧      R
                  │            │
                  │            ╧
                  │
                  └───────────────

Công thức:
┌─────────────────────────────────────────┐
│  fo = 1 / (2πRC)                        │
│  Điều kiện dao động: Rf/R1 = 2          │
│  Dạng sóng: Sin                         │
└─────────────────────────────────────────┘

Ứng dụng:
• Audio signal generator
• Test equipment
• Function generator
```

#### Square Wave Oscillator (Astable Multivibrator)

```
                        R
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
                  ├────┤-      ├┼──┬── Vout
                  │    │ Op-Amp│  │
                  │ ┌──┤+      │  │
                  │ │  └───────┘  │
                  │ │             │
                 C│ │  R1        │
                  │ ├──/\/\/─────┤
                  │ │             │
                  │ │  R2        │
                  │ └──/\/\/─────┘
                  │
                  ╧

Công thức:
┌─────────────────────────────────────────┐
│  T = 2RC × ln(1 + 2R1/R2)               │
│  fo = 1/T                               │
│  Với R1 = R2: fo ≈ 1/(2.2RC)           │
└─────────────────────────────────────────┘

Ứng dụng:
• Clock generator
• Timing circuits
• PWM generator
```

### 7.6 Ứng dụng đặc biệt

#### Instrumentation Amplifier (3 Op-Amp)

```
                             R3
                        ┌───/\/\/───┐
                        │           │
         ┌───────┐      │  ┌───────┐│      ┌───────┐
         │       │  R1  │  │       ││      │       │
    V1 ──┤+      ├─/\/\/┼──┤-      ├┼──────┤-      ├── Vout
         │ Op1   │      │  │ Op3   │       │ Op3   │
         │       ├──────┘  │       │  ┌────┤+      │
         └───┬───┘         └───┬───┘  │    └───────┘
             │                 │      │
            Rg                 │     R4
             │                 │      │
         ┌───┴───┐             │      │
         │       │  R2         │      │
    V2 ──┤+      ├─/\/\/───────┘      │
         │ Op2   │                    │
         │       ├────────────────────┘
         └───────┘

Công thức:
┌─────────────────────────────────────────┐
│  Vout = (V1 - V2) × (1 + 2R1/Rg) × R3/R2│
│                                         │
│  Gain điều chỉnh bởi Rg                 │
│  CMRR rất cao (> 100dB)                 │
└─────────────────────────────────────────┘

Ứng dụng:
• Strain gauge
• Thermocouple
• Biomedical (ECG, EEG)
• Wheatstone bridge
```

#### Current-to-Voltage Converter (Transimpedance Amp)

```
                        Rf
                  ┌────/\/\/────┐
                  │             │
                  │    ┌───────┐│
    Iin ──────────┼────┤-      ├┼──── Vout
                  │    │ Op-Amp│
                  │ ┌──┤+      │
                  │ │  └───────┘
                  │ ╧ GND

Công thức:
┌─────────────────────────────────────────┐
│  Vout = -Iin × Rf                       │
│  Transimpedance = Rf (Ω)                │
└─────────────────────────────────────────┘

Ứng dụng:
• Photodiode amplifier
• DAC current output
• Current measurement
```

#### Voltage-to-Current Converter

```
                    ┌───────────────┐
                    │               │
                    │    ┌───────┐  │
    Vin ───/\/\/────┼────┤-      ├──┤
              R     │    │ Op-Amp│  │
                    │ ┌──┤+      │  │
                    │ │  └───────┘  │
                    │ │             │
                    │ │  Rs        │
                    │ └──/\/\/─────┤
                    │              │
                    │              ▼ Iout (to load)
                    │

Công thức:
┌─────────────────────────────────────────┐
│  Iout = Vin / Rs                        │
│  Dòng ra không phụ thuộc tải            │
└─────────────────────────────────────────┘

Ứng dụng:
• LED driver
• Current loop (4-20mA)
• Grounded load current source
```

---

## 8. Bảng Tra Cứu Nhanh

### 8.1 Bảng so sánh các Op-Amp phổ biến

```
┌──────────────────────────────────────────────────────────────────────────────────────┐
│                           BẢNG SO SÁNH OP-AMP PHỔ BIẾN                               │
├───────────┬────────┬─────────┬────────┬─────────┬────────┬───────────┬──────────────┤
│  Part #   │  Type  │   GBW   │   SR   │   Vos   │   Ib   │ Vnoise    │    Giá       │
│           │        │  (MHz)  │ (V/μs) │  (μV)   │  (nA)  │ (nV/√Hz)  │    ($)       │
├───────────┼────────┼─────────┼────────┼─────────┼────────┼───────────┼──────────────┤
│ LM741     │  BJT   │    1    │  0.5   │  2000   │   80   │    20     │    0.30      │
│ LM358     │  BJT   │    1    │  0.3   │  3000   │   45   │    40     │    0.25      │
│ TL072     │  JFET  │    3    │   13   │  3000   │ 0.03   │    18     │    0.50      │
│ NE5532    │  BJT   │   10    │    9   │  500    │  200   │     5     │    0.60      │
│ OP07      │  BJT   │  0.6    │  0.3   │   30    │   1.2  │    10     │    1.50      │
│ OPA227    │  BJT   │    8    │  2.3   │   10    │   2.5  │     3     │    3.00      │
│ AD797     │  BJT   │  110    │   20   │   25    │  250   │    0.9    │   10.00      │
│ OPA627    │  JFET  │   16    │   55   │  100    │ 0.001  │    4.5    │   15.00      │
│ AD8065    │  JFET  │  145    │  180   │  500    │ 0.006  │     7     │    4.00      │
│ OPA656    │  JFET  │  230    │  290   │  300    │ 0.002  │     7     │    8.00      │
│ MCP6001   │  CMOS  │    1    │  0.6   │  3000   │0.00001 │    28     │    0.25      │
│ OPA344    │  CMOS  │    1    │  0.8   │  250    │0.00001 │    25     │    1.00      │
│ ADA4522   │Chopper │    3    │  1.5   │    5    │  200   │     6     │    3.50      │
│ INA128    │ InAmp  │   1.3   │  1.1   │   50    │   5    │     8     │    5.00      │
└───────────┴────────┴─────────┴────────┴─────────┴────────┴───────────┴──────────────┘
```

### 8.2 Công thức nhanh

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                            CÔNG THỨC NHANH OP-AMP                               │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  KHUẾCH ĐẠI:                                                                    │
│  ├── Đảo:        Av = -Rf/Rin                                                  │
│  ├── Không đảo:  Av = 1 + Rf/Rin                                               │
│  └── Vi sai:     Av = Rf/Rin (với R cân bằng)                                  │
│                                                                                 │
│  BĂNG THÔNG:                                                                    │
│  ├── BW = GBW / Av                                                             │
│  └── fmax (full power) = SR / (2π × Vpeak)                                     │
│                                                                                 │
│  SAI SỐ DO Vos:                                                                │
│  └── Error = Vos × (1 + Rf/Rin)                                                │
│                                                                                 │
│  SAI SỐ DO Ib:                                                                 │
│  └── Error = Ib × Rf  (có thể bù bằng R song song ở V+)                        │
│                                                                                 │
│  NHIỄU TỔNG:                                                                    │
│  └── en_total = √(en² + (in × Rsource)² + 4kTR)                                │
│                                                                                 │
│  BÙ OFFSET:                                                                     │
│  └── Rcomb = R1 || Rf (đặt ở ngõ vào +)                                        │
│                                                                                 │
│  ĐỘ ỔN ĐỊNH:                                                                    │
│  ├── Phase margin > 45° (tốt > 60°)                                            │
│  └── Gain margin > 10 dB                                                       │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 8.3 Checklist thiết kế

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        CHECKLIST THIẾT KẾ OP-AMP                                │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  ☐ XÁC ĐỊNH YÊU CẦU                                                            │
│    ├── ☐ Loại tín hiệu (DC/AC/Mixed)                                           │
│    ├── ☐ Dải tần số hoạt động                                                  │
│    ├── ☐ Độ chính xác cần thiết                                                │
│    ├── ☐ Trở kháng nguồn tín hiệu                                              │
│    └── ☐ Yêu cầu công suất/dòng ra                                              │
│                                                                                 │
│  ☐ CHỌN OP-AMP                                                                  │
│    ├── ☐ GBW đủ cho ứng dụng                                                   │
│    ├── ☐ Slew Rate phù hợp                                                     │
│    ├── ☐ Vos, drift trong giới hạn                                             │
│    ├── ☐ Ib phù hợp với trở kháng nguồn                                        │
│    ├── ☐ Nhiễu đủ thấp                                                         │
│    └── ☐ Nguồn cung cấp phù hợp                                                 │
│                                                                                 │
│  ☐ THIẾT KẾ MẠCH                                                                │
│    ├── ☐ Tính toán R, C theo công thức                                         │
│    ├── ☐ Kiểm tra độ ổn định                                                   │
│    ├── ☐ Thêm bù offset nếu cần                                                │
│    ├── ☐ Thêm tụ lọc nguồn (0.1μF gần IC)                                      │
│    └── ☐ Bảo vệ ESD cho ngõ vào                                                 │
│                                                                                 │
│  ☐ XÁC NHẬN                                                                     │
│    ├── ☐ Mô phỏng SPICE                                                        │
│    ├── ☐ Kiểm tra nhiệt độ hoạt động                                           │
│    ├── ☐ Đo thử prototype                                                      │
│    └── ☐ Stress test                                                            │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## Tài Liệu Tham Khảo

1. **Op Amps for Everyone** - Ron Mancini, Texas Instruments
2. **The Art of Electronics** - Horowitz & Hill
3. **Analog Devices Application Notes**
4. **Texas Instruments Application Reports**
5. **Linear Technology Application Notes**

---

*Tài liệu được biên soạn cho mục đích học tập và tham khảo kỹ thuật.*
*Phiên bản: 1.0 | Ngày tạo: 14/01/2026*
