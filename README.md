# Qt Models Learning Guide - Hướng dẫn học Qt Models

Đây là bộ hướng dẫn và ví dụ hoàn chỉnh về Qt Model/View Architecture, bao gồm tất cả các loại models quan trọng trong Qt.

## 📚 Nội dung

### 📖 Tài liệu
- **Qt_Models_Guide.md**: Hướng dẫn tổng quan về Qt Model/View Architecture
- **Qt_Models_Relationships.md**: Sơ đồ và giải thích mối quan hệ giữa các models

### 💻 Ví dụ Code
1. **1_string_list_model.cpp**: QStringListModel - List đơn giản
2. **2_table_model.cpp**: QStandardItemModel dùng cho table
3. **3_tree_model.cpp**: Tree structure với QStandardItemModel
4. **4_proxy_model.cpp**: QSortFilterProxyModel cho filter và sort
5. **5_sql_models.cpp**: SQL Models (QSqlTableModel, QSqlQueryModel, QSqlRelationalTableModel)
6. **6_complete_demo.cpp**: Ứng dụng demo tổng hợp tất cả models

## 🛠️ Cài đặt và Build

### Yêu cầu
- Qt 6.0 trở lên (hoặc Qt 5.15 với chỉnh sửa nhỏ)
- CMake 3.16+
- Compiler hỗ trợ C++17

### Build Instructions

```bash
# Clone hoặc download project
cd /workspace

# Tạo thư mục build
mkdir build
cd build

# Configure với CMake
cmake ..

# Build tất cả examples
make

# Hoặc build từng example
make string_list_example
make table_model_example
make tree_model_example
make proxy_model_example
make sql_model_example
make complete_demo
```

## 🚀 Chạy các ví dụ

```bash
# Từ thư mục build
./string_list_example  # List model cơ bản
./table_model_example  # Table với custom data
./tree_model_example   # Hierarchical data
./proxy_model_example  # Filter và sort
./sql_model_example    # Database models
./complete_demo        # Ứng dụng hoàn chỉnh
```

## 📝 Hướng dẫn học

### 1. Bắt đầu với cơ bản
- Đọc **Qt_Models_Guide.md** để hiểu tổng quan
- Chạy **string_list_example** - model đơn giản nhất
- Thử edit, thêm, xóa items để hiểu cách model hoạt động

### 2. Table và Tree
- Chạy **table_model_example** để học về:
  - Multiple columns
  - Custom data roles
  - Icons và checkboxes
  
- Chạy **tree_model_example** để học về:
  - Parent-child relationships
  - Recursive structures
  - Drag & drop

### 3. Proxy Models
- Chạy **proxy_model_example** để hiểu:
  - Filter không làm thay đổi source data
  - Chain nhiều proxy models
  - Custom filter logic

### 4. Database Integration
- Chạy **sql_model_example** để học:
  - CRUD operations với database
  - Foreign key relationships
  - Query models cho reports

### 5. Tích hợp hoàn chỉnh
- Chạy **complete_demo** để xem cách:
  - Kết hợp nhiều loại models
  - Master-detail views
  - Form binding với QDataWidgetMapper
  - Real-world application structure

## 🔑 Key Concepts cần nhớ

### QModelIndex
- "Con trỏ" đến vị trí trong model
- Chứa row, column, parent
- Valid vs Invalid index

### Roles
- `Qt::DisplayRole`: Text hiển thị
- `Qt::EditRole`: Data để edit
- `Qt::UserRole + X`: Custom data

### Signals quan trọng
- `dataChanged()`: Khi data thay đổi
- `rowsInserted()`: Khi thêm rows
- `modelReset()`: Khi rebuild toàn bộ

### Edit Strategies (SQL Models)
- `OnFieldChange`: Save ngay khi edit
- `OnRowChange`: Save khi đổi row
- `OnManualSubmit`: Save thủ công

## 🎯 Tips để đọc code nhanh

1. **Tìm Model-View Connection**:
   ```cpp
   view->setModel(model);  // Điểm kết nối
   ```

2. **Check Data Flow**:
   ```cpp
   model->data()     // Model → View
   model->setData()  // View → Model
   ```

3. **Debug với qDebug()**:
   ```cpp
   qDebug() << index.row() << index.column();
   qDebug() << model->rowCount();
   ```

## 📚 Tài liệu tham khảo

- [Qt Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html)
- [Qt Model/View Tutorial](https://doc.qt.io/qt-6/modelview.html)
- [QAbstractItemModel](https://doc.qt.io/qt-6/qabstractitemmodel.html)

## 🤝 Đóng góp

Nếu bạn tìm thấy lỗi hoặc muốn thêm ví dụ mới, vui lòng tạo issue hoặc pull request.

## 📄 License

MIT License - Tự do sử dụng cho mục đích học tập và thương mại.