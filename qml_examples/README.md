# 🎨 Qt Models với QML Views

## 📖 Tổng Quan

Project này minh họa kiến trúc **Model (C++) + View (QML)**:
- ✅ **Models**: Viết bằng C++ (logic, data management)
- ✅ **Views**: Viết bằng QML (UI, declarative)
- ✅ **Separation**: Tách biệt hoàn toàn business logic và presentation

---

## 🏗️ Kiến Trúc

```
┌─────────────────────────────────────────────┐
│         QML VIEW LAYER                      │
│   (TodoListView.qml / StudentTableView.qml) │
│   - UI/UX                                   │
│   - User interactions                       │
│   - Visual effects                          │
└──────────────┬──────────────────────────────┘
               │
               │ Q_PROPERTY, Q_INVOKABLE
               │ signals/slots
               │
┌──────────────▼──────────────────────────────┐
│         C++ MODEL LAYER                     │
│   (TodoListModel / StudentTableModel)       │
│   - Data storage                            │
│   - Business logic                          │
│   - Model operations                        │
└─────────────────────────────────────────────┘
```

---

## 📦 Yêu Cầu

- **Qt 5.12+** hoặc **Qt 6.x**
- **Qt Quick** module
- CMake 3.14+
- C++11 compiler

### Cài Đặt Qt Quick:

**Ubuntu/Debian:**
```bash
# Qt5
sudo apt install qtdeclarative5-dev qml-module-qtquick-controls2

# Qt6
sudo apt install qt6-declarative-dev qml6-module-qtquick-controls
```

**macOS:**
```bash
# Qt đã include Quick module
brew install qt@5
# hoặc
brew install qt@6
```

---

## 🚀 Build & Run

### Build:
```bash
cd qml_examples
mkdir build && cd build

# Auto-detect Qt version
cmake ..
make

# Hoặc chỉ định Qt path
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64
make
```

### Run Examples:

```bash
# Example 1: Todo List
./qml_todo_list

# Example 2: Student Table
./qml_student_table
```

---

## 📂 Cấu Trúc Project

```
qml_examples/
├── models/                     # C++ Models
│   ├── TodoListModel.h
│   ├── TodoListModel.cpp
│   ├── StudentTableModel.h
│   └── StudentTableModel.cpp
│
├── qml/                        # QML Views
│   ├── TodoListView.qml
│   └── StudentTableView.qml
│
├── main_todo.cpp               # Todo app entry point
├── main_student.cpp            # Student app entry point
├── qml.qrc                     # Qt Resource file
├── CMakeLists.txt              # Build config
└── README.md                   # This file
```

---

## 🎯 Examples Chi Tiết

### Example 1: Todo List

**C++ Model:** `TodoListModel`
- Quản lý danh sách todos
- Properties: text, completed, priority
- Methods: addTodo(), removeTodo(), toggleCompleted()

**QML View:** `TodoListView.qml`
- Modern Material Design UI
- Filter by completed status
- Add/Remove/Toggle todos
- Statistics display

**Features:**
- ✅ Add new todos with priority
- ✅ Mark as completed
- ✅ Delete todos
- ✅ Filter: All / Incomplete / Completed
- ✅ Statistics: Total, Completed, Remaining

**Screenshot:**
```
┌──────────────────────────────────┐
│ 📝 Todo List Application         │
│ Total: 4 items                   │
├──────────────────────────────────┤
│ [Input field] [Priority] [Add]  │
├──────────────────────────────────┤
│ [📋 All] [⏳ Pending] [✅ Done]  │
├──────────────────────────────────┤
│ ☐ Học Qt Models        [HIGH] 🗑️│
│ ☐ Học QML              [HIGH] 🗑️│
│ ☑️ Tạo ứng dụng         [MED] 🗑️│
│ ☐ Deploy app           [LOW] 🗑️│
├──────────────────────────────────┤
│ 📊 Total: 4 | ✅ Done: 1 | ⏳: 3│
└──────────────────────────────────┘
```

### Example 2: Student Table

**C++ Model:** `StudentTableModel`
- Quản lý bảng sinh viên
- Columns: ID, Name, Age, Grade, Email
- Methods: addStudent(), removeStudent(), get(), set()

**QML View:** `StudentTableView.qml`
- TableView với columns
- Search/Filter functionality
- Add/Edit/Delete operations
- Statistics

**Features:**
- ✅ Add new students
- ✅ Edit student info
- ✅ Delete students
- ✅ Search by name or email
- ✅ Statistics: Grade distribution, Average age

**Screenshot:**
```
┌────────────────────────────────────────────┐
│ 👨‍🎓 Student Management                     │
│ Total students: 5                          │
├────────────────────────────────────────────┤
│ ➕ Add: [Name] [Age] [Grade] [Email] [Add]│
├────────────────────────────────────────────┤
│ 🔍 Search: [____________]                 │
├────────────────────────────────────────────┤
│ ID │ Tên          │ Tuổi │ Điểm │ Email   │
│ 1  │ Nguyễn Văn A │ 20   │ A    │ a@...  │
│ 2  │ Trần Thị B   │ 19   │ B+   │ b@...  │
│ 3  │ Lê Văn C     │ 21   │ A+   │ c@...  │
├────────────────────────────────────────────┤
│ 📊 Grade A: 2 | Average Age: 20.0         │
└────────────────────────────────────────────┘
```

---

## 🔑 Key Concepts

### 1. Expose C++ Model sang QML

**Trong C++ (Model):**
```cpp
class TodoListModel : public QAbstractListModel {
    Q_OBJECT
    
    // Property cho QML
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    // Methods cho QML
    Q_INVOKABLE void addTodo(const QString &text, int priority);
    Q_INVOKABLE void removeTodo(int index);
    
    // Custom roles
    enum TodoRoles {
        TextRole = Qt::UserRole + 1,
        CompletedRole,
        PriorityRole
    };
    
    // PHẢI override roleNames()
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TextRole] = "text";
        roles[CompletedRole] = "completed";
        roles[PriorityRole] = "priority";
        return roles;
    }
    
signals:
    void countChanged();
};
```

**Trong main.cpp:**
```cpp
QQmlApplicationEngine engine;

// Set context property
engine.rootContext()->setContextProperty("todoModel", &todoModel);

engine.load(QStringLiteral("qrc:/qml/TodoListView.qml"));
```

**Trong QML:**
```qml
ListView {
    model: todoModel  // Access C++ model
    
    delegate: Rectangle {
        // Access custom roles
        Text { text: model.text }
        CheckBox { checked: model.completed }
    }
}

Button {
    onClicked: {
        // Call C++ method
        todoModel.addTodo("New task", 1)
    }
}
```

### 2. Custom Roles

**Tại sao cần?**
- QML ListView/TableView cần access data qua role names
- Không thể dùng index.column() trong QML ListView

**Cách implement:**
```cpp
// 1. Define roles
enum Roles {
    TextRole = Qt::UserRole + 1,
    CompletedRole
};

// 2. Override roleNames()
QHash<int, QByteArray> roleNames() const override {
    return {
        {TextRole, "text"},
        {CompletedRole, "completed"}
    };
}

// 3. Return data by role
QVariant data(const QModelIndex &index, int role) const override {
    switch (role) {
        case TextRole: return m_todos[index.row()].text;
        case CompletedRole: return m_todos[index.row()].completed;
    }
}
```

### 3. Q_INVOKABLE Methods

Cho phép QML gọi C++ methods:

```cpp
// C++
Q_INVOKABLE void addTodo(const QString &text, int priority);

// QML
Button {
    onClicked: todoModel.addTodo("Task", 1)
}
```

### 4. Signals từ C++ → QML

```cpp
// C++
signals:
    void countChanged();

// Emit signal
emit countChanged();

// QML tự động update khi signal emit
Label {
    text: "Count: " + todoModel.count
    // Tự động update khi countChanged() emitted
}
```

---

## 🔄 Data Flow

```
User Action (QML)
    │
    ▼
QML calls C++ method
(todoModel.addTodo(...))
    │
    ▼
C++ Model updates data
(beginInsertRows, append, endInsertRows)
    │
    ▼
Model emits signals
(countChanged, dataChanged)
    │
    ▼
QML automatically updates
(ListView re-renders)
```

---

## 📚 So Sánh: QML vs Qt Widgets

| Aspect | Qt Widgets (C++) | QML |
|--------|------------------|-----|
| **Language** | C++ | QML (JavaScript-like) |
| **UI Creation** | Code-based | Declarative |
| **Performance** | Fast | Fast (hardware accelerated) |
| **Animations** | Manual | Built-in, easy |
| **Learning Curve** | Medium | Easy |
| **Mobile Support** | Limited | Excellent |
| **Desktop Support** | Excellent | Good |
| **Hot Reload** | No | Yes (qmlscene) |

**Khi nào dùng QML:**
- ✅ Mobile apps
- ✅ Modern, animated UIs
- ✅ Rapid prototyping
- ✅ Touch interfaces

**Khi nào dùng Qt Widgets:**
- ✅ Desktop apps (traditional)
- ✅ Complex forms
- ✅ Existing codebase
- ✅ Mature widgets

---

## 🛠️ Development Tips

### Hot Reload với qmlscene:

```bash
# Run QML directly without C++ rebuild
qmlscene qml/TodoListView.qml
```

### Debug QML:

```qml
// Console output
console.log("Debug:", model.text)

// Debugger
debugger  // Breakpoint
```

### QML Profiler:

```bash
# Profile performance
QSG_RENDERER_DEBUG=1 ./qml_todo_list
```

---

## 🐛 Common Issues

### Issue 1: "Cannot assign to non-existent property"

**Problem:** Role name không match

**Solution:**
```cpp
// C++: roleNames() phải return đúng tên
roles[TextRole] = "text";  // Lowercase!

// QML: Dùng đúng tên
Text { text: model.text }  // Not model.Text
```

### Issue 2: Model không update

**Problem:** Quên emit signals

**Solution:**
```cpp
// Phải emit dataChanged
emit dataChanged(index, index, {role});

// Hoặc emit custom signal
emit countChanged();
```

### Issue 3: Segmentation fault

**Problem:** Model bị deleted trước engine

**Solution:**
```cpp
// Model phải tồn tại trong suốt app lifecycle
TodoListModel todoModel;  // Stack variable trong main()
engine.rootContext()->setContextProperty("todoModel", &todoModel);
```

---

## 📖 Further Learning

### Next Steps:

1. **Add QSqlTableModel với QML**
2. **Implement Proxy Models trong QML**
3. **Create Tree Models cho QML TreeView**
4. **Add drag & drop trong QML**
5. **Implement undo/redo**

### Resources:

- [Qt QML Documentation](https://doc.qt.io/qt-5/qtqml-index.html)
- [Qt Quick Controls](https://doc.qt.io/qt-5/qtquickcontrols-index.html)
- [QAbstractItemModel for QML](https://doc.qt.io/qt-5/qtquick-modelviewsdata-cppmodels.html)

---

## ✅ Checklist

- [x] C++ Models với Q_PROPERTY
- [x] Q_INVOKABLE methods
- [x] Custom roleNames()
- [x] QML Views
- [x] Context properties
- [x] Signal/Slot connections
- [x] Build system (CMakeLists)
- [ ] QSql integration (TODO)
- [ ] Proxy models (TODO)
- [ ] Tree models (TODO)

---

**Enjoy coding with QML! 🎨**
