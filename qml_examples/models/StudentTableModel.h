#ifndef STUDENTTABLEMODEL_H
#define STUDENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

/**
 * StudentTableModel - C++ Table Model cho QML
 * 
 * Model bảng sinh viên với columns:
 * - ID, Name, Age, Grade, Email
 */
class StudentTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged)
    
public:
    enum StudentRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        AgeRole,
        GradeRole,
        EmailRole
    };
    Q_ENUM(StudentRoles)
    
    enum ColumnIndex {
        IdColumn = 0,
        NameColumn,
        AgeColumn,
        GradeColumn,
        EmailColumn
    };
    Q_ENUM(ColumnIndex)
    
    explicit StudentTableModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Methods for QML
    Q_INVOKABLE void addStudent(const QString &name, int age, const QString &grade, const QString &email);
    Q_INVOKABLE void removeStudent(int row);
    Q_INVOKABLE QVariant get(int row, const QString &roleName);
    Q_INVOKABLE void set(int row, const QString &roleName, const QVariant &value);
    
signals:
    void rowCountChanged();
    void columnCountChanged();
    
private:
    struct Student {
        int id;
        QString name;
        int age;
        QString grade;
        QString email;
    };
    
    QVector<Student> m_students;
    int m_nextId;
    
    int roleFromName(const QString &roleName) const;
};

#endif // STUDENTTABLEMODEL_H
