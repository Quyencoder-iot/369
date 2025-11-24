# 🎨 Qt Models Complete - QML + C++

## 🎯 Tổng Quan

Project này chứa **ĐẦY ĐỦ** tất cả loại Qt Models với QML Views:

1. ✅ **ListModel** - QAbstractListModel
2. ✅ **TableModel** - QAbstractTableModel  
3. ✅ **TreeModel** - QAbstractItemModel
4. ✅ **QSqlModel** - QSqlRelationalTableModel
5. ✅ **ProxyModel** - QSortFilterProxyModel

**Tất cả Models: C++, Tất cả Views: QML!**

---

## 📁 Cấu Trúc

```
qml_complete/
├── models/                          # ← C++ MODELS
│   ├── TodoListModel.h/cpp         # Example 1: List Model
│   ├── StudentTableModel.h/cpp     # Example 2: Table Model
│   ├── FileSystemTreeModel.h/cpp   # Example 3: Tree Model
│   ├── EmployeeSqlModel.h/cpp      # Example 4: SQL Model
│   └── ProductFilterProxyModel.h/cpp # Example 5: Proxy Model
│
├── qml/                             # ← QML VIEWS
│   ├── TodoListView.qml            # List View
│   ├── StudentTableView.qml        # Table View
│   ├── FileTreeView.qml            # Tree View
│   ├── EmployeeSqlView.qml         # SQL View
│   └── ProductFilterView.qml       # Proxy View
│
├── main_01_list.cpp                 # Entry point Example 1
├── main_02_table.cpp                # Entry point Example 2
├── main_03_tree.cpp                 # Entry point Example 3
├── main_04_sql.cpp                  # Entry point Example 4
├── main_05_proxy.cpp                # Entry point Example 5
├── qml.qrc                          # Resource file
└── CMakeLists.txt                   # Build config
```

---

## 🚀 Build & Run

### Build All:
```bash
cd /workspace/qml_complete
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Run Examples:
```bash
# Example 1: List Model - Todo List
./qml_01_list

# Example 2: Table Model - Student Table
./qml_02_table

# Example 3: Tree Model - File System Tree
./qml_03_tree

# Example 4: SQL Model - Employee Database
./qml_04_sql

# Example 5: Proxy Model - Product Filter
./qml_05_proxy
```

---

## 📖 Chi Tiết Từng Example

### Example 1: List Model (TodoListModel)

**C++ Model:**
- `QAbstractListModel`
- Roles: text, completed, priority, createdDate
- Methods: addTodo(), removeTodo(), toggleCompleted()

**QML View:**
- ListView với delegates
- Add/Edit/Delete operations
- Filter by completed status
- Statistics display

**Key Concepts:**
- `roleNames()` - Expose roles sang QML
- `Q_INVOKABLE` - Call C++ methods từ QML
- `emit dataChanged()` - Update UI

---

### Example 2: Table Model (StudentTableModel)

**C++ Model:**
- `QAbstractTableModel`
- Columns: ID, Name, Age, Grade, Email, Phone
- Methods: addStudent(), removeStudent(), sort()

**QML View:**
- TableView với columns
- Search/Filter
- Inline editing
- Statistics (Grade A count, Average age)

**Key Concepts:**
- `rowCount()` và `columnCount()`
- `headerData()` - Column headers
- Custom delegates for editing

---

### Example 3: Tree Model (FileSystemTreeModel)

**C++ Model:**
- `QAbstractItemModel`
- Hierarchical structure (folders/files)
- Methods: index(), parent(), addFolder(), addFile()

**QML View:**
- TreeView với expand/collapse
- Folder/File icons
- Size display
- Add/Remove nodes

**Key Concepts:**
- `index()` - Create index với internalPointer
- `parent()` - Find parent index
- TreeNode structure

---

### Example 4: SQL Model (EmployeeSqlModel)

**C++ Model:**
- `QSqlRelationalTableModel`
- Foreign key: employees.department_id → departments.name
- Methods: addEmployee(), submitChanges(), filterByDepartment()

**QML View:**
- TableView từ database
- CRUD operations
- Department filter
- Statistics (Average salary)

**Key Concepts:**
- `QSqlRelation()` - Foreign keys
- `submitAll()` / `revertAll()`
- Database connection

---

### Example 5: Proxy Model (ProductFilterProxyModel)

**C++ Model:**
- `QSortFilterProxyModel`
- Source: ProductModel
- Filters: search, category, price, rating, stock

**QML View:**
- ListView with filtering
- Multiple filter controls
- Sort by price
- Statistics (Average price, In stock count)

**Key Concepts:**
- `filterAcceptsRow()` - Filter logic
- `lessThan()` - Sort logic
- `setSourceModel()` - Connect to source

---

## 🔑 Key Techniques

### 1. Expose Model sang QML

```cpp
// main.cpp
QQmlApplicationEngine engine;
TodoListModel todoModel;
engine.rootContext()->setContextProperty("todoModel", &todoModel);
engine.load(QUrl("qrc:/qml/TodoListView.qml"));
```

```qml
// TodoListView.qml
ListView {
    model: todoModel  // Access C++ model
}
```

### 2. Custom Roles

```cpp
QHash<int, QByteArray> roleNames() const override {
    return {
        {TextRole, "text"},
        {CompletedRole, "completed"}
    };
}
```

```qml
delegate: Text {
    text: model.text         // Access TextRole
    opacity: model.completed ? 0.5 : 1.0
}
```

### 3. Call C++ Methods

```cpp
Q_INVOKABLE void addTodo(const QString &text, int priority);
```

```qml
Button {
    onClicked: todoModel.addTodo("New task", 1)
}
```

### 4. Properties & Signals

```cpp
Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
```

```qml
Label {
    text: "Total: " + todoModel.count
    // Auto-updates when countChanged emitted
}
```

---

## 📊 Comparison Table

| Model Type | When to Use | Complexity | Example |
|------------|-------------|------------|---------|
| **ListModel** | 1D data | ⭐ Easy | Todo list, Music playlist |
| **TableModel** | 2D data | ⭐⭐ Medium | Spreadsheet, Student grades |
| **TreeModel** | Hierarchical | ⭐⭐⭐ Hard | File system, Org chart |
| **SqlModel** | Database | ⭐⭐ Medium | Employee DB, Inventory |
| **ProxyModel** | Filter/Sort | ⭐⭐ Medium | Search results, Filtered list |

---

## 🛠️ Development Workflow

### 1. Create C++ Model:
```cpp
class MyModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void addItem(const QString &text);
};
```

### 2. Expose to QML:
```cpp
MyModel myModel;
engine.rootContext()->setContextProperty("myModel", &myModel);
```

### 3. Create QML View:
```qml
ListView {
    model: myModel
    delegate: Text { text: model.text }
}
Button {
    onClicked: myModel.addItem("New")
}
```

---

## 🎓 Learning Path

```
1. Start: ListModel (Easiest)
   └─ Understand: roleNames(), Q_INVOKABLE
   
2. Next: TableModel
   └─ Learn: rowCount(), columnCount()
   
3. Then: ProxyModel
   └─ Master: filterAcceptsRow()
   
4. After: SqlModel
   └─ Understand: Database integration
   
5. Final: TreeModel (Most Complex)
   └─ Master: index(), parent(), hierarchies
```

---

## 💡 Tips & Tricks

### Hot Reload QML:
```bash
# Edit QML files without rebuilding C++
qmlscene qml/TodoListView.qml
```

### Debug:
```qml
console.log("Value:", model.text)
```

### Performance:
- Use `QAbstractListModel` for lists (faster than TableModel)
- Implement `canFetchMore()` for lazy loading
- Use Proxy for filtering (don't modify source)

---

## 🐛 Common Issues

### Issue 1: "model.xxx is undefined"

**Cause:** Role name not in `roleNames()`

**Fix:**
```cpp
QHash<int, QByteArray> roleNames() const override {
    return {{TextRole, "text"}};  // Add "text" role
}
```

### Issue 2: Model không update

**Cause:** Forgot emit signals

**Fix:**
```cpp
void addItem() {
    beginInsertRows(...);
    m_items.append(...);
    endInsertRows();  // This emits signals!
}
```

### Issue 3: Tree model không expand

**Cause:** Wrong `parent()` implementation

**Fix:** Check `parent()` returns correct QModelIndex

---

## 📚 Resources

- [Qt QML](https://doc.qt.io/qt-5/qtqml-index.html)
- [Models for QML](https://doc.qt.io/qt-5/qtquick-modelviewsdata-cppmodels.html)
- [QAbstractItemModel](https://doc.qt.io/qt-5/qabstractitemmodel.html)

---

## ✅ Checklist

- [x] 5 C++ Models implemented
- [x] 5 QML Views created
- [x] Q_PROPERTY for all models
- [x] Q_INVOKABLE methods
- [x] roleNames() override
- [x] Signals for updates
- [x] CMakeLists.txt
- [x] Documentation

---

**Start learning with Example 1 (List Model) - Easiest!** 📝

**Then progress through examples 2-5.** 🚀

**Master all Qt Model types!** 🎓
