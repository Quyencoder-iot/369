/**
 * VÍ DỤ 6: COMPLETE EXAMPLE
 * 
 * Kết hợp tất cả:
 * - QSqlRelationalTableModel (Database)
 * - QSortFilterProxyModel (Filter & Sort)
 * - QTableView (Display)
 * - Custom filtering & searching
 */

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>

// ============================================
// Custom Proxy Model cho Employee filtering
// ============================================
class EmployeeFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
    
    QString m_searchText;
    int m_minSalary = 0;
    QString m_departmentFilter;
    
public:
    EmployeeFilterProxy(QObject *parent = nullptr) 
        : QSortFilterProxyModel(parent) {
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }
    
protected:
    bool filterAcceptsRow(int sourceRow, 
                         const QModelIndex &sourceParent) const override {
        
        // Lấy source model
        QAbstractItemModel *model = sourceModel();
        
        // Get data from each column
        QString name = model->data(model->index(sourceRow, 1, sourceParent)).toString();
        int salary = model->data(model->index(sourceRow, 3, sourceParent)).toInt();
        QString department = model->data(model->index(sourceRow, 4, sourceParent)).toString();
        
        // Filter by search text (name)
        if (!m_searchText.isEmpty()) {
            if (!name.contains(m_searchText, Qt::CaseInsensitive))
                return false;
        }
        
        // Filter by minimum salary
        if (salary < m_minSalary)
            return false;
        
        // Filter by department
        if (!m_departmentFilter.isEmpty() && m_departmentFilter != "All") {
            if (department != m_departmentFilter)
                return false;
        }
        
        return true;
    }
    
public slots:
    void setSearchText(const QString &text) {
        m_searchText = text;
        invalidateFilter();
    }
    
    void setMinSalary(int salary) {
        m_minSalary = salary;
        invalidateFilter();
    }
    
    void setDepartmentFilter(const QString &dept) {
        m_departmentFilter = dept;
        invalidateFilter();
    }
};

// ============================================
// Setup Database
// ============================================
bool setupDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError().text();
        return false;
    }
    
    QSqlQuery query;
    
    // Departments table
    query.exec("CREATE TABLE departments ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "location TEXT, "
               "budget REAL)");
    
    query.exec("INSERT INTO departments (name, location, budget) VALUES "
               "('Engineering', 'Floor 3', 500000000), "
               "('Human Resources', 'Floor 2', 100000000), "
               "('Sales', 'Floor 1', 300000000), "
               "('Marketing', 'Floor 1', 200000000), "
               "('Finance', 'Floor 4', 150000000)");
    
    // Employees table
    query.exec("CREATE TABLE employees ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "age INTEGER, "
               "salary REAL, "
               "department_id INTEGER, "
               "hire_date TEXT, "
               "email TEXT, "
               "FOREIGN KEY (department_id) REFERENCES departments(id))");
    
    query.exec("INSERT INTO employees (name, age, salary, department_id, hire_date, email) VALUES "
               "('Nguyễn Văn Anh', 28, 18000000, 1, '2020-01-15', 'anh.nguyen@company.com'), "
               "('Trần Thị Bình', 25, 15000000, 1, '2021-03-20', 'binh.tran@company.com'), "
               "('Lê Văn Cường', 32, 22000000, 1, '2018-06-10', 'cuong.le@company.com'), "
               "('Phạm Thị Dung', 30, 14000000, 2, '2019-09-05', 'dung.pham@company.com'), "
               "('Hoàng Văn Em', 27, 13000000, 2, '2021-11-12', 'em.hoang@company.com'), "
               "('Vũ Thị Phượng', 29, 19000000, 3, '2020-02-28', 'phuong.vu@company.com'), "
               "('Đỗ Văn Giang', 31, 17000000, 3, '2019-07-15', 'giang.do@company.com'), "
               "('Bùi Thị Hà', 26, 16000000, 4, '2021-05-20', 'ha.bui@company.com'), "
               "('Lý Văn Ích', 33, 20000000, 1, '2017-12-01', 'ich.ly@company.com'), "
               "('Trịnh Thị Kim', 28, 15500000, 5, '2020-08-10', 'kim.trinh@company.com')");
    
    return true;
}

// ============================================
// Main Application
// ============================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Setup database
    if (!setupDatabase()) {
        QMessageBox::critical(nullptr, "Error", "Cannot setup database!");
        return 1;
    }
    
    // ========================================
    // LAYER 1: SQL Model (Data from Database)
    // ========================================
    QSqlRelationalTableModel *sqlModel = new QSqlRelationalTableModel;
    sqlModel->setTable("employees");
    
    // Setup foreign key relation
    // Column 4 (department_id) → departments.name
    sqlModel->setRelation(4, QSqlRelation("departments", "id", "name"));
    
    // Headers
    sqlModel->setHeaderData(0, Qt::Horizontal, "ID");
    sqlModel->setHeaderData(1, Qt::Horizontal, "Tên Nhân Viên");
    sqlModel->setHeaderData(2, Qt::Horizontal, "Tuổi");
    sqlModel->setHeaderData(3, Qt::Horizontal, "Lương (VNĐ)");
    sqlModel->setHeaderData(4, Qt::Horizontal, "Phòng Ban");
    sqlModel->setHeaderData(5, Qt::Horizontal, "Ngày Vào");
    sqlModel->setHeaderData(6, Qt::Horizontal, "Email");
    
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();
    
    // ========================================
    // LAYER 2: Proxy Model (Filter & Sort)
    // ========================================
    EmployeeFilterProxy *proxyModel = new EmployeeFilterProxy;
    proxyModel->setSourceModel(sqlModel);
    proxyModel->setDynamicSortFilter(true);
    
    // ========================================
    // LAYER 3: View (Display)
    // ========================================
    QTableView *view = new QTableView;
    view->setModel(proxyModel); // View uses PROXY, not SQL model!
    view->setItemDelegate(new QSqlRelationalDelegate(view));
    view->setSortingEnabled(true);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setAlternatingRowColors(true);
    view->horizontalHeader()->setStretchLastSection(true);
    view->resizeColumnsToContents();
    
    // ========================================
    // UI Controls
    // ========================================
    QWidget window;
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);
    
    // Title
    QLabel *titleLabel = new QLabel("<h2>🏢 Employee Management System</h2>");
    mainLayout->addWidget(titleLabel);
    
    // ----------------------------------------
    // Filter Controls
    // ----------------------------------------
    QGroupBox *filterGroup = new QGroupBox("🔍 Filters");
    QFormLayout *filterLayout = new QFormLayout(filterGroup);
    
    // Search by name
    QLineEdit *searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Tìm kiếm theo tên...");
    searchEdit->setClearButtonEnabled(true);
    filterLayout->addRow("Tên:", searchEdit);
    
    // Department filter
    QComboBox *deptCombo = new QComboBox;
    deptCombo->addItem("All");
    QSqlQuery deptQuery("SELECT name FROM departments ORDER BY name");
    while (deptQuery.next()) {
        deptCombo->addItem(deptQuery.value(0).toString());
    }
    filterLayout->addRow("Phòng ban:", deptCombo);
    
    // Minimum salary
    QSpinBox *salarySpinBox = new QSpinBox;
    salarySpinBox->setRange(0, 100000000);
    salarySpinBox->setSingleStep(1000000);
    salarySpinBox->setSuffix(" đ");
    salarySpinBox->setValue(0);
    filterLayout->addRow("Lương tối thiểu:", salarySpinBox);
    
    mainLayout->addWidget(filterGroup);
    
    // ----------------------------------------
    // Statistics Label
    // ----------------------------------------
    QLabel *statsLabel = new QLabel;
    auto updateStats = [=]() {
        int total = sqlModel->rowCount();
        int filtered = proxyModel->rowCount();
        statsLabel->setText(QString("📊 Hiển thị: <b>%1</b> / <b>%2</b> nhân viên")
                           .arg(filtered).arg(total));
    };
    updateStats();
    mainLayout->addWidget(statsLabel);
    
    // ----------------------------------------
    // Table View
    // ----------------------------------------
    mainLayout->addWidget(view);
    
    // ----------------------------------------
    // Action Buttons
    // ----------------------------------------
    QHBoxLayout *btnLayout = new QHBoxLayout;
    
    QPushButton *addBtn = new QPushButton("➕ Thêm Nhân Viên");
    QPushButton *deleteBtn = new QPushButton("🗑️ Xóa");
    QPushButton *saveBtn = new QPushButton("💾 Lưu Thay Đổi");
    QPushButton *revertBtn = new QPushButton("↶ Hoàn Tác");
    QPushButton *refreshBtn = new QPushButton("🔄 Refresh");
    
    saveBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    deleteBtn->setStyleSheet("background-color: #f44336; color: white;");
    
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(revertBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(refreshBtn);
    
    mainLayout->addLayout(btnLayout);
    
    // ========================================
    // Connect Signals
    // ========================================
    
    // Search filter
    QObject::connect(searchEdit, &QLineEdit::textChanged,
                    proxyModel, &EmployeeFilterProxy::setSearchText);
    
    // Department filter
    QObject::connect(deptCombo, &QComboBox::currentTextChanged,
                    proxyModel, &EmployeeFilterProxy::setDepartmentFilter);
    
    // Salary filter
    QObject::connect(salarySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    proxyModel, &EmployeeFilterProxy::setMinSalary);
    
    // Update stats when filter changes
    QObject::connect(proxyModel, &QAbstractItemModel::modelReset, updateStats);
    QObject::connect(proxyModel, &QAbstractItemModel::rowsInserted, updateStats);
    QObject::connect(proxyModel, &QAbstractItemModel::rowsRemoved, updateStats);
    
    // Add button
    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        int row = sqlModel->rowCount();
        sqlModel->insertRow(row);
        
        // Set default values
        sqlModel->setData(sqlModel->index(row, 1), "Nhân viên mới");
        sqlModel->setData(sqlModel->index(row, 2), 25);
        sqlModel->setData(sqlModel->index(row, 3), 10000000);
        sqlModel->setData(sqlModel->index(row, 5), QDate::currentDate().toString("yyyy-MM-dd"));
        sqlModel->setData(sqlModel->index(row, 6), "email@company.com");
        
        updateStats();
    });
    
    // Delete button
    QObject::connect(deleteBtn, &QPushButton::clicked, [=, &window]() {
        QModelIndex proxyIndex = view->currentIndex();
        if (!proxyIndex.isValid()) {
            QMessageBox::warning(&window, "Warning", "Vui lòng chọn nhân viên cần xóa!");
            return;
        }
        
        // Map proxy index to source index
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        
        QString name = sqlModel->data(sqlModel->index(sourceIndex.row(), 1)).toString();
        
        int ret = QMessageBox::question(&window, "Xác nhận", 
                                        QString("Xóa nhân viên '%1'?").arg(name));
        if (ret == QMessageBox::Yes) {
            sqlModel->removeRow(sourceIndex.row());
            updateStats();
        }
    });
    
    // Save button
    QObject::connect(saveBtn, &QPushButton::clicked, [=, &window]() {
        if (sqlModel->submitAll()) {
            QMessageBox::information(&window, "Success", "✅ Đã lưu thành công!");
            sqlModel->select();
            updateStats();
        } else {
            QMessageBox::critical(&window, "Error", 
                                 "❌ Lỗi: " + sqlModel->lastError().text());
        }
    });
    
    // Revert button
    QObject::connect(revertBtn, &QPushButton::clicked, [=]() {
        sqlModel->revertAll();
        updateStats();
    });
    
    // Refresh button
    QObject::connect(refreshBtn, &QPushButton::clicked, [=]() {
        sqlModel->select();
        updateStats();
    });
    
    // ========================================
    // Show Window
    // ========================================
    window.resize(1000, 600);
    window.setWindowTitle("Qt Models Complete Example");
    window.show();
    
    return app.exec();
}

#include "06_complete_example.moc"

// ============================================
// TÓM TẮT KIẾN TRÚC:
// ============================================
//
// DATA FLOW:
// ┌──────────────────────────────────────────────┐
// │         SQLite Database (employees)          │
// └─────────────────┬────────────────────────────┘
//                   │
//                   ▼
// ┌──────────────────────────────────────────────┐
// │    QSqlRelationalTableModel (sqlModel)       │
// │    - Kết nối với DB                          │
// │    - Xử lý foreign keys                      │
// │    - CRUD operations                         │
// └─────────────────┬────────────────────────────┘
//                   │
//                   ▼
// ┌──────────────────────────────────────────────┐
// │  EmployeeFilterProxy (proxyModel)            │
// │    - Filter by name, department, salary      │
// │    - Sort                                    │
// │    - Không thay đổi source data             │
// └─────────────────┬────────────────────────────┘
//                   │
//                   ▼
// ┌──────────────────────────────────────────────┐
// │         QTableView (view)                    │
// │    - Hiển thị data                          │
// │    - User interaction                        │
// │    - QSqlRelationalDelegate (editing)        │
// └──────────────────────────────────────────────┘
//
// KEY POINTS:
// 1. View luôn dùng PROXY model, không dùng SQL model trực tiếp
// 2. Khi get current index từ view → phải map sang source index
//    proxyModel->mapToSource(viewIndex) → sourceIndex
// 3. SQL model xử lý database operations
// 4. Proxy model xử lý filtering/sorting
// 5. View chỉ hiển thị và nhận input
//
// BENEFITS:
// ✓ Separation of concerns
// ✓ Reusable components
// ✓ Easy to test
// ✓ Flexible filtering without changing database
// ✓ Multiple views can share same model
