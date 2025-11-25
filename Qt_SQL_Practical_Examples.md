# Qt SQL - Ví Dụ Thực Tế (Practical Examples)

## 📁 Complete Working Examples

---

## Example 1: Basic SQLite CRUD Application

```cpp
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class UserDatabase {
public:
    UserDatabase() {
        // Setup database
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("users.db");
        
        if (!db.open()) {
            qFatal("Cannot open database: %s", 
                   qPrintable(db.lastError().text()));
        }
        
        createTable();
    }
    
    ~UserDatabase() {
        db.close();
    }
    
    void createTable() {
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "email TEXT UNIQUE NOT NULL, "
                   "age INTEGER)");
    }
    
    // CREATE
    bool addUser(const QString &name, const QString &email, int age) {
        QSqlQuery query;
        query.prepare("INSERT INTO users (name, email, age) "
                      "VALUES (:name, :email, :age)");
        query.bindValue(":name", name);
        query.bindValue(":email", email);
        query.bindValue(":age", age);
        
        if (!query.exec()) {
            qDebug() << "Insert failed:" << query.lastError().text();
            return false;
        }
        
        qDebug() << "User added with ID:" << query.lastInsertId().toInt();
        return true;
    }
    
    // READ
    void listAllUsers() {
        QSqlQuery query("SELECT * FROM users");
        
        qDebug() << "\n=== All Users ===";
        while (query.next()) {
            int id = query.value("id").toInt();
            QString name = query.value("name").toString();
            QString email = query.value("email").toString();
            int age = query.value("age").toInt();
            
            qDebug() << QString("ID: %1, Name: %2, Email: %3, Age: %4")
                        .arg(id).arg(name).arg(email).arg(age);
        }
    }
    
    // READ - Single user
    bool getUser(int id, QString &name, QString &email, int &age) {
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE id = ?");
        query.addBindValue(id);
        
        if (query.exec() && query.next()) {
            name = query.value("name").toString();
            email = query.value("email").toString();
            age = query.value("age").toInt();
            return true;
        }
        return false;
    }
    
    // UPDATE
    bool updateUser(int id, const QString &name, const QString &email, int age) {
        QSqlQuery query;
        query.prepare("UPDATE users SET name = ?, email = ?, age = ? "
                      "WHERE id = ?");
        query.addBindValue(name);
        query.addBindValue(email);
        query.addBindValue(age);
        query.addBindValue(id);
        
        if (!query.exec()) {
            qDebug() << "Update failed:" << query.lastError().text();
            return false;
        }
        
        qDebug() << "Rows updated:" << query.numRowsAffected();
        return query.numRowsAffected() > 0;
    }
    
    // DELETE
    bool deleteUser(int id) {
        QSqlQuery query;
        query.prepare("DELETE FROM users WHERE id = ?");
        query.addBindValue(id);
        
        if (!query.exec()) {
            qDebug() << "Delete failed:" << query.lastError().text();
            return false;
        }
        
        return query.numRowsAffected() > 0;
    }
    
    // SEARCH
    void searchUsers(const QString &keyword) {
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE name LIKE ? OR email LIKE ?");
        QString pattern = "%" + keyword + "%";
        query.addBindValue(pattern);
        query.addBindValue(pattern);
        
        query.exec();
        
        qDebug() << "\n=== Search Results for:" << keyword << "===";
        while (query.next()) {
            qDebug() << query.value("name").toString() 
                     << "-" << query.value("email").toString();
        }
    }

private:
    QSqlDatabase db;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    UserDatabase userDb;
    
    // CREATE
    userDb.addUser("Alice Johnson", "alice@example.com", 28);
    userDb.addUser("Bob Smith", "bob@example.com", 35);
    userDb.addUser("Charlie Brown", "charlie@example.com", 42);
    
    // READ
    userDb.listAllUsers();
    
    // UPDATE
    userDb.updateUser(1, "Alice Williams", "alice.w@example.com", 29);
    
    // SEARCH
    userDb.searchUsers("Alice");
    
    // DELETE
    userDb.deleteUser(2);
    
    userDb.listAllUsers();
    
    return 0;
}
```

**Compile:**
```bash
qmake -project
qmake
make
```

**Add to .pro file:**
```qmake
QT += sql
```

---

## Example 2: Transaction with Error Handling

```cpp
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class BankAccount {
public:
    static bool transfer(int fromAccount, int toAccount, double amount) {
        QSqlDatabase db = QSqlDatabase::database();
        
        // Check if database supports transactions
        if (!db.driver()->hasFeature(QSqlDriver::Transactions)) {
            qWarning("Database doesn't support transactions!");
            return false;
        }
        
        // Start transaction
        if (!db.transaction()) {
            qDebug() << "Failed to start transaction:" 
                     << db.lastError().text();
            return false;
        }
        
        QSqlQuery query;
        bool success = true;
        
        // Step 1: Withdraw from source account
        query.prepare("UPDATE accounts SET balance = balance - :amount "
                      "WHERE id = :id AND balance >= :amount");
        query.bindValue(":amount", amount);
        query.bindValue(":id", fromAccount);
        
        if (!query.exec() || query.numRowsAffected() == 0) {
            qDebug() << "Withdrawal failed:" << query.lastError().text();
            success = false;
        }
        
        // Step 2: Deposit to destination account (only if step 1 succeeded)
        if (success) {
            query.prepare("UPDATE accounts SET balance = balance + :amount "
                          "WHERE id = :id");
            query.bindValue(":amount", amount);
            query.bindValue(":id", toAccount);
            
            if (!query.exec()) {
                qDebug() << "Deposit failed:" << query.lastError().text();
                success = false;
            }
        }
        
        // Commit or rollback
        if (success) {
            if (db.commit()) {
                qDebug() << "Transfer successful!";
                return true;
            } else {
                qDebug() << "Commit failed:" << db.lastError().text();
            }
        }
        
        // Rollback on any failure
        db.rollback();
        qDebug() << "Transaction rolled back";
        return false;
    }
};

// Usage
int main() {
    // Setup
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bank.db");
    db.open();
    
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS accounts ("
               "id INTEGER PRIMARY KEY, "
               "name TEXT, "
               "balance REAL)");
    
    query.exec("INSERT INTO accounts VALUES (1, 'Alice', 1000.0)");
    query.exec("INSERT INTO accounts VALUES (2, 'Bob', 500.0)");
    
    // Transfer money
    BankAccount::transfer(1, 2, 100.0);  // Alice -> Bob: $100
    
    // Verify
    query.exec("SELECT * FROM accounts");
    while (query.next()) {
        qDebug() << query.value("name").toString() 
                 << ": $" << query.value("balance").toDouble();
    }
    
    return 0;
}
```

---

## Example 3: Batch Insert for Performance

```cpp
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QElapsedTimer>
#include <QDebug>

class BatchInsertDemo {
public:
    // Slow method: Individual inserts
    static void slowInsert(int count) {
        QElapsedTimer timer;
        timer.start();
        
        QSqlQuery query;
        for (int i = 0; i < count; ++i) {
            query.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
            query.addBindValue(QString("User%1").arg(i));
            query.addBindValue(20 + (i % 50));
            query.exec();
        }
        
        qDebug() << "Slow insert" << count << "rows:" 
                 << timer.elapsed() << "ms";
    }
    
    // Fast method: Transaction + prepared statement
    static void fastInsert(int count) {
        QElapsedTimer timer;
        timer.start();
        
        QSqlDatabase db = QSqlDatabase::database();
        db.transaction();
        
        QSqlQuery query;
        query.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
        
        for (int i = 0; i < count; ++i) {
            query.addBindValue(QString("User%1").arg(i));
            query.addBindValue(20 + (i % 50));
            query.exec();
        }
        
        db.commit();
        
        qDebug() << "Fast insert" << count << "rows:" 
                 << timer.elapsed() << "ms";
    }
    
    // Ultra-fast method: Batch execution
    static void batchInsert(int count) {
        QElapsedTimer timer;
        timer.start();
        
        QSqlQuery query;
        query.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
        
        QVariantList names;
        QVariantList ages;
        
        for (int i = 0; i < count; ++i) {
            names << QString("User%1").arg(i);
            ages << (20 + (i % 50));
        }
        
        query.addBindValue(names);
        query.addBindValue(ages);
        
        if (!query.execBatch()) {
            qDebug() << "Batch insert failed:" << query.lastError().text();
        }
        
        qDebug() << "Batch insert" << count << "rows:" 
                 << timer.elapsed() << "ms";
    }
};

int main() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");  // In-memory database
    db.open();
    
    QSqlQuery query;
    query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, age INTEGER)");
    
    // Compare performance
    BatchInsertDemo::slowInsert(1000);
    
    query.exec("DELETE FROM users");
    
    BatchInsertDemo::fastInsert(1000);
    
    query.exec("DELETE FROM users");
    
    BatchInsertDemo::batchInsert(1000);
    
    return 0;
}

// Typical output:
// Slow insert 1000 rows: 850 ms
// Fast insert 1000 rows: 45 ms
// Batch insert 1000 rows: 12 ms
```

---

## Example 4: QSqlTableModel with QTableView

```cpp
#include <QApplication>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

class DatabaseTableWidget : public QWidget {
    Q_OBJECT
    
public:
    DatabaseTableWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setupDatabase();
        setupUI();
        loadData();
    }

private:
    void setupDatabase() {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("employees.db");
        
        if (!db.open()) {
            QMessageBox::critical(this, "Error", 
                                  "Cannot open database: " + db.lastError().text());
            return;
        }
        
        // Create and populate table
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS employees ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "department TEXT, "
                   "salary REAL)");
        
        // Add sample data if table is empty
        query.exec("SELECT COUNT(*) FROM employees");
        if (query.next() && query.value(0).toInt() == 0) {
            query.exec("INSERT INTO employees (name, department, salary) VALUES "
                       "('Alice Johnson', 'Engineering', 85000), "
                       "('Bob Smith', 'Marketing', 65000), "
                       "('Charlie Brown', 'Engineering', 92000), "
                       "('Diana Prince', 'HR', 70000)");
        }
    }
    
    void setupUI() {
        // Create model
        model = new QSqlTableModel(this, db);
        model->setTable("employees");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        
        // Set headers
        model->setHeaderData(0, Qt::Horizontal, "ID");
        model->setHeaderData(1, Qt::Horizontal, "Name");
        model->setHeaderData(2, Qt::Horizontal, "Department");
        model->setHeaderData(3, Qt::Horizontal, "Salary");
        
        // Create view
        view = new QTableView;
        view->setModel(model);
        view->horizontalHeader()->setStretchLastSection(true);
        view->setAlternatingRowColors(true);
        view->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        // Create buttons
        QPushButton *addBtn = new QPushButton("Add Row");
        QPushButton *deleteBtn = new QPushButton("Delete Row");
        QPushButton *submitBtn = new QPushButton("Save Changes");
        QPushButton *revertBtn = new QPushButton("Cancel Changes");
        QPushButton *filterBtn = new QPushButton("Filter Engineering");
        QPushButton *clearFilterBtn = new QPushButton("Clear Filter");
        
        connect(addBtn, &QPushButton::clicked, this, &DatabaseTableWidget::addRow);
        connect(deleteBtn, &QPushButton::clicked, this, &DatabaseTableWidget::deleteRow);
        connect(submitBtn, &QPushButton::clicked, this, &DatabaseTableWidget::submitChanges);
        connect(revertBtn, &QPushButton::clicked, this, &DatabaseTableWidget::revertChanges);
        connect(filterBtn, &QPushButton::clicked, this, &DatabaseTableWidget::applyFilter);
        connect(clearFilterBtn, &QPushButton::clicked, this, &DatabaseTableWidget::clearFilter);
        
        // Layout
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(view);
        
        QHBoxLayout *btnLayout = new QHBoxLayout;
        btnLayout->addWidget(addBtn);
        btnLayout->addWidget(deleteBtn);
        btnLayout->addWidget(submitBtn);
        btnLayout->addWidget(revertBtn);
        btnLayout->addWidget(filterBtn);
        btnLayout->addWidget(clearFilterBtn);
        
        layout->addLayout(btnLayout);
    }
    
    void loadData() {
        if (!model->select()) {
            QMessageBox::critical(this, "Error", 
                                  "Cannot load data: " + model->lastError().text());
        }
    }

private slots:
    void addRow() {
        int row = model->rowCount();
        model->insertRow(row);
        
        // Set default values
        model->setData(model->index(row, 1), "New Employee");
        model->setData(model->index(row, 2), "Engineering");
        model->setData(model->index(row, 3), 50000);
        
        // Scroll to new row
        view->scrollToBottom();
        view->selectRow(row);
    }
    
    void deleteRow() {
        QModelIndexList selection = view->selectionModel()->selectedRows();
        
        if (selection.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please select a row to delete");
            return;
        }
        
        // Delete selected rows (from bottom to top to avoid index issues)
        QList<int> rows;
        for (const QModelIndex &index : selection) {
            rows.append(index.row());
        }
        std::sort(rows.begin(), rows.end(), std::greater<int>());
        
        for (int row : rows) {
            model->removeRow(row);
        }
    }
    
    void submitChanges() {
        if (model->submitAll()) {
            QMessageBox::information(this, "Success", "Changes saved successfully");
        } else {
            QMessageBox::critical(this, "Error", 
                                  "Failed to save: " + model->lastError().text());
            model->revertAll();
        }
    }
    
    void revertChanges() {
        model->revertAll();
        QMessageBox::information(this, "Reverted", "All changes cancelled");
    }
    
    void applyFilter() {
        model->setFilter("department = 'Engineering'");
        model->select();
    }
    
    void clearFilter() {
        model->setFilter("");
        model->select();
    }

private:
    QSqlDatabase db;
    QSqlTableModel *model;
    QTableView *view;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    DatabaseTableWidget window;
    window.resize(800, 400);
    window.setWindowTitle("Employee Database");
    window.show();
    
    return app.exec();
}

#include "main.moc"
```

---

## Example 5: Multi-threaded Database Access

```cpp
#include <QThread>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMutex>

class DatabaseWorker : public QThread {
    Q_OBJECT
    
public:
    DatabaseWorker(int workerId, QObject *parent = nullptr)
        : QThread(parent), m_workerId(workerId) {}
    
protected:
    void run() override {
        // IMPORTANT: Each thread needs its own connection
        QString connectionName = QString("Worker_%1").arg(m_workerId);
        
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("shared.db");
        
        if (!db.open()) {
            qDebug() << "Worker" << m_workerId << "failed to open DB";
            return;
        }
        
        // Do work
        QSqlQuery query(db);
        for (int i = 0; i < 10; ++i) {
            query.prepare("INSERT INTO logs (worker_id, message, timestamp) "
                          "VALUES (?, ?, datetime('now'))");
            query.addBindValue(m_workerId);
            query.addBindValue(QString("Message %1").arg(i));
            
            if (query.exec()) {
                qDebug() << "Worker" << m_workerId << "inserted message" << i;
            } else {
                qDebug() << "Worker" << m_workerId << "error:" 
                         << query.lastError().text();
            }
            
            // Simulate work
            QThread::msleep(100);
        }
        
        // Cleanup: Close and remove connection
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        
        qDebug() << "Worker" << m_workerId << "finished";
    }
    
private:
    int m_workerId;
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    // Setup main database
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "MainConnection");
        db.setDatabaseName("shared.db");
        db.open();
        
        QSqlQuery query(db);
        query.exec("CREATE TABLE IF NOT EXISTS logs ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "worker_id INTEGER, "
                   "message TEXT, "
                   "timestamp TEXT)");
        query.exec("DELETE FROM logs");  // Clear old data
        
        db.close();
        QSqlDatabase::removeDatabase("MainConnection");
    }
    
    // Start worker threads
    QList<DatabaseWorker*> workers;
    for (int i = 0; i < 3; ++i) {
        DatabaseWorker *worker = new DatabaseWorker(i);
        workers.append(worker);
        worker->start();
    }
    
    // Wait for all threads
    for (DatabaseWorker *worker : workers) {
        worker->wait();
        delete worker;
    }
    
    // Read results
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ReadConnection");
        db.setDatabaseName("shared.db");
        db.open();
        
        QSqlQuery query(db);
        query.exec("SELECT * FROM logs ORDER BY timestamp");
        
        qDebug() << "\n=== Final Results ===";
        while (query.next()) {
            qDebug() << QString("Worker %1: %2 at %3")
                        .arg(query.value("worker_id").toInt())
                        .arg(query.value("message").toString())
                        .arg(query.value("timestamp").toString());
        }
        
        db.close();
    }
    
    return 0;
}

#include "main.moc"
```

---

## Example 6: Custom QSqlQueryModel

```cpp
#include <QSqlQueryModel>
#include <QColor>
#include <QFont>

class CustomQueryModel : public QSqlQueryModel {
public:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        // Get original data
        QVariant value = QSqlQueryModel::data(index, role);
        
        if (role == Qt::DisplayRole) {
            // Format salary column with $ sign
            if (index.column() == 3) {  // Salary column
                return QString("$%1").arg(value.toDouble(), 0, 'f', 2);
            }
        }
        
        if (role == Qt::BackgroundRole) {
            // Highlight high salaries
            if (index.column() == 3 && value.toDouble() > 80000) {
                return QColor(Qt::green).lighter(170);
            }
        }
        
        if (role == Qt::ForegroundRole) {
            // Red text for low salaries
            if (index.column() == 3 && value.toDouble() < 60000) {
                return QColor(Qt::red);
            }
        }
        
        if (role == Qt::FontRole) {
            // Bold font for department column
            if (index.column() == 2) {
                QFont font;
                font.setBold(true);
                return font;
            }
        }
        
        if (role == Qt::TextAlignmentRole) {
            // Right-align salary
            if (index.column() == 3) {
                return int(Qt::AlignRight | Qt::AlignVCenter);
            }
        }
        
        return value;
    }
};

// Usage
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("employees.db");
    db.open();
    
    CustomQueryModel *model = new CustomQueryModel;
    model->setQuery("SELECT id, name, department, salary FROM employees");
    
    QTableView *view = new QTableView;
    view->setModel(model);
    view->show();
    
    return app.exec();
}
```

---

## Example 7: Connection Pool Manager

```cpp
#include <QSqlDatabase>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>

class ConnectionPool {
public:
    static ConnectionPool& instance() {
        static ConnectionPool instance;
        return instance;
    }
    
    QSqlDatabase getConnection() {
        QMutexLocker locker(&mutex);
        
        // Wait if no connections available
        while (availableConnections.isEmpty() && 
               activeConnections >= maxConnections) {
            condition.wait(&mutex);
        }
        
        QSqlDatabase db;
        
        if (!availableConnections.isEmpty()) {
            // Reuse existing connection
            QString connName = availableConnections.dequeue();
            db = QSqlDatabase::database(connName);
        } else {
            // Create new connection
            QString connName = QString("Connection_%1").arg(connectionCounter++);
            db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName("pooled.db");
            
            if (!db.open()) {
                qDebug() << "Failed to create connection:" 
                         << db.lastError().text();
                return QSqlDatabase();
            }
        }
        
        activeConnections++;
        return db;
    }
    
    void releaseConnection(QSqlDatabase &db) {
        QMutexLocker locker(&mutex);
        
        if (db.isValid()) {
            availableConnections.enqueue(db.connectionName());
            activeConnections--;
            condition.wakeOne();
        }
    }
    
    void setMaxConnections(int max) {
        maxConnections = max;
    }

private:
    ConnectionPool() : maxConnections(5), activeConnections(0), connectionCounter(0) {}
    
    QQueue<QString> availableConnections;
    QMutex mutex;
    QWaitCondition condition;
    int maxConnections;
    int activeConnections;
    int connectionCounter;
};

// RAII wrapper for automatic release
class ScopedConnection {
public:
    ScopedConnection() {
        db = ConnectionPool::instance().getConnection();
    }
    
    ~ScopedConnection() {
        ConnectionPool::instance().releaseConnection(db);
    }
    
    QSqlDatabase& database() { return db; }
    
private:
    QSqlDatabase db;
};

// Usage
void performDatabaseWork() {
    ScopedConnection conn;
    QSqlQuery query(conn.database());
    
    query.exec("SELECT * FROM users");
    while (query.next()) {
        // Process...
    }
    
    // Connection automatically released when function ends
}
```

---

## Example 8: JSON Export/Import

```cpp
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

class JsonExporter {
public:
    static bool exportToJson(const QString &tableName, const QString &fileName) {
        QSqlQuery query(QString("SELECT * FROM %1").arg(tableName));
        
        QJsonArray array;
        
        while (query.next()) {
            QJsonObject obj;
            QSqlRecord record = query.record();
            
            for (int i = 0; i < record.count(); ++i) {
                QString fieldName = record.fieldName(i);
                QVariant value = record.value(i);
                
                // Convert QVariant to JSON value
                if (value.isNull()) {
                    obj[fieldName] = QJsonValue::Null;
                } else {
                    obj[fieldName] = QJsonValue::fromVariant(value);
                }
            }
            
            array.append(obj);
        }
        
        QJsonDocument doc(array);
        
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Cannot open file for writing";
            return false;
        }
        
        file.write(doc.toJson(QJsonDocument::Indented));
        return true;
    }
    
    static bool importFromJson(const QString &tableName, const QString &fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Cannot open file for reading";
            return false;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        
        QSqlDatabase db = QSqlDatabase::database();
        db.transaction();
        
        for (const QJsonValue &value : array) {
            QJsonObject obj = value.toObject();
            
            // Build INSERT statement
            QStringList fields;
            QStringList placeholders;
            QVariantList values;
            
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                fields << it.key();
                placeholders << "?";
                values << it.value().toVariant();
            }
            
            QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                         .arg(tableName)
                         .arg(fields.join(", "))
                         .arg(placeholders.join(", "));
            
            QSqlQuery query;
            query.prepare(sql);
            for (const QVariant &v : values) {
                query.addBindValue(v);
            }
            
            if (!query.exec()) {
                qDebug() << "Import failed:" << query.lastError().text();
                db.rollback();
                return false;
            }
        }
        
        db.commit();
        return true;
    }
};

// Usage
int main() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    db.open();
    
    // Export
    JsonExporter::exportToJson("employees", "employees.json");
    
    // Import
    JsonExporter::importFromJson("employees_backup", "employees.json");
    
    return 0;
}
```

---

## 🎯 Complete .pro File Example

```qmake
QT += core sql widgets
CONFIG += c++17

SOURCES += main.cpp
HEADERS += 

# SQLite is built-in, but for other databases:
# MySQL
# LIBS += -L/usr/lib/mysql -lmysqlclient

# PostgreSQL  
# LIBS += -L/usr/lib -lpq

# Output
TARGET = DatabaseApp
TEMPLATE = app
```

---

## 🔧 CMakeLists.txt Example

```cmake
cmake_minimum_required(VERSION 3.16)
project(DatabaseApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)

qt_add_executable(DatabaseApp
    main.cpp
)

target_link_libraries(DatabaseApp PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Widgets
)
```

---

## 📚 Summary

Những ví dụ trên cover:
- ✅ Basic CRUD operations
- ✅ Transaction management
- ✅ Batch operations for performance
- ✅ Model-View integration
- ✅ Multi-threading
- ✅ Custom model với formatting
- ✅ Connection pooling
- ✅ JSON export/import

**Best Practices được demonstrate:**
- Prepared statements (SQL injection prevention)
- Transaction wrapping
- RAII pattern (automatic cleanup)
- Error handling
- Thread safety
- Performance optimization

**Compile và run bất kỳ example nào bằng:**
```bash
qmake -project
echo "QT += sql widgets" >> *.pro
qmake
make
./DatabaseApp
```

Happy coding! 🚀
