# Con Trỏ (Pointer) trong C++ - Hướng Dẫn Toàn Diện

## 📌 Mục Đích Sử Dụng Con Trỏ

Con trỏ trong C++ được sử dụng để:

1. **Quản lý bộ nhớ động** - Tạo và quản lý đối tượng trên heap
2. **Truyền đối tượng lớn hiệu quả** - Tránh copy dữ liệu lớn khi truyền vào hàm
3. **Chia sẻ dữ liệu** - Nhiều biến có thể trỏ đến cùng một đối tượng
4. **Polymorphism** - Làm việc với đối tượng thông qua base class pointer
5. **Quản lý tài nguyên** - Quản lý vòng đời của đối tượng (đặc biệt trong Qt)

## 🔑 Các Khái Niệm Cốt Lõi

### 1. Con Trỏ Là Gì?

Con trỏ là một biến lưu **địa chỉ bộ nhớ** của một biến khác.

```cpp
int x = 10;        // Biến thông thường
int* ptr = &x;     // Con trỏ lưu địa chỉ của x
// ptr chứa địa chỉ, *ptr là giá trị tại địa chỉ đó
```

### 2. Các Toán Tử Quan Trọng

- `&` - Lấy địa chỉ (address-of operator)
- `*` - Truy cập giá trị tại địa chỉ (dereference operator)
- `->` - Truy cập thành viên qua con trỏ (thay cho `(*ptr).member`)

```cpp
int value = 42;
int* ptr = &value;     // ptr trỏ đến value

cout << ptr;           // In địa chỉ (ví dụ: 0x7fff5fbff6ac)
cout << *ptr;          // In giá trị: 42
cout << &value;        // In địa chỉ của value

// Với đối tượng
MyClass obj;
MyClass* objPtr = &obj;
objPtr->method();      // Gọi method qua con trỏ
(*objPtr).method();    // Cách viết tương đương
```

### 3. Con Trỏ NULL/nullptr

Con trỏ chưa được khởi tạo hoặc không trỏ đến đâu nên được gán `nullptr`:

```cpp
int* ptr = nullptr;    // C++11 trở đi
int* ptr2 = NULL;      // Cách cũ (tránh dùng)

// Luôn kiểm tra trước khi dùng
if (ptr != nullptr) {
    *ptr = 10;
}
```

## 🆚 Con Trỏ vs Tham Chiếu (Reference)

### Bảng So Sánh

| Đặc điểm | Con Trỏ (Pointer) | Tham Chiếu (Reference) |
|----------|-------------------|------------------------|
| **Ký hiệu** | `int* ptr` | `int& ref` |
| **Có thể NULL** | ✅ Có | ❌ Không (phải khởi tạo ngay) |
| **Có thể đổi địa chỉ** | ✅ Có (ptr = &other) | ❌ Không (gắn chặt với biến) |
| **Cần dereference** | ✅ Cần `*ptr` | ❌ Không (dùng như biến thường) |
| **Có thể trỏ đến mảng** | ✅ Có | ❌ Không |
| **Có thể delete** | ✅ Có | ❌ Không |

### Ví Dụ So Sánh

```cpp
int value = 10;

// CON TRỎ
int* ptr = &value;     // Có thể = nullptr
ptr = nullptr;         // Có thể đổi
int x = *ptr;          // Cần dereference

// THAM CHIẾU
int& ref = value;      // Phải khởi tạo ngay, không thể = nullptr
// ref = nullptr;      // LỖI!
int y = ref;           // Dùng như biến thường, không cần *
```

### Khi Nào Dùng Con Trỏ? Khi Nào Dùng Tham Chiếu?

**Dùng THAM CHIẾU khi:**
- Muốn đảm bảo luôn có giá trị hợp lệ
- Muốn cú pháp đơn giản hơn
- Truyền vào hàm mà không muốn copy
- Không cần thay đổi địa chỉ trỏ đến

```cpp
void processData(int& data) {  // Tham chiếu
    data = 100;  // Thay đổi trực tiếp, không cần *
}
```

**Dùng CON TRỎ khi:**
- Cần khả năng NULL (optional parameter)
- Cần thay đổi địa chỉ trỏ đến
- Quản lý bộ nhớ động (new/delete)
- Làm việc với mảng động
- Polymorphism với base class
- Qt objects (parent-child relationship)

```cpp
void processData(int* data) {  // Con trỏ
    if (data != nullptr) {     // Có thể NULL
        *data = 100;
    }
}
```

## 🎯 Con Trỏ Trong Qt - Tại Sao Qt Dùng Nhiều Con Trỏ?

### 1. Parent-Child Relationship

Qt sử dụng con trỏ để quản lý vòng đời đối tượng:

```cpp
QWidget* parent = new QWidget();
QPushButton* button = new QPushButton("Click", parent);
// Khi parent bị xóa, button tự động bị xóa
// Không cần delete button thủ công!
```

### 2. Qt Model/View Architecture

```cpp
// Trong Qt Model
QStandardItemModel* model = new QStandardItemModel(this);

QStandardItem* item = new QStandardItem("Data");
model->appendRow(item);  // Model sở hữu item qua con trỏ

// View sử dụng model qua con trỏ
QTreeView* view = new QTreeView(this);
view->setModel(model);   // Truyền con trỏ, không copy model
```

### 3. Signal-Slot Mechanism

```cpp
// Signal-Slot cần con trỏ để kết nối
connect(button, &QPushButton::clicked, 
        this, &MyClass::onButtonClicked);
// button và this là con trỏ
```

### 4. Polymorphism với QObject

```cpp
QObject* obj = new QPushButton();  // Base class pointer
// Có thể trỏ đến nhiều loại QObject khác nhau
```

## 📚 Các Trường Hợp Sử Dụng Phổ Biến

### 1. Quản Lý Bộ Nhớ Động

```cpp
// Tạo đối tượng trên heap
int* arr = new int[100];
// ... sử dụng
delete[] arr;  // Nhớ giải phóng!

// C++11: Dùng smart pointer (khuyến nghị)
std::unique_ptr<int[]> arr(new int[100]);
// Tự động giải phóng khi ra khỏi scope
```

### 2. Truyền Đối Tượng Lớn Vào Hàm

```cpp
struct LargeData {
    int data[10000];
};

// KHÔNG hiệu quả - copy toàn bộ dữ liệu
void process1(LargeData data) { }

// HIỆU QUẢ - chỉ truyền địa chỉ (8 bytes)
void process2(LargeData* data) { }
void process3(LargeData& data) { }  // Tham chiếu cũng tốt
```

### 3. Trả Về Nhiều Giá Trị

```cpp
bool getValues(int* out1, int* out2) {
    if (someCondition) {
        *out1 = 10;
        *out2 = 20;
        return true;
    }
    return false;
}

int val1, val2;
if (getValues(&val1, &val2)) {
    // Sử dụng val1, val2
}
```

### 4. Mảng Động

```cpp
int size = 100;
int* dynamicArray = new int[size];
// ... sử dụng
delete[] dynamicArray;
```

### 5. Polymorphism

```cpp
class Animal {
public:
    virtual void makeSound() = 0;
};

class Dog : public Animal {
    void makeSound() override { cout << "Woof!"; }
};

class Cat : public Animal {
    void makeSound() override { cout << "Meow!"; }
};

// Con trỏ cho phép polymorphism
Animal* pet = new Dog();
pet->makeSound();  // "Woof!"
pet = new Cat();
pet->makeSound();  // "Meow!"
```

## ⚠️ Lỗi Thường Gặp và Cách Tránh

### 1. Con Trỏ Chưa Khởi Tạo

```cpp
int* ptr;        // NGUY HIỂM!
*ptr = 10;       // LỖI! ptr chứa giá trị rác

// ĐÚNG:
int* ptr = nullptr;
if (ptr != nullptr) {
    *ptr = 10;
}
```

### 2. Memory Leak (Rò Rỉ Bộ Nhớ)

```cpp
void badFunction() {
    int* ptr = new int(10);
    // Quên delete ptr
    // Bộ nhớ bị rò rỉ!
}

// ĐÚNG:
void goodFunction() {
    int* ptr = new int(10);
    // ... sử dụng
    delete ptr;  // Giải phóng
}

// TỐT NHẤT: Dùng smart pointer
void bestFunction() {
    std::unique_ptr<int> ptr(new int(10));
    // Tự động giải phóng
}
```

### 3. Double Delete

```cpp
int* ptr = new int(10);
delete ptr;
delete ptr;  // LỖI! Xóa 2 lần

// ĐÚNG:
delete ptr;
ptr = nullptr;  // Gán nullptr sau khi delete
```

### 4. Dangling Pointer

```cpp
int* ptr = new int(10);
delete ptr;
*ptr = 20;  // LỖI! ptr trỏ đến vùng nhớ đã bị giải phóng

// ĐÚNG:
delete ptr;
ptr = nullptr;  // Đánh dấu không còn hợp lệ
```

### 5. Nhầm Lẫn Con Trỏ và Tham Chiếu

```cpp
int value = 10;

// CON TRỎ
int* ptr = &value;
int x = *ptr;        // Cần * để lấy giá trị

// THAM CHIẾU
int& ref = value;
int y = ref;         // Không cần *, dùng như biến thường
```

## 💡 Best Practices

### 1. Luôn Khởi Tạo Con Trỏ

```cpp
int* ptr = nullptr;  // Luôn khởi tạo
```

### 2. Kiểm Tra NULL Trước Khi Dùng

```cpp
if (ptr != nullptr) {
    *ptr = 10;
}
```

### 3. Sử Dụng Smart Pointers (C++11+)

```cpp
// unique_ptr - sở hữu duy nhất
std::unique_ptr<int> ptr(new int(10));

// shared_ptr - chia sẻ quyền sở hữu
std::shared_ptr<int> ptr2 = std::make_shared<int>(10);

// weak_ptr - tham chiếu yếu (không tăng reference count)
std::weak_ptr<int> weak = ptr2;
```

### 4. Trong Qt: Để Qt Quản Lý Vòng Đời

```cpp
// Qt tự động xóa children khi parent bị xóa
QWidget* parent = new QWidget();
QPushButton* button = new QPushButton(parent);
// Không cần delete button!

// Hoặc dùng QScopedPointer
QScopedPointer<QWidget> widget(new QWidget());
```

### 5. Ưu Tiên Tham Chiếu Khi Có Thể

```cpp
// Nếu không cần NULL, dùng tham chiếu
void process(int& data) { }  // Đơn giản hơn

// Chỉ dùng con trỏ khi thực sự cần
void process(int* data) {    // Khi cần optional
    if (data) { /* ... */ }
}
```

## 🎓 Tóm Tắt - Cách Học Nhanh

1. **Hiểu cơ bản**: Con trỏ = địa chỉ bộ nhớ
2. **Nhớ các toán tử**: `&` (lấy địa chỉ), `*` (lấy giá trị), `->` (truy cập thành viên)
3. **Phân biệt rõ**: Con trỏ có thể NULL, tham chiếu không
4. **Qt context**: Qt dùng con trỏ cho parent-child và model-view
5. **Thực hành**: Viết code nhỏ để hiểu rõ
6. **Dùng smart pointer**: Tránh memory leak

## 📝 Ví Dụ Thực Tế - Qt Model

```cpp
class MyModel : public QAbstractItemModel {
    Q_OBJECT
public:
    MyModel(QObject* parent = nullptr) : QAbstractItemModel(parent) {
        // parent là con trỏ - có thể nullptr
    }
    
    void addItem(const QString& text) {
        // Tạo item mới trên heap
        QStandardItem* item = new QStandardItem(text);
        // Thêm vào model (model sở hữu item)
        m_items.append(item);
    }
    
private:
    QList<QStandardItem*> m_items;  // Danh sách con trỏ
};

// Sử dụng
MyModel* model = new MyModel(this);  // this là con trỏ QObject*
QTreeView* view = new QTreeView(this);
view->setModel(model);  // Truyền con trỏ model
```

## 🔍 Checklist Khi Làm Việc Với Con Trỏ

- [ ] Con trỏ đã được khởi tạo chưa? (nên = nullptr)
- [ ] Đã kiểm tra NULL trước khi dùng chưa?
- [ ] Đã giải phóng bộ nhớ (delete) chưa?
- [ ] Có bị double delete không?
- [ ] Có thể dùng tham chiếu thay thế không?
- [ ] Trong Qt: đã set parent chưa? (để Qt tự quản lý)

---

**Lưu ý**: Trong Qt, hầu hết các đối tượng đều được quản lý qua parent-child relationship, nên bạn không cần lo lắng về việc delete thủ công trong nhiều trường hợp. Qt sẽ tự động giải phóng khi parent bị hủy.
