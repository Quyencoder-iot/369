# Hướng Dẫn Đọc Code Nguồn Qt SQL - Từng Bước Chi Tiết

## 📖 Phương Pháp Đọc Source Code Hiệu Quả

### Bước 1: Đọc Header Files Trước (.h)
**Tại sao?** Header files chứa:
- Class declarations
- Public API
- Documentation comments
- Relationships giữa các classes

### Bước 2: Đọc Implementation Files (.cpp)
**Sau khi** hiểu interface, đọc implementation để hiểu:
- Chi tiết thuật toán
- Internal state management
- Error handling logic

### Bước 3: Đọc Private Headers (*_p.h)
- Private implementation details
- Internal helper classes
- Performance optimizations

---

## 🗂️ Roadmap Đọc Code - Theo Thứ Tự

### Level 1: Basic Classes (Bắt đầu từ đây)

#### 1. QSqlField (Đơn giản nhất)
📁 **File:** `kernel/qsqlfield.h` và `kernel/qsqlfield.cpp`

**Đọc theo thứ tự:**

```cpp
// qsqlfield.h - Public interface
class QSqlField {
    // 1. Đọc constructor - hiểu cách khởi tạo
    explicit QSqlField(const QString& fieldName, QMetaType type);
    
    // 2. Đọc các getters/setters - hiểu properties
    void setValue(const QVariant& value);
    QVariant value() const;
    void setName(const QString& name);
    QString name() const;
    
    // 3. Đọc metadata methods
    void setMetaType(QMetaType type);
    QMetaType metaType() const;
    void setRequired(bool required);
    bool isNull() const;
    
private:
    // 4. Chú ý data members
    QVariant val;
    QExplicitlySharedDataPointer<QSqlFieldPrivate> d;
};
```

**Key Insights từ QSqlField:**
- Sử dụng **Pimpl Pattern** (d pointer) để hide implementation
- **Implicit Sharing** với `QExplicitlySharedDataPointer` (copy-on-write)
- Lưu value trực tiếp trong `QVariant val`

---

#### 2. QSqlRecord (Container của QSqlField)
📁 **File:** `kernel/qsqlrecord.h` và `kernel/qsqlrecord.cpp`

**Hiểu implementation:**

```cpp
// qsqlrecord.cpp
QVariant QSqlRecord::value(int index) const
{
    // 1. Boundary check
    if (!d || index < 0 || index >= d->fields.count())
        return QVariant();
    
    // 2. Return field value
    return d->fields.at(index).value();
}

void QSqlRecord::setValue(int index, const QVariant &val)
{
    // 1. Boundary check
    if (!d || index < 0 || index >= d->fields.count())
        return;
    
    // 2. Detach for copy-on-write
    detach();
    
    // 3. Set value
    d->fields[index].setValue(val);
}
```

**Key Patterns:**
```cpp
// Copy-on-Write (COW) pattern
void QSqlRecord::detach()
{
    if (d)
        d.detach();
}

// Implicit sharing
QSqlRecord::QSqlRecord(const QSqlRecord &other)
    : d(other.d)  // Chỉ copy pointer, không copy data
{
}
```

---

#### 3. QSqlError (Error Handling)
📁 **File:** `kernel/qsqlerror.h`

**Structure:**
```cpp
class QSqlError {
public:
    enum ErrorType {
        NoError,
        ConnectionError,
        StatementError,
        TransactionError,
        UnknownError
    };
    
private:
    QString m_databaseText;    // Error từ database
    QString m_driverText;      // Error từ driver
    QString m_nativeErrorCode; // Native error code
    ErrorType m_type;
};
```

**Usage trong code:**
```cpp
// Trong QSqlResult::exec()
if (error_occurred) {
    setLastError(QSqlError(
        "Execution failed",          // driver text
        native_error_message,        // database text
        QSqlError::StatementError,   // type
        native_code                  // native code
    ));
    return false;
}
```

---

### Level 2: Core Query Classes

#### 4. QSqlDriver (Abstract Base)
📁 **File:** `kernel/qsqldriver.h` và `kernel/qsqldriver.cpp`

**Phân tích abstract interface:**

```cpp
class QSqlDriver : public QObject {
    // Pure virtual methods - Subclass phải implement
    virtual bool open(...) = 0;
    virtual void close() = 0;
    virtual QSqlResult *createResult() const = 0;
    virtual bool hasFeature(DriverFeature f) const = 0;
    
    // Virtual methods - Có default implementation, có thể override
    virtual bool beginTransaction();
    virtual bool commitTransaction();
    virtual bool rollbackTransaction();
    virtual QStringList tables(QSql::TableType type) const;
    
    // Non-virtual methods - Utility functions
    QString connectionName() const;
    QSqlError lastError() const;
};
```

**Flow phân tích:**
1. Đọc `DriverFeature` enum → hiểu capabilities
2. Đọc pure virtual methods → hiểu minimum requirements
3. Đọc virtual methods → hiểu optional features
4. Đọc helper methods → hiểu utilities

**Example Implementation Study:**
```cpp
// Trong MySQL driver (src/plugins/sqldrivers/mysql/)
class QMYSQLDriver : public QSqlDriver {
    bool hasFeature(DriverFeature f) const override {
        switch (f) {
        case Transactions:
            return d->mysql && 
                   mysql_get_server_version(d->mysql) >= 40100;
        case PreparedQueries:
            return d->preparedQuerysEnabled;
        case BLOB:
            return true;
        // ...
        }
    }
};
```

---

#### 5. QSqlResult (Query Result Backend)
📁 **File:** `kernel/qsqlresult.h` và `kernel/qsqlresult.cpp`

**Hierarchy:**
```
QSqlResult (abstract base)
    ├── QSqlCachedResult (caching layer)
    │   ├── QSQLiteResult
    │   ├── QMYSQLResult
    │   └── QPSQLResult
    └── Direct implementations
```

**State Machine:**
```cpp
class QSqlResult {
protected:
    int m_at;           // Current cursor position (-1 = before first)
    bool m_active;      // Query is active?
    bool m_select;      // Is it a SELECT query?
    bool m_forwardOnly; // Forward-only cursor?
    QString m_lastQuery;
    QSqlError m_error;
};
```

**Flow trong exec():**
```cpp
bool QSqlResult::exec()
{
    // 1. Validate state
    if (m_lastQuery.isEmpty())
        return false;
    
    // 2. Bind parameters
    if (!bindParameters())
        return false;
    
    // 3. Execute (driver-specific)
    bool success = reset(m_lastQuery);
    
    // 4. Update state
    if (success) {
        setActive(true);
        setAt(QSql::BeforeFirstRow);
    }
    
    return success;
}
```

**Key Method - fetch():**
```cpp
// Abstract method - driver implements
virtual bool fetch(int index) = 0;

// Used by navigation methods
bool QSqlResult::fetchNext()
{
    return fetch(at() + 1);
}

bool QSqlResult::fetchFirst()
{
    return fetch(QSql::FirstRow);
}
```

---

#### 6. QSqlQuery (High-Level Interface)
📁 **File:** `kernel/qsqlquery.h` và `kernel/qsqlquery.cpp`

**Wrapper Pattern:**
```cpp
class QSqlQuery {
private:
    QSqlQueryPrivate *d;  // Pimpl pattern
};

struct QSqlQueryPrivate {
    QSqlResult *sqlResult;  // Actual backend
};
```

**Method delegation:**
```cpp
bool QSqlQuery::exec(const QString &query)
{
    // Delegate to QSqlResult
    d->sqlResult->setQuery(query);
    return d->sqlResult->exec();
}

bool QSqlQuery::next()
{
    // Delegate to QSqlResult
    return d->sqlResult->fetchNext();
}

QVariant QSqlQuery::value(int index) const
{
    // Delegate to QSqlResult
    return d->sqlResult->data(index);
}
```

**Prepared Statement Flow:**
```cpp
// 1. Prepare
bool QSqlQuery::prepare(const QString &query)
{
    // Store query with placeholders
    return d->sqlResult->prepare(query);
}

// 2. Bind
void QSqlQuery::bindValue(const QString &placeholder, const QVariant &val)
{
    d->sqlResult->bindValue(placeholder, val, QSql::In);
}

// 3. Execute
bool QSqlQuery::exec()
{
    // Execute prepared query with bound values
    return d->sqlResult->exec();
}
```

---

#### 7. QSqlDatabase (Connection Manager)
📁 **File:** `kernel/qsqldatabase.h` và `kernel/qsqldatabase.cpp`

**Singleton Manager Pattern:**
```cpp
// Global connection dictionary
typedef QMap<QString, QSqlDatabasePrivate*> ConnectionDict;
Q_GLOBAL_STATIC(ConnectionDict, dbDict)

QSqlDatabase QSqlDatabase::addDatabase(const QString &type,
                                        const QString &connectionName)
{
    // 1. Check if connection exists
    if (dbDict()->contains(connectionName))
        qWarning("Connection already exists");
    
    // 2. Create driver
    QSqlDriver *driver = nullptr;
    if (type == "QSQLITE")
        driver = new QSQLiteDriver();
    else if (type == "QMYSQL")
        driver = new QMYSQLDriver();
    // ...
    
    // 3. Store in dictionary
    QSqlDatabasePrivate *d = new QSqlDatabasePrivate(driver);
    dbDict()->insert(connectionName, d);
    
    return QSqlDatabase(d);
}
```

**Transaction Management:**
```cpp
bool QSqlDatabase::transaction()
{
    if (!driver()->hasFeature(QSqlDriver::Transactions)) {
        qWarning("Database doesn't support transactions");
        return false;
    }
    
    return driver()->beginTransaction();
}

bool QSqlDatabase::commit()
{
    return driver()->commitTransaction();
}

bool QSqlDatabase::rollback()
{
    return driver()->rollbackTransaction();
}
```

---

### Level 3: Model-View Integration

#### 8. QSqlQueryModel (Read-Only Model)
📁 **File:** `models/qsqlquerymodel.h` và `models/qsqlquerymodel.cpp`

**Extends QAbstractTableModel:**
```cpp
class QSqlQueryModel : public QAbstractTableModel {
private:
    QSqlQuery query;
    QSqlRecord rec;
};
```

**QAbstractItemModel Implementation:**
```cpp
int QSqlQueryModel::rowCount(const QModelIndex &parent) const
{
    return query.size();
}

int QSqlQueryModel::columnCount(const QModelIndex &parent) const
{
    return rec.count();
}

QVariant QSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        // Seek to row
        QSqlQuery q = query;
        q.seek(index.row());
        
        // Return column value
        return q.value(index.column());
    }
    
    return QVariant();
}
```

---

#### 9. QSqlTableModel (Editable Model)
📁 **File:** `models/qsqltablemodel.h` và `models/qsqltablemodel.cpp`

**Caching Strategy:**
```cpp
class QSqlTableModelPrivate {
    struct ModifiedRow {
        enum Op { Insert, Update, Delete };
        Op op;
        QSqlRecord rec;
    };
    
    QMap<int, ModifiedRow> cache;  // Cached changes
    EditStrategy strategy;
};
```

**Edit Flow:**
```cpp
bool QSqlTableModel::setData(const QModelIndex &index, 
                              const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    
    // 1. Cache the change
    d->cache[index.row()].rec.setValue(index.column(), value);
    d->cache[index.row()].op = QSqlTableModelPrivate::Update;
    
    // 2. Emit signal
    emit dataChanged(index, index);
    
    // 3. Submit based on strategy
    if (d->strategy == OnFieldChange)
        return submit();
    
    return true;
}
```

**Submit Implementation:**
```cpp
bool QSqlTableModel::submitAll()
{
    for (auto it = d->cache.begin(); it != d->cache.end(); ++it) {
        int row = it.key();
        ModifiedRow &mod = it.value();
        
        switch (mod.op) {
        case Insert:
            if (!insertRowIntoTable(mod.rec))
                return false;
            break;
            
        case Update:
            if (!updateRowInTable(row, mod.rec))
                return false;
            break;
            
        case Delete:
            if (!deleteRowFromTable(row))
                return false;
            break;
        }
    }
    
    d->cache.clear();
    return select();  // Refresh
}
```

**SQL Generation:**
```cpp
QString QSqlTableModel::selectStatement() const
{
    QString query = "SELECT ";
    
    // Column list
    for (int i = 0; i < d->rec.count(); ++i) {
        if (i > 0) query += ", ";
        query += d->rec.fieldName(i);
    }
    
    query += " FROM " + d->tableName;
    
    // WHERE clause
    if (!d->filter.isEmpty())
        query += " WHERE " + d->filter;
    
    // ORDER BY clause
    query += orderByClause();
    
    return query;
}
```

---

## 🔍 Phân Tích Deep-Dive: Prepared Statement Internals

### Flow Chart
```
QSqlQuery::prepare("INSERT INTO users VALUES (?, ?)")
    │
    ├─→ QSqlResult::prepare(sql)
    │       │
    │       ├─→ Parse placeholders
    │       │   • Find all '?' or ':name' tokens
    │       │   • Store positions
    │       │
    │       └─→ Call driver's prepare
    │           • MySQL: mysql_stmt_prepare()
    │           • SQLite: sqlite3_prepare_v2()
    │
QSqlQuery::bindValue(0, "John")
QSqlQuery::bindValue(1, 25)
    │
    ├─→ QSqlResult::bindValue(pos, val)
    │       │
    │       └─→ Store in QVector<QVariant> boundValues
    │
QSqlQuery::exec()
    │
    └─→ QSqlResult::exec()
            │
            ├─→ Replace placeholders with bound values
            │   OR
            │   └─→ Use native binding (if driver supports)
            │       • MySQL: mysql_stmt_bind_param()
            │       • SQLite: sqlite3_bind_*()
            │
            └─→ Execute prepared statement
```

### Code Example: Parameter Binding

```cpp
// qsqlresult.cpp
bool QSqlResult::exec()
{
    // Get bound values
    const QVector<QVariant> &values = d->values;
    
    if (d->binds == NamedBinding) {
        // Named placeholders: :name
        QString query = d->sqlQuery;
        for (const auto &[name, value] : d->boundValues) {
            QString placeholder = ":" + name;
            QString escaped = driver()->formatValue(value);
            query.replace(placeholder, escaped);
        }
        return reset(query);
        
    } else {
        // Positional placeholders: ?
        QString query = d->sqlQuery;
        for (int i = 0; i < values.size(); ++i) {
            QString escaped = driver()->formatValue(values[i]);
            query.replace("?", escaped, 1);  // Replace first occurrence
        }
        return reset(query);
    }
}
```

---

## 🎯 Code Reading Checklist

### Khi đọc 1 class mới:

- [ ] **Constructor**: Khởi tạo state như thế nào?
- [ ] **Destructor**: Cleanup gì? (RAII pattern)
- [ ] **Copy/Move**: Có copy-on-write không?
- [ ] **Public Methods**: API cung cấp gì?
- [ ] **Virtual Methods**: Cho phép customize gì?
- [ ] **Private Members**: Internal state gì?
- [ ] **Signals**: Emit events nào?
- [ ] **Thread Safety**: Thread-safe không?
- [ ] **Error Handling**: Xử lý lỗi thế nào?

### Khi trace 1 feature:

1. **Start từ user API** (VD: `QSqlQuery::exec()`)
2. **Follow call chain** (Dùng IDE "Go to Definition")
3. **Identify abstractions** (Interface vs Implementation)
4. **Find driver implementation** (VD: MySQL driver)
5. **Understand state changes** (Trước/sau method call)

---

## 🧪 Debugging Techniques

### 1. Add Debug Output
```cpp
// Thêm vào qsqlquery.cpp để trace
bool QSqlQuery::exec(const QString &query)
{
    qDebug() << "Executing:" << query;
    bool result = d->sqlResult->exec(query);
    qDebug() << "Result:" << result;
    if (!result)
        qDebug() << "Error:" << lastError();
    return result;
}
```

### 2. Breakpoint Strategy
```
Đặt breakpoints tại:
1. Entry point: QSqlQuery::exec()
2. Driver dispatch: QSqlDriver::createResult()
3. Native call: mysql_query() / sqlite3_exec()
4. Result handling: QSqlResult::fetch()
5. Error handling: QSqlResult::setLastError()
```

### 3. Watch Variables
```cpp
// Quan sát:
- QSqlResult::m_at (cursor position)
- QSqlResult::m_active (query state)
- QSqlDatabasePrivate::driver (which driver?)
- QSqlTableModelPrivate::cache (cached changes)
```

---

## 📚 Recommended Reading Order

### For Beginners:
1. ✅ `qsqlfield.h` - Simplest class
2. ✅ `qsqlrecord.h` - Container class
3. ✅ `qsqlerror.h` - Error handling
4. ✅ `qsqlquery.h` - Main user API
5. ✅ `qsqldatabase.h` - Connection management

### For Advanced:
1. ✅ `qsqldriver.h` - Abstract interface
2. ✅ `qsqlresult.h` - Backend implementation
3. ✅ `qsqlcachedresult_p.h` - Caching strategy
4. ✅ `qsqltablemodel.cpp` - Complex model logic
5. ✅ Driver implementations trong `src/plugins/sqldrivers/`

### For Experts (Custom Driver Development):
1. ✅ `qsqldriverplugin.h` - Plugin interface
2. ✅ `src/plugins/sqldrivers/sqlite/` - Simplest driver
3. ✅ `src/plugins/sqldrivers/mysql/` - Full-featured driver
4. ✅ `qsqlcachedresult.cpp` - Caching helper

---

## 🔧 Tools for Reading Source Code

### 1. Qt Creator
- Go to Definition (F2)
- Find Usages (Ctrl+Shift+U)
- Follow Symbol (Ctrl+K, Ctrl+S)
- Class Hierarchy View

### 2. VS Code với C++ Extension
- Right-click → Go to Definition
- Right-click → Find All References
- Outline view (Ctrl+Shift+O)

### 3. Doxygen
```bash
cd qtbase/src/sql
doxygen  # Generate documentation
```

### 4. grep/ripgrep
```bash
# Find all uses of QSqlQuery::exec
rg "QSqlQuery::exec" --type cpp

# Find all classes inheriting QSqlDriver
rg "class.*:.*public QSqlDriver"
```

---

## 💡 Key Insights

### Design Principles Found in Qt SQL:

1. **Separation of Concerns**
   - QSqlQuery: User interface
   - QSqlResult: Backend logic
   - QSqlDriver: Database-specific code

2. **Dependency Inversion**
   - High-level code depends on abstractions (QSqlDriver)
   - Low-level code implements abstractions (QMYSQLDriver)

3. **Open/Closed Principle**
   - Mở cho extension (new drivers)
   - Đóng cho modification (core không đổi)

4. **Resource Management (RAII)**
   - QSqlDatabase auto-closes connection
   - QSqlQuery auto-cleans resources

5. **Lazy Initialization**
   - Connection không mở cho đến khi dùng
   - Query result không fetch cho đến khi iterate

---

## 🎓 Exercises

### Exercise 1: Trace a Simple Query
Đọc code và vẽ flow chart cho:
```cpp
QSqlQuery query;
query.exec("SELECT * FROM users");
while (query.next()) {
    qDebug() << query.value(0);
}
```

### Exercise 2: Understand Prepared Statement
Tìm trong code:
- Nơi parse placeholders
- Nơi lưu bound values
- Nơi replace placeholders với values

### Exercise 3: Analyze Transaction
Trace code khi gọi:
```cpp
db.transaction();
query.exec("UPDATE ...");
db.commit();
```

### Exercise 4: Model-View Sync
Hiểu cách `QSqlTableModel` sync changes:
- Khi nào cache changes?
- Khi nào submit to database?
- Khi nào refresh from database?

---

**Good luck with your code exploration! 🚀**

Nhớ: Đọc code không phải đọc từ đầu đến cuối, mà là **follow the flow of execution** từ API mà bạn quan tâm.
