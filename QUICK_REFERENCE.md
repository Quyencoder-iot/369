# 📋 Quick Reference: Con Trỏ Qt Model-View-Delegate

## ⚡ Nhớ Nhanh trong 30 Giây

```
Widget (Parent)
  ├── Model (child) ✅
  ├── View (child) ✅
  │   └── Delegate (child của View) ✅
```

**Quy tắc vàng**: Luôn truyền `parent` → Qt tự động cleanup!

---

## 🎯 Ownership Matrix

| Object | Parent | Owner | Delete Khi |
|--------|--------|-------|-----------|
| Model | Widget | Widget | Widget destroy |
| View | Widget | Widget | Widget destroy |
| Delegate | View | View | View destroy |
| Editor | View | View | Edit xong |
| Index | - | Không | Model thay đổi |

---

## ✅ Code Pattern Chuẩn

```cpp
class MyWidget : public QWidget {
    Q_OBJECT
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // ✅ Model với parent
        m_model = new QStandardItemModel(this);
        
        // ✅ View với parent
        m_view = new QTableView(this);
        m_view->setModel(m_model); // View KHÔNG sở hữu model
        
        // ✅ Delegate với parent là view
        m_delegate = new MyDelegate(m_view);
        m_view->setItemDelegate(m_delegate);
    }
    // ✅ Không cần destructor!
    
private:
    QStandardItemModel* m_model;
    QTableView* m_view;
    MyDelegate* m_delegate;
};
```

---

## 🚫 Các Lỗi Thường Gặp

### ❌ Lỗi 1: Delete object có parent
```cpp
m_model = new QStandardItemModel(this);
delete m_model; // ❌ SAI! Qt sẽ tự delete
```

### ❌ Lỗi 2: Model không có parent
```cpp
m_model = new QStandardItemModel(); // ❌ Thiếu parent
m_view->setModel(m_model);
// Model sẽ không được cleanup tự động!
```

### ❌ Lỗi 3: Model bị delete trước View
```cpp
QStandardItemModel* model = new QStandardItemModel();
view->setModel(model);
delete model; // ❌ View sẽ crash!
```

### ❌ Lỗi 4: Không kiểm tra index
```cpp
QVariant data = index.data(); // ❌ Có thể crash nếu invalid
```

### ✅ Đúng:
```cpp
if (index.isValid()) {
    QVariant data = index.data();
}
```

---

## 🔍 Checklist Nhanh

Trước khi code, hỏi:

- [ ] Model có parent chưa? → `new Model(this)`
- [ ] View có parent chưa? → `new View(this)`
- [ ] Delegate có parent chưa? → `new Delegate(view)`
- [ ] Đã kiểm tra `index.isValid()` chưa?
- [ ] Model sống lâu hơn View chưa?

---

## 📝 Index Safety

```cpp
// ✅ ĐÚNG: Kiểm tra ngay
QModelIndex index = model->index(row, col);
if (index.isValid()) {
    // Dùng index
}

// ✅ ĐÚNG: Dùng persistent nếu cần lưu
QPersistentModelIndex saved = model->index(0, 0);

// ❌ SAI: Lưu QModelIndex
QModelIndex saved = model->index(0, 0);
// Có thể invalid sau khi model thay đổi!
```

---

## 🔗 Model-View Connection

```cpp
// ✅ 1 Model, nhiều View
QStandardItemModel* model = new QStandardItemModel(this);

QTableView* view1 = new QTableView(this);
QTreeView* view2 = new QTreeView(this);

view1->setModel(model); // ✅ OK
view2->setModel(model); // ✅ OK - cùng model

// Khi model thay đổi, cả 2 view tự động cập nhật!
```

---

## 🎨 Delegate Pattern

```cpp
// ✅ Delegate được view sở hữu
QStyledItemDelegate* delegate = new QStyledItemDelegate(view);
view->setItemDelegate(delegate);

// ✅ Delegate riêng cho từng cột
QSpinBoxDelegate* spinDelegate = new QSpinBoxDelegate(view);
view->setItemDelegateForColumn(0, spinDelegate);

// ✅ Editor được view sở hữu
QWidget* createEditor(QWidget* parent, ...) {
    return new QSpinBox(parent); // parent là view
}
```

---

## 💡 Tips Học Nhanh

1. **Vẽ sơ đồ parent-child** trước khi code
2. **Dùng Qt Creator** để xem object tree
3. **Thêm qDebug()** để trace lifecycle
4. **Test với Valgrind** để check memory leaks
5. **Đọc constructor docs** - luôn có tham số `parent`

---

## 🎓 Thực Hành Ngay

1. Copy code pattern chuẩn ở trên
2. Chạy các ví dụ trong thư mục `examples/`
3. Sửa code để thấy lỗi (ví dụ: xóa parent)
4. Quan sát behavior khác nhau

---

## 📚 Khi Cần Chi Tiết

Xem file `QT_MODEL_VIEW_DELEGATE_GUIDE.md` để có:
- Giải thích chi tiết
- Nhiều ví dụ hơn
- Best practices đầy đủ
- Bài tập thực hành

---

**Nhớ**: Parent = Auto cleanup! 🚀
