// model_pointer_example.cpp - Ví dụ về con trỏ với Qt Model

#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QTableView>
#include <QListView>
#include <QSplitter>
#include <QDebug>
#include <QPointer>
#include <memory>

// Custom Model class để demo ownership
class MyCustomModel : public QStandardItemModel {
    Q_OBJECT
public:
    MyCustomModel(QObject* parent = nullptr) : QStandardItemModel(parent) {
        qDebug() << "MyCustomModel created at" << this;
    }
    
    ~MyCustomModel() {
        qDebug() << "MyCustomModel destroyed at" << this;
    }
};

// Demo class để minh họa các pattern con trỏ
class ModelPointerDemo : public QWidget {
    Q_OBJECT
    
private:
    // Con trỏ thông thường
    MyCustomModel* m_model;
    
    // Smart pointer - tự động quản lý memory
    std::unique_ptr<QStandardItemModel> m_smartModel;
    
    // QPointer - tự động set null khi object bị delete
    QPointer<MyCustomModel> m_safeModel;
    
    // Views
    QTableView* m_tableView;
    QTreeView* m_treeView;
    QListView* m_listView;
    
public:
    ModelPointerDemo(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        demonstratePointerPatterns();
    }
    
    void setupUI() {
        // Tạo layout với splitter
        QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
        
        // Khởi tạo views - parent là splitter nên tự động được delete
        m_tableView = new QTableView(splitter);
        m_treeView = new QTreeView(splitter);
        m_listView = new QListView(splitter);
        
        splitter->addWidget(m_tableView);
        splitter->addWidget(m_treeView);
        splitter->addWidget(m_listView);
        
        // Main layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(splitter);
    }
    
    void demonstratePointerPatterns() {
        qDebug() << "\n=== DEMO 1: Basic Pointer Usage ===";
        basicPointerDemo();
        
        qDebug() << "\n=== DEMO 2: Parent-Child Ownership ===";
        parentChildDemo();
        
        qDebug() << "\n=== DEMO 3: Shared Model Pattern ===";
        sharedModelDemo();
        
        qDebug() << "\n=== DEMO 4: Safe Pointer Usage ===";
        safePointerDemo();
        
        qDebug() << "\n=== DEMO 5: Item Pointer Management ===";
        itemPointerDemo();
    }
    
    // Demo 1: Cách dùng con trỏ cơ bản
    void basicPointerDemo() {
        // Tạo model KHÔNG có parent
        m_model = new MyCustomModel(); // Phải tự delete
        
        // Populate data
        for (int row = 0; row < 3; ++row) {
            QList<QStandardItem*> items;
            for (int col = 0; col < 3; ++col) {
                // Model sẽ sở hữu các item này
                QStandardItem* item = new QStandardItem(
                    QString("Row%1,Col%2").arg(row).arg(col)
                );
                items.append(item);
            }
            m_model->appendRow(items);
        }
        
        // Set model cho view - view KHÔNG sở hữu model
        m_tableView->setModel(m_model);
        
        qDebug() << "Model address:" << m_model;
        qDebug() << "TableView's model:" << m_tableView->model();
    }
    
    // Demo 2: Parent-Child ownership
    void parentChildDemo() {
        // Model với parent - tự động được delete khi parent bị delete
        MyCustomModel* modelWithParent = new MyCustomModel(this);
        
        qDebug() << "Model with parent:" << modelWithParent;
        qDebug() << "Parent:" << modelWithParent->parent();
        
        // Không cần delete modelWithParent - Qt tự xử lý
    }
    
    // Demo 3: Nhiều view dùng chung model
    void sharedModelDemo() {
        // Tất cả views dùng chung một model
        m_tableView->setModel(m_model);
        m_treeView->setModel(m_model);
        m_listView->setModel(m_model);
        
        qDebug() << "All views share the same model:";
        qDebug() << "TableView model:" << m_tableView->model();
        qDebug() << "TreeView model:" << m_treeView->model();
        qDebug() << "ListView model:" << m_listView->model();
    }
    
    // Demo 4: QPointer để an toàn
    void safePointerDemo() {
        // Tạo model và lưu trong QPointer
        m_safeModel = new MyCustomModel();
        
        qDebug() << "Safe model created:" << m_safeModel;
        
        // Simulate deletion
        delete m_safeModel; // QPointer tự động set về null
        
        // Kiểm tra an toàn trước khi dùng
        if (m_safeModel) {
            qDebug() << "Model is valid";
        } else {
            qDebug() << "Model has been deleted - QPointer is null";
        }
    }
    
    // Demo 5: Làm việc với item pointers
    void itemPointerDemo() {
        if (!m_model) return;
        
        // Lấy con trỏ đến item qua index
        QModelIndex index = m_model->index(0, 0);
        QStandardItem* item = m_model->itemFromIndex(index);
        
        if (item) {
            qDebug() << "Item text before:" << item->text();
            item->setText("Modified!");
            qDebug() << "Item text after:" << item->text();
            
            // Thêm child item
            QStandardItem* childItem = new QStandardItem("Child Item");
            item->appendRow(childItem); // item sở hữu childItem
            
            // KHÔNG delete childItem - parent item sẽ xử lý
        }
        
        // Lấy item qua row/column
        QStandardItem* directItem = m_model->item(1, 1);
        if (directItem) {
            directItem->setBackground(Qt::yellow);
        }
    }
    
    ~ModelPointerDemo() {
        // Phải clear model từ views trước khi delete
        m_tableView->setModel(nullptr);
        m_treeView->setModel(nullptr);
        m_listView->setModel(nullptr);
        
        // Delete model không có parent
        delete m_model;
        
        qDebug() << "ModelPointerDemo destroyed";
    }
};

// Hàm chính để chạy demo
void runModelPointerExamples() {
    qDebug() << "=== Qt Model Pointer Examples ===\n";
    
    // Example 1: Memory leak - WRONG WAY
    qDebug() << "WRONG: Memory leak example";
    {
        QStandardItemModel* leakyModel = new QStandardItemModel();
        QTableView view;
        view.setModel(leakyModel);
        // view bị destroy nhưng model vẫn còn trong memory!
    }
    
    // Example 2: Dangling pointer - WRONG WAY
    qDebug() << "\nWRONG: Dangling pointer example";
    {
        QTableView view;
        {
            QStandardItemModel model;
            view.setModel(&model);
        } // model bị destroy
        // view.model() giờ trỏ đến vùng nhớ không hợp lệ!
    }
    
    // Example 3: Correct usage
    qDebug() << "\nCORRECT: Proper pointer management";
    {
        // Cách 1: Model với parent
        QWidget parent;
        QStandardItemModel* model1 = new QStandardItemModel(&parent);
        
        // Cách 2: Smart pointer
        auto model2 = std::make_unique<QStandardItemModel>();
        
        // Cách 3: Stack allocation cho lifetime rõ ràng
        QStandardItemModel model3;
        
        QTableView view;
        view.setModel(&model3);
    }
}

// Main function
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Chạy console examples
    runModelPointerExamples();
    
    // Tạo GUI demo
    ModelPointerDemo demo;
    demo.setWindowTitle("Qt Model Pointer Demo");
    demo.resize(800, 600);
    demo.show();
    
    return app.exec();
}

#include "model_pointer_example.moc"