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

/**
 * ============================================
 * 📚 SUMMARY - StudentTableModel Header
 * ============================================
 * 
 * CLASS HIERARCHY:
 * QObject → QAbstractItemModel → QAbstractTableModel → StudentTableModel
 * 
 * MODEL TYPE: TABLE MODEL (2D - Rows × Columns)
 * 
 * KEY DIFFERENCE từ ListModel:
 * ✅ ListModel: 1D (chỉ có rows)
 * ✅ TableModel: 2D (có rows VÀ columns)
 * ✅ Cần columnCount() thêm vào rowCount()
 * ✅ data() phải xử lý cả row VÀ column
 * 
 * REQUIRED OVERRIDES cho TableModel:
 * ✅ rowCount() - Số dòng
 * ✅ columnCount() - Số cột (QUAN TRỌNG!)
 * ✅ data() - Dữ liệu cell (row, column)
 * ✅ roleNames() - Map roles → QML names
 * ✅ Optional: setData() - Cho editable cells
 * ✅ Optional: headerData() - Column headers
 * ✅ Optional: flags() - Cell properties (editable, etc.)
 * 
 * DUAL ROLES SYSTEM:
 * 
 * 1. Qt Widget Roles (DisplayRole, EditRole):
 *    - Dùng index.column() để lấy column number
 *    - switch (index.column()) { case 0: ...; case 1: ...; }
 * 
 * 2. QML Custom Roles (UserRole + N):
 *    - Không quan tâm column
 *    - switch (role) { case NameRole: ...; case AgeRole: ...; }
 * 
 * ROLES ĐỊNH NGHĨA:
 * enum StudentRoles {
 *     IdRole = Qt::UserRole + 1,     // 256 → "studentId"
 *     NameRole,                       // 257 → "name"
 *     AgeRole,                        // 258 → "age"
 *     GradeRole,                      // 259 → "grade"
 *     EmailRole,                      // 260 → "email"
 *     PhoneRole                       // 261 → "phone"
 * };
 * 
 * COLUMN INDEX (cho Qt Widgets):
 * enum ColumnIndex {
 *     IdColumn = 0,      // Column 0
 *     NameColumn,        // Column 1
 *     AgeColumn,         // Column 2
 *     GradeColumn,       // Column 3
 *     EmailColumn,       // Column 4
 *     PhoneColumn        // Column 5
 * };
 * 
 * Q_PROPERTY cho QML:
 * ✅ rowCount - Số students (READ rowCount NOTIFY rowCountChanged)
 * ✅ columnCount - Số columns (READ columnCount NOTIFY columnCountChanged)
 * 
 * Q_INVOKABLE METHODS (callable từ QML):
 * ✅ addStudent(name, age, grade, email, phone)
 * ✅ removeStudent(row)
 * ✅ get(row, roleName) - Get cell value by name
 * ✅ set(row, roleName, value) - Set cell value by name
 * ✅ gradeACount() - Statistics
 * ✅ averageAge() - Statistics
 * ✅ sort(column, order) - Sort table
 * 
 * DATA STRUCTURE:
 * struct Student {
 *     int id;           // Auto-increment ID
 *     QString name;     // Student name
 *     int age;          // Age (15-100)
 *     QString grade;    // Grade (A+, A, B, C, D, F)
 *     QString email;    // Email address
 *     QString phone;    // Phone number
 * };
 * QVector<Student> m_students;  // Storage
 * 
 * EDITABLE CELLS:
 * flags() returns Qt::ItemIsEditable
 * → Cells có thể edit (trừ ID column)
 * setData() handles cell edits
 * → Updates m_students và emit dataChanged()
 * 
 * HEADER DATA:
 * headerData() provides column names:
 * - ID, Tên, Tuổi, Điểm, Email, SĐT
 * 
 * STATISTICS METHODS:
 * ✅ gradeACount() - Đếm students có grade A
 * ✅ averageAge() - Tính tuổi trung bình
 * 
 * QML ACCESS EXAMPLES:
 * 
 * 1. TableView binding:
 *    TableView {
 *        model: studentModel
 *        delegate: Rectangle {
 *            // row và column available here
 *            Text { text: model.name }  // Role access
 *        }
 *    }
 * 
 * 2. Get/Set by name:
 *    var name = studentModel.get(0, "name")
 *    studentModel.set(0, "age", 21)
 * 
 * 3. CRUD operations:
 *    studentModel.addStudent("John", 20, "A", "john@email.com")
 *    studentModel.removeStudent(2)
 * 
 * 4. Statistics:
 *    Label { text: "Grade A: " + studentModel.gradeACount() }
 *    Label { text: "Avg Age: " + studentModel.averageAge() }
 * 
 * KEY CONCEPTS:
 * 
 * 1. 2D INDEXING:
 *    QModelIndex has BOTH row AND column
 *    index.row() → Row number
 *    index.column() → Column number
 * 
 * 2. DUAL DATA ACCESS:
 *    Qt Widgets: Uses column number
 *    QML: Uses role names (ignores column)
 * 
 * 3. EDITABLE CELLS:
 *    flags() → Mark cells as editable
 *    setData() → Handle edits
 *    emit dataChanged() → Notify views
 * 
 * 4. SORTING:
 *    layoutAboutToBeChanged() → Before sort
 *    std::sort() → Reorder data
 *    layoutChanged() → After sort
 * 
 * IMPORTANT NOTES:
 * ✅ columnCount() MUST return 6 (số columns)
 * ✅ data() MUST handle both roles AND columns
 * ✅ setData() for editable cells
 * ✅ headerData() for column names
 * ✅ ID column is NOT editable (flags)
 * ✅ emit rowCountChanged() khi add/remove
 * 
 * COMPARISON với ListModel:
 * 
 * ListModel:
 * - 1D data (rows only)
 * - rowCount() only
 * - data() uses roles only
 * 
 * TableModel:
 * - 2D data (rows × columns)
 * - rowCount() + columnCount()
 * - data() uses roles OR columns
 * - headerData() for column names
 * 
 * ============================================
 */
