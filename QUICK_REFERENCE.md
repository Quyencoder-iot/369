# 📋 Qt Models - Quick Reference Card

## 🎯 Chọn Model Phù Hợp

```
CÂU HỎI                          → MODEL
─────────────────────────────────────────────────
Danh sách đơn giản?              → QStringListModel
Danh sách custom?                → QAbstractListModel
Bảng 2D?                         → QAbstractTableModel
Cây phân cấp?                    → QAbstractItemModel (Tree)
Từ database, read-only?          → QSqlQueryModel
Từ database, editable, 1 table?  → QSqlTableModel
Database với foreign keys?       → QSqlRelationalTableModel
Cần filter/sort existing model?  → QSortFilterProxyModel
```

---

## 🔑 Phương Thức Bắt Buộc Phải Implement

### QAbstractListModel
```cpp
int rowCount(const QModelIndex &parent) const;
QVariant data(const QModelIndex &index, int role) const;
```

### QAbstractTableModel
```cpp
int rowCount(const QModelIndex &parent) const;
int columnCount(const QModelIndex &parent) const;
QVariant data(const QModelIndex &index, int role) const;
```

### QAbstractItemModel (Tree)
```cpp
int rowCount(const QModelIndex &parent) const;
int columnCount(const QModelIndex &parent) const;
QVariant data(const QModelIndex &index, int role) const;
QModelIndex index(int row, int col, const QModelIndex &parent) const;
QModelIndex parent(const QModelIndex &child) const;
```

### Editable Models (Thêm vào)
```cpp
bool setData(const QModelIndex &index, const QVariant &value, int role);
Qt::ItemFlags flags(const QModelIndex &index) const;
```

---

## 🎨 Data Roles - Quan Trọng!

| Role | Mục đích | Ví dụ return |
|------|----------|--------------|
| `Qt::DisplayRole` | Text hiển thị | `return "Hello"` |
| `Qt::EditRole` | Text khi edit | `return "Hello"` |
| `Qt::DecorationRole` | Icon | `return QIcon(...)` |
| `Qt::ToolTipRole` | Tooltip | `return "Tooltip text"` |
| `Qt::BackgroundRole` | Màu nền | `return QColor(Qt::yellow)` |
| `Qt::ForegroundRole` | Màu chữ | `return QColor(Qt::red)` |
| `Qt::FontRole` | Font | `return QFont(...)` |
| `Qt::TextAlignmentRole` | Căn chỉnh | `return Qt::AlignCenter` |
| `Qt::CheckStateRole` | Checkbox | `return Qt::Checked` |
| `Qt::UserRole + N` | Custom data | `return customValue` |

---

## 🔄 Thêm/Xóa Dữ Liệu - Template

### Thêm Row
```cpp
void MyModel::addItem(const Item &item) {
    int row = m_items.count();
    
    // PHẢI gọi TRƯỚC khi thêm
    beginInsertRows(QModelIndex(), row, row);
    
    m_items.append(item);
    
    // PHẢI gọi SAU khi thêm
    endInsertRows();
}
```

### Xóa Row
```cpp
bool MyModel::removeItem(int row) {
    if (row < 0 || row >= m_items.count())
        return false;
    
    // PHẢI gọi TRƯỚC khi xóa
    beginRemoveRows(QModelIndex(), row, row);
    
    m_items.remove(row);
    
    // PHẢI gọi SAU khi xóa
    endRemoveRows();
    
    return true;
}
```

### Update Data
```cpp
bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;
    
    if (role == Qt::EditRole) {
        m_items[index.row()].text = value.toString();
        
        // PHẢI emit signal
        emit dataChanged(index, index, {role});
        return true;
    }
    
    return false;
}
```

---

## 🗄️ QSql Models Cheat Sheet

### 1. Setup Database
```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("mydb.db");
if (!db.open()) {
    qDebug() << db.lastError().text();
}
```

### 2. QSqlQueryModel (Read-Only)
```cpp
QSqlQueryModel *model = new QSqlQueryModel;
model->setQuery("SELECT * FROM users WHERE age > 18");
view->setModel(model);
```

### 3. QSqlTableModel (Editable)
```cpp
QSqlTableModel *model = new QSqlTableModel;
model->setTable("users");
model->setEditStrategy(QSqlTableModel::OnManualSubmit);
model->setFilter("age > 18");
model->setSort(1, Qt::AscendingOrder);
model->select();

// Thêm row
int row = model->rowCount();
model->insertRow(row);
model->setData(model->index(row, 1), "John");
model->submitAll(); // Lưu vào DB

// Xóa row
model->removeRow(currentRow);
model->submitAll();

// Hoàn tác
model->revertAll();
```

### 4. QSqlRelationalTableModel (Foreign Keys)
```cpp
QSqlRelationalTableModel *model = new QSqlRelationalTableModel;
model->setTable("employees");

// Column 3 (department_id) → hiện tên từ departments.name
model->setRelation(3, QSqlRelation("departments", "id", "name"));

model->select();

// PHẢI dùng delegate này!
view->setItemDelegate(new QSqlRelationalDelegate(view));
```

---

## 🔍 Proxy Model Template

```cpp
class MyFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
    
    QString m_filter;
    
public:
    void setFilter(const QString &filter) {
        m_filter = filter;
        invalidateFilter(); // Refresh
    }
    
protected:
    bool filterAcceptsRow(int sourceRow, 
                         const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QString text = sourceModel()->data(index).toString();
        
        // Return true để HIỆN row, false để ẨN
        return text.contains(m_filter, Qt::CaseInsensitive);
    }
    
    bool lessThan(const QModelIndex &left, 
                 const QModelIndex &right) const override {
        // Custom sort logic
        return sourceModel()->data(left) < sourceModel()->data(right);
    }
};

// Usage
MyFilterProxy *proxy = new MyFilterProxy;
proxy->setSourceModel(sourceModel);
view->setModel(proxy); // View dùng PROXY!

// Mapping
QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
QModelIndex proxyIndex = proxy->mapFromSource(sourceIndex);
```

---

## 🌲 Tree Model Template

```cpp
struct Node {
    QString data;
    Node *parent;
    QVector<Node*> children;
    
    int row() const {
        if (parent)
            return parent->children.indexOf(const_cast<Node*>(this));
        return 0;
    }
};

class TreeModel : public QAbstractItemModel {
    Node *rootNode;
    
public:
    // Tạo index cho node
    QModelIndex index(int row, int column, 
                     const QModelIndex &parent) const override {
        if (!hasIndex(row, column, parent))
            return QModelIndex();
        
        Node *parentNode = parent.isValid() 
            ? static_cast<Node*>(parent.internalPointer())
            : rootNode;
        
        Node *childNode = parentNode->children.at(row);
        return createIndex(row, column, childNode);
    }
    
    // Tìm parent của node
    QModelIndex parent(const QModelIndex &child) const override {
        if (!child.isValid())
            return QModelIndex();
        
        Node *childNode = static_cast<Node*>(child.internalPointer());
        Node *parentNode = childNode->parent;
        
        if (parentNode == rootNode)
            return QModelIndex(); // Root
        
        return createIndex(parentNode->row(), 0, parentNode);
    }
    
    int rowCount(const QModelIndex &parent) const override {
        Node *parentNode = parent.isValid()
            ? static_cast<Node*>(parent.internalPointer())
            : rootNode;
        return parentNode->children.count();
    }
};
```

---

## 🐛 Debug Helpers

### Print Model Structure
```cpp
void debugModel(QAbstractItemModel *model, 
                QModelIndex parent = QModelIndex(), 
                int indent = 0) {
    for (int row = 0; row < model->rowCount(parent); ++row) {
        QModelIndex index = model->index(row, 0, parent);
        qDebug() << QString(indent * 2, ' ') 
                 << model->data(index).toString();
        
        if (model->hasChildren(index))
            debugModel(model, index, indent + 1);
    }
}
```

### Check Model Validity
```cpp
// Check if index is valid
if (!index.isValid()) {
    qDebug() << "Invalid index!";
}

// Check bounds
if (index.row() >= model->rowCount() || index.column() >= model->columnCount()) {
    qDebug() << "Out of bounds!";
}

// Print index info
qDebug() << "Row:" << index.row() 
         << "Col:" << index.column()
         << "Data:" << index.data().toString();
```

---

## ⚠️ Common Mistakes

### ❌ WRONG
```cpp
// Quên beginInsertRows/endInsertRows
m_items.append(item); // View không update!

// Quên emit dataChanged
m_items[row].text = newText; // View không update!

// View dùng source model thay vì proxy
view->setModel(sourceModel); // Filter không hoạt động!

// Quên map index khi dùng proxy
model->removeRow(view->currentIndex().row()); // SAI! Phải map index
```

### ✅ CORRECT
```cpp
// Đúng cách insert
beginInsertRows(QModelIndex(), row, row);
m_items.append(item);
endInsertRows();

// Đúng cách update
m_items[row].text = newText;
emit dataChanged(index, index, {Qt::DisplayRole});

// Đúng cách với proxy
view->setModel(proxyModel);
QModelIndex sourceIndex = proxyModel->mapToSource(view->currentIndex());
sourceModel->removeRow(sourceIndex.row());
```

---

## 🎯 Luồng Hoạt Động

### List/Table Model
```
User clicks view
    ↓
View calls model->data(index, role)
    ↓
Model returns QVariant based on role
    ↓
View displays data
```

### Tree Model
```
User expands node
    ↓
View calls model->rowCount(parentIndex)
    ↓
For each row: view calls model->index(row, 0, parentIndex)
    ↓
Model calls createIndex(row, 0, nodePointer)
    ↓
View calls model->data(index, role)
    ↓
View displays children
```

### Proxy Model
```
View requests data with proxyIndex
    ↓
Proxy maps to sourceIndex = mapToSource(proxyIndex)
    ↓
Proxy calls sourceModel->data(sourceIndex, role)
    ↓
Proxy returns data to view
```

### SQL Model
```
model->select()
    ↓
Execute SQL query
    ↓
Cache results
    ↓
View displays data
    ↓
User edits cell
    ↓
model->setData() updates cache
    ↓
model->submitAll() writes to database
```

---

## 📚 Đọc Code Nhanh - Checklist

Khi đọc code Qt Model, tìm theo thứ tự:

1. **Class kế thừa gì?**
   - `QAbstractListModel` → List
   - `QAbstractTableModel` → Table
   - `QAbstractItemModel` → Tree
   - `QSqlTableModel` → Database table
   - `QSortFilterProxyModel` → Filter/Sort

2. **Data được lưu ở đâu?**
   - `QVector<Item> m_items;`
   - `QStringList m_data;`
   - `Node *rootNode;`
   - Database (cho SQL models)

3. **rowCount() và columnCount()**
   - Trả về kích thước data như thế nào?

4. **data(index, role)**
   - `switch(role)` → xử lý các roles
   - `index.row()` và `index.column()` → lấy data từ đâu

5. **Có index() và parent() không?**
   - Có → Tree model
   - Dùng `internalPointer()` để lưu node

6. **Có setData() không?**
   - Có → Editable
   - Phải emit `dataChanged()`

7. **Có filterAcceptsRow() không?**
   - Có → Proxy model
   - Logic filter ở đây

8. **Có dùng QSql không?**
   - `setTable()` → QSqlTableModel
   - `setQuery()` → QSqlQueryModel
   - `setRelation()` → QSqlRelationalTableModel

---

## 🚀 Performance Tips

1. **Cache data** thay vì tính toán mỗi lần `data()` được gọi
2. **Dùng `beginResetModel()`/`endResetModel()`** khi thay đổi toàn bộ data
3. **Emit `dataChanged()` với range** thay vì từng cell riêng lẻ
4. **Dùng `canFetchMore()`/`fetchMore()`** cho lazy loading
5. **Override `sort()`** cho custom sorting nhanh hơn proxy
6. **Dùng `QSqlTableModel::OnManualSubmit`** cho batch updates

---

## 🎓 Learning Path

### Beginner
1. ✅ QStringListModel + QListView
2. ✅ Custom QAbstractListModel
3. ✅ QAbstractTableModel

### Intermediate
4. ✅ QSqlDatabase setup
5. ✅ QSqlTableModel
6. ✅ QSortFilterProxyModel

### Advanced
7. ✅ QAbstractItemModel (tree)
8. ✅ QSqlRelationalTableModel
9. ✅ Custom proxy models
10. ✅ Combine SQL + Proxy + Custom delegates

---

**💡 Tip:** Luôn bắt đầu với models đơn giản (List/Table) trước khi làm Tree models!
