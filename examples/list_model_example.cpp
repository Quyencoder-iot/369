// Ví dụ QAbstractListModel - Model cho danh sách đơn giản
#include <QAbstractListModel>
#include <QStringList>
#include <QDebug>

class StringListModel : public QAbstractListModel {
    Q_OBJECT
    
private:
    QStringList m_data;  // Nguồn dữ liệu thực tế
    
public:
    StringListModel(QObject *parent = nullptr) 
        : QAbstractListModel(parent) {}
    
    // 1. Số lượng items trong list
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_data.size();
    }
    
    // 2. Lấy dữ liệu tại vị trí index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_data.size())
            return QVariant();
            
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_data.at(index.row());
            
        return QVariant();
    }
    
    // 3. Tạo QModelIndex từ row
    QModelIndex index(int row, int column = 0, 
                      const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        Q_UNUSED(column);
        if (row < 0 || row >= m_data.size())
            return QModelIndex();
        return createIndex(row, 0);
    }
    
    // Thêm item vào cuối danh sách
    void addString(const QString &str) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(str);
        endInsertRows();
    }
    
    // Xóa item tại vị trí row
    void removeRow(int row) {
        if (row < 0 || row >= m_data.size())
            return;
        beginRemoveRows(QModelIndex(), row, row);
        m_data.removeAt(row);
        endRemoveRows();
    }
    
    // Sửa item tại vị trí row
    bool setData(const QModelIndex &index, const QVariant &value, 
                 int role = Qt::EditRole) override {
        if (!index.isValid() || role != Qt::EditRole)
            return false;
            
        m_data[index.row()] = value.toString();
        emit dataChanged(index, index);
        return true;
    }
    
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
};

// Cách sử dụng:
/*
#include <QListView>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    StringListModel *model = new StringListModel();
    model->addString("Item 1");
    model->addString("Item 2");
    model->addString("Item 3");
    
    QListView *view = new QListView();
    view->setModel(model);
    view->show();
    
    return app.exec();
}
*/

#include "list_model_example.moc"
