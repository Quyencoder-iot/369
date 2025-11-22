# ⚡ Checklist Đọc Code Qt Models Nhanh

## 🎯 Mục Tiêu: Hiểu code trong 5 phút

Khi gặp một file Qt Model code, làm theo checklist này theo thứ tự:

---

## ✅ BƯỚC 1: Xác Định Loại Model (10 giây)

Nhìn vào khai báo class:

```cpp
class MyModel : public QAbstractListModel        → LIST
class MyModel : public QAbstractTableModel       → TABLE
class MyModel : public QAbstractItemModel        → TREE
class MyModel : public QSqlQueryModel            → SQL (read-only)
class MyModel : public QSqlTableModel            → SQL (editable)
class MyModel : public QSqlRelationalTableModel  → SQL (foreign keys)
class MyModel : public QSortFilterProxyModel     → PROXY (filter/sort)
```

**✍️ Ghi nhớ:** Loại model này là: _____________

---

## ✅ BƯỚC 2: Tìm Data Storage (30 giây)

Tìm trong private/protected members:

```cpp
// Common patterns:
QVector<Item> m_items;           → List of structs
QStringList m_data;              → List of strings
QList<Row> m_rows;               → Table rows
Node *rootNode;                  → Tree root
QVector<QVector<T>> m_table;     → 2D array

// SQL models không có storage (data từ DB)
```

**✍️ Ghi nhớ:** Data được lưu ở: _____________

---

## ✅ BƯỚC 3: Đếm Dimensions (20 giây)

### List Model (1D)
```cpp
int rowCount(const QModelIndex &parent) const {
    return m_items.count();  ← CHỈ CÓ ROWS
}
```

### Table Model (2D)
```cpp
int rowCount(...)    { return m_rows.count(); }     ← ROWS
int columnCount(...) { return NUM_COLUMNS; }        ← COLUMNS
```

### Tree Model (Recursive)
```cpp
int rowCount(const QModelIndex &parent) const {
    Node *node = parent.isValid() 
        ? getNode(parent) 
        : rootNode;
    return node->children.count();  ← CHILDREN COUNT
}
```

**✍️ Ghi nhớ:** Kích thước: _____ hàng, _____ cột

---

## ✅ BƯỚC 4: Phân Tích data() Function (2 phút)

Đây là phần QUAN TRỌNG NHẤT! Xem model trả về gì cho mỗi role:

```cpp
QVariant data(const QModelIndex &index, int role) const {
    // 1. Check index validity
    if (!index.isValid()) return QVariant();
    
    // 2. Get data item
    const Item &item = m_items[index.row()];
    
    // 3. Handle roles
    switch (role) {
        case Qt::DisplayRole:
            return item.text;           ← Text hiển thị
        case Qt::DecorationRole:
            return item.icon;           ← Icon
        case Qt::BackgroundRole:
            return item.color;          ← Màu nền
        case Qt::UserRole:
            return item.customData;     ← Custom data
    }
}
```

**✍️ Điền bảng này:**

| Role | Trả về gì |
|------|-----------|
| DisplayRole | ________________ |
| DecorationRole | ________________ |
| BackgroundRole | ________________ |
| ForegroundRole | ________________ |
| UserRole + X | ________________ |

---

## ✅ BƯỚC 5: Kiểm Tra Editable (1 phút)

### Tìm setData()
```cpp
bool setData(const QModelIndex &index, 
             const QVariant &value, 
             int role) {
    // CÓ function này → EDITABLE
    // KHÔNG CÓ → READ-ONLY
    
    m_items[index.row()].text = value.toString();
    emit dataChanged(index, index, {role});  ← CHÚ Ý SIGNAL
    return true;
}
```

### Tìm flags()
```cpp
Qt::ItemFlags flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;      ← EDITABLE
    flags |= Qt::ItemIsUserCheckable; ← CHECKABLE
    return flags;
}
```

**✍️ Ghi nhớ:** 
- [ ] Editable?
- [ ] Checkable?
- [ ] Drag/Drop?

---

## ✅ BƯỚC 6: Insert/Remove Logic (1 phút)

### Tìm hàm thêm data
```cpp
void addItem(const Item &item) {
    int row = m_items.count();
    
    beginInsertRows(QModelIndex(), row, row);  ← BẮT ĐẦU
    m_items.append(item);
    endInsertRows();                           ← KẾT THÚC
}
```

### Tìm hàm xóa data
```cpp
bool removeItem(int row) {
    beginRemoveRows(QModelIndex(), row, row);  ← BẮT ĐẦU
    m_items.remove(row);
    endRemoveRows();                           ← KẾT THÚC
    return true;
}
```

**✍️ Ghi nhớ:**
- [ ] Có hàm thêm/xóa không?
- [ ] Có gọi begin/end không?

---

## ✅ BƯỚC 7: Tree-Specific (chỉ cho Tree Models)

### index() - Tạo index từ row/col/parent
```cpp
QModelIndex index(int row, int column, 
                 const QModelIndex &parent) const {
    // 1. Get parent node
    Node *parentNode = parent.isValid() 
        ? static_cast<Node*>(parent.internalPointer())
        : rootNode;
    
    // 2. Get child node
    Node *childNode = parentNode->children[row];
    
    // 3. Create index with child pointer
    return createIndex(row, column, childNode);
           ← QUAN TRỌNG: internalPointer
}
```

### parent() - Tìm parent từ child
```cpp
QModelIndex parent(const QModelIndex &child) const {
    // 1. Get child node from index
    Node *childNode = static_cast<Node*>(
        child.internalPointer()
    );
    
    // 2. Get parent node
    Node *parentNode = childNode->parent;
    
    // 3. Create parent index
    int row = findRowOfNode(parentNode);
    return createIndex(row, 0, parentNode);
}
```

**✍️ Ghi nhớ:**
- internalPointer lưu: _____________
- Root được biểu diễn bằng: _____________

---

## ✅ BƯỚC 8: SQL-Specific (chỉ cho SQL Models)

```cpp
// Setup
model->setTable("table_name");
model->select();

// Relations (cho QSqlRelationalTableModel)
model->setRelation(
    columnIndex,  ← Column có foreign key
    QSqlRelation(
        "other_table",  ← Bảng được join
        "id",           ← Foreign key column
        "name"          ← Display column
    )
);

// Edit strategy
model->setEditStrategy(
    QSqlTableModel::OnManualSubmit  ← Phải submitAll()
    // OnFieldChange → Lưu ngay
    // OnRowChange → Lưu khi đổi row
);
```

**✍️ Ghi nhớ:**
- Table name: _____________
- Edit strategy: _____________
- Foreign keys: _____________

---

## ✅ BƯỚC 9: Proxy-Specific (chỉ cho Proxy Models)

### filterAcceptsRow() - Logic filter
```cpp
bool filterAcceptsRow(int sourceRow, 
                     const QModelIndex &sourceParent) const {
    // Get data from SOURCE model
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString text = sourceModel()->data(index).toString();
    
    // Return true to SHOW, false to HIDE
    return text.contains(m_filter);
}
```

### lessThan() - Logic sort
```cpp
bool lessThan(const QModelIndex &left, 
             const QModelIndex &right) const {
    // Compare values from source model
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    return leftData < rightData;
}
```

**✍️ Ghi nhớ:**
- Filter logic: _____________
- Sort logic: _____________

---

## ✅ BƯỚC 10: Signals Emitted (30 giây)

Tìm các signals được emit:

```cpp
emit dataChanged(topLeft, bottomRight);      ← Data thay đổi
emit layoutAboutToBeChanged();               ← Trước khi đổi layout
emit layoutChanged();                        ← Sau khi đổi layout
emit rowsInserted(parent, first, last);      ← Thêm rows
emit rowsRemoved(parent, first, last);       ← Xóa rows
emit modelReset();                           ← Reset toàn bộ
```

**✍️ Ghi nhớ:** Signals nào được dùng: _____________

---

## 📊 SUMMARY TEMPLATE

Sau khi đọc xong, điền template này:

```
MODEL TYPE: _______________
DATA STORAGE: _______________
DIMENSIONS: _____ rows x _____ columns
EDITABLE: Yes / No
KEY ROLES HANDLED:
  - DisplayRole: _______________
  - Other: _______________

INSERT/REMOVE: Yes / No
TREE STRUCTURE: Yes / No
DATABASE: Yes / No
PROXY FILTERING: Yes / No

SPECIAL NOTES:
_______________________________________________
_______________________________________________
```

---

## 🚨 Red Flags - Lỗi Thường Gặp

Khi đọc code, CHÚ Ý những lỗi này:

### ❌ Thiếu begin/end
```cpp
// WRONG
m_items.append(item);  // Không có beginInsertRows()!
```

### ❌ Không emit dataChanged
```cpp
// WRONG
m_items[row] = newValue;  // Không emit signal!
```

### ❌ Không check index validity
```cpp
// WRONG
QVariant data(...) {
    return m_items[index.row()];  // Không check isValid()!
}
```

### ❌ View dùng wrong model với proxy
```cpp
// WRONG
view->setModel(sourceModel);  // Phải dùng proxyModel!

// WRONG
int row = view->currentIndex().row();
sourceModel->removeRow(row);  // Phải map index!
```

---

## 🎯 Quick Decision Tree

```
Đọc code → Xác định loại
              │
    ┌─────────┼─────────┐
    │         │         │
  List?    Table?    Tree?    SQL?    Proxy?
    │         │         │       │        │
    ▼         ▼         ▼       ▼        ▼
Tìm       Tìm        Tìm    Tìm      Tìm
m_items   m_rows    rootNode setTable() filterAccepts
    │         │         │       │        Row()
    ▼         ▼         ▼       ▼        ▼
data()    data()    index()  select()  lessThan()
          col loop  parent()
```

---

## 💡 Speed Reading Tips

1. **Skip constructor** lúc đầu - quay lại sau
2. **Đọc data() TRƯỚC** - đây là core logic
3. **Scan qua all functions** - tìm pattern
4. **Focus vào signals** - hiểu data flow
5. **Ignore helpers** lúc đầu - chi tiết sau

---

## 📱 Mobile Quick Reference

Khi đọc code trên điện thoại/tablet, CHỈ CẦN nhớ 3 điều:

1. **Loại model**: List/Table/Tree/SQL/Proxy?
2. **Data storage**: Lưu ở đâu?
3. **data() function**: Trả về gì?

Ba điều này cho bạn 80% understanding!

---

**Chúc bạn đọc code hiệu quả! ⚡**

---

## 🎓 Practice Exercise

Thử đọc một trong các example files theo checklist này:
- `examples/01_list_model.cpp` - BẮT ĐẦU ĐÂY
- `examples/02_table_model.cpp`
- `examples/04_proxy_model.cpp`

Mục tiêu: Hoàn thành checklist trong 5 phút!
