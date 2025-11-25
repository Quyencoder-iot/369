# Cấu Trúc Source Code QtDatabase

## Vị Trí Source Code trong Qt

Source code của QtDatabase thường nằm trong:
```
qtbase/src/sql/
```

## Cấu Trúc Thư Mục

```
qtbase/src/sql/
├── kernel/                    # Core classes
│   ├── qsqldatabase.h/cpp    # Connection management
│   ├── qsqldriver.h/cpp      # Driver interface
│   ├── qsqlresult.h/cpp      # Result processing
│   ├── qsqlquery.h/cpp       # Query execution
│   ├── qsqlrecord.h/cpp      # Record metadata
│   ├── qsqlfield.h/cpp       # Field metadata
│   ├── qsqlerror.h/cpp       # Error handling
│   └── qsqlindex.h/cpp       # Index information
│
├── models/                    # Model classes
│   ├── qsqlquerymodel.h/cpp  # Read-only query model
│   ├── qsqltablemodel.h/cpp  # Editable table model
│   └── qsqlrelationaltablemodel.h/cpp  # Relational model
│
├── drivers/                   # Database drivers
│   ├── sqlite/
│   │   ├── qsql_sqlite.h/cpp
│   │   └── main.cpp
│   ├── mysql/
│   │   ├── qsql_mysql.h/cpp
│   │   └── main.cpp
│   ├── psql/
│   │   ├── qsql_psql.h/cpp
│   │   └── main.cpp
│   └── odbc/
│       ├── qsql_odbc.h/cpp
│       └── main.cpp
│
└── CMakeLists.txt            # Build configuration
```

## Phân Tích Các File Quan Trọng

### 1. qsqldatabase.h/cpp

**Vai trò**: Quản lý database connections

**Cấu trúc chính**:
```cpp
class QSqlDatabase {
    // Connection registry (static)
    static QHash<QString, QSqlDatabase> dbDict;
    
    // Driver registry
    static QHash<QString, QSqlDriverCreatorBase*> drivers;
    
public:
    // Factory method
    static QSqlDatabase addDatabase(const QString &type, 
                                    const QString &name);
    
    // Connection management
    bool open();
    void close();
    bool isOpen() const;
    
    // Query execution
    QSqlQuery exec(const QString &query) const;
    
    // Transaction
    bool transaction();
    bool commit();
    bool rollback();
};
```

**Điểm quan trọng**:
- Sử dụng static hash để lưu connections
- Mỗi connection name là unique
- Thread-local storage cho thread safety

### 2. qsqldriver.h/cpp

**Vai trò**: Abstract interface cho database drivers

**Cấu trúc chính**:
```cpp
class QSqlDriver : public QObject {
public:
    // Connection
    virtual bool open(const QString &db, 
                     const QString &user = QString(),
                     const QString &password = QString(),
                     const QString &host = QString(),
                     int port = -1,
                     const QString &connOpts = QString()) = 0;
    
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    
    // Query execution
    virtual QSqlResult *createResult() const = 0;
    
    // Features
    virtual bool hasFeature(DriverFeature f) const = 0;
    
    // Transaction
    virtual bool beginTransaction();
    virtual bool commitTransaction();
    virtual bool rollbackTransaction();
    
protected:
    QSqlResult *d_func() const;
};
```

**Design Pattern**: Template Method Pattern
- Base class định nghĩa skeleton
- Derived classes implement chi tiết

### 3. qsqlresult.h/cpp

**Vai trò**: Xử lý kết quả của SQL queries

**Cấu trúc chính**:
```cpp
class QSqlResult {
public:
    // Execution
    virtual bool exec(const QString &query) = 0;
    virtual bool exec() = 0;
    
    // Navigation
    virtual bool fetch(int i) = 0;
    virtual bool fetchNext() = 0;
    virtual bool fetchPrevious() = 0;
    virtual bool fetchFirst() = 0;
    virtual bool fetchLast() = 0;
    
    // Data access
    QVariant value(int index) const;
    QVariant value(const QString &name) const;
    QSqlRecord record() const;
    
    // Status
    bool isActive() const;
    bool isValid() const;
    QSqlError lastError() const;
    
protected:
    // Cache management
    QSqlRecord rec;  // Current record cache
    QVector<QSqlRecord> cache;  // Record cache
};
```

**Điểm quan trọng**:
- Lazy evaluation: chỉ fetch khi cần
- Caching: cache records để tối ưu
- Forward-only cursor mặc định

### 4. qsqlquery.h/cpp

**Vai trò**: High-level interface để thực thi queries

**Cấu trúc chính**:
```cpp
class QSqlQuery {
public:
    // Construction
    QSqlQuery(QSqlDatabase db = QSqlDatabase());
    QSqlQuery(const QString &query, QSqlDatabase db = QSqlDatabase());
    
    // Execution
    bool exec(const QString &query);
    bool exec();
    bool prepare(const QString &query);
    
    // Binding
    void bindValue(const QString &placeholder, const QVariant &val);
    void bindValue(int pos, const QVariant &val);
    void addBindValue(const QVariant &val);
    
    // Navigation
    bool next();
    bool previous();
    bool first();
    bool last();
    bool seek(int index, bool relative = false);
    
    // Data access
    QVariant value(int index) const;
    QVariant value(const QString &name) const;
    
private:
    QSqlResult *d;  // Delegate to QSqlResult
};
```

**Điểm quan trọng**:
- Wrapper around QSqlResult
- Cung cấp convenient API
- Quản lý prepared statements

### 5. qsqltablemodel.h/cpp

**Vai trò**: Model cho database tables với editing support

**Cấu trúc chính**:
```cpp
class QSqlTableModel : public QAbstractTableModel {
public:
    // Setup
    void setTable(const QString &tableName);
    bool select();
    
    // Edit strategy
    enum EditStrategy {
        OnFieldChange,
        OnRowChange,
        OnManualSubmit
    };
    void setEditStrategy(EditStrategy strategy);
    
    // Data access
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, 
                int role) override;
    
    // Modification
    bool insertRecord(int row, const QSqlRecord &record);
    bool insertRows(int row, int count, 
                   const QModelIndex &parent) override;
    bool removeRows(int row, int count, 
                   const QModelIndex &parent) override;
    
    // Submit
    bool submitAll();
    void revertAll();
    bool submit();
    
private:
    QSqlDatabase db;
    QString tableName;
    QSqlRecord rec;  // Table structure
    QVector<QSqlRecord> cache;  // Cached records
    QHash<int, QSqlRecord> insertedRows;  // Pending inserts
    QHash<int, QSqlRecord> modifiedRows;  // Pending updates
    QSet<int> deletedRows;  // Pending deletes
};
```

**Điểm quan trọng**:
- Cache records trong memory
- Track changes (insert/update/delete)
- Generate SQL queries khi submit
- Thread-safe với proper locking

## Driver Implementation Example (SQLite)

### qsql_sqlite.h
```cpp
class QSQLiteDriver : public QSqlDriver {
    Q_OBJECT
public:
    QSQLiteDriver(QObject *parent = 0);
    ~QSQLiteDriver();
    
    bool hasFeature(DriverFeature f) const override;
    bool open(const QString &db, const QString &user,
              const QString &password, const QString &host,
              int port, const QString &connOpts) override;
    void close() override;
    QSqlResult *createResult() const override;
    
private:
    sqlite3 *access;  // SQLite handle
};
```

### qsql_sqlite.cpp - Key Methods

**open()**:
```cpp
bool QSQLiteDriver::open(const QString &db, ...) {
    int res = sqlite3_open(db.toUtf8().constData(), &access);
    if (res != SQLITE_OK) {
        setLastError(qMakeError(...));
        return false;
    }
    return true;
}
```

**createResult()**:
```cpp
QSqlResult *QSQLiteDriver::createResult() const {
    return new QSQLiteResult(this);
}
```

### qsql_sqlite_result.cpp

**exec()**:
```cpp
bool QSQLiteResult::exec(const QString &query) {
    // Prepare statement
    int res = sqlite3_prepare_v2(driver()->d->access,
                                 query.toUtf8().constData(),
                                 -1, &stmt, 0);
    if (res != SQLITE_OK) {
        setLastError(...);
        return false;
    }
    
    // Execute
    res = sqlite3_step(stmt);
    // ... handle results
    return true;
}
```

**fetchNext()**:
```cpp
bool QSQLiteResult::fetchNext() {
    if (!stmt) return false;
    
    int res = sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        // Extract values into QSqlRecord
        setAt(at() + 1);
        return true;
    }
    return false;
}
```

## Luồng Xử Lý Lỗi

```
Operation fails
    ↓
Driver/Result detects error
    ↓
Create QSqlError object
    ↓
Store in QSqlResult::lastError
    ↓
Return false from operation
    ↓
User checks lastError()
```

**QSqlError structure**:
```cpp
class QSqlError {
    ErrorType type;      // ConnectionError, StatementError, etc.
    QString driverText;  // Driver-specific message
    QString databaseText;// Database-specific message
    int number;          // Database error code
};
```

## Memory Management

### Ownership Rules

1. **QSqlDatabase**: 
   - Stored in static registry
   - Auto-cleanup khi application exit
   - Không delete manually

2. **QSqlQuery**:
   - Owns QSqlResult
   - Auto-cleanup khi destruct
   - Có thể copy (shared data)

3. **QSqlTableModel**:
   - Owns QSqlQuery
   - Cache records trong memory
   - Auto-cleanup khi destruct

4. **QSqlResult**:
   - Owned by QSqlQuery hoặc QSqlDriver
   - Không delete manually

## Thread Safety Implementation

### Connection Registry
```cpp
// Thread-local storage
static QThreadStorage<QHash<QString, QSqlDatabase> *> dbDict;

QSqlDatabase QSqlDatabase::addDatabase(...) {
    QHash<QString, QSqlDatabase> *dict = dbDict.localData();
    if (!dict) {
        dict = new QHash<QString, QSqlDatabase>;
        dbDict.setLocalData(dict);
    }
    // ...
}
```

### Driver Registry
```cpp
// Global, protected by mutex
static QMutex driverMutex;
static QHash<QString, QSqlDriverCreatorBase*> drivers;

void QSqlDatabase::registerSqlDriver(...) {
    QMutexLocker locker(&driverMutex);
    drivers[name] = creator;
}
```

## Build System Integration

### CMakeLists.txt Structure
```cmake
# Core SQL module
qt_add_library(Qt6::Sql ...)
target_sources(Qt6::Sql PRIVATE
    kernel/qsqldatabase.cpp
    kernel/qsqldriver.cpp
    # ...
)

# SQLite driver (optional)
if(QT_FEATURE_sql_sqlite)
    qt_add_plugin(Qt6::QSQLiteDriverPlugin
        SOURCES drivers/sqlite/qsql_sqlite.cpp
    )
endif()
```

## Điểm Quan Trọng Khi Đọc Source Code

1. **Follow the execution flow**: Từ QSqlQuery::exec() → QSqlDriver → QSqlResult
2. **Understand caching**: QSqlResult cache records để tối ưu
3. **Error propagation**: Lỗi được propagate từ driver → result → query
4. **Thread safety**: Mỗi thread có connection registry riêng
5. **Memory management**: Qt's parent-child ownership model
6. **Virtual methods**: Nhiều virtual methods để drivers override
7. **Prepared statements**: Được handle ở driver level

## Cách Đọc Source Code Hiệu Quả

1. **Bắt đầu từ qsqlquery.cpp**: Entry point chính
2. **Theo dõi QSqlDriver::createResult()**: Xem cách result được tạo
3. **Xem driver-specific implementation**: Ví dụ qsql_sqlite.cpp
4. **Hiểu QSqlResult flow**: fetchNext(), value(), etc.
5. **Xem model implementation**: qsqltablemodel.cpp để hiểu cách model hoạt động

---

**Lưu ý**: Source code thực tế có thể khác một chút tùy version Qt. Cấu trúc tổng thể vẫn giữ nguyên.
