# 🎉 QML Examples - Hoàn Thành!

## ✅ Đã Tạo Xong

Tôi đã tách riêng **Models (C++)** và **Views (QML)** cho bạn!

---

## 📁 Cấu Trúc Mới

```
/workspace/
│
├── examples/                    ← Qt Widgets (C++ cả Model & View)
│   ├── 01_list_model.cpp
│   ├── 02_table_model.cpp
│   └── ... (6 examples)
│
└── qml_examples/                ← QML (C++ Model + QML View) ⭐ MỚI
    ├── models/                  ← C++ Models
    │   ├── TodoListModel.h
    │   ├── TodoListModel.cpp
    │   ├── StudentTableModel.h
    │   └── StudentTableModel.cpp
    │
    ├── qml/                     ← QML Views
    │   ├── TodoListView.qml
    │   └── StudentTableView.qml
    │
    ├── main_todo.cpp            ← Entry point cho Todo app
    ├── main_student.cpp         ← Entry point cho Student app
    ├── qml.qrc                  ← Resource file
    ├── CMakeLists.txt           ← Build config
    │
    └── Documentation:
        ├── README.md            ← Đầy đủ documentation
        ├── QUICK_START.md       ← 5 phút chạy được
        └── QML_VS_WIDGETS_GUIDE.md  ← So sánh chi tiết
```

---

## 🎯 2 Examples Đã Tạo

### 1. **Todo List App** 📝

**Model (C++):** `models/TodoListModel.h/cpp`
- Quản lý danh sách todos
- Properties: text, completed, priority
- Methods: addTodo(), removeTodo(), toggleCompleted()

**View (QML):** `qml/TodoListView.qml`
- Modern UI với Material Design
- Features:
  - ✅ Add todos với priority
  - ✅ Mark completed/incomplete
  - ✅ Delete todos
  - ✅ Filter: All / Incomplete / Completed
  - ✅ Statistics display

**Entry:** `main_todo.cpp`

---

### 2. **Student Table App** 👨‍🎓

**Model (C++):** `models/StudentTableModel.h/cpp`
- Bảng quản lý sinh viên
- Columns: ID, Name, Age, Grade, Email
- Methods: addStudent(), removeStudent(), get(), set()

**View (QML):** `qml/StudentTableView.qml`
- TableView với nhiều columns
- Features:
  - ✅ Add new students
  - ✅ Edit student info
  - ✅ Delete students
  - ✅ Search by name/email
  - ✅ Statistics (Grade A count, Average age)

**Entry:** `main_student.cpp`

---

## 🚀 Cách Chạy

### Bước 1: Cài Qt Quick Module

```bash
# Ubuntu/Debian
sudo apt install qtdeclarative5-dev qml-module-qtquick-controls2

# macOS (Qt đã có Quick)
brew install qt@5
```

### Bước 2: Build

```bash
cd /workspace/qml_examples
mkdir build && cd build

# Auto-detect Qt5 or Qt6
cmake ..
make
```

### Bước 3: Run

```bash
# Todo List app
./qml_todo_list

# Student Table app
./qml_student_table
```

---

## 🔑 Key Concepts Đã Implement

### 1. **Q_PROPERTY** - Expose properties sang QML

```cpp
class TodoListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    //         ↑ Tên     ↑ Getter       ↑ Signal
};
```

**QML có thể access:**
```qml
Label {
    text: "Total: " + todoModel.count
}
```

---

### 2. **Q_INVOKABLE** - Expose methods sang QML

```cpp
Q_INVOKABLE void addTodo(const QString &text, int priority);
```

**QML có thể gọi:**
```qml
Button {
    onClicked: todoModel.addTodo("New task", 1)
}
```

---

### 3. **roleNames()** - Map roles to QML property names

```cpp
QHash<int, QByteArray> roleNames() const override {
    return {
        {TextRole, "text"},           // model.text trong QML
        {CompletedRole, "completed"}, // model.completed
        {PriorityRole, "priority"}    // model.priority
    };
}
```

**QML ListView delegate:**
```qml
ListView {
    model: todoModel
    delegate: Rectangle {
        Text { text: model.text }         // Access TextRole
        CheckBox { checked: model.completed }  // Access CompletedRole
    }
}
```

---

### 4. **setContextProperty()** - Kết nối C++ và QML

```cpp
// main.cpp
QQmlApplicationEngine engine;
engine.rootContext()->setContextProperty("todoModel", &todoModel);
//                                        ↑ Tên QML  ↑ C++ object

engine.load(QUrl("qrc:/qml/TodoListView.qml"));
```

**Trong QML:**
```qml
// Access todoModel trực tiếp
Button {
    onClicked: todoModel.addTodo("Task", 1)
}
```

---

## 📊 So Sánh: Qt Widgets vs QML

| Aspect | Qt Widgets | QML |
|--------|------------|-----|
| **UI Code** | C++ | QML (declarative) |
| **Animations** | Khó | Rất dễ |
| **Mobile Support** | Hạn chế | Tuyệt vời |
| **Hot Reload** | ❌ | ✅ |
| **Learning Curve** | Medium | Dễ hơn |
| **Model Code** | ✅ Giống nhau! | ✅ Giống nhau! |

**Kết luận:** 
- Models giống nhau (C++)
- Chỉ khác View layer!

---

## 📖 Tài Liệu Chi Tiết

### Bắt Đầu:
📘 **[QUICK_START.md](qml_examples/QUICK_START.md)**
- 5 phút chạy được
- Template tạo model mới
- Common errors & fixes

### Đầy Đủ:
📗 **[README.md](qml_examples/README.md)**
- Architecture overview
- Key concepts chi tiết
- Best practices
- Development tips

### So Sánh:
📙 **[QML_VS_WIDGETS_GUIDE.md](qml_examples/QML_VS_WIDGETS_GUIDE.md)**
- Side-by-side comparison
- Code size comparison
- When to use what
- Migration guide

---

## 💡 Lợi Ích Của Kiến Trúc Này

### ✅ Separation of Concerns
```
Business Logic (C++)  ←→  Presentation (QML)
      Model                   View
```

### ✅ Reusable Models
```cpp
// Cùng 1 model
TodoListModel model;

// Dùng cho Qt Widgets
QListView *widgetView = new QListView;
widgetView->setModel(&model);

// HOẶC dùng cho QML
engine.rootContext()->setContextProperty("todoModel", &model);
```

### ✅ Team Collaboration
- C++ developers → Models (logic)
- QML developers → Views (UI/UX)
- Độc lập nhau!

### ✅ Maintainability
- Fix bugs: Chỉ sửa Model
- Change UI: Chỉ sửa QML
- Easy to test Models riêng

---

## 🎨 QML Features Demo

### Modern UI Components:
```qml
// Material Design buttons
Button {
    text: "Add"
    highlighted: true  // Primary color
}

// Smooth animations
Rectangle {
    color: "blue"
    Behavior on color {
        ColorAnimation { duration: 300 }
    }
}

// Touch-friendly controls
SwipeView {
    Page { ... }
    Page { ... }
}
```

### Property Bindings:
```qml
Label {
    text: "Count: " + todoModel.count
    // Tự động update khi count thay đổi!
}
```

### Declarative Layout:
```qml
ColumnLayout {
    RowLayout {
        Label { text: "Name:" }
        TextField { }
    }
    Button { text: "Submit" }
}
// Đơn giản, dễ đọc!
```

---

## 🔮 Next Steps

### Có thể mở rộng:

1. **Add SQL Models**
   - QSqlTableModel với QML
   - Database operations

2. **Add Proxy Models**
   - Filter/Sort trong QML
   - Multiple filters

3. **Add Tree Model**
   - TreeView trong QML
   - Hierarchical data

4. **Advanced Features**
   - Drag & drop
   - Undo/Redo
   - Custom delegates

---

## 📚 Learning Path

```
1. ✅ Học Qt Widgets examples (examples/)
   → Hiểu Models sâu

2. ✅ Học QML examples (qml_examples/)
   → Hiểu cách expose models

3. → Tạo app của riêng bạn
   - C++ Model (business logic)
   - QML View (beautiful UI)
   - Best of both worlds!
```

---

## 🎯 Tóm Tắt

**Bạn giờ có:**

✅ **6 Qt Widgets examples** (examples/)
   - Học Models với C++ UI

✅ **2 QML examples** (qml_examples/)
   - Cùng Models, nhưng QML UI
   - Modern, declarative approach

✅ **Full documentation**
   - Guides
   - Comparisons
   - Quick starts
   - Best practices

✅ **Build system** ready
   - CMakeLists.txt cho cả 2
   - Qt5 & Qt6 compatible

---

## 🎉 Chúc Mừng!

Bạn đã có:
- ✅ Models C++ (business logic)
- ✅ Views QML (modern UI)
- ✅ Separation of concerns
- ✅ Ready for mobile & desktop

**Start building amazing apps! 🚀**

---

## 📞 Need Help?

- Đọc `QUICK_START.md` - Bắt đầu nhanh
- Đọc `README.md` - Chi tiết đầy đủ
- Đọc `QML_VS_WIDGETS_GUIDE.md` - So sánh & chọn lựa
- Xem code examples - Học từ thực tế

**Happy Coding! 🎨**
