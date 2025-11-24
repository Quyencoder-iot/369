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
