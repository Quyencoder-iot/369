# Qt Model-View-Delegate Pointer Tutorial

Đây là một tutorial chi tiết về cách quản lý con trỏ trong Qt Model-View-Delegate framework, được thiết kế để giúp bạn học nhanh và hiểu sâu về chủ đề này.

## Tổng Quan

Project này bao gồm:
- Hướng dẫn lý thuyết chi tiết (tiếng Việt)
- Các ví dụ code minh họa
- Ứng dụng demo đầy đủ với GUI

## Cấu Trúc Project

```
qt-model-view-tutorial/
├── docs/                    # Tài liệu hướng dẫn
│   └── huong-dan-con-tro-qt-mvc.md
├── examples/               # Các ví dụ độc lập
│   ├── model_pointer_example.cpp
│   ├── view_pointer_example.cpp
│   └── delegate_pointer_example.cpp
├── src/                    # Source code ứng dụng demo
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── mainwindow.cpp/h
│   ├── custom_model.cpp/h
│   ├── custom_delegate.cpp/h
│   └── pointer_manager.cpp/h
└── README.md
```

## Cách Build và Chạy

### Yêu cầu
- Qt 5.12+ hoặc Qt 6.x
- CMake 3.16+
- C++17 compiler

### Build Demo Application

```bash
# Tạo thư mục build
mkdir build
cd build

# Configure với CMake
cmake ../src

# Build
make -j4  # Linux/Mac
# hoặc
nmake     # Windows với MSVC
```

### Chạy Demo

```bash
./model_view_demo
```

### Build Examples (không bắt buộc)

Các file example có thể build riêng lẻ:

```bash
cd examples

# Build model example
qmake -project "QT += widgets" model_pointer_example.cpp
qmake
make

# Tương tự cho các examples khác
```

## Nội Dung Học

### 1. Đọc Hướng Dẫn Lý Thuyết
Bắt đầu với file `docs/huong-dan-con-tro-qt-mvc.md` để hiểu:
- Khái niệm cơ bản về Model-View-Delegate
- Quy tắc ownership trong Qt
- Các pattern phổ biến
- Lỗi thường gặp và cách tránh

### 2. Nghiên Cứu Examples
Mỗi file example tập trung vào một khía cạnh:
- `model_pointer_example.cpp`: Quản lý Model và Items
- `view_pointer_example.cpp`: Multiple views, selection models
- `delegate_pointer_example.cpp`: Custom delegates và editors

### 3. Chạy Demo Application
Demo app có 4 tabs:
- **Model Pointers**: Demo ownership, sharing, item management
- **View Pointers**: View-model relation, components
- **Delegate Pointers**: Lifecycle, custom editors
- **Memory Management**: Best practices, common leaks

## Tips Học Nhanh

1. **Chạy demo và xem console output**: Mọi operation đều được log chi tiết
2. **Thử nghiệm với các buttons**: Mỗi button demo một scenario khác nhau
3. **Đọc code comments**: Code được comment chi tiết bằng tiếng Việt
4. **Debug với breakpoints**: Đặt breakpoints để hiểu flow

## Các Nguyên Tắc Quan Trọng Cần Nhớ

### 1. Ownership Rules
- View KHÔNG sở hữu Model
- Model sở hữu Items sau khi add
- Parent sở hữu Children trong Qt

### 2. Safe Practices
- Luôn check nullptr trước khi dùng
- Dùng QPointer cho safety
- Clear model từ view trước khi delete

### 3. Memory Management
- Set parent khi possible
- Dùng smart pointers cho RAII
- Track object lifecycle trong development

## Debug và Troubleshooting

### Memory Leaks
```bash
# Linux: dùng valgrind
valgrind --leak-check=full ./model_view_demo

# Hoặc compile với address sanitizer
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" ../src
```

### Console Output
Demo app in ra nhiều thông tin debug. Có thể filter:
```bash
./model_view_demo 2>&1 | grep "Model\|View\|Delegate"
```

## Mở Rộng

Sau khi nắm vững basics, có thể:
1. Viết custom model kế thừa QAbstractItemModel
2. Implement drag & drop
3. Tạo custom delegates phức tạp hơn
4. Tìm hiểu QML với model/view

## Resources

- [Qt Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html)
- [Qt Model/View Tutorial](https://doc.qt.io/qt-6/modelview.html)
- Source code examples trong Qt Creator

## License

This tutorial is provided as-is for educational purposes.

---

Happy Learning! 🚀