#include <QAbstractItemModel>
#include <QDebug>
#include <QList>

// 1. Cấu trúc dữ liệu gốc (Node)
// Đây là cái mà "internalPointer" sẽ trỏ tới
class TreeItem {
public:
    explicit TreeItem(const QString &data, TreeItem *parent = nullptr)
        : m_itemData(data), m_parentItem(parent) {}

    ~TreeItem() { qDeleteAll(m_childItems); }

    void appendChild(TreeItem *child) { m_childItems.append(child); }
    TreeItem *child(int row) { return m_childItems.value(row); }
    int childCount() const { return m_childItems.count(); }
    int row() const {
        if (m_parentItem)
            return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
        return 0;
    }
    TreeItem *parentItem() { return m_parentItem; }
    QString data() const { return m_itemData; }

private:
    QList<TreeItem*> m_childItems;
    QString m_itemData;
    TreeItem *m_parentItem;
};

// 2. Model - Cầu nối giữa View và TreeItem
class SimpleTreeModel : public QAbstractItemModel {
public:
    explicit SimpleTreeModel(TreeItem* root, QObject *parent = nullptr)
        : QAbstractItemModel(parent), rootItem(root) {}

    // QModelIndex: View hỏi "Cho tôi index của dòng row, cột col, thuộc về cha parent"
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        TreeItem *parentItem;

        // Giải mã index cha để tìm pointer thực
        if (!parent.isValid())
            parentItem = rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        // Tìm con tương ứng
        TreeItem *childItem = parentItem->child(row);
        if (childItem)
            // TẠO INDEX: Nhét con trỏ thực (childItem) vào bên trong index
            return createIndex(row, column, childItem);
        
        return QModelIndex();
    }

    // Parent: View hỏi "Ai là cha của index này?"
    QModelIndex parent(const QModelIndex &index) const override {
        if (!index.isValid())
            return QModelIndex();

        // Lấy con trỏ thực từ index con
        TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
        TreeItem *parentItem = childItem->parentItem();

        if (parentItem == rootItem)
            return QModelIndex();

        // Tạo index cho cha
        return createIndex(parentItem->row(), 0, parentItem);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        TreeItem *parentItem;
        if (parent.column() > 0) return 0;

        if (!parent.isValid())
            parentItem = rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        return parentItem->childCount();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1;
    }

    // Data: View hỏi "Dữ liệu hiển thị cho index này là gì?"
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || role != Qt::DisplayRole)
            return QVariant();

        // Lấy con trỏ thực để truy cập dữ liệu
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data();
    }

private:
    TreeItem *rootItem;
};

// Cách sử dụng (Minh họa logic)
int main(int argc, char *argv[]) {
    // Setup dữ liệu
    TreeItem *root = new TreeItem("Root");
    TreeItem *child1 = new TreeItem("Child 1", root);
    TreeItem *child2 = new TreeItem("Child 2", root);
    root->appendChild(child1);
    root->appendChild(child2);
    
    // Setup Model
    SimpleTreeModel model(root);
    
    // Giả lập View gọi hàm
    QModelIndex idxChild1 = model.index(0, 0, QModelIndex()); // Lấy index Child 1
    
    // Kiểm tra pointer bên trong
    if(idxChild1.internalPointer() == child1) {
        qDebug() << "Hiểu bài: Index đang chứa con trỏ Child 1!";
    }
    
    QString data = model.data(idxChild1, Qt::DisplayRole).toString();
    qDebug() << "Dữ liệu:" << data;

    delete root;
    return 0;
}
