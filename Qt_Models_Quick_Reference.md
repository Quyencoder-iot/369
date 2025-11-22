# Qt Models Quick Reference / Cheat Sheet

## 📋 Model Types Decision Tree

```
Cần lưu trữ gì?
├── Danh sách 1 chiều (list)
│   ├── Chỉ strings → QStringListModel
│   └── Custom data → QAbstractListModel
│
├── Bảng 2 chiều (table)
│   ├── Từ database → QSqlTableModel
│   ├── Linh hoạt → QStandardItemModel
│   └── Custom → QAbstractTableModel
│
├── Cây phân cấp (tree)
│   ├── File system → QFileSystemModel
│   ├── Linh hoạt → QStandardItemModel
│   └── Custom → QAbstractItemModel
│
└── Thêm sort/filter cho model có sẵn → QSortFilterProxyModel
```

---

## 🔑 Essential Methods Reference

### QAbstractListModel
```cpp
class MyListModel : public QAbstractListModel {
    // REQUIRED:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    
    // FOR EDITING:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    // FOR INSERT/REMOVE:
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
};
```

### QAbstractTableModel
```cpp
class MyTableModel : public QAbstractTableModel {
    // REQUIRED:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    
    // OPTIONAL BUT USEFUL:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    
    // FOR EDITING: (same as list model)
    bool setData(...);
    Qt::ItemFlags flags(...);
};
```

### QAbstractItemModel (Tree)
```cpp
class MyTreeModel : public QAbstractItemModel {
    // REQUIRED (all of them!):
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
};
```

### QSortFilterProxyModel
```cpp
class MyProxyModel : public QSortFilterProxyModel {
protected:
    // OVERRIDE TO CUSTOMIZE:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        // Return true to show row, false to hide
    }
    
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        // Return true if left < right for sorting
    }
};
```

---

## 🎭 Common Roles

| Role | Purpose | Return Type |
|------|---------|-------------|
| `Qt::DisplayRole` | Text hiển thị | QString, int, etc. |
| `Qt::EditRole` | Giá trị để edit | Any editable type |
| `Qt::DecorationRole` | Icon | QIcon, QPixmap, QColor |
| `Qt::ToolTipRole` | Tooltip | QString |
| `Qt::BackgroundRole` | Màu nền | QBrush, QColor |
| `Qt::ForegroundRole` | Màu chữ | QBrush, QColor |
| `Qt::FontRole` | Font chữ | QFont |
| `Qt::TextAlignmentRole` | Căn lề | Qt::Alignment |
| `Qt::CheckStateRole` | Checkbox | Qt::CheckState |
| `Qt::UserRole + N` | Custom roles | Any type |

**Code example:**
```cpp
QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid())
        return QVariant();
    
    switch (role) {
        case Qt::DisplayRole:
            return m_data[index.row()].text;
        
        case Qt::DecorationRole:
            return QIcon(":/icons/item.png");
        
        case Qt::BackgroundRole:
            return QColor(Qt::lightGray);
        
        case Qt::ToolTipRole:
            return "This is a tooltip";
        
        case Qt::UserRole + 1: // Custom role
            return m_data[index.row()].customData;
    }
    
    return QVariant();
}
```

---

## 📢 Important Signals

| Signal | Khi nào emit | Cách dùng |
|--------|--------------|-----------|
| `dataChanged()` | Dữ liệu thay đổi | `emit dataChanged(topLeft, bottomRight, {roles})` |
| `layoutAboutToBeChanged()` | TRƯỚC khi thay đổi structure | `emit layoutAboutToBeChanged()` |
| `layoutChanged()` | SAU khi thay đổi structure | `emit layoutChanged()` |
| `beginInsertRows()` | TRƯỚC khi insert | `beginInsertRows(parent, first, last)` |
| `endInsertRows()` | SAU khi insert | `endInsertRows()` |
| `beginRemoveRows()` | TRƯỚC khi remove | `beginRemoveRows(parent, first, last)` |
| `endRemoveRows()` | SAU khi remove | `endRemoveRows()` |

**Pattern for inserting:**
```cpp
void addItem(const Item &item) {
    int row = m_items.size();
    beginInsertRows(QModelIndex(), row, row);
    m_items.append(item);
    endInsertRows();
}
```

**Pattern for removing:**
```cpp
void removeItem(int row) {
    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}
```

**Pattern for changing data:**
```cpp
bool setData(const QModelIndex &index, const QVariant &value, int role) override {
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    
    m_items[index.row()] = value.toString();
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}
```

---

## 🔍 QModelIndex Cheat Sheet

```cpp
QModelIndex index;

// Check validity
if (!index.isValid())
    return;

// Get position
int row = index.row();          // 0-based row number
int col = index.column();       // 0-based column number

// Get parent (for tree models)
QModelIndex parent = index.parent();

// Access data
QVariant data = model->data(index, Qt::DisplayRole);

// Get internal pointer (tree models only)
void *ptr = index.internalPointer();
TreeItem *item = static_cast<TreeItem*>(ptr);

// Create index (in model implementation)
QModelIndex newIndex = createIndex(row, col, internalPointer);

// Invalid index (represents root in tree)
QModelIndex root = QModelIndex();
```

---

## 🔄 Proxy Model Usage

```cpp
// Setup
MySourceModel *source = new MySourceModel;
QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
proxy->setSourceModel(source);

// Configure proxy
proxy->setFilterKeyColumn(1);                    // Filter on column 1
proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
proxy->setFilterRegExp(QRegExp("pattern"));
proxy->setSortRole(Qt::UserRole + 1);            // Sort by custom role

// Connect to view
view->setModel(proxy);  // NOT source!
view->setSortingEnabled(true);

// Index mapping
QModelIndex proxyIndex = proxy->index(0, 0);
QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
QModelIndex backToProxy = proxy->mapFromSource(sourceIndex);

// Trigger re-filter/re-sort
proxy->invalidateFilter();
proxy->sort(0, Qt::AscendingOrder);
```

---

## 💾 SQL Models Quick Reference

### QSqlQueryModel (Read-only)
```cpp
QSqlQueryModel *model = new QSqlQueryModel;
model->setQuery("SELECT name, age FROM users WHERE age > 18");
model->setHeaderData(0, Qt::Horizontal, "Name");

view->setModel(model);
```

### QSqlTableModel (Editable)
```cpp
QSqlTableModel *model = new QSqlTableModel;
model->setTable("employees");
model->setEditStrategy(QSqlTableModel::OnManualSubmit);
model->select();

// Filter
model->setFilter("age > 30");

// Sort
model->setSort(1, Qt::AscendingOrder);

// Edit
model->setData(model->index(0, 1), "New Value");
model->submitAll();  // Save to DB

// Revert changes
model->revertAll();

// Insert
int row = model->rowCount();
model->insertRow(row);
model->setData(model->index(row, 1), "Name");
model->submitAll();

// Remove
model->removeRow(0);
model->submitAll();
```

### QSqlRelationalTableModel (Foreign Keys)
```cpp
QSqlRelationalTableModel *model = new QSqlRelationalTableModel;
model->setTable("orders");
model->setRelation(2, QSqlRelation("customers", "id", "name"));
model->select();

// Column 2 will show customer name instead of customer_id
```

---

## 🎯 Common Patterns

### Pattern 1: Editable Flags
```cpp
Qt::ItemFlags flags(const QModelIndex &index) const override {
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    
    // Make editable
    flags |= Qt::ItemIsEditable;
    
    // Add checkbox
    if (index.column() == 0)
        flags |= Qt::ItemIsUserCheckable;
    
    // Make draggable
    flags |= Qt::ItemIsDragEnabled;
    
    // Make droppable
    flags |= Qt::ItemIsDropEnabled;
    
    return flags;
}
```

### Pattern 2: Checkbox Column
```cpp
QVariant data(const QModelIndex &index, int role) const override {
    if (index.column() == 0 && role == Qt::CheckStateRole) {
        return m_items[index.row()].checked ? Qt::Checked : Qt::Unchecked;
    }
    // ... other roles
}

bool setData(const QModelIndex &index, const QVariant &value, int role) override {
    if (index.column() == 0 && role == Qt::CheckStateRole) {
        m_items[index.row()].checked = (value.toInt() == Qt::Checked);
        emit dataChanged(index, index, {Qt::CheckStateRole});
        return true;
    }
    // ... other roles
}

Qt::ItemFlags flags(const QModelIndex &index) const override {
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (index.column() == 0)
        flags |= Qt::ItemIsUserCheckable;
    return flags;
}
```

### Pattern 3: Custom Sort in Proxy
```cpp
class NumericSortProxy : public QSortFilterProxyModel {
protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);
        
        // Numeric sort
        if (leftData.type() == QVariant::Int)
            return leftData.toInt() < rightData.toInt();
        
        // Date sort
        if (leftData.canConvert<QDate>())
            return leftData.toDate() < rightData.toDate();
        
        // String sort (case-insensitive)
        return leftData.toString().toLower() < rightData.toString().toLower();
    }
};
```

### Pattern 4: Async/Lazy Loading
```cpp
class LazyModel : public QAbstractListModel {
    int m_loadedCount = 0;
    int m_totalCount = 10000;
    
public:
    bool canFetchMore(const QModelIndex &parent) const override {
        return m_loadedCount < m_totalCount;
    }
    
    void fetchMore(const QModelIndex &parent) override {
        int remainder = m_totalCount - m_loadedCount;
        int itemsToFetch = qMin(100, remainder);
        
        beginInsertRows(QModelIndex(), m_loadedCount, m_loadedCount + itemsToFetch - 1);
        
        // Load data from DB/network/etc
        for (int i = 0; i < itemsToFetch; ++i) {
            m_items.append(loadItemFromSource(m_loadedCount + i));
        }
        m_loadedCount += itemsToFetch;
        
        endInsertRows();
    }
};
```

---

## 🐛 Debugging Checklist

- [ ] **Model crashes?**
  - Check `index.isValid()` before using
  - Check array bounds: `index.row() < m_items.size()`
  - Never return invalid QVariant() for DisplayRole if data exists

- [ ] **View không update?**
  - Emit `dataChanged()` after modifying data
  - Use `beginInsertRows()`/`endInsertRows()` when inserting
  - Use `beginRemoveRows()`/`endRemoveRows()` when removing

- [ ] **Proxy không work?**
  - Set source model: `proxy->setSourceModel(source)`
  - Use proxy for view: `view->setModel(proxy)` NOT source
  - Call `invalidateFilter()` after changing filter criteria
  - Map indices correctly: `mapToSource()` / `mapFromSource()`

- [ ] **Performance chậm?**
  - Don't do expensive computation in `data()`
  - Cache computed values
  - Implement `canFetchMore()`/`fetchMore()` for large datasets
  - Use `dataChanged()` with specific roles, not all roles

- [ ] **Tree model lỗi?**
  - `parent()` must return invalid index for root children
  - `internalPointer()` must point to valid object
  - Every node must know its parent
  - Check `hasIndex()` before creating index

---

## 📚 Learning Path

```
Week 1: QAbstractListModel
├── Tạo model đơn giản (QStringList)
├── Thêm edit support (setData, flags)
├── Thêm insert/remove
└── Thử custom roles

Week 2: QAbstractTableModel
├── Model với custom struct
├── Header data
├── Multiple columns
└── Conditional formatting (colors)

Week 3: Proxy Models
├── QSortFilterProxyModel basic
├── Custom filterAcceptsRow()
├── Custom lessThan()
└── Index mapping

Week 4: SQL Models
├── QSqlQueryModel
├── QSqlTableModel với edit
├── Filtering và sorting
└── QSqlRelationalTableModel

Week 5: Tree Models
├── Hiểu tree structure
├── Implement TreeItem class
├── index() và parent()
└── Complex hierarchies
```

---

## 🔗 Quick Links

**Qt Documentation:**
- Model/View Programming: https://doc.qt.io/qt-5/model-view-programming.html
- QAbstractItemModel: https://doc.qt.io/qt-5/qabstractitemmodel.html
- Model/View Tutorial: https://doc.qt.io/qt-5/modelview.html

**Common Gotchas:**
- Always check `index.isValid()` first
- Never emit signals inside `rowCount()`, `columnCount()`, or `data()`
- Tree models: root children have invalid parent index
- Proxy: always use proxy indices in view, map when needed
- SQL models: call `select()` after setting table/query

---

## 💡 Quick Tips

```cpp
// 1. Safe data access
QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() >= m_items.size())
        return QVariant();
    // ... safe to access m_items[index.row()]
}

// 2. Multiple role support
QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid())
        return QVariant();
    
    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return m_items[index.row()]; // Same data for both
        // ... other roles
    }
    return QVariant();
}

// 3. Efficient dataChanged
// GOOD: Specify roles
emit dataChanged(index, index, {Qt::DisplayRole});

// BAD: All roles (slower)
emit dataChanged(index, index);

// 4. Batch updates
emit layoutAboutToBeChanged();
// ... make many changes
emit layoutChanged();

// 5. Test with QAbstractItemModelTester
#include <QAbstractItemModelTester>
auto *tester = new QAbstractItemModelTester(model, this);
// Will auto-check model consistency
```

---

**Print this page and keep it next to your keyboard! 📌**
