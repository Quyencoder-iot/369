# Hướng dẫn học nhanh Qt Models & Views (PySide6)

Dự án này được tạo ra để giúp bạn hiểu nhanh về kiến trúc Model/View trong Qt thông qua code ví dụ thực tế.

## Cách chạy chương trình

1.  Cài đặt môi trường (nếu chưa có):
    ```bash
    pip install -r requirements.txt
    ```
2.  Chạy ứng dụng:
    ```bash
    python main.py
    ```

## Bản đồ tư duy (Mental Map)

Trong Qt, kiến trúc Model/View tách biệt **Dữ liệu** (Model) khỏi **Giao diện** (View).

1.  **Model (Người giữ dữ liệu)**: Chịu trách nhiệm cung cấp dữ liệu cho View.
2.  **View (Người hiển thị)**: Hỏi Model "Dữ liệu ở hàng X, cột Y là gì?" và hiển thị nó.
3.  **Delegate (Người vẽ)**: (Nâng cao) Kiểm soát cách vẽ từng ô (item) cụ thể.

## Cách đọc code Model nhanh chóng

Khi bạn đọc code của một Model bất kỳ (`List`, `Table`, hay `Tree`), hãy tìm ngay các hàm sau. Chúng là "trái tim" của Model:

### 1. `rowCount()` và `columnCount()`
Model phải nói cho View biết nó có bao nhiêu hàng và cột.
- Nếu là `ListModel`, `columnCount` thường là 1 (mặc định).

### 2. `data(index, role)` (QUAN TRỌNG NHẤT)
Đây là hàm View gọi liên tục để lấy dữ liệu.
- **`index`**: Vị trí (hàng, cột).
- **`role`**: Loại dữ liệu View cần.
    - `Qt.DisplayRole`: Text hiển thị.
    - `Qt.DecorationRole`: Icon hoặc ảnh.
    - `Qt.BackgroundRole`: Màu nền.
    
**Mẹo đọc code**: Tìm dòng `if role == Qt.DisplayRole:` để biết dữ liệu chính là gì.

## Các loại Model trong ví dụ

### 1. List Model (`custom_list_model.py`)
- Kế thừa từ: `QAbstractListModel`.
- Dùng cho: Danh sách đơn giản (1 cột).
- Ví dụ: Danh sách bài hát, danh sách file.

### 2. Table Model (`custom_table_model.py`)
- Kế thừa từ: `QAbstractTableModel`.
- Dùng cho: Dữ liệu dạng bảng (nhiều cột).
- Ví dụ: Bảng nhân viên, bảng excel.

### 3. Tree Model (`custom_tree_model.py`)
- Kế thừa từ: `QAbstractItemModel`.
- Phức tạp nhất vì phải xử lý quan hệ cha-con (`parent()`, `index()`).
- Dùng cho: Cấu trúc thư mục, XML, JSON.

### 4. Proxy Model (`main.py`)
- Là lớp trung gian.
- **Source Model** (Dữ liệu gốc) -> **Proxy Model** (Lọc/Sắp xếp) -> **View**.
- Bạn không cần viết lại Model để có tính năng lọc/sắp xếp.

### 5. SQL Model (`sql_model.py`)
- `QSqlTableModel`: Là một `TableModel` đã được viết sẵn để làm việc với Database.
- Bạn hiếm khi phải override `data()` vì nó tự lấy từ DB.
- Mối liên hệ: `QSqlTableModel` IS-A `QAbstractTableModel`. Nó hoạt động y hệt Table Model tự viết, nhưng dữ liệu đến từ SQL thay vì mảng Python.

## Mối liên hệ tổng quát

```mermaid
graph TD
    A[QAbstractItemModel] --> B[QAbstractListModel]
    A --> C[QAbstractTableModel]
    C --> D[QSqlTableModel]
    A --> E[QSortFilterProxyModel]
    
    F[View (List/Table/Tree)] -->|Hỏi data()| A
```

Hãy bắt đầu bằng cách đọc `custom_list_model.py`, nó đơn giản nhất!
