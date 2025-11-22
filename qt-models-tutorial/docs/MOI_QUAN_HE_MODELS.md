# Mối Quan Hệ Giữa Các Qt Models

## 🌐 Sơ Đồ Tổng Quan

```
┌─────────────────────────────────────────────────────────────────┐
│                        QAbstractItemModel                        │
│              (Base class cho tất cả models)                      │
└─────────────────────┬───────────────────────────────────────────┘
                      │
        ┌─────────────┼─────────────┬─────────────────┐
        │             │             │                 │
        ▼             ▼             ▼                 ▼
┌───────────────┐ ┌───────────────┐ ┌───────────────┐ ┌──────────────┐
│QAbstractList  │ │QAbstractTable │ │QStandardItem  │ │ Custom Tree  │
│    Model      │ │    Model      │ │    Model      │ │   Models     │
└───────────────┘ └───────┬───────┘ └───────────────┘ └──────────────┘
                          │
                          ▼
                ┌─────────────────┐
                │ QSqlTableModel  │
                └────────┬────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │QSqlRelationalTable   │
              │      Model           │
              └──────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                    QAbstractProxyModel                           │
│            (Proxy/Filter cho existing models)                    │
└─────────────────────┬───────────────────────────────────────────┘
                      │
        ┌─────────────┼─────────────┐
        │                           │
        ▼                           ▼
┌───────────────────┐     ┌──────────────────┐
│QSortFilterProxy   │     │QIdentityProxy    │
│     Model         │     │     Model        │
└───────────────────┘     └──────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                       QSqlQueryModel                             │
│              (Read-only model cho SQL queries)                   │
└─────────────────────────────────────────────────────────────────┘
```

## 📋 Chi Tiết Mối Quan Hệ

### 1. **Hierarchy (Kế thừa)**

```cpp
QObject
  └─> QAbstractItemModel
        ├─> QAbstractListModel
        │     └─> (Custom list models của bạn)
        │
        ├─> QAbstractTableModel
        │     └─> QSqlTableModel
        │           └─> QSqlRelationalTableModel
        │
        ├─> QAbstractProxyModel
        │     ├─> QSortFilterProxyModel
        │     └─> QIdentityProxyModel
        │
        ├─> QStandardItemModel
        │
        └─> QSqlQueryModel
```

### 2. **Use Cases và Relationships**

#### **Data Models** (Chứa dữ liệu thực)
- `QAbstractListModel`: Dữ liệu 1 chiều
- `QAbstractTableModel`: Dữ liệu 2 chiều
- `QStandardItemModel`: Multi-purpose, có thể dùng cho list/table/tree
- `QSqlTableModel`: Kết nối trực tiếp với database table
- `QSqlRelationalTableModel`: Database với foreign keys
- `QSqlQueryModel`: Kết quả từ SQL queries

#### **Proxy Models** (Không chứa dữ liệu)
- `QSortFilterProxyModel`: Lọc và sắp xếp
- `QIdentityProxyModel`: Transform hiển thị

### 3. **Cách Kết Hợp Models**

```cpp
// Pattern 1: Model + Proxy
QStandardItemModel *sourceModel = new QStandardItemModel;
QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;
proxyModel->setSourceModel(sourceModel);
view->setModel(proxyModel);

// Pattern 2: SQL + Proxy
QSqlTableModel *sqlModel = new QSqlTableModel;
QSortFilterProxyModel *filterModel = new QSortFilterProxyModel;
filterModel->setSourceModel(sqlModel);
view->setModel(filterModel);

// Pattern 3: Multiple Views
QStandardItemModel *model = new QStandardItemModel;
QListView *listView = new QListView;
QTableView *tableView = new QTableView;
listView->setModel(model);   // Same model
tableView->setModel(model);   // Different views
```

## 🔄 Data Flow

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Data       │ --> │   Model     │ --> │   View      │
│  Source     │     │             │     │             │
└─────────────┘     └─────────────┘     └─────────────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Proxy Model  │ (Optional)
                    └─────────────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Another View │
                    └─────────────┘
```

## 💡 Khi Nào Dùng Model Nào?

### **QAbstractListModel**
✅ Khi cần:
- List đơn giản (1 cột)
- Performance cao với nhiều items
- Custom data structure

❌ Không dùng khi:
- Cần nhiều cột → Dùng TableModel
- Cần hierarchy → Dùng TreeModel

### **QAbstractTableModel**
✅ Khi cần:
- Data dạng bảng (rows × columns)
- Headers cho cột/hàng
- Cell-based editing

❌ Không dùng khi:
- Chỉ 1 cột → Dùng ListModel
- Cần parent-child → Dùng TreeModel

### **QStandardItemModel**
✅ Khi cần:
- Prototype nhanh
- Không muốn subclass
- Mixed structure (list/table/tree)

❌ Không dùng khi:
- Performance critical
- Complex custom data

### **QSqlTableModel**
✅ Khi cần:
- Direct database table access
- CRUD operations
- Simple queries

❌ Không dùng khi:
- Complex joins → Dùng QSqlQueryModel
- Need relations → Dùng QSqlRelationalTableModel

### **QSortFilterProxyModel**
✅ Khi cần:
- Filter/search functionality
- Sort mà không ảnh hưởng source
- Multiple filtered views

❌ Không dùng khi:
- Chỉ cần hiển thị → Dùng model trực tiếp

## 🔧 Best Practices

1. **Chọn đúng base class**
   - Đừng kế thừa QAbstractItemModel nếu List/Table đủ dùng

2. **Proxy cho filtering**
   - Luôn dùng proxy model cho filter/sort
   - Giữ source model unchanged

3. **Memory management**
   - Models thường own data
   - Views không own models
   - Set parent cho proper cleanup

4. **Performance**
   - Batch operations với begin/end calls
   - Use dataChanged() efficiently
   - Avoid reset model unnecessarily

5. **SQL Models**
   - Check errors after operations
   - Use transactions cho bulk updates
   - Index database columns được query

## 📊 Tổng Kết Quick Reference

| Model Type | Data Dims | Editable | Use Case |
|------------|-----------|----------|----------|
| QAbstractListModel | 1D | Yes | Custom lists |
| QAbstractTableModel | 2D | Yes | Custom tables |
| QStandardItemModel | Any | Yes | Quick prototypes |
| QSqlTableModel | 2D | Yes | Database tables |
| QSqlRelationalTableModel | 2D | Yes | Tables with FK |
| QSqlQueryModel | 2D | No | Complex queries |
| QSortFilterProxyModel | Same as source | Depends | Filter/Sort |

## 🎯 Decision Tree

```
Cần hiển thị data?
│
├─> Data từ đâu?
│   ├─> Memory/File
│   │   ├─> 1 cột? → QAbstractListModel
│   │   ├─> Nhiều cột? → QAbstractTableModel
│   │   ├─> Tree? → QAbstractItemModel
│   │   └─> Quick? → QStandardItemModel
│   │
│   └─> Database
│       ├─> Single table? → QSqlTableModel
│       ├─> With relations? → QSqlRelationalTableModel
│       └─> Complex query? → QSqlQueryModel
│
└─> Cần filter/sort?
    └─> Yes → Wrap với QSortFilterProxyModel
```