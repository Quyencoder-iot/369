#ifndef STUDENTTABLEMODEL_H
#define STUDENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

/**
 * Example 2: QAbstractTableModel - Student Table
 * 
 * TABLE MODEL:
 * - 2 chiều (rows & columns)
 * - Dùng cho data dạng bảng
 * - Cần implement: rowCount(), columnCount(), data()
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
        EmailRole,
        PhoneRole
    };
    Q_ENUM(StudentRoles)
    
    enum ColumnIndex {
        IdColumn = 0,
        NameColumn,
        AgeColumn,
        GradeColumn,
        EmailColumn,
        PhoneColumn
    };
    Q_ENUM(ColumnIndex)
    
    explicit StudentTableModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface - BẮT BUỘC
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Methods for QML
    Q_INVOKABLE void addStudent(const QString &name, int age, const QString &grade, 
                                const QString &email, const QString &phone = "");
    Q_INVOKABLE void removeStudent(int row);
    Q_INVOKABLE QVariant get(int row, const QString &roleName) const;
    Q_INVOKABLE void set(int row, const QString &roleName, const QVariant &value);
    Q_INVOKABLE int gradeACount() const;
    Q_INVOKABLE double averageAge() const;
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    
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
        QString phone;
    };
    
    QVector<Student> m_students;
    int m_nextId;
    
    int roleFromName(const QString &roleName) const;
};

#endif // STUDENTTABLEMODEL_H
