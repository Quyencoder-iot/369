# ⚡ Quick Start - Bắt Đầu Ngay Trong 5 Phút

## 🎯 Mục Tiêu
Hiểu cơ bản về con trỏ trong Qt MVC và chạy được ví dụ đầu tiên!

---

## 📚 Bước 1: Đọc 3 Khái Niệm Core (2 phút)

### 1️⃣ Model = Dữ Liệu
```cpp
QStandardItemModel* model = new QStandardItemModel(this);
//                  ^                              ^^^^
//                  Con trỏ                        Parent
```
- Model LƯU dữ liệu
- Tạo với `new`, truyền `this` làm parent
- Parent tự động xóa → không cần `delete`

### 2️⃣ View = Hiển Thị
```cpp
QTableView* view = new QTableView(this);
view->setModel(model);  // View trỏ đến model
//     ^
//     View GIỮ con trỏ đến model
```
- View HIỂN THỊ dữ liệu từ model
- `setModel()` → view lưu địa chỉ model
- Nhiều views có thể cùng trỏ đến 1 model

### 3️⃣ Delegate = Tùy Chỉnh Hiển Thị
```cpp
MyDelegate* delegate = new MyDelegate(view);
view->setItemDelegate(delegate);
```
- Delegate TÙY CHỈNH cách vẽ và edit
- Nhận con trỏ `QPainter*` để vẽ
- Tạo và trả về con trỏ editor widget

---

## 🚀 Bước 2: Chạy Ví Dụ Đầu Tiên (3 phút)

### Option A: Có Qt & CMake sẵn

```bash
# Build
mkdir build && cd build
cmake ..
make

# Chạy
./simple_model_example
```

### Option B: Có Qt Creator

1. Mở Qt Creator
2. File → Open → chọn `CMakeLists.txt`
3. Click nút Run ▶️
4. Xem output trong console

### Option C: Đọc code không build

Mở file `simple_model_example.cpp` và đọc:
- Dòng 30-40: Tạo model với parent
- Dòng 50: Kết nối view-model
- Dòng 80: Demo con trỏ hoạt động

---

## 💡 3 Điều Quan Trọng Nhất

### ✅ 1. Luôn Có Parent
```cpp
// ❌ SAI - Memory leak
QStandardItemModel* model = new QStandardItemModel();

// ✅ ĐÚNG - Qt tự xóa
QStandardItemModel* model = new QStandardItemModel(this);
```

### ✅ 2. View Dùng Con Trỏ Model
```cpp
QStandardItemModel* model = new QStandardItemModel(this);
QTableView* view = new QTableView(this);

view->setModel(model);  // View lưu con trỏ
// Thay đổi model → view tự động cập nhật!
```

### ✅ 3. Null Check Trước Khi Dùng
```cpp
QAbstractItemModel* model = view->model();

// ✅ ĐÚNG - Kiểm tra null
if (model != nullptr) {
    model->setData(...);
}

// ❌ SAI - Có thể crash
model->setData(...);  // Nếu model = null → CRASH!
```

---

## 🎓 Tiếp Theo: Học Gì?

### Vừa xong Quick Start → Bây giờ:

1. **Đọc chi tiết**: [HUONG_DAN_QT_MVC.md](./HUONG_DAN_QT_MVC.md)
   - Hiểu sâu hơn về pointers
   - Học các loại con trỏ: Raw, QPointer, QSharedPointer
   - Nắm vững parent-child ownership

2. **Xem Cheat Sheet**: [CHEAT_SHEET.md](./CHEAT_SHEET.md)
   - Tra cứu nhanh khi code
   - Copy-paste code snippets
   - Xem các lỗi thường gặp

3. **Chạy các ví dụ khác**:
   ```bash
   ./custom_delegate_example   # Delegate với con trỏ
   ./pointer_patterns          # 8 patterns thường dùng
   ```

4. **Làm bài tập**: Xem phần "Bài Tập Thực Hành" trong README.md

---

## 🔑 Code Template Cơ Bản

Copy-paste này vào project của bạn:

```cpp
#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Tạo window
    QWidget window;
    
    // Tạo model với parent
    QStandardItemModel* model = new QStandardItemModel(3, 2, &window);
    model->setHorizontalHeaderItem(0, new QStandardItem("Column 1"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Column 2"));
    
    // Điền dữ liệu
    for (int row = 0; row < 3; ++row) {
        model->setItem(row, 0, new QStandardItem(QString("Row %1").arg(row)));
        model->setItem(row, 1, new QStandardItem(QString("Data %1").arg(row)));
    }
    
    // Tạo view và kết nối model
    QTableView* view = new QTableView(&window);
    view->setModel(model);  // View trỏ đến model
    
    // Layout
    QVBoxLayout* layout = new QVBoxLayout(&window);
    layout->addWidget(view);
    
    window.show();
    return app.exec();
}
```

**Lưu vào file `test.cpp`, build và chạy!**

---

## ❓ Câu Hỏi Thường Gặp

### Q: Tại sao phải dùng con trỏ?
**A**: Vì Qt cần:
- Chia sẻ model giữa nhiều views
- Đa hình (polymorphism) với base class pointers
- Quản lý bộ nhớ tự động với parent-child
- Hiệu năng (không copy object lớn)

### Q: Khi nào phải `delete`?
**A**: 
- ✅ Có parent → KHÔNG cần delete
- ❌ Không parent → PHẢI delete thủ công
- ✅ Dùng smart pointer → Tự động delete

### Q: `view->setModel()` có copy model không?
**A**: **KHÔNG!** View chỉ LƯU CON TRỎ (địa chỉ) của model.
- Nhiều views có thể trỏ đến cùng 1 model
- Thay đổi model → tất cả views cập nhật

### Q: QPointer vs Raw Pointer?
**A**:
- **Raw pointer (`T*`)**: Nhanh, nhưng nguy hiểm nếu object bị xóa
- **QPointer**: An toàn, tự động = nullptr khi object bị xóa

---

## 🎯 Checklist Hoàn Thành Quick Start

Đánh dấu ✅ khi hoàn thành:

- [ ] Hiểu Model = dữ liệu, View = hiển thị, Delegate = tùy chỉnh
- [ ] Biết tạo model với parent: `new Model(this)`
- [ ] Hiểu `view->setModel()` lưu con trỏ, không copy
- [ ] Biết null check: `if (ptr != nullptr)`
- [ ] Chạy được `simple_model_example`
- [ ] Đọc xong 1 trong 3 file code example

**Hoàn thành checklist? → Bạn đã sẵn sàng học tiếp! 🚀**

---

## 📞 Cần Giúp Đỡ?

- **Lỗi compile**: Xem phần "Lỗi Thường Gặp" trong README.md
- **Chưa hiểu**: Đọc lại HUONG_DAN_QT_MVC.md phần cơ bản
- **Code không chạy**: Kiểm tra đã cài Qt đúng version chưa

---

**Giờ bắt đầu với `simple_model_example.cpp`! 💪**

*Chỉ mất 3 phút để chạy ví dụ đầu tiên!*
