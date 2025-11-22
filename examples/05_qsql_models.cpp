/**
 * VÍ DỤ 5: QSql Models - Database Models
 * 
 * ĐẶC ĐIỂM:
 * - Kết nối trực tiếp với database
 * - QSqlQueryModel: Read-only
 * - QSqlTableModel: Editable, 1 table
 * - QSqlRelationalTableModel: Foreign keys
 */

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QMessageBox>
#include <QDebug>

// ============================================
// Database Setup
// ============================================
bool setupDatabase() {
    // Tạo database SQLite trong memory
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // In-memory database
    
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError().text();
        return false;
    }
    
    QSqlQuery query;
    
    // ----------------------------------------
    // Tạo bảng: departments (phòng ban)
    // ----------------------------------------
    query.exec("CREATE TABLE departments ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "location TEXT)");
    
    query.exec("INSERT INTO departments (name, location) VALUES "
               "('IT', 'Floor 3'), "
               "('HR', 'Floor 2'), "
               "('Sales', 'Floor 1'), "
               "('Marketing', 'Floor 1')");
    
    // ----------------------------------------
    // Tạo bảng: employees (nhân viên)
    // ----------------------------------------
    query.exec("CREATE TABLE employees ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "age INTEGER, "
               "salary REAL, "
               "department_id INTEGER, "
               "FOREIGN KEY (department_id) REFERENCES departments(id))");
    
    query.exec("INSERT INTO employees (name, age, salary, department_id) VALUES "
               "('Nguyễn Văn A', 28, 15000000, 1), "
               "('Trần Thị B', 25, 12000000, 1), "
               "('Lê Văn C', 32, 18000000, 1), "
               "('Phạm Thị D', 30, 13000000, 2), "
               "('Hoàng Văn E', 27, 14000000, 2), "
               "('Vũ Thị F', 29, 16000000, 3), "
               "('Đỗ Văn G', 31, 15500000, 3), "
               "('Bùi Thị H', 26, 11000000, 4)");
    
    // ----------------------------------------
    // Tạo bảng: projects (dự án)
    // ----------------------------------------
    query.exec("CREATE TABLE projects ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "budget REAL, "
               "department_id INTEGER, "
               "status TEXT, "
               "FOREIGN KEY (department_id) REFERENCES departments(id))");
    
    query.exec("INSERT INTO projects (name, budget, department_id, status) VALUES "
               "('Website Redesign', 50000000, 1, 'Active'), "
               "('Mobile App', 80000000, 1, 'Planning'), "
               "('Recruitment Drive', 20000000, 2, 'Active'), "
               "('Q4 Campaign', 100000000, 4, 'Active')");
    
    return true;
}

// ============================================
// 1. QSqlQueryModel - Read-Only
// ============================================
QWidget* createQueryModelTab() {
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Model: QSqlQueryModel (READ-ONLY)
    QSqlQueryModel *model = new QSqlQueryModel;
    
    // Query tùy ý (JOIN, WHERE, ORDER BY, etc.)
    model->setQuery(
        "SELECT e.name AS 'Tên', "
        "       e.age AS 'Tuổi', "
        "       e.salary AS 'Lương', "
        "       d.name AS 'Phòng ban' "
        "FROM employees e "
        "LEFT JOIN departments d ON e.department_id = d.id "
        "ORDER BY e.salary DESC"
    );
    
    // Kiểm tra lỗi
    if (model->lastError().isValid()) {
        qDebug() << "Query error:" << model->lastError().text();
    }
    
    // Tùy chỉnh headers (optional, query đã có alias)
    // model->setHeaderData(0, Qt::Horizontal, "Tên Nhân Viên");
    
    // View
    QTableView *view = new QTableView;
    view->setModel(model);
    view->resizeColumnsToContents();
    
    layout->addWidget(new QLabel("<b>QSqlQueryModel:</b> Read-only, SQL query tùy ý"));
    layout->addWidget(view);
    layout->addWidget(new QLabel("✓ Có thể JOIN nhiều tables\n"
                                  "✓ Linh hoạt với SQL\n"
                                  "✗ Không edit được"));
    
    return widget;
}

// ============================================
// 2. QSqlTableModel - Editable Single Table
// ============================================
QWidget* createTableModelTab() {
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Model: QSqlTableModel (EDITABLE)
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("employees");
    
    // Edit strategy
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    // Options: OnFieldChange, OnRowChange, OnManualSubmit
    
    // Set headers (Vietnamese)
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Tên");
    model->setHeaderData(2, Qt::Horizontal, "Tuổi");
    model->setHeaderData(3, Qt::Horizontal, "Lương");
    model->setHeaderData(4, Qt::Horizontal, "Phòng Ban ID");
    
    // Filter
    model->setFilter("salary > 12000000"); // Chỉ lương > 12M
    
    // Sort
    model->setSort(3, Qt::DescendingOrder); // Sort by salary desc
    
    // Load data
    model->select();
    
    // View
    QTableView *view = new QTableView;
    view->setModel(model);
    view->resizeColumnsToContents();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Buttons
    QPushButton *addBtn = new QPushButton("Thêm nhân viên");
    QPushButton *deleteBtn = new QPushButton("Xóa nhân viên được chọn");
    QPushButton *submitBtn = new QPushButton("💾 Lưu thay đổi");
    QPushButton *revertBtn = new QPushButton("↶ Hoàn tác");
    
    // Thêm row
    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        int row = model->rowCount();
        model->insertRow(row);
        
        // Set default values
        model->setData(model->index(row, 1), "Nhân viên mới");
        model->setData(model->index(row, 2), 25);
        model->setData(model->index(row, 3), 10000000);
        model->setData(model->index(row, 4), 1);
    });
    
    // Xóa row
    QObject::connect(deleteBtn, &QPushButton::clicked, [=, widget]() {
        QModelIndex current = view->currentIndex();
        if (current.isValid()) {
            model->removeRow(current.row());
        }
    });
    
    // Submit changes
    QObject::connect(submitBtn, &QPushButton::clicked, [=, widget]() {
        if (model->submitAll()) {
            QMessageBox::information(widget, "Success", "Đã lưu thành công!");
        } else {
            QMessageBox::critical(widget, "Error", 
                                 "Lỗi: " + model->lastError().text());
        }
    });
    
    // Revert changes
    QObject::connect(revertBtn, &QPushButton::clicked, [=]() {
        model->revertAll();
    });
    
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(submitBtn);
    btnLayout->addWidget(revertBtn);
    
    layout->addWidget(new QLabel("<b>QSqlTableModel:</b> Editable, 1 table"));
    layout->addWidget(view);
    layout->addLayout(btnLayout);
    layout->addWidget(new QLabel("✓ Có thể edit, insert, delete\n"
                                  "✓ Tự động CRUD\n"
                                  "✓ Filter & Sort\n"
                                  "✗ Chỉ 1 table"));
    
    return widget;
}

// ============================================
// 3. QSqlRelationalTableModel - Foreign Keys
// ============================================
QWidget* createRelationalModelTab() {
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Model: QSqlRelationalTableModel (HANDLES FOREIGN KEYS)
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel;
    model->setTable("employees");
    
    // Thiết lập relation cho foreign key
    // Cột 4 (department_id) → departments table
    model->setRelation(4, QSqlRelation("departments", "id", "name"));
    //                     ↑ Table name   ↑ FK column  ↑ Display column
    
    // Headers
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Tên");
    model->setHeaderData(2, Qt::Horizontal, "Tuổi");
    model->setHeaderData(3, Qt::Horizontal, "Lương");
    model->setHeaderData(4, Qt::Horizontal, "Phòng Ban"); // Sẽ hiện TÊN phòng ban
    
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    
    // View
    QTableView *view = new QTableView;
    view->setModel(model);
    
    // QUAN TRỌNG: Phải dùng QSqlRelationalDelegate
    // để edit foreign key column (hiện dropdown)
    view->setItemDelegate(new QSqlRelationalDelegate(view));
    
    view->resizeColumnsToContents();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Buttons
    QPushButton *addBtn = new QPushButton("Thêm nhân viên");
    QPushButton *deleteBtn = new QPushButton("Xóa");
    QPushButton *submitBtn = new QPushButton("💾 Lưu");
    QPushButton *revertBtn = new QPushButton("↶ Hoàn tác");
    
    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        int row = model->rowCount();
        model->insertRow(row);
        model->setData(model->index(row, 1), "Nhân viên mới");
        model->setData(model->index(row, 2), 25);
        model->setData(model->index(row, 3), 10000000);
        // Cột 4 sẽ có dropdown chọn phòng ban
    });
    
    QObject::connect(deleteBtn, &QPushButton::clicked, [=]() {
        QModelIndex current = view->currentIndex();
        if (current.isValid())
            model->removeRow(current.row());
    });
    
    QObject::connect(submitBtn, &QPushButton::clicked, [=, widget]() {
        if (model->submitAll()) {
            QMessageBox::information(widget, "Success", "Đã lưu!");
            model->select(); // Refresh
        } else {
            QMessageBox::critical(widget, "Error", model->lastError().text());
        }
    });
    
    QObject::connect(revertBtn, &QPushButton::clicked, [=]() {
        model->revertAll();
    });
    
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(submitBtn);
    btnLayout->addWidget(revertBtn);
    
    layout->addWidget(new QLabel("<b>QSqlRelationalTableModel:</b> Foreign keys"));
    layout->addWidget(view);
    layout->addLayout(btnLayout);
    layout->addWidget(new QLabel("✓ Xử lý foreign keys tự động\n"
                                  "✓ Hiện tên thay vì ID\n"
                                  "✓ Dropdown khi edit\n"
                                  "✓ Tự động JOIN"));
    
    return widget;
}

// ============================================
// 4. Advanced: Custom Query + Proxy Filter
// ============================================
QWidget* createAdvancedTab() {
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Query with aggregation
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(
        "SELECT d.name AS 'Phòng Ban', "
        "       COUNT(e.id) AS 'Số NV', "
        "       AVG(e.salary) AS 'Lương TB', "
        "       MIN(e.salary) AS 'Lương Min', "
        "       MAX(e.salary) AS 'Lương Max' "
        "FROM departments d "
        "LEFT JOIN employees e ON d.id = e.department_id "
        "GROUP BY d.id, d.name "
        "ORDER BY COUNT(e.id) DESC"
    );
    
    QTableView *view = new QTableView;
    view->setModel(model);
    view->resizeColumnsToContents();
    
    layout->addWidget(new QLabel("<b>Advanced Query:</b> Aggregation & GROUP BY"));
    layout->addWidget(view);
    
    // Projects table
    QSqlRelationalTableModel *projectModel = new QSqlRelationalTableModel;
    projectModel->setTable("projects");
    projectModel->setRelation(2, QSqlRelation("departments", "id", "name"));
    
    projectModel->setHeaderData(0, Qt::Horizontal, "ID");
    projectModel->setHeaderData(1, Qt::Horizontal, "Tên Dự Án");
    projectModel->setHeaderData(2, Qt::Horizontal, "Phòng Ban");
    projectModel->setHeaderData(3, Qt::Horizontal, "Ngân Sách");
    projectModel->setHeaderData(4, Qt::Horizontal, "Trạng Thái");
    
    projectModel->select();
    
    QTableView *projectView = new QTableView;
    projectView->setModel(projectModel);
    projectView->setItemDelegate(new QSqlRelationalDelegate(projectView));
    projectView->resizeColumnsToContents();
    
    layout->addWidget(new QLabel("<b>Projects Table:</b>"));
    layout->addWidget(projectView);
    
    return widget;
}

// ============================================
// Main
// ============================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Setup database
    if (!setupDatabase()) {
        QMessageBox::critical(nullptr, "Error", "Cannot setup database");
        return 1;
    }
    
    // Main window
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // Tab widget
    QTabWidget *tabs = new QTabWidget;
    tabs->addTab(createQueryModelTab(), "QSqlQueryModel");
    tabs->addTab(createTableModelTab(), "QSqlTableModel");
    tabs->addTab(createRelationalModelTab(), "QSqlRelationalTableModel");
    tabs->addTab(createAdvancedTab(), "Advanced");
    
    layout->addWidget(tabs);
    
    window.resize(900, 600);
    window.setWindowTitle("QSql Models Demo");
    window.show();
    
    return app.exec();
}

// ============================================
// CÁCH ĐỌC CODE QSQL MODELS:
// ============================================
//
// 1. QSqlQueryModel:
//    - setQuery(SQL)
//    - Read-only
//    - Dùng cho SELECT queries phức tạp
//    - Có thể JOIN, GROUP BY, etc.
//
// 2. QSqlTableModel:
//    - setTable(tableName)
//    - Editable
//    - setFilter() để filter
//    - setSort() để sort
//    - insertRow(), removeRow(), setData()
//    - submitAll() / revertAll()
//
// 3. QSqlRelationalTableModel:
//    - Kế thừa từ QSqlTableModel
//    - setRelation(column, QSqlRelation(...))
//    - Tự động JOIN foreign key tables
//    - PHẢI dùng QSqlRelationalDelegate cho view
//
// EDIT STRATEGIES:
// - OnFieldChange: Lưu ngay khi edit
// - OnRowChange: Lưu khi chuyển sang row khác
// - OnManualSubmit: Phải gọi submitAll() thủ công
//
// KEY METHODS:
// - select(): Load data từ DB
// - submitAll(): Lưu changes vào DB
// - revertAll(): Hủy changes
// - insertRow(row): Thêm row mới
// - removeRow(row): Xóa row
// - setFilter(whereClause): WHERE condition
// - setSort(column, order): ORDER BY
//
// DATABASE SETUP:
// 1. QSqlDatabase::addDatabase("QSQLITE")
// 2. db.setDatabaseName(path)
// 3. db.open()
// 4. QSqlQuery để execute SQL
