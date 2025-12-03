#!/bin/bash
# Build script for QML Multi-Monitor Panel example

set -e

echo "Building QML Multi-Monitor Panel Example..."
echo "==========================================="
echo ""

# Check dependencies
if ! pkg-config --exists LayerShellQtInterface; then
    echo "ERROR: LayerShellQtInterface not found"
    exit 1
fi

if ! pkg-config --exists Qt5Quick; then
    echo "ERROR: Qt5Quick not found"
    echo "Install with: sudo apt install qtdeclarative5-dev"
    exit 1
fi

echo "Found dependencies:"
echo "  LayerShellQt: $(pkg-config --modversion LayerShellQtInterface)"
echo "  Qt5Quick: $(pkg-config --modversion Qt5Quick)"
echo ""

# Get compiler flags
CFLAGS=$(pkg-config --cflags Qt5Quick Qt5Qml LayerShellQtInterface)
LIBS=$(pkg-config --libs Qt5Quick Qt5Qml LayerShellQtInterface)

# Generate resource file
echo "Generating resources..."
rcc resources.qrc -o qrc_resources.cpp

# Compile
echo "Compiling..."
g++ 06-qml-multi-monitor-panel.cpp qrc_resources.cpp -o 06-qml-multi-monitor-panel \
    $CFLAGS $LIBS -fPIC

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Build successful!"
    echo ""
    echo "Run with: ./06-qml-multi-monitor-panel"
    echo ""
else
    echo ""
    echo "✗ Build failed!"
    exit 1
fi
