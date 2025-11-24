#ifndef FILESYSTEMTREEMODEL_H
#define FILESYSTEMTREEMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QIcon>

/**
 * Example 3: QAbstractItemModel - File System Tree
 * 
 * TREE MODEL:
 * - Hierarchical data (parent-child)
 * - Cần implement: index(), parent(), rowCount(), columnCount(), data()
 * - Phức tạp nhất nhưng linh hoạt nhất
 */

class TreeNode
{
public:
    enum NodeType {
        FolderType,
        FileType
    };
    
    QString name;
    NodeType type;
    qint64 size; // File size in bytes
    TreeNode *parentNode;
    QVector<TreeNode*> children;
    
    TreeNode(const QString &name, NodeType type = FolderType, TreeNode *parent = nullptr)
        : name(name), type(type), size(0), parentNode(parent) {}
    
    ~TreeNode() {
        qDeleteAll(children);
    }
    
    void addChild(TreeNode *child) {
        child->parentNode = this;
        children.append(child);
    }
    
    int row() const {
        if (parentNode)
            return parentNode->children.indexOf(const_cast<TreeNode*>(this));
        return 0;
    }
};

class FileSystemTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int nodeCount READ nodeCount NOTIFY nodeCountChanged)
    
public:
    enum TreeRoles {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        SizeRole,
        IsFileRole,
        IsFolderRole,
        DepthRole
    };
    Q_ENUM(TreeRoles)
    
    explicit FileSystemTreeModel(QObject *parent = nullptr);
    ~FileSystemTreeModel();
    
    // QAbstractItemModel interface - BẮT BUỘC cho Tree
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Methods for QML
    Q_INVOKABLE void addFolder(const QString &name, const QModelIndex &parent = QModelIndex());
    Q_INVOKABLE void addFile(const QString &name, qint64 size, const QModelIndex &parent = QModelIndex());
    Q_INVOKABLE void removeNode(const QModelIndex &index);
    Q_INVOKABLE QString getNodeName(const QModelIndex &index) const;
    Q_INVOKABLE bool isFolder(const QModelIndex &index) const;
    Q_INVOKABLE int depth(const QModelIndex &index) const;
    Q_INVOKABLE int nodeCount() const;
    Q_INVOKABLE qint64 totalSize() const;
    
signals:
    void nodeCountChanged();
    
private:
    TreeNode *rootNode;
    
    TreeNode *getNode(const QModelIndex &index) const;
    int countNodes(TreeNode *node) const;
    qint64 calculateSize(TreeNode *node) const;
    void createSampleData();
};

#endif // FILESYSTEMTREEMODEL_H
