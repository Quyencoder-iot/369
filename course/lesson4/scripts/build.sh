#!/bin/bash
# ============================================
# BUILD SCRIPT - Build all components
# ============================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "=========================================="
echo "  AUTOSAR Dashboard - Build"
echo "=========================================="
echo "Project: $PROJECT_DIR"

# Build options
BUILD_SERVICE=${BUILD_SERVICE:-ON}
BUILD_CLIENT=${BUILD_CLIENT:-ON}
BUILD_QT=${BUILD_QT:-ON}

# Lesson 2 - Service
if [ "$BUILD_SERVICE" = "ON" ]; then
    echo ""
    echo "[Service] Building Dashboard Service..."
    cd "$PROJECT_DIR/course/lesson2/example2"
    mkdir -p build && cd build
    cmake .. 2>&1 | tail -5
    make -j$(nproc) 2>&1 | tail -5
    echo "[Service] Done"
fi

# Lesson 3 - Qt UI
if [ "$BUILD_QT" = "ON" ]; then
    echo ""
    echo "[Qt UI] Building Qt Dashboard..."
    cd "$PROJECT_DIR/course/lesson3/example2"
    mkdir -p build && cd build
    cmake .. 2>&1 | tail -5
    make -j$(nproc) 2>&1 | tail -5
    echo "[Qt UI] Done"
fi

echo ""
echo "=========================================="
echo "  Build Complete!"
echo "=========================================="
