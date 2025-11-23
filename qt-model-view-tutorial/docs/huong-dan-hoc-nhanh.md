# Hướng Dẫn Học Nhanh Con Trỏ Trong Qt Model-View-Delegate

## 🚀 Lộ Trình Học Nhanh (Quick Learning Path)

### Bước 1: Hiểu Concepts Cơ Bản (30 phút)
1. Đọc phần **Tổng Quan** trong `huong-dan-con-tro-qt-mvc.md`
2. Focus vào 3 khái niệm chính:
   - **Model**: Chứa dữ liệu
   - **View**: Hiển thị dữ liệu
   - **Delegate**: Customize hiển thị và edit

### Bước 2: Nắm Vững Quy Tắc Quan Trọng (20 phút)
**MUST REMEMBER:**
```cpp
// Rule #1: View KHÔNG sở hữu Model
view->setModel(model); // view không delete model

// Rule #2: Model sở hữu Items
model->appendRow(item); // model sẽ delete item

// Rule #3: Parent quản lý Children
QWidget* parent = new QWidget();
QTableView* view = new QTableView(parent); // parent delete view
```

### Bước 3: Practice Với Demo App (45 phút)
1. Build và chạy demo:
   ```bash
   cd qt-model-view-tutorial
   ./build.sh
   ./build/model_view_demo
   ```

2. Thử từng tab theo thứ tự:
   - **Model Pointers**: Click "Demo Ownership" → "Demo Sharing"
   - **View Pointers**: Click "View-Model Relation" → "Shared Selection"
   - **Delegate Pointers**: Click "Delegate Lifecycle" → Double-click cells
   - **Memory Management**: Đọc và test từng pattern

### Bước 4: Code Theo Examples (1 giờ)
1. Copy và modify `examples/model_pointer_example.cpp`
2. Thêm breakpoints ở các destructor
3. Run trong debugger để xem object lifecycle

## 💡 Tips Đọc Hiểu Code Nhanh

### 1. Tìm Ownership Pattern
Khi đọc code Qt, luôn tìm:
```cpp
// Có parent? → Tự động delete
new QTableView(parent);

// Không parent? → Cần manual delete hoặc smart pointer
new QStandardItemModel(); // Memory leak risk!
```

### 2. Check Pointer Validity
```cpp
// ALWAYS check before use
if (model) {
    model->setData(index, value);
}

// Or use QPointer
QPointer<QStandardItemModel> safeModel = model;
if (safeModel) { // Auto-null when deleted
    // Safe to use
}
```

### 3. Trace Signal Flow
Model-View kết nối qua signals:
```cpp
// Model → View: dataChanged()
// View → Model: through delegate
// Selection: selectionChanged()
```

## 🔍 Debug Techniques

### 1. Log Object Lifecycle
```cpp
class MyModel : public QStandardItemModel {
public:
    MyModel() { qDebug() << "Model created" << this; }
    ~MyModel() { qDebug() << "Model destroyed" << this; }
};
```

### 2. Use Address Sanitizer
```bash
# Detect memory issues
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" ../src
```

### 3. Track Pointers
```cpp
qDebug() << "Model pointer:" << model;
qDebug() << "View's model:" << view->model();
qDebug() << "Same?" << (model == view->model());
```

## 📝 Checklist Khi Viết Code

Trước khi commit code, check:

- [ ] Mọi `new` có tương ứng với `delete` hoặc parent?
- [ ] Views có clear model trước khi delete model?
- [ ] Delegates có cùng lifetime với view?
- [ ] Items đã được add vào model chưa?
- [ ] Có check nullptr trước khi dùng?
- [ ] Có circular reference không?

## 🎯 Common Patterns - Copy & Use

### Pattern 1: Safe Model Creation
```cpp
// GOOD: Model with parent
auto model = new QStandardItemModel(this);

// GOOD: Smart pointer
auto model = std::make_unique<QStandardItemModel>();
view->setModel(model.get());
```

### Pattern 2: Multiple Views
```cpp
// One model, many views
QStandardItemModel* model = new QStandardItemModel(this);
tableView->setModel(model);
treeView->setModel(model);
listView->setModel(model);
```

### Pattern 3: Safe Cleanup
```cpp
// Clear views first
tableView->setModel(nullptr);
treeView->setModel(nullptr);
// Then delete model
delete model;
```

## 🚨 Red Flags - Cần Tránh

1. **Stack object address in heap container**
   ```cpp
   QList<QWidget*> widgets;
   QWidget localWidget;
   widgets.append(&localWidget); // CRASH khi out of scope!
   ```

2. **Delete item đã add vào model**
   ```cpp
   model->appendRow(item);
   delete item; // WRONG! Model owns it
   ```

3. **Forget to clear model**
   ```cpp
   delete model;
   // view->model() is dangling pointer!
   ```

## 📚 Đọc Thêm

Sau khi nắm basics, đọc thêm:
1. [QAbstractItemModel](https://doc.qt.io/qt-6/qabstractitemmodel.html) - Tự viết model
2. [QSortFilterProxyModel](https://doc.qt.io/qt-6/qsortfilterproxymodel.html) - Filter và sort
3. [Drag and Drop](https://doc.qt.io/qt-6/model-view-programming.html#using-drag-and-drop-with-item-views) - Advanced features

---

**Remember**: Practice makes perfect! Chạy demo, break things, fix them, và học từ lỗi! 💪