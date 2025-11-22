// Ví dụ Tree Model - Model cho cấu trúc cây phân cấp
#include <QAbstractItemModel>
#include <QDebug>

// Node trong tree
struct TreeNode {
    QString name;
    QList<TreeNode*> children;
    TreeNode *parent;
    
    TreeNode(const QString &n, TreeNode *p = nullptr) 
        : name(n), parent(p) {}
    
    ~TreeNode() {
        qDeleteAll(children);
    }
};

class TreeModel : public QAbstractItemModel {
    Q_OBJECT
    
private:
    TreeNode *m_root;  // Root node
    
public:
    TreeModel(QObject *parent = nullptr) 
        : QAbstractItemModel(parent) {
        m_root = new TreeNode("Root");
        
        // Tạo cấu trúc mẫu
        TreeNode *node1 = new TreeNode("Node 1", m_root);
        TreeNode *node2 = new TreeNode("Node 2", m_root);
        m_root->children << node1 << node2;
        
        TreeNode *node1_1 = new TreeNode("Node 1.1", node1);
        TreeNode *node1_2 = new TreeNode("Node 1.2", node1);
        node1->children << node1_1 << node1_2;
    }
    
    ~TreeModel() {
        delete m_root;
    }
    
    // 1. Số hàng (children của parent)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        TreeNode *parentNode = nodeFromIndex(parent);
        return parentNode ? parentNode->children.size() : 0;
    }
    
    // 2. Số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 1;
    }
    
    // 3. Lấy dữ liệu
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();
            
        TreeNode *node = nodeFromIndex(index);
        if (!node)
            return QVariant();
            
        if (role == Qt::DisplayRole)
            return node->name;
            
        return QVariant();
    }
    
    // 4. Tạo index từ (row, column, parent)
    QModelIndex index(int row, int column, 
                      const QModelIndex &parent = QModelIndex()) const override {
        TreeNode *parentNode = nodeFromIndex(parent);
        if (!parentNode || row < 0 || row >= parentNode->children.size())
            return QModelIndex();
            
        TreeNode *childNode = parentNode->children.at(row);
        return createIndex(row, column, childNode);  // Lưu con trỏ node vào index
    }
    
    // 5. Lấy parent index
    QModelIndex parent(const QModelIndex &index) const override {
        TreeNode *node = nodeFromIndex(index);
        if (!node || node == m_root || !node->parent)
            return QModelIndex();
            
        TreeNode *parentNode = node->parent;
        TreeNode *grandParent = parentNode->parent;
        
        // Tìm vị trí của parentNode trong children của grandParent
        int row = 0;
        if (grandParent) {
            row = grandParent->children.indexOf(parentNode);
        }
        
        return createIndex(row, 0, parentNode);
    }
    
    // Header
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
            return "Tên";
        return QVariant();
    }
    
private:
    // Helper: Lấy node từ index
    TreeNode* nodeFromIndex(const QModelIndex &index) const {
        if (!index.isValid())
            return m_root;
        return static_cast<TreeNode*>(index.internalPointer());
    }
};

// Cách sử dụng:
/*
#include <QTreeView>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TreeModel *model = new TreeModel();
    
    QTreeView *view = new QTreeView();
    view->setModel(model);
    view->expandAll();
    view->show();
    
    return app.exec();
}
*/

#include "tree_model_example.moc"
