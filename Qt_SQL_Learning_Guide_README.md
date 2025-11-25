# 📚 Hướng Dẫn Học Qt SQL Database - Toàn Diện

> Tài liệu đầy đủ để hiểu nhanh code nguồn Qt SQL Module

---

## 🎯 Mục Đích

Bộ tài liệu này giúp bạn:
- ✅ Hiểu **kiến trúc** của Qt SQL Module
- ✅ Đọc và phân tích **source code** Qt SQL
- ✅ Sử dụng Qt SQL một cách **hiệu quả** trong projects
- ✅ Học các **design patterns** từ Qt source code
- ✅ Debug và **troubleshoot** database issues

---

## 📖 Các Tài Liệu Trong Bộ

### 1. **Qt_SQL_Architecture_Guide.md** 📐
**Dành cho:** Người muốn hiểu tổng quan kiến trúc

**Nội dung:**
- Tổng quan kiến trúc 3 tầng (Application, Abstraction, Drivers)
- Giải thích chi tiết từng class cốt lõi:
  - QSqlDatabase - Connection management
  - QSqlDriver - Abstract driver interface
  - QSqlQuery - High-level query API
  - QSqlResult - Low-level result handling
  - QSqlRecord & QSqlField - Data structures
  - QSqlTableModel - Model-View integration
- Luồng hoạt động của queries
- Design patterns được sử dụng
- Class diagram tổng quát

**Đọc file này trước tiên!** 👈

---

### 2. **Qt_SQL_Quick_Reference.md** ⚡
**Dành cho:** Developer cần tra cứu nhanh API

**Nội dung:**
- Quick reference cho tất cả Qt SQL classes
- Code snippets cho các tác vụ thường gặp:
  - Kết nối database (SQLite, MySQL, PostgreSQL)
  - CRUD operations
  - Prepared statements
  - Transactions
  - Batch operations
  - Model-View usage
- Common patterns (Singleton, Repository)
- Debug tips
- Available drivers

**Bookmark file này để tra cứu nhanh!** 🔖

---

### 3. **How_To_Read_Qt_SQL_Source.md** 🔍
**Dành cho:** Người muốn đọc và hiểu source code

**Nội dung:**
- Phương pháp đọc source code hiệu quả
- Roadmap đọc code theo levels:
  - Level 1: Basic classes (QSqlField, QSqlRecord)
  - Level 2: Core classes (QSqlDriver, QSqlQuery)
  - Level 3: Model-View (QSqlTableModel)
- Deep-dive vào implementation:
  - Prepared statement internals
  - Parameter binding mechanism
  - Copy-on-Write pattern
  - Pimpl pattern
- Code reading checklist
- Debugging techniques
- Exercises để practice

**Đọc file này để master source code!** 🎓

---

### 4. **Qt_SQL_Practical_Examples.md** 💻
**Dành cho:** Developer cần code examples hoàn chỉnh

**Nội dung:**
- 8 ví dụ hoàn chỉnh, có thể compile và chạy:
  1. Basic CRUD Application
  2. Transaction with Error Handling
  3. Batch Insert for Performance
  4. QSqlTableModel with QTableView (GUI)
  5. Multi-threaded Database Access
  6. Custom QSqlQueryModel
  7. Connection Pool Manager
  8. JSON Export/Import
- Best practices demonstration
- Performance optimization
- Project configuration (.pro, CMakeLists.txt)

**Copy-paste và chạy thử ngay!** 🚀

---

## 🗺️ Lộ Trình Học

### Cho Beginners (Mới bắt đầu):

```
1. Đọc Qt_SQL_Architecture_Guide.md
   ↓ (Hiểu big picture)
   
2. Đọc Qt_SQL_Quick_Reference.md
   ↓ (Học cách dùng API)
   
3. Chạy examples trong Qt_SQL_Practical_Examples.md
   ↓ (Thực hành)
   
4. Build một simple app (CRUD todo list)
   ↓ (Apply kiến thức)
```

**Thời gian:** 1-2 ngày

---

### Cho Intermediate (Đã biết Qt SQL cơ bản):

```
1. Review Qt_SQL_Architecture_Guide.md
   ↓ (Refresh kiến thức)
   
2. Đọc How_To_Read_Qt_SQL_Source.md
   ↓ (Học cách đọc source)
   
3. Clone Qt source code và đọc files trong:
   - qtbase/src/sql/kernel/
   - qtbase/src/sql/models/
   ↓ (Đọc implementation thực tế)
   
4. Study advanced examples:
   - Multi-threading
   - Connection pooling
   - Custom models
```

**Thời gian:** 3-5 ngày

---

### Cho Advanced (Muốn implement custom driver):

```
1. Deep-dive vào How_To_Read_Qt_SQL_Source.md
   ↓
   
2. Đọc driver implementations:
   - src/plugins/sqldrivers/sqlite/
   - src/plugins/sqldrivers/mysql/
   - src/plugins/sqldrivers/psql/
   ↓
   
3. Understand QSqlDriver và QSqlResult abstract interfaces
   ↓
   
4. Implement custom driver cho database mới
```

**Thời gian:** 1-2 tuần

---

## 📂 Qt Source Code Location

### Đã clone trong workspace này:

```
/workspace/qtbase/src/sql/
├── kernel/                    # Core classes
│   ├── qsqldatabase.h/.cpp   # Connection management
│   ├── qsqlquery.h/.cpp      # Query execution
│   ├── qsqldriver.h/.cpp     # Driver interface
│   ├── qsqlresult.h/.cpp     # Result handling
│   ├── qsqlrecord.h/.cpp     # Row representation
│   ├── qsqlfield.h/.cpp      # Column representation
│   └── qsqlerror.h/.cpp      # Error handling
│
├── models/                    # Model-View classes
│   ├── qsqlquerymodel.h/.cpp
│   ├── qsqltablemodel.h/.cpp
│   └── qsqlrelationaltablemodel.h/.cpp
│
└── doc/                       # Documentation & examples
    └── snippets/              # Code examples
```

### Để xem full source với drivers:

```bash
cd /workspace/qtbase
git sparse-checkout set src/sql src/plugins/sqldrivers
```

---

## 🔧 Quick Start - Chạy Example Đầu Tiên

### 1. Tạo file `main.cpp`:

```cpp
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    // Kết nối SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    
    if (!db.open()) {
        qDebug() << "Cannot open database";
        return -1;
    }
    
    // Tạo table
    QSqlQuery query;
    query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    
    // Insert
    query.exec("INSERT INTO users VALUES (1, 'Alice')");
    query.exec("INSERT INTO users VALUES (2, 'Bob')");
    
    // Select
    query.exec("SELECT * FROM users");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        qDebug() << "ID:" << id << "Name:" << name;
    }
    
    return 0;
}
```

### 2. Tạo project file `test.pro`:

```qmake
QT += core sql
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += main.cpp
```

### 3. Compile và chạy:

```bash
qmake
make
./test
```

**Output:**
```
ID: 1 Name: "Alice"
ID: 2 Name: "Bob"
```

✅ **Thành công!** Bạn đã chạy Qt SQL app đầu tiên!

---

## 🎯 Key Concepts Cần Nhớ

### 1. Kiến Trúc 3 Tầng
```
Application (QSqlQuery, QSqlTableModel)
    ↕
Abstraction (QSqlDatabase, QSqlDriver)
    ↕
Drivers (QSQLITE, QMYSQL, QPSQL, ...)
```

### 2. Core Classes Relationship
```
QSqlDatabase ─┬─→ QSqlDriver ─→ QSqlResult
              │
              └─→ QSqlQuery ─→ QSqlResult
              
QSqlRecord contains QSqlField(s)
```

### 3. Design Patterns
- **Factory:** QSqlDatabase::addDatabase()
- **Bridge:** QSqlQuery delegates to QSqlResult
- **Strategy:** QSqlTableModel::EditStrategy
- **Pimpl:** All classes use d-pointer
- **RAII:** Automatic resource cleanup

### 4. Thread Safety Rule
⚠️ **MỘT connection = MỘT thread**

```cpp
// ❌ WRONG - Sharing connection
QSqlDatabase db = QSqlDatabase::database();
thread1.setDb(db);  // DON'T!
thread2.setDb(db);  // DON'T!

// ✅ CORRECT - Separate connections
// Thread 1:
QSqlDatabase db1 = QSqlDatabase::database("conn1");

// Thread 2:
QSqlDatabase db2 = QSqlDatabase::cloneDatabase(
    QSqlDatabase::database(), "conn2"
);
```

### 5. Prepared Statement Best Practice
```cpp
// ❌ BAD - SQL Injection risk
query.exec("SELECT * FROM users WHERE name = '" + userInput + "'");

// ✅ GOOD - Safe
query.prepare("SELECT * FROM users WHERE name = ?");
query.addBindValue(userInput);
query.exec();
```

---

## 🐛 Common Issues & Solutions

### Issue 1: "Driver not loaded"
```
QSqlDatabase: QMYSQL driver not loaded
```

**Solution:**
```bash
# Check available drivers
qDebug() << QSqlDatabase::drivers();

# Install MySQL plugin (Ubuntu)
sudo apt-get install libqt5sql5-mysql

# Or compile Qt with MySQL support
./configure -sql-mysql
```

---

### Issue 2: "Database is locked" (SQLite)
```
QSqlError: database is locked
```

**Solution:**
```cpp
// Use Write-Ahead Logging (WAL) mode
QSqlQuery query;
query.exec("PRAGMA journal_mode=WAL");

// Or use transactions properly
db.transaction();
// ... queries ...
db.commit();
```

---

### Issue 3: Slow batch inserts

**Solution:**
```cpp
// ❌ SLOW - No transaction
for (int i = 0; i < 10000; ++i) {
    query.exec("INSERT INTO ...");
}

// ✅ FAST - Use transaction
db.transaction();
for (int i = 0; i < 10000; ++i) {
    query.exec("INSERT INTO ...");
}
db.commit();

// ✅ FASTEST - Use batch execution
query.prepare("INSERT INTO ...");
// Add all values
query.execBatch();
```

---

### Issue 4: Memory leak với QSqlQuery

**Solution:**
```cpp
// ❌ WRONG - Query keeps all results in memory
QSqlQuery query("SELECT * FROM huge_table");  // 1M rows!
while (query.next()) { ... }

// ✅ CORRECT - Forward-only mode
QSqlQuery query;
query.setForwardOnly(true);  // Less memory
query.exec("SELECT * FROM huge_table");
while (query.next()) { ... }
```

---

## 📚 Additional Resources

### Official Documentation:
- **Qt SQL Programming Guide:** https://doc.qt.io/qt-6/sql-programming.html
- **Qt SQL Classes:** https://doc.qt.io/qt-6/qtsql-module.html

### Source Code:
- **GitHub:** https://github.com/qt/qtbase/tree/dev/src/sql
- **Local:** `/workspace/qtbase/src/sql/`

### Community:
- **Qt Forum:** https://forum.qt.io/category/16/sql
- **Stack Overflow:** Tag `qt` + `qsql`

---

## 🎓 Learning Exercises

### Exercise 1: Simple App
Build một CRUD todo list app với SQLite.

**Requirements:**
- Add todo
- Mark as done
- Delete todo
- List all todos
- Filter by status

---

### Exercise 2: Model-View App
Build một employee management app với QSqlTableModel.

**Requirements:**
- Display employees trong QTableView
- Edit inline
- Filter by department
- Sort by column
- Manual submit mode

---

### Exercise 3: Multi-Database App
Connect to multiple databases simultaneously.

**Requirements:**
- SQLite cho local cache
- MySQL cho production data
- Sync between them
- Handle connection failures

---

### Exercise 4: Custom Driver (Advanced)
Implement một simple driver cho NoSQL database (e.g., Redis).

**Requirements:**
- Inherit QSqlDriver
- Implement required virtual methods
- Support basic SELECT/INSERT
- Plugin architecture

---

## 🌟 Best Practices Checklist

Khi viết Qt SQL code, hãy check:

- [ ] Dùng prepared statements (not string concatenation)
- [ ] Check return values (open(), exec(), etc.)
- [ ] Handle QSqlError properly
- [ ] Use transactions cho multiple writes
- [ ] One connection per thread
- [ ] Close connections when done (or use RAII)
- [ ] Set forward-only mode cho large results
- [ ] Use batch operations khi có thể
- [ ] Escape identifiers với QSqlDriver::escapeIdentifier()
- [ ] Test with different database backends

---

## 🤝 Contributing

Nếu bạn tìm thấy lỗi hoặc muốn thêm examples:

1. Fork repository
2. Thêm/sửa content
3. Submit pull request

---

## 📝 License

Tài liệu này dựa trên Qt Source Code (LGPL/GPL).

Qt Source Code © The Qt Company Ltd.
Documentation © Contributors

---

## 🎉 Kết Luận

Qt SQL Module là một ví dụ xuất sắc về:
- ✅ Clean architecture
- ✅ Well-designed APIs
- ✅ Extensible plugin system
- ✅ Cross-platform abstraction

Sau khi học xong bộ tài liệu này, bạn sẽ:
- ✅ Hiểu sâu Qt SQL architecture
- ✅ Đọc và phân tích Qt source code
- ✅ Viết efficient database code
- ✅ Debug database issues
- ✅ Implement custom drivers (nếu cần)

---

## 📞 Support

Nếu có câu hỏi:
- 📧 Qt Forum: https://forum.qt.io/
- 💬 Stack Overflow: Tag `qt` + `qsql`
- 📖 Documentation: https://doc.qt.io/

---

**Happy Learning! 🚀**

*Chúc bạn thành công với Qt SQL Database!*

---

## 📋 Quick Links

| Document | Purpose | Read Time |
|----------|---------|-----------|
| [Architecture Guide](Qt_SQL_Architecture_Guide.md) | Understand design | 1-2 hours |
| [Quick Reference](Qt_SQL_Quick_Reference.md) | API lookup | As needed |
| [Source Code Guide](How_To_Read_Qt_SQL_Source.md) | Read source | 2-3 hours |
| [Practical Examples](Qt_SQL_Practical_Examples.md) | Code samples | 1 hour |

**Total learning time:** 4-6 hours (comprehensive understanding)

---

*Last updated: November 2025*
*Qt Version: Qt 6.x*
*Source: qtbase/src/sql/*
