# Ví Dụ Thực Tế: Qt Model-View-Delegate với Con Trỏ

## Cách Chạy Các Ví Dụ

### Yêu Cầu
- Qt6 (hoặc Qt5)
- CMake 3.16+
- C++17 compiler

### Cách 1: Dùng CMake

```bash
cd examples
mkdir build
cd build
cmake ..
make

# Chạy các ví dụ
./basic_example
./delegate_example
./multi_view_example
```

### Cách 2: Compile Thủ Công

#### Basic Example
```bash
g++ -fPIC -std=c++17 basic_example.cpp -o basic_example \
    $(pkg-config --cflags --libs Qt6Core Qt6Widgets)
./basic_example
```

#### Delegate Example
```bash
g++ -fPIC -std=c++17 delegate_example.cpp -o delegate_example \
    $(pkg-config --cflags --libs Qt6Core Qt6Widgets)
./delegate_example
```

#### Multi-View Example
```bash
g++ -fPIC -std=c++17 multi_view_example.cpp -o multi_view_example \
    $(pkg-config --cflags --libs Qt6Core Qt6Widgets)
./multi_view_example
```

## Mô Tả Các Ví Dụ

### 1. basic_example.cpp
- **Mục đích**: Hiểu cách Model và View được tạo với parent-child relationship
- **Học được**: 
  - Cách tạo Model và View với parent
  - Cách setModel() hoạt động
  - Cách index được sử dụng

### 2. delegate_example.cpp
- **Mục đích**: Hiểu cách Delegate được quản lý bởi View
- **Học được**:
  - Cách tạo custom delegate
  - Cách delegate được view sở hữu
  - Cách editor widget được tạo và quản lý

### 3. multi_view_example.cpp
- **Mục đích**: Hiểu cách nhiều View có thể share 1 Model
- **Học được**:
  - Model không bị View sở hữu
  - Khi model thay đổi, tất cả view tự động cập nhật
  - Ownership của Model vs View

## Bài Tập Thực Hành

1. **Sửa basic_example.cpp**:
   - Thêm nút "Xóa dòng" để xóa dòng được chọn
   - Thêm nút "Thêm dòng" để thêm dòng mới

2. **Sửa delegate_example.cpp**:
   - Thêm delegate cho cột "Tên" với QLineEdit
   - Validate input (ví dụ: tên không được rỗng)

3. **Sửa multi_view_example.cpp**:
   - Thêm QColumnView vào danh sách view
   - Khi edit ở một view, highlight ở view khác

## Debug Tips

Để hiểu rõ hơn về con trỏ và ownership:

1. **Thêm breakpoint** trong destructor của các class
2. **Xem object tree** trong Qt Creator
3. **Dùng Valgrind** để kiểm tra memory leaks:
   ```bash
   valgrind --leak-check=full ./basic_example
   ```

## Lưu Ý

- Tất cả ví dụ đều có parent-child relationship đúng
- Không có memory leaks nếu compile và chạy đúng
- Có thể thêm `qDebug()` để theo dõi lifecycle của objects
