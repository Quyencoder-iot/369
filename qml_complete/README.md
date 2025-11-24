# 🎓 Qt Models with QML - Complete Examples

## 📋 Tổng quan

Bộ ví dụ **HOÀN CHỈNH** về Qt Model/View với QML, bao gồm **TẤT CẢ** các loại model:

1. ✅ **List Model** - Todo List
2. ✅ **Table Model** - Student Table
3. ✅ **Tree Model** - File System
4. ✅ **SQL Model** - Employee Database
5. ✅ **Proxy Model** - Product Filter

## 🏗️ Cấu trúc dự án

```
qml_complete/
├── README.md                          (Bạn đang đọc đây!)
├── CMakeLists.txt                     (Build configuration)
├── qml.qrc                            (QML resources)
│
├── main_01_todo.cpp                   (Todo List app entry)
├── main_02_student.cpp                (Student Table app entry)
├── main_03_tree.cpp                   (File System app entry)
├── main_04_sql.cpp                    (Employee SQL app entry)
├── main_05_proxy.cpp                  (Product Filter app entry)
│
├── models/                            (C++ Models)
│   ├── TodoListModel.h/cpp           (QAbstractListModel)
│   ├── StudentTableModel.h/cpp       (QAbstractTableModel)
│   ├── FileSystemTreeModel.h/cpp     (QAbstractItemModel)
│   ├── EmployeeSqlModel.h/cpp        (QSqlRelationalTableModel wrapper)
│   └── ProductFilterProxyModel.h/cpp (QSortFilterProxyModel)
│
└── qml/                               (QML Views)
    ├── TodoListView.qml              (Todo List UI)
    ├── StudentTableView.qml          (Student Table UI)
    ├── FileSystemTreeView.qml        (File System Tree UI)
    ├── EmployeeSqlView.qml           (Employee Database UI)
    └── ProductFilterProxyView.qml    (Product Filter UI)
```

## 🚀 Build & Run

### Prerequisites

- Qt5 (5.12+) hoặc Qt6 (6.0+)
- CMake 3.5+
- C++11 compiler

### Build Instructions

```bash
cd qml_complete
mkdir build && cd build
cmake ..
make
```

### Run Examples

```bash
# Example 1: Todo List
./example_01_todo

# Example 2: Student Table
./example_02_student

# Example 3: File System Tree
./example_03_tree

# Example 4: Employee SQL
./example_04_sql

# Example 5: Product Filter Proxy
./example_05_proxy
```

## 📚 Chi tiết từng Example

### 1️⃣ Example 1: Todo List (QAbstractListModel)

**Model:** `TodoListModel`
**View:** `TodoListView.qml`

**Học được gì:**
- ✅ QAbstractListModel cơ bản
- ✅ Custom roles (text, completed, priority)
- ✅ CRUD operations (add, remove, update)
- ✅ Q_PROPERTY cho property binding
- ✅ Q_INVOKABLE methods
- ✅ ListView trong QML

**Key Features:**
- Add/Remove todos
- Toggle completed status
- Priority levels (High/Medium/Low)
- Statistics (total, completed, incomplete)
- Filter by status

### 2️⃣ Example 2: Student Table (QAbstractTableModel)

**Model:** `StudentTableModel`
**View:** `StudentTableView.qml`

**Học được gì:**
- ✅ QAbstractTableModel (2D data)
- ✅ rowCount() AND columnCount()
- ✅ Multiple columns (6 columns)
- ✅ Editable cells
- ✅ TableView trong QML
- ✅ HorizontalHeaderView

**Key Features:**
- Add/Edit/Delete students
- 6 columns: ID, Name, Age, Grade, Email, Phone
- Search functionality
- Statistics (Grade A count, Average Age)
- Column headers

### 3️⃣ Example 3: File System Tree (QAbstractItemModel)

**Model:** `FileSystemTreeModel`
**View:** `FileSystemTreeView.qml`

**Học được gì:**
- ✅ QAbstractItemModel (hierarchical data)
- ✅ QModelIndex concept
- ✅ parent() và index() methods
- ✅ Recursive structure
- ✅ Expand/Collapse state
- ✅ Tree navigation

**Key Features:**
- Hierarchical file/folder structure
- Expand/Collapse folders
- Add/Remove files and folders
- Show file size and modified date
- Depth-based indentation
- Statistics (total, folders, files)

### 4️⃣ Example 4: Employee SQL (QSqlRelationalTableModel)

**Model:** `EmployeeSqlModel`
**View:** `EmployeeSqlView.qml`

**Học được gì:**
- ✅ QSqlTableModel (database mapping)
- ✅ QSqlRelationalTableModel (foreign keys)
- ✅ Direct table editing
- ✅ Transaction support (submit/revert)
- ✅ Foreign key relationships
- ✅ In-memory SQLite database

**Key Features:**
- Direct database CRUD
- Foreign key to departments table
- Editable cells with auto-save
- Submit/Revert changes
- Statistics (average salary, count)
- Department filter

### 5️⃣ Example 5: Product Filter Proxy (QSortFilterProxyModel)

**Model:** `ProductFilterProxyModel` (wraps `ProductModel`)
**View:** `ProductFilterProxyView.qml`

**Học được gì:**
- ✅ QSortFilterProxyModel concept
- ✅ Source model vs Proxy model
- ✅ filterAcceptsRow() override
- ✅ lessThan() for sorting
- ✅ Multiple filter criteria
- ✅ Dynamic filtering

**Key Features:**
- Filter by category, price range, stock
- Search by product name
- Sort by name, price, stock
- Statistics (source vs filtered)
- Real-time filter updates
- Source model unchanged

## 🎯 Concepts Covered

### C++ Model Concepts

1. **QAbstractListModel** (1D list)
   - rowCount()
   - data(index, role)
   - roleNames()

2. **QAbstractTableModel** (2D table)
   - rowCount() + columnCount()
   - data(index, role) with column logic

3. **QAbstractItemModel** (Tree/Hierarchical)
   - parent(child)
   - index(row, col, parent)
   - QModelIndex for navigation

4. **QSqlTableModel** (Database)
   - Direct table mapping
   - setData() for editing
   - submit()/revert() for transactions

5. **QSortFilterProxyModel** (Filter/Sort)
   - setSourceModel()
   - filterAcceptsRow()
   - lessThan()

### QML Integration

1. **setContextProperty**
   ```cpp
   engine.rootContext()->setContextProperty("modelName", &modelObject);
   ```

2. **Access from QML**
   ```qml
   ListView {
       model: modelName
       delegate: Text { text: model.roleName }
   }
   ```

3. **Call C++ methods**
   ```qml
   Button {
       onClicked: modelName.methodName(params)
   }
   ```

4. **Property binding**
   ```qml
   Label {
       text: "Count: " + modelName.propertyName
   }
   ```

### Qt Meta-Object System

- **Q_OBJECT** - Enable meta-object features
- **Q_PROPERTY** - Expose properties to QML
- **Q_INVOKABLE** - Make methods callable from QML
- **signals** - Emit notifications
- **slots** - Receive notifications

## 📖 Code Comments

Mỗi file đều có:
- ✅ **Header comments** - Giải thích mục đích
- ✅ **Inline comments** - Giải thích từng đoạn code
- ✅ **Summary section** - Tổng kết ở cuối file
- ✅ **Key concepts** - Những điểm quan trọng
- ✅ **Data flow diagrams** - Sơ đồ luồng dữ liệu

## 🎓 Learning Path

### Beginner (Tuần 1-2)

1. Start with **Example 1 (Todo List)**
   - Hiểu QAbstractListModel
   - Hiểu roleNames()
   - Hiểu ListView trong QML

2. Move to **Example 2 (Student Table)**
   - Hiểu QAbstractTableModel
   - Hiểu columnCount()
   - Hiểu TableView trong QML

### Intermediate (Tuần 3)

3. Study **Example 3 (File System Tree)**
   - Hiểu QAbstractItemModel
   - Hiểu QModelIndex
   - Hiểu recursive structures

4. Learn **Example 4 (Employee SQL)**
   - Hiểu QSqlTableModel
   - Hiểu foreign keys
   - Hiểu transactions

### Advanced (Tuần 4)

5. Master **Example 5 (Product Filter Proxy)**
   - Hiểu proxy pattern
   - Hiểu filtering logic
   - Hiểu source vs proxy

## 🔧 Customization

### Thêm roles mới

```cpp
// In model header
enum Roles {
    ExistingRole = Qt::UserRole + 1,
    NewRole = Qt::UserRole + 2  // Add here
};

// In roleNames()
QHash<int, QByteArray> roleNames() const override {
    return {
        {ExistingRole, "existing"},
        {NewRole, "newRole"}  // Add here
    };
}

// In data()
case NewRole:
    return m_data.at(index.row()).newField;
```

### Thêm methods mới

```cpp
// In model header
Q_INVOKABLE void newMethod(int param);

// In model implementation
void Model::newMethod(int param) {
    // Implementation
    emit someSignal();  // Notify QML
}

// In QML
Button {
    onClicked: modelName.newMethod(value)
}
```

## 🐛 Common Issues

### 1. QML không thấy C++ model

**Fix:** Check setContextProperty:
```cpp
engine.rootContext()->setContextProperty("modelName", &modelObject);
```

### 2. QML property không update

**Fix:** Check Q_PROPERTY NOTIFY:
```cpp
Q_PROPERTY(int count READ count NOTIFY countChanged)
signals:
    void countChanged();
```

### 3. Roles không work trong QML

**Fix:** Check roleNames() override:
```cpp
QHash<int, QByteArray> roleNames() const override {
    return {{TextRole, "text"}};
}
```

### 4. beginInsertRows crash

**Fix:** Check parent QModelIndex:
```cpp
beginInsertRows(QModelIndex(), row, row);  // For list/table
// NOT beginInsertRows(invalidIndex, ...)
```

## 📝 Best Practices

1. ✅ **Always emit signals** when data changes
2. ✅ **Always call begin/end methods** for insert/remove
3. ✅ **Use Q_PROPERTY** for QML-accessible properties
4. ✅ **Use Q_INVOKABLE** for QML-callable methods
5. ✅ **Override roleNames()** for QML role access
6. ✅ **Use const references** for parameters
7. ✅ **Check indices** before accessing data
8. ✅ **Document your code** with comments

## 🚀 Next Steps

Sau khi học xong 5 examples này, bạn có thể:

1. ✅ Tạo custom models cho app của bạn
2. ✅ Integrate với databases (SQLite, PostgreSQL, etc.)
3. ✅ Tạo complex filters với proxy models
4. ✅ Build production-ready QML apps
5. ✅ Understand Qt Model/View architecture deeply

## 📚 Additional Resources

- [Qt Model/View Documentation](https://doc.qt.io/qt-6/model-view-programming.html)
- [QML Documentation](https://doc.qt.io/qt-6/qmlapplications.html)
- [Qt Examples](https://doc.qt.io/qt-6/qtexamples.html)

## 📧 Contact & Feedback

Nếu có câu hỏi hoặc feedback, vui lòng:
- Đọc lại comments trong code
- Check summary sections ở cuối mỗi file
- Review data flow diagrams

## ⚖️ License

This project is for educational purposes.

---

**🎉 Chúc bạn học tốt Qt Models & QML!**

Made with ❤️ for Qt learners
