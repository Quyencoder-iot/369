#!/bin/bash
# Complete fix for all missing includes in the project

PROJECT_DIR="/home/mq369/working-space/B5_qt_models_ver_sonet"

echo "🔧 Fixing ALL missing includes..."
echo "=================================="

# Function to add includes to a file
fix_file() {
    local file=$1
    local includes=$2
    
    if [ ! -f "$file" ]; then
        echo "⚠️  File not found: $file"
        return
    fi
    
    echo "📝 Fixing: $(basename $file)"
    
    # Backup
    cp "$file" "${file}.backup_$(date +%Y%m%d_%H%M%S)"
    
    # Find line to insert after (after first #include or line 10)
    local insert_line=$(grep -n "^#include" "$file" | head -1 | cut -d: -f1)
    if [ -z "$insert_line" ]; then
        insert_line=10
    fi
    
    # Create temp file with includes
    echo "$includes" > /tmp/temp_includes.txt
    
    # Check if includes already exist
    if grep -q "QGroupBox\|QFormLayout\|QLabel\|QPushButton" "$file"; then
        echo "   ℹ️  Some includes already present, adding missing ones..."
    fi
    
    # Add includes after first include line
    sed -i "${insert_line}r /tmp/temp_includes.txt" "$file"
    
    # Remove duplicate includes
    awk '!seen[$0]++' "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
    
    echo "   ✅ Fixed!"
}

# ============================================
# Fix SqlSetup.cpp
# ============================================
if [ -f "$PROJECT_DIR/src/SqlSetup.cpp" ]; then
    fix_file "$PROJECT_DIR/src/SqlSetup.cpp" "#include <QLabel>"
fi

# ============================================
# Fix example_main.cpp
# ============================================
if [ -f "$PROJECT_DIR/src/example_main.cpp" ]; then
    fix_file "$PROJECT_DIR/src/example_main.cpp" "$(cat << 'EOF'
#include <QGroupBox>
#include <QFormLayout>
#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
EOF
)"
fi

# ============================================
# Fix TableModel.cpp
# ============================================
if [ -f "$PROJECT_DIR/src/TableModel.cpp" ]; then
    fix_file "$PROJECT_DIR/src/TableModel.cpp" "$(cat << 'EOF'
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
EOF
)"
fi

# ============================================
# Fix FilterProxy.cpp
# ============================================
if [ -f "$PROJECT_DIR/src/FilterProxy.cpp" ]; then
    fix_file "$PROJECT_DIR/src/FilterProxy.cpp" "$(cat << 'EOF'
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
EOF
)"
fi

# ============================================
# Fix any other .cpp files
# ============================================
for file in "$PROJECT_DIR/src"/*.cpp; do
    if [ -f "$file" ]; then
        basename=$(basename "$file")
        case "$basename" in
            SqlSetup.cpp|example_main.cpp|TableModel.cpp|FilterProxy.cpp)
                # Already processed
                ;;
            *)
                echo "📝 Checking: $basename"
                # Add common includes
                fix_file "$file" "$(cat << 'EOF'
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
EOF
)"
                ;;
        esac
    fi
done

echo ""
echo "=================================="
echo "✅ All files fixed!"
echo ""
echo "📦 Backups saved with timestamp"
echo ""
echo "🚀 Now rebuild:"
echo "   cd $PROJECT_DIR/build"
echo "   make clean"
echo "   make -j\$(nproc)"
echo ""
