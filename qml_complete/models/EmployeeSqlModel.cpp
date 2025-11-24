#include "EmployeeSqlModel.h"
#include <QSqlRelation>
#include <QSqlRecord>

EmployeeSqlModel::EmployeeSqlModel(QObject *parent)
    : QObject(parent), m_model(nullptr), m_connected(false)
{
    m_model = new QSqlRelationalTableModel(this);
}

EmployeeSqlModel::~EmployeeSqlModel()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool EmployeeSqlModel::connectDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "employee_connection");
    m_db.setDatabaseName(":memory:"); // In-memory database
    
    if (!m_db.open()) {
        emit errorOccurred("Cannot open database: " + m_db.lastError().text());
        return false;
    }
    
    if (!createTables()) {
        emit errorOccurred("Cannot create tables");
        return false;
    }
    
    if (!insertSampleData()) {
        emit errorOccurred("Cannot insert sample data");
        return false;
    }
    
    // Setup model
    m_model->setDatabase(m_db);
    m_model->setTable("employees");
    
    // Setup foreign key relation
    m_model->setRelation(4, QSqlRelation("departments", "id", "name"));
    
    // Set headers
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Name");
    m_model->setHeaderData(2, Qt::Horizontal, "Age");
    m_model->setHeaderData(3, Qt::Horizontal, "Salary");
    m_model->setHeaderData(4, Qt::Horizontal, "Department");
    
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    
    m_connected = true;
    emit connectedChanged();
    emit rowCountChanged();
    
    return true;
}

bool EmployeeSqlModel::createTables()
{
    QSqlQuery query(m_db);
    
    // Departments table
    if (!query.exec("CREATE TABLE departments ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "location TEXT)")) {
        return false;
    }
    
    // Employees table
    if (!query.exec("CREATE TABLE employees ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "age INTEGER, "
                   "salary REAL, "
                   "department_id INTEGER, "
                   "FOREIGN KEY (department_id) REFERENCES departments(id))")) {
        return false;
    }
    
    return true;
}

bool EmployeeSqlModel::insertSampleData()
{
    QSqlQuery query(m_db);
    
    // Insert departments
    query.exec("INSERT INTO departments (name, location) VALUES "
               "('Engineering', 'Floor 3'), "
               "('HR', 'Floor 2'), "
               "('Sales', 'Floor 1'), "
               "('Marketing', 'Floor 1')");
    
    // Insert employees
    query.exec("INSERT INTO employees (name, age, salary, department_id) VALUES "
               "('Nguyễn Văn Anh', 28, 18000000, 1), "
               "('Trần Thị Bình', 25, 15000000, 1), "
               "('Lê Văn Cường', 32, 22000000, 1), "
               "('Phạm Thị Dung', 30, 14000000, 2), "
               "('Hoàng Văn Em', 27, 13000000, 2), "
               "('Vũ Thị Phượng', 29, 19000000, 3), "
               "('Đỗ Văn Giang', 31, 17000000, 3), "
               "('Bùi Thị Hà', 26, 16000000, 4)");
    
    return !query.lastError().isValid();
}

int EmployeeSqlModel::rowCount() const
{
    return m_model ? m_model->rowCount() : 0;
}

void EmployeeSqlModel::addEmployee(const QString &name, int age, double salary, int departmentId)
{
    if (!m_model) return;
    
    int row = m_model->rowCount();
    m_model->insertRow(row);
    m_model->setData(m_model->index(row, 1), name);
    m_model->setData(m_model->index(row, 2), age);
    m_model->setData(m_model->index(row, 3), salary);
    m_model->setData(m_model->index(row, 4), departmentId);
    
    emit rowCountChanged();
}

void EmployeeSqlModel::removeEmployee(int row)
{
    if (!m_model || row < 0 || row >= m_model->rowCount())
        return;
    
    m_model->removeRow(row);
    emit rowCountChanged();
}

bool EmployeeSqlModel::submitChanges()
{
    if (!m_model) return false;
    
    if (m_model->submitAll()) {
        m_model->select();
        emit rowCountChanged();
        return true;
    } else {
        emit errorOccurred("Submit failed: " + m_model->lastError().text());
        return false;
    }
}

void EmployeeSqlModel::revertChanges()
{
    if (m_model) {
        m_model->revertAll();
        emit rowCountChanged();
    }
}

void EmployeeSqlModel::refreshData()
{
    if (m_model) {
        m_model->select();
        emit rowCountChanged();
    }
}

QVariant EmployeeSqlModel::getData(int row, const QString &fieldName) const
{
    if (!m_model) return QVariant();
    
    int col = fieldIndex(fieldName);
    if (col < 0) return QVariant();
    
    return m_model->data(m_model->index(row, col));
}

void EmployeeSqlModel::setData(int row, const QString &fieldName, const QVariant &value)
{
    if (!m_model) return;
    
    int col = fieldIndex(fieldName);
    if (col < 0) return;
    
    m_model->setData(m_model->index(row, col), value);
}

void EmployeeSqlModel::filterByDepartment(const QString &departmentName)
{
    if (!m_model) return;
    
    if (departmentName.isEmpty() || departmentName == "All") {
        clearFilter();
    } else {
        m_model->setFilter(QString("departments.name = '%1'").arg(departmentName));
        emit rowCountChanged();
    }
}

void EmployeeSqlModel::clearFilter()
{
    if (m_model) {
        m_model->setFilter("");
        emit rowCountChanged();
    }
}

double EmployeeSqlModel::averageSalary() const
{
    QSqlQuery query(m_db);
    query.exec("SELECT AVG(salary) FROM employees");
    
    if (query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

int EmployeeSqlModel::employeeCount() const
{
    return m_model ? m_model->rowCount() : 0;
}

QStringList EmployeeSqlModel::getDepartmentNames() const
{
    QStringList names;
    QSqlQuery query(m_db);
    query.exec("SELECT name FROM departments ORDER BY name");
    
    while (query.next()) {
        names << query.value(0).toString();
    }
    
    return names;
}

int EmployeeSqlModel::getDepartmentId(const QString &name) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM departments WHERE name = ?");
    query.addBindValue(name);
    query.exec();
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int EmployeeSqlModel::fieldIndex(const QString &fieldName) const
{
    if (!m_model) return -1;
    
    if (fieldName == "id") return 0;
    if (fieldName == "name") return 1;
    if (fieldName == "age") return 2;
    if (fieldName == "salary") return 3;
    if (fieldName == "department") return 4;
    
    return -1;
}

/**
 * ============================================
 * 📚 SUMMARY - EmployeeSqlModel Implementation
 * ============================================
 * 
 * MODEL TYPE: QSqlRelationalTableModel (Database)
 * 
 * KEY IMPLEMENTATIONS:
 * 
 * 1. connectDatabase():
 *    - Create QSqlDatabase connection
 *    - Use :memory: for in-memory DB (temporary)
 *    - Call createTables() to setup schema
 *    - Call insertSampleData() to populate
 *    - Setup QSqlRelationalTableModel
 *    - Set foreign key relation
 *    - Set edit strategy (OnManualSubmit)
 *    - Call select() to load data
 * 
 * 2. createTables():
 *    - CREATE TABLE departments (id, name, location)
 *    - CREATE TABLE employees (id, name, age, salary, department_id)
 *    - FOREIGN KEY (department_id) REFERENCES departments(id)
 * 
 * 3. insertSampleData():
 *    - INSERT INTO departments (4 departments)
 *    - INSERT INTO employees (8 employees)
 * 
 * 4. setRelation():
 *    setRelation(4, QSqlRelation("departments", "id", "name"))
 *    → Column 4 (department_id) shows departments.name
 * 
 * 5. CRUD OPERATIONS:
 *    - addEmployee(): insertRow() + setData() for each column
 *    - removeEmployee(): removeRow()
 *    - getData(): Get value from model by fieldName
 *    - setData(): Set value in model by fieldName
 * 
 * 6. TRANSACTIONS:
 *    - submitChanges(): submitAll() + select() → COMMIT to DB
 *    - revertChanges(): revertAll() → ROLLBACK changes
 *    - refreshData(): select() → Re-query from DB
 * 
 * 7. FILTERING:
 *    - filterByDepartment(): setFilter("departments.name = 'X'")
 *    - Uses SQL WHERE clause
 *    - Fast filtering (done by database)
 * 
 * 8. STATISTICS:
 *    - averageSalary(): SELECT AVG(salary) FROM employees
 *    - Direct SQL query, not through model
 * 
 * DATA FLOW EXAMPLE (Add Employee):
 * QML: employeeModel.addEmployee("John", 30, 50000, 1)
 *   ↓
 * C++: Get row = m_model->rowCount()
 *   ↓
 * C++: m_model->insertRow(row)
 *   ↓
 * C++: m_model->setData(index(row, 1), "John")
 * C++: m_model->setData(index(row, 2), 30)
 * C++: m_model->setData(index(row, 3), 50000)
 * C++: m_model->setData(index(row, 4), 1)
 *   ↓
 * C++: emit rowCountChanged()
 *   ↓
 * QML: rowCount property updates
 *   ↓
 * User clicks "Submit"
 *   ↓
 * QML: employeeModel.submitChanges()
 *   ↓
 * C++: m_model->submitAll()
 *   ↓
 * SQL: INSERT INTO employees VALUES (...)
 *   ↓
 * Database: Row inserted
 *   ↓
 * C++: m_model->select() to refresh
 *   ↓
 * QML: TableView shows new employee
 * 
 * FOREIGN KEY RESOLUTION:
 * Database stores: department_id = 1
 *   ↓
 * QSqlRelation configured:
 *   setRelation(4, QSqlRelation("departments", "id", "name"))
 *   ↓
 * Model automatically JOINs:
 *   SELECT employees.*, departments.name 
 *   FROM employees 
 *   JOIN departments ON employees.department_id = departments.id
 *   ↓
 * UI displays: "Engineering" instead of "1"
 * 
 * EDIT STRATEGY COMPARISON:
 * 
 * OnFieldChange:
 * - Auto-save each cell edit
 * - COMMIT immediately
 * - Cannot revert
 * - Use for simple forms
 * 
 * OnRowChange:
 * - Save when moving to next row
 * - COMMIT per row
 * - Can revert current row
 * - Use for row-by-row editing
 * 
 * OnManualSubmit (USED HERE):
 * - Buffer all changes
 * - Require explicit submit()
 * - Can revert all changes
 * - Use for batch editing
 * - Best for QML (user controls save)
 * 
 * BEST PRACTICES:
 * ✅ Use connection name for multiple DBs
 * ✅ Check database.open() success
 * ✅ Handle QSqlError properly
 * ✅ Use prepared statements (query.prepare)
 * ✅ Call select() after schema changes
 * ✅ Use OnManualSubmit for QML apps
 * ✅ Emit signals for QML updates
 * ✅ Provide helper methods for common operations
 * 
 * ============================================
 */
