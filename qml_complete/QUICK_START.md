# 🚀 Quick Start Guide

## 📦 Build trong 3 bước

```bash
# 1. Tạo build directory
mkdir build && cd build

# 2. Generate build files
cmake ..

# 3. Build tất cả examples
make
```

## ▶️ Chạy từng example

```bash
# Example 1: Todo List
./example_01_todo

# Example 2: Student Table
./example_02_student

# Example 3: File System Tree
./example_03_tree

# Example 4: Employee SQL Database
./example_04_sql

# Example 5: Product Filter Proxy
./example_05_proxy
```

## 🎯 Học theo thứ tự

### 1. Bắt đầu với Todo List (Dễ nhất)
```bash
./example_01_todo
```

**Đọc files:**
1. `models/TodoListModel.h` - Xem cấu trúc
2. `models/TodoListModel.cpp` - Xem implementation
3. `qml/TodoListView.qml` - Xem UI
4. `main_01_todo.cpp` - Xem integration

**Key Concepts:**
- QAbstractListModel (1D list)
- roleNames() mapping
- Q_INVOKABLE methods
- Q_PROPERTY binding
- ListView trong QML

### 2. Tiếp theo: Student Table
```bash
./example_02_student
```

**Khác biệt so với List:**
- ✅ 2D data (rows × columns)
- ✅ columnCount() method
- ✅ TableView thay vì ListView
- ✅ HorizontalHeaderView

### 3. Nâng cao: File System Tree
```bash
./example_03_tree
```

**Khác biệt so với Table:**
- ✅ Hierarchical data (parent-child)
- ✅ QModelIndex concept
- ✅ parent() và index() methods
- ✅ Recursive structure

### 4. Database: Employee SQL
```bash
./example_04_sql
```

**Khác biệt:**
- ✅ QSqlTableModel (database mapping)
- ✅ Foreign key relationships
- ✅ Transaction support (submit/revert)
- ✅ Direct table editing

### 5. Advanced: Product Filter Proxy
```bash
./example_05_proxy
```

**Khác biệt:**
- ✅ Proxy model pattern
- ✅ Filter without modifying source
- ✅ Multiple filter criteria
- ✅ Dynamic sorting

## 📖 Đọc code như thế nào?

### Cho mỗi example, đọc theo thứ tự:

1. **Model Header (.h)**
   - Xem class structure
   - Xem Q_PROPERTY
   - Xem Q_INVOKABLE methods
   - Xem custom roles

2. **Model Implementation (.cpp)**
   - Xem data storage (QList, QVector, etc.)
   - Xem data() method
   - Xem CRUD operations
   - ⭐ **Đọc SUMMARY ở cuối file!**

3. **QML View (.qml)**
   - Xem UI structure
   - Xem model binding
   - Xem delegate template
   - Xem method calls
   - ⭐ **Đọc SUMMARY ở cuối file!**

4. **Main file (.cpp)**
   - Xem model setup
   - Xem setContextProperty
   - Xem sample data
   - ⭐ **Đọc SUMMARY ở cuối file!**

## 💡 Debugging Tips

### QML không hiển thị data?

**Check 1:** Model có được expose không?
```cpp
// main.cpp
engine.rootContext()->setContextProperty("todoModel", &model);
```

**Check 2:** roleNames() có đúng không?
```cpp
// Model.cpp
QHash<int, QByteArray> roleNames() const override {
    return {{TextRole, "text"}};  // "text" matches QML
}
```

**Check 3:** QML có dùng đúng tên không?
```qml
// View.qml
ListView {
    model: todoModel  // Matches setContextProperty name
    delegate: Text { text: model.text }  // Matches roleNames
}
```

### Property không update?

**Check:** Q_PROPERTY có NOTIFY không?
```cpp
Q_PROPERTY(int count READ count NOTIFY countChanged)
signals:
    void countChanged();  // MUST emit this!
```

### Method không work trong QML?

**Check:** Method có Q_INVOKABLE không?
```cpp
Q_INVOKABLE void addTodo(const QString &text, int priority);
```

## 🔍 Code Navigation

### Tìm implementation của một method:

1. **Trong Model Header:** Khai báo method
2. **Trong Model Implementation:** Implementation
3. **Trong QML:** Usage

Example: `addTodo()`
```cpp
// TodoListModel.h
Q_INVOKABLE void addTodo(const QString &text, int priority);

// TodoListModel.cpp
void TodoListModel::addTodo(const QString &text, int priority) {
    beginInsertRows(QModelIndex(), m_todos.count(), m_todos.count());
    m_todos.append({text, false, priority, QDate::currentDate().toString()});
    endInsertRows();
    emit countChanged();
}

// TodoListView.qml
Button {
    onClicked: todoModel.addTodo(inputField.text, priorityValue)
}
```

## 📊 Data Flow Diagram

```
User clicks button in QML
        ↓
QML calls model.method()  (Q_INVOKABLE)
        ↓
C++ method modifies data
        ↓
C++ calls beginInsertRows/endInsertRows
        ↓
C++ emits signal (e.g., countChanged)
        ↓
QML property binding updates (e.g., model.count)
        ↓
QML ListView calls data() for each item
        ↓
C++ returns data via roles
        ↓
QML delegate displays data
        ↓
UI updated!
```

## 🎨 Customization Examples

### Thêm role mới

```cpp
// 1. Add to enum (Model.h)
enum Roles {
    TextRole = Qt::UserRole + 1,
    NewRole = Qt::UserRole + 2  // Add here
};

// 2. Add to roleNames() (Model.cpp)
{NewRole, "newField"}

// 3. Add to data() (Model.cpp)
case NewRole:
    return m_data.at(index.row()).newField;

// 4. Use in QML (View.qml)
Text { text: model.newField }
```

### Thêm Q_PROPERTY mới

```cpp
// 1. Declare (Model.h)
Q_PROPERTY(int newProperty READ newProperty NOTIFY newPropertyChanged)

// 2. Getter (Model.h)
int newProperty() const { return m_newProperty; }

// 3. Signal (Model.h)
signals:
    void newPropertyChanged();

// 4. Emit when changed (Model.cpp)
emit newPropertyChanged();

// 5. Use in QML (View.qml)
Label { text: "Value: " + model.newProperty }
```

## 🚨 Common Mistakes

### ❌ Forget to call begin/end methods
```cpp
// WRONG
m_data.append(item);
emit dataChanged(...);

// CORRECT
beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
m_data.append(item);
endInsertRows();
```

### ❌ Forget to emit signals
```cpp
// WRONG
void setCount(int count) {
    m_count = count;
}

// CORRECT
void setCount(int count) {
    if (m_count != count) {
        m_count = count;
        emit countChanged();  // Notify QML!
    }
}
```

### ❌ Wrong parent QModelIndex
```cpp
// WRONG (for list models)
beginInsertRows(someInvalidIndex, row, row);

// CORRECT
beginInsertRows(QModelIndex(), row, row);  // Empty = root
```

### ❌ Not checking indices
```cpp
// WRONG
QVariant data(const QModelIndex &index, int role) const {
    return m_data.at(index.row()).field;  // Crash if invalid!
}

// CORRECT
QVariant data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_data.size())
        return QVariant();
    return m_data.at(index.row()).field;
}
```

## 📚 Additional Help

### Đọc comments trong code!

Mỗi file có:
- ✅ Header comments (giải thích mục đích)
- ✅ Inline comments (giải thích logic)
- ✅ **⭐ SUMMARY section ở cuối (TÓM TẮT QUAN TRỌNG!)**

### Summary sections chứa:

1. **Key concepts** - Khái niệm quan trọng
2. **Data flow** - Luồng dữ liệu
3. **Roles mapping** - Roles → QML names
4. **Methods called** - C++ methods từ QML
5. **Properties used** - C++ properties trong QML
6. **Learning points** - Điểm cần nhớ

## ✅ Checklist để hiểu một example

- [ ] Build và run example
- [ ] Chơi với UI, test features
- [ ] Đọc Model Header (.h)
- [ ] Đọc Model Implementation (.cpp)
- [ ] **Đọc SUMMARY ở cuối .cpp**
- [ ] Đọc QML View (.qml)
- [ ] **Đọc SUMMARY ở cuối .qml**
- [ ] Đọc Main file (.cpp)
- [ ] **Đọc SUMMARY ở cuối main.cpp**
- [ ] Thử modify code
- [ ] Rebuild và test changes

## 🎯 Goals

Sau khi hoàn thành 5 examples, bạn sẽ hiểu:

- ✅ Qt Model/View architecture
- ✅ QAbstractListModel (1D)
- ✅ QAbstractTableModel (2D)
- ✅ QAbstractItemModel (Tree)
- ✅ QSqlTableModel (Database)
- ✅ QSortFilterProxyModel (Filter/Sort)
- ✅ C++ ↔ QML integration
- ✅ roleNames() mapping
- ✅ Q_PROPERTY và Q_INVOKABLE
- ✅ Signal/Slot mechanism
- ✅ ListView, TableView trong QML
- ✅ Property binding trong QML

---

**🎉 Bắt đầu với Example 1:**
```bash
./example_01_todo
```

**Chúc bạn học tốt! 🚀**
