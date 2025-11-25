# Phân Tích Kiến Trúc QtDatabase (Qt SQL)

## 1. Tổng Quan Kiến Trúc

QtDatabase là module SQL của Qt Framework, cung cấp interface thống nhất để làm việc với nhiều loại database khác nhau thông qua driver system.

### 1.1. Kiến Trúc Tổng Thể

```
Application Layer
    ↓
Qt SQL API (QSqlDatabase, QSqlQuery, QSqlTableModel, etc.)
    ↓
Driver Manager (QSqlDriver)
    ↓
Database Drivers (QSQLITE, QMYSQL, QPSQL, QODBC, etc.)
    ↓
Native Database APIs
```

## 2. Các Lớp Chính và Trách Nhiệm

### 2.1. QSqlDatabase - Quản Lý Kết Nối

**Vai trò**: Quản lý kết nối đến database

**Điểm quan trọng**:
- Singleton pattern: Mỗi connection name chỉ có một instance
- Connection pooling: Có thể tạo nhiều connections với các tên khác nhau
- Thread-safe: Mỗi thread nên có connection riêng

**Luồng hoạt động**:
```cpp
// 1. Đăng ký driver
QSqlDatabase::addDatabase("QSQLITE", "connection_name");

// 2. Cấu hình connection
QSqlDatabase db = QSqlDatabase::database("connection_name");
db.setDatabaseName("database.db");

// 3. Mở connection
db.open();

// 4. Sử dụng connection
QSqlQuery query(db);
query.exec("SELECT * FROM users");
```

### 2.2. QSqlDriver - Abstract Driver Interface

**Vai trò**: Interface cơ bản cho tất cả database drivers

**Các phương thức quan trọng**:
- `hasFeature()`: Kiểm tra tính năng driver hỗ trợ
- `open()`: Mở kết nối
- `close()`: Đóng kết nối
- `createResult()`: Tạo QSqlResult
- `beginTransaction()`, `commitTransaction()`, `rollbackTransaction()`

**Design Pattern**: Strategy Pattern - mỗi driver implement interface này

### 2.3. QSqlResult - Xử Lý Kết Quả Query

**Vai trò**: Xử lý kết quả của SQL queries

**Luồng xử lý**:
```
QSqlQuery::exec()
    ↓
QSqlDriver::createResult()
    ↓
QSqlResult::exec()
    ↓
QSqlResult::fetch() (lazy loading)
    ↓
QSqlResult::value() (lấy giá trị từ cache)
```

**Đặc điểm**:
- Forward-only cursor (mặc định)
- Lazy evaluation: chỉ fetch khi cần
- Caching: cache một số records để tối ưu

### 2.4. QSqlQuery - Thực Thi SQL Queries

**Vai trò**: Thực thi và duyệt kết quả SQL queries

**Các phương thức chính**:
- `exec()`: Thực thi query
- `prepare()`: Chuẩn bị prepared statement
- `bindValue()`: Bind parameters
- `next()`, `previous()`, `first()`, `last()`: Navigation
- `value()`: Lấy giá trị cột

**Prepared Statements**:
```cpp
QSqlQuery query;
query.prepare("INSERT INTO users (name, email) VALUES (:name, :email)");
query.bindValue(":name", "John");
query.bindValue(":email", "john@example.com");
query.exec();
```

### 2.5. QSqlRecord - Metadata của Record

**Vai trò**: Chứa metadata về một record (tên cột, kiểu dữ liệu, giá trị)

**Cấu trúc**:
- `QSqlField`: Thông tin về một field (tên, type, value)
- `QSqlRecord`: Collection của QSqlField

### 2.6. QSqlTableModel - Model cho Table

**Vai trò**: Model để hiển thị và chỉnh sửa một database table

**Kiến trúc**:
```
QSqlTableModel (QAbstractTableModel)
    ↓
QSqlQuery để fetch data
    ↓
Cache records trong memory
    ↓
Submit changes về database
```

**Các phương thức quan trọng**:
- `setTable()`: Chọn table
- `select()`: Load data từ database
- `setEditStrategy()`: OnFieldChange, OnRowChange, OnManualSubmit
- `submitAll()`: Commit changes
- `revertAll()`: Rollback changes

### 2.7. QSqlQueryModel - Read-only Model

**Vai trò**: Model chỉ đọc cho custom queries

**Khác biệt với QSqlTableModel**:
- Không hỗ trợ editing
- Có thể dùng với bất kỳ SQL query nào
- Nhẹ hơn QSqlTableModel

## 3. Driver System

### 3.1. Cách Driver Được Load

```
QSqlDatabase::addDatabase("QSQLITE")
    ↓
QSqlDatabasePrivate::registerSqlDriver()
    ↓
QSqlDriverPlugin::create() (nếu là plugin)
    hoặc
QSqlDriver constructor (nếu built-in)
```

### 3.2. Các Driver Phổ Biến

**QSQLITE**:
- File-based database
- Không cần server
- Hỗ trợ đầy đủ SQL features

**QMYSQL**:
- MySQL/MariaDB
- Cần MySQL client library
- Hỗ trợ transactions, prepared statements

**QPSQL**:
- PostgreSQL
- Cần libpq
- Hỗ trợ advanced features

**QODBC**:
- Generic ODBC driver
- Có thể kết nối nhiều loại database
- Performance thấp hơn native drivers

## 4. Transaction Management

### 4.1. Luồng Transaction

```
QSqlDatabase::transaction()
    ↓
QSqlDriver::beginTransaction()
    ↓
... thực hiện queries ...
    ↓
QSqlDatabase::commit() hoặc rollback()
    ↓
QSqlDriver::commitTransaction() hoặc rollbackTransaction()
```

### 4.2. Auto-commit Mode

- Mặc định: Mỗi query tự động commit
- Transaction mode: Phải gọi commit() hoặc rollback() rõ ràng

## 5. Error Handling

### 5.1. QSqlError

**Cấu trúc**:
- `type()`: Loại lỗi (NoError, ConnectionError, StatementError, TransactionError)
- `number()`: Database-specific error code
- `text()`: Error message
- `driverText()`: Driver-specific message
- `databaseText()`: Database-specific message

### 5.2. Kiểm Tra Lỗi

```cpp
QSqlQuery query;
if (!query.exec("SELECT * FROM users")) {
    QSqlError error = query.lastError();
    qDebug() << "Error:" << error.text();
}
```

## 6. Thread Safety

### 6.1. Nguyên Tắc

- **Mỗi thread cần connection riêng**: Không share QSqlDatabase giữa threads
- **QSqlQuery không thread-safe**: Mỗi thread tạo QSqlQuery riêng
- **QSqlTableModel không thread-safe**: Chỉ dùng trong thread tạo nó

### 6.2. Best Practices

```cpp
// Thread 1
QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE", "conn1");
db1.setDatabaseName("db1.db");
db1.open();

// Thread 2
QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE", "conn2");
db2.setDatabaseName("db2.db");
db2.open();
```

## 7. Performance Optimization

### 7.1. Prepared Statements

**Lợi ích**:
- Parse query một lần
- Tối ưu execution plan
- Bảo mật hơn (tránh SQL injection)

### 7.2. Batch Operations

```cpp
QSqlDatabase::database().transaction();
QSqlQuery query;
query.prepare("INSERT INTO users (name) VALUES (?)");
for (const QString &name : names) {
    query.bindValue(0, name);
    query.exec();
}
QSqlDatabase::database().commit();
```

### 7.3. Caching

- QSqlTableModel cache records trong memory
- Có thể set cache size với `setQuery()`
- Balance giữa memory và performance

## 8. Design Patterns Sử Dụng

### 8.1. Factory Pattern
- `QSqlDatabase::addDatabase()` tạo driver instances
- `QSqlDriver::createResult()` tạo QSqlResult

### 8.2. Strategy Pattern
- Mỗi driver implement QSqlDriver interface
- Runtime selection của driver

### 8.3. Template Method Pattern
- QSqlResult định nghĩa skeleton của algorithm
- Subclasses implement các bước cụ thể

### 8.4. Observer Pattern
- QSqlTableModel notify views khi data thay đổi
- Signal/slot mechanism

## 9. Luồng Dữ Liệu Tổng Thể

### 9.1. Read Flow

```
User Code: query.exec("SELECT * FROM users")
    ↓
QSqlQuery::exec()
    ↓
QSqlDriver::createResult()
    ↓
QSqlResult::exec() (driver-specific)
    ↓
Native DB API: Execute query
    ↓
QSqlResult::fetch() (lazy)
    ↓
QSqlQuery::value() → Return to user
```

### 9.2. Write Flow (QSqlTableModel)

```
User: model.setData(index, value)
    ↓
QSqlTableModel::setData()
    ↓
Cache change trong memory
    ↓
Emit dataChanged() signal
    ↓
User: model.submitAll()
    ↓
QSqlTableModel::submitAll()
    ↓
Generate UPDATE/INSERT/DELETE queries
    ↓
QSqlQuery::exec()
    ↓
Database updated
```

## 10. Các File Nguồn Quan Trọng

### 10.1. Core Classes
- `qsqldatabase.h/cpp`: QSqlDatabase implementation
- `qsqldriver.h/cpp`: QSqlDriver base class
- `qsqlresult.h/cpp`: QSqlResult base class
- `qsqlquery.h/cpp`: QSqlQuery implementation
- `qsqlrecord.h/cpp`: QSqlRecord và QSqlField

### 10.2. Model Classes
- `qsqltablemodel.h/cpp`: QSqlTableModel
- `qsqlquerymodel.h/cpp`: QSqlQueryModel
- `qsqlrelationaltablemodel.h/cpp`: QSqlRelationalTableModel

### 10.3. Driver Implementations
- `qsql_sqlite.h/cpp`: SQLite driver
- `qsql_mysql.h/cpp`: MySQL driver
- `qsql_psql.h/cpp`: PostgreSQL driver
- `qsql_odbc.h/cpp`: ODBC driver

## 11. Điểm Quan Trọng Cần Nhớ

1. **Connection Management**: Luôn đóng connections khi không dùng
2. **Error Handling**: Luôn kiểm tra return values và lastError()
3. **Thread Safety**: Mỗi thread cần connection riêng
4. **Prepared Statements**: Dùng cho queries lặp lại
5. **Transactions**: Dùng cho batch operations
6. **Model Updates**: Gọi select() sau khi thay đổi database từ bên ngoài
7. **Memory Management**: QSqlQuery và QSqlResult tự động cleanup

## 12. Ví Dụ Code Mẫu

### 12.1. Basic Query

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("mydb.db");
if (!db.open()) {
    qDebug() << "Error:" << db.lastError();
    return;
}

QSqlQuery query;
if (query.exec("SELECT id, name FROM users")) {
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        qDebug() << id << name;
    }
}
```

### 12.2. Prepared Statement

```cpp
QSqlQuery query;
query.prepare("INSERT INTO users (name, email) VALUES (?, ?)");
query.addBindValue("John Doe");
query.addBindValue("john@example.com");
if (!query.exec()) {
    qDebug() << "Error:" << query.lastError();
}
```

### 12.3. Transaction

```cpp
QSqlDatabase::database().transaction();
QSqlQuery query;
// ... multiple queries ...
if (/* success */) {
    QSqlDatabase::database().commit();
} else {
    QSqlDatabase::database().rollback();
}
```

### 12.4. Model Usage

```cpp
QSqlTableModel *model = new QSqlTableModel(this);
model->setTable("users");
model->setEditStrategy(QSqlTableModel::OnManualSubmit);
model->select();

// Display in view
QTableView *view = new QTableView;
view->setModel(model);

// Edit data
model->setData(model->index(0, 1), "New Name");
model->submitAll();
```

## 13. Debugging Tips

1. **Enable SQL Debug**: `QT_LOGGING_RULES="qt.sql.drivers.debug=true"`
2. **Check lastError()**: Sau mỗi database operation
3. **Log Queries**: In ra SQL queries được thực thi
4. **Check Connection**: Verify connection trước khi query
5. **Monitor Transactions**: Đảm bảo commit/rollback đúng

---

**Tóm Tắt**: QtDatabase sử dụng driver-based architecture với abstraction layer mạnh mẽ, cho phép làm việc với nhiều loại database thông qua API thống nhất. Hiểu rõ kiến trúc này giúp sử dụng hiệu quả và debug dễ dàng hơn.
