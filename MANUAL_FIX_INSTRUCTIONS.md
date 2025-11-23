# 🔧 Manual Fix Instructions

## Lỗi Hiện Tại:

File `FilterProxy.cpp` thiếu includes cho:
- `QPushButton`
- `QLabel`

---

## ✅ Giải Pháp Nhanh Nhất:

### Option 1: Thêm Includes Thủ Công

Mở file:
```bash
nano /home/mq369/working-space/B5_qt_models_ver_sonet/src/FilterProxy.cpp
```

Thêm vào đầu file (sau dòng 10-15):

```cpp
// Thêm các dòng này:
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
```

Save (Ctrl+O, Enter, Ctrl+X) và build lại:
```bash
cd /home/mq369/working-space/B5_qt_models_ver_sonet/build
make
```

---

### Option 2: Dùng sed (One-liner)

```bash
# Backup
cp /home/mq369/working-space/B5_qt_models_ver_sonet/src/FilterProxy.cpp \
   /home/mq369/working-space/B5_qt_models_ver_sonet/src/FilterProxy.cpp.bak

# Add includes at line 10
sed -i '10i\
#include <QPushButton>\n\
#include <QLabel>\n\
#include <QLineEdit>\n\
#include <QComboBox>\n\
#include <QCheckBox>\n\
#include <QVBoxLayout>\n\
#include <QHBoxLayout>\n\
#include <QWidget>\n\
' /home/mq369/working-space/B5_qt_models_ver_sonet/src/FilterProxy.cpp

# Rebuild
cd /home/mq369/working-space/B5_qt_models_ver_sonet/build
make
```

---

### Option 3: Dùng Script Tự Động

```bash
# Copy script
cd /workspace
chmod +x QUICK_FIX_ALL_INCLUDES.sh

# Run script
./QUICK_FIX_ALL_INCLUDES.sh

# Rebuild
cd /home/mq369/working-space/B5_qt_models_ver_sonet/build
make clean
make
```

---

## 📋 Checklist Includes Cho Mỗi File:

### TableModel.cpp cần:
```cpp
#include <QApplication>
#include <QWidget>
#include <QAbstractTableModel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVector>
#include <QVariant>
#include <QFont>
#include <QColor>
```

### FilterProxy.cpp cần:
```cpp
#include <QApplication>
#include <QWidget>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QVector>
#include <QString>
```

### ListModel.cpp cần:
```cpp
#include <QApplication>
#include <QWidget>
#include <QAbstractListModel>
#include <QListView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
```

### TreeModel.cpp cần:
```cpp
#include <QApplication>
#include <QWidget>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVector>
```

### SqlModels.cpp cần:
```cpp
#include <QApplication>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QMessageBox>
```

---

## 🎯 Template Header Block Cho Tất Cả Files:

Copy block này vào đầu mỗi .cpp file:

```cpp
/**
 * [Filename].cpp - Qt Model Example
 */

// ============================================
// Qt Core
// ============================================
#include <QVector>
#include <QList>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDate>

// ============================================
// Qt GUI
// ============================================
#include <QFont>
#include <QColor>
#include <QIcon>

// ============================================
// Qt Widgets - Core
// ============================================
#include <QApplication>
#include <QWidget>

// ============================================
// Qt Widgets - Controls
// ============================================
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QTabWidget>

// ============================================
// Qt Widgets - Layouts
// ============================================
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>

// ============================================
// Qt Widgets - Views
// ============================================
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>

// ============================================
// Qt Widgets - Models
// ============================================
#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QSortFilterProxyModel>

// ============================================
// Qt SQL (if needed)
// ============================================
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>

// ============================================
// Qt Dialogs (if needed)
// ============================================
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

// ============================================
// Your code starts here
// ============================================
```

---

## 🚨 Nếu Vẫn Lỗi:

### 1. Kiểm tra CMakeLists.txt có đủ modules:

```cmake
find_package(Qt5 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui 
    Sql
)

target_link_libraries(your_target
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Sql
)
```

### 2. Clean build:

```bash
cd /home/mq369/working-space/B5_qt_models_ver_sonet/build
make clean
rm -rf *
cmake ..
make
```

### 3. Kiểm tra Qt installation:

```bash
# Check Qt version
qmake --version

# Find Qt includes
find /usr/include -name "QPushButton" 2>/dev/null
find /usr/include -name "QLabel" 2>/dev/null

# If found, your Qt is installed correctly
```

---

## 📝 Example Fixed File:

**FilterProxy.cpp** (Đầu file):

```cpp
/**
 * VÍ DỤ 4: QSortFilterProxyModel - Filter và Sort
 */

#include <QSortFilterProxyModel>
#include <QAbstractListModel>
#include <QListView>
#include <QApplication>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

// THÊM CÁC DÒNG NÀY:
#include <QPushButton>
#include <QLabel>
#include <QWidget>

// ============================================
// Source Model: Danh sách Sản phẩm
// ============================================
class ProductModel : public QAbstractListModel {
    Q_OBJECT
    // ... rest of code
```

---

## ✅ Sau Khi Fix:

```bash
cd /home/mq369/working-space/B5_qt_models_ver_sonet/build
make

# Kết quả mong đợi:
# [ 20%] Building CXX object CMakeFiles/proxy_model.dir/src/FilterProxy.cpp.o
# [ 40%] Linking CXX executable proxy_model
# [100%] Built target proxy_model
```

---

## 💡 Pro Tip:

Tạo file header chung:

**qt_common.h:**
```cpp
#ifndef QT_COMMON_H
#define QT_COMMON_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>
#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QFont>
#include <QColor>

#endif // QT_COMMON_H
```

Rồi trong mỗi .cpp chỉ cần:
```cpp
#include "qt_common.h"
```

---

**Fix và build lại là xong! 🚀**
