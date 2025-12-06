#!/usr/bin/env python3
"""
CMOS OPAMP Calculator - Tính toán các thông số quan trọng của OPAMP CMOS
Ở cấp độ nguyên tử và phân tử
"""

import math
import numpy as np
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt

# Các hằng số vật lý cơ bản
q = 1.602e-19  # Điện tích electron (Coulomb)
k_B = 1.38e-23  # Hằng số Boltzmann (J/K)
T = 300  # Nhiệt độ (Kelvin)
eps_0 = 8.854e-12  # Hằng số điện môi chân không (F/m)
eps_si = 11.7 * eps_0  # Hằng số điện môi Silicon
eps_ox = 3.9 * eps_0  # Hằng số điện môi SiO2

class CMOSTransistor:
    """Lớp mô tả transistor CMOS ở cấp độ nguyên tử"""
    
    def __init__(self, transistor_type='NMOS', W=10e-6, L=0.18e-6, 
                 tox=3e-9, Vth=0.4, mu=0.05, NA=1e17):
        """
        Parameters:
        -----------
        transistor_type : 'NMOS' hoặc 'PMOS'
        W : Chiều rộng gate (m)
        L : Chiều dài gate (m) - kích thước quan trọng ở cấp độ nguyên tử
        tox : Độ dày oxide (m) - chỉ vài nanomet (vài chục nguyên tử)
        Vth : Threshold voltage (V)
        mu : Mobility (m²/V·s) - khả năng di chuyển của electron/lỗ trống
        NA : Nồng độ doping (atoms/cm³)
        """
        self.type = transistor_type
        self.W = W
        self.L = L
        self.tox = tox
        self.Vth = Vth
        self.mu = mu
        self.NA = NA
        
        # Tính điện dung gate oxide (phụ thuộc vào độ dày oxide ở cấp độ nguyên tử)
        self.Cox = eps_ox / tox  # F/m²
        
        # Tính số nguyên tử trong kênh dẫn điện (ước tính)
        # Thể tích kênh ≈ W × L × độ sâu kênh (~10nm)
        channel_depth = 10e-9  # m
        self.channel_volume = W * L * channel_depth  # m³
        self.atoms_in_channel = self.channel_volume * NA * 1e6  # Chuyển từ cm³ sang m³
        
    def calculate_transconductance(self, Vgs, Vds=0.5):
        """
        Tính transconductance (gm) - độ nhạy của dòng điện với điện áp gate
        Ở cấp độ nguyên tử: phụ thuộc vào số electron trong kênh
        """
        if Vgs < self.Vth:
            return 0
        
        # Công thức: gm = μn × Cox × (W/L) × (Vgs - Vth)
        gm = self.mu * self.Cox * (self.W / self.L) * (Vgs - self.Vth)
        return gm
    
    def calculate_drain_current(self, Vgs, Vds):
        """
        Tính dòng điện drain - phụ thuộc vào số electron di chuyển qua kênh
        """
        if Vgs < self.Vth:
            return 0
        
        Vov = Vgs - self.Vth  # Overdrive voltage
        
        if Vds < Vov:  # Triode region
            Id = self.mu * self.Cox * (self.W / self.L) * (Vov * Vds - 0.5 * Vds**2)
        else:  # Saturation region
            Id = 0.5 * self.mu * self.Cox * (self.W / self.L) * Vov**2
        
        return Id
    
    def calculate_threshold_voltage(self, Vfb=-0.8, phi_f=None):
        """
        Tính threshold voltage dựa trên các thông số vật lý ở cấp độ nguyên tử
        """
        if phi_f is None:
            # Tính Fermi potential từ nồng độ doping
            ni = 1.5e10  # Nồng độ carrier tự nhiên trong Si (cm⁻³)
            phi_f = (k_B * T / q) * math.log(self.NA / ni)
        
        # Công thức threshold voltage
        Vth_calc = Vfb + 2 * phi_f + math.sqrt(2 * q * eps_si * self.NA * 1e6 * 2 * phi_f) / self.Cox
        return Vth_calc
    
    def get_atomic_info(self):
        """Trả về thông tin về số nguyên tử và cấu trúc"""
        # Tính số nguyên tử silicon trong kênh
        si_atoms_per_cm3 = 5e22  # Số nguyên tử Si trong 1 cm³
        si_atoms = self.channel_volume * si_atoms_per_cm3 * 1e6
        
        # Tính số nguyên tử dopant
        dopant_atoms = self.atoms_in_channel
        
        return {
            'silicon_atoms': si_atoms,
            'dopant_atoms': dopant_atoms,
            'dopant_concentration_ppm': (dopant_atoms / si_atoms) * 1e6,
            'oxide_thickness_atoms': self.tox / (2.27e-10),  # Khoảng cách Si-O trong SiO2
            'channel_length_atoms': self.L / (5.43e-10),  # Lattice constant của Si
        }


class CMOSOPAMP:
    """Lớp mô tả OPAMP CMOS 2-stage"""
    
    def __init__(self):
        # Stage 1: Differential Amplifier
        self.M1 = CMOSTransistor('NMOS', W=20e-6, L=0.18e-6, Vth=0.4, mu=0.05)
        self.M2 = CMOSTransistor('NMOS', W=20e-6, L=0.18e-6, Vth=0.4, mu=0.05)
        self.M3 = CMOSTransistor('PMOS', W=10e-6, L=0.18e-6, Vth=-0.4, mu=0.02)
        self.M4 = CMOSTransistor('PMOS', W=10e-6, L=0.18e-6, Vth=-0.4, mu=0.02)
        
        # Stage 2: Common Source
        self.M5 = CMOSTransistor('NMOS', W=50e-6, L=0.18e-6, Vth=0.4, mu=0.05)
        self.M6 = CMOSTransistor('PMOS', W=20e-6, L=0.18e-6, Vth=-0.4, mu=0.02)
        
        # Compensation capacitor (tính bằng số nguyên tử)
        self.Cc = 2e-12  # 2pF
        
    def calculate_gain(self, Vcm=0.9, Vdd=1.8):
        """
        Tính độ lợi điện áp của OPAMP
        """
        # Bias voltage cho differential pair
        Vgs1 = Vcm - 0.2  # Giả sử source ở 0.2V
        
        # Transconductance của Stage 1
        gm1 = self.M1.calculate_transconductance(Vgs1)
        
        # Output resistance của Stage 1 (simplified)
        ro1 = 1 / (0.1 * gm1)  # Giả sử lambda = 0.1
        
        # Transconductance của Stage 2
        Vgs5 = Vdd / 2
        gm5 = self.M5.calculate_transconductance(Vgs5)
        ro5 = 1 / (0.1 * gm5)
        
        # Độ lợi Stage 1
        Av1 = -gm1 * ro1
        
        # Độ lợi Stage 2
        Av2 = -gm5 * ro5
        
        # Tổng độ lợi
        Av_total = Av1 * Av2
        
        return {
            'stage1_gain': Av1,
            'stage2_gain': Av2,
            'total_gain': Av_total,
            'total_gain_dB': 20 * math.log10(abs(Av_total))
        }
    
    def calculate_bandwidth(self):
        """
        Tính bandwidth dựa trên compensation capacitor
        """
        gm1 = self.M1.calculate_transconductance(0.7)
        GBW = gm1 / (2 * math.pi * self.Cc)  # Gain-Bandwidth Product
        return GBW
    
    def analyze_atomic_structure(self):
        """Phân tích cấu trúc nguyên tử của OPAMP"""
        info = {}
        
        # Thông tin về các transistor
        for i, transistor in enumerate([self.M1, self.M2, self.M3, self.M4, self.M5, self.M6], 1):
            info[f'M{i}'] = transistor.get_atomic_info()
        
        # Tổng số nguyên tử trong OPAMP
        total_si_atoms = sum([t['silicon_atoms'] for t in info.values()])
        total_dopant_atoms = sum([t['dopant_atoms'] for t in info.values()])
        
        info['total'] = {
            'total_silicon_atoms': total_si_atoms,
            'total_dopant_atoms': total_dopant_atoms,
            'average_doping_ppm': (total_dopant_atoms / total_si_atoms) * 1e6
        }
        
        return info


def plot_transfer_characteristic():
    """Vẽ đặc tuyến truyền đạt của OPAMP"""
    opamp = CMOSOPAMP()
    
    # Tạo dải điện áp đầu vào
    Vin_diff = np.linspace(-0.1, 0.1, 1000)  # Differential input
    Vout = []
    
    for vd in Vin_diff:
        # Simplified model: Vout = A × Vin_diff
        gain = opamp.calculate_gain()['total_gain']
        vout = gain * vd
        # Giới hạn bởi Vdd và GND
        vout = max(-1.8, min(1.8, vout))
        Vout.append(vout)
    
    plt.figure(figsize=(10, 6))
    plt.plot(Vin_diff * 1000, Vout, 'b-', linewidth=2)
    plt.xlabel('Điện áp đầu vào vi sai (mV)', fontsize=12)
    plt.ylabel('Điện áp đầu ra (V)', fontsize=12)
    plt.title('Đặc Tuyến Truyền Đạt OPAMP CMOS', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.axhline(y=0, color='k', linestyle='--', alpha=0.5)
    plt.axvline(x=0, color='k', linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.savefig('opamp_transfer_characteristic.png', dpi=150)
    print("Đã lưu biểu đồ: opamp_transfer_characteristic.png")


def plot_gm_vs_Vgs():
    """Vẽ transconductance theo Vgs để hiểu cơ chế ở cấp độ nguyên tử"""
    nmos = CMOSTransistor('NMOS', W=20e-6, L=0.18e-6)
    pmos = CMOSTransistor('PMOS', W=20e-6, L=0.18e-6, Vth=-0.4)
    
    Vgs_nmos = np.linspace(0, 1.0, 100)
    Vgs_pmos = np.linspace(-1.0, 0, 100)
    
    gm_nmos = [nmos.calculate_transconductance(vgs) for vgs in Vgs_nmos]
    gm_pmos = [pmos.calculate_transconductance(abs(vgs), Vds=0.5) for vgs in Vgs_pmos]
    
    plt.figure(figsize=(10, 6))
    plt.plot(Vgs_nmos, gm_nmos, 'b-', linewidth=2, label='NMOS')
    plt.plot(Vgs_pmos, gm_pmos, 'r-', linewidth=2, label='PMOS')
    plt.xlabel('Điện áp Gate-Source (V)', fontsize=12)
    plt.ylabel('Transconductance (S)', fontsize=12)
    plt.title('Transconductance vs Vgs - Phụ thuộc vào số electron trong kênh', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('transconductance_vs_vgs.png', dpi=150)
    print("Đã lưu biểu đồ: transconductance_vs_vgs.png")


def main():
    """Hàm chính để chạy các tính toán và phân tích"""
    print("=" * 70)
    print("PHÂN TÍCH OPAMP CMOS Ở CẤP ĐỘ NGUYÊN TỬ VÀ PHÂN TỬ")
    print("=" * 70)
    print()
    
    # Tạo OPAMP
    opamp = CMOSOPAMP()
    
    # Tính độ lợi
    print("1. THÔNG SỐ ĐỘ LỢI:")
    print("-" * 70)
    gain_info = opamp.calculate_gain()
    print(f"   Độ lợi Stage 1: {gain_info['stage1_gain']:.2f}")
    print(f"   Độ lợi Stage 2: {gain_info['stage2_gain']:.2f}")
    print(f"   Tổng độ lợi: {gain_info['total_gain']:.2f} ({gain_info['total_gain_dB']:.2f} dB)")
    print()
    
    # Tính bandwidth
    print("2. BANDWIDTH:")
    print("-" * 70)
    gbw = opamp.calculate_bandwidth()
    print(f"   Gain-Bandwidth Product: {gbw/1e6:.2f} MHz")
    print()
    
    # Phân tích cấu trúc nguyên tử
    print("3. PHÂN TÍCH CẤU TRÚC NGUYÊN TỬ:")
    print("-" * 70)
    atomic_info = opamp.analyze_atomic_structure()
    
    for key, value in atomic_info.items():
        if key != 'total':
            print(f"\n   Transistor {key}:")
            print(f"      - Nguyên tử Silicon: {value['silicon_atoms']:.2e}")
            print(f"      - Nguyên tử Dopant: {value['dopant_atoms']:.2e}")
            print(f"      - Nồng độ doping: {value['dopant_concentration_ppm']:.2f} ppm")
            print(f"      - Độ dày oxide (số lớp nguyên tử): {value['oxide_thickness_atoms']:.0f}")
            print(f"      - Chiều dài kênh (số nguyên tử Si): {value['channel_length_atoms']:.0f}")
    
    print(f"\n   TỔNG QUAN:")
    total = atomic_info['total']
    print(f"      - Tổng nguyên tử Silicon: {total['total_silicon_atoms']:.2e}")
    print(f"      - Tổng nguyên tử Dopant: {total['total_dopant_atoms']:.2e}")
    print(f"      - Nồng độ doping trung bình: {total['average_doping_ppm']:.2f} ppm")
    print()
    
    # Tính threshold voltage từ các thông số vật lý
    print("4. THRESHOLD VOLTAGE (Tính từ thông số vật lý):")
    print("-" * 70)
    nmos = CMOSTransistor('NMOS', NA=1e17)
    vth_calc = nmos.calculate_threshold_voltage()
    print(f"   Vth tính toán: {vth_calc:.3f} V")
    print(f"   Vth thực tế: {nmos.Vth:.3f} V")
    print()
    
    # Vẽ các biểu đồ
    print("5. TẠO CÁC BIỂU ĐỒ:")
    print("-" * 70)
    plot_transfer_characteristic()
    plot_gm_vs_Vgs()
    print()
    
    print("=" * 70)
    print("HOÀN TẤT!")
    print("=" * 70)


if __name__ == "__main__":
    main()
