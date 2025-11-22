# Hướng Dẫn Học Qt Model/View Architecture

## 📚 Tổng Quan Kiến Trúc Model/View

Qt sử dụng kiến trúc **Model/View** để tách biệt dữ liệu (Model) và giao diện hiển thị (View). Điều này giúp:
- Tái sử dụng dữ liệu với nhiều view khác nhau
- Dễ dàng thay đổi cách hiển thị mà không ảnh hưởng dữ liệu
- Hiệu suất tốt với dữ liệu lớn

```
┌─────────────┐
│    Model    │  ← Dữ liệu (QAbstractItemModel)
└──────┬──────┘
       │
       │ signals/slots
       │
┌──────▼──────┐
│    View     │  ← Hiển thị (QListView, QTableView, QTreeView)
└─────────────┘
```

## 🎯 Cây Phân Cấp Các Model

```
QAbstractItemModel (Base class - trừu tượng)
│
├── QAbstractListModel (1 chiều - danh sách)
│   └── QStringListModel
│   └── QStandardItemModel (có thể dùng như list)
│
├── QAbstractTableModel (2 chiều - bảng)
│   └── QStandardItemModel (có thể dùng như table)
│
├── QAbstractProxyModel (Proxy - lọc/sắp xếp/chuyển đổi)
│   ├── QSortFilterProxyModel
│   ├── QIdentityProxyModel
│   └── QTransposeProxyModel
│
└── QSqlQueryModel (Database - chỉ đọc)
    └── QSqlTableModel (Database - đọc/ghi)
        └── QSqlRelationalTableModel (Database - có quan hệ)
```

## 📖 1. QAbstractItemModel - Base Class

**Vai trò**: Lớp cơ sở trừu tượng cho TẤT CẢ các model trong Qt.

**Các hàm quan trọng cần implement:**
```cpp
// Trả về số hàng
virtual int rowCount(const QModelIndex &parent = QModelIndex()) const = 0;

// Trả về số cột
virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;

// Trả về dữ liệu tại index
virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const = 0;

// Trả về header
virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

// Tạo index từ parent, row, column
virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

// Trả về parent của index
virtual QModelIndex parent(const QModelIndex &index) const;
```

**Khi nào dùng**: Khi cần tạo model tùy chỉnh hoàn toàn mới.

## 📋 2. QAbstractListModel - Model Danh Sách (1 chiều)

**Vai trò**: Hiển thị dữ liệu dạng danh sách (1 cột).

**Đặc điểm**:
- Chỉ có 1 cột
- Không có parent/child phức tạp
- Đơn giản nhất trong các model

**Các hàm cần implement:**
```cpp
int rowCount(const QModelIndex &parent = QModelIndex()) const override;
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
```

**Ví dụ sử dụng**: Danh sách tên, danh sách file, menu items...

## 📊 3. QAbstractTableModel - Model Bảng (2 chiều)

**Vai trò**: Hiển thị dữ liệu dạng bảng (nhiều hàng × nhiều cột).

**Đặc điểm**:
- Có nhiều hàng và nhiều cột
- Không có cấu trúc phân cấp (parent/child)
- Phẳng như bảng Excel

**Các hàm cần implement:**
```cpp
int rowCount(const QModelIndex &parent = QModelIndex()) const override;
int columnCount(const QModelIndex &parent = QModelIndex()) const override;
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
```

**Ví dụ sử dụng**: Bảng nhân viên, bảng sản phẩm, spreadsheet...

## 🌳 4. QAbstractItemModel cho Tree - Model Cây

**Vai trò**: Hiển thị dữ liệu dạng cây (có parent/child).

**Đặc điểm**:
- Có cấu trúc phân cấp
- Mỗi item có thể có parent và children
- Phức tạp nhất nhưng linh hoạt nhất

**Các hàm cần implement:**
```cpp
int rowCount(const QModelIndex &parent = QModelIndex()) const override;
int columnCount(const QModelIndex &parent = QModelIndex()) const override;
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
QModelIndex index(int row, int column, const QModelIndex &parent) const override;
QModelIndex parent(const QModelIndex &index) const override;
```

**Ví dụ sử dụng**: File system, menu đa cấp, cấu trúc tổ chức...

## 🔄 5. QAbstractProxyModel - Proxy Model

**Vai trò**: Đứng giữa Model gốc và View để:
- **Lọc** dữ liệu (filter)
- **Sắp xếp** dữ liệu (sort)
- **Chuyển đổi** cấu trúc dữ liệu (transform)

**Đặc điểm**:
- Không chứa dữ liệu thực sự
- Chỉ ánh xạ index từ model gốc
- Có thể xếp chồng nhiều proxy

**Các loại Proxy phổ biến:**

### QSortFilterProxyModel
- Sắp xếp và lọc dữ liệu
- Dùng phổ biến nhất

### QIdentityProxyModel
- Giữ nguyên cấu trúc, chỉ thêm logic xử lý

### QTransposeProxyModel
- Đảo hàng thành cột và ngược lại

**Luồng dữ liệu:**
```
Original Model → Proxy Model → View
```

## 🗄️ 6. QSQL Models - Database Models

### QSqlQueryModel
**Vai trò**: Đọc dữ liệu từ database (chỉ đọc).

**Đặc điểm**:
- Chỉ đọc, không thể chỉnh sửa
- Kết nối trực tiếp với SQL query
- Nhanh và đơn giản

**Sử dụng khi**: Hiển thị kết quả query, báo cáo...

### QSqlTableModel
**Vai trò**: Đọc và ghi dữ liệu vào 1 bảng database.

**Đặc điểm**:
- Có thể chỉnh sửa (CRUD)
- Tự động sync với database
- Làm việc với 1 bảng

**Sử dụng khi**: Form quản lý, CRUD operations...

### QSqlRelationalTableModel
**Vai trò**: Mở rộng QSqlTableModel với quan hệ giữa các bảng.

**Đặc điểm**:
- Hỗ trợ foreign key
- Hiển thị dữ liệu từ bảng liên quan
- Phức tạp hơn nhưng mạnh mẽ hơn

**Sử dụng khi**: Database có nhiều bảng liên quan, cần hiển thị dữ liệu từ bảng khác...

## 🔗 Mối Quan Hệ Giữa Các Model

### 1. Kế Thừa
```
QAbstractItemModel (base)
    ↓
QAbstractListModel, QAbstractTableModel, QAbstractProxyModel
    ↓
Các model cụ thể (QStringListModel, QSqlTableModel...)
```

### 2. Kết Hợp Sử Dụng

**Ví dụ phổ biến:**
```
QSqlTableModel → QSortFilterProxyModel → QTableView
     ↑                    ↑                    ↑
  Database          Lọc/Sắp xếp          Hiển thị
```

**Luồng dữ liệu:**
1. Database → QSqlTableModel (đọc dữ liệu)
2. QSqlTableModel → QSortFilterProxyModel (lọc/sắp xếp)
3. QSortFilterProxyModel → QTableView (hiển thị)

### 3. View Tương Ứng

| Model Type | View Tương Ứng |
|------------|----------------|
| QAbstractListModel | QListView |
| QAbstractTableModel | QTableView |
| QAbstractItemModel (Tree) | QTreeView |
| QAbstractProxyModel | Tất cả các View |
| QSqlQueryModel | QTableView, QListView |
| QSqlTableModel | QTableView |

## 🎓 Cách Đọc Code Nhanh

### Bước 1: Xác định loại Model
```cpp
// Tìm khai báo model
QAbstractListModel *model;      // → List model
QAbstractTableModel *model;     // → Table model
QSqlTableModel *model;          // → SQL model
QSortFilterProxyModel *proxy;  // → Proxy model
```

### Bước 2: Tìm các hàm quan trọng
- `rowCount()` - Số hàng
- `columnCount()` - Số cột
- `data()` - Dữ liệu hiển thị
- `setData()` - Cập nhật dữ liệu (nếu có)
- `index()` - Tạo index (cho tree)
- `parent()` - Parent index (cho tree)

### Bước 3: Tìm View được kết nối
```cpp
view->setModel(model);  // Kết nối model với view
```

### Bước 4: Tìm Proxy Model (nếu có)
```cpp
proxy->setSourceModel(sourceModel);  // Proxy kết nối với model gốc
view->setModel(proxy);               // View kết nối với proxy
```

## 📝 Checklist Đọc Code

- [ ] Xác định loại model (List/Table/Tree/Proxy/SQL)
- [ ] Tìm nơi khởi tạo model
- [ ] Tìm các hàm data(), rowCount(), columnCount()
- [ ] Tìm view nào sử dụng model này
- [ ] Kiểm tra có proxy model không
- [ ] Tìm nơi cập nhật dữ liệu (setData(), insertRows()...)
- [ ] Tìm signals/slots liên quan (dataChanged(), modelReset()...)

## 🚀 Tips Học Nhanh

1. **Bắt đầu từ đơn giản**: Học QAbstractListModel trước, sau đó Table, rồi Tree
2. **Xem ví dụ**: Chạy code examples để hiểu cách hoạt động
3. **Debug**: Đặt breakpoint tại data() để xem khi nào được gọi
4. **Đọc documentation**: Qt docs có ví dụ rất tốt
5. **Thực hành**: Tạo model đơn giản của riêng bạn

## 📚 Tài Liệu Tham Khảo

- Qt Model/View Programming: https://doc.qt.io/qt-6/model-view-programming.html
- QAbstractItemModel: https://doc.qt.io/qt-6/qabstractitemmodel.html
- QSqlTableModel: https://doc.qt.io/qt-6/qsqltablemodel.html
