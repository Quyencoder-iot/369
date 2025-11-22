# Hướng Dẫn Học Qt Model/View Architecture

## 📚 Tổng Quan

Qt Model/View Architecture tách biệt dữ liệu (Model) khỏi cách hiển thị (View). Điều này giúp:
- Tái sử dụng code tốt hơn
- Dễ dàng thay đổi cách hiển thị mà không ảnh hưởng dữ liệu
- Hỗ trợ nhiều views cho cùng một model

## 🎯 Chiến Lược Học Nhanh

### 1. **Hiểu Kiến Trúc Cơ Bản**
```
Model (Dữ liệu) <---> View (Hiển thị)
        ^
        |
    Delegate (Tùy chỉnh hiển thị)
```

### 2. **Thứ Tự Học Đề Xuất**

1. **QAbstractItemModel** - Lớp cơ sở cho tất cả models
2. **QAbstractListModel** - Model 1 chiều (danh sách)
3. **QAbstractTableModel** - Model 2 chiều (bảng)
4. **QStandardItemModel** - Model đa năng, dễ sử dụng
5. **Proxy Models** - Lọc và sắp xếp dữ liệu
6. **QSql Models** - Làm việc với database

## 🔍 Chi Tiết Các Loại Model

### 1. **QAbstractListModel**
- **Dùng cho**: Danh sách đơn giản (1 cột)
- **Ví dụ**: Danh sách tên, menu items
- **Methods chính**:
  - `rowCount()` - Số lượng items
  - `data()` - Lấy dữ liệu
  - `setData()` - Cập nhật dữ liệu

### 2. **QAbstractTableModel**
- **Dùng cho**: Dữ liệu dạng bảng (nhiều cột)
- **Ví dụ**: Spreadsheet, danh sách sản phẩm
- **Methods thêm**:
  - `columnCount()` - Số lượng cột
  - `headerData()` - Tiêu đề cột/hàng

### 3. **QAbstractItemModel**
- **Dùng cho**: Cấu trúc phân cấp (tree)
- **Ví dụ**: File system, org chart
- **Methods phức tạp hơn**:
  - `index()` - Tạo model index
  - `parent()` - Lấy parent của item
  - `hasChildren()` - Kiểm tra có con không

### 4. **Proxy Models**
- **QSortFilterProxyModel**: Lọc và sắp xếp
- **QIdentityProxyModel**: Thay đổi cách hiển thị
- **Không chứa dữ liệu**, chỉ "proxy" từ model khác

### 5. **QSql Models**
- **QSqlTableModel**: Bảng đơn từ database
- **QSqlRelationalTableModel**: Bảng với foreign keys
- **QSqlQueryModel**: Kết quả từ SQL query

## 📊 Sơ Đồ Mối Quan Hệ

```
                    QAbstractItemModel
                           |
        +------------------+------------------+
        |                  |                  |
QAbstractListModel  QAbstractTableModel  QStandardItemModel
                           |                  |
                    QSqlTableModel     (Dùng cho mọi loại)
                           |
                QSqlRelationalTableModel

        QAbstractProxyModel
               |
    +----------+----------+
    |                     |
QSortFilterProxyModel  QIdentityProxyModel
```

## 💡 Tips Đọc Code Nhanh

### 1. **Nhận Diện Pattern**
```cpp
// List Model Pattern
int rowCount(const QModelIndex &parent) const override {
    Q_UNUSED(parent);
    return m_data.size();
}

// Table Model Pattern  
int columnCount(const QModelIndex &parent) const override {
    Q_UNUSED(parent);
    return m_columnCount;
}

// Tree Model Pattern
QModelIndex parent(const QModelIndex &index) const override {
    // Logic phức tạp hơn
}
```

### 2. **Roles Quan Trọng**
- `Qt::DisplayRole` - Text hiển thị
- `Qt::EditRole` - Dữ liệu khi edit
- `Qt::DecorationRole` - Icon
- `Qt::UserRole` - Custom data

### 3. **Signals Cần Nhớ**
- `beginInsertRows()` / `endInsertRows()`
- `beginRemoveRows()` / `endRemoveRows()`
- `dataChanged()`
- `layoutChanged()`

## 🚀 Workflow Học Hiệu Quả

1. **Bắt đầu với QStandardItemModel**
   - Dễ dùng, không cần subclass
   - Hiểu cách model/view hoạt động

2. **Thực hành với List Model**
   - Subclass QAbstractListModel
   - Implement các methods cơ bản

3. **Nâng cao với Table Model**
   - Thêm columns
   - Custom headers

4. **Master Tree Model**
   - Hiểu QModelIndex
   - Parent-child relationships

5. **Áp dụng Proxy Models**
   - Filtering
   - Sorting
   - Custom transformations

6. **Tích hợp Database**
   - QSqlTableModel cho CRUD đơn giản
   - QSqlQueryModel cho queries phức tạp

## 📝 Checklist Khi Đọc Code

- [ ] Model kế thừa từ class nào?
- [ ] Có bao nhiêu methods được override?
- [ ] Roles nào được sử dụng?
- [ ] Có emit signals không?
- [ ] Data được lưu ở đâu?
- [ ] Có sử dụng proxy model không?

## 🔧 Debug Tips

1. **Sử dụng qDebug()**
```cpp
qDebug() << "Row:" << index.row() << "Col:" << index.column();
qDebug() << "Role:" << role << "Data:" << m_data[index.row()];
```

2. **Check Model Index**
```cpp
if (!index.isValid()) return QVariant();
```

3. **Monitor Signals**
- Connect to dataChanged()
- Log beginInsertRows/endInsertRows

Tiếp theo, tôi sẽ tạo các ví dụ code cụ thể cho từng loại model!