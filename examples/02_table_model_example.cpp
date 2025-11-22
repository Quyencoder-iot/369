/**
 * Ví dụ 2: QAbstractTableModel - Model Bảng
 * 
 * Mục đích: Hiểu cách tạo và sử dụng Table Model
 */

#include <QAbstractTableModel>
#include <QVariant>
#include <QColor>

// Custom Table Model - Bảng nhân viên
class EmployeeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    EmployeeTableModel(QObject *parent = nullptr)
        : QAbstractTableModel(parent)
    {
        // Khởi tạo dữ liệu mẫu
        m_employees = {
            {"NV001", "Nguyễn Văn A", "Phòng IT", 5000000},
            {"NV002", "Trần Thị B", "Phòng HR", 4500000},
            {"NV003", "Lê Văn C", "Phòng IT", 5500000}
        };
    }

    // BẮT BUỘC: Trả về số hàng
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_employees.size();
    }

    // BẮT BUỘC: Trả về số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return 4; // Mã NV, Tên, Phòng ban, Lương
    }

    // BẮT BUỘC: Trả về dữ liệu tại index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_employees.size())
            return QVariant();

        const auto &emp = m_employees.at(index.row());

        switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
            case 0: return emp.code;
            case 1: return emp.name;
            case 2: return emp.department;
            case 3: return QString::number(emp.salary);
            }
            break;

        case Qt::TextAlignmentRole:
            if (index.column() == 3) // Cột lương căn phải
                return Qt::AlignRight | Qt::AlignVCenter;
            return Qt::AlignLeft | Qt::AlignVCenter;

        case Qt::BackgroundRole:
            if (index.row() % 2 == 0)
                return QColor(240, 240, 240); // Màu nền xen kẽ
            break;
        }

        return QVariant();
    }

    // BẮT BUỘC: Trả về header (tên cột)
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0: return "Mã NV";
            case 1: return "Tên";
            case 2: return "Phòng ban";
            case 3: return "Lương";
            }
        }
        return QVariant();
    }

    // Tùy chọn: Cho phép chỉnh sửa
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    // Tùy chọn: Cập nhật dữ liệu khi edit
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        if (!index.isValid() || role != Qt::EditRole)
            return false;

        auto &emp = m_employees[index.row()];
        switch (index.column()) {
        case 1:
            emp.name = value.toString();
            break;
        case 2:
            emp.department = value.toString();
            break;
        case 3:
            emp.salary = value.toInt();
            break;
        default:
            return false;
        }

        emit dataChanged(index, index, {role});
        return true;
    }

    // Thêm nhân viên mới
    void addEmployee(const QString &code, const QString &name, 
                     const QString &dept, int salary)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_employees.append({code, name, dept, salary});
        endInsertRows();
    }

private:
    struct Employee {
        QString code;
        QString name;
        QString department;
        int salary;
    };
    QList<Employee> m_employees;
};

// Sử dụng:
/*
#include <QTableView>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    EmployeeTableModel model;
    QTableView view;
    view.setModel(&model);
    view.show();

    return app.exec();
}
*/
