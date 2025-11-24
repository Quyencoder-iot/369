#include "StudentTableModel.h"
#include <algorithm>

StudentTableModel::StudentTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_nextId(6)
{
    // Sample data
    m_students = {
        {1, "Nguyễn Văn Anh", 20, "A", "anh@example.com", "0901234567"},
        {2, "Trần Thị Bình", 19, "B+", "binh@example.com", "0912345678"},
        {3, "Lê Văn Cường", 21, "A+", "cuong@example.com", "0923456789"},
        {4, "Phạm Thị Dung", 20, "C", "dung@example.com", "0934567890"},
        {5, "Hoàng Văn Em", 22, "B", "em@example.com", "0945678901"}
    };
}

int StudentTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_students.count();
}

int StudentTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 6;
}

QVariant StudentTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_students.size())
        return QVariant();
    
    const Student &student = m_students.at(index.row());
    
    // For QML
    if (role >= Qt::UserRole) {
        switch (role) {
            case IdRole: return student.id;
            case NameRole: return student.name;
            case AgeRole: return student.age;
            case GradeRole: return student.grade;
            case EmailRole: return student.email;
            case PhoneRole: return student.phone;
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
            case PhoneColumn: return student.phone;
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
            case NameRole: student.name = value.toString(); break;
            case AgeRole: student.age = value.toInt(); break;
            case GradeRole: student.grade = value.toString(); break;
            case EmailRole: student.email = value.toString(); break;
            case PhoneRole: student.phone = value.toString(); break;
            default: return false;
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
            case PhoneColumn: return "SĐT";
        }
    }
    return QVariant();
}

Qt::ItemFlags StudentTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() != IdColumn)
        flags |= Qt::ItemIsEditable;
    
    return flags;
}

QHash<int, QByteArray> StudentTableModel::roleNames() const
{
    return {
        {IdRole, "studentId"},
        {NameRole, "name"},
        {AgeRole, "age"},
        {GradeRole, "grade"},
        {EmailRole, "email"},
        {PhoneRole, "phone"}
    };
}

void StudentTableModel::addStudent(const QString &name, int age, const QString &grade, 
                                   const QString &email, const QString &phone)
{
    int row = m_students.count();
    beginInsertRows(QModelIndex(), row, row);
    m_students.append({m_nextId++, name, age, grade, email, phone});
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

QVariant StudentTableModel::get(int row, const QString &roleName) const
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

int StudentTableModel::gradeACount() const
{
    int count = 0;
    for (const Student &s : m_students) {
        if (s.grade.startsWith("A")) count++;
    }
    return count;
}

double StudentTableModel::averageAge() const
{
    if (m_students.isEmpty()) return 0;
    
    int total = 0;
    for (const Student &s : m_students) {
        total += s.age;
    }
    return static_cast<double>(total) / m_students.count();
}

void StudentTableModel::sort(int column, Qt::SortOrder order)
{
    emit layoutAboutToBeChanged();
    
    std::sort(m_students.begin(), m_students.end(), 
             [column, order](const Student &a, const Student &b) {
        bool less = false;
        switch (column) {
            case IdColumn: less = a.id < b.id; break;
            case NameColumn: less = a.name < b.name; break;
            case AgeColumn: less = a.age < b.age; break;
            case GradeColumn: less = a.grade < b.grade; break;
            case EmailColumn: less = a.email < b.email; break;
            case PhoneColumn: less = a.phone < b.phone; break;
        }
        return order == Qt::AscendingOrder ? less : !less;
    });
    
    emit layoutChanged();
}

int StudentTableModel::roleFromName(const QString &roleName) const
{
    if (roleName == "studentId") return IdRole;
    if (roleName == "name") return NameRole;
    if (roleName == "age") return AgeRole;
    if (roleName == "grade") return GradeRole;
    if (roleName == "email") return EmailRole;
    if (roleName == "phone") return PhoneRole;
    return Qt::DisplayRole;
}
