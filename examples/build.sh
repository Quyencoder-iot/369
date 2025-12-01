#!/bin/bash
# Build script for LayerShellQt examples

set -e  # Exit on error

echo "Building LayerShellQt Examples..."
echo "=================================="
echo ""

# Check if pkg-config can find LayerShellQt
if ! pkg-config --exists LayerShellQtInterface; then
    echo "ERROR: LayerShellQtInterface not found via pkg-config"
    echo "Please install LayerShellQt library first"
    exit 1
fi

echo "Found LayerShellQt version: $(pkg-config --modversion LayerShellQtInterface)"
echo ""

# Get compiler flags
CFLAGS=$(pkg-config --cflags Qt5Widgets LayerShellQtInterface)
LIBS=$(pkg-config --libs Qt5Widgets LayerShellQtInterface)

# Examples to build
EXAMPLES=(
    "01-simple-panel"
    "02-notification"
    "03-desktop-widget"
    "04-animated-wallpaper"
    "05-multi-monitor-panel"
)

# Build each example
for example in "${EXAMPLES[@]}"; do
    echo "Building $example..."
    g++ "${example}.cpp" -o "$example" $CFLAGS $LIBS -fPIC
    
    if [ $? -eq 0 ]; then
        echo "  ✓ Success"
    else
        echo "  ✗ Failed"
        exit 1
    fi
done

echo ""
echo "=================================="
echo "All examples built successfully!"
echo ""
echo "To run an example:"
echo "  ./<example-name>"
echo ""
echo "For example:"
echo "  ./01-simple-panel"
echo ""
