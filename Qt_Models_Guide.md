# Hướng Dẫn Học Qt Models Nhanh Chóng

## 1. Tổng Quan về Qt Model/View Architecture

### Kiến trúc Model/View/Delegate
Qt sử dụng mô hình MVC được điều chỉnh thành Model/View với các thành phần:
- **Model**: Chứa dữ liệu và logic xử lý dữ liệu
- **View**: Hiển thị dữ liệu từ model
- **Delegate**: Xử lý việc hiển thị và chỉnh sửa từng item

### Lợi ích
1. **Tách biệt dữ liệu và hiển thị**: Dữ liệu độc lập với cách hiển thị
2. **Chia sẻ dữ liệu**: Nhiều view có thể dùng chung một model
3. **Hiệu năng cao**: Chỉ load và hiển thị dữ liệu cần thiết
4. **Linh hoạt**: Dễ dàng thay đổi cách hiển thị hoặc nguồn dữ liệu

## 2. Các Loại Models Chính

### 2.1 List Models
- **QStringListModel**: Model đơn giản cho danh sách string
- **QListWidget**: Widget kết hợp model và view (tiện lợi nhưng ít linh hoạt)

### 2.2 Table Models
- **QStandardItemModel**: Model đa năng cho table/tree
- **QTableWidget**: Widget kết hợp cho table đơn giản

### 2.3 Tree Models
- **QStandardItemModel**: Có thể dùng cho tree
- **QTreeWidget**: Widget kết hợp cho tree đơn giản
- **Custom Tree Model**: Kế thừa QAbstractItemModel

### 2.4 Proxy Models
- **QSortFilterProxyModel**: Lọc và sắp xếp dữ liệu
- **QIdentityProxyModel**: Base class cho custom proxy

### 2.5 SQL Models
- **QSqlTableModel**: Làm việc với một bảng SQL
- **QSqlQueryModel**: Chỉ đọc kết quả từ query
- **QSqlRelationalTableModel**: Xử lý foreign keys

## 3. Mối Quan Hệ Giữa Các Models

```
                    QAbstractItemModel (Base)
                           |
        +------------------+------------------+
        |                  |                  |
   QAbstractListModel  QAbstractTableModel  QAbstractProxyModel
        |                  |                  |
   QStringListModel   QSqlTableModel    QSortFilterProxyModel
                           |
                   QSqlRelationalTableModel
```

## 4. Cách Học Hiệu Quả

### Bước 1: Hiểu Interface Cơ Bản
Học các methods quan trọng của QAbstractItemModel:
- `data()`: Lấy dữ liệu
- `setData()`: Set dữ liệu  
- `rowCount()`: Số hàng
- `columnCount()`: Số cột
- `index()`: Tạo QModelIndex
- `parent()`: Cho tree model

### Bước 2: Bắt Đầu Với Model Đơn Giản
1. QStringListModel cho list
2. QStandardItemModel cho table
3. Sau đó mới tới custom model

### Bước 3: Hiểu QModelIndex
- Là "con trỏ" tới vị trí trong model
- Chứa row, column, parent
- Là cách view và model giao tiếp

### Bước 4: Thực Hành Với Proxy Model
- Filter dữ liệu
- Sort dữ liệu
- Transform dữ liệu

### Bước 5: Kết Nối Database
- QSqlDatabase để kết nối
- QSqlTableModel cho CRUD operations
- QSqlQuery cho complex queries

## 5. Tips Đọc Code Nhanh

### Pattern Recognition
1. **Tìm signal/slot connections**: Xem model kết nối với view như thế nào
2. **Xem data() implementation**: Hiểu cách model trả về dữ liệu
3. **Check role handling**: Qt::DisplayRole, Qt::EditRole, custom roles

### Debug Techniques  
1. Dùng `qDebug()` in ra QModelIndex
2. Override `data()` để log requests
3. Dùng Model Test từ Qt

### Common Patterns
```cpp
// Pattern 1: Setup model với view
model->setData(...);
view->setModel(model);

// Pattern 2: Proxy model chain
sourceModel -> proxyModel1 -> proxyModel2 -> view

// Pattern 3: Custom role
enum CustomRoles {
    IdRole = Qt::UserRole + 1,
    DescriptionRole
};
```