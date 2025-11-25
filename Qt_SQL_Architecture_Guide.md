# Hướng Dẫn Hiểu Code Nguồn Qt SQL Database

## 📋 Mục Lục
1. [Tổng Quan Kiến Trúc](#tổng-quan-kiến-trúc)
2. [Cấu Trúc Thư Mục](#cấu-trúc-thư-mục)
3. [Các Lớp Cốt Lõi](#các-lớp-cốt-lõi)
4. [Luồng Hoạt Động](#luồng-hoạt-động)
5. [Design Patterns Được Sử Dụng](#design-patterns-được-sử-dụng)
6. [Ví Dụ Minh Họa](#ví-dụ-minh-họa)

---

## 🏗️ Tổng Quan Kiến Trúc

Qt SQL Module được thiết kế theo kiến trúc phân lớp với 3 tầng chính:

```
┌─────────────────────────────────────────────────┐
│          APPLICATION LAYER (Ứng dụng)           │
│  QSqlTableModel, QSqlQueryModel, QSqlQuery     │
└────────────────────┬────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────┐
│         ABSTRACTION LAYER (Trừu tượng)          │
│   QSqlDatabase, QSqlDriver, QSqlResult         │
└────────────────────┬────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────┐
│       DATABASE DRIVERS (Drivers cụ thể)         │
│  MySQL, PostgreSQL, SQLite, Oracle, etc.        │
└─────────────────────────────────────────────────┘
```

### Nguyên Tắc Thiết Kế Chính:
- **Tách biệt interface và implementation** (Abstract Factory Pattern)
- **Plugin architecture** cho database drivers
- **Resource management** với RAII pattern
- **Thread-safety** cho kết nối database

---

## 📂 Cấu Trúc Thư Mục

```
src/sql/
├── kernel/          # Core classes - Lớp cốt lõi
│   ├── qsqldatabase.*      # Quản lý kết nối database
│   ├── qsqldriver.*        # Interface cho driver
│   ├── qsqlquery.*         # Thực thi SQL queries
│   ├── qsqlresult.*        # Xử lý kết quả query
│   ├── qsqlrecord.*        # Đại diện 1 record/row
│   ├── qsqlfield.*         # Đại diện 1 field/column
│   ├── qsqlindex.*         # Index của table
│   ├── qsqlerror.*         # Xử lý lỗi
│   └── qsqlcachedresult.*  # Cache kết quả query
│
├── models/          # Model-View classes
│   ├── qsqlquerymodel.*           # Read-only model
│   ├── qsqltablemodel.*           # Editable table model
│   └── qsqlrelationaltablemodel.* # Model với foreign keys
│
└── doc/            # Documentation và examples
```

---

## 🎯 Các Lớp Cốt Lõi

### 1. **QSqlDatabase** - Trung Tâm Quản Lý Kết Nối

**Chức năng chính:**
- Quản lý các kết nối database (có thể có nhiều kết nối cùng lúc)
- Factory pattern để tạo kết nối mới
- Quản lý transaction (begin, commit, rollback)
- Lấy metadata về database (tables, indexes, etc.)

**Key Methods:**
```cpp
// Tạo kết nối mới
static QSqlDatabase addDatabase(const QString& type, 
                                 const QString& connectionName);

// Mở/đóng kết nối
bool open();
void close();

// Transaction management
bool transaction();
bool commit();
bool rollback();

// Database metadata
QStringList tables(QSql::TableType type);
QSqlIndex primaryIndex(const QString& tablename);
QSqlRecord record(const QString& tablename);
```

**Design Pattern:** 
- **Singleton-like** cho mỗi connection name
- **Factory** cho việc tạo driver
- **RAII** cho resource management

---

### 2. **QSqlDriver** - Interface Trừu Tượng Cho Drivers

**Mục đích:**
- Định nghĩa interface chung cho tất cả database drivers
- Mỗi loại database (MySQL, PostgreSQL, SQLite...) implement lớp này
- Cho phép Qt hỗ trợ nhiều loại database mà không thay đổi code người dùng

**Key Features:**
```cpp
enum DriverFeature {
    Transactions,           // Hỗ trợ transaction
    QuerySize,             // Biết trước số rows
    BLOB,                  // Binary Large Objects
    Unicode,               // Unicode support
    PreparedQueries,       // Prepared statements
    NamedPlaceholders,     // :name placeholders
    PositionalPlaceholders,// ? placeholders
    BatchOperations,       // Batch insert/update
    // ... và nhiều features khác
};

// Các phương thức abstract phải implement:
virtual bool open(...) = 0;
virtual void close() = 0;
virtual QSqlResult *createResult() const = 0;
virtual bool hasFeature(DriverFeature f) const = 0;
```

**Plugin Architecture:**
- Drivers được load động qua plugin system
- Mỗi driver là 1 shared library riêng
- Có thể thêm custom driver mà không biên dịch lại Qt

---

### 3. **QSqlQuery** - Thực Thi SQL Queries

**Vai trò:**
- High-level API để thực thi SQL commands
- Hỗ trợ prepared statements với parameter binding
- Iterator pattern để duyệt qua kết quả
- Hỗ trợ batch operations

**Usage Flow:**
```cpp
// 1. Tạo query
QSqlQuery query(db);

// 2. Prepare statement (optional nhưng recommended)
query.prepare("SELECT * FROM users WHERE age > :age");

// 3. Bind parameters
query.bindValue(":age", 18);

// 4. Execute
query.exec();

// 5. Iterate results
while (query.next()) {
    QString name = query.value(0).toString();
    int age = query.value(1).toInt();
}
```

**Key Methods:**
```cpp
bool prepare(const QString& query);     // Chuẩn bị query
bool exec();                            // Thực thi prepared query
bool exec(const QString& query);        // Direct execution

void bindValue(const QString& placeholder, const QVariant& val);
void addBindValue(const QVariant& val);

// Navigation
bool next();      // Dòng tiếp theo
bool previous();  // Dòng trước
bool first();     // Dòng đầu
bool last();      // Dòng cuối
bool seek(int i); // Nhảy đến dòng i

// Data access
QVariant value(int index);
QVariant value(const QString& name);
```

---

### 4. **QSqlResult** - Xử Lý Kết Quả Query (Backend)

**Mô tả:**
- Đây là lớp LOW-LEVEL mà driver developers phải implement
- `QSqlQuery` là wrapper bên ngoài, `QSqlResult` là engine bên trong
- Mỗi driver implement subclass của `QSqlResult`

**Responsibility:**
- Fetch data từ database backend
- Quản lý cursor position
- Handle parameter binding
- Batch execution support

**Abstract Methods (Driver phải implement):**
```cpp
virtual QVariant data(int index) = 0;    // Lấy data tại column index
virtual bool isNull(int index) = 0;      // Check NULL
virtual bool reset(const QString& query) = 0;  // Execute query
virtual bool fetch(int index) = 0;       // Di chuyển đến row index
virtual bool fetchFirst() = 0;           // First row
virtual bool fetchLast() = 0;            // Last row
virtual int size() = 0;                  // Số rows (-1 nếu unknown)
virtual int numRowsAffected() = 0;       // Rows affected by UPDATE/DELETE
```

---

### 5. **QSqlRecord** - Đại Diện Một Row

**Chức năng:**
- Container chứa 1 row data từ database
- Bao gồm collection của `QSqlField` objects
- Dùng để insert/update rows

**Structure:**
```
QSqlRecord
├── QSqlField (column 1) → value, name, type, metadata
├── QSqlField (column 2) → value, name, type, metadata
├── QSqlField (column 3) → value, name, type, metadata
└── ...
```

**Key Operations:**
```cpp
// Access data
QVariant value(int index);
QVariant value(const QString& name);
void setValue(int index, const QVariant& val);

// Field management
void append(const QSqlField& field);
QSqlField field(int index);
int count();  // Số columns

// Metadata
QString fieldName(int index);
int indexOf(const QString& name);
bool contains(const QString& name);
```

---

### 6. **QSqlField** - Đại Diện Một Column

**Thuộc tính:**
```cpp
class QSqlField {
    QString name;              // Tên field
    QMetaType metaType;        // Kiểu dữ liệu
    QVariant value;            // Giá trị
    QString tableName;         // Table chứa field này
    RequiredStatus required;   // Optional/Required
    int length;                // Max length
    int precision;             // Số thập phân
    bool readOnly;             // Chỉ đọc?
    bool generated;            // Có được generate trong query không?
    bool autoValue;            // Auto-increment?
};
```

**Metadata Rich:**
- Không chỉ lưu value mà còn metadata đầy đủ
- Hỗ trợ form generation, validation
- Integration với Qt's property system

---

### 7. **QSqlTableModel** - Model-View Integration

**Đặc điểm:**
- Kết nối Qt SQL với Model-View architecture
- Tự động sync với database
- Hỗ trợ editing, sorting, filtering
- Cache changes trước khi submit

**Edit Strategies:**
```cpp
enum EditStrategy {
    OnFieldChange,     // Submit ngay khi field thay đổi
    OnRowChange,       // Submit khi chuyển row
    OnManualSubmit     // Phải gọi submitAll() thủ công
};
```

**Usage Example:**
```cpp
QSqlTableModel model;
model.setTable("employees");
model.setEditStrategy(QSqlTableModel::OnManualSubmit);
model.select();

// Connect to QTableView
tableView->setModel(&model);

// Make changes
model.setData(model.index(0, 1), "New Name");

// Submit all changes
model.submitAll();
```

---

## 🔄 Luồng Hoạt Động

### Kịch Bản 1: Thực Thi Simple Query

```
User Code
    │
    ├─→ QSqlQuery::exec("SELECT * FROM users")
    │       │
    │       ├─→ QSqlDriver::createResult()
    │       │       │
    │       │       └─→ Tạo MySQLResult (hoặc SQLiteResult, etc.)
    │       │
    │       └─→ QSqlResult::reset(query)
    │               │
    │               └─→ Native Database API (mysql_query, sqlite3_exec, etc.)
    │
    ├─→ QSqlQuery::next()
    │       │
    │       └─→ QSqlResult::fetchNext()
    │               │
    │               └─→ mysql_fetch_row() hoặc sqlite3_step()
    │
    └─→ QSqlQuery::value(0)
            │
            └─→ QSqlResult::data(0)
                    │
                    └─→ Convert native type → QVariant
```

### Kịch Bản 2: Prepared Statement với Binding

```
1. Prepare Phase:
   QSqlQuery::prepare("INSERT INTO users (name, age) VALUES (?, ?)")
       └─→ QSqlResult::prepare(sql)
           └─→ mysql_stmt_prepare() hoặc sqlite3_prepare_v2()

2. Binding Phase:
   QSqlQuery::bindValue(0, "John")
   QSqlQuery::bindValue(1, 25)
       └─→ Store bindings trong QSqlResult::boundValues

3. Execution Phase:
   QSqlQuery::exec()
       └─→ QSqlResult::exec()
           ├─→ Bind parameters: mysql_stmt_bind_param()
           └─→ Execute: mysql_stmt_execute()
```

### Kịch Bản 3: Model-View-Database Sync

```
QTableView (UI)
    │
    │ user edit cell
    │
    ▼
QSqlTableModel::setData()
    │
    │ if strategy == OnFieldChange
    │     └─→ submit() immediately
    │
    │ else
    │     └─→ cache change
    │
    └─→ emit dataChanged()
            │
            └─→ QTableView updates

Later: model.submitAll()
    │
    └─→ QSqlTableModel::updateRowInTable()
            │
            └─→ Generate UPDATE statement
                    │
                    └─→ QSqlQuery::exec(updateSql)
```

---

## 🎨 Design Patterns Được Sử Dụng

### 1. **Abstract Factory Pattern**
```cpp
// QSqlDatabase acts as factory
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
                                              ↓
                          Creates appropriate QSqlDriver subclass
                                              ↓
                                      QMYSQLDriver instance
```

### 2. **Bridge Pattern**
```
QSqlQuery (Abstraction)
    │
    │ delegates to
    │
QSqlResult (Implementation)
    │
    └─→ MySQLResult, PostgreSQLResult, SQLiteResult (Concrete Implementations)
```

### 3. **Strategy Pattern**
```cpp
// QSqlTableModel::EditStrategy
setEditStrategy(OnFieldChange);    // Strategy 1
setEditStrategy(OnRowChange);      // Strategy 2
setEditStrategy(OnManualSubmit);   // Strategy 3
```

### 4. **Template Method Pattern**
```cpp
// QSqlResult defines the template:
bool QSqlResult::exec() {
    // Template method
    if (!prepareAndBind()) return false;
    return execImplementation();  // Subclass implements
}
```

### 5. **Iterator Pattern**
```cpp
QSqlQuery query;
query.exec("SELECT * FROM users");

while (query.next()) {  // Iterator pattern
    // process row
}
```

### 6. **Lazy Initialization**
```cpp
QSqlDatabase::database("connection1");
    // Kết nối chỉ mở khi thực sự cần
    // Không mở ngay khi gọi addDatabase()
```

---

## 💡 Ví Dụ Minh Họa Code Flow

### Example 1: Basic Connection & Query

```cpp
// 1. SETUP - Tạo và config database
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("mydb.sqlite");

// 2. OPEN - Mở kết nối
if (!db.open()) {
    qDebug() << "Error:" << db.lastError().text();
    return;
}

// 3. QUERY - Thực thi query
QSqlQuery query;
query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
query.exec("INSERT INTO users VALUES (1, 'Alice', 30)");
query.exec("INSERT INTO users VALUES (2, 'Bob', 25)");

// 4. READ - Đọc dữ liệu
query.exec("SELECT * FROM users");
while (query.next()) {
    int id = query.value(0).toInt();
    QString name = query.value(1).toString();
    int age = query.value(2).toInt();
    qDebug() << id << name << age;
}

// 5. CLEANUP - Qt tự động cleanup khi db out of scope
```

### Example 2: Prepared Statement (Bảo Mật Cao Hơn)

```cpp
QSqlQuery query;

// PREPARE - Tách riêng SQL structure và data
query.prepare("INSERT INTO users (name, age) VALUES (:name, :age)");

// BIND - Bind parameters (tự động escape, prevent SQL injection)
query.bindValue(":name", "Charlie");
query.bindValue(":age", 35);

// EXECUTE
if (!query.exec()) {
    qDebug() << "Insert failed:" << query.lastError().text();
}

// Có thể re-use prepared statement
query.bindValue(":name", "Diana");
query.bindValue(":age", 28);
query.exec();
```

### Example 3: Transaction (ACID Compliance)

```cpp
QSqlDatabase db = QSqlDatabase::database();

// BEGIN TRANSACTION
if (!db.transaction()) {
    qDebug() << "Cannot start transaction";
    return;
}

QSqlQuery query;

// Multiple operations
bool success = true;
success &= query.exec("UPDATE accounts SET balance = balance - 100 WHERE id = 1");
success &= query.exec("UPDATE accounts SET balance = balance + 100 WHERE id = 2");

if (success) {
    // COMMIT - Apply all changes
    db.commit();
    qDebug() << "Transaction successful";
} else {
    // ROLLBACK - Undo all changes
    db.rollback();
    qDebug() << "Transaction failed, rolled back";
}
```

### Example 4: Using QSqlTableModel

```cpp
// 1. SETUP MODEL
QSqlTableModel *model = new QSqlTableModel;
model->setTable("employees");
model->setEditStrategy(QSqlTableModel::OnManualSubmit);

// 2. ADD FILTER & SORT
model->setFilter("age > 25");
model->setSort(1, Qt::AscendingOrder);  // Sort by column 1

// 3. LOAD DATA
model->select();

// 4. CONNECT TO VIEW
QTableView *view = new QTableView;
view->setModel(model);
view->show();

// 5. PROGRAMMATIC EDIT
QModelIndex idx = model->index(0, 1);  // row 0, column 1
model->setData(idx, "New Name");

// 6. INSERT NEW ROW
int row = model->rowCount();
model->insertRow(row);
model->setData(model->index(row, 1), "John Doe");
model->setData(model->index(row, 2), 30);

// 7. SUBMIT ALL CHANGES
if (model->submitAll()) {
    qDebug() << "Changes saved";
} else {
    qDebug() << "Save failed:" << model->lastError().text();
    model->revertAll();
}
```

---

## 🔍 Chi Tiết Implementation Quan Trọng

### Thread Safety

```cpp
// Mỗi QSqlDatabase connection chỉ dùng trong 1 thread
// Để dùng multi-thread:

// Thread 1:
QSqlDatabase db1 = QSqlDatabase::database("connection1");

// Thread 2:
QSqlDatabase db2 = QSqlDatabase::cloneDatabase(
    QSqlDatabase::database(), 
    "connection2"  // Unique name per thread
);

// db1 và db2 là độc lập, có thể dùng parallel
```

### Memory Management

```cpp
// Qt uses IMPLICIT SHARING (Copy-on-Write)

QSqlRecord record1 = query.record();
QSqlRecord record2 = record1;  // Không copy thật, chỉ share pointer

record2.setValue(0, "modified");  // Lúc này mới thật sự copy
// record1 và record2 giờ là 2 objects độc lập
```

### Error Handling

```cpp
QSqlQuery query;
if (!query.exec("SELECT * FROM users")) {
    QSqlError error = query.lastError();
    
    qDebug() << "Type:" << error.type();           // ErrorType enum
    qDebug() << "Database text:" << error.databaseText();
    qDebug() << "Driver text:" << error.driverText();
    qDebug() << "Native code:" << error.nativeErrorCode();
}
```

---

## 📊 Class Diagram Tổng Quát

```
                    ┌─────────────────┐
                    │  QSqlDatabase   │
                    │  (Connection)   │
                    └────────┬────────┘
                             │ manages
                ┌────────────┴────────────┐
                │                         │
        ┌───────▼────────┐       ┌───────▼────────┐
        │   QSqlDriver   │       │   QSqlQuery    │
        │   (Abstract)   │◄──────│   (High-level) │
        └───────┬────────┘ uses  └───────┬────────┘
                │                        │ uses
                │ creates                │
        ┌───────▼────────┐       ┌───────▼────────┐
        │   QSqlResult   │◄──────│   QSqlRecord   │
        │   (Low-level)  │ uses  │   (Row data)   │
        └────────────────┘       └───────┬────────┘
                                         │ contains
                                 ┌───────▼────────┐
                                 │   QSqlField    │
                                 │  (Column data) │
                                 └────────────────┘

            Model-View Integration:
                ┌────────────────────┐
                │  QSqlQueryModel    │
                │  (Read-only)       │
                └──────────┬─────────┘
                           │ inherits
                ┌──────────▼─────────┐
                │  QSqlTableModel    │
                │  (Editable)        │
                └──────────┬─────────┘
                           │ inherits
        ┌──────────────────▼──────────────────┐
        │  QSqlRelationalTableModel          │
        │  (With Foreign Keys)                │
        └─────────────────────────────────────┘
```

---

## 🚀 Best Practices

### 1. **Luôn sử dụng Prepared Statements**
```cpp
// ❌ BAD - SQL Injection vulnerability
QString name = userInput;
query.exec("SELECT * FROM users WHERE name = '" + name + "'");

// ✅ GOOD - Safe from SQL injection
query.prepare("SELECT * FROM users WHERE name = :name");
query.bindValue(":name", userInput);
query.exec();
```

### 2. **Error Checking**
```cpp
// ✅ Luôn check return value
if (!db.open()) {
    handleError(db.lastError());
    return;
}

if (!query.exec()) {
    handleError(query.lastError());
    return;
}
```

### 3. **Transaction cho Multiple Operations**
```cpp
// ✅ Wrap multiple writes trong transaction
db.transaction();
// ... multiple INSERTs/UPDATEs ...
db.commit();  // hoặc rollback() nếu có lỗi
```

### 4. **Connection Naming**
```cpp
// ✅ Đặt tên rõ ràng cho connections
QSqlDatabase mainDb = QSqlDatabase::addDatabase("QSQLITE", "main");
QSqlDatabase logDb = QSqlDatabase::addDatabase("QSQLITE", "logging");

// Dễ quản lý nhiều databases
```

### 5. **Cleanup**
```cpp
// ✅ Remove connection khi không dùng nữa
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "temp");
    // ... use db ...
}  // db auto-closed here

QSqlDatabase::removeDatabase("temp");  // Cleanup connection
```

---

## 📚 Tài Nguyên Học Thêm

### Source Code Locations:
- **Core classes**: `src/sql/kernel/`
- **Model classes**: `src/sql/models/`
- **Driver implementations**: `src/plugins/sqldrivers/`

### Documentation:
- `src/sql/doc/` - Official Qt documentation
- `src/sql/doc/snippets/` - Code examples

### Key Files to Study:
1. `qsqldatabase.cpp` - Connection management
2. `qsqlquery.cpp` - Query execution logic
3. `qsqldriver.cpp` - Driver abstraction
4. `qsqlresult.cpp` - Result handling
5. `qsqltablemodel.cpp` - Model-View integration

---

## 🎓 Kết Luận

**Qt SQL Module** là một ví dụ xuất sắc về:
- **Clean Architecture** - Tách biệt rõ ràng các concerns
- **Plugin System** - Extensible design
- **API Design** - Dễ dùng nhưng mạnh mẽ
- **Cross-platform** - Unified API cho nhiều databases

**Để nắm vững, bạn nên:**
1. ✅ Đọc header files (`.h`) trước để hiểu interface
2. ✅ Sau đó đọc implementation (`.cpp`) để hiểu chi tiết
3. ✅ Chạy thử examples trong `doc/snippets/`
4. ✅ Thử implement một simple driver để hiểu sâu hơn

**Happy Coding! 🚀**
