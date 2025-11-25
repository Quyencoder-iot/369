# Hướng dẫn Đọc hiểu nhanh Source Code QtSql (Qt Database)

Vì kho lưu trữ hiện tại chưa có mã nguồn cụ thể, tài liệu này sẽ phân tích kiến trúc và mã nguồn của mô-đun **QtSql** (thường được gọi là qtdatabase) từ thư viện Qt chính chuẩn (Qt Framework). Đây là hướng dẫn giúp bạn điều hướng và hiểu nhanh cách Qt xử lý cơ sở dữ liệu.

## 1. Cấu trúc thư mục gốc (trong Qt Source)
Mã nguồn QtSql thường nằm trong `qtbase/src/sql`. Cấu trúc chính bao gồm:

```text
src/sql/
├── kernel/          # Chứa các lớp cốt lõi (Core classes)
│   ├── qsqldatabase.cpp/h   # Quản lý kết nối
│   ├── qsqldriver.cpp/h     # Interface cho các driver
│   ├── qsqlquery.cpp/h      # Thực thi câu lệnh SQL
│   └── ...
├── drivers/         # Các plugin cho từng loại DB (MySQL, PSQL, SQLite,...)
│   ├── sqlite/
│   ├── psql/
│   └── ...
└── models/          # Tích hợp với Model/View của Qt
    ├── qsqltablemodel.cpp/h
    └── qsqlquerymodel.cpp/h
```

## 2. Các lớp quan trọng và Cơ chế hoạt động

### A. QSqlDatabase (Quản lý kết nối)
- **Vai trò**: Là cổng vào chính, quản lý danh sách các kết nối.
- **Design Pattern**: Sử dụng **Singleton** (cho danh sách kết nối) và **Factory Pattern** (để tạo driver).
- **Điểm cần đọc trong code**:
  - `QSqlDatabase::addDatabase(type, connectionName)`: Hàm này tìm kiếm plugin driver tương ứng (ví dụ "QSQLITE") và khởi tạo nó.
  - Nó giữ một `static QConnectionDict` để lưu trữ các kết nối đã mở.

### B. QSqlDriver (Lớp trừu tượng cho Driver)
- **Vai trò**: Định nghĩa giao diện mà mọi database driver phải tuân theo (Open, Close, BeginTransaction, ...).
- **Cách đọc**: Xem `qsqldriver.h` để thấy các hàm `pure virtual` hoặc `virtual` mà các driver con phải implement.
- **QSqlResult**: Đi kèm với Driver, chịu trách nhiệm lấy dữ liệu thô từ DB API cấp thấp.

### C. QSqlQuery (Thực thi lệnh)
- **Vai trò**: Class người dùng hay dùng nhất để chạy SQL.
- **Source Code Flow**:
  - Khi bạn gọi `exec()`, `QSqlQuery` sẽ chuyển lệnh gọi xuống `QSqlResult::exec()`.
  - `QSqlResult` sẽ gọi API C/C++ của database cụ thể (ví dụ `sqlite3_step`).

### D. Models (QSqlTableModel, QSqlQueryModel)
- **Vai trò**: Cầu nối đưa dữ liệu lên giao diện (QTableView, QListView).
- **Source Code Flow**:
  - `data()`: Lấy dữ liệu từ cache hoặc fetch từ query để hiển thị.
  - `setData()`: (Trong TableModel) tạo câu lệnh `UPDATE` tự động gửi xuống database.

## 3. Luồng dữ liệu điển hình (Code Path)

Khi bạn chạy lệnh:
```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("test.db");
db.open();
```

**Luồng trong mã nguồn:**
1. `addDatabase` -> Load Plugin "qsqlite" (trong thư mục `plugins/sqldrivers`).
2. Plugin trả về một instance của `QSQLiteDriver`.
3. `db.open()` -> gọi `QSQLiteDriver::open()`.
4. `QSQLiteDriver` gọi hàm C `sqlite3_open()` của thư viện SQLite.

## 4. Mẹo đọc code nhanh
1. **Bắt đầu từ Interface**: Đọc `qsqldriver.h` trước để hiểu những gì một database *có thể* làm.
2. **Xem một Implementation cụ thể**: Chọn `drivers/sqlite` (vì nó đơn giản, khép kín) để xem cách map từ Qt sang C API.
3. **Bỏ qua chi tiết về Type Conversion**: Ban đầu đừng sa đà vào cách Qt chuyển đổi `QVariant` sang SQL type, hãy tập trung vào luồng lệnh (Command Flow).

## 5. Ví dụ minh họa (Giả lập)
Nếu bạn muốn test thử cơ chế, hãy xem file `example_usage.cpp` (tôi sẽ tạo ngay sau đây).
