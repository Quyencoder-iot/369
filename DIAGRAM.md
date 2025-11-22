# Sơ Đồ Mối Quan Hệ Qt Models

## 1. Cây Phân Cấp Kế Thừa

```
                    QAbstractItemModel
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
QAbstractListModel  QAbstractTableModel  QAbstractProxyModel
        │                  │                  │
        │                  │                  ├── QSortFilterProxyModel
        │                  │                  ├── QIdentityProxyModel
        │                  │                  └── QTransposeProxyModel
        │                  │
        └──────────────────┼──────────────────┐
                           │                  │
                    QSqlQueryModel      QStandardItemModel
                           │                  │
                           ├── QSqlTableModel │
                           │                  └── (có thể dùng như List/Table/Tree)
                           │
                           └── QSqlRelationalTableModel
```

## 2. Luồng Dữ Liệu Cơ Bản

### Pattern Đơn Giản:
```
    Data Source
        │
        ↓
    Model
        │
        ↓
    View
```

### Pattern Với Proxy:
```
    Data Source (Database/Memory)
        │
        ↓
    Source Model (QSqlTableModel / Custom Model)
        │
        ↓
    Proxy Model (QSortFilterProxyModel)
        │
        ↓
    View (QTableView / QListView / QTreeView)
```

## 3. Ví Dụ Cụ Thể

### Example 1: SQL Table với Filter
```
SQLite Database
    │
    ↓ (SELECT * FROM employees)
QSqlTableModel
    │
    ↓ (setSourceModel)
QSortFilterProxyModel (filter by name)
    │
    ↓ (setModel)
QTableView
```

### Example 2: Custom Table Model
```
QList<Employee> (in-memory data)
    │
    ↓ (stored in m_employees)
CustomTableModel (kế thừa QAbstractTableModel)
    │
    ↓ (setModel)
QTableView
```

### Example 3: Tree Model
```
Tree Structure (TreeNode*)
    │
    ↓ (stored in rootNode)
CustomTreeModel (kế thừa QAbstractItemModel)
    │
    ↓ (setModel)
QTreeView
```

## 4. Signals và Slots

```
Model emits signals:
    dataChanged()      → View tự động cập nhật
    rowsInserted()    → View tự động thêm hàng
    rowsRemoved()     → View tự động xóa hàng
    modelReset()      → View reload toàn bộ

View emits signals:
    clicked()         → User click vào item
    doubleClicked()  → User double click
    selectionChanged() → Selection thay đổi
```

## 5. Roles trong Qt Model

```
Qt::DisplayRole        → Text hiển thị
Qt::EditRole          → Text để edit
Qt::ToolTipRole       → Tooltip
Qt::StatusTipRole     → Status bar text
Qt::DecorationRole    → Icon/image
Qt::BackgroundRole    → Màu nền
Qt::ForegroundRole    → Màu chữ
Qt::TextAlignmentRole → Căn lề
Qt::FontRole          → Font
Qt::SizeHintRole      → Kích thước
```

## 6. Index và Parent (Tree Model)

```
Root (invalid index)
├── Node A (index(0, 0, root))
│   ├── Node A1 (index(0, 0, A))
│   └── Node A2 (index(1, 0, A))
└── Node B (index(1, 0, root))
    └── Node B1 (index(0, 0, B))
```

**Quan trọng:**
- `index(row, column, parent)` - Tạo index cho child
- `parent(index)` - Lấy parent của index
- Invalid index = root

## 7. Proxy Model Chain

```
Source Model
    │
    ↓ setSourceModel()
Proxy 1 (Filter)
    │
    ↓ setSourceModel()
Proxy 2 (Sort)
    │
    ↓ setSourceModel()
Proxy 3 (Transform)
    │
    ↓ setModel()
View
```

**Lưu ý:** View chỉ kết nối với Proxy cuối cùng!

## 8. SQL Models Comparison

```
QSqlQueryModel
    │
    ├── Chỉ đọc
    ├── Từ query bất kỳ
    └── Dùng cho: Reports, Statistics

QSqlTableModel
    │
    ├── Đọc và ghi
    ├── 1 bảng
    └── Dùng cho: CRUD operations

QSqlRelationalTableModel
    │
    ├── Đọc và ghi
    ├── Nhiều bảng (foreign key)
    └── Dùng cho: Complex database
```

## 9. View Types

```
QAbstractItemView (base)
    │
    ├── QListView      → Cho List Model
    ├── QTableView     → Cho Table Model
    ├── QTreeView      → Cho Tree Model
    └── QColumnView    → Cho Column Model
```

**Matching:**
- QListView ↔ QAbstractListModel
- QTableView ↔ QAbstractTableModel
- QTreeView ↔ QAbstractItemModel (với parent/child)

## 10. Memory và Performance

```
Model chỉ chứa metadata, không chứa View items
    │
    ├── View tự động request data khi cần
    ├── Virtual scrolling (chỉ render visible items)
    └── Caching trong View, không phải Model
```

**Tip:** Model nên lightweight, View sẽ cache và optimize rendering.
