// Ví dụ về QSql Models - Làm việc với database
#include <QApplication>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QMessageBox>
#include <QFormLayout>
#include <QSpinBox>

// SQL includes
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>

#include <QDebug>

// Helper class để setup và quản lý database
class DatabaseManager {
public:
    static bool createConnection() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("company.db");
        
        if (!db.open()) {
            qDebug() << "Không thể mở database:" << db.lastError().text();
            return false;
        }
        
        // Tạo tables nếu chưa tồn tại
        QSqlQuery query;
        
        // Table phòng ban
        query.exec("CREATE TABLE IF NOT EXISTS departments ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT NOT NULL,"
                   "location TEXT)");
        
        // Table nhân viên
        query.exec("CREATE TABLE IF NOT EXISTS employees ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT NOT NULL,"
                   "department_id INTEGER,"
                   "salary INTEGER,"
                   "hire_date DATE,"
                   "FOREIGN KEY (department_id) REFERENCES departments(id))");
        
        // Table dự án
        query.exec("CREATE TABLE IF NOT EXISTS projects ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT NOT NULL,"
                   "budget INTEGER,"
                   "status TEXT)");
        
        // Thêm dữ liệu mẫu nếu tables rỗng
        query.exec("SELECT COUNT(*) FROM departments");
        if (query.next() && query.value(0).toInt() == 0) {
            insertSampleData();
        }
        
        return true;
    }
    
private:
    static void insertSampleData() {
        QSqlQuery query;
        
        // Insert departments
        query.prepare("INSERT INTO departments (name, location) VALUES (?, ?)");
        QStringList depts = {"IT", "HR", "Sales", "Finance"};
        QStringList locations = {"Tầng 3", "Tầng 2", "Tầng 1", "Tầng 4"};
        
        for (int i = 0; i < depts.size(); ++i) {
            query.addBindValue(depts[i]);
            query.addBindValue(locations[i]);
            query.exec();
        }
        
        // Insert employees
        query.prepare("INSERT INTO employees (name, department_id, salary, hire_date) VALUES (?, ?, ?, ?)");
        QStringList names = {"Nguyễn Văn A", "Trần Thị B", "Lê Văn C", "Phạm Thị D", 
                            "Hoàng Văn E", "Vũ Thị F", "Đặng Văn G", "Bùi Thị H"};
        QList<int> deptIds = {1, 2, 3, 1, 4, 3, 1, 2};
        QList<int> salaries = {20000000, 15000000, 18000000, 25000000, 
                              16000000, 19000000, 30000000, 14000000};
        
        for (int i = 0; i < names.size(); ++i) {
            query.addBindValue(names[i]);
            query.addBindValue(deptIds[i]);
            query.addBindValue(salaries[i]);
            query.addBindValue(QDate::currentDate().addDays(-365 * (i + 1)));
            query.exec();
        }
        
        // Insert projects
        query.prepare("INSERT INTO projects (name, budget, status) VALUES (?, ?, ?)");
        QStringList projects = {"Website Redesign", "Mobile App", "ERP System", "Cloud Migration"};
        QList<int> budgets = {50000000, 100000000, 200000000, 150000000};
        QStringList statuses = {"In Progress", "Planning", "Completed", "In Progress"};
        
        for (int i = 0; i < projects.size(); ++i) {
            query.addBindValue(projects[i]);
            query.addBindValue(budgets[i]);
            query.addBindValue(statuses[i]);
            query.exec();
        }
    }
};

// Tab 1: QSqlTableModel Demo
class SqlTableModelTab : public QWidget {
    Q_OBJECT
    
public:
    SqlTableModelTab(QWidget *parent = nullptr) : QWidget(parent) {
        setupModel();
        setupUI();
    }
    
private:
    void setupModel() {
        // QSqlTableModel cho direct table access
        m_model = new QSqlTableModel(this);
        m_model->setTable("employees");
        m_model->setEditStrategy(QSqlTableModel::OnFieldChange); // Auto-save
        m_model->select(); // Load data
        
        // Đặt headers tiếng Việt
        m_model->setHeaderData(0, Qt::Horizontal, "ID");
        m_model->setHeaderData(1, Qt::Horizontal, "Tên nhân viên");
        m_model->setHeaderData(2, Qt::Horizontal, "ID Phòng ban");
        m_model->setHeaderData(3, Qt::Horizontal, "Lương");
        m_model->setHeaderData(4, Qt::Horizontal, "Ngày vào làm");
    }
    
    void setupUI() {
        auto *layout = new QVBoxLayout(this);
        
        // Info
        layout->addWidget(new QLabel("📋 QSqlTableModel - CRUD operations trực tiếp trên table"));
        
        // View
        m_view = new QTableView;
        m_view->setModel(m_model);
        m_view->setAlternatingRowColors(true);
        m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
        layout->addWidget(m_view);
        
        // Filter controls
        auto *filterLayout = new QHBoxLayout;
        filterLayout->addWidget(new QLabel("Lọc theo tên:"));
        m_filterEdit = new QLineEdit;
        filterLayout->addWidget(m_filterEdit);
        
        auto *filterBtn = new QPushButton("Áp dụng lọc");
        auto *clearFilterBtn = new QPushButton("Xóa lọc");
        filterLayout->addWidget(filterBtn);
        filterLayout->addWidget(clearFilterBtn);
        layout->addLayout(filterLayout);
        
        // CRUD buttons
        auto *crudLayout = new QHBoxLayout;
        auto *addBtn = new QPushButton("Thêm nhân viên");
        auto *deleteBtn = new QPushButton("Xóa nhân viên");
        auto *revertBtn = new QPushButton("Hoàn tác");
        auto *submitBtn = new QPushButton("Lưu thay đổi");
        
        crudLayout->addWidget(addBtn);
        crudLayout->addWidget(deleteBtn);
        crudLayout->addWidget(revertBtn);
        crudLayout->addWidget(submitBtn);
        layout->addLayout(crudLayout);
        
        // Edit strategy combo
        auto *strategyLayout = new QHBoxLayout;
        strategyLayout->addWidget(new QLabel("Chiến lược lưu:"));
        m_strategyCombo = new QComboBox;
        m_strategyCombo->addItems({"OnFieldChange (Tự động)", "OnRowChange", "OnManualSubmit"});
        strategyLayout->addWidget(m_strategyCombo);
        layout->addLayout(strategyLayout);
        
        // Connections
        connect(filterBtn, &QPushButton::clicked, this, [this]() {
            QString filter = QString("name LIKE '%%%1%%'").arg(m_filterEdit->text());
            m_model->setFilter(filter);
        });
        
        connect(clearFilterBtn, &QPushButton::clicked, this, [this]() {
            m_filterEdit->clear();
            m_model->setFilter("");
        });
        
        connect(addBtn, &QPushButton::clicked, this, [this]() {
            int row = m_model->rowCount();
            m_model->insertRow(row);
            m_model->setData(m_model->index(row, 1), "Nhân viên mới");
            m_model->setData(m_model->index(row, 2), 1);
            m_model->setData(m_model->index(row, 3), 10000000);
            m_model->setData(m_model->index(row, 4), QDate::currentDate());
        });
        
        connect(deleteBtn, &QPushButton::clicked, this, [this]() {
            QModelIndexList selection = m_view->selectionModel()->selectedRows();
            if (!selection.isEmpty()) {
                m_model->removeRow(selection.first().row());
            }
        });
        
        connect(revertBtn, &QPushButton::clicked, m_model, &QSqlTableModel::revertAll);
        connect(submitBtn, &QPushButton::clicked, this, [this]() {
            if (!m_model->submitAll()) {
                QMessageBox::warning(this, "Lỗi", m_model->lastError().text());
            }
        });
        
        connect(m_strategyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int index) {
            m_model->setEditStrategy(static_cast<QSqlTableModel::EditStrategy>(index));
        });
    }
    
private:
    QSqlTableModel *m_model;
    QTableView *m_view;
    QLineEdit *m_filterEdit;
    QComboBox *m_strategyCombo;
};

// Tab 2: QSqlRelationalTableModel Demo
class SqlRelationalModelTab : public QWidget {
    Q_OBJECT
    
public:
    SqlRelationalModelTab(QWidget *parent = nullptr) : QWidget(parent) {
        setupModel();
        setupUI();
    }
    
private:
    void setupModel() {
        // QSqlRelationalTableModel cho foreign key relationships
        m_model = new QSqlRelationalTableModel(this);
        m_model->setTable("employees");
        m_model->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);
        
        // Thiết lập relation cho department_id
        m_model->setRelation(2, QSqlRelation("departments", "id", "name"));
        
        m_model->select();
        
        // Headers
        m_model->setHeaderData(0, Qt::Horizontal, "ID");
        m_model->setHeaderData(1, Qt::Horizontal, "Tên nhân viên");
        m_model->setHeaderData(2, Qt::Horizontal, "Phòng ban");
        m_model->setHeaderData(3, Qt::Horizontal, "Lương");
        m_model->setHeaderData(4, Qt::Horizontal, "Ngày vào làm");
    }
    
    void setupUI() {
        auto *layout = new QVBoxLayout(this);
        
        // Info
        layout->addWidget(new QLabel("🔗 QSqlRelationalTableModel - Tự động join với foreign keys"));
        
        // View
        m_view = new QTableView;
        m_view->setModel(m_model);
        m_view->setAlternatingRowColors(true);
        m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        // Sử dụng delegate đặc biệt cho relations
        m_view->setItemDelegate(new QSqlRelationalDelegate(m_view));
        
        layout->addWidget(m_view);
        
        // Add employee form
        auto *formGroup = new QGroupBox("Thêm nhân viên mới");
        auto *formLayout = new QFormLayout(formGroup);
        
        m_nameEdit = new QLineEdit;
        m_deptCombo = new QComboBox;
        m_salarySpinBox = new QSpinBox;
        m_salarySpinBox->setRange(0, 999999999);
        m_salarySpinBox->setSuffix(" VNĐ");
        
        // Populate department combo
        QSqlQuery query("SELECT id, name FROM departments");
        while (query.next()) {
            m_deptCombo->addItem(query.value(1).toString(), query.value(0));
        }
        
        formLayout->addRow("Tên:", m_nameEdit);
        formLayout->addRow("Phòng ban:", m_deptCombo);
        formLayout->addRow("Lương:", m_salarySpinBox);
        
        auto *addBtn = new QPushButton("Thêm nhân viên");
        formLayout->addRow(addBtn);
        
        layout->addWidget(formGroup);
        
        // Statistics
        m_statsLabel = new QLabel;
        updateStats();
        layout->addWidget(m_statsLabel);
        
        // Connections
        connect(addBtn, &QPushButton::clicked, this, [this]() {
            if (m_nameEdit->text().isEmpty()) {
                QMessageBox::warning(this, "Lỗi", "Vui lòng nhập tên nhân viên!");
                return;
            }
            
            int row = m_model->rowCount();
            m_model->insertRow(row);
            m_model->setData(m_model->index(row, 1), m_nameEdit->text());
            m_model->setData(m_model->index(row, 2), m_deptCombo->currentData());
            m_model->setData(m_model->index(row, 3), m_salarySpinBox->value());
            m_model->setData(m_model->index(row, 4), QDate::currentDate());
            
            if (m_model->submitAll()) {
                m_nameEdit->clear();
                m_salarySpinBox->setValue(0);
                updateStats();
            } else {
                QMessageBox::warning(this, "Lỗi", m_model->lastError().text());
            }
        });
    }
    
    void updateStats() {
        QSqlQuery query("SELECT d.name, COUNT(e.id), AVG(e.salary) "
                       "FROM departments d "
                       "LEFT JOIN employees e ON d.id = e.department_id "
                       "GROUP BY d.id");
        
        QString stats = "📊 Thống kê theo phòng ban:\n";
        while (query.next()) {
            stats += QString("%1: %2 người, Lương TB: %3 VNĐ\n")
                    .arg(query.value(0).toString())
                    .arg(query.value(1).toInt())
                    .arg(QLocale().toString(query.value(2).toDouble(), 'f', 0));
        }
        
        m_statsLabel->setText(stats);
    }
    
private:
    QSqlRelationalTableModel *m_model;
    QTableView *m_view;
    QLineEdit *m_nameEdit;
    QComboBox *m_deptCombo;
    QSpinBox *m_salarySpinBox;
    QLabel *m_statsLabel;
};

// Tab 3: QSqlQueryModel Demo
class SqlQueryModelTab : public QWidget {
    Q_OBJECT
    
public:
    SqlQueryModelTab(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        runDefaultQuery();
    }
    
private:
    void setupUI() {
        auto *layout = new QVBoxLayout(this);
        
        // Info
        layout->addWidget(new QLabel("🔍 QSqlQueryModel - Chạy custom SQL queries (read-only)"));
        
        // Query input
        auto *queryLayout = new QHBoxLayout;
        m_queryEdit = new QLineEdit;
        m_queryEdit->setPlaceholderText("Nhập SQL query...");
        m_queryEdit->setText("SELECT e.name, d.name as department, e.salary "
                            "FROM employees e JOIN departments d ON e.department_id = d.id");
        queryLayout->addWidget(m_queryEdit);
        
        auto *runBtn = new QPushButton("Chạy Query");
        queryLayout->addWidget(runBtn);
        layout->addLayout(queryLayout);
        
        // Predefined queries
        auto *predefGroup = new QGroupBox("Queries mẫu");
        auto *predefLayout = new QVBoxLayout(predefGroup);
        
        auto *query1Btn = new QPushButton("Top 5 lương cao nhất");
        auto *query2Btn = new QPushButton("Nhân viên theo phòng ban");
        auto *query3Btn = new QPushButton("Thống kê lương");
        auto *query4Btn = new QPushButton("Danh sách dự án");
        
        predefLayout->addWidget(query1Btn);
        predefLayout->addWidget(query2Btn);
        predefLayout->addWidget(query3Btn);
        predefLayout->addWidget(query4Btn);
        layout->addWidget(predefGroup);
        
        // View
        m_view = new QTableView;
        m_model = new QSqlQueryModel(this);
        m_view->setModel(m_model);
        m_view->setAlternatingRowColors(true);
        layout->addWidget(m_view);
        
        // Result info
        m_resultLabel = new QLabel;
        layout->addWidget(m_resultLabel);
        
        // Connections
        connect(runBtn, &QPushButton::clicked, this, &SqlQueryModelTab::runQuery);
        
        connect(query1Btn, &QPushButton::clicked, this, [this]() {
            m_queryEdit->setText("SELECT name, salary FROM employees ORDER BY salary DESC LIMIT 5");
            runQuery();
        });
        
        connect(query2Btn, &QPushButton::clicked, this, [this]() {
            m_queryEdit->setText("SELECT d.name as department, GROUP_CONCAT(e.name) as employees "
                                "FROM departments d "
                                "LEFT JOIN employees e ON d.id = e.department_id "
                                "GROUP BY d.id");
            runQuery();
        });
        
        connect(query3Btn, &QPushButton::clicked, this, [this]() {
            m_queryEdit->setText("SELECT 'Tổng lương' as metric, SUM(salary) as value FROM employees "
                                "UNION ALL "
                                "SELECT 'Lương trung bình', AVG(salary) FROM employees "
                                "UNION ALL "
                                "SELECT 'Lương cao nhất', MAX(salary) FROM employees "
                                "UNION ALL "
                                "SELECT 'Lương thấp nhất', MIN(salary) FROM employees");
            runQuery();
        });
        
        connect(query4Btn, &QPushButton::clicked, this, [this]() {
            m_queryEdit->setText("SELECT name, budget, status FROM projects ORDER BY budget DESC");
            runQuery();
        });
    }
    
    void runQuery() {
        QString queryStr = m_queryEdit->text();
        m_model->setQuery(queryStr);
        
        if (m_model->lastError().isValid()) {
            m_resultLabel->setText("❌ Lỗi: " + m_model->lastError().text());
            m_resultLabel->setStyleSheet("color: red;");
        } else {
            m_resultLabel->setText(QString("✅ Kết quả: %1 hàng").arg(m_model->rowCount()));
            m_resultLabel->setStyleSheet("color: green;");
            
            // Auto-resize columns
            m_view->resizeColumnsToContents();
        }
    }
    
    void runDefaultQuery() {
        runQuery();
    }
    
private:
    QSqlQueryModel *m_model;
    QTableView *m_view;
    QLineEdit *m_queryEdit;
    QLabel *m_resultLabel;
};

// Main window với tabs
class SqlModelsDemo : public QWidget {
    Q_OBJECT
    
public:
    SqlModelsDemo() {
        // Setup database connection
        if (!DatabaseManager::createConnection()) {
            QMessageBox::critical(this, "Lỗi", "Không thể kết nối database!");
            return;
        }
        
        auto *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("🗄️ Demo QSql Models"));
        
        // Create tab widget
        auto *tabWidget = new QTabWidget;
        tabWidget->addTab(new SqlTableModelTab, "QSqlTableModel");
        tabWidget->addTab(new SqlRelationalModelTab, "QSqlRelationalTableModel");
        tabWidget->addTab(new SqlQueryModelTab, "QSqlQueryModel");
        
        layout->addWidget(tabWidget);
        
        // Database info
        auto *dbInfoLabel = new QLabel("📁 Database: company.db (SQLite)");
        dbInfoLabel->setStyleSheet("font-style: italic;");
        layout->addWidget(dbInfoLabel);
    }
};

#include "sql_model_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    SqlModelsDemo demo;
    demo.setWindowTitle("SQL Models Example");
    demo.resize(900, 700);
    demo.show();
    
    return app.exec();
}

/*
 * TỔNG KẾT VỀ QSql Models:
 * 
 * 1. QSqlTableModel:
 *    - CRUD operations trực tiếp trên 1 table
 *    - 3 edit strategies: OnFieldChange, OnRowChange, OnManualSubmit
 *    - setFilter() cho WHERE clause
 *    - insertRow(), removeRow(), setData()
 * 
 * 2. QSqlRelationalTableModel:
 *    - Extends QSqlTableModel với foreign key support
 *    - setRelation() để define relationships
 *    - Tự động hiển thị lookup values thay vì IDs
 *    - Cần QSqlRelationalDelegate cho editing
 * 
 * 3. QSqlQueryModel:
 *    - Read-only model cho custom queries
 *    - Flexible - chạy bất kỳ SELECT query nào
 *    - Không support editing
 *    - Tốt cho reports và complex joins
 * 
 * 4. Common patterns:
 *    - Check lastError() sau mọi operation
 *    - select() để refresh data
 *    - submitAll() khi dùng OnManualSubmit
 *    - setHeaderData() cho friendly column names
 * 
 * 5. Performance tips:
 *    - Dùng setFilter() thay vì load all + proxy
 *    - Limit rows với SQL thay vì Qt
 *    - Index database columns được filter/sort
 * 
 * 6. Database connection:
 *    - QSqlDatabase::addDatabase() một lần
 *    - Multiple connections với connection names
 *    - Close connections khi app exit
 */