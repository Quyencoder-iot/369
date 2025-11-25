#include <QCoreApplication>
#include <QtSql>
#include <QDebug>

// Ví dụ này minh họa cách sử dụng các lớp QtSql
// Qua đó hiểu được cách các lớp QSqlDatabase, QSqlQuery tương tác với nhau.

void demonstrateQtSql() {
    // 1. QSqlDatabase: Factory tạo kết nối
    // Trong source code: QSqlDatabase::addDatabase load plugin driver (ví dụ QSQLITE)
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // Database trong RAM

    // 2. Mở kết nối
    // Trong source code: Gọi QSqlDriver::open() -> sqlite3_open()
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError().text();
        return;
    }

    // 3. QSqlQuery: Thực thi lệnh
    // Trong source code: Tạo một QSqlResult từ Driver để xử lý
    QSqlQuery query;
    
    // Tạo bảng
    QString createTable = "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, role TEXT)";
    if (!query.exec(createTable)) {
        qDebug() << "Create table error:" << query.lastError();
    }

    // 4. Prepared Statement (Hiệu năng & Bảo mật)
    // Trong source code: Driver sẽ parse chuỗi này và bind giá trị
    query.prepare("INSERT INTO users (name, role) VALUES (:name, :role)");
    
    query.bindValue(":name", "Alice");
    query.bindValue(":role", "Admin");
    query.exec();

    query.bindValue(":name", "Bob");
    query.bindValue(":role", "User");
    query.exec();

    // 5. Fetch dữ liệu
    // Trong source code: QSqlQuery::next() gọi QSqlResult::fetchNext() 
    // -> driver di chuyển con trỏ cursor
    query.exec("SELECT * FROM users");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString role = query.value(2).toString();
        qDebug() << "User:" << id << name << role;
    }

    // 6. Đóng kết nối
    db.close();
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    demonstrateQtSql();
    return 0; // Kết thúc, không cần event loop cho ví dụ console đơn giản này
}
