# Hướng Dẫn Học Nhanh Con Trỏ Trong Qt Model-View-Delegate

## Tổng Quan

Qt Model-View-Delegate (MVD) là một framework mạnh mẽ để hiển thị và quản lý dữ liệu. Hiểu rõ về con trỏ trong framework này sẽ giúp bạn viết code hiệu quả và tránh lỗi.

## 1. Khái Niệm Cơ Bản

### 1.1. Model-View-Delegate là gì?
- **Model**: Chứa dữ liệu và logic xử lý dữ liệu
- **View**: Hiển thị dữ liệu từ Model  
- **Delegate**: Điều khiển cách dữ liệu được hiển thị và chỉnh sửa

### 1.2. Con Trỏ Quan Trọng

```cpp
// Con trỏ đến model
QAbstractItemModel* model;

// Con trỏ đến view  
QTableView* view;
QTreeView* treeView;
QListView* listView;

// Con trỏ đến delegate
QStyledItemDelegate* delegate;

// Con trỏ đến item
QModelIndex index;  // Không phải con trỏ nhưng hoạt động như reference
```

## 2. Quy Tắc Quan Trọng Về Con Trỏ

### 2.1. Ownership (Quyền Sở Hữu)

**Rule #1**: View KHÔNG sở hữu Model
```cpp
// SAI - View sẽ xóa model khi bị hủy
QTableView* view = new QTableView();
QStandardItemModel* model = new QStandardItemModel(view); // KHÔNG NÊN

// ĐÚNG - Quản lý riêng biệt
QStandardItemModel* model = new QStandardItemModel();
QTableView* view = new QTableView();
view->setModel(model);
```

**Rule #2**: Model sở hữu các Item
```cpp
QStandardItemModel* model = new QStandardItemModel();
QStandardItem* item = new QStandardItem("Data");
model->appendRow(item); // Model giờ sở hữu item
// KHÔNG delete item thủ công!
```

### 2.2. Parent-Child Relationship

```cpp
// Parent quản lý con trỏ của child
QWidget* parent = new QWidget();
QTableView* view = new QTableView(parent); // parent sẽ tự động delete view
```

## 3. Các Pattern Phổ Biến

### 3.1. Model Sharing (Chia sẻ Model)
```cpp
QStandardItemModel* model = new QStandardItemModel();

QTableView* view1 = new QTableView();
view1->setModel(model);

QListView* view2 = new QListView();  
view2->setModel(model); // Cùng model, khác view
```

### 3.2. Safe Pointer Usage
```cpp
// Sử dụng QPointer cho safety
QPointer<QStandardItemModel> safeModel = new QStandardItemModel();

// Kiểm tra trước khi dùng
if (safeModel) {
    safeModel->setData(index, value);
}
```

### 3.3. Model Index và Con Trỏ
```cpp
// QModelIndex KHÔNG phải con trỏ
QModelIndex index = model->index(0, 0);

// Lấy con trỏ đến item từ index
QStandardItem* item = model->itemFromIndex(index);
if (item) { // Luôn kiểm tra nullptr
    item->setText("New Text");
}
```

## 4. Lỗi Thường Gặp và Cách Tránh

### 4.1. Dangling Pointer
```cpp
// LỖI: Con trỏ treo
QStandardItemModel* model = new QStandardItemModel();
view->setModel(model);
delete model; // View vẫn giữ con trỏ đến model đã bị xóa!

// ĐÚNG: Clear model trước
view->setModel(nullptr);
delete model;
```

### 4.2. Memory Leak
```cpp
// LỖI: Rò rỉ bộ nhớ
void createView() {
    QTableView* view = new QTableView(); // Không có parent
    // view không bao giờ được delete
}

// ĐÚNG: 
void createView(QWidget* parent) {
    QTableView* view = new QTableView(parent);
    // parent sẽ tự động delete view
}
```

### 4.3. Invalid Index
```cpp
// Luôn kiểm tra index hợp lệ
if (index.isValid()) {
    model->setData(index, value);
}
```

## 5. Tips Học Nhanh

### 5.1. Bắt đầu Đơn Giản
1. Thực hành với `QStringListModel` trước
2. Chuyển sang `QStandardItemModel`
3. Cuối cùng mới tự viết custom model

### 5.2. Debug Con Trỏ
```cpp
// In địa chỉ con trỏ
qDebug() << "Model pointer:" << model;
qDebug() << "View's model:" << view->model();

// Kiểm tra ownership
qDebug() << "Model parent:" << model->parent();
```

### 5.3. Sử Dụng Smart Pointers
```cpp
// C++11 smart pointers
std::unique_ptr<QStandardItemModel> model(new QStandardItemModel());
std::shared_ptr<CustomDelegate> delegate = std::make_shared<CustomDelegate>();
```

## 6. Workflow Khuyến Nghị

1. **Tạo Model trước**: Định nghĩa cấu trúc dữ liệu
2. **Setup View**: Chọn loại view phù hợp  
3. **Connect Model-View**: Dùng setModel()
4. **Custom Delegate** (nếu cần): Override paint() và createEditor()
5. **Handle Signals/Slots**: Connect các signal quan trọng

## 7. Checklist Khi Làm Việc Với Con Trỏ

- [ ] Model có parent không? Nếu không, ai sẽ delete nó?
- [ ] View có kiểm tra model != nullptr trước khi dùng?
- [ ] Các item được add vào model chưa? (để model quản lý memory)
- [ ] Index có valid không trước khi truy cập?
- [ ] Có circular reference giữa các object không?
- [ ] Signal/slot có được disconnect khi cần không?

## Tài Liệu Tham Khảo
- Qt Documentation: Model/View Programming
- Qt Examples: itemviews
- Qt Wiki: Model View Examples