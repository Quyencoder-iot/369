/**
 * VÍ DỤ 3: QAbstractItemModel - Cây File System
 * 
 * ĐẶC ĐIỂM:
 * - Dữ liệu phân cấp (tree)
 * - CẦN PHẢI implement: index() và parent()
 * - Phức tạp nhất trong các models
 */

#include <QAbstractItemModel>
#include <QTreeView>
#include <QApplication>
#include <QVector>

// ============================================
// BƯỚC 1: Cấu trúc Node cho Tree
// ============================================
class TreeNode {
public:
    QString name;
    QString type; // "folder" hoặc "file"
    TreeNode *parentNode;
    QVector<TreeNode*> children;
    
    TreeNode(const QString &name, const QString &type, TreeNode *parent = nullptr)
        : name(name), type(type), parentNode(parent) {}
    
    ~TreeNode() {
        qDeleteAll(children);
    }
    
    void addChild(TreeNode *child) {
        child->parentNode = this;
        children.append(child);
    }
    
    // Tìm row của node này trong parent
    int row() const {
        if (parentNode)
            return parentNode->children.indexOf(const_cast<TreeNode*>(this));
        return 0;
    }
};

// ============================================
// BƯỚC 2: Tree Model
// ============================================
class FileSystemModel : public QAbstractItemModel {
    Q_OBJECT
    
    TreeNode *rootNode;
    
public:
    FileSystemModel(QObject *parent = nullptr) 
        : QAbstractItemModel(parent) {
        
        // Tạo cấu trúc cây mẫu
        rootNode = new TreeNode("Root", "folder");
        
        // Documents/
        TreeNode *documents = new TreeNode("Documents", "folder");
        rootNode->addChild(documents);
        
        documents->addChild(new TreeNode("CV.pdf", "file"));
        documents->addChild(new TreeNode("Report.docx", "file"));
        
        TreeNode *work = new TreeNode("Work", "folder");
        documents->addChild(work);
        work->addChild(new TreeNode("Project1.txt", "file"));
        work->addChild(new TreeNode("Project2.txt", "file"));
        
        // Pictures/
        TreeNode *pictures = new TreeNode("Pictures", "folder");
        rootNode->addChild(pictures);
        
        pictures->addChild(new TreeNode("Vacation", "folder"));
        pictures->addChild(new TreeNode("photo1.jpg", "file"));
        pictures->addChild(new TreeNode("photo2.jpg", "file"));
        
        // Music/
        TreeNode *music = new TreeNode("Music", "folder");
        rootNode->addChild(music);
        music->addChild(new TreeNode("song1.mp3", "file"));
        music->addChild(new TreeNode("song2.mp3", "file"));
    }
    
    ~FileSystemModel() {
        delete rootNode;
    }
    
    // ----------------------------------------
    // QUAN TRỌNG: Tạo QModelIndex cho item
    // ----------------------------------------
    QModelIndex index(int row, int column, 
                     const QModelIndex &parent = QModelIndex()) const override {
        
        // Kiểm tra hợp lệ
        if (!hasIndex(row, column, parent))
            return QModelIndex();
        
        // Lấy parent node
        TreeNode *parentNode;
        if (!parent.isValid())
            parentNode = rootNode; // Root
        else
            parentNode = static_cast<TreeNode*>(parent.internalPointer());
        
        // Lấy child node
        if (row >= 0 && row < parentNode->children.count()) {
            TreeNode *childNode = parentNode->children.at(row);
            // Tạo index với internalPointer trỏ đến childNode
            return createIndex(row, column, childNode);
        }
        
        return QModelIndex();
    }
    
    // ----------------------------------------
    // QUAN TRỌNG: Tìm parent của item
    // ----------------------------------------
    QModelIndex parent(const QModelIndex &child) const override {
        if (!child.isValid())
            return QModelIndex();
        
        // Lấy node từ index
        TreeNode *childNode = static_cast<TreeNode*>(child.internalPointer());
        TreeNode *parentNode = childNode->parentNode;
        
        // Nếu parent là root → return invalid index
        if (parentNode == rootNode || parentNode == nullptr)
            return QModelIndex();
        
        // Tìm row của parent trong grandparent
        int row = parentNode->row();
        return createIndex(row, 0, parentNode);
    }
    
    // ----------------------------------------
    // Số hàng (số children của parent)
    // ----------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        TreeNode *parentNode;
        
        if (!parent.isValid())
            parentNode = rootNode;
        else
            parentNode = static_cast<TreeNode*>(parent.internalPointer());
        
        return parentNode->children.count();
    }
    
    // ----------------------------------------
    // Số cột
    // ----------------------------------------
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 2; // Name và Type
    }
    
    // ----------------------------------------
    // Data
    // ----------------------------------------
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();
        
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        
        if (role == Qt::DisplayRole) {
            if (index.column() == 0)
                return node->name;
            else if (index.column() == 1)
                return node->type;
        }
        
        // Icon cho folder/file
        if (role == Qt::DecorationRole && index.column() == 0) {
            if (node->type == "folder")
                return QIcon::fromTheme("folder");
            else
                return QIcon::fromTheme("text-x-generic");
        }
        
        // Màu
        if (role == Qt::ForegroundRole) {
            if (node->type == "folder")
                return QColor(Qt::blue);
            return QColor(Qt::black);
        }
        
        // Font
        if (role == Qt::FontRole) {
            QFont font;
            if (node->type == "folder")
                font.setBold(true);
            return font;
        }
        
        return QVariant();
    }
    
    // ----------------------------------------
    // Headers
    // ----------------------------------------
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            if (section == 0)
                return "Tên";
            else if (section == 1)
                return "Loại";
        }
        return QVariant();
    }
    
    // ----------------------------------------
    // Flags
    // ----------------------------------------
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        
        return QAbstractItemModel::flags(index);
    }
    
    // ----------------------------------------
    // Helper: Thêm node
    // ----------------------------------------
    void addNode(const QModelIndex &parent, const QString &name, 
                 const QString &type) {
        TreeNode *parentNode;
        if (!parent.isValid())
            parentNode = rootNode;
        else
            parentNode = static_cast<TreeNode*>(parent.internalPointer());
        
        int row = parentNode->children.count();
        beginInsertRows(parent, row, row);
        
        TreeNode *newNode = new TreeNode(name, type);
        parentNode->addChild(newNode);
        
        endInsertRows();
    }
    
    // ----------------------------------------
    // Helper: Xóa node
    // ----------------------------------------
    bool removeNode(const QModelIndex &index) {
        if (!index.isValid())
            return false;
        
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        TreeNode *parentNode = node->parentNode;
        
        if (!parentNode)
            return false;
        
        int row = node->row();
        QModelIndex parentIndex = parent(index);
        
        beginRemoveRows(parentIndex, row, row);
        parentNode->children.remove(row);
        delete node;
        endRemoveRows();
        
        return true;
    }
};

// ============================================
// BƯỚC 3: Sử dụng Tree Model
// ============================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Model
    FileSystemModel *model = new FileSystemModel;
    
    // View
    QTreeView *view = new QTreeView;
    view->setModel(model);
    
    // Tùy chỉnh
    view->setAlternatingRowColors(true);
    view->setAnimated(true);
    view->expandAll(); // Mở tất cả folders
    
    // Resize columns
    view->resizeColumnToContents(0);
    view->resizeColumnToContents(1);
    
    // Window
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // Buttons
    QPushButton *addFolderBtn = new QPushButton("Thêm Folder");
    QPushButton *addFileBtn = new QPushButton("Thêm File");
    QPushButton *removeBtn = new QPushButton("Xóa item được chọn");
    
    QObject::connect(addFolderBtn, &QPushButton::clicked, [=]() {
        QModelIndex current = view->currentIndex();
        model->addNode(current, "New Folder", "folder");
        view->expand(current);
    });
    
    QObject::connect(addFileBtn, &QPushButton::clicked, [=]() {
        QModelIndex current = view->currentIndex();
        model->addNode(current, "New File.txt", "file");
        view->expand(current);
    });
    
    QObject::connect(removeBtn, &QPushButton::clicked, [=]() {
        QModelIndex current = view->currentIndex();
        if (current.isValid()) {
            model->removeNode(current);
        }
    });
    
    layout->addWidget(view);
    
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(addFolderBtn);
    btnLayout->addWidget(addFileBtn);
    btnLayout->addWidget(removeBtn);
    layout->addLayout(btnLayout);
    
    window.resize(600, 400);
    window.show();
    
    return app.exec();
}

#include "03_tree_model.moc"

// ============================================
// CÁCH ĐỌC CODE TREE MODEL:
// ============================================
// 1. Tìm Node structure: TreeNode với parent & children
// 2. index(row, col, parent):
//    - parent → lấy parentNode
//    - parentNode->children[row] → lấy childNode  
//    - createIndex(row, col, childNode) → tạo index
// 3. parent(child):
//    - child → lấy childNode
//    - childNode->parent → lấy parentNode
//    - Tìm row của parentNode trong grandparent
//    - createIndex(row, 0, parentNode)
// 4. rowCount(parent):
//    - parent → lấy parentNode
//    - return parentNode->children.count()
// 5. QModelIndex.internalPointer() lưu con trỏ đến TreeNode
//
// KEY CONCEPT:
// - Root: QModelIndex() không hợp lệ (isValid() == false)
// - Mỗi item: có pointer trong internalPointer()
// - index() tạo index từ row/col/parent
// - parent() tìm parent index từ child index
