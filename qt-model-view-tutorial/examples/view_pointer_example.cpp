// view_pointer_example.cpp - Ví dụ về con trỏ với Qt View

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QScrollBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>
#include <QPointer>
#include <memory>

// Custom View để demo lifecycle
class CustomTableView : public QTableView {
    Q_OBJECT
public:
    CustomTableView(QWidget* parent = nullptr) : QTableView(parent) {
        qDebug() << "CustomTableView created at" << this;
    }
    
    ~CustomTableView() {
        qDebug() << "CustomTableView destroyed at" << this;
    }
    
    // Override để demo khi model thay đổi
    void setModel(QAbstractItemModel* model) override {
        qDebug() << "Setting model" << model << "to view" << this;
        QAbstractItemModel* oldModel = this->model();
        if (oldModel) {
            qDebug() << "Replacing old model" << oldModel;
        }
        QTableView::setModel(model);
    }
};

// Demo class cho View pointers
class ViewPointerDemo : public QMainWindow {
    Q_OBJECT
    
private:
    // Models
    QStandardItemModel* m_mainModel;
    QSortFilterProxyModel* m_proxyModel;
    
    // Views - raw pointers với parent management
    CustomTableView* m_tableView;
    QTreeView* m_treeView;
    QListView* m_listView;
    
    // Selection models
    QItemSelectionModel* m_sharedSelection;
    
    // Smart pointer cho view không có parent
    std::unique_ptr<QTableView> m_standaloneView;
    
    // QPointer cho safety
    QPointer<QTableView> m_safeViewPointer;
    
public:
    ViewPointerDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupModels();
        setupUI();
        demonstrateViewPointers();
    }
    
    void setupModels() {
        // Main model - owned by this
        m_mainModel = new QStandardItemModel(this);
        
        // Populate với data
        m_mainModel->setHorizontalHeaderLabels({"Column 1", "Column 2", "Column 3"});
        for (int row = 0; row < 10; ++row) {
            QList<QStandardItem*> items;
            for (int col = 0; col < 3; ++col) {
                items << new QStandardItem(QString("Item %1-%2").arg(row).arg(col));
            }
            m_mainModel->appendRow(items);
        }
        
        // Proxy model cho filtering/sorting
        m_proxyModel = new QSortFilterProxyModel(this);
        m_proxyModel->setSourceModel(m_mainModel);
    }
    
    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Group 1: Basic Views
        QGroupBox* basicGroup = new QGroupBox("Basic View Pointers", centralWidget);
        QHBoxLayout* basicLayout = new QHBoxLayout(basicGroup);
        
        // Views với parent - tự động delete
        m_tableView = new CustomTableView(basicGroup);
        m_treeView = new QTreeView(basicGroup);
        m_listView = new QListView(basicGroup);
        
        basicLayout->addWidget(m_tableView);
        basicLayout->addWidget(m_treeView);
        basicLayout->addWidget(m_listView);
        
        mainLayout->addWidget(basicGroup);
        
        // Control buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        
        QPushButton* btnViewModel = new QPushButton("Demo View-Model Pointers");
        connect(btnViewModel, &QPushButton::clicked, this, &ViewPointerDemo::demoViewModelPointers);
        
        QPushButton* btnSelection = new QPushButton("Demo Selection Pointers");
        connect(btnSelection, &QPushButton::clicked, this, &ViewPointerDemo::demoSelectionPointers);
        
        QPushButton* btnHeader = new QPushButton("Demo Header Pointers");
        connect(btnHeader, &QPushButton::clicked, this, &ViewPointerDemo::demoHeaderPointers);
        
        QPushButton* btnProxy = new QPushButton("Demo Proxy Model");
        connect(btnProxy, &QPushButton::clicked, this, &ViewPointerDemo::demoProxyModel);
        
        buttonLayout->addWidget(btnViewModel);
        buttonLayout->addWidget(btnSelection);
        buttonLayout->addWidget(btnHeader);
        buttonLayout->addWidget(btnProxy);
        
        mainLayout->addLayout(buttonLayout);
    }
    
    void demonstrateViewPointers() {
        qDebug() << "\n=== View Pointer Demonstrations ===";
        
        // Set models cho views
        m_tableView->setModel(m_mainModel);
        m_treeView->setModel(m_mainModel);
        m_listView->setModel(m_mainModel);
        
        // Demo standalone view với smart pointer
        m_standaloneView = std::make_unique<QTableView>();
        m_standaloneView->setModel(m_mainModel);
        m_standaloneView->setWindowTitle("Standalone View (Smart Pointer)");
        m_standaloneView->show();
    }
    
private slots:
    void demoViewModelPointers() {
        qDebug() << "\n=== Demo: View-Model Pointer Relationship ===";
        
        // Lấy con trỏ model từ view
        QAbstractItemModel* modelFromView = m_tableView->model();
        qDebug() << "Model from view:" << modelFromView;
        qDebug() << "Original model:" << m_mainModel;
        qDebug() << "Same pointer?" << (modelFromView == m_mainModel);
        
        // Swap models
        qDebug() << "\nSwapping models...";
        QStandardItemModel* tempModel = new QStandardItemModel(5, 2, this);
        tempModel->setHorizontalHeaderLabels({"Temp 1", "Temp 2"});
        
        // Lưu model cũ
        QAbstractItemModel* oldModel = m_tableView->model();
        
        // Set model mới
        m_tableView->setModel(tempModel);
        
        // Model cũ vẫn còn và có thể dùng lại
        m_treeView->setModel(oldModel);
        
        // Clear model - set nullptr
        qDebug() << "\nClearing list view model...";
        m_listView->setModel(nullptr);
    }
    
    void demoSelectionPointers() {
        qDebug() << "\n=== Demo: Selection Model Pointers ===";
        
        // Mỗi view có selection model riêng theo mặc định
        QItemSelectionModel* tableSelection = m_tableView->selectionModel();
        QItemSelectionModel* treeSelection = m_treeView->selectionModel();
        
        qDebug() << "Table selection model:" << tableSelection;
        qDebug() << "Tree selection model:" << treeSelection;
        qDebug() << "Same selection?" << (tableSelection == treeSelection);
        
        // Tạo shared selection model
        if (!m_sharedSelection) {
            m_sharedSelection = new QItemSelectionModel(m_mainModel, this);
        }
        
        // Set cùng selection model cho nhiều views
        m_tableView->setSelectionModel(m_sharedSelection);
        m_treeView->setSelectionModel(m_sharedSelection);
        
        qDebug() << "\nAfter sharing selection model:";
        qDebug() << "Table selection:" << m_tableView->selectionModel();
        qDebug() << "Tree selection:" << m_treeView->selectionModel();
        
        // Connect để demo shared selection
        connect(m_sharedSelection, &QItemSelectionModel::currentChanged,
                [](const QModelIndex& current, const QModelIndex& previous) {
            qDebug() << "Selection changed from" << previous << "to" << current;
        });
        
        // Select một item
        m_sharedSelection->select(m_mainModel->index(0, 0), 
                                 QItemSelectionModel::Select);
    }
    
    void demoHeaderPointers() {
        qDebug() << "\n=== Demo: Header View Pointers ===";
        
        // Lấy con trỏ đến header views
        QHeaderView* horizontalHeader = m_tableView->horizontalHeader();
        QHeaderView* verticalHeader = m_tableView->verticalHeader();
        
        qDebug() << "Horizontal header:" << horizontalHeader;
        qDebug() << "Vertical header:" << verticalHeader;
        qDebug() << "Header parent:" << horizontalHeader->parent();
        
        // Headers được view sở hữu - không delete thủ công
        horizontalHeader->setSectionResizeMode(QHeaderView::Stretch);
        verticalHeader->hide();
        
        // Viewport và scrollbars cũng là con của view
        QWidget* viewport = m_tableView->viewport();
        QScrollBar* vScrollBar = m_tableView->verticalScrollBar();
        QScrollBar* hScrollBar = m_tableView->horizontalScrollBar();
        
        qDebug() << "\nView components:";
        qDebug() << "Viewport:" << viewport << "parent:" << viewport->parent();
        qDebug() << "V-ScrollBar:" << vScrollBar << "parent:" << vScrollBar->parent();
        qDebug() << "H-ScrollBar:" << hScrollBar << "parent:" << hScrollBar->parent();
    }
    
    void demoProxyModel() {
        qDebug() << "\n=== Demo: Proxy Model Pointers ===";
        
        // Set proxy model cho một view
        m_listView->setModel(m_proxyModel);
        
        // Lấy source model từ proxy
        QAbstractItemModel* sourceFromProxy = m_proxyModel->sourceModel();
        qDebug() << "Source model from proxy:" << sourceFromProxy;
        qDebug() << "Original model:" << m_mainModel;
        qDebug() << "Same?" << (sourceFromProxy == m_mainModel);
        
        // Map index between proxy and source
        QModelIndex proxyIndex = m_proxyModel->index(0, 0);
        QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
        
        qDebug() << "\nIndex mapping:";
        qDebug() << "Proxy index:" << proxyIndex;
        qDebug() << "Source index:" << sourceIndex;
        
        // Filter proxy
        m_proxyModel->setFilterRegularExpression(".*1.*");
        qDebug() << "Applied filter - row count changed from" 
                 << m_mainModel->rowCount() << "to" << m_proxyModel->rowCount();
    }
    
    ~ViewPointerDemo() {
        qDebug() << "\n=== ViewPointerDemo Cleanup ===";
        
        // Views với parent sẽ tự động được delete
        // Smart pointer view sẽ tự động được delete
        // Models với parent sẽ tự động được delete
        
        // Nếu có view không có parent và dùng raw pointer, phải delete:
        // delete m_orphanView; // ví dụ
    }
};

// Các pattern và best practices cho View pointers
void demonstrateViewPatterns() {
    qDebug() << "\n=== View Pointer Patterns & Best Practices ===";
    
    // Pattern 1: View lifecycle management
    {
        qDebug() << "\nPattern 1: View Lifecycle";
        QWidget parent;
        QTableView* view = new QTableView(&parent);
        // view sẽ được parent delete - GOOD
    }
    
    // Pattern 2: Multiple views, one model
    {
        qDebug() << "\nPattern 2: Multiple Views";
        QStandardItemModel model;
        
        QTableView view1;
        QListView view2;
        
        view1.setModel(&model);
        view2.setModel(&model);
        
        // Cả hai view dùng chung model - GOOD
    }
    
    // Pattern 3: View without model
    {
        qDebug() << "\nPattern 3: View without Model";
        QTableView view;
        // view.model() returns nullptr - SAFE
        
        if (view.model()) {
            // Won't execute
        }
    }
    
    // Anti-pattern: Deleting model while view still uses it
    {
        qDebug() << "\nAnti-pattern: Premature Model Deletion";
        QTableView view;
        
        {
            QStandardItemModel model;
            view.setModel(&model);
        } // model destroyed here!
        
        // view still has pointer to destroyed model - DANGEROUS
        // Solution: view.setModel(nullptr) before model destruction
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Run pattern demonstrations
    demonstrateViewPatterns();
    
    // Create main demo window
    ViewPointerDemo demo;
    demo.setWindowTitle("Qt View Pointer Demonstrations");
    demo.resize(1000, 600);
    demo.show();
    
    return app.exec();
}

#include "view_pointer_example.moc"