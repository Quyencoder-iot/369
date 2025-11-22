# Lộ Trình Học Qt Model/View

## 🎯 Mục Tiêu
Hiểu và đọc code nhanh các loại Model trong Qt: List, Table, Tree, Proxy, và QSQL.

## 📚 Thứ Tự Học (Từ Dễ Đến Khó)

### Bước 1: Hiểu Kiến Trúc Model/View (30 phút)
**Đọc:** `README.md` - Phần "Tổng Quan Kiến Trúc Model/View"

**Hiểu:**
- Model = Dữ liệu
- View = Hiển thị
- Tách biệt để tái sử dụng

**Thực hành:** Xem sơ đồ trong `DIAGRAM.md`

---

### Bước 2: QAbstractListModel - Đơn Giản Nhất (1 giờ)
**Đọc:** `examples/01_list_model_example.cpp`

**Tập trung vào:**
- `rowCount()` - Trả về số item
- `data()` - Trả về dữ liệu tại index
- Chỉ có 1 chiều (hàng), không có cột

**Thực hành:**
1. Đọc code ví dụ
2. Tìm các hàm `rowCount()` và `data()`
3. Hiểu cách View gọi `data()` để lấy dữ liệu

**Checklist:**
- [ ] Hiểu tại sao chỉ cần `rowCount()` và `data()`
- [ ] Hiểu parameter `role` trong `data()`
- [ ] Biết khi nào View gọi `data()`

---

### Bước 3: QAbstractTableModel - Thêm Cột (1 giờ)
**Đọc:** `examples/02_table_model_example.cpp`

**Tập trung vào:**
- `rowCount()` - Số hàng
- `columnCount()` - Số cột (KHÁC với List Model)
- `data()` - Dữ liệu tại (row, column)
- `headerData()` - Tên cột

**So sánh với List Model:**
- List: 1 chiều (chỉ hàng)
- Table: 2 chiều (hàng × cột)

**Thực hành:**
1. So sánh với ví dụ List Model
2. Tìm sự khác biệt trong `data()` (có `index.column()`)
3. Hiểu `headerData()` dùng để làm gì

**Checklist:**
- [ ] Hiểu sự khác biệt giữa List và Table Model
- [ ] Biết cách dùng `index.column()` trong `data()`
- [ ] Hiểu `headerData()` trả về tên cột

---

### Bước 4: QAbstractItemModel (Tree) - Phức Tạp Nhất (2 giờ)
**Đọc:** `examples/03_tree_model_example.cpp`

**Tập trung vào:**
- `rowCount(parent)` - Số con của parent
- `index(row, column, parent)` - Tạo index cho child
- `parent(index)` - Lấy parent của index
- Cấu trúc phân cấp (parent/child)

**Khó khăn:**
- Phải hiểu `index()` và `parent()`
- Phải quản lý cấu trúc cây

**Thực hành:**
1. Vẽ cây trên giấy
2. Trace code: `index()` và `parent()` với từng node
3. Hiểu invalid index = root

**Checklist:**
- [ ] Hiểu cách `index()` tạo index cho child
- [ ] Hiểu cách `parent()` lấy parent
- [ ] Biết invalid index nghĩa là gì

---

### Bước 5: QAbstractProxyModel - Lọc và Sắp Xếp (1 giờ)
**Đọc:** `examples/04_proxy_model_example.cpp`

**Tập trung vào:**
- `setSourceModel()` - Kết nối với model gốc
- Proxy KHÔNG chứa dữ liệu, chỉ ánh xạ index
- Luồng: Source Model → Proxy → View

**Quan trọng:**
- View phải kết nối với Proxy, KHÔNG phải Source Model
- Proxy có thể xếp chồng (chain)

**Thực hành:**
1. Tìm `setSourceModel()` trong code
2. Tìm `view->setModel()` - xem dùng proxy hay source?
3. Hiểu luồng dữ liệu

**Checklist:**
- [ ] Hiểu Proxy không chứa dữ liệu
- [ ] Biết View phải dùng Proxy, không dùng Source
- [ ] Hiểu có thể chain nhiều Proxy

---

### Bước 6: QSQL Models - Database (1.5 giờ)
**Đọc:** `examples/05_qsql_model_example.cpp`

**Tập trung vào:**
- `QSqlQueryModel` - Chỉ đọc
- `QSqlTableModel` - Đọc và ghi
- `QSqlRelationalTableModel` - Có quan hệ
- `setTable()`, `select()`, `submitAll()`

**So sánh:**
| Model | Đọc | Ghi | Số bảng |
|-------|-----|-----|---------|
| QSqlQueryModel | ✅ | ❌ | Nhiều (query) |
| QSqlTableModel | ✅ | ✅ | 1 |
| QSqlRelationalTableModel | ✅ | ✅ | Nhiều (FK) |

**Thực hành:**
1. So sánh 3 loại SQL Model
2. Tìm `setTable()` và `select()`
3. Hiểu `submitAll()` dùng để làm gì

**Checklist:**
- [ ] Biết khi nào dùng QueryModel vs TableModel
- [ ] Hiểu `select()` load dữ liệu
- [ ] Biết `submitAll()` lưu thay đổi

---

### Bước 7: Ví Dụ Hoàn Chỉnh - Kết Hợp Tất Cả (1 giờ)
**Đọc:** `examples/06_complete_example.cpp`

**Tập trung vào:**
- SQL Model → Proxy Model → View
- Luồng dữ liệu hoàn chỉnh
- Tương tác user (search, edit, save)

**Thực hành:**
1. Trace luồng dữ liệu từ Database đến View
2. Tìm nơi user tương tác (search box, edit cell)
3. Hiểu cách lưu dữ liệu (`submitAll()`)

**Checklist:**
- [ ] Hiểu luồng dữ liệu hoàn chỉnh
- [ ] Biết cách Proxy lọc dữ liệu
- [ ] Hiểu cách lưu thay đổi vào database

---

## 🔍 Cách Đọc Code Nhanh (Sau Khi Học Xong)

### Khi Gặp Code Qt Model:

1. **Tìm khai báo model** (30 giây)
   ```cpp
   QAbstractListModel *model;      // → List
   QAbstractTableModel *model;     // → Table
   QSqlTableModel *model;          // → SQL
   QSortFilterProxyModel *proxy;   // → Proxy
   ```

2. **Tìm view kết nối** (10 giây)
   ```cpp
   view->setModel(model);
   ```

3. **Tìm proxy chain** (nếu có) (20 giây)
   ```cpp
   proxy->setSourceModel(sourceModel);
   ```

4. **Tìm data source** (30 giây)
   - SQL: `setTable()`, `select()`
   - Custom: Tìm nơi lưu dữ liệu (QList, QVector...)

5. **Đọc các hàm quan trọng** (5 phút)
   - `rowCount()`, `columnCount()`, `data()`
   - `index()`, `parent()` (nếu là tree)
   - `setData()` (nếu có chỉnh sửa)

**Tổng thời gian: ~7 phút để hiểu cấu trúc code**

---

## 📖 Tài Liệu Tham Khảo

### Trong Project:
- `README.md` - Tổng quan và giải thích chi tiết
- `QUICK_REFERENCE.md` - Tra cứu nhanh
- `DIAGRAM.md` - Sơ đồ và hình ảnh
- `examples/` - Code ví dụ từng loại model

### Ngoài Project:
- Qt Documentation: https://doc.qt.io/qt-6/model-view-programming.html
- QAbstractItemModel: https://doc.qt.io/qt-6/qabstractitemmodel.html
- QSqlTableModel: https://doc.qt.io/qt-6/qsqltablemodel.html

---

## ✅ Kiểm Tra Kiến Thức

Sau khi học xong, bạn nên có thể:

- [ ] Phân biệt được List, Table, Tree Model
- [ ] Hiểu Proxy Model làm gì và cách dùng
- [ ] Biết khi nào dùng SQL Model nào
- [ ] Đọc được code Qt Model trong 5-10 phút
- [ ] Hiểu luồng dữ liệu: Model → Proxy → View
- [ ] Biết các hàm quan trọng cần implement
- [ ] Hiểu signals/slots trong Model/View

---

## 🚀 Tips Học Hiệu Quả

1. **Đọc code song song với documentation**
   - Đọc ví dụ → Hiểu → Đọc docs → Hiểu sâu hơn

2. **Debug để hiểu luồng**
   - Đặt breakpoint tại `data()`
   - Xem khi nào được gọi, với index nào

3. **Vẽ sơ đồ**
   - Vẽ cây cho Tree Model
   - Vẽ luồng dữ liệu cho Proxy Model

4. **Thực hành**
   - Tạo model đơn giản của riêng bạn
   - Modify ví dụ để hiểu rõ hơn

5. **Đọc code thực tế**
   - Tìm project Qt trên GitHub
   - Đọc code model của họ
   - So sánh với những gì đã học

---

## ⏱️ Tổng Thời Gian Học

- **Tối thiểu:** 8 giờ (đọc và hiểu cơ bản)
- **Khuyến nghị:** 12-16 giờ (đọc + thực hành + debug)
- **Thành thạo:** 20+ giờ (thực hành nhiều project)

**Lưu ý:** Học từng bước một, đừng nhảy cóc. Mỗi bước xây dựng trên bước trước.
