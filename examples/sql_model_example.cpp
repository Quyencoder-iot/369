// Ví dụ SQL Models - Kết nối với database
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QApplication>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>

// Ví dụ 1: QSqlQueryModel (Read-only)
void example1_QueryModel() {
    // Kết nối database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        qDebug() << "Cannot open database";
        return;
    }
    
    // Tạo bảng và dữ liệu mẫu
    QSqlQuery query;
    query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    query.exec("INSERT INTO users VALUES (1, 'Alice', 25)");
    query.exec("INSERT INTO users VALUES (2, 'Bob', 30)");
    query.exec("INSERT INTO users VALUES (3, 'Charlie', 20)");
    
    // Tạo model từ query
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT name, age FROM users");
    
    // Set header
    model->setHeaderData(0, Qt::Horizontal, "Tên");
    model->setHeaderData(1, Qt::Horizontal, "Tuổi");
    
    // Hiển thị
    QTableView *view = new QTableView();
    view->setModel(model);
    view->show();
}

// Ví dụ 2: QSqlTableModel (Read-write)
void example2_TableModel() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        qDebug() << "Cannot open database";
        return;
    }
    
    QSqlQuery query;
    query.exec("CREATE TABLE products (id INTEGER PRIMARY KEY, name TEXT, price REAL)");
    query.exec("INSERT INTO products VALUES (1, 'Book', 10.5)");
    query.exec("INSERT INTO products VALUES (2, 'Pen', 2.0)");
    query.exec("INSERT INTO products VALUES (3, 'Notebook', 5.5)");
    
    // Tạo model
    QSqlTableModel *model = new QSqlTableModel();
    model->setTable("products");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Lọc
    model->setFilter("price > 3.0");
    
    // Sắp xếp
    model->setSort(2, Qt::AscendingOrder);  // Sắp xếp theo cột price
    
    // Load dữ liệu
    model->select();
    
    // View có thể edit
    QTableView *view = new QTableView();
    view->setModel(model);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);
    view->show();
    
    // Lưu thay đổi
    // model->submitAll();
}

// Ví dụ 3: SQL Model + Proxy Model (Lọc và sắp xếp)
int example3_SqlWithProxy(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        qDebug() << "Cannot open database";
        return -1;
    }
    
    QSqlQuery query;
    query.exec("CREATE TABLE employees (id INTEGER PRIMARY KEY, name TEXT, department TEXT)");
    query.exec("INSERT INTO employees VALUES (1, 'Alice', 'IT')");
    query.exec("INSERT INTO employees VALUES (2, 'Bob', 'HR')");
    query.exec("INSERT INTO employees VALUES (3, 'Charlie', 'IT')");
    query.exec("INSERT INTO employees VALUES (4, 'David', 'Sales')");
    
    // SQL Model
    QSqlTableModel *sqlModel = new QSqlTableModel();
    sqlModel->setTable("employees");
    sqlModel->select();
    
    // Proxy Model để lọc
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(sqlModel);
    proxyModel->setFilterKeyColumn(2);  // Lọc theo cột department
    
    // View
    QTableView *view = new QTableView();
    view->setModel(proxyModel);
    
    // Filter input
    QLineEdit *filterEdit = new QLineEdit();
    filterEdit->setPlaceholderText("Lọc theo department...");
    QObject::connect(filterEdit, &QLineEdit::textChanged, 
                     [proxyModel](const QString &text) {
        proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
    });
    
    // Layout
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(filterEdit);
    layout->addWidget(view);
    window.show();
    
    return app.exec();
}

/*
Luồng dữ liệu:
Database (SQLite)
    ↓
QSqlTableModel (source model)
    ↓ setSourceModel()
QSortFilterProxyModel (proxy model)
    ↓ setModel()
QTableView (view)
    ↓
User sees filtered data
*/

// Uncomment để chạy ví dụ:
// int main(int argc, char *argv[]) {
//     return example3_SqlWithProxy(argc, argv);
// }
