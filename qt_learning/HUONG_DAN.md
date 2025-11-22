# Hướng dẫn đọc code Qt Models nhanh chóng

## 1. Bản đồ kiến trúc (The Big Picture)
Hãy tưởng tượng kiến trúc Model/View như một nhà hàng:

*   **Data Source (Database/File/Array):** Kho nguyên liệu.
*   **Model (`QAbstractItemModel`):** Đầu bếp. Người duy nhất được phép chạm vào nguyên liệu và chế biến món ăn (Rows/Cols).
*   **Proxy Model (`QSortFilterProxyModel`):** Người kiểm duyệt/sắp xếp. Chỉ cho món nào ra bàn (Filter) hoặc sắp xếp lại thứ tự đĩa (Sort).
*   **View (`QListView`, `QTableView`):** Bàn ăn/Khách hàng. Chỉ hiển thị những gì Model/Proxy đưa ra.

## 2. Các loại Model thường gặp

| Tên Class | Đặc điểm nhận dạng | Dùng khi nào? |
| :--- | :--- | :--- |
| **`QAbstractListModel`** | Hàm `rowCount` | Dữ liệu dạng danh sách dọc (1 cột). |
| **`QAbstractTableModel`** | Hàm `rowCount` + `columnCount` | Dữ liệu dạng bảng (nhiều cột). |
| **`QStandardItemModel`** | Code set item bằng `setItem()` | Dữ liệu nhỏ, đơn giản, không muốn tạo class kế thừa. |
| **`QSqlTableModel`** | Có `setTable("tableName")` | Làm việc trực tiếp với 1 bảng trong Database. |
| **`QSqlQueryModel`** | Có `setQuery("SELECT...")` | Chỉ đọc dữ liệu từ SQL phức tạp. |

## 3. Mẹo đọc code nhanh (Scan Strategy)

Khi bạn mở một file Model `.cpp` hoặc `.h`, hãy quét mắt theo thứ tự sau:

### Bước 1: Xem nó kế thừa từ ai?
*   Nhìn dòng `class X : public Y`.
*   Nếu `Y` là `QAbstractTableModel` -> Chuẩn bị tinh thần đọc logic bảng.
*   Nếu `Y` là `QSortFilterProxyModel` -> Đây chỉ là bộ lọc, tìm logic lọc ở `filterAcceptsRow`.

### Bước 2: Tìm hàm `data()` (Quan trọng nhất)
*   Đây là trái tim của Model.
*   View sẽ gọi hàm này liên tục: "Ô hàng 1, cột 2 hiển thị cái gì?".
*   Đọc `switch(role)` và `switch(index.column())` để biết cột nào hiển thị dữ liệu gì.

### Bước 3: Tìm `roleNames()` (Nếu code có dính tới QML)
*   C++ giao tiếp với QML qua `roleNames`.
*   Ví dụ: `roles[NameRole] = "name"`. Trong QML sẽ dùng `text: model.name`.

### Bước 4: Tìm `setSourceModel()` (Nếu là Proxy)
*   Nếu thấy dòng này, hãy tìm xem "Source" thực sự là biến nào. Đó mới là nơi chứa dữ liệu thật.

## 4. Mối liên hệ giữa các thành phần

Code thường được tổ chức theo chuỗi:

```
[Database/Vector] <-> [Model] <-> [Proxy (Optional)] <-> [View]
```

1.  **List/Table/Tree Model:** Là các lớp cơ sở (Base classes).
2.  **QSql:** Là implementation (hiện thực) cụ thể của Table Model dành cho Database.
3.  **Proxy:** Là lớp bọc (Wrapper). Nó **không chứa dữ liệu**, nó chỉ trỏ đến Model thật.

### Check-list khi debug/đọc code:
*   **Mất dữ liệu?** Kiểm tra `rowCount()` của Model gốc.
*   **Dữ liệu sai?** Kiểm tra hàm `data()` của Model gốc.
*   **Không filter được?** Kiểm tra `filterAcceptsRow()` của Proxy.
*   **View trắng trơn?** Kiểm tra xem đã gọi `setModel()` chưa, hoặc Database đã `open()` chưa.
