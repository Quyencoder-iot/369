# 📋 CHEAT SHEET: Con Trỏ trong Qt Model-View-Delegate

## 🎯 Tóm Tắt Nhanh (1 phút)

```cpp
// Tạo với parent → Qt tự xóa
Model* m = new Model(this);

// Kết nối view-model → view lưu con trỏ
view->setModel(m);

// Luôn null check
if (ptr != nullptr) { /* ... */ }
```

---

## 📊 Bảng Tra Cứu Nhanh

### 1. Các Loại Con Trỏ

| Type | Khi nào dùng | Ví dụ | Tự động xóa? |
|------|-------------|-------|--------------|
| `T*` | Có parent | `new Model(this)` | ✅ (nếu có parent) |
| `QPointer<T>` | Lo ngại dangling | `QPointer<View> v = new View()` | ❌ (nhưng auto null) |
| `QSharedPointer<T>` | Chia sẻ ownership | `QSharedPointer<M>(new M())` | ✅ (khi ref = 0) |

### 2. Functions Trả Con Trỏ

| Function | Trả về | Null được không? | Ví dụ |
|----------|--------|-----------------|-------|
| `view->model()` | `QAbstractItemModel*` | ✅ | `auto m = view->model();` |
| `model->item(r,c)` | `QStandardItem*` | ✅ | `auto item = model->item(0,0);` |
| `view->itemDelegate()` | `QAbstractItemDelegate*` | ❌ | `auto d = view->itemDelegate();` |
| `createEditor(...)` | `QWidget*` | ❌ | `return new QSpinBox(parent);` |

### 3. Pointer Operators

| Operator | Ý nghĩa | Ví dụ |
|----------|---------|-------|
| `->` | Truy cập member qua pointer | `ptr->show();` |
| `*` | Dereference | `int x = *ptr;` |
| `&` | Lấy địa chỉ | `T* ptr = &obj;` |
| `==` | So sánh địa chỉ | `if (p1 == p2)` |

---

## 🔑 Code Snippets Hay Dùng

### Snippet 1: Tạo Model-View

```cpp
// Trong constructor của window/widget
QStandardItemModel* m_model;
QTableView* m_view;

// Setup
m_model = new QStandardItemModel(rows, cols, this); // parent = this
m_view = new QTableView(this);
m_view->setModel(m_model);  // View lưu con trỏ model
```

### Snippet 2: Custom Delegate

```cpp
class MyDelegate : public QStyledItemDelegate {
public:
    MyDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {}
    
    void paint(QPainter* painter, 
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        // Dùng painter pointer
        painter->save();
        // ... vẽ ...
        painter->restore();
    }
};

// Sử dụng
auto delegate = new MyDelegate(view);  // parent = view
view->setItemDelegate(delegate);
```

### Snippet 3: Null Check Pattern

```cpp
// Pattern 1: Explicit
if (ptr != nullptr) {
    ptr->doSomething();
}

// Pattern 2: Implicit
if (ptr) {
    ptr->doSomething();
}

// Pattern 3: Early return
if (!ptr) return;
ptr->doSomething();
```

### Snippet 4: QPointer Safety

```cpp
QPointer<QTableView> m_view = new QTableView();

// An toàn khi object có thể bị xóa
if (m_view) {
    m_view->show();
} else {
    qDebug() << "View đã bị xóa";
}
```

### Snippet 5: Shared Model

```cpp
QStandardItemModel* model = new QStandardItemModel(parent);

QTableView* view1 = new QTableView();
QTableView* view2 = new QTableView();
QListView* view3 = new QListView();

// Tất cả dùng chung model
view1->setModel(model);
view2->setModel(model);
view3->setModel(model);
```

### Snippet 6: Get Item Pointer

```cpp
QStandardItem* item = model->item(row, col);
if (item) {
    // Đọc
    QString text = item->text();
    
    // Sửa
    item->setText("New text");
    item->setForeground(Qt::red);
}
```

### Snippet 7: QObject Cast

```cpp
QAbstractItemModel* basePtr = view->model();

// Cast an toàn
QStandardItemModel* standardModel = 
    qobject_cast<QStandardItemModel*>(basePtr);

if (standardModel) {
    // Cast thành công
    QStandardItem* item = standardModel->item(0, 0);
}
```

---

## ⚠️ Lỗi Thường Gặp

### ❌ Lỗi 1: Quên Parent

```cpp
// SAI
QStandardItemModel* m = new QStandardItemModel();
// ... quên delete → MEMORY LEAK

// ĐÚNG
QStandardItemModel* m = new QStandardItemModel(this);
// Qt tự xóa khi 'this' bị hủy
```

### ❌ Lỗi 2: Dangling Pointer

```cpp
// SAI
QTableView* view = new QTableView();
delete view;
view->show();  // CRASH!

// ĐÚNG - Dùng QPointer
QPointer<QTableView> view = new QTableView();
delete view;
if (view) view->show();  // An toàn
```

### ❌ Lỗi 3: Không Null Check

```cpp
// SAI
QStandardItem* item = model->item(10, 10);
item->setText("Crash!");  // Có thể CRASH nếu item = nullptr

// ĐÚNG
QStandardItem* item = model->item(10, 10);
if (item) {
    item->setText("Safe!");
}
```

### ❌ Lỗi 4: Xóa Nhầm

```cpp
// SAI
void MyView::setMyModel(QAbstractItemModel* model) {
    m_model = model;
}
// Nếu ai đó xóa model bên ngoài → m_model dangling!

// ĐÚNG
void MyView::setMyModel(QAbstractItemModel* model) {
    m_model = model;
    // Hoặc connect destroyed signal để cleanup
}
```

---

## 💡 Tips & Tricks

### Tip 1: Debug Pointer Address

```cpp
qDebug() << "Pointer:" << ptr;                    // In địa chỉ
qDebug() << "Pointer:" << static_cast<void*>(ptr); // Cách khác
```

### Tip 2: Check Parent

```cpp
QObject* parent = widget->parent();
qDebug() << "Parent:" << parent;
if (parent) {
    qDebug() << "Parent name:" << parent->objectName();
}
```

### Tip 3: Count References

```cpp
QSharedPointer<Model> ptr(new Model());
qDebug() << "Ref count:" << ptr.use_count();
```

### Tip 4: Lambda với Pointer

```cpp
QPushButton* btn = new QPushButton("Click");
QTableView* view = new QTableView();

// Capture pointer
connect(btn, &QPushButton::clicked, [view]() {
    if (view) view->resizeColumnsToContents();
});
```

### Tip 5: This Pointer trong Qt

```cpp
class MyWidget : public QWidget {
    void init() {
        // 'this' là con trỏ đến object hiện tại
        auto model = new Model(this);  // this làm parent
        connect(btn, SIGNAL(clicked()), this, SLOT(onClicked()));
    }
};
```

---

## 📐 Memory Diagrams

### Diagram 1: View → Model

```
┌─────────────┐
│  QTableView │
│   (View)    │
│             │
│  m_model ───┼──→  ┌──────────────────┐
└─────────────┘     │ QStandardItemModel│
                    │     (Model)       │
                    └──────────────────┘
                    
View LƯU CON TRỎ đến Model
```

### Diagram 2: Shared Model

```
┌─────────────┐
│  TableView  │──┐
└─────────────┘  │
                 │    ┌──────────────┐
┌─────────────┐  ├──→ │    MODEL     │
│  ListView   │──┘    └──────────────┘
└─────────────┘
                 
Nhiều views → Cùng 1 model
```

### Diagram 3: Parent-Child

```
┌─────────────────────┐
│   QWidget (Parent)  │
│                     │
│  ┌──────────────┐   │
│  │  Child 1     │◄──┼─── parent pointer
│  └──────────────┘   │
│  ┌──────────────┐   │
│  │  Child 2     │◄──┼─── parent pointer
│  └──────────────┘   │
└─────────────────────┘

Delete parent → Auto delete children
```

---

## 🎓 Quiz Nhanh

### Q1: Code này có lỗi không?

```cpp
QStandardItemModel* model = new QStandardItemModel();
QTableView* view = new QTableView();
view->setModel(model);
```

<details>
<summary>Trả lời</summary>

✅ Code chạy được nhưng CÓ MEMORY LEAK!
- Không có parent → phải delete thủ công
- **Fix**: Thêm parent: `new QStandardItemModel(this)`
</details>

### Q2: Sau khi chạy code này, model1 và model2 có cùng dữ liệu không?

```cpp
QAbstractItemModel* model1 = view1->model();
QAbstractItemModel* model2 = view2->model();
```

<details>
<summary>Trả lời</summary>

❓ CHƯA BIẾT!
- Nếu `view1->setModel(m)` và `view2->setModel(m)` cùng model m → **CÓ**
- Nếu setModel khác model → **KHÔNG**
- Kiểm tra: `model1 == model2` (so sánh địa chỉ)
</details>

### Q3: Code này crash không?

```cpp
QTableView* view = new QTableView();
QAbstractItemModel* model = view->model();
model->setData(...);  // ← Dòng này?
```

<details>
<summary>Trả lời</summary>

🔥 CÓ THỂ CRASH!
- Nếu chưa setModel → `model == nullptr` → crash!
- **Fix**: Null check: `if (model) model->setData(...);`
</details>

---

## 📚 Tài Liệu Tham Khảo

- **Qt Docs**: https://doc.qt.io/qt-6/model-view-programming.html
- **Object Trees**: https://doc.qt.io/qt-6/objecttrees.html
- **QPointer**: https://doc.qt.io/qt-6/qpointer.html
- **QSharedPointer**: https://doc.qt.io/qt-6/qsharedpointer.html

---

## 🚀 Bài Tập Thực Hành

1. ✏️ Tạo 1 model, 3 views khác nhau (Table, List, Tree)
2. ✏️ Implement custom delegate vẽ progress bar
3. ✏️ Dùng QPointer để quản lý con trỏ view an toàn
4. ✏️ Tạo proxy model để filter dữ liệu
5. ✏️ In địa chỉ tất cả pointers ra console để hiểu rõ

---

**In sheet này ra và dán bên cạnh màn hình khi code Qt! 💪**
