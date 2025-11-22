# Qt Models Tutorial - Hướng Dẫn Học Nhanh

## 📚 Giới Thiệu

Project này giúp bạn học nhanh về Qt Model/View Architecture với các ví dụ thực tế:

- **QAbstractListModel** - Model cho danh sách
- **QAbstractTableModel** - Model cho bảng  
- **QAbstractItemModel** - Model cho cấu trúc cây
- **QSortFilterProxyModel** - Lọc và sắp xếp dữ liệu
- **QSql Models** - Làm việc với database

## 🚀 Build và Chạy

### Yêu cầu
- Qt 6.x (hoặc Qt 5.x với chỉnh sửa CMakeLists.txt)
- CMake 3.16+
- C++ compiler với C++17 support
- SQLite (đã include trong Qt)

### Build Steps

```bash
# 1. Tạo build directory
mkdir build
cd build

# 2. Configure với CMake
cmake ..

# 3. Build
make -j4  # Linux/Mac
# hoặc
cmake --build . --config Release  # Windows

# 4. Chạy main app
./qt_models_demo
```

### Chạy từng ví dụ riêng

```bash
./list_model_example     # Demo List Model
./table_model_example    # Demo Table Model
./tree_model_example     # Demo Tree Model
./proxy_model_example    # Demo Proxy Model
./sql_model_example      # Demo SQL Models
```

## 📖 Tài Liệu

1. **Lý thuyết**: [`docs/HUONG_DAN_QT_MODELS.md`](docs/HUONG_DAN_QT_MODELS.md)
2. **Mối quan hệ**: [`docs/MOI_QUAN_HE_MODELS.md`](docs/MOI_QUAN_HE_MODELS.md)
3. **Source code**: Thư mục `examples/`

## 🎯 Lộ Trình Học Đề Xuất

1. **Tuần 1: Cơ bản**
   - Đọc tổng quan về Model/View
   - Thực hành QAbstractListModel
   - Master các methods: rowCount(), data(), setData()

2. **Tuần 2: Table Model** 
   - Hiểu thêm columnCount(), headerData()
   - Thực hành CRUD operations
   - Custom roles và display

3. **Tuần 3: Tree Model**
   - Hiểu index() và parent()
   - QModelIndex và tree navigation
   - Hierarchical data structures

4. **Tuần 4: Advanced**
   - Proxy models cho filtering
   - SQL integration
   - Performance optimization

## 💡 Tips Học Nhanh

### Đọc Code Hiệu Quả

1. **Nhận diện patterns**:
```cpp
// List Model pattern
int rowCount(...) { return m_list.size(); }

// Table Model pattern  
int columnCount(...) { return COLUMN_COUNT; }

// Tree Model pattern
QModelIndex parent(...) { /* complex logic */ }
```

2. **Focus vào signals**:
- `beginInsertRows()` / `endInsertRows()`
- `dataChanged()`
- `layoutChanged()`

3. **Debug với qDebug()**:
```cpp
qDebug() << "Index:" << index << "Role:" << role;
```

### Common Mistakes

❌ **Sai**: Quên emit signals khi data thay đổi
✅ **Đúng**: Luôn emit `dataChanged()` sau khi update

❌ **Sai**: Không check `index.isValid()`
✅ **Đúng**: Validate index trước khi dùng

❌ **Sai**: Memory leak với tree nodes
✅ **Đúng**: Proper parent-child ownership

## 🔧 Troubleshooting

### Lỗi build
```bash
# Qt không tìm thấy
export CMAKE_PREFIX_PATH=/path/to/qt6

# Lỗi MOC
make clean && cmake .. && make
```

### Lỗi runtime
- Database error: Check SQLite driver
- Crash: Debug với `gdb` hoặc Qt Creator
- Empty views: Check model's rowCount()

## 📝 Exercises

1. **Easy**: Modify list model để support drag & drop
2. **Medium**: Add search highlight trong proxy model
3. **Hard**: Implement lazy loading cho tree model
4. **Expert**: Custom delegate với inline editing

## 🤝 Contributing

Feel free to:
- Add more examples
- Improve documentation
- Fix bugs
- Suggest improvements

## 📄 License

Educational purposes - Free to use and modify!

---

**Happy Learning! 🚀**

Nếu gặp khó khăn, hãy:
1. Check documentation Qt chính thức
2. Debug step by step
3. Ask trên Qt forums