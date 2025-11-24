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

/**
 * ============================================
 * 📚 SUMMARY - FileSystemTreeModel Implementation
 * ============================================
 * 
 * MODEL TYPE: QAbstractItemModel (Tree/Hierarchical)
 * 
 * CORE CONCEPT: QMODELINDEX
 * - Temporary reference to tree nodes
 * - Contains: row, column, void* internalPointer
 * - internalPointer → TreeNode*
 * - Invalid index = root node
 * - MUST NOT store QModelIndex long-term!
 * 
 * KEY METHODS IMPLEMENTATION:
 * 
 * 1. index(row, column, parent):
 *    PURPOSE: Create QModelIndex for child node
 *    
 *    LOGIC:
 *    - Get parentNode from parent QModelIndex
 *    - Check if row is valid (< parentNode->children.count())
 *    - Get childNode = parentNode->children[row]
 *    - return createIndex(row, column, childNode)
 *    
 *    EXAMPLE:
 *    index(0, 0, documentsIndex) → Returns index for first child of Documents
 * 
 * 2. parent(child):
 *    PURPOSE: Get parent QModelIndex of child
 *    
 *    LOGIC:
 *    - Get childNode from child QModelIndex
 *    - Get parentNode = childNode->parentNode
 *    - If parentNode == rootNode → return invalid (root has no parent)
 *    - Get grandParentNode = parentNode->parentNode
 *    - Find parentNode's row in grandParentNode
 *    - return createIndex(row, 0, parentNode)
 *    
 *    EXAMPLE:
 *    parent(workIndex) → Returns documentsIndex
 *    parent(documentsIndex) → Returns invalid (root)
 * 
 * 3. rowCount(parent):
 *    PURPOSE: Number of children of parent
 *    
 *    LOGIC:
 *    - Get parentNode from parent QModelIndex
 *    - return parentNode->children.count()
 *    
 *    NOT total rows in tree!
 *    Each node returns only its direct children count
 *    
 *    EXAMPLE:
 *    rowCount(invalid) → 3 (Documents, Pictures, Music)
 *    rowCount(documentsIndex) → 2 (CV.pdf, Work)
 * 
 * 4. data(index, role):
 *    PURPOSE: Return node data for given role
 *    
 *    LOGIC:
 *    - Get node from index
 *    - switch (role):
 *        case NameRole: return node->name
 *        case TypeRole: return node->type
 *        case SizeRole: return node->size
 *        etc.
 * 
 * 5. roleNames():
 *    PURPOSE: Map Qt roles → QML property names
 *    
 *    MAPPING:
 *    NameRole → "name"
 *    TypeRole → "type"
 *    SizeRole → "size"
 *    etc.
 * 
 * DATA FLOW EXAMPLES:
 * 
 * 1. ADD FOLDER:
 *    QML: treeModel.addFolder("NewFolder", parentIndex)
 *      ↓
 *    C++: Get parentNode from parentIndex
 *      ↓
 *    C++: int row = parentNode->children.count()
 *      ↓
 *    C++: beginInsertRows(parentIndex, row, row)
 *      ↓
 *    C++: parentNode->addChild(newNode)
 *      ↓
 *    C++: endInsertRows()
 *      ↓
 *    C++: emit nodeCountChanged()
 *      ↓
 *    QML: View detects new node
 *      ↓
 *    QML: Creates delegate for new node
 *      ↓
 *    QML: Calls data() for node info
 *      ↓
 *    QML: Displays new folder
 * 
 * 2. REMOVE NODE:
 *    QML: treeModel.removeNode(nodeIndex)
 *      ↓
 *    C++: Get node from nodeIndex
 *      ↓
 *    C++: Get parentNode = node->parentNode
 *      ↓
 *    C++: int row = node->row()
 *      ↓
 *    C++: Get parentIndex = parent(nodeIndex)
 *      ↓
 *    C++: beginRemoveRows(parentIndex, row, row)
 *      ↓
 *    C++: parentNode->children.remove(row)
 *      ↓
 *    C++: delete node
 *      ↓
 *    C++: endRemoveRows()
 *      ↓
 *    QML: View removes delegate
 * 
 * 3. NAVIGATE TREE:
 *    QML wants to display Documents and its children
 *      ↓
 *    QML: rowCount(invalid) → 3
 *      ↓
 *    QML: For i in 0..2:
 *      ↓
 *    QML: index(i, 0, invalid) → documentsIndex, picturesIndex, musicIndex
 *      ↓
 *    QML: data(documentsIndex, NameRole) → "Documents"
 *      ↓
 *    QML: rowCount(documentsIndex) → 2
 *      ↓
 *    QML: For j in 0..1:
 *      ↓
 *    QML: index(j, 0, documentsIndex) → child indices
 *      ↓
 *    QML: Display children
 * 
 * HELPER METHODS:
 * 
 * ✅ getNode(index):
 *    - If index.isValid(): return static_cast<TreeNode*>(index.internalPointer())
 *    - Else: return rootNode
 *    - Convenient way to get TreeNode* from QModelIndex
 * 
 * ✅ countNodes(node):
 *    - RECURSIVE function
 *    - Count node + all descendants
 *    - Used for statistics
 * 
 * ✅ calculateSize(node):
 *    - RECURSIVE function
 *    - Sum size of node + all descendants
 *    - Used for statistics
 * 
 * ✅ depth(index):
 *    - Count parents until root
 *    - Used for indentation in QML
 * 
 * TREE STRUCTURE EXAMPLE:
 * 
 * Root (rootNode, invalid index)
 *  ├─ Documents (row=0, parent=invalid)
 *  │   ├─ CV.pdf (row=0, parent=documents)
 *  │   ├─ Report.docx (row=1, parent=documents)
 *  │   └─ Work (row=2, parent=documents)
 *  │       ├─ Project1.txt (row=0, parent=work)
 *  │       └─ Project2.txt (row=1, parent=work)
 *  ├─ Pictures (row=1, parent=invalid)
 *  │   ├─ Vacation (row=0, parent=pictures)
 *  │   │   ├─ photo1.jpg (row=0, parent=vacation)
 *  │   │   └─ photo2.jpg (row=1, parent=vacation)
 *  │   └─ selfie.jpg (row=1, parent=pictures)
 *  └─ Music (row=2, parent=invalid)
 *      ├─ song1.mp3 (row=0, parent=music)
 *      └─ song2.mp3 (row=1, parent=music)
 * 
 * INTERNAL POINTER USAGE:
 * 
 * Create index:
 * createIndex(row, column, pointer)
 * → Creates QModelIndex with pointer to TreeNode
 * 
 * Get pointer:
 * TreeNode* node = static_cast<TreeNode*>(index.internalPointer())
 * → Retrieves TreeNode* from QModelIndex
 * 
 * Why void*?
 * - Qt doesn't know about TreeNode
 * - Uses void* for flexibility
 * - We cast to TreeNode* when needed
 * 
 * PARENT-CHILD NAVIGATION:
 * 
 * Child → Parent:
 * 1. Get childNode from child QModelIndex
 * 2. Get parentNode = childNode->parentNode
 * 3. Create parent QModelIndex from parentNode
 * 
 * Parent → Child:
 * 1. Get parentNode from parent QModelIndex
 * 2. Get childNode = parentNode->children[row]
 * 3. Create child QModelIndex from childNode
 * 
 * RECURSIVE FUNCTIONS:
 * 
 * countNodes():
 * int countNodes(TreeNode *node) {
 *     if (!node) return 0;
 *     int count = 1;
 *     for (child : node->children) {
 *         count += countNodes(child);  // Recursion!
 *     }
 *     return count;
 * }
 * 
 * calculateSize():
 * qint64 calculateSize(TreeNode *node) {
 *     if (!node) return 0;
 *     qint64 total = node->size;
 *     for (child : node->children) {
 *         total += calculateSize(child);  // Recursion!
 *     }
 *     return total;
 * }
 * 
 * MEMORY MANAGEMENT:
 * 
 * ✅ Root node owns all descendants
 * ✅ TreeNode destructor is recursive:
 *    ~TreeNode() { qDeleteAll(children); }
 * ✅ Delete root → deletes entire tree
 * ✅ When removing node:
 *    - Remove from parent's children list
 *    - delete node (automatically deletes descendants)
 * 
 * IMPORTANT CONCEPTS:
 * 
 * 1. QMODELINDEX IS TEMPORARY:
 *    - Don't store in member variables
 *    - Don't store in containers
 *    - Model can recreate at any time
 *    - Use row/parent to recreate if needed
 * 
 * 2. INVALID INDEX = ROOT:
 *    - QModelIndex() is invalid
 *    - Represents root node
 *    - parent() returns invalid for top-level nodes
 * 
 * 3. ROW IS RELATIVE TO PARENT:
 *    - row=0 in Documents ≠ row=0 in Pictures
 *    - Each parent has own row numbering
 *    - Row is position in parent's children list
 * 
 * 4. BEGIN/END ROWS:
 *    - beginInsertRows(PARENT, first, last)
 *    - Insert children
 *    - endInsertRows()
 *    - PARENT is important! Not child index.
 * 
 * 5. INTERNAL POINTER LIFETIME:
 *    - Must be valid while QModelIndex exists
 *    - Don't delete nodes while views active
 *    - Use begin/endRemoveRows before delete
 * 
 * COMPARISON với List/Table:
 * 
 * LIST/TABLE:
 * - Flat structure
 * - Simple row index
 * - No parent() method needed
 * - rowCount() returns total
 * 
 * TREE:
 * - Hierarchical structure
 * - QModelIndex with parent relationship
 * - Must implement index() and parent()
 * - rowCount(parent) returns children count
 * - Each node can have children
 * - Recursive by nature
 * 
 * BEST PRACTICES:
 * ✅ Always check index.isValid()
 * ✅ Return invalid index for root's parent
 * ✅ Use internalPointer for fast access
 * ✅ Don't store QModelIndex long-term
 * ✅ Use begin/endInsertRows with correct parent
 * ✅ Delete nodes only after endRemoveRows
 * ✅ Implement recursive helpers for statistics
 * ✅ Test with multi-level trees (3+ levels)
 * 
 * DEBUGGING TIPS:
 * - Print index.row(), index.column(), index.internalPointer()
 * - Check parent() returns invalid for top-level
 * - Check rowCount(parent) matches children count
 * - Verify index() creates valid indices
 * - Test add/remove at different levels
 * 
 * ============================================
 */
