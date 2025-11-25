# Qt SQL - Tài Liệu Tham Khảo Nhanh (Quick Reference)

## 🔗 Kết Nối Database

### Tạo Kết Nối
```cpp
// SQLite
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName("database.db");

// MySQL
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
db.setHostName("localhost");
db.setDatabaseName("mydb");
db.setUserName("user");
db.setPassword("pass");

// PostgreSQL
QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
db.setHostName("localhost");
db.setDatabaseName("mydb");
db.setUserName("user");
db.setPassword("pass");
db.setPort(5432);
```

### Mở & Đóng
```cpp
if (!db.open()) {
    qDebug() << db.lastError();
}

db.close();
```

### Nhiều Kết Nối
```cpp
QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE", "connection1");
QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE", "connection2");

// Sử dụng
QSqlQuery query1(db1);
QSqlQuery query2(db2);
```

---

## 📝 QSqlQuery - Thực Thi Query

### Direct Query
```cpp
QSqlQuery query;
query.exec("CREATE TABLE users (id INT, name TEXT)");
query.exec("INSERT INTO users VALUES (1, 'Alice')");
```

### Prepared Statement
```cpp
// Named placeholders
query.prepare("INSERT INTO users VALUES (:id, :name)");
query.bindValue(":id", 1);
query.bindValue(":name", "Bob");
query.exec();

// Positional placeholders
query.prepare("INSERT INTO users VALUES (?, ?)");
query.addBindValue(2);
query.addBindValue("Charlie");
query.exec();
```

### Select & Iterate
```cpp
query.exec("SELECT * FROM users");
while (query.next()) {
    int id = query.value(0).toInt();
    QString name = query.value(1).toString();
    qDebug() << id << name;
}
```

### Navigation
```cpp
query.first();      // Dòng đầu
query.last();       // Dòng cuối
query.next();       // Dòng tiếp theo
query.previous();   // Dòng trước
query.seek(5);      // Nhảy đến dòng 5
int pos = query.at(); // Vị trí hiện tại
```

### Batch Insert
```cpp
query.prepare("INSERT INTO users VALUES (?, ?)");

QVariantList ids;
ids << 1 << 2 << 3;

QVariantList names;
names << "Alice" << "Bob" << "Charlie";

query.addBindValue(ids);
query.addBindValue(names);

query.execBatch();
```

---

## 🔄 Transaction

```cpp
db.transaction();

bool success = true;
success &= query.exec("UPDATE accounts SET balance = balance - 100 WHERE id = 1");
success &= query.exec("UPDATE accounts SET balance = balance + 100 WHERE id = 2");

if (success)
    db.commit();
else
    db.rollback();
```

---

## 📊 QSqlTableModel

### Setup
```cpp
QSqlTableModel *model = new QSqlTableModel;
model->setTable("employees");

// Edit strategies
model->setEditStrategy(QSqlTableModel::OnFieldChange);   // Auto-save
model->setEditStrategy(QSqlTableModel::OnRowChange);     // Save on row change
model->setEditStrategy(QSqlTableModel::OnManualSubmit);  // Manual save

model->select();  // Load data
```

### Filter & Sort
```cpp
model->setFilter("age > 25 AND city = 'Hanoi'");
model->setSort(1, Qt::AscendingOrder);  // Sort by column 1
model->select();  // Re-query
```

### Edit Data
```cpp
// Edit cell
model->setData(model->index(row, col), newValue);

// Insert row
int row = model->rowCount();
model->insertRow(row);
model->setData(model->index(row, 0), 123);
model->setData(model->index(row, 1), "New Name");

// Delete row
model->removeRow(row);

// Submit changes (OnManualSubmit mode)
model->submitAll();

// Revert changes
model->revertAll();
```

### Connect to View
```cpp
QTableView *view = new QTableView;
view->setModel(model);
```

### Signals
```cpp
connect(model, &QSqlTableModel::beforeInsert, [](QSqlRecord &record) {
    // Modify record before insert
});

connect(model, &QSqlTableModel::beforeUpdate, [](int row, QSqlRecord &record) {
    // Modify record before update
});

connect(model, &QSqlTableModel::beforeDelete, [](int row) {
    // Confirmation dialog
});
```

---

## 📋 QSqlRecord & QSqlField

### QSqlRecord - Đại diện 1 Row
```cpp
QSqlRecord record = query.record();

// Access by index
QVariant value = record.value(0);

// Access by name
QVariant name = record.value("name");
int age = record.value("age").toInt();

// Set values
record.setValue("name", "John");
record.setValue("age", 30);

// Add field
QSqlField field("email", QMetaType::fromType<QString>());
field.setValue("john@example.com");
record.append(field);

// Info
int count = record.count();
QString fieldName = record.fieldName(0);
bool contains = record.contains("email");
```

### QSqlField - Đại diện 1 Column
```cpp
QSqlField field = record.field(0);

QString name = field.name();
QMetaType type = field.metaType();
QVariant value = field.value();

field.setValue("New Value");
field.setReadOnly(true);
field.setRequired(true);
field.setAutoValue(true);  // Auto-increment

bool isNull = field.isNull();
bool isValid = field.isValid();
```

---

## 📈 QSqlQueryModel (Read-Only)

```cpp
QSqlQueryModel *model = new QSqlQueryModel;
model->setQuery("SELECT name, age FROM users");

// Custom headers
model->setHeaderData(0, Qt::Horizontal, "Name");
model->setHeaderData(1, Qt::Horizontal, "Age");

// Access data
QVariant data = model->data(model->index(row, col));

// Connect to view
QTableView *view = new QTableView;
view->setModel(model);
```

---

## 🔗 QSqlRelationalTableModel (Foreign Keys)

```cpp
QSqlRelationalTableModel *model = new QSqlRelationalTableModel;
model->setTable("employees");

// Set foreign key relation
// employees.department_id → departments.id, display departments.name
model->setRelation(2, QSqlRelation("departments", "id", "name"));

model->select();

// Use QSqlRelationalDelegate for editing
QSqlRelationalDelegate *delegate = new QSqlRelationalDelegate(view);
view->setItemDelegate(delegate);
```

---

## 🗄️ Database Metadata

### Tables
```cpp
QStringList tables = db.tables();                    // All tables
QStringList tables = db.tables(QSql::Tables);       // Only tables
QStringList views = db.tables(QSql::Views);         // Only views
```

### Table Structure
```cpp
QSqlRecord record = db.record("users");
for (int i = 0; i < record.count(); ++i) {
    QString name = record.fieldName(i);
    QMetaType type = record.field(i).metaType();
    qDebug() << name << type.name();
}
```

### Primary Key
```cpp
QSqlIndex primaryKey = db.primaryIndex("users");
for (int i = 0; i < primaryKey.count(); ++i) {
    qDebug() << primaryKey.fieldName(i);
}
```

---

## ⚠️ Error Handling

### QSqlError
```cpp
QSqlError error = query.lastError();

if (error.isValid()) {
    qDebug() << "Type:" << error.type();
    qDebug() << "Database:" << error.databaseText();
    qDebug() << "Driver:" << error.driverText();
    qDebug() << "Native code:" << error.nativeErrorCode();
}

// Error types
QSqlError::NoError
QSqlError::ConnectionError
QSqlError::StatementError
QSqlError::TransactionError
QSqlError::UnknownError
```

---

## 🧵 Multi-Threading

```cpp
// Thread 1
void thread1() {
    QSqlDatabase db = QSqlDatabase::database("connection1");
    // Use db...
}

// Thread 2  
void thread2() {
    // Clone connection với tên khác
    QSqlDatabase db = QSqlDatabase::cloneDatabase(
        QSqlDatabase::database(), 
        "connection2"
    );
    db.open();
    // Use db...
}
```

**⚠️ Quan trọng:**
- Mỗi thread phải có connection riêng
- Không share QSqlDatabase object giữa các threads
- Không share QSqlQuery object giữa các threads

---

## 🎯 QSql Namespace

### TableType
```cpp
QSql::Tables        // Chỉ tables
QSql::Views         // Chỉ views
QSql::AllTables     // Cả tables và views
```

### ParamType
```cpp
QSql::In            // Input parameter
QSql::Out           // Output parameter
QSql::InOut         // Both
```

### NumericalPrecisionPolicy
```cpp
QSql::LowPrecisionInt32     // Convert to int
QSql::LowPrecisionInt64     // Convert to qint64
QSql::LowPrecisionDouble    // Convert to double
QSql::HighPrecision         // Keep as QString
```

---

## 🔌 Available Drivers

| Driver | Database |
|--------|----------|
| QSQLITE | SQLite 3 |
| QMYSQL | MySQL 5.0+ |
| QPSQL | PostgreSQL 7.3+ |
| QOCI | Oracle 10g+ |
| QODBC | ODBC |
| QDB2 | IBM DB2 |
| QIBASE | Borland InterBase |
| QTDS | Sybase Adaptive Server |

### Check Available Drivers
```cpp
QStringList drivers = QSqlDatabase::drivers();
qDebug() << drivers;

bool hasMySQL = QSqlDatabase::isDriverAvailable("QMYSQL");
```

---

## 🛠️ Utility Functions

### QSqlDriver Features
```cpp
QSqlDriver *driver = db.driver();

if (driver->hasFeature(QSqlDriver::Transactions))
    qDebug() << "Supports transactions";

if (driver->hasFeature(QSqlDriver::PreparedQueries))
    qDebug() << "Supports prepared queries";

if (driver->hasFeature(QSqlDriver::BLOB))
    qDebug() << "Supports BLOB";

if (driver->hasFeature(QSqlDriver::Unicode))
    qDebug() << "Supports Unicode";

if (driver->hasFeature(QSqlDriver::BatchOperations))
    qDebug() << "Supports batch operations";
```

### Format & Escape
```cpp
QSqlDriver *driver = db.driver();

QString escaped = driver->escapeIdentifier("table_name", QSqlDriver::TableName);
QString formatted = driver->formatValue(field);
```

---

## 📌 Common Patterns

### Singleton Database Manager
```cpp
class DatabaseManager {
public:
    static DatabaseManager& instance() {
        static DatabaseManager instance;
        return instance;
    }
    
    QSqlDatabase& database() {
        return m_db;
    }
    
private:
    DatabaseManager() {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("app.db");
        m_db.open();
    }
    
    ~DatabaseManager() {
        m_db.close();
    }
    
    QSqlDatabase m_db;
};

// Usage
QSqlQuery query(DatabaseManager::instance().database());
```

### Repository Pattern
```cpp
class UserRepository {
public:
    UserRepository(QSqlDatabase db) : m_db(db) {}
    
    QList<User> findAll() {
        QSqlQuery query(m_db);
        query.exec("SELECT * FROM users");
        
        QList<User> users;
        while (query.next()) {
            users.append(User{
                query.value("id").toInt(),
                query.value("name").toString()
            });
        }
        return users;
    }
    
    bool save(const User& user) {
        QSqlQuery query(m_db);
        query.prepare("INSERT INTO users VALUES (:id, :name)");
        query.bindValue(":id", user.id);
        query.bindValue(":name", user.name);
        return query.exec();
    }
    
private:
    QSqlDatabase m_db;
};
```

### Safe Transaction Wrapper
```cpp
class Transaction {
public:
    Transaction(QSqlDatabase db) : m_db(db), m_committed(false) {
        m_db.transaction();
    }
    
    ~Transaction() {
        if (!m_committed)
            m_db.rollback();
    }
    
    void commit() {
        m_db.commit();
        m_committed = true;
    }
    
private:
    QSqlDatabase m_db;
    bool m_committed;
};

// Usage (RAII pattern)
{
    Transaction trans(db);
    
    // Do work...
    query.exec("UPDATE ...");
    
    trans.commit();  // If not called, auto-rollback in destructor
}
```

---

## 🐛 Debug Tips

```cpp
// Enable SQL logging
query.exec();
qDebug() << "Last query:" << query.lastQuery();
qDebug() << "Executed query:" << query.executedQuery();

// Check bound values
qDebug() << "Bound values:" << query.boundValues();

// Check result size
qDebug() << "Result size:" << query.size();
qDebug() << "Rows affected:" << query.numRowsAffected();

// Detailed error info
if (!query.exec()) {
    QSqlError err = query.lastError();
    qDebug() << "Error type:" << err.type();
    qDebug() << "DB text:" << err.databaseText();
    qDebug() << "Driver text:" << err.driverText();
    qDebug() << "Native:" << err.nativeErrorCode();
}
```

---

## 📚 References

- **Header Files**: `<QSqlDatabase>`, `<QSqlQuery>`, `<QSqlTableModel>`, etc.
- **Qt Docs**: https://doc.qt.io/qt-6/sql-programming.html
- **Source Code**: `qtbase/src/sql/`

---

**Lưu ý:** Tài liệu này dựa trên Qt 6. Một số APIs có thể khác ở Qt 5.
