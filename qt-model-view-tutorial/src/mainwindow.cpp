// mainwindow.cpp - Implementation của main window

#include "mainwindow.h"
#include "custom_model.h"
#include "custom_delegate.h"
#include "pointer_manager.h"

#include <QTabWidget>
#include <QTextEdit>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_customModel(nullptr)
    , m_standardModel(nullptr)
    , m_proxyModel(nullptr)
    , m_pointerManager(nullptr)
{
    setupUI();
    connectSignals();
    
    log("=== Qt Model-View-Delegate Pointer Demo Started ===");
    log("Mỗi tab chứa các ví dụ về quản lý con trỏ khác nhau.");
    log("Xem console output và log panel để hiểu chi tiết.");
}

MainWindow::~MainWindow()
{
    log("MainWindow destructor called - cleaning up...");
    
    // Views sẽ tự động được delete (có parent)
    // Models với parent sẽ tự động được delete
    // Smart pointers tự động cleanup
    
    qDebug() << "MainWindow destroyed";
}

void MainWindow::setupUI()
{
    // Central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(centralWidget);
    
    createModelTab();
    createViewTab();
    createDelegateTab();
    createMemoryTab();
    
    // Log output at bottom
    m_logOutput = new QTextEdit(centralWidget);
    m_logOutput->setReadOnly(true);
    m_logOutput->setMaximumHeight(150);
    
    // Add to layout with splitter
    QSplitter* splitter = new QSplitter(Qt::Vertical, centralWidget);
    splitter->addWidget(m_tabWidget);
    splitter->addWidget(m_logOutput);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
}

void MainWindow::createModelTab()
{
    QWidget* modelTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(modelTab);
    
    // Views
    QSplitter* viewSplitter = new QSplitter(Qt::Horizontal);
    
    m_modelTableView = new QTableView();
    m_modelTreeView = new QTreeView();
    
    viewSplitter->addWidget(m_modelTableView);
    viewSplitter->addWidget(m_modelTreeView);
    
    layout->addWidget(viewSplitter);
    
    // Control buttons
    QGroupBox* controlGroup = new QGroupBox("Model Pointer Demos");
    QHBoxLayout* buttonLayout = new QHBoxLayout(controlGroup);
    
    auto addButton = [&](const QString& text, auto slot) {
        QPushButton* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, slot);
        buttonLayout->addWidget(btn);
    };
    
    addButton("Demo Ownership", &MainWindow::demoModelOwnership);
    addButton("Demo Sharing", &MainWindow::demoModelSharing);
    addButton("Demo Item Pointers", &MainWindow::demoItemPointers);
    addButton("Demo Cleanup", &MainWindow::demoModelCleanup);
    
    layout->addWidget(controlGroup);
    
    m_tabWidget->addTab(modelTab, "Model Pointers");
    
    // Initialize models
    m_standardModel = new QStandardItemModel(5, 3, this);
    m_standardModel->setHorizontalHeaderLabels({"Column 1", "Column 2", "Column 3"});
}

void MainWindow::createViewTab()
{
    QWidget* viewTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(viewTab);
    
    // Multiple views
    QSplitter* viewSplitter = new QSplitter(Qt::Horizontal);
    
    m_viewTable1 = new QTableView();
    m_viewTable2 = new QTableView();
    m_viewList = new QListView();
    
    viewSplitter->addWidget(m_viewTable1);
    viewSplitter->addWidget(m_viewTable2);
    viewSplitter->addWidget(m_viewList);
    
    layout->addWidget(viewSplitter);
    
    // Control buttons
    QGroupBox* controlGroup = new QGroupBox("View Pointer Demos");
    QHBoxLayout* buttonLayout = new QHBoxLayout(controlGroup);
    
    auto addButton = [&](const QString& text, auto slot) {
        QPushButton* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, slot);
        buttonLayout->addWidget(btn);
    };
    
    addButton("View-Model Relation", &MainWindow::demoViewModelRelation);
    addButton("Shared Selection", &MainWindow::demoSharedSelection);
    addButton("View Components", &MainWindow::demoViewComponents);
    addButton("Proxy Model", &MainWindow::demoProxyModel);
    
    layout->addWidget(controlGroup);
    
    m_tabWidget->addTab(viewTab, "View Pointers");
}

void MainWindow::createDelegateTab()
{
    QWidget* delegateTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(delegateTab);
    
    m_delegateTableView = new QTableView();
    layout->addWidget(m_delegateTableView);
    
    // Control buttons
    QGroupBox* controlGroup = new QGroupBox("Delegate Pointer Demos");
    QHBoxLayout* buttonLayout = new QHBoxLayout(controlGroup);
    
    auto addButton = [&](const QString& text, auto slot) {
        QPushButton* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, slot);
        buttonLayout->addWidget(btn);
    };
    
    addButton("Delegate Lifecycle", &MainWindow::demoDelegateLifecycle);
    addButton("Custom Editors", &MainWindow::demoCustomEditors);
    addButton("Delegate Swapping", &MainWindow::demoDelegateSwapping);
    
    layout->addWidget(controlGroup);
    
    m_tabWidget->addTab(delegateTab, "Delegate Pointers");
    
    // Initialize delegate
    m_customDelegate = std::make_unique<CustomDelegate>();
}

void MainWindow::createMemoryTab()
{
    QWidget* memoryTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(memoryTab);
    
    // Info text
    QTextEdit* infoText = new QTextEdit();
    infoText->setReadOnly(true);
    infoText->setHtml(
        "<h3>Memory Management Best Practices</h3>"
        "<ul>"
        "<li><b>Parent-Child:</b> Qt objects với parent sẽ tự động được delete</li>"
        "<li><b>Smart Pointers:</b> Sử dụng std::unique_ptr, std::shared_ptr</li>"
        "<li><b>QPointer:</b> Tự động null khi object bị delete</li>"
        "<li><b>Model Items:</b> Model sở hữu items - không delete manual</li>"
        "<li><b>View không sở hữu Model:</b> Phải quản lý model lifetime riêng</li>"
        "</ul>"
    );
    layout->addWidget(infoText);
    
    // Control buttons
    QGroupBox* controlGroup = new QGroupBox("Memory Management Demos");
    QHBoxLayout* buttonLayout = new QHBoxLayout(controlGroup);
    
    auto addButton = [&](const QString& text, auto slot) {
        QPushButton* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, slot);
        buttonLayout->addWidget(btn);
    };
    
    addButton("Smart Pointers", &MainWindow::demoSmartPointers);
    addButton("QPointer Safety", &MainWindow::demoQPointer);
    addButton("Common Leaks", &MainWindow::demoMemoryLeaks);
    addButton("Best Practices", &MainWindow::demoBestPractices);
    
    layout->addWidget(controlGroup);
    
    m_tabWidget->addTab(memoryTab, "Memory Management");
    
    // Initialize pointer manager
    m_pointerManager = new PointerManager(this);
}

void MainWindow::connectSignals()
{
    // Clear log button
    QPushButton* clearBtn = new QPushButton("Clear Log");
    statusBar()->addPermanentWidget(clearBtn);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearLog);
}

// Model demo implementations
void MainWindow::demoModelOwnership()
{
    log("\n=== Demo: Model Ownership ===");
    
    // Model without parent - manual management needed
    CustomModel* orphanModel = new CustomModel();
    log(QString("Created orphan model at %1").arg((qintptr)orphanModel, 0, 16));
    
    // Model with parent - automatic cleanup
    CustomModel* ownedModel = new CustomModel(this);
    log(QString("Created owned model at %1 with parent %2")
        .arg((qintptr)ownedModel, 0, 16)
        .arg((qintptr)this, 0, 16));
    
    // Set to view
    m_modelTableView->setModel(orphanModel);
    
    log("QUAN TRỌNG: orphanModel phải được delete manual!");
    log("ownedModel sẽ tự động delete khi MainWindow bị destroy");
    
    // Cleanup orphan để tránh leak
    delete orphanModel;
    m_modelTableView->setModel(nullptr);
}

void MainWindow::demoModelSharing()
{
    log("\n=== Demo: Model Sharing Between Views ===");
    
    // Populate model
    m_standardModel->clear();
    m_standardModel->setHorizontalHeaderLabels({"Shared", "Model", "Data"});
    
    for (int i = 0; i < 5; ++i) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString("Row %1").arg(i));
        row << new QStandardItem(QString("Shared %1").arg(i));
        row << new QStandardItem(QString("Data %1").arg(i));
        m_standardModel->appendRow(row);
    }
    
    // Share model between views
    m_modelTableView->setModel(m_standardModel);
    m_modelTreeView->setModel(m_standardModel);
    
    log("Cùng model được share giữa TableView và TreeView");
    log(QString("Model pointer: %1").arg((qintptr)m_standardModel, 0, 16));
    log(QString("TableView's model: %1").arg((qintptr)m_modelTableView->model(), 0, 16));
    log(QString("TreeView's model: %1").arg((qintptr)m_modelTreeView->model(), 0, 16));
}

void MainWindow::demoItemPointers()
{
    log("\n=== Demo: Item Pointer Management ===");
    
    if (!m_standardModel->rowCount()) {
        demoModelSharing(); // Ensure we have data
    }
    
    // Get item via index
    QModelIndex index = m_standardModel->index(0, 0);
    QStandardItem* item = m_standardModel->itemFromIndex(index);
    
    log(QString("Got item pointer: %1").arg((qintptr)item, 0, 16));
    
    if (item) {
        // Item is owned by model
        log(QString("Item text: %1").arg(item->text()));
        
        // Add child
        QStandardItem* child = new QStandardItem("Child Item");
        item->appendRow(child);
        log("Added child item - model now owns it");
        
        // Expand in tree view
        m_modelTreeView->expand(index);
    }
    
    log("NHỚ: Không delete items đã add vào model!");
}

void MainWindow::demoModelCleanup()
{
    log("\n=== Demo: Model Cleanup Patterns ===");
    
    // Pattern 1: Clear model từ views trước khi delete
    log("Pattern 1: Clear views before deleting model");
    
    QStandardItemModel* tempModel = new QStandardItemModel();
    m_modelTableView->setModel(tempModel);
    
    // Wrong way - commented out
    // delete tempModel; // View still has pointer!
    
    // Right way
    m_modelTableView->setModel(nullptr);
    delete tempModel;
    log("Safely deleted model after clearing from view");
    
    // Pattern 2: Use smart pointers
    log("\nPattern 2: Smart pointer auto cleanup");
    auto smartModel = std::make_unique<QStandardItemModel>();
    m_modelTableView->setModel(smartModel.get());
    // smartModel tự động delete khi out of scope
    
    // Restore original model
    m_modelTableView->setModel(m_standardModel);
}

// View demo implementations
void MainWindow::demoViewModelRelation()
{
    log("\n=== Demo: View-Model Pointer Relationship ===");
    
    // Setup model for views
    if (!m_standardModel->rowCount()) {
        demoModelSharing();
    }
    
    m_viewTable1->setModel(m_standardModel);
    m_viewTable2->setModel(m_standardModel);
    m_viewList->setModel(m_standardModel);
    
    log("All views share the same model:");
    log(QString("Model: %1").arg((qintptr)m_standardModel, 0, 16));
    log(QString("Table1 model: %1").arg((qintptr)m_viewTable1->model(), 0, 16));
    log(QString("Table2 model: %1").arg((qintptr)m_viewTable2->model(), 0, 16));
    log(QString("List model: %1").arg((qintptr)m_viewList->model(), 0, 16));
}

void MainWindow::demoSharedSelection()
{
    log("\n=== Demo: Shared Selection Model ===");
    
    if (!m_viewTable1->model()) {
        demoViewModelRelation();
    }
    
    // Create shared selection model
    if (!m_sharedSelection || !m_sharedSelection->model()) {
        m_sharedSelection = new QItemSelectionModel(m_standardModel, this);
    }
    
    // Share selection between views
    m_viewTable1->setSelectionModel(m_sharedSelection);
    m_viewTable2->setSelectionModel(m_sharedSelection);
    
    log("Table1 và Table2 share selection model");
    log("Select trong một view sẽ update view kia");
    
    // Demo selection
    m_sharedSelection->select(m_standardModel->index(0, 0), 
                             QItemSelectionModel::Select);
}

void MainWindow::demoViewComponents()
{
    log("\n=== Demo: View Component Pointers ===");
    
    QTableView* view = m_viewTable1;
    
    // View owns these components
    QHeaderView* hHeader = view->horizontalHeader();
    QHeaderView* vHeader = view->verticalHeader();
    QWidget* viewport = view->viewport();
    QScrollBar* vScrollBar = view->verticalScrollBar();
    QScrollBar* hScrollBar = view->horizontalScrollBar();
    
    log("View component pointers (all owned by view):");
    log(QString("Horizontal header: %1").arg((qintptr)hHeader, 0, 16));
    log(QString("Vertical header: %1").arg((qintptr)vHeader, 0, 16));
    log(QString("Viewport: %1").arg((qintptr)viewport, 0, 16));
    log(QString("V-ScrollBar: %1").arg((qintptr)vScrollBar, 0, 16));
    log(QString("H-ScrollBar: %1").arg((qintptr)hScrollBar, 0, 16));
    
    log("Tất cả components này tự động delete khi view bị destroy");
}

void MainWindow::demoProxyModel()
{
    log("\n=== Demo: Proxy Model Pointers ===");
    
    if (!m_proxyModel) {
        m_proxyModel = new QSortFilterProxyModel(this);
        m_proxyModel->setSourceModel(m_standardModel);
    }
    
    // Set proxy to view
    m_viewList->setModel(m_proxyModel);
    
    log("Proxy model wraps source model:");
    log(QString("Proxy: %1").arg((qintptr)m_proxyModel, 0, 16));
    log(QString("Source: %1").arg((qintptr)m_proxyModel->sourceModel(), 0, 16));
    
    // Apply filter
    m_proxyModel->setFilterRegularExpression(".*[02468].*");
    log("Applied filter - chỉ hiện rows chứa số chẵn");
}

// Delegate demo implementations
void MainWindow::demoDelegateLifecycle()
{
    log("\n=== Demo: Delegate Lifecycle ===");
    
    // Ensure model
    if (!m_delegateTableView->model()) {
        m_delegateTableView->setModel(m_standardModel);
    }
    
    // Current delegate
    QAbstractItemDelegate* currentDelegate = m_delegateTableView->itemDelegate();
    log(QString("Current delegate: %1").arg((qintptr)currentDelegate, 0, 16));
    
    // Set custom delegate
    m_delegateTableView->setItemDelegate(m_customDelegate.get());
    log(QString("Set custom delegate: %1").arg((qintptr)m_customDelegate.get(), 0, 16));
    
    log("View KHÔNG sở hữu delegate - phải tự quản lý!");
}

void MainWindow::demoCustomEditors()
{
    log("\n=== Demo: Custom Editor Pointers ===");
    log("Double-click cells để xem custom editors");
    log("Editors được tạo với viewport làm parent");
    log("View tự động quản lý editor lifecycle");
    
    // Ensure custom delegate is set
    if (m_delegateTableView->itemDelegate() != m_customDelegate.get()) {
        m_delegateTableView->setItemDelegate(m_customDelegate.get());
    }
}

void MainWindow::demoDelegateSwapping()
{
    log("\n=== Demo: Delegate Swapping ===");
    
    static bool useCustom = true;
    
    if (useCustom) {
        m_delegateTableView->setItemDelegate(m_customDelegate.get());
        log("Switched to custom delegate");
    } else {
        // Create default delegate with view as parent
        QStyledItemDelegate* defaultDelegate = new QStyledItemDelegate(m_delegateTableView);
        m_delegateTableView->setItemDelegate(defaultDelegate);
        log("Switched to default delegate (owned by view)");
    }
    
    useCustom = !useCustom;
}

// Memory management demos
void MainWindow::demoSmartPointers()
{
    log("\n=== Demo: Smart Pointers với Qt Objects ===");
    
    // unique_ptr
    {
        auto model = std::make_unique<QStandardItemModel>();
        log(QString("Created unique_ptr model: %1").arg((qintptr)model.get(), 0, 16));
        
        // Can use with views
        QTableView tempView;
        tempView.setModel(model.get());
        
        log("unique_ptr tự động delete khi out of scope");
    } // model deleted here
    
    // shared_ptr
    {
        auto model = std::make_shared<QStandardItemModel>();
        log(QString("Created shared_ptr model: %1").arg((qintptr)model.get(), 0, 16));
        
        auto model2 = model; // Share ownership
        log(QString("Reference count: %1").arg(model.use_count()));
    }
    
    log("Smart pointers giúp tránh memory leaks");
}

void MainWindow::demoQPointer()
{
    log("\n=== Demo: QPointer Safety ===");
    
    // Create object
    QStandardItemModel* model = new QStandardItemModel();
    QPointer<QStandardItemModel> safePtr = model;
    
    log(QString("Created model: %1").arg((qintptr)model, 0, 16));
    log(QString("QPointer valid: %1").arg(safePtr ? "true" : "false"));
    
    // Delete object
    delete model;
    
    log("After delete:");
    log(QString("QPointer valid: %1").arg(safePtr ? "true" : "false"));
    log("QPointer tự động set null khi object bị delete!");
}

void MainWindow::demoMemoryLeaks()
{
    log("\n=== Demo: Common Memory Leak Patterns ===");
    
    log("Pattern 1: Model không có parent và quên delete");
    log("  QStandardItemModel* model = new QStandardItemModel();");
    log("  // Forgot to delete model - LEAK!");
    
    log("\nPattern 2: View còn giữ pointer đến deleted model");
    log("  delete model;");
    log("  // view->model() is now dangling pointer!");
    
    log("\nPattern 3: Items không được add vào model");
    log("  QStandardItem* item = new QStandardItem();");
    log("  // Forgot to add to model - LEAK!");
    
    log("\nSolution: Luôn set parent hoặc dùng smart pointers!");
}

void MainWindow::demoBestPractices()
{
    log("\n=== Best Practices Tổng Hợp ===");
    
    log("1. MODELS:");
    log("   - Set parent khi possible");
    log("   - Hoặc dùng smart pointers");
    log("   - Clear từ views trước khi delete");
    
    log("\n2. VIEWS:");
    log("   - Luôn có parent widget");
    log("   - Check model != nullptr trước khi dùng");
    log("   - Set model(nullptr) khi cleanup");
    
    log("\n3. DELEGATES:");
    log("   - Delegate cùng lifetime với view");
    log("   - Không store permanent editor pointers");
    log("   - Use qobject_cast cho type safety");
    
    log("\n4. ITEMS:");
    log("   - Model owns items sau khi add");
    log("   - Không delete items manually");
    log("   - Check index.isValid() trước khi access");
    
    log("\n5. GENERAL:");
    log("   - Dùng QPointer cho safety");
    log("   - Debug với valgrind/sanitizers");
    log("   - Log creation/destruction");
}

// Utility functions
void MainWindow::log(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logOutput->append(QString("[%1] %2").arg(timestamp).arg(message));
    
    // Also print to console
    qDebug() << message;
}

void MainWindow::clearLog()
{
    m_logOutput->clear();
    log("Log cleared");
}