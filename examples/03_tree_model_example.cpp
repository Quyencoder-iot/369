/**
 * Ví dụ 3: QAbstractItemModel - Model Cây (Tree)
 * 
 * Mục đích: Hiểu cách tạo và sử dụng Tree Model
 */

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

// Tree Node - Nút trong cây
struct TreeNode {
    QString name;
    QString type; // "folder" hoặc "file"
    QList<TreeNode*> children;
    TreeNode *parent = nullptr;

    ~TreeNode() {
        qDeleteAll(children);
    }
};

// Custom Tree Model - Cấu trúc file/folder
class FileTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FileTreeModel(QObject *parent = nullptr)
        : QAbstractItemModel(parent)
    {
        // Tạo cấu trúc cây mẫu
        rootNode = new TreeNode{"Root", "folder", {}, nullptr};

        TreeNode *folder1 = new TreeNode{"Documents", "folder", {}, rootNode};
        TreeNode *file1 = new TreeNode{"readme.txt", "file", {}, folder1};
        TreeNode *file2 = new TreeNode{"notes.txt", "file", {}, folder1};
        folder1->children = {file1, file2};

        TreeNode *folder2 = new TreeNode{"Pictures", "folder", {}, rootNode};
        TreeNode *file3 = new TreeNode{"photo.jpg", "file", {}, folder2};
        folder2->children = {file3};

        rootNode->children = {folder1, folder2};
    }

    ~FileTreeModel() {
        delete rootNode;
    }

    // BẮT BUỘC: Trả về số hàng (số con của parent)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        TreeNode *parentNode = nodeFromIndex(parent);
        return parentNode ? parentNode->children.size() : 0;
    }

    // BẮT BUỘC: Trả về số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return 2; // Tên và Loại
    }

    // BẮT BUỘC: Trả về dữ liệu tại index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid())
            return QVariant();

        TreeNode *node = nodeFromIndex(index);
        if (!node)
            return QVariant();

        switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
            case 0: return node->name;
            case 1: return node->type;
            }
            break;
        }

        return QVariant();
    }

    // BẮT BUỘC: Trả về header
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0: return "Tên";
            case 1: return "Loại";
            }
        }
        return QVariant();
    }

    // QUAN TRỌNG: Tạo index từ parent, row, column
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        TreeNode *parentNode = nodeFromIndex(parent);
        if (!parentNode || row >= parentNode->children.size())
            return QModelIndex();

        TreeNode *childNode = parentNode->children.at(row);
        return createIndex(row, column, childNode);
    }

    // QUAN TRỌNG: Trả về parent của index
    QModelIndex parent(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return QModelIndex();

        TreeNode *node = nodeFromIndex(index);
        if (!node || !node->parent || node->parent == rootNode)
            return QModelIndex();

        TreeNode *parentNode = node->parent;
        TreeNode *grandParent = parentNode->parent;
        if (!grandParent)
            return QModelIndex();

        // Tìm vị trí của parent trong grandParent
        int row = grandParent->children.indexOf(parentNode);
        return createIndex(row, 0, parentNode);
    }

private:
    TreeNode *rootNode;

    // Helper: Lấy TreeNode từ QModelIndex
    TreeNode *nodeFromIndex(const QModelIndex &index) const
    {
        if (!index.isValid())
            return rootNode;
        return static_cast<TreeNode*>(index.internalPointer());
    }
};

// Sử dụng:
/*
#include <QTreeView>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FileTreeModel model;
    QTreeView view;
    view.setModel(&model);
    view.expandAll(); // Mở rộng tất cả các nút
    view.show();

    return app.exec();
}
*/
