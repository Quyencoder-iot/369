#!/bin/bash
# Quick fix script để thêm tất cả includes cần thiết vào các file Qt

echo "🔧 Fixing missing includes in Qt files..."

PROJECT_DIR="/home/mq369/working-space/B5_qt_models_ver_sonet"

# Function to add includes to a file
add_includes() {
    local file=$1
    local temp_file="${file}.temp"
    
    # Backup
    cp "$file" "${file}.backup_$(date +%Y%m%d_%H%M%S)"
    
    # Check if includes already exist
    if grep -q "#include <QPushButton>" "$file" && \
       grep -q "#include <QLabel>" "$file"; then
        echo "✓ $file already has includes"
        return
    fi
    
    # Find first #include line
    first_include=$(grep -n "^#include" "$file" | head -1 | cut -d: -f1)
    
    if [ -z "$first_include" ]; then
        # No includes yet, add after comments
        first_include=10
    fi
    
    # Create includes block
    cat > /tmp/qt_includes.txt << 'EOF'
// Qt Widgets includes
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>

// Qt Layouts
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

// Qt Views
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>

// Qt Models
#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

// Qt Core
#include <QVector>
#include <QVariant>
#include <QString>
#include <QDebug>

// Qt GUI
#include <QFont>
#include <QColor>

EOF
    
    # Insert includes after first existing include or at line 10
    {
        head -n "$first_include" "$file"
        cat /tmp/qt_includes.txt
        tail -n +$((first_include + 1)) "$file"
    } > "$temp_file"
    
    mv "$temp_file" "$file"
    echo "✓ Fixed $file"
}

# Fix TableModel.cpp
if [ -f "$PROJECT_DIR/src/TableModel.cpp" ]; then
    echo "Fixing TableModel.cpp..."
    add_includes "$PROJECT_DIR/src/TableModel.cpp"
fi

# Fix FilterProxy.cpp
if [ -f "$PROJECT_DIR/src/FilterProxy.cpp" ]; then
    echo "Fixing FilterProxy.cpp..."
    add_includes "$PROJECT_DIR/src/FilterProxy.cpp"
fi

# Fix any other cpp files
for file in "$PROJECT_DIR/src"/*.cpp; do
    if [ -f "$file" ]; then
        basename=$(basename "$file")
        if [[ "$basename" != "TableModel.cpp" && "$basename" != "FilterProxy.cpp" ]]; then
            echo "Checking $basename..."
            add_includes "$file"
        fi
    fi
done

echo ""
echo "✅ Done! All files fixed."
echo ""
echo "Now rebuild:"
echo "  cd $PROJECT_DIR/build"
echo "  make clean"
echo "  make"
echo ""
echo "Backups saved with timestamp."
