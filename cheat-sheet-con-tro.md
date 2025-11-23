# Cheat Sheet - Con Trỏ trong C++

## 🔑 Cú Pháp Cơ Bản

```cpp
int value = 10;
int* ptr = &value;      // Con trỏ
int& ref = value;       // Tham chiếu

*ptr = 20;              // Thay đổi qua con trỏ (cần *)
ref = 20;               // Thay đổi qua tham chiếu (không cần *)
```

## 📊 So Sánh Nhanh

| | Con Trỏ `int*` | Tham Chiếu `int&` |
|---|---|---|
| **NULL được không?** | ✅ Có | ❌ Không |
| **Cần `*` để dùng?** | ✅ Có | ❌ Không |
| **Có thể đổi địa chỉ?** | ✅ Có | ❌ Không |
| **Khi nào dùng?** | Optional, dynamic memory, Qt | Luôn có giá trị |

## 🎯 Khi Nào Dùng Gì?

### Dùng **THAM CHIẾU** khi:
- ✅ Đảm bảo luôn có giá trị hợp lệ
- ✅ Muốn cú pháp đơn giản
- ✅ Truyền vào hàm (tránh copy)

```cpp
void func(int& x) { x = 100; }  // Đơn giản
```

### Dùng **CON TRỎ** khi:
- ✅ Cần NULL (optional parameter)
- ✅ Quản lý bộ nhớ động (`new`/`delete`)
- ✅ Qt objects (parent-child)
- ✅ Polymorphism
- ✅ Mảng động

```cpp
void func(int* x) {
    if (x) *x = 100;  // Có thể NULL
}
```

## ⚠️ Lỗi Thường Gặp

```cpp
// ❌ SAI: Chưa khởi tạo
int* ptr;
*ptr = 10;  // LỖI!

// ✅ ĐÚNG:
int* ptr = nullptr;
if (ptr) *ptr = 10;
```

```cpp
// ❌ SAI: Quên delete
int* arr = new int[10];
// Memory leak!

// ✅ ĐÚNG:
int* arr = new int[10];
delete[] arr;
arr = nullptr;
```

```cpp
// ❌ SAI: Double delete
delete ptr;
delete ptr;  // LỖI!

// ✅ ĐÚNG:
delete ptr;
ptr = nullptr;
```

## 🎨 Qt Pattern

```cpp
// Parent-child: Qt tự động xóa children
QWidget* parent = new QWidget();
QPushButton* btn = new QPushButton(parent);
delete parent;  // btn tự động bị xóa!

// Model-View: Truyền con trỏ
QStandardItemModel* model = new QStandardItemModel(this);
view->setModel(model);  // Con trỏ, không copy
```

## 💡 Best Practices

1. **Luôn khởi tạo**: `int* ptr = nullptr;`
2. **Kiểm tra NULL**: `if (ptr != nullptr) { ... }`
3. **Dùng smart pointer**: `std::unique_ptr<int> ptr(new int(10));`
4. **Trong Qt**: Để Qt quản lý (set parent)
5. **Gán nullptr sau delete**: `delete ptr; ptr = nullptr;`

## 🔄 Nhớ Nhanh

- **Con trỏ** = địa chỉ bộ nhớ, có thể NULL, cần `*`
- **Tham chiếu** = alias, không thể NULL, dùng như biến thường
- **Qt** = dùng con trỏ cho parent-child và model-view
- **Smart pointer** = tự động quản lý bộ nhớ (C++11+)
