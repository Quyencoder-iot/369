# QtDatabase - Hướng Dẫn Tham Khảo Nhanh

## Sơ Đồ Kiến Trúc Trực Quan

```
┌─────────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                        │
│  (Your Qt Application Code)                                 │
└───────────────────────┬─────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│                   Qt SQL API LAYER                          │
│                                                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │QSqlDatabase  │  │ QSqlQuery    │  │QSqlTableModel│     │
│  │              │  │              │  │              │     │
│  │• Connections │  │• Execute SQL │  │• Table View  │     │
│  │• Driver Mgmt │  │• Fetch Data  │  │• CRUD Ops    │     │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘     │
│         │                  │                  │             │
│         └──────────────────┼──────────────────┘             │
│                            │                                 │
│                            ▼                                 │
│                   ┌──────────────┐                           │
│                   │ QSqlDriver   │                           │
│                   │ (Interface)  │                           │
│                   └──────┬───────┘                           │
└──────────────────────────┼───────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                    DRIVER LAYER                              │
│                                                              │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │QSQLITE   │  │ QMYSQL   │  │ QPSQL    │  │ QODBC    │   │
│  │          │  │          │  │          │  │          │   │
│  │SQLite    │  │MySQL     │  │PostgreSQL│  │ODBC      │   │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘   │
│       │             │             │             │          │
│       └─────────────┼─────────────┼─────────────┘          │
│                     │             │                         │
│                     ▼             ▼                         │
│              ┌──────────────────────────┐                   │
│              │    QSqlResult            │                   │
│              │  (Result Processing)     │                   │
│              └──────────┬───────────────┘                   │
└─────────────────────────┼───────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                 NATIVE DATABASE APIs                         │
│  (sqlite3, libmysqlclient, libpq, ODBC drivers)             │
└─────────────────────────────────────────────────────────────┘
```

## Class Hierarchy

```
QObject
  └── QAbstractItemModel
        ├── QSqlQueryModel (read-only)
        ├── QSqlTableModel (editable table)
        └── QSqlRelationalTableModel (with relations)

QSqlDriver (abstract)
  ├── QSQLiteDriver
  ├── QMYSQLDriver
  ├── QPSQLDriver
  └── QODBCDriver

QSqlResult (abstract)
  ├── QSQLiteResult
  ├── QMYSQLResult
  ├── QPSQLResult
  └── QODBCResult
```

## Luồng Dữ Liệu Chi Tiết

### 1. Connection Flow
```
addDatabase("QSQLITE", "conn1")
    ↓
QSqlDatabasePrivate::registerSqlDriver()
    ↓
Create driver instance
    ↓
Store in connection registry
    ↓
setDatabaseName("db.db")
    ↓
open()
    ↓
QSqlDriver::open()
    ↓
Native DB: sqlite3_open()
```

### 2. Query Execution Flow
```
QSqlQuery::exec("SELECT * FROM users")
    ↓
QSqlQuery::exec() [qsqlquery.cpp]
    ↓
QSqlDriver::createResult()
    ↓
QSqlResult::exec() [driver-specific]
    ↓
Native API: Execute query
    ↓
QSqlResult::fetchNext()
    ↓
Cache record in QSqlRecord
    ↓
QSqlQuery::next()
    ↓
QSqlQuery::value(index)
```

### 3. Model Update Flow
```
QSqlTableModel::select()
    ↓
Clear cache
    ↓
Build SELECT query
    ↓
QSqlQuery::exec()
    ↓
Fetch all records
    ↓
Cache in QVector<QSqlRecord>
    ↓
beginResetModel()
    ↓
endResetModel()
    ↓
Views update automatically
```

## Cheat Sheet - Các Phương Thức Quan Trọng

### QSqlDatabase
```cpp
// Static methods
static QSqlDatabase addDatabase(const QString &type, 
                                const QString &connectionName = ...)
static QSqlDatabase database(const QString &connectionName = ...)
static QStringList drivers()
static void removeDatabase(const QString &connectionName)

// Instance methods
bool open()
void close()
bool isOpen() const
QSqlError lastError() const
QSqlDriver* driver() const
```

### QSqlQuery
```cpp
// Execution
bool exec(const QString &query)
bool exec()
bool prepare(const QString &query)

// Navigation
bool next()
bool previous()
bool first()
bool last()
bool seek(int index, bool relative = false)

// Data access
QVariant value(int index) const
QVariant value(const QString &name) const
QSqlRecord record() const

// Binding
void bindValue(const QString &placeholder, const QVariant &val)
void addBindValue(const QVariant &val)
void bindValue(int pos, const QVariant &val)

// Status
bool isActive() const
bool isValid() const
QSqlError lastError() const
int size() const
```

### QSqlTableModel
```cpp
// Setup
void setTable(const QString &tableName)
bool select()
void setEditStrategy(EditStrategy strategy)

// Data access
QVariant data(const QModelIndex &index, int role) const
bool setData(const QModelIndex &index, const QVariant &value, int role)

// Modification
bool insertRecord(int row, const QSqlRecord &record)
bool insertRows(int row, int count, const QModelIndex &parent)
bool removeRows(int row, int count, const QModelIndex &parent)

// Submit
bool submitAll()
void revertAll()
bool submit()

// Filtering/Sorting
void setFilter(const QString &filter)
void setSort(int column, Qt::SortOrder order)
```

## Các Lỗi Thường Gặp và Cách Xử Lý

### 1. Connection Error
```cpp
QSqlError error = db.lastError();
if (error.type() == QSqlError::ConnectionError) {
    // Database không tồn tại, không thể kết nối
    qDebug() << "Connection failed:" << error.text();
}
```

### 2. Query Syntax Error
```cpp
if (!query.exec("SELECT * FROM users")) {
    QSqlError error = query.lastError();
    if (error.type() == QSqlError::StatementError) {
        qDebug() << "SQL Error:" << error.text();
        qDebug() << "Query:" << query.executedQuery();
    }
}
```

### 3. Transaction Error
```cpp
if (!db.transaction()) {
    qDebug() << "Failed to start transaction";
    return;
}
// ... operations ...
if (!db.commit()) {
    db.rollback();
    qDebug() << "Transaction failed, rolled back";
}
```

## Performance Tips

### 1. Sử dụng Prepared Statements
```cpp
// ❌ Chậm - parse mỗi lần
for (const QString &name : names) {
    query.exec("INSERT INTO users (name) VALUES ('" + name + "')");
}

// ✅ Nhanh - parse một lần
query.prepare("INSERT INTO users (name) VALUES (?)");
for (const QString &name : names) {
    query.bindValue(0, name);
    query.exec();
}
```

### 2. Batch Transactions
```cpp
// ❌ Chậm - commit mỗi lần
for (const QString &name : names) {
    query.exec("INSERT INTO users (name) VALUES ('" + name + "')");
}

// ✅ Nhanh - commit một lần
db.transaction();
query.prepare("INSERT INTO users (name) VALUES (?)");
for (const QString &name : names) {
    query.bindValue(0, name);
    query.exec();
}
db.commit();
```

### 3. Chỉ Fetch Cần Thiết
```cpp
// ❌ Fetch tất cả
query.exec("SELECT * FROM users");
while (query.next()) {
    // process
}

// ✅ Chỉ fetch cần thiết
query.exec("SELECT id, name FROM users WHERE active = 1");
while (query.next()) {
    // process
}
```

## Code Patterns Thường Dùng

### Pattern 1: Safe Query Execution
```cpp
QSqlQuery query(db);
if (!query.prepare("SELECT * FROM users WHERE id = ?")) {
    qWarning() << "Prepare failed:" << query.lastError();
    return;
}
query.bindValue(0, userId);
if (!query.exec()) {
    qWarning() << "Execute failed:" << query.lastError();
    return;
}
while (query.next()) {
    // process results
}
```

### Pattern 2: Model với Filter
```cpp
QSqlTableModel *model = new QSqlTableModel(this, db);
model->setTable("users");
model->setFilter("active = 1");
model->setSort(1, Qt::AscendingOrder);
model->select();
```

### Pattern 3: Custom Query Model
```cpp
class CustomModel : public QSqlQueryModel {
    Q_OBJECT
public:
    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            // Custom formatting
            QVariant value = QSqlQueryModel::data(index, role);
            return formatValue(value);
        }
        return QSqlQueryModel::data(index, role);
    }
};
```

## Debugging Commands

```bash
# Enable SQL debugging
export QT_LOGGING_RULES="qt.sql.drivers.debug=true"

# Check available drivers
qDebug() << QSqlDatabase::drivers();

# Print last query
qDebug() << query.executedQuery();
qDebug() << query.boundValues();

# Check connection status
qDebug() << "Open:" << db.isOpen();
qDebug() << "Valid:" << db.isValid();
```

## Checklist Khi Làm Việc với QtDatabase

- [ ] Kiểm tra driver có sẵn: `QSqlDatabase::drivers()`
- [ ] Kiểm tra connection mở thành công: `db.isOpen()`
- [ ] Kiểm tra lỗi sau mỗi operation: `lastError()`
- [ ] Sử dụng prepared statements cho queries lặp lại
- [ ] Dùng transactions cho batch operations
- [ ] Đóng connections khi không dùng
- [ ] Mỗi thread có connection riêng
- [ ] Gọi `model->select()` sau khi thay đổi DB từ bên ngoài
- [ ] Kiểm tra `query.isValid()` trước khi đọc data
- [ ] Sử dụng `query.value()` với index hoặc tên cột

---

**Lưu ý**: Tài liệu này là quick reference. Xem `QTDATABASE_ANALYSIS.md` để hiểu sâu hơn về kiến trúc.
