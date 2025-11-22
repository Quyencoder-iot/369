# Hướng dẫn Qt Models & Views (C++)

Dự án này là phiên bản C++ của tutorial về Qt Models/Views.

## Cấu trúc dự án

- **CMakeLists.txt**: File cấu hình để build dự án với CMake và Qt6.
- **main.cpp**: Điểm bắt đầu của ứng dụng, tạo cửa sổ chính và các tab ví dụ.
- **custom_list_model.h/cpp**: Ví dụ về `QAbstractListModel`.
- **custom_table_model.h/cpp**: Ví dụ về `QAbstractTableModel`.
- **custom_tree_model.h/cpp**: Ví dụ về `QAbstractItemModel` (Tree).

## Cách build và chạy

Để chạy dự án này, bạn cần cài đặt Qt6 và CMake.

1.  Tạo thư mục build:
    ```bash
    mkdir build
    cd build
    ```

2.  Chạy CMake:
    ```bash
    cmake ..
    ```
    *(Nếu CMake không tìm thấy Qt, bạn có thể cần chỉ định đường dẫn, ví dụ: `cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..`)*

3.  Compile:
    ```bash
    make
    ```

4.  Chạy ứng dụng:
    ```bash
    ./QtModelsTutorial
    ```

## Điểm khác biệt so với Python (PySide6)

### 1. Quản lý bộ nhớ
- Trong C++, bạn cần quản lý con trỏ. Tuy nhiên, Qt sử dụng cơ chế **Parent-Child ownership**. Khi bạn tạo một object (`new Object(parent)`), `parent` sẽ tự động xóa `child` khi nó bị hủy.
- Ví dụ: `new CustomListModel(data, tab)` -> `tab` (là một QWidget) sẽ chịu trách nhiệm xóa `CustomListModel`.

### 2. Hàm `data()` trả về `QVariant`
- Trong Python, bạn có thể trả về chuỗi, số, hoặc `None`.
- Trong C++, bạn phải trả về `QVariant`. Nếu không có dữ liệu, trả về `QVariant()` (invalid variant).

### 3. Macro `Q_OBJECT`
- Bắt buộc phải có macro `Q_OBJECT` trong khai báo class (trong file .h) để hệ thống Signal/Slot và Meta-Object của Qt hoạt động.

### 4. Header files
- C++ tách biệt khai báo (header `.h`) và định nghĩa (source `.cpp`).
- `CustomTableModel` trong ví dụ này sử dụng `struct PersonData` để định nghĩa cấu trúc dữ liệu thay vì list of list như Python.

## Mối liên hệ (Vẫn giữ nguyên logic)

Dù là C++ hay Python, logic cốt lõi không đổi:
1.  **View gọi `rowCount()`**: "Có bao nhiêu hàng?"
2.  **View gọi `data(index, role)`**: "Dữ liệu ở index này là gì?"
3.  **Model trả về `QVariant`**.
