# Hướng Dẫn Học Qt Models - Kiến Trúc Model/View

## 📚 Mục Lục
1. [Tổng Quan Kiến Trúc Model/View](#tổng-quan)
2. [QAbstractItemModel - Base Class](#qabstractitemmodel)
3. [QAbstractListModel - List Model](#qabstractlistmodel)
4. [QAbstractTableModel - Table Model](#qabstracttablemodel)
5. [QAbstractItemModel - Tree Model](#tree-model)
6. [QAbstractProxyModel - Proxy Model](#proxy-model)
7. [QSqlQueryModel & QSqlTableModel - SQL Models](#sql-models)
8. [Mối Liên Hệ Giữa Các Models](#mối-liên-hệ)
9. [Cách Đọc Code Nhanh](#cách-đọc-code)

---

## 🎯 Tổng Quan Kiến Trúc Model/View {#tổng-quan}

```
┌─────────────┐
│    VIEW     │  ← Hiển thị dữ liệu (QListView, QTableView, QTreeView)
└──────┬──────┘
       │
       │ signals/slots
       │
┌──────▼──────┐
│    MODEL    │  ← Quản lý dữ liệu (QAbstractItemModel và các subclass)
└──────┬──────┘
       │
       │
┌──────▼──────┐
│    DATA     │  ← Nguồn dữ liệu thực tế (List, Database, File...)
└─────────────┘
```

**Nguyên lý cơ bản:**
- **Model**: Quản lý dữ liệu, không quan tâm cách hiển thị
- **View**: Hiển thị dữ liệu, không quan tâm cách lưu trữ
- **Delegate**: Định dạng cách hiển thị từng item

---

## 🔷 QAbstractItemModel - Base Class {#qabstractitemmodel}

**Đây là lớp cơ sở của TẤT CẢ các model trong Qt.**

### Cấu trúc phân cấp:
```
QAbstractItemModel (abstract - không thể dùng trực tiếp)
    │
    ├── QAbstractListModel (1 chiều - danh sách)
    │   └── QStringListModel
    │
    ├── QAbstractTableModel (2 chiều - bảng)
    │   └── QStandardItemModel
    │
    ├── QAbstractItemModel (đa chiều - tree)
    │   └── QStandardItemModel
    │
    ├── QAbstractProxyModel (proxy - lọc/sắp xếp)
    │   ├── QSortFilterProxyModel
    │   ├── QIdentityProxyModel
    │   └── QTransposeProxyModel
    │
    └── QSqlQueryModel (SQL)
        ├── QSqlQueryModel
        └── QSqlTableModel
```

### Các phương thức QUAN TRỌNG nhất cần nhớ:

```cpp
// 1. rowCount() - Số hàng
virtual int rowCount(const QModelIndex &parent = QModelIndex()) const = 0;

// 2. columnCount() - Số cột (chỉ cho Table/Tree)
virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;

// 3. data() - Lấy dữ liệu tại index
virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const = 0;

// 4. index() - Tạo QModelIndex từ row/column/parent
virtual QModelIndex index(int row, int column, 
                          const QModelIndex &parent = QModelIndex()) const = 0;

// 5. parent() - Lấy parent index (chỉ cho Tree)
virtual QModelIndex parent(const QModelIndex &index) const = 0;

// 6. setData() - Đặt dữ liệu (cho editable models)
virtual bool setData(const QModelIndex &index, const QVariant &value, 
                    int role = Qt::EditRole);

// 7. flags() - Quy định item có thể edit/select/check...
virtual Qt::ItemFlags flags(const QModelIndex &index) const;
```

### QModelIndex - "Địa chỉ" của một item:
```cpp
QModelIndex index = model->index(row, column, parent);
// index là "con trỏ" đến một item trong model
// Không phải con trỏ thật, mà là "handle" để truy cập
```

---

## 📋 QAbstractListModel - List Model {#qabstractlistmodel}

**Dùng cho dữ liệu 1 chiều (danh sách)**

### Đặc điểm:
- Chỉ có **row**, không có **column** (hoặc columnCount() = 1)
- Không có parent-child (flat structure)
- Ví dụ: Danh sách tên, danh sách số, danh sách email...

### Ví dụ đơn giản:

```cpp
#include <QAbstractListModel>
#include <QStringList>

class StringListModel : public QAbstractListModel {
    Q_OBJECT
    
private:
    QStringList m_data;  // Nguồn dữ liệu thực tế
    
public:
    StringListModel(QObject *parent = nullptr) 
        : QAbstractListModel(parent) {}
    
    // BẮT BUỘC phải implement 3 phương thức:
    
    // 1. Số lượng items
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_data.size();  // Số phần tử trong list
    }
    
    // 2. Lấy dữ liệu
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_data.size())
            return QVariant();
            
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_data.at(index.row());
            
        return QVariant();
    }
    
    // 3. Tạo index
    QModelIndex index(int row, int column = 0, 
                      const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        Q_UNUSED(column);
        if (row < 0 || row >= m_data.size())
            return QModelIndex();
        return createIndex(row, 0);  // column luôn = 0 cho ListModel
    }
    
    // Thêm dữ liệu
    void addString(const QString &str) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(str);
        endInsertRows();
    }
};
```

### Cách đọc code ListModel:
1. Tìm `rowCount()` → biết số lượng items
2. Tìm `data()` → biết cách lấy dữ liệu
3. Tìm container (QStringList, QList, QVector...) → biết nguồn dữ liệu thực tế

---

## 📊 QAbstractTableModel - Table Model {#qabstracttablemodel}

**Dùng cho dữ liệu 2 chiều (bảng)**

### Đặc điểm:
- Có cả **row** và **column**
- Không có parent-child (flat structure)
- Ví dụ: Bảng Excel, bảng database, ma trận...

### Ví dụ đơn giản:

```cpp
#include <QAbstractTableModel>
#include <QList>
#include <QStringList>

class TableModel : public QAbstractTableModel {
    Q_OBJECT
    
private:
    QList<QStringList> m_data;  // Mỗi QStringList là một hàng
    QStringList m_headers;       // Tên các cột
    
public:
    TableModel(QObject *parent = nullptr) 
        : QAbstractTableModel(parent) {
        m_headers << "Tên" << "Tuổi" << "Địa chỉ";
    }
    
    // BẮT BUỘC phải implement 4 phương thức:
    
    // 1. Số hàng
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_data.size();
    }
    
    // 2. Số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_headers.size();  // Hoặc m_data.isEmpty() ? 0 : m_data[0].size()
    }
    
    // 3. Lấy dữ liệu
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || 
            index.row() >= m_data.size() || 
            index.column() >= m_headers.size())
            return QVariant();
            
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_data.at(index.row()).at(index.column());
            
        return QVariant();
    }
    
    // 4. Header (tên cột/hàng)
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole)
            return QVariant();
            
        if (orientation == Qt::Horizontal)
            return m_headers.at(section);
        else
            return QString::number(section + 1);
    }
    
    // 5. Tạo index
    QModelIndex index(int row, int column, 
                      const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }
    
    // Thêm hàng
    void addRow(const QStringList &rowData) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(rowData);
        endInsertRows();
    }
};
```

### Cách đọc code TableModel:
1. Tìm `rowCount()` → số hàng
2. Tìm `columnCount()` → số cột
3. Tìm `data(index)` → cách lấy dữ liệu tại (row, column)
4. Tìm container 2D (QList<QStringList>, QVector<QVector<...>>) → nguồn dữ liệu

---

## 🌳 Tree Model {#tree-model}

**Dùng cho dữ liệu phân cấp (cây)**

### Đặc điểm:
- Có parent-child relationship
- Mỗi item có thể có nhiều children
- Ví dụ: File system, menu đa cấp, cấu trúc tổ chức...

### Ví dụ đơn giản:

```cpp
#include <QAbstractItemModel>

// Node trong tree
struct TreeNode {
    QString name;
    QList<TreeNode*> children;
    TreeNode *parent;
    
    TreeNode(const QString &n, TreeNode *p = nullptr) 
        : name(n), parent(p) {}
    
    ~TreeNode() {
        qDeleteAll(children);
    }
};

class TreeModel : public QAbstractItemModel {
    Q_OBJECT
    
private:
    TreeNode *m_root;  // Root node
    
public:
    TreeModel(QObject *parent = nullptr) 
        : QAbstractItemModel(parent) {
        m_root = new TreeNode("Root");
    }
    
    // BẮT BUỘC phải implement 5 phương thức:
    
    // 1. Số hàng (children của parent)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        TreeNode *parentNode = nodeFromIndex(parent);
        return parentNode ? parentNode->children.size() : 0;
    }
    
    // 2. Số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 1;  // Chỉ có 1 cột
    }
    
    // 3. Lấy dữ liệu
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();
            
        TreeNode *node = nodeFromIndex(index);
        if (!node)
            return QVariant();
            
        if (role == Qt::DisplayRole)
            return node->name;
            
        return QVariant();
    }
    
    // 4. Tạo index
    QModelIndex index(int row, int column, 
                      const QModelIndex &parent = QModelIndex()) const override {
        TreeNode *parentNode = nodeFromIndex(parent);
        if (!parentNode || row < 0 || row >= parentNode->children.size())
            return QModelIndex();
            
        TreeNode *childNode = parentNode->children.at(row);
        return createIndex(row, column, childNode);  // Lưu con trỏ node vào index
    }
    
    // 5. Lấy parent index
    QModelIndex parent(const QModelIndex &index) const override {
        TreeNode *node = nodeFromIndex(index);
        if (!node || node == m_root || !node->parent)
            return QModelIndex();
            
        TreeNode *parentNode = node->parent;
        TreeNode *grandParent = parentNode->parent;
        
        // Tìm vị trí của parentNode trong children của grandParent
        int row = 0;
        if (grandParent) {
            row = grandParent->children.indexOf(parentNode);
        }
        
        return createIndex(row, 0, parentNode);
    }
    
private:
    // Helper: Lấy node từ index
    TreeNode* nodeFromIndex(const QModelIndex &index) const {
        if (!index.isValid())
            return m_root;
        return static_cast<TreeNode*>(index.internalPointer());
    }
};
```

### Cách đọc code TreeModel:
1. Tìm cấu trúc Node (struct/class) → hiểu cấu trúc dữ liệu
2. Tìm `parent()` → hiểu cách đi lên cây
3. Tìm `index()` → hiểu cách đi xuống cây
4. Tìm `rowCount(parent)` → số children của một node
5. Chú ý `internalPointer()` → lưu con trỏ node trong QModelIndex

---

## 🔄 QAbstractProxyModel - Proxy Model {#proxy-model}

**Dùng để lọc, sắp xếp, chuyển đổi dữ liệu từ model khác**

### Đặc điểm:
- **KHÔNG** chứa dữ liệu thực tế
- Làm việc với một **source model** khác
- Chuyển đổi index giữa proxy và source
- Ví dụ: Lọc danh sách, sắp xếp bảng, transpose...

### Các loại Proxy Model phổ biến:

#### 1. QSortFilterProxyModel - Sắp xếp và lọc
```cpp
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

QStandardItemModel *sourceModel = new QStandardItemModel();
// ... thêm dữ liệu vào sourceModel

QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
proxyModel->setSourceModel(sourceModel);  // Kết nối với source

// Sắp xếp theo cột 0
proxyModel->sort(0, Qt::AscendingOrder);

// Lọc theo regex
proxyModel->setFilterRegExp(QRegExp(".*test.*", Qt::CaseInsensitive));

// View sử dụng proxyModel thay vì sourceModel
QTableView *view = new QTableView();
view->setModel(proxyModel);  // Dùng proxy, không dùng source
```

#### 2. QIdentityProxyModel - Pass-through (ít dùng)
```cpp
// Chỉ chuyển tiếp, không thay đổi gì
// Dùng làm base class cho custom proxy
```

#### 3. Custom Proxy Model - Ví dụ đơn giản
```cpp
class CustomProxyModel : public QAbstractProxyModel {
    Q_OBJECT
    
private:
    QAbstractItemModel *m_sourceModel;
    
public:
    CustomProxyModel(QObject *parent = nullptr) 
        : QAbstractProxyModel(parent), m_sourceModel(nullptr) {}
    
    void setSourceModel(QAbstractItemModel *sourceModel) override {
        beginResetModel();
        if (m_sourceModel)
            disconnect(m_sourceModel, nullptr, this, nullptr);
            
        m_sourceModel = sourceModel;
        
        if (m_sourceModel) {
            connect(m_sourceModel, &QAbstractItemModel::dataChanged,
                    this, &CustomProxyModel::onSourceDataChanged);
        }
        
        endResetModel();
    }
    
    // Chuyển đổi index từ proxy → source
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override {
        if (!m_sourceModel || !proxyIndex.isValid())
            return QModelIndex();
        // Ví dụ: đảo ngược hàng
        int sourceRow = m_sourceModel->rowCount() - 1 - proxyIndex.row();
        return m_sourceModel->index(sourceRow, proxyIndex.column());
    }
    
    // Chuyển đổi index từ source → proxy
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override {
        if (!m_sourceModel || !sourceIndex.isValid())
            return QModelIndex();
        // Đảo ngược lại
        int proxyRow = m_sourceModel->rowCount() - 1 - sourceIndex.row();
        return createIndex(proxyRow, sourceIndex.column());
    }
    
    // Các phương thức khác delegate cho sourceModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_sourceModel ? m_sourceModel->rowCount(mapToSource(parent)) : 0;
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        return m_sourceModel ? m_sourceModel->data(mapToSource(index), role) : QVariant();
    }
    
private slots:
    void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
    }
};
```

### Cách đọc code ProxyModel:
1. Tìm `setSourceModel()` → biết source model nào
2. Tìm `mapToSource()` → cách chuyển proxy index → source index
3. Tìm `mapFromSource()` → cách chuyển source index → proxy index
4. Các phương thức khác thường delegate cho sourceModel

---

## 🗄️ QSqlQueryModel & QSqlTableModel - SQL Models {#sql-models}

**Dùng để hiển thị dữ liệu từ database**

### QSqlQueryModel - Read-only
```cpp
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>

// Kết nối database
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("database.db");
db.open();

// Tạo model từ query
QSqlQueryModel *model = new QSqlQueryModel();
model->setQuery("SELECT name, age, address FROM users");

// Set header
model->setHeaderData(0, Qt::Horizontal, "Tên");
model->setHeaderData(1, Qt::Horizontal, "Tuổi");
model->setHeaderData(2, Qt::Horizontal, "Địa chỉ");

// Dùng với view
QTableView *view = new QTableView();
view->setModel(model);
```

### QSqlTableModel - Read-write
```cpp
#include <QSqlTableModel>

QSqlTableModel *model = new QSqlTableModel();
model->setTable("users");  // Tên bảng
model->setEditStrategy(QSqlTableModel::OnManualSubmit);  // Hoặc OnFieldChange, OnRowChange

// Lọc
model->setFilter("age > 18");

// Sắp xếp
model->setSort(1, Qt::AscendingOrder);

// Load dữ liệu
model->select();

// View có thể edit
QTableView *view = new QTableView();
view->setModel(model);
view->setEditTriggers(QAbstractItemView::DoubleClicked);

// Lưu thay đổi
model->submitAll();
```

### Mối quan hệ với các model khác:
- `QSqlQueryModel` kế thừa từ `QAbstractTableModel`
- `QSqlTableModel` kế thừa từ `QSqlQueryModel`
- Có thể dùng với `QSortFilterProxyModel` để lọc/sắp xếp

```cpp
QSqlTableModel *sourceModel = new QSqlTableModel();
sourceModel->setTable("users");
sourceModel->select();

QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
proxyModel->setSourceModel(sourceModel);
proxyModel->setFilterRegExp(QRegExp(".*admin.*"));

QTableView *view = new QTableView();
view->setModel(proxyModel);  // View → Proxy → SQL Model → Database
```

---

## 🔗 Mối Liên Hệ Giữa Các Models {#mối-liên-hệ}

### Sơ đồ phân cấp:
```
                    QAbstractItemModel (abstract base)
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
QAbstractListModel  QAbstractTableModel  QAbstractProxyModel
        │                  │                  │
        │                  │                  ├── QSortFilterProxyModel
        │                  │                  ├── QIdentityProxyModel
        │                  │                  └── QTransposeProxyModel
        │                  │
        │                  ├── QSqlQueryModel
        │                  │       │
        │                  │       └── QSqlTableModel
        │                  │
        │                  └── QStandardItemModel (có thể dùng cho cả Table và Tree)
        │
        └── QStringListModel
```

### Quan hệ sử dụng:

```
┌─────────────────────────────────────────────────────────┐
│                    VIEW (QListView, QTableView...)      │
└────────────────────┬────────────────────────────────────┘
                     │
                     │ setModel()
                     │
        ┌────────────▼────────────┐
        │   PROXY MODEL (optional)│
        │  (QSortFilterProxyModel)│
        └────────────┬────────────┘
                     │
                     │ setSourceModel()
                     │
        ┌────────────▼────────────┐
        │   SOURCE MODEL          │
        │  (List/Table/Tree/SQL)  │
        └────────────┬────────────┘
                     │
                     │ truy cập
                     │
        ┌────────────▼────────────┐
        │   DATA SOURCE           │
        │  (List/DB/File/...)     │
        └─────────────────────────┘
```

### Ví dụ chuỗi kết nối:

```cpp
// 1. Tạo source model (SQL)
QSqlTableModel *sqlModel = new QSqlTableModel();
sqlModel->setTable("products");
sqlModel->select();

// 2. Tạo proxy để lọc/sắp xếp
QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
proxyModel->setSourceModel(sqlModel);
proxyModel->setFilterRegExp(QRegExp(".*book.*", Qt::CaseInsensitive));
proxyModel->sort(1, Qt::AscendingOrder);

// 3. Kết nối với view
QTableView *view = new QTableView();
view->setModel(proxyModel);

// Luồng dữ liệu:
// Database → SQL Model → Proxy Model → View → User
```

---

## 📖 Cách Đọc Code Nhanh {#cách-đọc-code}

### Bước 1: Xác định loại Model
```cpp
// Tìm class declaration
class MyModel : public QAbstractListModel  // → List Model
class MyModel : public QAbstractTableModel // → Table Model  
class MyModel : public QAbstractItemModel  // → Tree Model
class MyModel : public QAbstractProxyModel // → Proxy Model
class MyModel : public QSqlQueryModel      // → SQL Model
```

### Bước 2: Tìm nguồn dữ liệu thực tế
```cpp
// Tìm member variables
QStringList m_data;           // → List data
QList<QStringList> m_data;    // → Table data
TreeNode *m_root;             // → Tree data
QAbstractItemModel *m_source; // → Proxy model
```

### Bước 3: Đọc các phương thức bắt buộc

#### Cho ListModel:
- ✅ `rowCount()` - Số items
- ✅ `data()` - Lấy dữ liệu
- ✅ `index()` - Tạo index

#### Cho TableModel:
- ✅ `rowCount()` - Số hàng
- ✅ `columnCount()` - Số cột
- ✅ `data()` - Lấy dữ liệu
- ✅ `index()` - Tạo index
- ✅ `headerData()` - Tên cột/hàng

#### Cho TreeModel:
- ✅ `rowCount(parent)` - Số children
- ✅ `columnCount()` - Số cột
- ✅ `data()` - Lấy dữ liệu
- ✅ `index(row, col, parent)` - Tạo index
- ✅ `parent(index)` - Lấy parent

#### Cho ProxyModel:
- ✅ `setSourceModel()` - Kết nối source
- ✅ `mapToSource()` - Proxy → Source index
- ✅ `mapFromSource()` - Source → Proxy index
- ✅ Các phương thức khác delegate cho source

### Bước 4: Tìm các phương thức thay đổi dữ liệu
```cpp
// Tìm các phương thức này để hiểu cách thêm/sửa/xóa
beginInsertRows() / endInsertRows()
beginRemoveRows() / endRemoveRows()
beginResetModel() / endResetModel()
setData()
```

### Bước 5: Tìm cách sử dụng Model
```cpp
// Tìm nơi tạo model
MyModel *model = new MyModel();

// Tìm nơi kết nối với view
view->setModel(model);

// Tìm nơi kết nối với proxy
proxy->setSourceModel(model);
```

### Checklist đọc code:

```
□ 1. Model kế thừa từ class nào?
□ 2. Dữ liệu thực tế lưu ở đâu? (member variable)
□ 3. rowCount() trả về gì?
□ 4. columnCount() trả về gì? (nếu có)
□ 5. data() lấy dữ liệu như thế nào?
□ 6. index() tạo index như thế nào?
□ 7. parent() hoạt động ra sao? (nếu là Tree)
□ 8. mapToSource/mapFromSource như thế nào? (nếu là Proxy)
□ 9. Model được kết nối với View nào?
□ 10. Có Proxy Model nào không?
```

---

## 🎓 Ví Dụ Tổng Hợp

### Ví dụ: SQL Model → Proxy → View

```cpp
#include <QApplication>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QLineEdit>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 1. Kết nối database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.open();
    
    // Tạo bảng mẫu
    QSqlQuery query;
    query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    query.exec("INSERT INTO users VALUES (1, 'Alice', 25)");
    query.exec("INSERT INTO users VALUES (2, 'Bob', 30)");
    query.exec("INSERT INTO users VALUES (3, 'Charlie', 20)");
    
    // 2. Tạo SQL Model
    QSqlTableModel *sqlModel = new QSqlTableModel();
    sqlModel->setTable("users");
    sqlModel->select();
    
    // 3. Tạo Proxy Model để lọc
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(sqlModel);
    proxyModel->setFilterKeyColumn(1);  // Lọc theo cột name
    
    // 4. Tạo View
    QTableView *view = new QTableView();
    view->setModel(proxyModel);
    
    // 5. Tạo filter input
    QLineEdit *filterEdit = new QLineEdit();
    QObject::connect(filterEdit, &QLineEdit::textChanged, [proxyModel](const QString &text) {
        proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
    });
    
    // 6. Layout
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(filterEdit);
    layout->addWidget(view);
    window.show();
    
    return app.exec();
}
```

**Luồng dữ liệu:**
```
Database (SQLite)
    ↓
QSqlTableModel (source model)
    ↓ setSourceModel()
QSortFilterProxyModel (proxy model)
    ↓ setModel()
QTableView (view)
    ↓
User sees filtered data
```

---

## 💡 Tips Học Nhanh

1. **Bắt đầu từ đơn giản**: Học ListModel trước, rồi TableModel, rồi TreeModel
2. **Hiểu QModelIndex**: Đây là "chìa khóa" để truy cập dữ liệu
3. **Nhớ 3 phương thức cốt lõi**: `rowCount()`, `data()`, `index()`
4. **Proxy Model**: Luôn nhớ có `mapToSource()` và `mapFromSource()`
5. **SQL Model**: Chỉ là TableModel đặc biệt, dữ liệu từ database
6. **Đọc code từ trên xuống**: Class declaration → Member variables → Methods
7. **Tìm pattern**: Hầu hết model đều có pattern giống nhau

---

## 📚 Tài Liệu Tham Khảo

- Qt Official Docs: https://doc.qt.io/qt-5/model-view-programming.html
- QAbstractItemModel: https://doc.qt.io/qt-5/qabstractitemmodel.html
- Model/View Tutorial: https://doc.qt.io/qt-5/modelview.html

---

**Chúc bạn học tốt! 🚀**
