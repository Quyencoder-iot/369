# 📚 Qt5 Includes Reference - Complete List

## 🎯 Quick Fix Commands

### Fix example_main.cpp:
```bash
cd /home/mq369/working-space/B5_qt_models_ver_sonet

# Add missing includes
sed -i '20a\
#include <QGroupBox>\n\
#include <QFormLayout>\n\
#include <QDate>\n\
' src/example_main.cpp

# Rebuild
cd build && make
```

### Fix SqlSetup.cpp:
```bash
sed -i '25a\#include <QLabel>' \
    /home/mq369/working-space/B5_qt_models_ver_sonet/src/SqlSetup.cpp
```

### Fix All Files At Once:
```bash
cd /workspace
chmod +x FIX_ALL_MISSING_INCLUDES.sh
./FIX_ALL_MISSING_INCLUDES.sh
```

---

## 📋 Complete Include List by Widget/Class

### Core Classes
```cpp
#include <QApplication>     // App initialization
#include <QWidget>          // Base widget class
#include <QObject>          // Base for Q_OBJECT
#include <QString>          // String class
#include <QVariant>         // Generic value
#include <QVector>          // Dynamic array
#include <QList>            // List container
#include <QDebug>           // Debug output
```

### Date/Time
```cpp
#include <QDate>            // Date class (for QDate::currentDate())
#include <QTime>            // Time class
#include <QDateTime>        // DateTime class
```

### Layouts
```cpp
#include <QVBoxLayout>      // Vertical layout
#include <QHBoxLayout>      // Horizontal layout
#include <QGridLayout>      // Grid layout
#include <QFormLayout>      // Form layout (key-value pairs)
#include <QBoxLayout>       // Base layout class
#include <QLayout>          // Layout base
```

### Basic Widgets
```cpp
#include <QLabel>           // Text label
#include <QPushButton>      // Button
#include <QLineEdit>        // Text input
#include <QTextEdit>        // Multi-line text
#include <QCheckBox>        // Checkbox
#include <QRadioButton>     // Radio button
#include <QSpinBox>         // Number spinner
#include <QDoubleSpinBox>   // Double spinner
#include <QSlider>          // Slider
#include <QProgressBar>     // Progress bar
```

### Container Widgets
```cpp
#include <QGroupBox>        // Group box with title
#include <QFrame>           // Frame container
#include <QTabWidget>       // Tab container
#include <QScrollArea>      // Scrollable area
#include <QSplitter>        // Splitter widget
#include <QStackedWidget>   // Stacked pages
```

### Selection Widgets
```cpp
#include <QComboBox>        // Dropdown list
#include <QListWidget>      // Simple list
#include <QTreeWidget>      // Simple tree
#include <QTableWidget>     // Simple table
```

### View Classes (Model/View)
```cpp
#include <QListView>        // List view
#include <QTableView>       // Table view
#include <QTreeView>        // Tree view
#include <QHeaderView>      // Header for views
#include <QAbstractItemView> // Base view class
```

### Model Classes
```cpp
#include <QAbstractItemModel>      // Base model
#include <QAbstractListModel>      // List model base
#include <QAbstractTableModel>     // Table model base
#include <QStringListModel>        // String list model
#include <QStandardItemModel>      // Standard model
#include <QSortFilterProxyModel>   // Proxy model
#include <QIdentityProxyModel>     // Identity proxy
```

### SQL Classes
```cpp
#include <QSqlDatabase>              // Database connection
#include <QSqlQuery>                 // SQL query execution
#include <QSqlError>                 // SQL errors
#include <QSqlQueryModel>            // SQL query model (read-only)
#include <QSqlTableModel>            // SQL table model (editable)
#include <QSqlRelationalTableModel>  // SQL with foreign keys
#include <QSqlRelation>              // Foreign key relation
#include <QSqlRelationalDelegate>    // Delegate for relations
#include <QSqlRecord>                // SQL record
#include <QSqlField>                 // SQL field
```

### Dialogs
```cpp
#include <QDialog>          // Dialog base
#include <QMessageBox>      // Message box
#include <QFileDialog>      // File picker
#include <QColorDialog>     // Color picker
#include <QFontDialog>      // Font picker
#include <QInputDialog>     // Input dialog
#include <QProgressDialog>  // Progress dialog
```

### Menus & Toolbars
```cpp
#include <QMenuBar>         // Menu bar
#include <QMenu>            // Menu
#include <QAction>          // Action
#include <QToolBar>         // Toolbar
#include <QToolButton>      // Tool button
#include <QStatusBar>       // Status bar
```

### Graphics & Painting
```cpp
#include <QPainter>         // Painting
#include <QPen>             // Pen for drawing
#include <QBrush>           // Brush for filling
#include <QColor>           // Color
#include <QFont>            // Font
#include <QPixmap>          // Pixmap image
#include <QImage>           // Image
#include <QIcon>            // Icon
```

---

## 🗂️ Includes by File Type

### List Model File (example: 01_list_model.cpp)
```cpp
#include <QApplication>
#include <QWidget>
#include <QAbstractListModel>
#include <QListView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QFont>
#include <QColor>
```

### Table Model File (example: 02_table_model.cpp)
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

### Tree Model File (example: 03_tree_model.cpp)
```cpp
#include <QApplication>
#include <QWidget>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QFont>
#include <QColor>
#include <QIcon>
```

### Proxy Model File (example: 04_proxy_model.cpp)
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

### SQL Models File (example: 05_qsql_models.cpp)
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
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>
```

### Complete Example (example: 06_complete_example.cpp)
```cpp
#include <QApplication>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>          // ← For form layout
#include <QGroupBox>             // ← For filter group
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QDate>                 // ← For QDate::currentDate()
#include <QDebug>
```

---

## 🔍 Error Message to Include Mapping

| Error Message | Missing Include |
|--------------|-----------------|
| `'QPushButton' was not declared` | `#include <QPushButton>` |
| `'QLabel' was not declared` | `#include <QLabel>` |
| `'QLineEdit' was not declared` | `#include <QLineEdit>` |
| `'QComboBox' was not declared` | `#include <QComboBox>` |
| `'QCheckBox' was not declared` | `#include <QCheckBox>` |
| `'QSpinBox' was not declared` | `#include <QSpinBox>` |
| `'QVBoxLayout' was not declared` | `#include <QVBoxLayout>` |
| `'QHBoxLayout' was not declared` | `#include <QHBoxLayout>` |
| `'QFormLayout' was not declared` | `#include <QFormLayout>` |
| `'QGroupBox' was not declared` | `#include <QGroupBox>` |
| `'QTableView' was not declared` | `#include <QTableView>` |
| `'QListView' was not declared` | `#include <QListView>` |
| `'QTreeView' was not declared` | `#include <QTreeView>` |
| `'QMessageBox' was not declared` | `#include <QMessageBox>` |
| `'QDate' was not declared` | `#include <QDate>` |
| `incomplete type 'class QLabel'` | `#include <QLabel>` (forward declaration not enough) |
| `incomplete type 'class QDate'` | `#include <QDate>` |
| `invalid use of incomplete type` | Need full include, not forward declaration |

---

## 🚨 Common Pitfalls

### 1. Forward Declaration vs Full Include

❌ **WRONG** (can't create objects):
```cpp
// In header
class QLabel;  // Forward declaration

// In cpp
new QLabel("text");  // ERROR! Incomplete type
```

✅ **CORRECT**:
```cpp
// In cpp
#include <QLabel>  // Full definition
new QLabel("text");  // OK!
```

### 2. QDate Static Method

❌ **WRONG**:
```cpp
// No include
QDate::currentDate();  // ERROR! Incomplete type
```

✅ **CORRECT**:
```cpp
#include <QDate>
QDate::currentDate();  // OK!
```

### 3. Missing Layout Include

❌ **WRONG**:
```cpp
QFormLayout *layout = new QFormLayout();  // ERROR!
```

✅ **CORRECT**:
```cpp
#include <QFormLayout>
QFormLayout *layout = new QFormLayout();  // OK!
```

---

## 📝 Best Practices

### 1. Order of Includes

```cpp
// 1. Own header (if exists)
#include "MyWidget.h"

// 2. Qt includes
#include <QApplication>
#include <QWidget>
// ... other Qt

// 3. System includes
#include <iostream>
#include <algorithm>
```

### 2. Group Related Includes

```cpp
// Core
#include <QApplication>
#include <QWidget>

// Layouts
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

// Widgets
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

// Models
#include <QAbstractTableModel>
#include <QTableView>
```

### 3. Use #ifndef Guards in Headers

```cpp
// MyModel.h
#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>

class MyModel : public QAbstractTableModel {
    // ...
};

#endif // MYMODEL_H
```

---

## 🎯 Template Header File

Create `qt_common.h` for all projects:

```cpp
#ifndef QT_COMMON_H
#define QT_COMMON_H

// Core
#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>

// Layouts
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

// Widgets
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>

// Views
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>

// Models
#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

// Dialogs
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

// Graphics
#include <QFont>
#include <QColor>
#include <QIcon>

#endif // QT_COMMON_H
```

Then in each .cpp:
```cpp
#include "qt_common.h"
// That's it!
```

---

**📖 Keep this as reference when you get include errors!**
