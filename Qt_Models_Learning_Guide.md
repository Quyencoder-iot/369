# Hướng Dẫn Học Qt Models - Model/View Architecture

## Mục Lục
1. [Tổng Quan Kiến Trúc Model/View](#tổng-quan-kiến-trúc-modelview)
2. [Hierarchy và Mối Quan Hệ](#hierarchy-và-mối-quan-hệ)
3. [Chi Tiết Các Loại Model](#chi-tiết-các-loại-model)
4. [Phương Pháp Đọc Code Hiệu Quả](#phương-pháp-đọc-code-hiệu-quả)
5. [Ví Dụ Thực Tế](#ví-dụ-thực-tế)

---

## Tổng Quan Kiến Trúc Model/View

### Nguyên Lý Cơ Bản
Qt sử dụng kiến trúc **Model/View** để tách biệt:
- **Model**: Quản lý dữ liệu và logic nghiệp vụ
- **View**: Hiển thị dữ liệu (QListView, QTableView, QTreeView)
- **Delegate**: Tùy chỉnh cách render và edit (không bắt buộc)

```
┌─────────────┐         ┌──────────────┐
│   Model     │────────▶│     View     │
│  (Data)     │         │  (Display)   │
└─────────────┘         └──────────────┘
      ▲                        │
      │                        ▼
      │                 ┌──────────────┐
      └─────────────────│   Delegate   │
                        │   (Render)   │
                        └──────────────┘
```

---

## Hierarchy và Mối Quan Hệ

### Sơ Đồ Phân Cấp Class

```
QAbstractItemModel (Abstract base class)
├── QAbstractListModel (For list data - 1D)
│   ├── QStringListModel
│   └── Custom List Models
│
├── QAbstractTableModel (For table data - 2D)
│   └── Custom Table Models
│
├── QStandardItemModel (Ready-to-use, flexible)
│   └── Can be used for lists, tables, trees
│
├── QFileSystemModel (For file systems)
│
└── SQL Models (Database integration)
    ├── QSqlQueryModel (Read-only SQL)
    ├── QSqlTableModel (Editable single table)
    └── QSqlRelationalTableModel (With foreign keys)

Proxy Models (Wrapper around other models)
├── QSortFilterProxyModel (Sorting & filtering)
├── QIdentityProxyModel (Base for custom proxies)
└── Other specialized proxy models
```

### Mối Quan Hệ Giữa Các Model

```
┌─────────────────────────────────────────────────┐
│         QAbstractItemModel                      │
│  (Định nghĩa interface chung cho tất cả)       │
│  - data(), setData(), rowCount(), etc.         │
└─────────────────────────────────────────────────┘
                     ▲
                     │ inherits
        ┌────────────┼────────────┐
        │            │            │
┌───────▼──────┐ ┌──▼───────┐ ┌──▼──────────┐
│ List Model   │ │Table Model│ │ Tree Model  │
│ (1 column)   │ │(rows×cols)│ │(hierarchy)  │
└──────────────┘ └───────────┘ └─────────────┘
                     │
                     ▼
              ┌─────────────┐
              │ Proxy Model │ ◄── Wraps any model
              │ (Transform) │     to add features
              └─────────────┘
```

---

## Chi Tiết Các Loại Model

### 1. QAbstractItemModel
**Là gì?** Class cơ sở trừu tượng cho tất cả models.

**Các method quan trọng cần implement:**
```cpp
// BẮT BUỘC cho read-only model:
int rowCount(const QModelIndex &parent = QModelIndex()) const override;
int columnCount(const QModelIndex &parent = QModelIndex()) const override;
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
QModelIndex parent(const QModelIndex &child) const override;

// THÊM cho editable model:
bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
Qt::ItemFlags flags(const QModelIndex &index) const override;

// THÊM cho insertable/removable:
bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
```

**Khi nào dùng?** Khi cần cấu trúc dữ liệu phức tạp hoặc tree hierarchy.

---

### 2. QAbstractListModel
**Là gì?** Model đơn giản cho dữ liệu dạng danh sách (1 chiều).

**Đơn giản hóa:**
- `columnCount()` luôn return 1
- `index()` và `parent()` đã được implement sẵn

**Chỉ cần implement:**
```cpp
int rowCount(const QModelIndex &parent = QModelIndex()) const override;
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
```

**Ví dụ use case:**
- Danh sách tên
- Lịch sử chat messages
- Todo list

**Code mẫu:**
```cpp
class MyListModel : public QAbstractListModel {
    QStringList m_data;
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_data.count();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_data.size())
            return QVariant();
        
        if (role == Qt::DisplayRole)
            return m_data.at(index.row());
        
        return QVariant();
    }
};
```

---

### 3. QAbstractTableModel
**Là gì?** Model cho dữ liệu dạng bảng (2 chiều: rows × columns).

**Đơn giản hóa:**
- `index()` và `parent()` đã được implement
- Không có hierarchy

**Chỉ cần implement:**
```cpp
int rowCount(const QModelIndex &parent = QModelIndex()) const override;
int columnCount(const QModelIndex &parent = QModelIndex()) const override;
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
```

**Ví dụ use case:**
- Bảng điểm
- Spreadsheet
- Dữ liệu CSV

**Code mẫu:**
```cpp
class MyTableModel : public QAbstractTableModel {
    QVector<QVector<QString>> m_data; // 2D array
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_data.size();
    }
    
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_data.isEmpty() ? 0 : m_data[0].size();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();
        
        if (role == Qt::DisplayRole)
            return m_data[index.row()][index.column()];
        
        return QVariant();
    }
};
```

---

### 4. Tree Model (QAbstractItemModel)
**Là gì?** Model cho dữ liệu có cấu trúc phân cấp (tree/hierarchy).

**Phức tạp nhất** vì phải implement:
- `index()`: Tạo index từ (row, column, parent)
- `parent()`: Tìm parent của một node

**Ví dụ use case:**
- File system
- Organization chart
- XML/JSON tree viewer

**Khái niệm quan trọng:**
```cpp
// Mỗi node cần track parent của nó
class TreeItem {
    TreeItem *m_parent;
    QList<TreeItem*> m_children;
    QVariant m_data;
};

// index() - tạo index và lưu pointer vào internalPointer
QModelIndex index(int row, int column, const QModelIndex &parent) const override {
    TreeItem *parentItem = parent.isValid() ? 
        static_cast<TreeItem*>(parent.internalPointer()) : rootItem;
    
    TreeItem *childItem = parentItem->child(row);
    return createIndex(row, column, childItem);
}

// parent() - lấy parent từ internalPointer
QModelIndex parent(const QModelIndex &child) const override {
    TreeItem *childItem = static_cast<TreeItem*>(child.internalPointer());
    TreeItem *parentItem = childItem->parent();
    
    if (parentItem == rootItem)
        return QModelIndex();
    
    return createIndex(parentItem->row(), 0, parentItem);
}
```

---

### 5. Proxy Models
**Là gì?** Wrapper bọc quanh model khác để thêm tính năng mà KHÔNG thay đổi model gốc.

#### QSortFilterProxyModel
**Chức năng:**
- Sắp xếp (sorting)
- Lọc (filtering)
- Không thay đổi dữ liệu gốc

**Cách dùng:**
```cpp
QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
proxy->setSourceModel(myOriginalModel);  // Set model gốc
proxy->setFilterKeyColumn(1);             // Lọc theo cột 1
proxy->setFilterRegExp(QRegExp("pattern", Qt::CaseInsensitive));

view->setModel(proxy);  // View dùng proxy, không dùng model gốc
```

**Override để custom:**
```cpp
class MyProxyModel : public QSortFilterProxyModel {
protected:
    // Custom filter logic
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
        QString text = sourceModel()->data(index).toString();
        return text.contains("keyword");
    }
    
    // Custom sort logic
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        // Custom comparison logic
        return sourceModel()->data(left).toInt() < sourceModel()->data(right).toInt();
    }
};
```

**Sơ đồ hoạt động:**
```
Original Model              Proxy Model              View
┌──────────┐               ┌──────────┐            ┌──────┐
│ Row 0: C │               │ Row 0: A │            │  A   │
│ Row 1: A │──────────────▶│ Row 1: B │───────────▶│  B   │
│ Row 2: B │  setSourceModel│ Row 2: C │  setModel │  C   │
└──────────┘               └──────────┘            └──────┘
                           (sorted/filtered)
```

#### QIdentityProxyModel
**Là gì?** Proxy 1:1 (không thay đổi gì), dùng làm base class để custom.

**Khi nào dùng:**
- Override `data()` để thay đổi cách hiển thị (thêm icon, màu, format)
- Thêm columns ảo
- Transform dữ liệu không invasive

```cpp
class HighlightProxyModel : public QIdentityProxyModel {
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (role == Qt::BackgroundRole) {
            QVariant value = QIdentityProxyModel::data(index, Qt::DisplayRole);
            if (value.toInt() > 100)
                return QColor(Qt::red);
        }
        return QIdentityProxyModel::data(index, role);
    }
};
```

---

### 6. SQL Models (QSql)
**Là gì?** Models kết nối trực tiếp với database SQL.

#### QSqlQueryModel (Read-only)
```cpp
QSqlQueryModel *model = new QSqlQueryModel;
model->setQuery("SELECT name, age FROM users");
model->setHeaderData(0, Qt::Horizontal, "Name");
model->setHeaderData(1, Qt::Horizontal, "Age");

tableView->setModel(model);
```

#### QSqlTableModel (Editable)
```cpp
QSqlTableModel *model = new QSqlTableModel;
model->setTable("users");
model->setEditStrategy(QSqlTableModel::OnManualSubmit);
model->select();  // Load data

// Edit
model->setData(model->index(0, 1), "New Name");
model->submitAll();  // Save to database
```

#### QSqlRelationalTableModel (With Foreign Keys)
```cpp
QSqlRelationalTableModel *model = new QSqlRelationalTableModel;
model->setTable("orders");
model->setRelation(2, QSqlRelation("customers", "id", "name"));
model->select();

// Cột 2 sẽ tự động hiển thị customer name thay vì customer_id
```

**Sơ đồ:**
```
Database                QSqlTableModel           View
┌──────────┐           ┌──────────┐            ┌──────┐
│ SELECT * │           │ Row 0    │            │ Data │
│ FROM tbl │──────────▶│ Row 1    │───────────▶│ Grid │
│          │  setTable │ Row 2    │  setModel  │      │
└──────────┘           └──────────┘            └──────┘
     ▲                      │
     └──────submitAll()─────┘
```

---

## Phương Pháp Đọc Code Hiệu Quả

### Bước 1: Xác Định Loại Model
Khi đọc code Qt model, hãy trả lời:

1. **Model này inherit từ class nào?**
   - `QAbstractListModel` → Dữ liệu 1D (danh sách)
   - `QAbstractTableModel` → Dữ liệu 2D (bảng)
   - `QAbstractItemModel` → Dữ liệu tree hoặc phức tạp
   - `QSortFilterProxyModel` → Wrapper/proxy
   - `QSqlTableModel` → Database

2. **Mục đích của model?**
   - Read-only hay editable?
   - Static data hay dynamic (insert/remove)?

### Bước 2: Tập Trung Vào Các Methods Quan Trọng

**Thứ tự ưu tiên đọc:**

```cpp
// 1. HIỂU CẤU TRÚC DỮ LIỆU
private:
    QVector<MyData> m_data;  // ← Đọc đầu tiên: Data structure

// 2. ĐỌC rowCount/columnCount
int rowCount(...) const override {  // ← Số lượng items
    return m_data.size();
}

// 3. ĐỌC data() - QUAN TRỌNG NHẤT
QVariant data(const QModelIndex &index, int role) const override {
    // Role là gì?
    // - Qt::DisplayRole: Text hiển thị
    // - Qt::EditRole: Dữ liệu để edit
    // - Qt::DecorationRole: Icon
    // - Qt::BackgroundRole: Màu nền
    // - Custom roles: App-specific data
    
    if (role == Qt::DisplayRole)
        return m_data[index.row()].name;
    
    return QVariant();
}

// 4. setData() nếu editable
bool setData(const QModelIndex &index, const QVariant &value, int role) override {
    if (role == Qt::EditRole) {
        m_data[index.row()].name = value.toString();
        emit dataChanged(index, index, {role});  // ← Notify view
        return true;
    }
    return false;
}

// 5. Signals
// dataChanged() - Dữ liệu thay đổi
// rowsInserted() - Thêm rows
// rowsRemoved() - Xóa rows
// layoutChanged() - Toàn bộ structure thay đổi
```

### Bước 3: Hiểu QModelIndex

```cpp
QModelIndex index;

// QModelIndex chứa 4 thông tin:
index.row()              // Hàng (0-based)
index.column()           // Cột (0-based)
index.parent()           // Parent index (cho tree)
index.internalPointer()  // Pointer đến internal data (tree)

// Kiểm tra validity
if (!index.isValid())
    return QVariant();

// Lấy data từ index
QVariant value = model->data(index, Qt::DisplayRole);
```

### Bước 4: Theo Dõi Luồng Dữ Liệu

```
User Action (View)
    ↓
View calls model->data(index, role)
    ↓
Model returns QVariant
    ↓
View displays data

User Edit (View)
    ↓
View calls model->setData(index, value, role)
    ↓
Model updates internal data
    ↓
Model emits dataChanged(index, index, {role})
    ↓
View updates display
```

### Bước 5: Debug Tips

```cpp
// In debug info trong data()
QVariant data(const QModelIndex &index, int role) const override {
    qDebug() << "data() called - row:" << index.row() 
             << "col:" << index.column() 
             << "role:" << role;
    
    // Giúp hiểu view đang request gì
}

// Test model độc lập
MyModel model;
qDebug() << "Rows:" << model.rowCount();
qDebug() << "Data:" << model.data(model.index(0, 0), Qt::DisplayRole);
```

---

## Ví Dụ Thực Tế

### Example 1: Simple List Model

```cpp
// PersonListModel.h
class Person {
public:
    QString name;
    int age;
};

class PersonListModel : public QAbstractListModel {
    Q_OBJECT
    QList<Person> m_people;
    
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        AgeRole
    };
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_people.count();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_people.size())
            return QVariant();
        
        const Person &person = m_people.at(index.row());
        
        switch (role) {
        case Qt::DisplayRole:
            return QString("%1 (%2)").arg(person.name).arg(person.age);
        case NameRole:
            return person.name;
        case AgeRole:
            return person.age;
        }
        
        return QVariant();
    }
    
    // Editable
    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (!index.isValid() || index.row() >= m_people.size())
            return false;
        
        Person &person = m_people[index.row()];
        
        if (role == NameRole) {
            person.name = value.toString();
            emit dataChanged(index, index, {role, Qt::DisplayRole});
            return true;
        } else if (role == AgeRole) {
            person.age = value.toInt();
            emit dataChanged(index, index, {role, Qt::DisplayRole});
            return true;
        }
        
        return false;
    }
    
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
    }
    
    // Custom methods
    void addPerson(const Person &person) {
        beginInsertRows(QModelIndex(), m_people.size(), m_people.size());
        m_people.append(person);
        endInsertRows();
    }
};

// Usage
PersonListModel *model = new PersonListModel;
model->addPerson({"Alice", 25});
model->addPerson({"Bob", 30});

QListView *view = new QListView;
view->setModel(model);
```

### Example 2: Table Model with Proxy

```cpp
// Data model
class DataTableModel : public QAbstractTableModel {
    QVector<QVector<QString>> m_data;
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_data.size();
    }
    
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_data.isEmpty() ? 0 : m_data[0].size();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || role != Qt::DisplayRole)
            return QVariant();
        return m_data[index.row()][index.column()];
    }
};

// Filter proxy
class MyFilterProxy : public QSortFilterProxyModel {
    QString m_filterText;
public:
    void setFilterText(const QString &text) {
        m_filterText = text;
        invalidateFilter();  // Trigger re-filtering
    }
    
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        if (m_filterText.isEmpty())
            return true;
        
        // Check if any column contains filter text
        for (int col = 0; col < sourceModel()->columnCount(); ++col) {
            QModelIndex index = sourceModel()->index(source_row, col, source_parent);
            QString data = sourceModel()->data(index).toString();
            if (data.contains(m_filterText, Qt::CaseInsensitive))
                return true;
        }
        return false;
    }
};

// Usage
DataTableModel *sourceModel = new DataTableModel;
MyFilterProxy *proxyModel = new MyFilterProxy;
proxyModel->setSourceModel(sourceModel);

QTableView *view = new QTableView;
view->setModel(proxyModel);

// Connect search box
QLineEdit *searchBox = new QLineEdit;
connect(searchBox, &QLineEdit::textChanged, proxyModel, &MyFilterProxy::setFilterText);
```

### Example 3: SQL Model with Proxy

```cpp
// Setup database
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("mydb.db");
db.open();

// Create and populate SQL model
QSqlTableModel *sqlModel = new QSqlTableModel;
sqlModel->setTable("employees");
sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
sqlModel->select();

// Add sorting/filtering
QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
proxy->setSourceModel(sqlModel);
proxy->setFilterKeyColumn(1);  // Filter on "name" column

QTableView *view = new QTableView;
view->setModel(proxy);
view->setSortingEnabled(true);

// Edit and save
sqlModel->setData(sqlModel->index(0, 1), "New Name");
sqlModel->submitAll();
```

---

## Checklist Khi Đọc Code Model

- [ ] Model inherit từ class nào? (List/Table/ItemModel/Proxy/SQL)
- [ ] Cấu trúc dữ liệu internal là gì? (QVector, QList, database, etc.)
- [ ] Model có editable không? (Kiểm tra `setData()` và `flags()`)
- [ ] Có dynamic insert/remove không? (Kiểm tra `insertRows()`/`removeRows()`)
- [ ] Roles nào được support? (DisplayRole, EditRole, custom roles)
- [ ] Có sử dụng proxy không? (Sorting/filtering/transformation)
- [ ] Signals nào được emit? (dataChanged, rowsInserted, etc.)
- [ ] Performance: Có cache không? Có lazy loading không?

---

## Tài Nguyên Học Thêm

### Tài liệu chính thức:
- Qt Documentation: Model/View Programming
- Qt Examples: Item Views Examples

### Thứ tự học đề xuất:
1. **Bắt đầu:** QStringListModel (simplest)
2. **Tiếp theo:** QAbstractListModel với custom data
3. **Sau đó:** QAbstractTableModel
4. **Nâng cao:** QSortFilterProxyModel
5. **Database:** QSqlTableModel
6. **Cao cấp:** Tree model với QAbstractItemModel

### Practice Tips:
- Tạo model đơn giản trước (list of strings)
- Thêm edit functionality
- Thử proxy để sort/filter
- Implement custom roles
- Tạo tree model sau cùng (khó nhất)

---

## Tổng Kết

### Model nào khi nào?

| Use Case | Model Type | Complexity |
|----------|-----------|------------|
| Danh sách đơn giản | QStringListModel | ⭐ |
| Danh sách custom data | QAbstractListModel | ⭐⭐ |
| Bảng dữ liệu | QAbstractTableModel | ⭐⭐ |
| Database table | QSqlTableModel | ⭐⭐ |
| Cây phân cấp | QAbstractItemModel | ⭐⭐⭐⭐ |
| Sort/Filter | QSortFilterProxyModel | ⭐⭐⭐ |
| Transform data | QIdentityProxyModel | ⭐⭐⭐ |

### Key Takeaways:
1. **Model = Data Management**, View = Display
2. **List < Table < Tree** (độ phức tạp tăng dần)
3. **Proxy = Wrapper** (không thay đổi source model)
4. **SQL models** connect trực tiếp database
5. **Signals** (dataChanged, etc.) để notify views
6. **QModelIndex** để truy cập data (row, column, parent)

Chúc bạn học tốt! 🚀
