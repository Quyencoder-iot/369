# Mối Quan Hệ Giữa Các Qt Models

## 1. Sơ Đồ Hierarchy (Kế Thừa)

```
                              QAbstractItemModel
                                     |
                   +-----------------+-----------------+
                   |                                   |
         QAbstractListModel                   QAbstractProxyModel
                   |                                   |
                   |                                   |
      +------------+------------+                      |
      |            |            |                      |
QStringListModel  QSqlQueryModel               QSortFilterProxyModel
                   |                                   |
                   |                            QIdentityProxyModel
           QSqlTableModel
                   |
         QSqlRelationalTableModel
                   
                   
         QStandardItemModel (implements QAbstractItemModel directly)
```

## 2. Các Nhóm Model Theo Chức Năng

### 2.1 Data Source Models (Models Gốc)
```
┌─────────────────────────────────────┐
│        DATA SOURCE MODELS           │
├─────────────────────────────────────┤
│                                     │
│  • QStringListModel                 │
│    └─> List of strings              │
│                                     │
│  • QStandardItemModel               │
│    └─> Generic table/tree/list      │
│                                     │
│  • QSqlTableModel                   │
│    └─> Database table               │
│                                     │
│  • QSqlQueryModel                   │
│    └─> SQL query results            │
│                                     │
│  • QSqlRelationalTableModel         │
│    └─> Table with foreign keys      │
│                                     │
└─────────────────────────────────────┘
```

### 2.2 Proxy Models (Models Trung Gian)
```
┌─────────────────────────────────────┐
│          PROXY MODELS               │
├─────────────────────────────────────┤
│                                     │
│  • QSortFilterProxyModel            │
│    ├─> Filter data                  │
│    ├─> Sort data                    │
│    └─> Search functionality         │
│                                     │
│  • QIdentityProxyModel              │
│    └─> Base for custom proxies      │
│                                     │
└─────────────────────────────────────┘
```

## 3. Model-View-Delegate Relationships

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   MODEL     │<--->│    VIEW     │<--->│  DELEGATE   │
└─────────────┘     └─────────────┘     └─────────────┘
      ↑                    ↑                    ↑
      |                    |                    |
  Data Storage        Data Display         Data Editing
  
Views có thể sử dụng:
- QListView      → QAbstractListModel
- QTableView     → QAbstractTableModel  
- QTreeView      → QAbstractItemModel (full hierarchy)
```

## 4. Proxy Model Chain Pattern

```
Source Model → Proxy Model 1 → Proxy Model 2 → View
     ↓              ↓               ↓           ↓
  Raw Data    First Filter    Second Filter  Display

Ví dụ thực tế:
QSqlTableModel → FilterProxy → SortProxy → QTableView
```

## 5. Quan Hệ Giữa SQL Models

```
                    QSqlDatabase
                         |
        +----------------+----------------+
        |                |                |
  QSqlQueryModel  QSqlTableModel  Custom Model
   (Read-only)    (Read/Write)         |
        |                |              |
        |                |         QSqlQuery
        |                |
        |    QSqlRelationalTableModel
        |      (Foreign Key Support)
        |                |
        +----------------+
                |
            QTableView
```

## 6. Decision Tree - Chọn Model Phù Hợp

```
Bạn cần hiển thị data gì?
│
├─> Simple String List?
│   └─> QStringListModel
│
├─> Structured Data (Table/Tree)?
│   ├─> Need to edit?
│   │   ├─> Yes → QStandardItemModel
│   │   └─> No  → Custom read-only model
│   │
│   └─> From Database?
│       ├─> Single table → QSqlTableModel
│       ├─> With relations → QSqlRelationalTableModel
│       └─> Complex query → QSqlQueryModel
│
└─> Need Filtering/Sorting?
    └─> Add QSortFilterProxyModel on top
```

## 7. Model Capabilities Comparison

| Feature | QStringListModel | QStandardItemModel | QSqlTableModel | QSortFilterProxyModel |
|---------|-----------------|-------------------|----------------|---------------------|
| Data Type | Strings only | Any QVariant | Database rows | Depends on source |
| Structure | List | List/Table/Tree | Table | Same as source |
| Editable | Yes | Yes | Yes | Depends on source |
| Custom Roles | Limited | Yes | Yes | Pass-through |
| Performance | Excellent | Good | Good | Excellent |
| Memory Usage | Low | High | Medium | Low (no data copy) |

## 8. Common Usage Patterns

### Pattern 1: Simple List
```cpp
QStringListModel → QListView
```

### Pattern 2: Editable Table
```cpp
QStandardItemModel → QTableView
```

### Pattern 3: Database with Search
```cpp
QSqlTableModel → QSortFilterProxyModel → QTableView
```

### Pattern 4: Tree with Multiple Views
```cpp
                  ┌→ QTreeView (full tree)
QStandardItemModel┤
                  └→ QSortFilterProxyModel → QListView (filtered)
```

### Pattern 5: Master-Detail View
```cpp
QSqlRelationalTableModel → QTableView (master)
         ↓
   Selection Model
         ↓
QSqlTableModel → QTableView (detail)
```

## 9. Key Concepts để Hiểu

1. **QModelIndex**: "Con trỏ" đến vị trí trong model
2. **Roles**: Qt::DisplayRole, Qt::EditRole, Qt::UserRole...
3. **Signals**: dataChanged, rowsInserted, modelReset...
4. **Selection Model**: Quản lý việc chọn items
5. **Persistent Index**: Index không thay đổi khi model update

## 10. Performance Tips

1. **Sử dụng Proxy Models** thay vì copy data
2. **Lazy Loading** cho data lớn
3. **beginInsertRows/endInsertRows** cho batch updates
4. **setData() với role cụ thể** thay vì update toàn bộ
5. **Virtual models** cho data cực lớn