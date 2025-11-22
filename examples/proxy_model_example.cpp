// Ví dụ Proxy Model - Lọc và sắp xếp dữ liệu từ model khác
#include <QAbstractProxyModel>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QDebug>

// Custom Proxy Model - Đảo ngược thứ tự hàng
class ReverseProxyModel : public QAbstractProxyModel {
    Q_OBJECT
    
public:
    ReverseProxyModel(QObject *parent = nullptr) 
        : QAbstractProxyModel(parent) {}
    
    // Kết nối với source model
    void setSourceModel(QAbstractItemModel *sourceModel) override {
        beginResetModel();
        
        if (m_sourceModel)
            disconnect(m_sourceModel, nullptr, this, nullptr);
            
        m_sourceModel = sourceModel;
        
        if (m_sourceModel) {
            // Kết nối signals để cập nhật khi source thay đổi
            connect(m_sourceModel, &QAbstractItemModel::dataChanged,
                    this, &ReverseProxyModel::onSourceDataChanged);
            connect(m_sourceModel, &QAbstractItemModel::rowsInserted,
                    this, &ReverseProxyModel::onSourceRowsInserted);
            connect(m_sourceModel, &QAbstractItemModel::rowsRemoved,
                    this, &ReverseProxyModel::onSourceRowsRemoved);
        }
        
        endResetModel();
    }
    
    // Chuyển đổi index từ proxy → source (đảo ngược hàng)
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override {
        if (!m_sourceModel || !proxyIndex.isValid())
            return QModelIndex();
            
        int sourceRow = m_sourceModel->rowCount() - 1 - proxyIndex.row();
        return m_sourceModel->index(sourceRow, proxyIndex.column());
    }
    
    // Chuyển đổi index từ source → proxy (đảo ngược lại)
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override {
        if (!m_sourceModel || !sourceIndex.isValid())
            return QModelIndex();
            
        int proxyRow = m_sourceModel->rowCount() - 1 - sourceIndex.row();
        return createIndex(proxyRow, sourceIndex.column());
    }
    
    // Delegate các phương thức cho sourceModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_sourceModel ? m_sourceModel->rowCount(mapToSource(parent)) : 0;
    }
    
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_sourceModel ? m_sourceModel->columnCount(mapToSource(parent)) : 0;
    }
    
    QVariant data(const QModelIndex &index, int role) const override {
        return m_sourceModel ? m_sourceModel->data(mapToSource(index), role) : QVariant();
    }
    
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override {
        return m_sourceModel ? m_sourceModel->headerData(section, orientation, role) : QVariant();
    }
    
    QModelIndex index(int row, int column, 
                      const QModelIndex &parent = QModelIndex()) const override {
        return createIndex(row, column);
    }
    
    QModelIndex parent(const QModelIndex &child) const override {
        Q_UNUSED(child);
        return QModelIndex();
    }
    
private slots:
    void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
    }
    
    void onSourceRowsInserted(const QModelIndex &parent, int first, int last) {
        Q_UNUSED(parent);
        beginInsertRows(QModelIndex(), 
                       m_sourceModel->rowCount() - 1 - last,
                       m_sourceModel->rowCount() - 1 - first);
        endInsertRows();
    }
    
    void onSourceRowsRemoved(const QModelIndex &parent, int first, int last) {
        Q_UNUSED(parent);
        beginRemoveRows(QModelIndex(),
                       m_sourceModel->rowCount() - first,
                       m_sourceModel->rowCount() - last);
        endRemoveRows();
    }
    
private:
    QAbstractItemModel *m_sourceModel = nullptr;
};

// Cách sử dụng với QSortFilterProxyModel (built-in):
/*
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 1. Tạo source model
    QStandardItemModel *sourceModel = new QStandardItemModel();
    sourceModel->setHorizontalHeaderLabels(QStringList() << "Tên" << "Tuổi");
    sourceModel->appendRow(QList<QStandardItem*>() 
        << new QStandardItem("Alice") << new QStandardItem("25"));
    sourceModel->appendRow(QList<QStandardItem*>() 
        << new QStandardItem("Bob") << new QStandardItem("30"));
    sourceModel->appendRow(QList<QStandardItem*>() 
        << new QStandardItem("Charlie") << new QStandardItem("20"));
    
    // 2. Tạo proxy model để sắp xếp
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(sourceModel);
    proxyModel->sort(1, Qt::AscendingOrder);  // Sắp xếp theo cột 1 (Tuổi)
    
    // 3. Tạo view
    QTableView *view = new QTableView();
    view->setModel(proxyModel);
    view->show();
    
    return app.exec();
}
*/

#include "proxy_model_example.moc"
