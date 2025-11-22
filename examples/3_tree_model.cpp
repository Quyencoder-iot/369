// Ví dụ 3: Tree Model với QStandardItemModel
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QStandardItemModel>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileIconProvider>

// Custom item để lưu thêm data
class TreeItem : public QStandardItem {
public:
    enum ItemType {
        Company,
        Department,
        Employee
    };
    
    TreeItem(const QString &text, ItemType type) 
        : QStandardItem(text), itemType(type) {
        // Set icon theo type
        QFileIconProvider iconProvider;
        switch (type) {
            case Company:
                setIcon(QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
                break;
            case Department:
                setIcon(QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
                break;
            case Employee:
                setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
                break;
        }
    }
    
    ItemType getItemType() const { return itemType; }
    
private:
    ItemType itemType;
};

class TreeModelExample : public QWidget {
    Q_OBJECT

public:
    TreeModelExample(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupModel();
        connectSignals();
    }

private:
    void setupUI() {
        auto *mainLayout = new QVBoxLayout(this);
        
        // Title
        auto *titleLabel = new QLabel("Tree Model Example - Cấu trúc công ty");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
        mainLayout->addWidget(titleLabel);
        
        // Tree View
        treeView = new QTreeView();
        treeView->setAlternatingRowColors(true);
        treeView->setAnimated(true);
        treeView->setSortingEnabled(true);
        treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        mainLayout->addWidget(treeView);
        
        // Control Panel
        auto *controlLayout = new QHBoxLayout();
        
        itemNameEdit = new QLineEdit();
        itemNameEdit->setPlaceholderText("Tên item mới...");
        
        addChildButton = new QPushButton("Thêm con");
        addSiblingButton = new QPushButton("Thêm cùng cấp");
        removeButton = new QPushButton("Xóa");
        expandAllButton = new QPushButton("Mở rộng tất cả");
        collapseAllButton = new QPushButton("Thu gọn tất cả");
        
        controlLayout->addWidget(itemNameEdit);
        controlLayout->addWidget(addChildButton);
        controlLayout->addWidget(addSiblingButton);
        controlLayout->addWidget(removeButton);
        controlLayout->addWidget(expandAllButton);
        controlLayout->addWidget(collapseAllButton);
        controlLayout->addStretch();
        
        mainLayout->addLayout(controlLayout);
        
        // Info
        infoLabel = new QLabel("Click phải để xem context menu. Drag & drop để di chuyển items.");
        infoLabel->setStyleSheet("color: #666; padding: 5px;");
        mainLayout->addWidget(infoLabel);
        
        setMinimumSize(800, 600);
        setWindowTitle("Tree Model Demo");
    }
    
    void setupModel() {
        model = new QStandardItemModel(this);
        model->setHorizontalHeaderLabels({"Tên", "Chức vụ", "Email", "Số điện thoại"});
        
        // Tạo cấu trúc công ty mẫu
        auto *rootItem = model->invisibleRootItem();
        
        // Công ty
        auto *company = new TreeItem("TechCorp Vietnam", TreeItem::Company);
        company->setData("Công ty TNHH TechCorp Vietnam", Qt::UserRole);
        rootItem->appendRow({
            company,
            new QStandardItem(""),
            new QStandardItem("info@techcorp.vn"),
            new QStandardItem("024-1234-5678")
        });
        
        // Phòng IT
        auto *itDept = new TreeItem("Phòng IT", TreeItem::Department);
        itDept->setData("Information Technology Department", Qt::UserRole);
        company->appendRow({
            itDept,
            new QStandardItem(""),
            new QStandardItem("it@techcorp.vn"),
            new QStandardItem("Ext: 101")
        });
        
        // Nhân viên IT
        QStringList itEmployees = {"Nguyễn Văn A", "Trần Thị B", "Lê Văn C"};
        QStringList itPositions = {"Team Lead", "Senior Dev", "Junior Dev"};
        for (int i = 0; i < itEmployees.size(); ++i) {
            auto *employee = new TreeItem(itEmployees[i], TreeItem::Employee);
            employee->setEditable(true);
            
            auto *position = new QStandardItem(itPositions[i]);
            position->setEditable(true);
            
            auto *email = new QStandardItem(QString("user%1@techcorp.vn").arg(i+1));
            email->setEditable(true);
            
            auto *phone = new QStandardItem(QString("090%1").arg(1234567 + i));
            phone->setEditable(true);
            
            itDept->appendRow({employee, position, email, phone});
        }
        
        // Phòng Sales
        auto *salesDept = new TreeItem("Phòng Sales", TreeItem::Department);
        salesDept->setData("Sales Department", Qt::UserRole);
        company->appendRow({
            salesDept,
            new QStandardItem(""),
            new QStandardItem("sales@techcorp.vn"),
            new QStandardItem("Ext: 102")
        });
        
        // Nhân viên Sales
        auto *salesManager = new TreeItem("Phạm Văn D", TreeItem::Employee);
        salesDept->appendRow({
            salesManager,
            new QStandardItem("Sales Manager"),
            new QStandardItem("manager@techcorp.vn"),
            new QStandardItem("0912345678")
        });
        
        // Set model và cấu hình view
        treeView->setModel(model);
        treeView->setColumnWidth(0, 250);
        treeView->setColumnWidth(1, 150);
        treeView->setColumnWidth(2, 200);
        
        // Expand mặc định
        treeView->expandAll();
        
        // Enable drag & drop
        treeView->setDragDropMode(QAbstractItemView::InternalMove);
        treeView->setDefaultDropAction(Qt::MoveAction);
    }
    
    void connectSignals() {
        // Thêm child item
        connect(addChildButton, &QPushButton::clicked, [this]() {
            QModelIndex current = treeView->currentIndex();
            if (!current.isValid()) {
                updateInfo("Chọn một item để thêm con");
                return;
            }
            
            QString name = itemNameEdit->text().trimmed();
            if (name.isEmpty()) name = "Item mới";
            
            auto *parentItem = model->itemFromIndex(current);
            
            // Xác định type cho item mới
            TreeItem::ItemType newType = TreeItem::Employee;
            if (auto *treeItem = dynamic_cast<TreeItem*>(parentItem)) {
                if (treeItem->getItemType() == TreeItem::Company) {
                    newType = TreeItem::Department;
                }
            }
            
            auto *newItem = new TreeItem(name, newType);
            parentItem->appendRow({
                newItem,
                new QStandardItem(""),
                new QStandardItem(""),
                new QStandardItem("")
            });
            
            // Expand parent và select item mới
            treeView->expand(current);
            treeView->setCurrentIndex(newItem->index());
            
            itemNameEdit->clear();
            updateInfo("Đã thêm: " + name);
        });
        
        // Thêm sibling item
        connect(addSiblingButton, &QPushButton::clicked, [this]() {
            QModelIndex current = treeView->currentIndex();
            if (!current.isValid()) {
                updateInfo("Chọn một item để thêm cùng cấp");
                return;
            }
            
            QString name = itemNameEdit->text().trimmed();
            if (name.isEmpty()) name = "Item mới";
            
            QModelIndex parentIndex = current.parent();
            auto *parentItem = parentIndex.isValid() ? 
                model->itemFromIndex(parentIndex) : model->invisibleRootItem();
            
            // Xác định type
            TreeItem::ItemType newType = TreeItem::Employee;
            if (auto *currentTreeItem = dynamic_cast<TreeItem*>(model->itemFromIndex(current))) {
                newType = currentTreeItem->getItemType();
            }
            
            auto *newItem = new TreeItem(name, newType);
            parentItem->appendRow({
                newItem,
                new QStandardItem(""),
                new QStandardItem(""),
                new QStandardItem("")
            });
            
            treeView->setCurrentIndex(newItem->index());
            itemNameEdit->clear();
            updateInfo("Đã thêm: " + name);
        });
        
        // Remove item
        connect(removeButton, &QPushButton::clicked, [this]() {
            QModelIndex current = treeView->currentIndex();
            if (!current.isValid()) {
                updateInfo("Chọn item để xóa");
                return;
            }
            
            QString itemName = model->data(current).toString();
            model->removeRow(current.row(), current.parent());
            updateInfo("Đã xóa: " + itemName);
        });
        
        // Expand/Collapse all
        connect(expandAllButton, &QPushButton::clicked, [this]() {
            treeView->expandAll();
            updateInfo("Đã mở rộng tất cả");
        });
        
        connect(collapseAllButton, &QPushButton::clicked, [this]() {
            treeView->collapseAll();
            updateInfo("Đã thu gọn tất cả");
        });
        
        // Selection changed - hiển thị info
        connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged,
                [this](const QModelIndex &current, const QModelIndex &previous) {
            if (!current.isValid()) return;
            
            auto *item = model->itemFromIndex(current);
            QString info = "Đã chọn: " + item->text();
            
            // Hiển thị custom data nếu có
            QVariant customData = item->data(Qt::UserRole);
            if (customData.isValid()) {
                info += " (" + customData.toString() + ")";
            }
            
            // Đếm số con
            if (item->hasChildren()) {
                info += QString(" - %1 items con").arg(item->rowCount());
            }
            
            updateInfo(info);
        });
        
        // Double click để edit inline
        connect(treeView, &QTreeView::doubleClicked, [this](const QModelIndex &index) {
            if (index.column() == 0) { // Chỉ cho edit cột đầu
                treeView->edit(index);
            }
        });
        
        // Context menu
        connect(treeView, &QTreeView::customContextMenuRequested,
                [this](const QPoint &pos) {
            QModelIndex index = treeView->indexAt(pos);
            if (!index.isValid()) return;
            
            QMenu contextMenu;
            contextMenu.addAction("Thêm con", [this]() { addChildButton->click(); });
            contextMenu.addAction("Thêm cùng cấp", [this]() { addSiblingButton->click(); });
            contextMenu.addSeparator();
            contextMenu.addAction("Xóa", [this]() { removeButton->click(); });
            
            contextMenu.exec(treeView->viewport()->mapToGlobal(pos));
        });
    }
    
    void updateInfo(const QString &text) {
        infoLabel->setText(text);
        
        // Log tree structure
        qDebug() << "\n=== Tree Structure ===";
        printTree(model->invisibleRootItem(), 0);
    }
    
    void printTree(QStandardItem *item, int indent) {
        for (int row = 0; row < item->rowCount(); ++row) {
            auto *child = item->child(row, 0);
            qDebug() << QString("  ").repeated(indent) + "- " + child->text();
            if (child->hasChildren()) {
                printTree(child, indent + 1);
            }
        }
    }

private:
    QTreeView *treeView;
    QStandardItemModel *model;
    QLineEdit *itemNameEdit;
    QPushButton *addChildButton;
    QPushButton *addSiblingButton;
    QPushButton *removeButton;
    QPushButton *expandAllButton;
    QPushButton *collapseAllButton;
    QLabel *infoLabel;
};

#include "3_tree_model.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Tree Model với QStandardItemModel
    // Ưu điểm:
    // - Dễ tạo cấu trúc phân cấp
    // - Hỗ trợ nhiều columns cho mỗi node
    // - Drag & drop built-in
    // - Có thể mix types trong tree
    
    // Concepts quan trọng:
    // - parent(): Lấy node cha
    // - index(): Lấy vị trí của item
    // - hasChildren(): Kiểm tra có con không
    // - invisibleRootItem(): Root ẩn của tree
    
    // Use cases:
    // - File explorer
    // - Organization chart
    // - Category/subcategory
    // - XML/JSON viewer
    
    TreeModelExample window;
    window.show();
    
    return app.exec();
}