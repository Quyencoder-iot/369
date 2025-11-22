# Hướng Dẫn Học Qt Models - Model/View Architecture

Tài liệu này giúp bạn học nhanh cách đọc và hiểu code Qt Models.

## 📁 Cấu Trúc Project

```
/workspace/
├── QT_MODELS_GUIDE.md          # Tài liệu hướng dẫn chi tiết (tiếng Việt)
├── examples/
│   ├── list_model_example.cpp  # Ví dụ QAbstractListModel
│   ├── table_model_example.cpp # Ví dụ QAbstractTableModel
│   ├── tree_model_example.cpp  # Ví dụ Tree Model
│   ├── proxy_model_example.cpp # Ví dụ Proxy Model
│   ├── sql_model_example.cpp   # Ví dụ SQL Models
│   ├── complete_example.cpp    # Ví dụ tổng hợp đầy đủ
│   └── CMakeLists.txt          # Build configuration
└── README.md                   # File này
```

## 🚀 Bắt Đầu Nhanh

### 1. Đọc Tài Liệu
Mở file `QT_MODELS_GUIDE.md` để đọc hướng dẫn chi tiết về:
- Kiến trúc Model/View
- Các loại Model (List, Table, Tree, Proxy, SQL)
- Mối quan hệ giữa chúng
- Cách đọc code nhanh

### 2. Xem Ví Dụ Code
Mỗi file trong thư mục `examples/` là một ví dụ độc lập:
- `list_model_example.cpp` - Model cho danh sách 1 chiều
- `table_model_example.cpp` - Model cho bảng 2 chiều
- `tree_model_example.cpp` - Model cho cấu trúc cây
- `proxy_model_example.cpp` - Proxy model để lọc/sắp xếp
- `sql_model_example.cpp` - Model kết nối database
- `complete_example.cpp` - Ví dụ tổng hợp đầy đủ

### 3. Build và Chạy

```bash
cd examples
mkdir build && cd build
cmake ..
make

# Chạy các ví dụ
./complete_example        # Ví dụ tổng hợp (khuyến nghị)
./list_model_example
./table_model_example
./tree_model_example
./proxy_model_example
./sql_model_example
```

## 📚 Kiến Trúc Tổng Quan

```
View (QListView, QTableView, QTreeView)
    ↓ setModel()
Model (QAbstractItemModel và các subclass)
    ↓ truy cập
Data Source (List, Database, File...)
```

## 🔑 Các Khái Niệm Quan Trọng

### 1. QAbstractItemModel
- Lớp cơ sở của TẤT CẢ các model
- Định nghĩa interface chung
- Không thể dùng trực tiếp (abstract class)

### 2. Các Loại Model

#### QAbstractListModel
- Dữ liệu 1 chiều (danh sách)
- Chỉ có `row`, không có `column`
- Ví dụ: Danh sách tên, số điện thoại...

#### QAbstractTableModel
- Dữ liệu 2 chiều (bảng)
- Có cả `row` và `column`
- Ví dụ: Bảng Excel, bảng database...

#### Tree Model (QAbstractItemModel)
- Dữ liệu phân cấp (cây)
- Có parent-child relationship
- Ví dụ: File system, menu đa cấp...

#### QAbstractProxyModel
- Không chứa dữ liệu thực tế
- Làm việc với một source model khác
- Dùng để lọc, sắp xếp, chuyển đổi

#### QSqlQueryModel & QSqlTableModel
- Kết nối với database
- QSqlQueryModel: Read-only
- QSqlTableModel: Read-write

## 🔗 Mối Quan Hệ

```
QAbstractItemModel (base)
    ├── QAbstractListModel → QStringListModel
    ├── QAbstractTableModel → QStandardItemModel
    ├── QAbstractItemModel → QStandardItemModel (tree)
    ├── QAbstractProxyModel → QSortFilterProxyModel
    └── QSqlQueryModel → QSqlTableModel
```

## 💡 Cách Đọc Code Nhanh

1. **Xác định loại Model**: Tìm class declaration
2. **Tìm nguồn dữ liệu**: Tìm member variables
3. **Đọc các phương thức bắt buộc**: `rowCount()`, `data()`, `index()`
4. **Tìm cách sử dụng**: `setModel()`, `setSourceModel()`

Xem chi tiết trong `QT_MODELS_GUIDE.md` phần "Cách Đọc Code Nhanh".

## 📖 Tài Liệu Tham Khảo

- [Qt Model/View Programming](https://doc.qt.io/qt-5/model-view-programming.html)
- [QAbstractItemModel](https://doc.qt.io/qt-5/qabstractitemmodel.html)
- [Model/View Tutorial](https://doc.qt.io/qt-5/modelview.html)

## 🎯 Checklist Học Tập

- [ ] Đọc `QT_MODELS_GUIDE.md`
- [ ] Xem ví dụ `list_model_example.cpp`
- [ ] Xem ví dụ `table_model_example.cpp`
- [ ] Xem ví dụ `tree_model_example.cpp`
- [ ] Xem ví dụ `proxy_model_example.cpp`
- [ ] Xem ví dụ `sql_model_example.cpp`
- [ ] Chạy `complete_example.cpp` và thử nghiệm
- [ ] Tự viết một model đơn giản

Chúc bạn học tốt! 🚀
