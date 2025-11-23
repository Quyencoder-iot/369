# Hướng Dẫn Nhanh: Con Trỏ trong Qt Model-View-Delegate

## 🎯 Mục Tiêu Học Nhanh

Hiểu rõ cách Qt sử dụng **con trỏ (pointers)** trong kiến trúc Model-View-Delegate trong 30 phút!

## 📚 Kiến Thức Cơ Bản

### 1. Kiến Trúc Model-View-Delegate Là Gì?

```
┌─────────┐      ┌─────────┐      ┌───────────┐
│  MODEL  │◄────►│  VIEW   │◄────►│ DELEGATE  │
│ (Dữ liệu)│      │(Hiển thị)│      │(Tùy chỉnh) │
└─────────┘      └─────────┘      └───────────┘
```

- **Model**: Quản lý dữ liệu (QAbstractItemModel, QStandardItemModel)
- **View**: Hiển thị dữ liệu (QTableView, QListView, QTreeView)
- **Delegate**: Tùy chỉnh cách hiển thị và chỉnh sửa (QStyledItemDelegate)

### 2. Tại Sao Dùng Con Trỏ?

Qt sử dụng con trỏ vì:
- ✅ **Quản lý bộ nhớ tự động** với parent-child hierarchy
- ✅ **Đa hình (polymorphism)**: dùng base class pointer cho nhiều loại model/view
- ✅ **Chia sẻ dữ liệu**: nhiều view có thể trỏ đến cùng một model
- ✅ **Hiệu năng**: không copy toàn bộ object, chỉ truyền địa chỉ

## 🔑 Các Loại Con Trỏ Quan Trọng

### 1. Con Trỏ Thô (Raw Pointers) - `T*`

```cpp
// Tạo model với parent
QStandardItemModel* model = new QStandardItemModel(this);
//                   ^                              ^
//                   |                              |
//              Con trỏ thô                    Parent (Qt tự xóa)
```

**Quy tắc vàng**: Nếu truyền `parent`, Qt sẽ tự động xóa object khi parent bị hủy!

### 2. Con Trỏ Thông Minh (Smart Pointers) - `QSharedPointer`, `QPointer`

```cpp
// QSharedPointer: Đếm số lần sử dụng, tự động xóa khi không còn ai dùng
QSharedPointer<QStandardItemModel> model(new QStandardItemModel);

// QPointer: Tự động set về nullptr nếu object bị xóa
QPointer<QTableView> view = new QTableView;
if (view) {  // An toàn: kiểm tra xem còn tồn tại không
    view->show();
}
```

## 💡 Cách Con Trỏ Hoạt Động Trong Qt MVC

### Ví Dụ 1: Kết Nối Model-View

```cpp
// 1. Tạo model (con trỏ)
QStandardItemModel* model = new QStandardItemModel(4, 2);  // 4 hàng, 2 cột

// 2. Tạo view (con trỏ)
QTableView* tableView = new QTableView;

// 3. Kết nối: View GIỮ CON TRỎ đến model
tableView->setModel(model);
//         ^
//         |__ View LƯU địa chỉ model, KHÔNG COPY dữ liệu!

// 4. Nhiều view có thể dùng CÙNG model
QTableView* anotherView = new QTableView;
anotherView->setModel(model);  // Cùng dữ liệu!
```

### Ví Dụ 2: Custom Delegate với Con Trỏ

```cpp
// 1. Tạo delegate tùy chỉnh
class MyDelegate : public QStyledItemDelegate {
public:
    // Constructor nhận parent pointer
    MyDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    
    // Override: nhận con trỏ đến painter, option, index
    void paint(QPainter* painter,           // Con trỏ đến painter
               const QStyleOptionViewItem& option, 
               const QModelIndex& index) const override {
        // Sử dụng pointer operator ->
        painter->save();
        painter->drawText(option.rect, "Custom!");
        painter->restore();
    }
};

// 2. Gắn delegate vào view
MyDelegate* delegate = new MyDelegate(tableView);
tableView->setItemDelegate(delegate);
//         ^
//         |__ View GIỮ CON TRỎ đến delegate
```

## 🚀 Chiến Lược Học Nhanh

### Bước 1: Hiểu 3 Pattern Cơ Bản (5 phút)

```cpp
// Pattern 1: TẠO với parent → Qt tự xóa
Widget* w = new Widget(parentWidget);

// Pattern 2: LƯU CON TRỎ → không copy object
view->setModel(modelPointer);

// Pattern 3: TRUYỀN CON TRỎ vào hàm → cho phép modify
void processModel(QAbstractItemModel* model) {
    model->setData(...);  // Thay đổi model gốc
}
```

### Bước 2: Nhớ Các Hàm Trả Con Trỏ (10 phút)

```cpp
// Các hàm trả con trỏ:
QAbstractItemModel* model = view->model();          // Lấy model từ view
QAbstractItemDelegate* del = view->itemDelegate();  // Lấy delegate
QModelIndex* index = ...;                           // Index của item

// NULL CHECK quan trọng!
if (model != nullptr) {
    // An toàn sử dụng
}
```

### Bước 3: Thực Hành với Code Mẫu (15 phút)

Xem các file code mẫu trong workspace:
- `simple_model_example.cpp` - Model cơ bản
- `custom_delegate_example.cpp` - Delegate tùy chỉnh
- `pointer_patterns.cpp` - Các pattern thường dùng

## ⚠️ Lỗi Thường Gặp và Cách Tránh

### Lỗi 1: Quên Set Parent → Memory Leak

```cpp
// ❌ SAI: Không có parent, phải tự xóa
QStandardItemModel* model = new QStandardItemModel();
// ... quên delete → MEMORY LEAK!

// ✅ ĐÚNG: Có parent, Qt tự xóa
QStandardItemModel* model = new QStandardItemModel(this);
```

### Lỗi 2: Dangling Pointer (Con trỏ treo)

```cpp
// ❌ SAI:
QTableView* view = new QTableView;
delete view;
view->show();  // CRASH! Con trỏ trỏ đến vùng nhớ đã xóa

// ✅ ĐÚNG: Dùng QPointer
QPointer<QTableView> view = new QTableView;
delete view;
if (view) {  // view tự động = nullptr
    view->show();
}
```

### Lỗi 3: Model Bị Xóa Trước View

```cpp
// ❌ SAI:
{
    QStandardItemModel model;  // Stack object
    tableView->setModel(&model);
}  // model bị xóa ở đây
tableView->show();  // CRASH! Model không còn tồn tại

// ✅ ĐÚNG:
QStandardItemModel* model = new QStandardItemModel(this);
tableView->setModel(model);  // Model tồn tại cùng với this
```

## 📖 Tài Liệu Tham Khảo Nhanh

### Con Trỏ trong Model

| Hàm | Trả về | Ý nghĩa |
|-----|--------|---------|
| `view->model()` | `QAbstractItemModel*` | Lấy con trỏ đến model |
| `model->parent(index)` | `QModelIndex` | Parent của index (không phải QObject parent!) |
| `model->index(row, col)` | `QModelIndex` | Index tại vị trí |

### Con Trỏ trong Delegate

| Hàm | Tham số | Ý nghĩa |
|-----|---------|---------|
| `paint()` | `QPainter*` | Con trỏ để vẽ |
| `createEditor()` | `QWidget*` parent | Trả `QWidget*` editor |
| `setEditorData()` | `QWidget*` editor | Nhận con trỏ editor |

### Object Ownership (Quyền sở hữu)

```cpp
// Qt quản lý bộ nhớ theo Parent-Child:
QWidget* parent = new QWidget;
QWidget* child1 = new QWidget(parent);  // parent sở hữu child1
QWidget* child2 = new QWidget(parent);  // parent sở hữu child2

delete parent;  // Tự động xóa child1 và child2!
```

## 🎓 Bài Tập Thực Hành

1. **Cơ bản**: Tạo QTableView với QStandardItemModel, populate 10 rows
2. **Trung bình**: Tạo custom delegate để hiển thị progress bar
3. **Nâng cao**: Một model chia sẻ cho 2 views khác nhau

## 💪 Tips Học Nhanh

1. **Vẽ sơ đồ**: Vẽ mũi tên con trỏ giữa Model ←→ View ←→ Delegate
2. **Debug với breakpoint**: Đặt breakpoint xem giá trị con trỏ
3. **In địa chỉ**: `qDebug() << "Model address:" << model;`
4. **Đọc Qt source code**: Xem cách Qt implement QTableView::setModel()
5. **Thực hành 30 phút/ngày**: Tạo 1 ví dụ nhỏ mỗi ngày

## 🔗 Resources

- [Qt Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html)
- [QAbstractItemModel Documentation](https://doc.qt.io/qt-6/qabstractitemmodel.html)
- [Object Trees & Ownership](https://doc.qt.io/qt-6/objecttrees.html)

---

**Chúc bạn học tốt! Bắt đầu với file `simple_model_example.cpp` để thực hành ngay! 🚀**
