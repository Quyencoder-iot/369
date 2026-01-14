# Nguyên lý Cốt lõi của Operational Amplifier (Op-Amp)

Tài liệu này giải thích chi tiết về nguyên lý hoạt động của Op-Amp từ cấp độ vật lý nguyên tử đến cấu trúc mạch transistor.

## 1. Cấp độ Nguyên tử: Vật lý Bán dẫn

Để hiểu Op-Amp, ta phải bắt đầu từ vật liệu tạo nên nó: **Chất bán dẫn (Semiconductor)**, thường là Silicon (Si).

### Cấu trúc tinh thể Silicon
- Một nguyên tử Silicon có 4 electron ở lớp vỏ ngoài cùng (lớp hóa trị).
- Trong tinh thể tinh khiết, mỗi nguyên tử Si liên kết cộng hóa trị với 4 nguyên tử lân cận, tạo nên cấu trúc mạng lưới bền vững. Ở trạng thái này, Si dẫn điện rất kém (gần như cách điện).

### Doping (Pha tạp) - Tạo ra hạt tải điện
Để dẫn điện và điều khiển dòng điện, người ta pha tạp chất vào Si:
1.  **Bán dẫn loại N (Negative):** Pha Photpho (P) hoặc Asen (As) (có 5 electron hóa trị). 4 electron liên kết với Si, dư ra 1 electron tự do.
    *   Hạt tải điện đa số: **Electron (âm)**.
2.  **Bán dẫn loại P (Positive):** Pha Bo (B) hoặc Gallium (Ga) (có 3 electron hóa trị). Thiếu 1 electron để tạo liên kết đủ 4, tạo ra một "lỗ trống" (hole).
    *   Hạt tải điện đa số: **Lỗ trống (dương)**.

### Tiếp giáp P-N (P-N Junction) - Trái tim của linh kiện
Khi ghép bán dẫn P và N lại với nhau:
- Tại mặt tiếp xúc, electron từ miền N khuếch tán sang miền P lấp vào lỗ trống.
- Quá trình này tạo ra một vùng không có hạt tải điện tự do gọi là **Vùng nghèo (Depletion Region)**.
- Vùng này tạo ra một điện trường nội tại ngăn cản dòng điện tiếp tục khuếch tán.
- **Phân cực thuận (Forward Bias):** Đặt điện áp dương vào P, âm vào N. Điện trường ngoài thắng điện trường nội tại -> Dòng điện chạy qua dễ dàng.
- **Phân cực ngược (Reverse Bias):** Đặt điện áp âm vào P, dương vào N. Vùng nghèo mở rộng -> Không có dòng điện (hoặc rất nhỏ).

=> Đây là nguyên lý của **Diode** và là cơ sở của Transistor.

---

## 2. Cấp độ Linh kiện: Transistor (BJT & MOSFET)

Op-Amp được cấu tạo từ hàng chục đến hàng nghìn transistor. Có hai loại chính: BJT (Bipolar Junction Transistor) và MOSFET (Metal-Oxide-Semiconductor Field-Effect Transistor).

### BJT (NPN hoặc PNP)
Cấu trúc 3 lớp: Ví dụ NPN (N-P-N).
- **Cực E (Emitter):** Vùng phát, nồng độ tạp chất cao nhất, cung cấp electron.
- **Cực B (Base):** Vùng nền, rất mỏng, nồng độ tạp chất thấp.
- **Cực C (Collector):** Vùng thu.

**Cơ chế khuếch đại (Nguyên tử):**
1.  Khi đặt một điện áp nhỏ $V_{BE}$ (phân cực thuận E-B), hàng rào thế năng giảm.
2.  Một lượng lớn electron từ E tràn sang B.
3.  Do vùng B rất mỏng và ít lỗ trống, chỉ một phần nhỏ electron tái hợp (tạo dòng $I_B$ nhỏ).
4.  Phần lớn electron bị điện trường mạnh của cực C (phân cực ngược B-C) hút về C, tạo dòng $I_C$ lớn.
5.  **Kết quả:** Một dòng nhỏ $I_B$ điều khiển được dòng lớn $I_C$. ($I_C = \beta I_B$).

### MOSFET (CMOS)
Cấu trúc G (Gate), D (Drain), S (Source), B (Body).
- **Gate:** Cách điện với kênh dẫn bằng lớp oxide ($SiO_2$).
- **Nguyên lý:** Dùng **điện trường** để điều khiển độ dẫn điện của kênh.
1.  Đặt điện áp vào Gate tạo ra điện trường hút các hạt tải điện (electron với NMOS) tập trung dưới lớp oxide.
2.  Khi mật độ hạt đủ lớn, một "kênh dẫn" hình thành nối D và S.
3.  Dòng điện chạy từ D sang S.
4.  Không có dòng điện chạy vào Gate (trở kháng đầu vào vô cùng lớn).

---

## 3. Cấu trúc Op-Amp: Các tầng khuếch đại ghép nối

Một Op-Amp chuẩn thường gồm 3 tầng chính ghép nối tiếp nhau. Mỗi tầng đảm nhiệm một vai trò cụ thể để đạt được đặc tính lý tưởng: Hệ số khuếch đại vô cùng, trở kháng vào vô cùng, trở kháng ra bằng 0.

### Tầng 1: Tầng khuếch đại vi sai (Differential Input Stage)
*Đây là tầng quan trọng nhất quyết định độ chính xác của Op-Amp.*

- **Cấu tạo:** Gồm 2 transistor (Q1, Q2) giống hệt nhau, chung cực E (với BJT) hoặc S (với FET), được cấp dòng bởi một nguồn dòng hằng (Current Source).
- **Nguyên lý:**
    - Có 2 ngõ vào: Đảo (-) và Không đảo (+).
    - Nó chỉ khuếch đại **hiệu điện thế** giữa hai ngõ vào ($V_+ - V_-$).
    - Loại bỏ các tín hiệu nhiễu xuất hiện đồng thời ở cả 2 ngõ (Common Mode Rejection).
- **Tại sao cần?** Để so sánh tín hiệu đầu vào và cung cấp trở kháng đầu vào cao.

### Tầng 2: Tầng khuếch đại điện áp (Voltage Amplification Stage - VAS)
*Tầng cung cấp hệ số khuếch đại chính.*

- **Cấu tạo:** Thường là mạch Common-Emitter (BJT) hoặc Common-Source (MOSFET) tải tích cực.
- **Nhiệm vụ:**
    - Nhận tín hiệu nhỏ từ tầng vi sai và khuếch đại lên biên độ điện áp rất lớn (hàng nghìn đến hàng triệu lần).
    - Thường có tụ bù tần số (Miller compensation capacitor) để đảm bảo Op-Amp hoạt động ổn định, không bị tự dao động.

### Tầng 3: Tầng công suất đầu ra (Output Stage)
*Tầng giúp Op-Amp "lái" tải (loa, motor, mạch sau).*

- **Cấu tạo:** Thường là cấu trúc **Push-Pull (Đẩy-Kéo)** dùng cặp transistor bù (NPN + PNP hoặc NMOS + PMOS).
- **Nguyên lý:**
    - Transistor phía trên (NPN/NMOS) dẫn dòng khi điện áp ra dương (Đẩy dòng ra tải).
    - Transistor phía dưới (PNP/PMOS) dẫn dòng khi điện áp ra âm (Kéo dòng từ tải về).
- **Nhiệm vụ:**
    - Cung cấp dòng điện lớn cho tải (Khuếch đại dòng điện).
    - Giữ trở kháng đầu ra thấp (để điện áp ra không bị sụt khi gắn tải nặng).

## Tóm tắt dòng chảy tín hiệu
1.  **Input:** Tín hiệu vào chênh lệch rất nhỏ tại chân (+) và (-).
2.  **Diff Stage:** Chuyển đổi chênh lệch áp thành dòng điện, loại bỏ nhiễu chung.
3.  **VAS:** Biến đổi dòng điện thành điện áp lớn.
4.  **Output Stage:** Tăng cường dòng điện cho điện áp đó để điều khiển tải.
5.  **Output:** Tín hiệu ra là bản sao khuếch đại của hiệu số tín hiệu vào.
