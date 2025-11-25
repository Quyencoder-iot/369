# Qt SQL Architecture - Visual Diagrams

## 📊 Sơ Đồ Kiến Trúc Chi Tiết

---

## 1. Tổng Quan Kiến Trúc (High-Level)

```
┌─────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                         │
│                                                                  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐  │
│  │  QSqlQuery   │  │QSqlTableModel│  │ QSqlRelationalTable  │  │
│  │              │  │              │  │       Model          │  │
│  │ (Direct SQL) │  │ (Editable)   │  │  (Foreign Keys)      │  │
│  └──────┬───────┘  └──────┬───────┘  └──────────┬───────────┘  │
│         │                 │                      │              │
└─────────┼─────────────────┼──────────────────────┼──────────────┘
          │                 │                      │
          └─────────────────┴──────────────────────┘
                            │
┌───────────────────────────┼──────────────────────────────────────┐
│                           │   ABSTRACTION LAYER                   │
│                           │                                       │
│                    ┌──────▼──────┐                                │
│                    │QSqlDatabase │◄────── Connection Manager      │
│                    │             │        (Singleton Pattern)     │
│                    └──────┬──────┘                                │
│                           │                                       │
│           ┌───────────────┼───────────────┐                       │
│           │               │               │                       │
│     ┌─────▼─────┐  ┌─────▼─────┐  ┌─────▼─────┐                 │
│     │QSqlDriver │  │QSqlResult │  │ QSqlError │                 │
│     │(Abstract) │  │(Abstract) │  │           │                 │
│     └─────┬─────┘  └─────┬─────┘  └───────────┘                 │
│           │              │                                        │
└───────────┼──────────────┼────────────────────────────────────────┘
            │              │
            │ Plugin       │ Implements
            │ System       │
            │              │
┌───────────┼──────────────┼────────────────────────────────────────┐
│           │              │         DRIVER LAYER                   │
│           │              │                                        │
│    ┌──────▼──────┐ ┌─────▼─────┐                                 │
│    │MySQL Driver │ │MySQL      │                                 │
│    │             │ │Result     │                                 │
│    └─────────────┘ └───────────┘                                 │
│                                                                   │
│    ┌──────────────┐ ┌───────────┐                                │
│    │SQLite Driver│ │SQLite     │                                 │
│    │             │ │Result     │                                 │
│    └─────────────┘ └───────────┘                                 │
│                                                                   │
│    ┌──────────────┐ ┌───────────┐                                │
│    │PostgreSQL   │ │PostgreSQL │                                 │
│    │Driver       │ │Result     │                                 │
│    └──────┬──────┘ └─────┬─────┘                                 │
│           │              │                                        │
└───────────┼──────────────┼────────────────────────────────────────┘
            │              │
            ▼              ▼
    ┌────────────────────────────┐
    │  Native Database Libraries  │
    │  (libmysqlclient,          │
    │   libpq, sqlite3, etc.)    │
    └────────────────────────────┘
```

---

## 2. Class Relationship Diagram

```
                    QObject
                       │
                       │ inherits
                       │
                 ┌─────▼──────┐
                 │ QSqlDriver │
                 │ (abstract) │
                 └─────┬──────┘
                       │
         ┌─────────────┼─────────────┐
         │             │             │
    ┌────▼────┐   ┌────▼────┐  ┌────▼────┐
    │MySQL    │   │SQLite   │  │PostgreSQL│
    │Driver   │   │Driver   │  │Driver   │
    └─────────┘   └─────────┘  └─────────┘


    ┌──────────────┐
    │  QSqlResult  │
    │  (abstract)  │
    └──────┬───────┘
           │
           ├─────────────────┐
           │                 │
    ┌──────▼──────┐   ┌──────▼──────────┐
    │QSqlCached   │   │Direct           │
    │Result       │   │Implementations  │
    │(helper)     │   │                 │
    └──────┬──────┘   └─────────────────┘
           │
           ├──────────┬───────────┐
           │          │           │
      ┌────▼───┐ ┌────▼───┐ ┌────▼───┐
      │MySQL   │ │SQLite  │ │PostgreSQL│
      │Result  │ │Result  │ │Result  │
      └────────┘ └────────┘ └────────┘


    ┌──────────────┐
    │ QSqlDatabase │  ◄────── manages ────┐
    │ (value type) │                      │
    └──────┬───────┘                      │
           │ contains                     │
           │                              │
    ┌──────▼──────────────┐         ┌─────▼─────┐
    │QSqlDatabasePrivate  │────────►│QSqlDriver │
    │(d-pointer)          │ owns    │           │
    └─────────────────────┘         └───────────┘


    ┌──────────────┐
    │  QSqlQuery   │  ◄───── uses ────┐
    │ (value type) │                  │
    └──────┬───────┘                  │
           │ contains                 │
           │                          │
    ┌──────▼────────────┐       ┌─────▼──────┐
    │QSqlQueryPrivate   │──────►│ QSqlResult │
    │(d-pointer)        │ owns  │            │
    └───────────────────┘       └────────────┘


    ┌──────────────┐        contains       ┌──────────────┐
    │  QSqlRecord  │◄──────────────────────│  QSqlField   │
    │              │        (many)         │              │
    └──────────────┘                       └──────────────┘
    
    
    QAbstractItemModel
           │
           │ inherits
           │
    ┌──────▼───────────┐
    │ QSqlQueryModel   │
    │  (read-only)     │
    └──────┬───────────┘
           │
           │ inherits
           │
    ┌──────▼──────────┐
    │ QSqlTableModel  │
    │  (editable)     │
    └──────┬──────────┘
           │
           │ inherits
           │
    ┌──────▼──────────────────────┐
    │ QSqlRelationalTableModel    │
    │ (with foreign keys)          │
    └──────────────────────────────┘
```

---

## 3. Query Execution Flow

```
User Code: query.exec("SELECT * FROM users WHERE age > 25")
    │
    ▼
┌───────────────────────────────────────────────────────┐
│ QSqlQuery::exec(QString)                              │
│ • Validate connection                                 │
│ • Get QSqlResult from QSqlQueryPrivate               │
└────────────────────┬──────────────────────────────────┘
                     │
                     ▼
┌───────────────────────────────────────────────────────┐
│ QSqlResult::exec()                                    │
│ • Store query string                                  │
│ • Call reset() to execute                            │
└────────────────────┬──────────────────────────────────┘
                     │
                     ▼
┌───────────────────────────────────────────────────────┐
│ MySQLResult::reset(QString)                          │
│ • Call mysql_real_query()                            │
│ • Store result set handle                            │
│ • Set state: active = true, at = BeforeFirstRow     │
└────────────────────┬──────────────────────────────────┘
                     │
                     ▼
        ┌────────────────────┐
        │ SUCCESS or ERROR   │
        └────────┬───────────┘
                 │
    ┌────────────┴────────────┐
    │                         │
    ▼                         ▼
[SUCCESS]                 [ERROR]
    │                         │
    ▼                         ▼
query.next()          return false
    │                 + set QSqlError
    ▼
┌───────────────────────────────────────────────────────┐
│ QSqlResult::next()                                    │
│ • Call fetchNext()                                    │
└────────────────────┬──────────────────────────────────┘
                     │
                     ▼
┌───────────────────────────────────────────────────────┐
│ MySQLResult::fetch(at() + 1)                         │
│ • Call mysql_fetch_row()                             │
│ • Store row data                                     │
│ • Increment cursor position                          │
└────────────────────┬──────────────────────────────────┘
                     │
                     ▼
        ┌────────────────────┐
        │  HAS MORE ROWS?    │
        └────────┬───────────┘
                 │
    ┌────────────┴────────────┐
    │                         │
    ▼                         ▼
 [YES]                     [NO]
return true              return false
    │
    ▼
query.value(0)
    │
    ▼
┌───────────────────────────────────────────────────────┐
│ QSqlResult::data(int index)                          │
│ • Delegate to MySQLResult::data()                    │
└────────────────────┬──────────────────────────────────┘
                     │
                     ▼
┌───────────────────────────────────────────────────────┐
│ MySQLResult::data(int index)                         │
│ • Get column value from stored row                   │
│ • Convert to QVariant                                │
│ • Return QVariant                                    │
└───────────────────────────────────────────────────────┘
```

---

## 4. Prepared Statement Flow

```
PHASE 1: PREPARE
─────────────────
query.prepare("INSERT INTO users VALUES (?, ?)")
    │
    ▼
QSqlResult::prepare(QString sql)
    │
    ├─→ Parse placeholders ('?' or ':name')
    │   • Store positions/names
    │   • Detect binding syntax (positional/named)
    │
    └─→ MySQLResult::prepare(sql)
        • Call mysql_stmt_init()
        • Call mysql_stmt_prepare()
        • Store prepared statement handle


PHASE 2: BIND
─────────────
query.bindValue(0, "John")
query.bindValue(1, 25)
    │
    ▼
QSqlResult::bindValue(int pos, QVariant value)
    │
    └─→ Store in QVector<QVariant> boundValues[pos] = value


PHASE 3: EXECUTE
────────────────
query.exec()
    │
    ▼
QSqlResult::exec()
    │
    ├─→ Validate: all placeholders bound?
    │
    └─→ MySQLResult::exec()
        │
        ├─→ Bind parameters:
        │   • Create MYSQL_BIND array
        │   • Convert QVariant → MySQL types
        │   • Call mysql_stmt_bind_param()
        │
        └─→ Execute:
            • Call mysql_stmt_execute()
            • Handle results
            • Update state


PHASE 4: REUSE (Optional)
─────────────────────────
query.bindValue(0, "Alice")
query.bindValue(1, 30)
query.exec()
    │
    └─→ Reuse prepared statement
        (skip PREPARE phase, only BIND + EXECUTE)
```

---

## 5. Transaction Flow

```
db.transaction()
    │
    ▼
┌───────────────────────────────────────┐
│ QSqlDatabase::transaction()          │
│ • Check driver supports transactions  │
└────────────┬──────────────────────────┘
             │
             ▼
┌───────────────────────────────────────┐
│ QSqlDriver::beginTransaction()       │
│ • MySQL: "START TRANSACTION"         │
│ • SQLite: "BEGIN TRANSACTION"        │
│ • PostgreSQL: "BEGIN"                │
└────────────┬──────────────────────────┘
             │
             ▼
    [Transaction Active]
             │
    ┌────────┴────────┐
    │  Execute SQL    │
    │  Operations     │
    │  (INSERT,       │
    │   UPDATE,       │
    │   DELETE, etc.) │
    └────────┬────────┘
             │
    ┌────────┴─────────┐
    │                  │
    ▼                  ▼
[SUCCESS]         [FAILURE]
    │                  │
    │                  │
db.commit()      db.rollback()
    │                  │
    ▼                  ▼
┌─────────────────┐ ┌──────────────────┐
│QSqlDriver::     │ │QSqlDriver::      │
│commit           │ │rollback          │
│Transaction()    │ │Transaction()     │
│                 │ │                  │
│• MySQL: "COMMIT"│ │• MySQL: "ROLLBACK"│
│• SQLite: "COMMIT"│ │• SQLite: "ROLLBACK"│
└─────────────────┘ └──────────────────┘
    │                  │
    ▼                  ▼
[Changes Saved]   [Changes Undone]
```

---

## 6. Model-View-Database Sync

```
┌──────────────┐                                    ┌────────────┐
│  QTableView  │                                    │  Database  │
│   (UI/View)  │                                    │            │
└──────┬───────┘                                    └─────▲──────┘
       │                                                  │
       │ User edits cell                                 │
       │                                                  │
       ▼                                                  │
┌─────────────────────────────────────────────────┐     │
│ QSqlTableModel::setData(index, value)          │     │
│                                                 │     │
│  if (strategy == OnFieldChange):                │     │
│    └─→ submit() immediately ─────────────────────────►│
│                                                 │     │
│  else:                                          │     │
│    └─→ cache[row].rec.setValue(col, value)     │     │
│        cache[row].op = Update                   │     │
│        emit dataChanged(index)                  │     │
└─────────┬───────────────────────────────────────┘     │
          │                                              │
          ▼                                              │
┌─────────────────────┐                                 │
│ View updates UI     │                                 │
│ (cell shows new val)│                                 │
└─────────────────────┘                                 │
                                                         │
          Later: model.submitAll()                      │
                  │                                      │
                  ▼                                      │
┌──────────────────────────────────────────────────┐    │
│ QSqlTableModel::submitAll()                      │    │
│                                                   │    │
│ for each cached change:                          │    │
│   • if op == Insert:                             │    │
│     └─→ insertRowIntoTable() ────────────────────────►│
│                                                   │    │
│   • if op == Update:                             │    │
│     └─→ updateRowInTable() ──────────────────────────►│
│                                                   │    │
│   • if op == Delete:                             │    │
│     └─→ deleteRowFromTable() ─────────────────────────│
│                                                   │
│ clear cache                                       │
│ select() to refresh ──────────────────────────────────►│
└───────────────────────────────────────────────────┘    │
                                                          │
                  ◄───────────────────────────────────────┘
                  Fresh data
                  │
                  ▼
┌──────────────────────────────────┐
│ Model emits dataChanged()        │
│ View refreshes                   │
└──────────────────────────────────┘
```

---

## 7. Multi-Threading Architecture

```
MAIN THREAD                    WORKER THREAD 1          WORKER THREAD 2
     │                                │                        │
     │ Create connections             │                        │
     │                                │                        │
     ▼                                │                        │
db = addDatabase("QSQLITE",          │                        │
    "MainConnection")                │                        │
     │                                │                        │
     │ Start threads                  │                        │
     ├───────────────────────────────►│                        │
     │                                │                        │
     └────────────────────────────────┼───────────────────────►│
                                      │                        │
                                      ▼                        ▼
                          db1 = addDatabase("QSQLITE",    db2 = addDatabase(
                                  "Worker1Connection")        "Worker2Connection")
                                      │                        │
                                      │                        │
                                      ▼                        ▼
                                  db1.open()              db2.open()
                                      │                        │
                                      ▼                        ▼
                                  QSqlQuery(db1)         QSqlQuery(db2)
                                      │                        │
                                      │                        │
                                      ▼                        ▼
                              [Independent Work]       [Independent Work]
                              • Insert                 • Insert
                              • Update                 • Update
                              • Select                 • Select
                                      │                        │
                                      │                        │
                                      ▼                        ▼
                                  db1.close()            db2.close()
                              removeDatabase(           removeDatabase(
                                  "Worker1")                "Worker2")
                                      │                        │
     ┌────────────────────────────────┘                        │
     │                                                          │
     ◄──────────────────────────────────────────────────────────┘
     │
     │ Threads finished
     ▼
[Continue main thread work]


⚠️ KEY RULES:
• Each thread = Own connection (different connection name)
• Never share QSqlDatabase across threads
• Never share QSqlQuery across threads
• Use QMutex if accessing shared data structures
```

---

## 8. Plugin Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   Qt Application                         │
│                                                          │
│  QSqlDatabase::addDatabase("QMYSQL")                    │
└────────────────────┬─────────────────────────────────────┘
                     │
                     │ 1. Lookup driver
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│            QSqlDriverPlugin System                        │
│                                                           │
│  • Scan plugin directories                               │
│  • Load shared libraries (.so/.dll/.dylib)              │
│  • Check metadata (driver names)                         │
└────────────────────┬─────────────────────────────────────┘
                     │
                     │ 2. Load plugin
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│         Plugin: libqsqlmysql.so (example)                │
│                                                           │
│  class QMYSQLDriverPlugin : public QSqlDriverPlugin {    │
│    QSqlDriver* create(const QString &key) override {     │
│      if (key == "QMYSQL")                                │
│        return new QMYSQLDriver();                        │
│      return nullptr;                                     │
│    }                                                      │
│  };                                                       │
│                                                           │
│  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSqlDriver")  │
└────────────────────┬─────────────────────────────────────┘
                     │
                     │ 3. Instantiate driver
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│              QMYSQLDriver instance                        │
│                                                           │
│  • Implements QSqlDriver interface                       │
│  • Links to libmysqlclient.so                           │
│  • Provides MySQL-specific functionality                 │
└──────────────────────────────────────────────────────────┘


Plugin Loading Paths (in order):
1. QT_PLUGIN_PATH environment variable
2. ./plugins/sqldrivers/
3. Qt installation directory/plugins/sqldrivers/
4. Application directory


Available Plugins (typical installation):
├── libqsqlite.so       (Built-in, usually)
├── libqsqlmysql.so
├── libqsqlpsql.so      (PostgreSQL)
├── libqsqloci.so       (Oracle)
├── libqsqlodbc.so      (ODBC)
└── libqsqldb2.so       (IBM DB2)
```

---

## 9. Memory Management (Implicit Sharing)

```
SCENARIO: Copy QSqlRecord
─────────────────────────

Step 1: Create original
──────────────────────
QSqlRecord record1;
record1.append(QSqlField("name", QMetaType::fromType<QString>()));
record1.setValue("name", "Alice");

Memory:
┌──────────────┐
│   record1    │
│              │
│  d_ptr ──────┼──►┌────────────────────┐
└──────────────┘   │ QSqlRecordPrivate  │
                   │  refCount = 1      │
                   │  fields = [...]    │
                   └────────────────────┘


Step 2: Copy record
───────────────────
QSqlRecord record2 = record1;  // Shallow copy!

Memory:
┌──────────────┐      ┌────────────────────┐
│   record1    │      │ QSqlRecordPrivate  │
│              │      │  refCount = 2      │◄────┐
│  d_ptr ──────┼──────►  fields = [...]    │     │
└──────────────┘      └────────────────────┘     │
                                                  │
┌──────────────┐                                  │
│   record2    │                                  │
│              │                                  │
│  d_ptr ──────┼──────────────────────────────────┘
└──────────────┘

⚠️ No actual data copied! Just pointer shared.


Step 3: Modify record2
──────────────────────
record2.setValue("name", "Bob");  // Triggers detach!

Memory:
┌──────────────┐      ┌────────────────────┐
│   record1    │      │ QSqlRecordPrivate  │
│              │      │  refCount = 1      │
│  d_ptr ──────┼──────►  fields = ["Alice"]│
└──────────────┘      └────────────────────┘

┌──────────────┐      ┌────────────────────┐
│   record2    │      │ QSqlRecordPrivate  │
│              │      │  refCount = 1      │
│  d_ptr ──────┼──────►  fields = ["Bob"]  │← New copy!
└──────────────┘      └────────────────────┘

✅ Now they're independent!


BENEFITS:
• Fast copies (O(1) instead of O(n))
• Memory efficient (share until modified)
• Copy-on-Write (detach on modify)
• Thread-safe refcounting (QAtomicInt)
```

---

## 10. Connection Pool Diagram

```
┌─────────────────────────────────────────────────────────┐
│              ConnectionPool (Singleton)                  │
│                                                          │
│  ┌────────────────────────────────────────────┐         │
│  │  Available Connections Queue               │         │
│  │  ┌───────┬───────┬───────┬───────┬───────┐│         │
│  │  │ Conn1 │ Conn2 │ Conn3 │ Conn4 │ Conn5 ││         │
│  │  └───────┴───────┴───────┴───────┴───────┘│         │
│  └────────────────────────────────────────────┘         │
│                                                          │
│  Config:                                                 │
│  • maxConnections = 5                                    │
│  • activeConnections = 0                                 │
│  • QMutex (thread-safe)                                 │
└──────────────┬───────────────────────────────────────────┘
               │
               │
    ┌──────────┼──────────────────────────┐
    │          │                          │
    ▼          ▼                          ▼
┌────────┐ ┌────────┐              ┌────────┐
│Worker1 │ │Worker2 │   ...        │Worker N│
└───┬────┘ └───┬────┘              └───┬────┘
    │          │                       │
    │ request  │ request               │ request
    ▼          ▼                       ▼
┌──────────────────────────────────────────────┐
│ ConnectionPool::getConnection()              │
│                                              │
│ if (available.isEmpty() &&                   │
│     active >= max) {                         │
│   wait();  ◄──┐  Block until available      │
│ }              │                              │
│                │                              │
│ connection = available.dequeue();            │
│ active++;                                    │
│ return connection;                           │
└──────────────┬───────────────────────────────┘
               │
               │ Use connection
               │
               ▼
┌──────────────────────────────────────────────┐
│ Worker performs queries                      │
└──────────────┬───────────────────────────────┘
               │
               │ Done
               ▼
┌──────────────────────────────────────────────┐
│ ConnectionPool::releaseConnection()          │
│                                              │
│ available.enqueue(connection);               │
│ active--;                                    │
│ wakeOne(); ──┘  Wake blocked thread          │
└──────────────────────────────────────────────┘


FLOW EXAMPLE:
────────────
Time 0: Pool has 5 available connections
Time 1: Worker1 gets Conn1 (4 available, 1 active)
Time 2: Worker2 gets Conn2 (3 available, 2 active)
Time 3: Worker3 gets Conn3 (2 available, 3 active)
Time 4: Worker4 gets Conn4 (1 available, 4 active)
Time 5: Worker5 gets Conn5 (0 available, 5 active)
Time 6: Worker6 requests → BLOCKS (max reached)
Time 7: Worker1 releases Conn1 → Worker6 wakes up
Time 8: Worker6 gets Conn1 (0 available, 5 active)
```

---

## 📌 Key Takeaways từ Diagrams

### Architecture:
- ✅ 3-tier clean separation
- ✅ Plugin system cho extensibility
- ✅ Abstract interfaces cho portability

### Design Patterns:
- ✅ Factory (driver creation)
- ✅ Bridge (Query → Result)
- ✅ Strategy (edit strategies)
- ✅ Singleton (connection manager)
- ✅ Pimpl (d-pointer)
- ✅ RAII (automatic cleanup)

### Performance:
- ✅ Implicit sharing (copy-on-write)
- ✅ Prepared statements (SQL reuse)
- ✅ Connection pooling (reuse connections)
- ✅ Batch operations (bulk inserts)

### Thread Safety:
- ✅ One connection per thread
- ✅ Atomic refcounting
- ✅ Mutex-protected pools

---

**Use these diagrams khi:**
- 📖 Học kiến trúc lần đầu
- 🐛 Debug complex issues
- 👥 Explain to team members
- 📝 Design new features
- 🎓 Teaching Qt SQL

---

*Happy visualizing! 📊*
