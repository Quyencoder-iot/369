# Qt Models Examples

Các ví dụ thực hành cho Qt Model/View framework.

## 📦 Yêu Cầu

- Qt 6.x
- CMake 3.16+
- C++17 compiler

## 🔨 Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 🚀 Chạy Examples

```bash
# Example 1: List Model - Todo List
./example_01_list_model

# Example 2: Table Model - Student Table
./example_02_table_model

# Example 3: Tree Model - File System
./example_03_tree_model

# Example 4: Proxy Model - Product Filter
./example_04_proxy_model

# Example 5: QSql Models - Database Tables
./example_05_qsql_models

# Example 6: Complete - Employee Management
./example_06_complete
```

## 📚 Mô Tả Examples

### 01_list_model.cpp
- **Nội dung:** Todo list với QAbstractListModel
- **Học được:**
  - Implement rowCount() và data()
  - Sử dụng các roles (Display, Check, Foreground)
  - Thêm/xóa items với beginInsertRows/endInsertRows
  - Edit data với setData()

### 02_table_model.cpp
- **Nội dung:** Bảng quản lý sinh viên
- **Học được:**
  - Implement rowCount(), columnCount(), data()
  - headerData() cho tiêu đề
  - Tùy chỉnh màu sắc, font
  - Sorting
  - Edit table cells

### 03_tree_model.cpp
- **Nội dung:** File system tree structure
- **Học được:**
  - Cấu trúc Node với parent/children
  - Implement index() và parent()
  - QModelIndex.internalPointer()
  - Tree navigation
  - Add/remove nodes

### 04_proxy_model.cpp
- **Nội dung:** Product list với filtering
- **Học được:**
  - QSortFilterProxyModel
  - Custom filterAcceptsRow()
  - Multiple filter criteria
  - Sorting với lessThan()
  - invalidateFilter()

### 05_qsql_models.cpp
- **Nội dung:** Database models với SQLite
- **Học được:**
  - QSqlDatabase setup
  - QSqlQueryModel (read-only)
  - QSqlTableModel (editable)
  - QSqlRelationalTableModel (foreign keys)
  - submitAll() / revertAll()

### 06_complete_example.cpp
- **Nội dung:** Employee management system hoàn chỉnh
- **Học được:**
  - Kết hợp SQL Model + Proxy Model
  - Multiple filters
  - Index mapping (mapToSource/mapFromSource)
  - Complete CRUD operations
  - Professional UI

## 🎯 Lộ Trình Học

1. Bắt đầu với `01_list_model.cpp` - Đơn giản nhất
2. Tiếp theo `02_table_model.cpp` - Thêm cột
3. Sau đó `04_proxy_model.cpp` - Filtering
4. Tiếp `05_qsql_models.cpp` - Database
5. Thử `03_tree_model.cpp` - Phức tạp hơn
6. Cuối cùng `06_complete_example.cpp` - Tổng hợp

## 💡 Tips

- Đọc comments trong code để hiểu chi tiết
- Thử chỉnh sửa code để thực hành
- Debug với qDebug() để xem data flow
- Tham khảo `QUICK_REFERENCE.md` và `VISUAL_GUIDE.md` ở thư mục root

## 📖 Tài Liệu Liên Quan

- `../QT_MODELS_GUIDE.md` - Hướng dẫn toàn diện
- `../QUICK_REFERENCE.md` - Tra cứu nhanh
- `../VISUAL_GUIDE.md` - Sơ đồ trực quan

## ⚠️ Lưu Ý

- Examples sử dụng in-memory SQLite database
- Không cần cài đặt database riêng
- Mỗi example độc lập, có thể chạy riêng lẻ
- Code có nhiều comments tiếng Việt để dễ hiểu

## 🐛 Troubleshooting

### Qt không tìm thấy
```bash
# Set Qt path (adjust version as needed)
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
```

### Build errors
```bash
# Clean rebuild
rm -rf build
mkdir build
cd build
cmake ..
make
```

### Runtime errors
```bash
# Ensure Qt libraries are in path
export LD_LIBRARY_PATH=/path/to/Qt/6.x.x/gcc_64/lib:$LD_LIBRARY_PATH
```

---

**Happy Learning! 📚**
