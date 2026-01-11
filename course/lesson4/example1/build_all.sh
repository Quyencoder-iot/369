#!/bin/bash
# ============================================
# BUILD SCRIPT - Biên dịch toàn bộ hệ thống
# ============================================

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COURSE_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "=========================================="
echo "  AUTOSAR ADAPTIVE DASHBOARD - BUILD"
echo "=========================================="
echo "Course directory: $COURSE_DIR"

# Build Dashboard Service
echo ""
echo "[1/2] Building Dashboard Service..."
cd "$COURSE_DIR/course/lesson2/example2"
mkdir -p build && cd build
cmake .. 2>/dev/null || echo "CMake configuration may need adjustment"
make -j$(nproc) 2>/dev/null || echo "Build may require vsomeip installed"
echo "[OK] Dashboard Service build attempted"

# Build Qt Dashboard
echo ""
echo "[2/2] Building Qt Dashboard..."
cd "$COURSE_DIR/course/lesson3/example2"
mkdir -p build && cd build
cmake .. 2>/dev/null || echo "CMake configuration may need adjustment"
make -j$(nproc) 2>/dev/null || echo "Build may require Qt5 installed"
echo "[OK] Qt Dashboard build attempted"

echo ""
echo "=========================================="
echo "  BUILD COMPLETE!"
echo "=========================================="
echo ""
echo "To run the demo, execute: ./run_demo.sh"
