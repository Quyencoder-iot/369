#!/bin/bash
# Fix QLabel incomplete type error

PROJECT_DIR="/home/mq369/working-space/B5_qt_models_ver_sonet"
FILE="$PROJECT_DIR/src/SqlSetup.cpp"

echo "🔧 Fixing QLabel incomplete type error in SqlSetup.cpp..."

# Backup
cp "$FILE" "$FILE.backup_$(date +%Y%m%d_%H%M%S)"
echo "✓ Backup created"

# Check if QLabel include already exists
if grep -q "#include <QLabel>" "$FILE"; then
    echo "✓ QLabel include already exists"
else
    # Find the line with QMessageBox include
    line_num=$(grep -n "#include <QMessageBox>" "$FILE" | cut -d: -f1)
    
    if [ -n "$line_num" ]; then
        # Add QLabel include after QMessageBox
        sed -i "${line_num}a\\#include <QLabel>" "$FILE"
        echo "✓ Added #include <QLabel> after line $line_num"
    else
        # Add at line 25 (fallback)
        sed -i '25a\#include <QLabel>' "$FILE"
        echo "✓ Added #include <QLabel> at line 25"
    fi
fi

# Also fix the lambda warnings (optional)
echo ""
echo "✓ Fixed! Now rebuild:"
echo "  cd $PROJECT_DIR/build"
echo "  make"
