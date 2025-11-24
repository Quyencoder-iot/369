# 🎓 Qt Models & QSql Learning Guide

Hướng dẫn toàn diện về Qt Model/View framework và QSql, kèm theo ví dụ thực hành.

## 📚 Tài Liệu

### 🎨 NEW: QML Examples (Model C++ + View QML)
- **[qml_examples/](qml_examples/)** - Examples với QML Views
- **[QUICK_START.md](qml_examples/QUICK_START.md)** - Bắt đầu nhanh với QML
- **[QML_VS_WIDGETS_GUIDE.md](qml_examples/QML_VS_WIDGETS_GUIDE.md)** - So sánh QML vs Qt Widgets

### 📖 Main Guides

1. **[QT_MODELS_GUIDE.md](QT_MODELS_GUIDE.md)** - Hướng dẫn chi tiết từ A-Z (Qt Widgets)
   - Tổng quan Model/View Architecture
   - QAbstractListModel, QAbstractTableModel, QAbstractItemModel
   - QSql Models (QSqlQueryModel, QSqlTableModel, QSqlRelationalTableModel)
   - Proxy Models
   - Tips đọc code nhanh
   - Lộ trình học

2. **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Tra cứu nhanh
   - Chọn model phù hợp
   - Phương thức bắt buộc
   - Data roles
   - Templates cho Insert/Delete/Update
   - Common mistakes
   - Checklist đọc code

3. **[VISUAL_GUIDE.md](VISUAL_GUIDE.md)** - Sơ đồ trực quan
   - Model hierarchy
   - Data flow diagrams
   - Tree structure visualization
   - SQL model architecture
   - Proxy chains
   - Memory layouts

### 💻 Examples

#### Qt Widgets Examples (C++ UI):
Thư mục `examples/` chứa 6 ví dụ hoàn chỉnh:

```
examples/
├── 01_list_model.cpp          - QAbstractListModel (Todo List)
├── 02_table_model.cpp         - QAbstractTableModel (Student Table)
├── 03_tree_model.cpp          - QAbstractItemModel (File System Tree)
├── 04_proxy_model.cpp         - QSortFilterProxyModel (Product Filter)
├── 05_qsql_models.cpp         - QSql Models (Database)
├── 06_complete_example.cpp    - Complete App (SQL + Proxy)
├── CMakeLists.txt             - Build configuration
└── README.md                  - Examples documentation
```

#### QML Examples (C++ Model + QML View):
Thư mục `qml_examples/` - Kiến trúc hiện đại:

```
qml_examples/
├── models/                    - C++ Models
│   ├── TodoListModel.h/cpp
│   └── StudentTableModel.h/cpp
├── qml/                       - QML Views
│   ├── TodoListView.qml
│   └── StudentTableView.qml
├── main_todo.cpp              - Todo app
├── main_student.cpp           - Student app
├── CMakeLists.txt
├── QUICK_START.md             - Bắt đầu nhanh
└── QML_VS_WIDGETS_GUIDE.md    - So sánh chi tiết
```

## 🚀 Quick Start

### 1. Đọc Tài Liệu

```bash
# Bắt đầu với guide tổng quan
cat QT_MODELS_GUIDE.md | less

# Xem sơ đồ trực quan
cat VISUAL_GUIDE.md | less

# Bookmark quick reference để tra cứu
cat QUICK_REFERENCE.md | less
```

### 2. Build Examples (Qt5 hoặc Qt6)

```bash
cd examples
mkdir build && cd build

# Auto-detect Qt version
cmake ..
cmake --build .

# Hoặc chỉ định Qt path
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64
cmake --build .
```

📖 **Hướng dẫn build chi tiết:** [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)

> ✅ **Tương thích:** Qt 5.6+ và Qt 6.x

### 3. Chạy Examples

```bash
# Bắt đầu với list model
./example_01_list_model

# Sau đó thử table model
./example_02_table_model

# Và các examples khác...
```

## 📊 Tổng Quan Kiến Trúc

```
┌─────────────────────────────────────────────┐
│        Qt Model/View Architecture            │
├─────────────────────────────────────────────┤
│                                              │
│  ┌─────────┐      ┌──────────┐             │
│  │  MODEL  │ ◄────┤   VIEW   │             │
│  │ (Data)  │      │(Display) │             │
│  └─────────┘      └──────────┘             │
│       ▲                ▲                     │
│       │                │                     │
│       └────┬───────────┘                     │
│            │                                 │
│      ┌─────▼──────┐                         │
│      │ DELEGATE   │                         │
│      │ (Editing)  │                         │
│      └────────────┘                         │
└─────────────────────────────────────────────┘
```

## 🗺️ Lộ Trình Học (4 Tuần)

### Tuần 1: Cơ Bản
- [ ] Đọc phần "Model/View Architecture" trong guide
- [ ] Làm example 01: QAbstractListModel
- [ ] Làm example 02: QAbstractTableModel
- [ ] Hiểu rõ rowCount(), data(), setData()

### Tuần 2: Database
- [ ] Đọc phần "QSql Models" trong guide
- [ ] Setup QSqlDatabase
- [ ] Làm example 05: QSql Models
- [ ] Thực hành CRUD operations

### Tuần 3: Advanced
- [ ] Làm example 03: Tree Model (QAbstractItemModel)
- [ ] Hiểu rõ index() và parent()
- [ ] Làm example 04: Proxy Model
- [ ] Custom filterAcceptsRow()

### Tuần 4: Integration
- [ ] Làm example 06: Complete Example
- [ ] Kết hợp SQL + Proxy
- [ ] Build một app nhỏ của riêng bạn
- [ ] Review toàn bộ kiến thức

## 🎯 Chọn Model Phù Hợp

| Nhu cầu | Model |
|---------|-------|
| Danh sách đơn giản | QStringListModel |
| Danh sách custom | QAbstractListModel |
| Bảng 2D | QAbstractTableModel |
| Cây phân cấp | QAbstractItemModel |
| Database read-only | QSqlQueryModel |
| Database editable | QSqlTableModel |
| Foreign keys | QSqlRelationalTableModel |
| Filter/Sort | QSortFilterProxyModel |

## 📖 Concepts Chính

### 1. Model (Quản lý dữ liệu)
- `rowCount()` - Số hàng
- `columnCount()` - Số cột
- `data()` - Trả về dữ liệu cho mỗi cell
- `setData()` - Cập nhật dữ liệu
- `index()` - Tạo index (cho tree)
- `parent()` - Tìm parent (cho tree)

### 2. View (Hiển thị)
- `QListView` - Danh sách
- `QTableView` - Bảng
- `QTreeView` - Cây

### 3. Roles (Vai trò dữ liệu)
- `Qt::DisplayRole` - Text hiển thị
- `Qt::EditRole` - Text khi edit
- `Qt::DecorationRole` - Icon
- `Qt::BackgroundRole` - Màu nền
- `Qt::ForegroundRole` - Màu chữ
- `Qt::UserRole + N` - Custom data

### 4. Signals (Thông báo thay đổi)
- `dataChanged()` - Data đã thay đổi
- `rowsInserted()` - Thêm rows
- `rowsRemoved()` - Xóa rows
- `layoutChanged()` - Cấu trúc thay đổi

## 🔍 Tips Đọc Code Nhanh

Khi đọc Qt Model code:

1. ✅ Xác định loại model (List/Table/Tree/SQL/Proxy)
2. ✅ Tìm data storage (QVector, QStringList, Node*, etc.)
3. ✅ Xem rowCount() và columnCount()
4. ✅ Đọc data() - xử lý roles như thế nào
5. ✅ Check setData() - có editable không?
6. ✅ Tìm index()/parent() - tree structure?
7. ✅ Xem signals được emit khi nào

## 💡 Common Patterns

### Thêm Item
```cpp
beginInsertRows(parent, row, row);
m_items.append(item);
endInsertRows();
```

### Xóa Item
```cpp
beginRemoveRows(parent, row, row);
m_items.remove(row);
endRemoveRows();
```

### Update Item
```cpp
m_items[row] = newValue;
emit dataChanged(index, index, {role});
```

### Filter với Proxy
```cpp
QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
proxy->setSourceModel(sourceModel);
proxy->setFilterRegularExpression(QRegularExpression("pattern"));
view->setModel(proxy); // View uses proxy!
```

## 🐛 Common Mistakes

❌ **WRONG**
```cpp
m_items.append(item); // Quên beginInsertRows()!
```

✅ **CORRECT**
```cpp
beginInsertRows(QModelIndex(), row, row);
m_items.append(item);
endInsertRows();
```

❌ **WRONG**
```cpp
// View dùng source thay vì proxy
view->setModel(sourceModel);
```

✅ **CORRECT**
```cpp
view->setModel(proxyModel);
QModelIndex sourceIdx = proxy->mapToSource(viewIdx);
```

## 🔗 Resources

### Official Documentation
- [Qt Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html)
- [QAbstractItemModel](https://doc.qt.io/qt-6/qabstractitemmodel.html)
- [QSql Classes](https://doc.qt.io/qt-6/qtsql-index.html)

### Books
- "Advanced Qt Programming" - Mark Summerfield
- "C++ GUI Programming with Qt 5" - Jasmin Blanchette

### Examples trong Qt
```bash
# Qt Creator → Welcome → Examples → Item Views
# hoặc
$QTDIR/examples/widgets/itemviews/
```

## 📝 Practice Projects

Để thực hành, hãy thử build:

1. **Todo App** - List model với filtering
2. **Contact Manager** - Table model với search
3. **File Explorer** - Tree model
4. **Database CRUD** - SQL models
5. **Product Catalog** - SQL + Proxy filters

## ❓ FAQ

**Q: List model vs Table model khác nhau gì?**
- List: 1 chiều, chỉ cần rowCount()
- Table: 2 chiều, cần rowCount() VÀ columnCount()

**Q: Khi nào dùng QSqlTableModel vs QSqlRelationalTableModel?**
- QSqlTableModel: Bảng đơn giản, không foreign keys
- QSqlRelationalTableModel: Có foreign keys, cần hiển thị tên thay vì ID

**Q: Proxy model có thay đổi source data không?**
- KHÔNG! Proxy chỉ filter/sort, không thay đổi source

**Q: Tree model khó ở chỗ nào?**
- Phải implement index() và parent() đúng
- Quản lý internalPointer() cho nodes

## 🤝 Contributing

Nếu bạn tìm thấy lỗi hoặc muốn cải thiện:
1. Tạo issue
2. Hoặc submit pull request

## 📜 License

Free to use for learning purposes.

---

**Chúc bạn học tốt! 🚀**

---

## 📞 Contact

Nếu có thắc mắc, hãy tạo issue trên GitHub!

---

**Made with ❤️ for Qt Learners**
