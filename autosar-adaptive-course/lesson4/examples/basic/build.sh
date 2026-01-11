#!/bin/bash
#
# build.sh - VÍ DỤ 1: Script build cơ bản
#
# Các yếu tố BẮT BUỘC:
# 1. Kiểm tra dependencies
# 2. Tạo thư mục build
# 3. Chạy CMake
# 4. Compile
# 5. Thông báo kết quả

set -e  # Exit on error

echo "==================================="
echo "  AUTOSAR Dashboard - Basic Build"
echo "==================================="

# 1. KIỂM TRA DEPENDENCIES
echo ""
echo "[1/5] Checking dependencies..."

check_command() {
    if ! command -v $1 &> /dev/null; then
        echo "ERROR: $1 is not installed"
        exit 1
    fi
    echo "  ✓ $1 found"
}

check_command cmake
check_command g++
check_command pkg-config

# Check vsomeip
if ! pkg-config --exists vsomeip3 2>/dev/null; then
    echo "  ⚠ vsomeip3 not found via pkg-config"
    echo "  Trying to find manually..."
    if [ -f /usr/local/lib/libvsomeip3.so ]; then
        echo "  ✓ vsomeip3 found in /usr/local/lib"
    else
        echo "ERROR: vsomeip3 not installed. Please install vsomeip first."
        exit 1
    fi
else
    echo "  ✓ vsomeip3 found"
fi

# Check Boost
if ! dpkg -l | grep -q libboost-dev; then
    echo "ERROR: Boost not installed"
    exit 1
fi
echo "  ✓ Boost found"

echo ""
echo "[2/5] Creating build directory..."
mkdir -p build
cd build

echo ""
echo "[3/5] Running CMake..."
cmake ..

echo ""
echo "[4/5] Compiling..."
make -j$(nproc)

echo ""
echo "[5/5] Build complete!"
echo ""
echo "==================================="
echo "  Executables created in ./build/"
echo "==================================="

# Liệt kê executables
echo ""
echo "Built files:"
find . -maxdepth 1 -type f -executable -name "*.out" -o -name "*_service*" -o -name "*_client*" 2>/dev/null || true
ls -la *.out *_service* *_client* 2>/dev/null || echo "  (check build directory for executables)"
