# Hướng Dẫn Học Qt Models và QSql

## 📚 Mục Lục
1. [Tổng Quan](#tổng-quan)
2. [Model/View Architecture](#modelview-architecture)
3. [Các Loại Models](#các-loại-models)
4. [QSql Models](#qsql-models)
5. [Proxy Models](#proxy-models)
6. [Mối Liên Hệ Giữa Các Models](#mối-liên-hệ-giữa-các-models)
7. [Tips Đọc Code Nhanh](#tips-đọc-code-nhanh)

---

## 🎯 Tổng Quan

Qt Model/View là một kiến trúc tách biệt **dữ liệu (Model)** khỏi **giao diện hiển thị (View)**:

```
┌─────────────────────────────────────────────┐
│           MODEL/VIEW ARCHITECTURE            │
├─────────────────────────────────────────────┤
│                                              │
│  ┌─────────┐      ┌──────────┐             │
│  │  MODEL  │ ◄────┤  VIEW    │             │
│  │ (Data)  │      │ (Display)│             │
│  └─────────┘      └──────────┘             │
│       ▲                ▲                     │
│       │                │                     │
│       └────┬───────────┘                     │
│            │                                 │
│      ┌─────▼──────┐                         │
│      │ DELEGATE   │                         │
│      │ (Editing)  │                         │
│      └────────────┘                         │
│                                              │
└─────────────────────────────────────────────┘
```

### Lợi Ích:
- ✅ Tái sử dụng code
- ✅ Dễ bảo trì
- ✅ Nhiều views có thể dùng chung 1 model
- ✅ Dễ test

---

## 🏗️ Model/View Architecture

### 3 Thành Phần Chính:

1. **Model** (QAbstractItemModel): Quản lý dữ liệu
2. **View** (QListView, QTableView, QTreeView): Hiển thị dữ liệu
3. **Delegate** (QStyledItemDelegate): Tùy chỉnh cách hiển thị và edit

### Các Phương Thức Quan Trọng Của Model:

```cpp
// CẦN PHẢI IMPLEMENT:
virtual int rowCount(const QModelIndex &parent) const = 0;
virtual QVariant data(const QModelIndex &index, int role) const = 0;

// CHO TABLE/TREE:
virtual int columnCount(const QModelIndex &parent) const;
virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
virtual QModelIndex parent(const QModelIndex &child) const;

// CHO EDITING:
virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
virtual Qt::ItemFlags flags(const QModelIndex &index) const;

// THÊM/XÓA DATA:
virtual bool insertRows(int row, int count, const QModelIndex &parent);
virtual bool removeRows(int row, int count, const QModelIndex &parent);
```

---

## 📝 Các Loại Models

### 1. **QAbstractListModel** - Cho Danh Sách 1 Chiều

**Khi nào dùng:**
- Danh sách đơn giản (contacts, tasks, items)
- Không cần cột, chỉ cần hàng
- Dữ liệu tuần tự

**Chỉ cần implement:**
- `rowCount()`
- `data()`
- `setData()` (nếu editable)

**Ví dụ thực tế:**
```cpp
class TodoListModel : public QAbstractListModel {
    QStringList m_todos;
public:
    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_todos.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_todos.size())
            return QVariant();
        
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_todos.at(index.row());
        
        return QVariant();
    }
};
```

### 2. **QAbstractTableModel** - Cho Bảng 2 Chiều

**Khi nào dùng:**
- Dữ liệu dạng bảng (spreadsheet)
- Có hàng và cột
- Mỗi ô độc lập

**Cần implement:**
- `rowCount()`
- `columnCount()`
- `data()`
- `headerData()` (cho tiêu đề)
- `setData()` (nếu editable)

**Ví dụ thực tế:**
```cpp
class StudentTableModel : public QAbstractTableModel {
    struct Student { QString name; int age; QString grade; };
    QVector<Student> m_students;
    
public:
    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_students.count();
    }
    
    int columnCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : 3; // Name, Age, Grade
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_students.size())
            return QVariant();
        
        if (role == Qt::DisplayRole) {
            const Student &student = m_students.at(index.row());
            switch (index.column()) {
                case 0: return student.name;
                case 1: return student.age;
                case 2: return student.grade;
            }
        }
        return QVariant();
    }
    
    QVariant headerData(int section, Qt::Orientation orientation, 
                        int role) const override {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
                case 0: return "Tên";
                case 1: return "Tuổi";
                case 2: return "Điểm";
            }
        }
        return QVariant();
    }
};
```

### 3. **QAbstractItemModel** - Cho Cây Phân Cấp

**Khi nào dùng:**
- Dữ liệu phân cấp (file system, org chart)
- Parent-child relationships
- Recursive structures

**Cần implement:**
- `rowCount()`
- `columnCount()`
- `data()`
- `index()` - TẠO INDEX CHO ITEM
- `parent()` - TÌM PARENT CỦA ITEM

**Khái niệm quan trọng - QModelIndex:**
```cpp
// QModelIndex lưu:
// - row: hàng
// - column: cột  
// - internalPointer: con trỏ đến data (cho tree)
// - model: model chứa nó

// INVALID index = root
QModelIndex root; // root.isValid() == false
```

**Ví dụ thực tế:**
```cpp
class FileSystemModel : public QAbstractItemModel {
    struct Node {
        QString name;
        Node *parent;
        QVector<Node*> children;
    };
    Node *rootNode;
    
public:
    // Tạo index cho item
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
    
    // Tìm parent của item
    QModelIndex parent(const QModelIndex &child) const override {
        if (!child.isValid())
            return QModelIndex();
        
        Node *childNode = static_cast<Node*>(child.internalPointer());
        Node *parentNode = childNode->parent;
        
        if (parentNode == rootNode)
            return QModelIndex(); // root
        
        // Tìm row của parent trong grandparent
        int row = parentNode->parent->children.indexOf(parentNode);
        return createIndex(row, 0, parentNode);
    }
    
    int rowCount(const QModelIndex &parent) const override {
        Node *parentNode = parent.isValid()
            ? static_cast<Node*>(parent.internalPointer())
            : rootNode;
        return parentNode->children.count();
    }
    
    int columnCount(const QModelIndex &parent) const override {
        return 1;
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid())
            return QVariant();
        
        if (role == Qt::DisplayRole) {
            Node *node = static_cast<Node*>(index.internalPointer());
            return node->name;
        }
        return QVariant();
    }
};
```

---

## 🗄️ QSql Models

QSql Models kết nối trực tiếp với database:

### 1. **QSqlQueryModel** - Read-Only

**Đặc điểm:**
- Chỉ đọc (read-only)
- Dùng SQL query tùy ý
- Đơn giản nhất

```cpp
QSqlQueryModel *model = new QSqlQueryModel;
model->setQuery("SELECT name, age FROM students WHERE age > 18");
model->setHeaderData(0, Qt::Horizontal, "Tên");
model->setHeaderData(1, Qt::Horizontal, "Tuổi");

QTableView *view = new QTableView;
view->setModel(model);
```

### 2. **QSqlTableModel** - Editable Single Table

**Đặc điểm:**
- Có thể edit
- Cho 1 table
- Tự động CRUD operations
- Strategies: OnFieldChange, OnRowChange, OnManualSubmit

```cpp
QSqlTableModel *model = new QSqlTableModel;
model->setTable("students");
model->setEditStrategy(QSqlTableModel::OnManualSubmit);
model->select();

// Set headers
model->setHeaderData(0, Qt::Horizontal, "ID");
model->setHeaderData(1, Qt::Horizontal, "Tên");
model->setHeaderData(2, Qt::Horizontal, "Tuổi");

// Filter
model->setFilter("age > 18");
model->setSort(1, Qt::AscendingOrder);

QTableView *view = new QTableView;
view->setModel(model);

// Thêm row mới
int row = model->rowCount();
model->insertRow(row);
model->setData(model->index(row, 1), "Nguyễn Văn A");
model->setData(model->index(row, 2), 20);
model->submitAll(); // Lưu vào DB
```

### 3. **QSqlRelationalTableModel** - Foreign Key Relations

**Đặc điểm:**
- Xử lý foreign keys
- Hiển thị data từ related tables
- Tự động join tables

```cpp
// Table: students (id, name, department_id)
// Table: departments (id, name)

QSqlRelationalTableModel *model = new QSqlRelationalTableModel;
model->setTable("students");

// Thiết lập relation: column 2 (department_id) -> departments table
model->setRelation(2, QSqlRelation("departments", "id", "name"));

model->setHeaderData(0, Qt::Horizontal, "ID");
model->setHeaderData(1, Qt::Horizontal, "Tên SV");
model->setHeaderData(2, Qt::Horizontal, "Khoa"); // Sẽ hiện tên khoa

model->select();

// View sẽ hiển thị tên khoa thay vì ID
QTableView *view = new QTableView;
view->setModel(model);

// Delegate cho foreign key column
view->setItemDelegate(new QSqlRelationalDelegate(view));
```

---

## 🔍 Proxy Models

Proxy models là lớp trung gian giữa model và view, cho phép:
- **Filtering** (lọc)
- **Sorting** (sắp xếp)
- **Transforming** (biến đổi)

Mà KHÔNG thay đổi source model!

### 1. **QSortFilterProxyModel** - Phổ Biến Nhất

```cpp
// Source model
QStringListModel *sourceModel = new QStringListModel;
sourceModel->setStringList({"Apple", "Banana", "Cherry", "Date"});

// Proxy model
QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;
proxyModel->setSourceModel(sourceModel);

// Filter: chỉ hiện items chứa "a"
proxyModel->setFilterRegularExpression(QRegularExpression("a", 
    QRegularExpression::CaseInsensitiveOption));
proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

// Sort
proxyModel->sort(0, Qt::AscendingOrder);

QListView *view = new QListView;
view->setModel(proxyModel);
// View sẽ hiện: Apple, Banana, Date (đã lọc và sort)
```

### 2. **Custom Proxy Model**

```cpp
class AgeFilterProxyModel : public QSortFilterProxyModel {
protected:
    bool filterAcceptsRow(int sourceRow, 
                         const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 1, sourceParent);
        int age = sourceModel()->data(index).toInt();
        return age >= 18; // Chỉ hiện người >= 18 tuổi
    }
};
```

### 3. **Chaining Proxy Models**

```cpp
// Có thể xâu chuỗi nhiều proxy!
QSqlTableModel *source = new QSqlTableModel;
source->setTable("students");
source->select();

// Proxy 1: Filter theo tuổi
AgeFilterProxyModel *ageProxy = new AgeFilterProxyModel;
ageProxy->setSourceModel(source);

// Proxy 2: Sort theo tên
QSortFilterProxyModel *sortProxy = new QSortFilterProxyModel;
sortProxy->setSourceModel(ageProxy);
sortProxy->sort(1);

view->setModel(sortProxy);
```

---

## 🔗 Mối Liên Hệ Giữa Các Models

```
┌─────────────────────────────────────────────────────────┐
│                    HIERARCHY DIAGRAM                     │
├─────────────────────────────────────────────────────────┤
│                                                          │
│              QAbstractItemModel (Base)                   │
│                        │                                 │
│          ┌─────────────┼─────────────┬────────────┐     │
│          │             │             │            │     │
│    QAbstractList  QAbstractTable  QSqlQuery  QIdentity  │
│       Model           Model          Model     Proxy     │
│          │             │             │          Model    │
│          │             │             │                   │
│    ┌─────┴──┐    ┌─────┴──────┐     │                   │
│    │        │    │            │     │                   │
│ QString  Custom QSqlTable  Custom   │                   │
│  List    List    Model     Table    │                   │
│  Model   Model      │      Model    │                   │
│                     │               │                   │
│              QSqlRelational      QSortFilter            │
│                TableModel        ProxyModel             │
│                                      │                   │
│                                  Custom Proxy           │
│                                                          │
└─────────────────────────────────────────────────────────┘

LUỒNG DỮ LIỆU:
═══════════════

Database/Source Data
        │
        ▼
┌───────────────┐
│  SQL Model    │ ◄─── Direct DB connection
│ hoặc Custom   │
│    Model      │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│ Proxy Model   │ ◄─── Filter/Sort/Transform
│  (optional)   │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│     View      │ ◄─── Display
│ (Table/List/  │
│     Tree)     │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│   Delegate    │ ◄─── Custom rendering/editing
└───────────────┘
```

### Khi Nào Dùng Model Nào?

| Use Case | Model Type | Lý Do |
|----------|-----------|-------|
| Danh sách đơn giản | QStringListModel | Built-in, đơn giản |
| Danh sách custom | QAbstractListModel | Linh hoạt |
| Bảng read-only từ DB | QSqlQueryModel | Đơn giản, SQL tùy ý |
| Bảng editable từ DB | QSqlTableModel | Tự động CRUD |
| Bảng có foreign keys | QSqlRelationalTableModel | Xử lý relations |
| Cây phân cấp | QAbstractItemModel | Cần index() và parent() |
| Filter/Sort existing model | QSortFilterProxyModel | Không thay đổi source |
| Custom transformation | Custom QAbstractProxyModel | Logic phức tạp |

---

## 💡 Tips Đọc Code Nhanh

### 1. **Xác Định Loại Model**

Nhìn vào class kế thừa:
```cpp
class MyModel : public QAbstractListModel    // → List
class MyModel : public QAbstractTableModel   // → Table  
class MyModel : public QAbstractItemModel    // → Tree
class MyModel : public QSqlTableModel        // → SQL Table
class MyModel : public QSortFilterProxyModel // → Proxy
```

### 2. **Tìm Data Storage**

```cpp
// List model
QStringList m_data;
QVector<Item> m_items;

// Table model  
QVector<QVector<QVariant>> m_data;
struct Row { ... }; QVector<Row> m_rows;

// Tree model
struct Node { Node *parent; QVector<Node*> children; };
```

### 3. **Đọc Theo Thứ Tự**

1. **Constructor**: Khởi tạo data
2. **rowCount()/columnCount()**: Kích thước data
3. **data()**: Làm gì với từng cell
4. **setData()**: Update logic
5. **index()/parent()**: Tree structure (nếu có)

### 4. **Chú Ý Roles**

```cpp
if (role == Qt::DisplayRole)      // Text hiển thị
if (role == Qt::EditRole)         // Edit mode
if (role == Qt::DecorationRole)   // Icon
if (role == Qt::BackgroundRole)   // Background color
if (role == Qt::ForegroundRole)   // Text color
if (role == Qt::ToolTipRole)      // Tooltip
if (role == Qt::UserRole + X)     // Custom data
```

### 5. **Tìm Signals**

Models emit signals khi data thay đổi:
```cpp
emit dataChanged(topLeft, bottomRight);
emit layoutAboutToBeChanged();
emit layoutChanged();
emit rowsInserted(parent, first, last);
emit rowsRemoved(parent, first, last);
```

### 6. **Proxy Models - Tìm Mapping**

```cpp
// View index -> Source model index
QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);

// Source model index -> View index  
QModelIndex proxyIndex = proxyModel->mapFromSource(sourceIndex);
```

### 7. **Debug Techniques**

```cpp
// In ra cấu trúc model
void debugModel(QAbstractItemModel *model, QModelIndex parent = QModelIndex(), int indent = 0) {
    for (int row = 0; row < model->rowCount(parent); ++row) {
        QModelIndex index = model->index(row, 0, parent);
        qDebug() << QString(indent * 2, ' ') 
                 << model->data(index).toString();
        
        if (model->hasChildren(index)) {
            debugModel(model, index, indent + 1);
        }
    }
}
```

---

## 📖 Ví Dụ Tổng Hợp

### Example: Todo App với Filter

```cpp
// Model: Lưu todos
class TodoModel : public QAbstractListModel {
    struct Todo {
        QString text;
        bool completed;
    };
    QVector<Todo> m_todos;
    
public:
    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_todos.count();
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_todos.size())
            return QVariant();
        
        const Todo &todo = m_todos.at(index.row());
        
        switch (role) {
            case Qt::DisplayRole:
                return todo.text;
            case Qt::CheckStateRole:
                return todo.completed ? Qt::Checked : Qt::Unchecked;
            case Qt::ForegroundRole:
                return todo.completed ? QColor(Qt::gray) : QColor(Qt::black);
        }
        return QVariant();
    }
    
    bool setData(const QModelIndex &index, const QVariant &value, 
                 int role) override {
        if (!index.isValid() || index.row() >= m_todos.size())
            return false;
        
        if (role == Qt::CheckStateRole) {
            m_todos[index.row()].completed = value.toInt() == Qt::Checked;
            emit dataChanged(index, index, {role});
            return true;
        }
        return false;
    }
    
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractListModel::flags(index) | Qt::ItemIsUserCheckable;
    }
    
    void addTodo(const QString &text) {
        int row = m_todos.count();
        beginInsertRows(QModelIndex(), row, row);
        m_todos.append({text, false});
        endInsertRows();
    }
};

// Proxy: Filter completed/incomplete
class TodoFilterProxy : public QSortFilterProxyModel {
    bool m_showCompleted = true;
    
public:
    void setShowCompleted(bool show) {
        m_showCompleted = show;
        invalidateFilter();
    }
    
protected:
    bool filterAcceptsRow(int sourceRow, 
                         const QModelIndex &sourceParent) const override {
        if (m_showCompleted)
            return true;
        
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        bool completed = sourceModel()->data(index, Qt::CheckStateRole).toInt() 
                        == Qt::Checked;
        return !completed; // Chỉ hiện chưa hoàn thành
    }
};

// Usage
TodoModel *model = new TodoModel;
model->addTodo("Học Qt Models");
model->addTodo("Làm bài tập");

TodoFilterProxy *proxy = new TodoFilterProxy;
proxy->setSourceModel(model);

QListView *view = new QListView;
view->setModel(proxy);

// Toggle filter
QCheckBox *showCompleted = new QCheckBox("Hiện đã hoàn thành");
connect(showCompleted, &QCheckBox::toggled, proxy, &TodoFilterProxy::setShowCompleted);
```

---

## 🎓 Lộ Trình Học

### Tuần 1: Basics
- [ ] Đọc Qt documentation về Model/View
- [ ] Làm QStringListModel với QListView
- [ ] Làm custom QAbstractListModel

### Tuần 2: Tables
- [ ] Làm QAbstractTableModel
- [ ] Học QSqlDatabase setup
- [ ] Làm QSqlTableModel với QTableView

### Tuần 3: Advanced
- [ ] Làm QAbstractItemModel (tree)
- [ ] Làm QSqlRelationalTableModel
- [ ] Custom QSortFilterProxyModel

### Tuần 4: Practice
- [ ] Build complete CRUD app
- [ ] Combine SQL models + Proxy models
- [ ] Custom delegates

---

## 📚 Resources

- **Official Docs**: https://doc.qt.io/qt-6/model-view-programming.html
- **Examples**: Qt Creator → Examples → Model/View
- **Video**: "Qt Model/View Programming" tutorials
- **Book**: "Advanced Qt Programming" by Mark Summerfield

---

## ✅ Checklist Khi Đọc Code

- [ ] Model kế thừa từ class nào?
- [ ] Data được lưu ở đâu? (member variables)
- [ ] rowCount()/columnCount() trả về gì?
- [ ] data() xử lý những roles nào?
- [ ] Có setData() không? → Editable?
- [ ] Có index()/parent() không? → Tree structure?
- [ ] Có dùng proxy model không?
- [ ] Signals nào được emit?
- [ ] View nào được dùng? (QListView/QTableView/QTreeView)

---

**Good luck! 🚀**
