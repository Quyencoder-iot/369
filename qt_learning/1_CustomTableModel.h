#include <QAbstractTableModel>
#include <vector>

// Dữ liệu thực tế (struct)
struct Person {
    QString name;
    int age;
};

// 1. Custom Model: Tự quản lý dữ liệu trong RAM (vector, list, array...)
// Kế thừa từ QAbstractTableModel (cho dạng bảng) hoặc QAbstractListModel (cho dạng danh sách 1 cột)
class PersonModel : public QAbstractTableModel {
    Q_OBJECT
public:
    PersonModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    // INPUT: Dữ liệu đầu vào
    void setPersons(const std::vector<Person> &persons) {
        beginResetModel(); // Báo cho View biết dữ liệu sắp thay đổi toàn bộ
        m_data = persons;
        endResetModel();
    }

    // READING TIP 1: Tìm hàm rowCount/columnCount để biết kích thước bảng
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return static_cast<int>(m_data.size());
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 2; // Cột 0: Name, Cột 1: Age
    }

    // READING TIP 2: Đây là hàm QUAN TRỌNG NHẤT. View gọi hàm này để lấy dữ liệu hiển thị.
    // index: vị trí (hàng, cột)
    // role: kiểu dữ liệu cần lấy (DisplayRole = text hiển thị, DecorationRole = icon, v.v.)
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_data.size())
            return QVariant();

        const Person &person = m_data[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case 0: return person.name;
                case 1: return person.age;
            }
        }
        return QVariant();
    }

    // READING TIP 3: headerData định nghĩa tên cột
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            return (section == 0) ? "Name" : "Age";
        }
        return QVariant();
    }

private:
    std::vector<Person> m_data; // Dữ liệu nằm trong RAM
};
