#ifndef EMPLOYEESQLMODEL_H
#define EMPLOYEESQLMODEL_H

#include <QObject>
#include <QSqlRelationalTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

/**
 * Example 4: QSqlRelationalTableModel - Employee Database
 * 
 * SQL MODEL:
 * - Kết nối trực tiếp với database
 * - QSqlRelationalTableModel: Foreign keys support
 * - Tự động CRUD operations
 */
class EmployeeSqlModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSqlRelationalTableModel* model READ model CONSTANT)
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    
public:
    explicit EmployeeSqlModel(QObject *parent = nullptr);
    ~EmployeeSqlModel();
    
    QSqlRelationalTableModel* model() const { return m_model; }
    int rowCount() const;
    bool isConnected() const { return m_connected; }
    
    // Methods for QML
    Q_INVOKABLE bool connectDatabase();
    Q_INVOKABLE void addEmployee(const QString &name, int age, double salary, int departmentId);
    Q_INVOKABLE void removeEmployee(int row);
    Q_INVOKABLE bool submitChanges();
    Q_INVOKABLE void revertChanges();
    Q_INVOKABLE void refreshData();
    Q_INVOKABLE QVariant getData(int row, const QString &fieldName) const;
    Q_INVOKABLE void setData(int row, const QString &fieldName, const QVariant &value);
    Q_INVOKABLE void filterByDepartment(const QString &departmentName);
    Q_INVOKABLE void clearFilter();
    Q_INVOKABLE double averageSalary() const;
    Q_INVOKABLE int employeeCount() const;
    
    // Department methods
    Q_INVOKABLE QStringList getDepartmentNames() const;
    Q_INVOKABLE int getDepartmentId(const QString &name) const;
    
signals:
    void rowCountChanged();
    void connectedChanged();
    void errorOccurred(const QString &error);
    
private:
    QSqlRelationalTableModel *m_model;
    QSqlDatabase m_db;
    bool m_connected;
    
    bool createTables();
    bool insertSampleData();
    int fieldIndex(const QString &fieldName) const;
};

#endif // EMPLOYEESQLMODEL_H

/**
 * ============================================
 * 📚 SUMMARY - EmployeeSqlModel Header
 * ============================================
 * 
 * CLASS HIERARCHY:
 * QObject → EmployeeSqlModel (wrapper)
 *   └─ Contains: QSqlRelationalTableModel
 * 
 * MODEL TYPE: SQL MODEL (Database-backed)
 * 
 * WHY WRAPPER CLASS?
 * ✅ QSqlRelationalTableModel works but lacks convenient QML API
 * ✅ Wrapper adds Q_PROPERTY for easy QML access
 * ✅ Wrapper adds Q_INVOKABLE helper methods
 * ✅ Wrapper handles database setup/connection
 * ✅ Better error handling and signals
 * 
 * KEY DIFFERENCE từ In-Memory Models:
 * ✅ In-Memory (List/Table/Tree): Data in QVector/QList
 * ✅ SQL Model: Data in DATABASE (SQLite, PostgreSQL, etc.)
 * ✅ In-Memory: Manual persistence (save/load files)
 * ✅ SQL Model: Auto-persistence (database handles it)
 * ✅ In-Memory: Full control over data structure
 * ✅ SQL Model: Database schema defines structure
 * 
 * QSQLTABLEMODEL HIERARCHY:
 * QAbstractItemModel
 *   ↑
 * QSqlQueryModel (read-only SQL queries)
 *   ↑
 * QSqlTableModel (editable single table)
 *   ↑
 * QSqlRelationalTableModel (foreign keys!)
 * 
 * WHY QSqlRelationalTableModel?
 * - Supports FOREIGN KEYS
 * - Auto-resolves foreign key relationships
 * - Shows human-readable names instead of IDs
 * - Example: Shows "Engineering" instead of department_id=1
 * 
 * DATABASE SCHEMA:
 * 
 * departments table:
 * - id (INTEGER PRIMARY KEY)
 * - name (TEXT) - "Engineering", "HR", etc.
 * - location (TEXT) - "Floor 3", etc.
 * 
 * employees table:
 * - id (INTEGER PRIMARY KEY)
 * - name (TEXT) - Employee name
 * - age (INTEGER) - Employee age
 * - salary (REAL) - Salary amount
 * - department_id (INTEGER) - FOREIGN KEY → departments.id
 * 
 * FOREIGN KEY RELATION:
 * employees.department_id → departments.id (shows departments.name)
 * 
 * Q_PROPERTY cho QML:
 * ✅ model - QSqlRelationalTableModel* (READ model CONSTANT)
 *    → QML can access internal model directly if needed
 * ✅ rowCount - Number of employees (READ rowCount NOTIFY rowCountChanged)
 * ✅ connected - Database connection status (READ isConnected NOTIFY connectedChanged)
 * 
 * Q_INVOKABLE METHODS:
 * 
 * Database Setup:
 * ✅ connectDatabase() - Initialize DB, create tables, load data
 * 
 * CRUD Operations:
 * ✅ addEmployee(name, age, salary, deptId) - Add new employee
 * ✅ removeEmployee(row) - Remove employee
 * ✅ getData(row, fieldName) - Get cell value
 * ✅ setData(row, fieldName, value) - Set cell value
 * 
 * Transaction Control:
 * ✅ submitChanges() - COMMIT changes to database
 * ✅ revertChanges() - ROLLBACK changes
 * ✅ refreshData() - Re-query database
 * 
 * Filtering:
 * ✅ filterByDepartment(deptName) - SQL WHERE clause
 * ✅ clearFilter() - Remove filter
 * 
 * Statistics:
 * ✅ averageSalary() - AVG(salary) from SQL
 * ✅ employeeCount() - COUNT(*) from model
 * 
 * Department Helpers:
 * ✅ getDepartmentNames() - List of department names
 * ✅ getDepartmentId(name) - Get department ID by name
 * 
 * EDIT STRATEGY:
 * QSqlTableModel::OnManualSubmit
 * 
 * Why?
 * - Changes are BUFFERED
 * - NOT auto-saved to database
 * - Requires submitChanges() call
 * - Allows revertChanges() to discard
 * 
 * Alternatives:
 * - OnFieldChange: Auto-save each cell edit
 * - OnRowChange: Auto-save when moving rows
 * 
 * SIGNALS:
 * ✅ rowCountChanged() - Employee count changed
 * ✅ connectedChanged() - Database connection status changed
 * ✅ errorOccurred(QString) - Database error occurred
 * 
 * FOREIGN KEY SETUP:
 * m_model->setRelation(columnIndex, QSqlRelation(table, fk, display))
 * 
 * Example:
 * setRelation(4, QSqlRelation("departments", "id", "name"))
 * 
 * Means:
 * - Column 4 (department_id)
 * - References "departments" table
 * - Foreign key: "id" column
 * - Display: "name" column
 * 
 * Result:
 * Instead of showing department_id (1, 2, 3...)
 * Shows department name ("Engineering", "HR", ...)
 * 
 * QML ACCESS EXAMPLES:
 * 
 * 1. Initialize database:
 *    Component.onCompleted: employeeModel.connectDatabase()
 * 
 * 2. Add employee:
 *    Button {
 *        onClicked: employeeModel.addEmployee("John", 30, 50000, 1)
 *    }
 * 
 * 3. Submit changes:
 *    Button {
 *        onClicked: employeeModel.submitChanges()
 *    }
 * 
 * 4. Revert changes:
 *    Button {
 *        onClicked: employeeModel.revertChanges()
 *    }
 * 
 * 5. Filter:
 *    ComboBox {
 *        onCurrentTextChanged: employeeModel.filterByDepartment(currentText)
 *    }
 * 
 * 6. Statistics:
 *    Label { text: "Avg Salary: $" + employeeModel.averageSalary() }
 * 
 * KEY CONCEPTS:
 * 
 * 1. DATABASE CONNECTION:
 *    - QSqlDatabase handles connection
 *    - Connection name important for multiple DBs
 *    - In-memory (:memory:) for temp data
 *    - File path (file.db) for persistent data
 * 
 * 2. TABLE MAPPING:
 *    - setTable("employees") → Map model to table
 *    - Each row = database row
 *    - Each column = database column
 *    - Model automatically syncs with DB
 * 
 * 3. FOREIGN KEYS:
 *    - QSqlRelation resolves FKs
 *    - Shows related table's display column
 *    - Makes UI more user-friendly
 *    - No need to manually join tables
 * 
 * 4. TRANSACTIONS:
 *    - OnManualSubmit = buffered changes
 *    - submitAll() = COMMIT
 *    - revertAll() = ROLLBACK
 *    - Like database transactions
 * 
 * 5. FILTERING:
 *    - setFilter("WHERE clause") → SQL filtering
 *    - Fast (done by database)
 *    - Returns subset of rows
 *    - clearFilter() shows all rows
 * 
 * COMPARISON với In-Memory Models:
 * 
 * IN-MEMORY (List/Table/Tree):
 * - Data in C++ containers
 * - Fast access (in memory)
 * - Manual persistence
 * - Full control over structure
 * - No SQL knowledge needed
 * 
 * SQL MODEL:
 * - Data in database
 * - Slightly slower (DB queries)
 * - Auto-persistence
 * - Schema-driven structure
 * - Requires SQL knowledge
 * - Transaction support
 * - Foreign key relationships
 * - SQL filtering/sorting
 * 
 * WHEN TO USE SQL MODEL:
 * ✅ Large datasets (> 10,000 rows)
 * ✅ Need persistence
 * ✅ Multiple apps access same data
 * ✅ Complex relationships (foreign keys)
 * ✅ Advanced queries (JOIN, GROUP BY, etc.)
 * ✅ ACID transactions required
 * 
 * WHEN TO USE IN-MEMORY:
 * ✅ Small datasets (< 1,000 rows)
 * ✅ Temp data (no persistence needed)
 * ✅ Maximum performance
 * ✅ Complex custom logic
 * ✅ No database setup wanted
 * 
 * ============================================
 */
