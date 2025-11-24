#include "StudentTableModel.h"

StudentTableModel::StudentTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_nextId(6)
{
    // Sample data
    m_students = {
        {1, "Nguyễn Văn A", 20, "A", "a@example.com"},
        {2, "Trần Thị B", 19, "B+", "b@example.com"},
        {3, "Lê Văn C", 21, "A+", "c@example.com"},
        {4, "Phạm Thị D", 20, "C", "d@example.com"},
        {5, "Hoàng Văn E", 22, "B", "e@example.com"}
    };
}

int StudentTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_students.count();
}

int StudentTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 5;
}

QVariant StudentTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_students.size())
        return QVariant();
    
    const Student &student = m_students.at(index.row());
    
    // For QML TableView
    if (role >= Qt::UserRole) {
        switch (role) {
            case IdRole: return student.id;
            case NameRole: return student.name;
            case AgeRole: return student.age;
            case GradeRole: return student.grade;
            case EmailRole: return student.email;
        }
    }
    
    // For Qt Widgets
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case IdColumn: return student.id;
            case NameColumn: return student.name;
            case AgeColumn: return student.age;
            case GradeColumn: return student.grade;
            case EmailColumn: return student.email;
        }
    }
    
    return QVariant();
}

bool StudentTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_students.size())
        return false;
    
    Student &student = m_students[index.row()];
    
    if (role == Qt::EditRole || role >= Qt::UserRole) {
        int actualRole = (role == Qt::EditRole) ? (Qt::UserRole + index.column() + 1) : role;
        
        switch (actualRole) {
            case NameRole:
                student.name = value.toString();
                break;
            case AgeRole:
                student.age = value.toInt();
                break;
            case GradeRole:
                student.grade = value.toString();
                break;
            case EmailRole:
                student.email = value.toString();
                break;
            default:
                return false;
        }
        
        emit dataChanged(index, index, {role});
        return true;
    }
    
    return false;
}

QVariant StudentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case IdColumn: return "ID";
            case NameColumn: return "Tên";
            case AgeColumn: return "Tuổi";
            case GradeColumn: return "Điểm";
            case EmailColumn: return "Email";
        }
    }
    return QVariant();
}

Qt::ItemFlags StudentTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    
    // ID column is not editable
    if (index.column() != IdColumn)
        flags |= Qt::ItemIsEditable;
    
    return flags;
}

QHash<int, QByteArray> StudentTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "studentId";
    roles[NameRole] = "name";
    roles[AgeRole] = "age";
    roles[GradeRole] = "grade";
    roles[EmailRole] = "email";
    return roles;
}

void StudentTableModel::addStudent(const QString &name, int age, const QString &grade, const QString &email)
{
    int row = m_students.count();
    beginInsertRows(QModelIndex(), row, row);
    m_students.append({m_nextId++, name, age, grade, email});
    endInsertRows();
    emit rowCountChanged();
}

void StudentTableModel::removeStudent(int row)
{
    if (row < 0 || row >= m_students.count())
        return;
    
    beginRemoveRows(QModelIndex(), row, row);
    m_students.remove(row);
    endRemoveRows();
    emit rowCountChanged();
}

QVariant StudentTableModel::get(int row, const QString &roleName)
{
    if (row < 0 || row >= m_students.count())
        return QVariant();
    
    int role = roleFromName(roleName);
    QModelIndex index = createIndex(row, 0);
    return data(index, role);
}

void StudentTableModel::set(int row, const QString &roleName, const QVariant &value)
{
    if (row < 0 || row >= m_students.count())
        return;
    
    int role = roleFromName(roleName);
    QModelIndex index = createIndex(row, 0);
    setData(index, value, role);
}

int StudentTableModel::roleFromName(const QString &roleName) const
{
    if (roleName == "studentId") return IdRole;
    if (roleName == "name") return NameRole;
    if (roleName == "age") return AgeRole;
    if (roleName == "grade") return GradeRole;
    if (roleName == "email") return EmailRole;
    return Qt::DisplayRole;
}
