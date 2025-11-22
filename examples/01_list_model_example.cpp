/**
 * Ví dụ 1: QAbstractListModel - Model Danh Sách Đơn Giản
 * 
 * Mục đích: Hiểu cách tạo và sử dụng List Model
 */

#include <QAbstractListModel>
#include <QStringList>
#include <QVariant>

// Custom List Model - Lưu danh sách tên người
class PersonListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        AgeRole
    };

    PersonListModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {
        // Khởi tạo dữ liệu mẫu
        m_persons = {
            {"Nguyễn Văn A", 25},
            {"Trần Thị B", 30},
            {"Lê Văn C", 28}
        };
    }

    // BẮT BUỘC: Trả về số hàng (số item trong danh sách)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_persons.size();
    }

    // BẮT BUỘC: Trả về dữ liệu tại index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_persons.size())
            return QVariant();

        const auto &person = m_persons.at(index.row());

        switch (role) {
        case Qt::DisplayRole:        // Hiển thị mặc định
        case NameRole:
            return person.name;
        case AgeRole:
            return person.age;
        default:
            return QVariant();
        }
    }

    // Tùy chọn: Định nghĩa các role names (cho QML)
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[AgeRole] = "age";
        return roles;
    }

    // Thêm person mới
    void addPerson(const QString &name, int age)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_persons.append({name, age});
        endInsertRows();
    }

private:
    struct Person {
        QString name;
        int age;
    };
    QList<Person> m_persons;
};

// Sử dụng:
/*
#include <QListView>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PersonListModel model;
    QListView view;
    view.setModel(&model);
    view.show();

    return app.exec();
}
*/
