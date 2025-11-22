// Ví dụ QAbstractTableModel - Model cho bảng 2 chiều
#include <QAbstractTableModel>
#include <QList>
#include <QStringList>
#include <QDebug>

class TableModel : public QAbstractTableModel {
    Q_OBJECT
    
private:
    QList<QStringList> m_data;  // Mỗi QStringList là một hàng
    QStringList m_headers;      // Tên các cột
    
public:
    TableModel(QObject *parent = nullptr) 
        : QAbstractTableModel(parent) {
        m_headers << "Tên" << "Tuổi" << "Địa chỉ";
    }
    
    // 1. Số hàng
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_data.size();
    }
    
    // 2. Số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_headers.size();
    }
    
    // 3. Lấy dữ liệu tại (row, column)
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || 
            index.row() >= m_data.size() || 
            index.column() >= m_headers.size())
            return QVariant();
            
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_data.at(index.row()).at(index.column());
            
        return QVariant();
    }
    
    // 4. Header (tên cột/hàng)
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole)
            return QVariant();
            
        if (orientation == Qt::Horizontal)
            return m_headers.at(section);
        else
            return QString::number(section + 1);
    }
    
    // 5. Tạo index từ (row, column)
    QModelIndex index(int row, int column, 
                      const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        if (row < 0 || row >= m_data.size() || 
            column < 0 || column >= m_headers.size())
            return QModelIndex();
        return createIndex(row, column);
    }
    
    // Thêm hàng mới
    void addRow(const QStringList &rowData) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(rowData);
        endInsertRows();
    }
    
    // Sửa dữ liệu
    bool setData(const QModelIndex &index, const QVariant &value, 
                 int role = Qt::EditRole) override {
        if (!index.isValid() || role != Qt::EditRole)
            return false;
            
        m_data[index.row()][index.column()] = value.toString();
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
#include <QTableView>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TableModel *model = new TableModel();
    model->addRow(QStringList() << "Nguyễn Văn A" << "25" << "Hà Nội");
    model->addRow(QStringList() << "Trần Thị B" << "30" << "TP.HCM");
    model->addRow(QStringList() << "Lê Văn C" << "22" << "Đà Nẵng");
    
    QTableView *view = new QTableView();
    view->setModel(model);
    view->show();
    
    return app.exec();
}
*/

#include "table_model_example.moc"
