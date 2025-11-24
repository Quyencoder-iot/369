#include "FileSystemTreeModel.h"

FileSystemTreeModel::FileSystemTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootNode = new TreeNode("Root", TreeNode::FolderType);
    createSampleData();
}

FileSystemTreeModel::~FileSystemTreeModel()
{
    delete rootNode;
}

void FileSystemTreeModel::createSampleData()
{
    // Documents folder
    TreeNode *documents = new TreeNode("Documents", TreeNode::FolderType);
    rootNode->addChild(documents);
    
    documents->addChild(new TreeNode("CV.pdf", TreeNode::FileType, documents));
    documents->children.last()->size = 245000;
    
    documents->addChild(new TreeNode("Report.docx", TreeNode::FileType, documents));
    documents->children.last()->size = 1240000;
    
    TreeNode *work = new TreeNode("Work", TreeNode::FolderType, documents);
    documents->addChild(work);
    
    work->addChild(new TreeNode("Project1.txt", TreeNode::FileType, work));
    work->children.last()->size = 52000;
    
    work->addChild(new TreeNode("Project2.txt", TreeNode::FileType, work));
    work->children.last()->size = 48000;
    
    // Pictures folder
    TreeNode *pictures = new TreeNode("Pictures", TreeNode::FolderType);
    rootNode->addChild(pictures);
    
    TreeNode *vacation = new TreeNode("Vacation", TreeNode::FolderType, pictures);
    pictures->addChild(vacation);
    
    vacation->addChild(new TreeNode("photo1.jpg", TreeNode::FileType, vacation));
    vacation->children.last()->size = 2500000;
    
    vacation->addChild(new TreeNode("photo2.jpg", TreeNode::FileType, vacation));
    vacation->children.last()->size = 3200000;
    
    pictures->addChild(new TreeNode("selfie.jpg", TreeNode::FileType, pictures));
    pictures->children.last()->size = 1800000;
    
    // Music folder
    TreeNode *music = new TreeNode("Music", TreeNode::FolderType);
    rootNode->addChild(music);
    
    music->addChild(new TreeNode("song1.mp3", TreeNode::FileType, music));
    music->children.last()->size = 4500000;
    
    music->addChild(new TreeNode("song2.mp3", TreeNode::FileType, music));
    music->children.last()->size = 5200000;
}

QModelIndex FileSystemTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    
    TreeNode *parentNode = getNode(parent);
    
    if (row >= 0 && row < parentNode->children.count()) {
        TreeNode *childNode = parentNode->children.at(row);
        return createIndex(row, column, childNode);
    }
    
    return QModelIndex();
}

QModelIndex FileSystemTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();
    
    TreeNode *childNode = getNode(child);
    TreeNode *parentNode = childNode->parentNode;
    
    if (parentNode == rootNode || parentNode == nullptr)
        return QModelIndex();
    
    return createIndex(parentNode->row(), 0, parentNode);
}

int FileSystemTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeNode *parentNode = getNode(parent);
    return parentNode ? parentNode->children.count() : 0;
}

int FileSystemTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2; // Name, Size
}

QVariant FileSystemTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    TreeNode *node = getNode(index);
    
    if (role >= Qt::UserRole) {
        switch (role) {
            case NameRole: return node->name;
            case TypeRole: return node->type == TreeNode::FolderType ? "folder" : "file";
            case SizeRole: return node->size;
            case IsFileRole: return node->type == TreeNode::FileType;
            case IsFolderRole: return node->type == TreeNode::FolderType;
            case DepthRole: return depth(index);
        }
    }
    
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return node->name;
        else if (index.column() == 1 && node->type == TreeNode::FileType)
            return QString::number(node->size / 1024) + " KB";
    }
    
    return QVariant();
}

QVariant FileSystemTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return section == 0 ? "Name" : "Size";
    }
    return QVariant();
}

Qt::ItemFlags FileSystemTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return QAbstractItemModel::flags(index);
}

QHash<int, QByteArray> FileSystemTreeModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {TypeRole, "type"},
        {SizeRole, "size"},
        {IsFileRole, "isFile"},
        {IsFolderRole, "isFolder"},
        {DepthRole, "depth"}
    };
}

void FileSystemTreeModel::addFolder(const QString &name, const QModelIndex &parent)
{
    TreeNode *parentNode = getNode(parent);
    
    int row = parentNode->children.count();
    beginInsertRows(parent, row, row);
    
    TreeNode *newNode = new TreeNode(name, TreeNode::FolderType);
    parentNode->addChild(newNode);
    
    endInsertRows();
    emit nodeCountChanged();
}

void FileSystemTreeModel::addFile(const QString &name, qint64 size, const QModelIndex &parent)
{
    TreeNode *parentNode = getNode(parent);
    
    int row = parentNode->children.count();
    beginInsertRows(parent, row, row);
    
    TreeNode *newNode = new TreeNode(name, TreeNode::FileType);
    newNode->size = size;
    parentNode->addChild(newNode);
    
    endInsertRows();
    emit nodeCountChanged();
}

void FileSystemTreeModel::removeNode(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    
    TreeNode *node = getNode(index);
    TreeNode *parentNode = node->parentNode;
    
    if (!parentNode)
        return;
    
    int row = node->row();
    QModelIndex parentIndex = parent(index);
    
    beginRemoveRows(parentIndex, row, row);
    parentNode->children.remove(row);
    delete node;
    endRemoveRows();
    emit nodeCountChanged();
}

QString FileSystemTreeModel::getNodeName(const QModelIndex &index) const
{
    TreeNode *node = getNode(index);
    return node ? node->name : QString();
}

bool FileSystemTreeModel::isFolder(const QModelIndex &index) const
{
    TreeNode *node = getNode(index);
    return node && node->type == TreeNode::FolderType;
}

int FileSystemTreeModel::depth(const QModelIndex &index) const
{
    int d = 0;
    QModelIndex current = index;
    while (current.isValid()) {
        d++;
        current = parent(current);
    }
    return d;
}

int FileSystemTreeModel::nodeCount() const
{
    return countNodes(rootNode);
}

qint64 FileSystemTreeModel::totalSize() const
{
    return calculateSize(rootNode);
}

TreeNode *FileSystemTreeModel::getNode(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        if (node) return node;
    }
    return rootNode;
}

int FileSystemTreeModel::countNodes(TreeNode *node) const
{
    if (!node) return 0;
    
    int count = 1;
    for (TreeNode *child : node->children) {
        count += countNodes(child);
    }
    return count - 1; // Exclude root
}

qint64 FileSystemTreeModel::calculateSize(TreeNode *node) const
{
    if (!node) return 0;
    
    qint64 total = node->size;
    for (TreeNode *child : node->children) {
        total += calculateSize(child);
    }
    return total;
}
