/**
 * VÍ DỤ 2: QAbstractTableModel - Bảng học sinh
 * 
 * ĐẶC ĐIỂM:
 * - Dữ liệu 2 chiều (table)
 * - Cần implement rowCount(), columnCount(), data()
 * - headerData() cho tiêu đề cột
 */

#include <QAbstractTableModel>
#include <QTableView>
#include <QApplication>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QVector>

// ============================================
// BƯỚC 1: Định nghĩa Model
// ============================================
class StudentTableModel : public QAbstractTableModel {
    Q_OBJECT
    
    // Cấu trúc 1 hàng
    struct Student {
        int id;
        QString name;
        int age;
        QString grade;
        QString email;
    };
    
    QVector<Student> m_students; // Data storage
    QStringList m_headers;       // Column headers
    
public:
    // Constructor
    StudentTableModel(QObject *parent = nullptr) 
        : QAbstractTableModel(parent) {
        
        // Headers
        m_headers << "ID" << "Tên" << "Tuổi" << "Điểm" << "Email";
        
        // Data mẫu
        m_students = {
            {1, "Nguyễn Văn A", 20, "A", "a@example.com"},
            {2, "Trần Thị B", 19, "B+", "b@example.com"},
            {3, "Lê Văn C", 21, "A+", "c@example.com"},
            {4, "Phạm Thị D", 20, "C", "d@example.com"},
            {5, "Hoàng Văn E", 22, "B", "e@example.com"}
        };
    }
    
    // ----------------------------------------
    // BẮT BUỘC: Số hàng
    // ----------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid())
            return 0; // Table model không có children
        
        return m_students.count();
    }
    
    // ----------------------------------------
    // BẮT BUỘC: Số cột
    // ----------------------------------------
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid())
            return 0;
        
        return m_headers.count(); // 5 cột
    }
    
    // ----------------------------------------
    // BẮT BUỘC: Data cho mỗi cell
    // ----------------------------------------
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        // Kiểm tra index
        if (!index.isValid() 
            || index.row() >= m_students.size()
            || index.column() >= m_headers.size())
            return QVariant();
        
        const Student &student = m_students.at(index.row());
        
        // Role: Display / Edit
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
                case 0: return student.id;
                case 1: return student.name;
                case 2: return student.age;
                case 3: return student.grade;
                case 4: return student.email;
            }
        }
        
        // Role: Text Alignment
        if (role == Qt::TextAlignmentRole) {
            if (index.column() == 0 || index.column() == 2) {
                return Qt::AlignCenter; // ID và Age căn giữa
            }
            return Qt::AlignLeft;
        }
        
        // Role: Background Color
        if (role == Qt::BackgroundRole) {
            // Tô màu theo điểm
            if (index.column() == 3) {
                QString grade = student.grade;
                if (grade.startsWith("A"))
                    return QColor(200, 255, 200); // Xanh lá nhạt
                if (grade.startsWith("C") || grade.startsWith("D"))
                    return QColor(255, 200, 200); // Đỏ nhạt
            }
            
            // Màu xen kẽ cho mỗi hàng
            if (index.row() % 2 == 0)
                return QColor(245, 245, 245);
        }
        
        // Role: Font
        if (role == Qt::FontRole) {
            QFont font;
            if (index.column() == 0) { // ID column bold
                font.setBold(true);
            }
            return font;
        }
        
        return QVariant();
    }
    
    // ----------------------------------------
    // Headers cho cột/hàng
    // ----------------------------------------
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                // Column headers
                if (section < m_headers.size())
                    return m_headers.at(section);
            } else {
                // Row headers (số thứ tự)
                return QString::number(section + 1);
            }
        }
        
        // Bold headers
        if (role == Qt::FontRole) {
            QFont font;
            font.setBold(true);
            return font;
        }
        
        return QVariant();
    }
    
    // ----------------------------------------
    // Edit data
    // ----------------------------------------
    bool setData(const QModelIndex &index, const QVariant &value, 
                 int role = Qt::EditRole) override {
        if (!index.isValid() || index.row() >= m_students.size())
            return false;
        
        if (role == Qt::EditRole) {
            Student &student = m_students[index.row()];
            
            switch (index.column()) {
                case 1: student.name = value.toString(); break;
                case 2: student.age = value.toInt(); break;
                case 3: student.grade = value.toString(); break;
                case 4: student.email = value.toString(); break;
                default: return false;
            }
            
            emit dataChanged(index, index, {role});
            return true;
        }
        
        return false;
    }
    
    // ----------------------------------------
    // Flags
    // ----------------------------------------
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        
        Qt::ItemFlags flags = QAbstractTableModel::flags(index);
        
        // ID column không edit được
        if (index.column() != 0) {
            flags |= Qt::ItemIsEditable;
        }
        
        return flags;
    }
    
    // ----------------------------------------
    // Thêm student
    // ----------------------------------------
    void addStudent(const QString &name, int age, const QString &grade, 
                    const QString &email) {
        int row = m_students.count();
        int newId = row > 0 ? m_students.last().id + 1 : 1;
        
        beginInsertRows(QModelIndex(), row, row);
        m_students.append({newId, name, age, grade, email});
        endInsertRows();
    }
    
    // ----------------------------------------
    // Xóa student
    // ----------------------------------------
    bool removeStudent(int row) {
        if (row < 0 || row >= m_students.count())
            return false;
        
        beginRemoveRows(QModelIndex(), row, row);
        m_students.remove(row);
        endRemoveRows();
        
        return true;
    }
    
    // ----------------------------------------
    // Sort
    // ----------------------------------------
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override {
        emit layoutAboutToBeChanged();
        
        std::sort(m_students.begin(), m_students.end(), 
                 [column, order](const Student &a, const Student &b) {
            bool less = false;
            switch (column) {
                case 0: less = a.id < b.id; break;
                case 1: less = a.name < b.name; break;
                case 2: less = a.age < b.age; break;
                case 3: less = a.grade < b.grade; break;
                case 4: less = a.email < b.email; break;
            }
            return order == Qt::AscendingOrder ? less : !less;
        });
        
        emit layoutChanged();
    }
};

// ============================================
// BƯỚC 2: Sử dụng Model
// ============================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Tạo model
    StudentTableModel *model = new StudentTableModel;
    
    // Tạo view
    QTableView *view = new QTableView;
    view->setModel(model);
    
    // Tùy chỉnh view
    view->setSortingEnabled(true);           // Cho phép sort
    view->setAlternatingRowColors(true);     // Màu xen kẽ
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Resize columns
    view->horizontalHeader()->setStretchLastSection(true);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // Auto-resize
    view->resizeColumnsToContents();
    
    // Window
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // Buttons
    QPushButton *addBtn = new QPushButton("Thêm sinh viên");
    QPushButton *removeBtn = new QPushButton("Xóa sinh viên được chọn");
    
    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        model->addStudent("Sinh viên mới", 20, "B", "new@example.com");
    });
    
    QObject::connect(removeBtn, &QPushButton::clicked, [=]() {
        QModelIndex current = view->currentIndex();
        if (current.isValid()) {
            model->removeStudent(current.row());
        }
    });
    
    layout->addWidget(view);
    layout->addWidget(addBtn);
    layout->addWidget(removeBtn);
    
    window.resize(800, 400);
    window.show();
    
    return app.exec();
}

#include "02_table_model.moc"

// ============================================
// CÁCH ĐỌC CODE TABLE MODEL:
// ============================================
// 1. Data storage: m_students (QVector<Student>)
// 2. rowCount(): số hàng
// 3. columnCount(): số cột
// 4. data(index, role): 
//    - index.row() → hàng nào
//    - index.column() → cột nào
//    - role → muốn data gì (Display, Color, Font, etc.)
// 5. headerData(): Tiêu đề cột/hàng
// 6. setData(): Update cell → emit dataChanged()
