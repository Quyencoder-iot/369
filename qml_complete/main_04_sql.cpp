/**
 * ============================================
 * EXAMPLE 4: EMPLOYEE SQL APPLICATION
 * ============================================
 * 
 * C++ Model: EmployeeSqlModel (QSqlRelationalTableModel wrapper)
 * QML View: EmployeeSqlView.qml
 * 
 * PURPOSE:
 * Demonstrate how to integrate SQL DATABASE with QML
 * 
 * KEY FEATURES:
 * - Direct database table mapping
 * - CRUD operations (Create, Read, Update, Delete)
 * - Foreign key relationships
 * - Transaction support (submit/revert)
 * - Auto-save or manual commit
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "models/EmployeeSqlModel.h"

/**
 * Initialize SQLite database with tables and sample data
 */
bool initDatabase()
{
    // ============================================
    // STEP 1: Create in-memory SQLite database
    // ============================================
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");  // In-memory (temporary)
    
    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Database opened successfully";
    
    QSqlQuery query;
    
    // ============================================
    // STEP 2: Create departments table
    // ============================================
    QString createDepartments = R"(
        CREATE TABLE departments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        )
    )";
    
    if (!query.exec(createDepartments)) {
        qCritical() << "Failed to create departments table:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Departments table created";
    
    // ============================================
    // STEP 3: Create employees table with foreign key
    // ============================================
    QString createEmployees = R"(
        CREATE TABLE employees (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            position TEXT NOT NULL,
            department_id INTEGER NOT NULL,
            salary INTEGER NOT NULL,
            FOREIGN KEY (department_id) REFERENCES departments(id)
        )
    )";
    
    if (!query.exec(createEmployees)) {
        qCritical() << "Failed to create employees table:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Employees table created";
    
    // ============================================
    // STEP 4: Insert sample departments
    // ============================================
    QStringList departments = {
        "Engineering",
        "Marketing",
        "Sales",
        "HR",
        "Finance"
    };
    
    for (const QString &dept : departments) {
        query.prepare("INSERT INTO departments (name) VALUES (?)");
        query.addBindValue(dept);
        if (!query.exec()) {
            qCritical() << "Failed to insert department:" << query.lastError().text();
            return false;
        }
    }
    
    qDebug() << "✅ Sample departments inserted";
    
    // ============================================
    // STEP 5: Insert sample employees
    // ============================================
    struct Employee {
        QString name;
        QString position;
        int departmentId;
        int salary;
    };
    
    QList<Employee> employees = {
        {"Alice Johnson", "Software Engineer", 1, 85000},
        {"Bob Smith", "Marketing Manager", 2, 75000},
        {"Carol White", "Sales Representative", 3, 55000},
        {"David Brown", "HR Specialist", 4, 60000},
        {"Eva Davis", "Financial Analyst", 5, 70000},
        {"Frank Wilson", "Senior Developer", 1, 95000},
        {"Grace Lee", "Product Manager", 1, 90000},
        {"Henry Taylor", "Sales Manager", 3, 80000}
    };
    
    for (const Employee &emp : employees) {
        query.prepare("INSERT INTO employees (name, position, department_id, salary) "
                     "VALUES (?, ?, ?, ?)");
        query.addBindValue(emp.name);
        query.addBindValue(emp.position);
        query.addBindValue(emp.departmentId);
        query.addBindValue(emp.salary);
        
        if (!query.exec()) {
            qCritical() << "Failed to insert employee:" << query.lastError().text();
            return false;
        }
    }
    
    qDebug() << "✅ Sample employees inserted";
    
    return true;
}

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // ============================================
    // Initialize Database
    // ============================================
    if (!initDatabase()) {
        qCritical() << "❌ Failed to initialize database!";
        return -1;
    }
    
    // ============================================
    // Create QML Engine
    // ============================================
    QQmlApplicationEngine engine;
    
    // ============================================
    // Create SQL Model
    // ============================================
    EmployeeSqlModel employeeModel;
    
    // ============================================
    // Expose to QML
    // ============================================
    engine.rootContext()->setContextProperty("employeeModel", &employeeModel);
    
    // ============================================
    // Load QML
    // ============================================
    const QUrl url(QStringLiteral("qrc:/qml/EmployeeSqlView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

/**
 * ============================================
 * 📚 SUMMARY - SQL MODEL INTEGRATION
 * ============================================
 * 
 * 1. SQL MODEL vs IN-MEMORY MODELS:
 *    IN-MEMORY (List/Table/Tree):
 *    - Data stored in C++ containers (QList, QVector)
 *    - Manual persistence (save/load files)
 *    - Full control over data structure
 *    
 *    SQL MODEL:
 *    - Data stored in database
 *    - Auto-persistence
 *    - Database schema constraints
 *    - Transaction support
 *    - Foreign key relationships
 * 
 * 2. QSqlTableModel HIERARCHY:
 *    QAbstractItemModel
 *         ↑
 *    QSqlQueryModel (read-only)
 *         ↑
 *    QSqlTableModel (editable, single table)
 *         ↑
 *    QSqlRelationalTableModel (foreign keys)
 * 
 * 3. DATABASE SETUP:
 *    ✅ QSqlDatabase::addDatabase("QSQLITE")
 *    ✅ db.setDatabaseName(":memory:" or "file.db")
 *    ✅ db.open()
 *    ✅ CREATE TABLE queries
 *    ✅ INSERT sample data
 * 
 * 4. MODEL SETUP:
 *    ✅ model.setTable("employees")
 *    ✅ model.setRelation(col, QSqlRelation(...))  ← Foreign keys
 *    ✅ model.setEditStrategy(OnManualSubmit)
 *    ✅ model.select()  ← Load data
 * 
 * 5. FOREIGN KEY RELATIONS:
 *    QSqlRelation(foreignTable, foreignKey, displayColumn)
 *    
 *    Example:
 *      model.setRelation(3, QSqlRelation("departments", "id", "name"));
 *      
 *    Result:
 *      Instead of showing department_id (1, 2, 3...)
 *      Shows department name ("Engineering", "Marketing", ...)
 * 
 * 6. EDIT STRATEGIES:
 *    OnFieldChange - Auto-save each cell edit
 *    OnRowChange - Save when moving to different row
 *    OnManualSubmit - Require explicit submit() call
 *    
 *    OnManualSubmit allows:
 *      ✅ Batch edits
 *      ✅ Validation before save
 *      ✅ Revert changes
 * 
 * 7. TRANSACTION WORKFLOW:
 *    User edits cell
 *         ↓
 *    setData(index, value)
 *         ↓
 *    Model marks row as dirty
 *         ↓
 *    hasUnsavedChanges = true
 *         ↓
 *    User clicks "Save"
 *         ↓
 *    model.submit()
 *         ↓
 *    SQL UPDATE executed
 *         ↓
 *    Database committed
 * 
 * 8. CRUD OPERATIONS:
 *    CREATE: model.insertRow() + setData() + submit()
 *    READ:   model.select()
 *    UPDATE: setData() + submit()
 *    DELETE: model.removeRow() + submit()
 * 
 * 9. ADVANTAGES:
 *    ✅ No manual SQL queries in C++
 *    ✅ Auto-sync with database
 *    ✅ Built-in transaction support
 *    ✅ Foreign key resolution
 *    ✅ Filtering and sorting
 * 
 * 10. QML INTEGRATION:
 *     Same as other models!
 *     - setContextProperty
 *     - Access via roles
 *     - Call Q_INVOKABLE methods
 *     - Property binding works
 * 
 * ============================================
 * 💡 LEARNING POINTS
 * ============================================
 * 
 * ★ Database initialization:
 *   QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
 *   db.setDatabaseName(":memory:");  // Or "file.db"
 *   db.open();
 * 
 * ★ Table model setup:
 *   QSqlTableModel model;
 *   model.setTable("table_name");
 *   model.select();  // Load data
 * 
 * ★ Foreign keys:
 *   model.setRelation(column, QSqlRelation("foreign_table", "fk", "display"));
 *   → Shows human-readable text instead of IDs
 * 
 * ★ Manual transactions:
 *   model.setEditStrategy(QSqlTableModel::OnManualSubmit);
 *   // ... edits ...
 *   model.submit();  // Commit
 *   // OR
 *   model.revert();  // Rollback
 * 
 * ★ Custom wrapper class benefits:
 *   - Add Q_PROPERTY for easy QML access
 *   - Add Q_INVOKABLE helper methods
 *   - Add business logic (validation, calculations)
 *   - Customize role names for QML
 * 
 * ============================================
 */
