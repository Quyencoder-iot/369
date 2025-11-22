// Ví dụ về QAbstractItemModel - Tree Model
#include <QApplication>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <memory>

// Node trong tree - đại diện cho file hoặc folder
class TreeNode {
public:
    explicit TreeNode(const QString &name, bool isFolder = false, TreeNode *parent = nullptr)
        : m_name(name), m_isFolder(isFolder), m_parentNode(parent) {
        if (m_parentNode)
            m_parentNode->appendChild(this);
    }
    
    ~TreeNode() {
        qDeleteAll(m_children);
    }
    
    // Thêm child node
    void appendChild(TreeNode *child) {
        m_children.append(child);
        child->m_parentNode = this;
    }
    
    // Getters
    TreeNode *child(int row) const {
        if (row < 0 || row >= m_children.size())
            return nullptr;
        return m_children.at(row);
    }
    
    int childCount() const { return m_children.count(); }
    int columnCount() const { return 2; } // Name và Size/Type
    
    QVariant data(int column) const {
        switch (column) {
        case 0: // Name column
            return m_name;
        case 1: // Type/Size column
            return m_isFolder ? "Folder" : "File";
        }
        return QVariant();
    }
    
    TreeNode *parentNode() { return m_parentNode; }
    
    int row() const {
        if (m_parentNode)
            return m_parentNode->m_children.indexOf(const_cast<TreeNode*>(this));
        return 0;
    }
    
    QString name() const { return m_name; }
    bool isFolder() const { return m_isFolder; }
    
    void setName(const QString &name) { m_name = name; }
    
private:
    QString m_name;
    bool m_isFolder;
    QList<TreeNode*> m_children;
    TreeNode *m_parentNode;
};

// Tree Model implementation
class FileSystemTreeModel : public QAbstractItemModel {
    Q_OBJECT
    
public:
    explicit FileSystemTreeModel(QObject *parent = nullptr)
        : QAbstractItemModel(parent) {
        // Tạo root node
        m_rootNode = new TreeNode("Root", true);
        
        // Tạo cấu trúc file system mẫu
        setupModelData();
    }
    
    ~FileSystemTreeModel() override {
        delete m_rootNode;
    }
    
    // 1. PHƯƠNG THỨC BẮT BUỘC: Tạo index từ row, column và parent
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (!hasIndex(row, column, parent))
            return QModelIndex();
            
        TreeNode *parentNode;
        
        if (!parent.isValid())
            parentNode = m_rootNode;
        else
            parentNode = static_cast<TreeNode*>(parent.internalPointer());
            
        TreeNode *childNode = parentNode->child(row);
        if (childNode)
            return createIndex(row, column, childNode);
        
        return QModelIndex();
    }
    
    // 2. PHƯƠNG THỨC BẮT BUỘC: Lấy parent của một index
    QModelIndex parent(const QModelIndex &index) const override {
        if (!index.isValid())
            return QModelIndex();
            
        TreeNode *childNode = static_cast<TreeNode*>(index.internalPointer());
        TreeNode *parentNode = childNode->parentNode();
        
        if (parentNode == m_rootNode)
            return QModelIndex();
            
        return createIndex(parentNode->row(), 0, parentNode);
    }
    
    // 3. PHƯƠNG THỨC BẮT BUỘC: Số hàng (children) của một parent
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        TreeNode *parentNode;
        if (parent.column() > 0)
            return 0;
            
        if (!parent.isValid())
            parentNode = m_rootNode;
        else
            parentNode = static_cast<TreeNode*>(parent.internalPointer());
            
        return parentNode->childCount();
    }
    
    // 4. PHƯƠNG THỨC BẮT BUỘC: Số cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 2; // Name và Type
    }
    
    // 5. PHƯƠNG THỨC BẮT BUỘC: Dữ liệu cho mỗi cell
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();
            
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return node->data(index.column());
        }
        else if (role == Qt::DecorationRole && index.column() == 0) {
            // Icon cho folder/file
            return node->isFolder() ? "📁" : "📄";
        }
        else if (role == Qt::ToolTipRole) {
            return QString("Path: %1\nType: %2")
                .arg(getFullPath(index))
                .arg(node->isFolder() ? "Folder" : "File");
        }
        
        return QVariant();
    }
    
    // 6. PHƯƠNG THỨC TÙY CHỌN: Headers
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return "Name";
            case 1:
                return "Type";
            }
        }
        return QVariant();
    }
    
    // 7. PHƯƠNG THỨC TÙY CHỌN: Flags
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
            
        Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        
        // Chỉ cho phép edit column 0 (name)
        if (index.column() == 0) {
            flags |= Qt::ItemIsEditable;
        }
        
        return flags;
    }
    
    // 8. PHƯƠNG THỨC TÙY CHỌN: Set data
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {
        if (!index.isValid() || role != Qt::EditRole || index.column() != 0)
            return false;
            
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        node->setName(value.toString());
        
        emit dataChanged(index, index);
        return true;
    }
    
    // CUSTOM METHODS: Thêm file/folder
    void addItem(const QModelIndex &parent, const QString &name, bool isFolder) {
        TreeNode *parentNode;
        
        if (!parent.isValid())
            parentNode = m_rootNode;
        else
            parentNode = static_cast<TreeNode*>(parent.internalPointer());
            
        // Chỉ cho phép thêm vào folder
        if (!parentNode->isFolder() && parentNode != m_rootNode)
            return;
            
        int row = parentNode->childCount();
        beginInsertRows(parent, row, row);
        new TreeNode(name, isFolder, parentNode);
        endInsertRows();
    }
    
    // Xóa item
    void removeItem(const QModelIndex &index) {
        if (!index.isValid())
            return;
            
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        TreeNode *parentNode = node->parentNode();
        
        if (!parentNode)
            return;
            
        beginRemoveRows(parent(index), index.row(), index.row());
        delete node;
        endRemoveRows();
    }
    
    // Helper: Lấy full path
    QString getFullPath(const QModelIndex &index) const {
        if (!index.isValid())
            return "/";
            
        QStringList path;
        QModelIndex current = index;
        
        while (current.isValid()) {
            TreeNode *node = static_cast<TreeNode*>(current.internalPointer());
            path.prepend(node->name());
            current = parent(current);
        }
        
        return "/" + path.join("/");
    }
    
    // 9. PHƯƠNG THỨC QUAN TRỌNG: hasChildren
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override {
        if (!parent.isValid())
            return true; // Root luôn có children
            
        TreeNode *node = static_cast<TreeNode*>(parent.internalPointer());
        return node->isFolder() && node->childCount() > 0;
    }
    
private:
    void setupModelData() {
        // Tạo cấu trúc thư mục mẫu
        TreeNode *documents = new TreeNode("Documents", true, m_rootNode);
        new TreeNode("report.docx", false, documents);
        new TreeNode("presentation.pptx", false, documents);
        
        TreeNode *projects = new TreeNode("Projects", true, documents);
        new TreeNode("project1.txt", false, projects);
        new TreeNode("project2.txt", false, projects);
        
        TreeNode *pictures = new TreeNode("Pictures", true, m_rootNode);
        new TreeNode("photo1.jpg", false, pictures);
        new TreeNode("photo2.png", false, pictures);
        
        TreeNode *vacation = new TreeNode("Vacation", true, pictures);
        new TreeNode("beach.jpg", false, vacation);
        new TreeNode("mountain.jpg", false, vacation);
        
        TreeNode *downloads = new TreeNode("Downloads", true, m_rootNode);
        new TreeNode("setup.exe", false, downloads);
        new TreeNode("document.pdf", false, downloads);
    }
    
    TreeNode *m_rootNode;
};

// Widget demo
class TreeModelDemo : public QWidget {
    Q_OBJECT
    
public:
    TreeModelDemo() {
        // Tạo model
        m_model = new FileSystemTreeModel(this);
        
        // Tạo view
        m_treeView = new QTreeView;
        m_treeView->setModel(m_model);
        m_treeView->expandAll(); // Mở rộng toàn bộ tree
        m_treeView->setEditTriggers(QAbstractItemView::DoubleClicked);
        m_treeView->setAlternatingRowColors(true);
        
        // Input controls
        m_nameEdit = new QLineEdit;
        m_nameEdit->setPlaceholderText("Nhập tên file/folder...");
        
        auto *addFileBtn = new QPushButton("Thêm File");
        auto *addFolderBtn = new QPushButton("Thêm Folder");
        auto *removeBtn = new QPushButton("Xóa");
        auto *expandBtn = new QPushButton("Mở rộng tất cả");
        auto *collapseBtn = new QPushButton("Thu gọn tất cả");
        
        // Layouts
        auto *inputLayout = new QHBoxLayout;
        inputLayout->addWidget(m_nameEdit);
        inputLayout->addWidget(addFileBtn);
        inputLayout->addWidget(addFolderBtn);
        
        auto *actionLayout = new QHBoxLayout;
        actionLayout->addWidget(removeBtn);
        actionLayout->addWidget(expandBtn);
        actionLayout->addWidget(collapseBtn);
        
        auto *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("🌳 Demo Tree Model (QAbstractItemModel)"));
        layout->addWidget(m_treeView);
        layout->addLayout(inputLayout);
        layout->addLayout(actionLayout);
        
        // Path label
        m_pathLabel = new QLabel("Path: /");
        layout->addWidget(m_pathLabel);
        
        // Connections
        connect(addFileBtn, &QPushButton::clicked, this, [this]() {
            addItem(false);
        });
        
        connect(addFolderBtn, &QPushButton::clicked, this, [this]() {
            addItem(true);
        });
        
        connect(removeBtn, &QPushButton::clicked, this, [this]() {
            auto index = m_treeView->currentIndex();
            if (index.isValid()) {
                m_model->removeItem(index);
            }
        });
        
        connect(expandBtn, &QPushButton::clicked, m_treeView, &QTreeView::expandAll);
        connect(collapseBtn, &QPushButton::clicked, m_treeView, &QTreeView::collapseAll);
        
        // Update path label khi selection thay đổi
        connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
                this, [this](const QModelIndex &current) {
            QString path = m_model->getFullPath(current);
            m_pathLabel->setText("Path: " + path);
        });
    }
    
private:
    void addItem(bool isFolder) {
        QString name = m_nameEdit->text();
        if (name.isEmpty()) {
            name = isFolder ? "New Folder" : "new_file.txt";
        }
        
        auto current = m_treeView->currentIndex();
        m_model->addItem(current, name, isFolder);
        m_nameEdit->clear();
        
        // Expand parent để thấy item mới
        if (current.isValid()) {
            m_treeView->expand(current);
        }
    }
    
    FileSystemTreeModel *m_model;
    QTreeView *m_treeView;
    QLineEdit *m_nameEdit;
    QLabel *m_pathLabel;
};

#include "tree_model_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TreeModelDemo demo;
    demo.setWindowTitle("Tree Model Example");
    demo.resize(600, 700);
    demo.show();
    
    return app.exec();
}

/*
 * TỔNG KẾT VỀ QAbstractItemModel (Tree Model):
 * 
 * 1. Phức tạp hơn List/Table Model vì có hierarchy:
 *    - Cần implement index() và parent()
 *    - Sử dụng internalPointer() để lưu node data
 *    - createIndex() để tạo QModelIndex
 * 
 * 2. Quan trọng nhất:
 *    - index(): Map từ row/col/parent -> QModelIndex
 *    - parent(): Lấy parent của một index
 *    - Hai methods này phải nhất quán với nhau!
 * 
 * 3. Tips:
 *    - Dùng tree node structure riêng để quản lý hierarchy
 *    - Root node không hiển thị, chỉ dùng làm container
 *    - hasChildren() giúp optimize performance
 * 
 * 4. Debug:
 *    - Check index.isValid() trước khi dùng
 *    - Log parent-child relationships
 *    - Verify createIndex() parameters
 * 
 * 5. Common bugs:
 *    - Quên check column > 0 trong rowCount()
 *    - Sai logic trong parent() method
 *    - Memory leaks với tree nodes
 */