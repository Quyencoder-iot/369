# Quick Reference - Qt Models

## 🔍 Cách Đọc Code Nhanh

### Bước 1: Tìm Model Declaration
```cpp
QAbstractListModel *model;        // → List Model
QAbstractTableModel *model;       // → Table Model  
QAbstractItemModel *model;        // → Tree Model hoặc custom
QSortFilterProxyModel *proxy;     // → Proxy Model
QSqlTableModel *model;            // → SQL Model
```

### Bước 2: Tìm View Connection
```cpp
view->setModel(model);  // Model nào được kết nối?
```

### Bước 3: Tìm Proxy Chain (nếu có)
```cpp
proxy->setSourceModel(sourceModel);  // Proxy lấy dữ liệu từ đâu?
view->setModel(proxy);               // View lấy dữ liệu từ đâu?
```

### Bước 4: Tìm Data Source
```cpp
// SQL Model
model->setTable("table_name");
model->select();

// Custom Model
// Tìm nơi lưu trữ dữ liệu (QList, QVector, database...)
```

## 📊 Bảng So Sánh Models

| Model | Hàng | Cột | Parent/Child | Chỉnh Sửa | Database |
|-------|------|-----|--------------|-----------|----------|
| QAbstractListModel | ✅ | ❌ (1) | ❌ | ✅ | ❌ |
| QAbstractTableModel | ✅ | ✅ | ❌ | ✅ | ❌ |
| QAbstractItemModel (Tree) | ✅ | ✅ | ✅ | ✅ | ❌ |
| QSortFilterProxyModel | ✅ | ✅ | ✅ | ✅ | ❌ |
| QSqlQueryModel | ✅ | ✅ | ❌ | ❌ | ✅ |
| QSqlTableModel | ✅ | ✅ | ❌ | ✅ | ✅ |
| QSqlRelationalTableModel | ✅ | ✅ | ❌ | ✅ | ✅ |

## 🎯 Hàm Quan Trọng Cần Tìm

### Tất Cả Models
- `rowCount()` - Số hàng
- `data()` - Dữ liệu hiển thị
- `setModel()` - Kết nối với view

### Table Model
- `columnCount()` - Số cột
- `headerData()` - Tên cột

### Tree Model
- `index()` - Tạo index
- `parent()` - Parent của index

### Proxy Model
- `setSourceModel()` - Model gốc
- `setFilterKeyColumn()` - Cột lọc
- `setFilterFixedString()` - Chuỗi lọc

### SQL Model
- `setTable()` - Tên bảng
- `select()` - Load dữ liệu
- `submitAll()` - Lưu thay đổi

## 🔗 Mối Quan Hệ Thường Gặp

### Pattern 1: SQL + Proxy + View
```
QSqlTableModel → QSortFilterProxyModel → QTableView
```
**Dùng khi**: Cần lọc/sắp xếp dữ liệu database mà không query lại

### Pattern 2: Custom Model + View
```
CustomModel (kế thừa QAbstractTableModel) → QTableView
```
**Dùng khi**: Dữ liệu không từ database, cần logic tùy chỉnh

### Pattern 3: Proxy Chain
```
SourceModel → Proxy1 → Proxy2 → View
```
**Dùng khi**: Cần nhiều lớp xử lý (lọc + transform + sort...)

## ⚡ Tips Debug

1. **Đặt breakpoint tại `data()`** - Xem khi nào được gọi và với index nào
2. **Kiểm tra `index.isValid()`** - Nhiều bug do invalid index
3. **Xem `role` parameter** - Qt::DisplayRole, Qt::EditRole, Qt::BackgroundRole...
4. **Kiểm tra `parent` parameter** - Quan trọng với tree model
5. **Xem signals**: `dataChanged()`, `modelReset()`, `rowsInserted()`...

## 🚨 Lỗi Thường Gặp

### Lỗi 1: View không hiển thị gì
- ✅ Kiểm tra `rowCount()` có trả về > 0?
- ✅ Kiểm tra `data()` có trả về QVariant hợp lệ?
- ✅ Kiểm tra `view->setModel()` đã được gọi?

### Lỗi 2: Proxy không lọc được
- ✅ Kiểm tra `setSourceModel()` đã được gọi?
- ✅ Kiểm tra `setFilterKeyColumn()` đúng cột?
- ✅ View có dùng proxy hay dùng source model?

### Lỗi 3: SQL Model không load được
- ✅ Database đã mở (`db.open()`)?
- ✅ `select()` đã được gọi?
- ✅ Kiểm tra `lastError()`?

### Lỗi 4: Tree Model không hiển thị đúng
- ✅ `index()` có trả về valid index?
- ✅ `parent()` có implement đúng?
- ✅ `rowCount()` có tính đúng số con?

## 📝 Checklist Code Review

Khi đọc code Qt Model, kiểm tra:

- [ ] Model nào được sử dụng?
- [ ] View nào được kết nối?
- [ ] Có proxy model không? Nếu có, chain như thế nào?
- [ ] Dữ liệu đến từ đâu? (Database/Memory/File...)
- [ ] Model có thể chỉnh sửa không? (`setData()`, `flags()`)
- [ ] Có signals/slots nào liên quan? (`dataChanged()`, `modelReset()`...)
- [ ] Có filter/sort không? Ở đâu? (Proxy hay Model?)
- [ ] Performance: Có virtualize không? Có cache không?
