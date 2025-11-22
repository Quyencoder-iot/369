// Ví dụ 6: Complete Demo - Ứng dụng quản lý sản phẩm tổng hợp
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QTableView>
#include <QListView>
#include <QTreeView>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDataWidgetMapper>
#include <QFormLayout>
#include <QSpinBox>
#include <QDebug>
#include <QMessageBox>

// Main application window combining all model types
class ProductManagementApp : public QMainWindow {
    Q_OBJECT

public:
    ProductManagementApp(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupDatabase();
        setupUI();
        setupModels();
        setupConnections();
        
        // Load initial data
        loadCategories();
        statusBar()->showMessage("Ứng dụng đã sẵn sàng", 3000);
    }

private:
    void setupDatabase() {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("products.db");
        
        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", 
                "Không thể mở database: " + db.lastError().text());
            return;
        }
        
        // Create tables if not exists
        QSqlQuery query;
        
        // Categories table
        query.exec("CREATE TABLE IF NOT EXISTS categories ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "name VARCHAR(50) UNIQUE NOT NULL,"
                  "description TEXT)");
        
        // Products table
        query.exec("CREATE TABLE IF NOT EXISTS products ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "name VARCHAR(100) NOT NULL,"
                  "price INTEGER NOT NULL,"
                  "category_id INTEGER,"
                  "stock INTEGER DEFAULT 0,"
                  "description TEXT,"
                  "FOREIGN KEY(category_id) REFERENCES categories(id))");
        
        // Tags table (for many-to-many demo)
        query.exec("CREATE TABLE IF NOT EXISTS tags ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "name VARCHAR(30) UNIQUE NOT NULL)");
        
        // Product-Tags junction table
        query.exec("CREATE TABLE IF NOT EXISTS product_tags ("
                  "product_id INTEGER,"
                  "tag_id INTEGER,"
                  "PRIMARY KEY(product_id, tag_id),"
                  "FOREIGN KEY(product_id) REFERENCES products(id),"
                  "FOREIGN KEY(tag_id) REFERENCES tags(id))");
        
        // Insert sample data if tables are empty
        query.exec("SELECT COUNT(*) FROM categories");
        if (query.next() && query.value(0).toInt() == 0) {
            insertSampleData();
        }
    }
    
    void insertSampleData() {
        QSqlQuery query;
        
        // Categories
        query.exec("INSERT INTO categories (name, description) VALUES "
                  "('Điện thoại', 'Smartphone và điện thoại di động'),"
                  "('Laptop', 'Máy tính xách tay'),"
                  "('Phụ kiện', 'Phụ kiện điện tử')");
        
        // Products
        query.exec("INSERT INTO products (name, price, category_id, stock, description) VALUES "
                  "('iPhone 14', 25000000, 1, 10, 'Latest iPhone model'),"
                  "('Samsung S23', 20000000, 1, 15, 'Android flagship'),"
                  "('MacBook Air', 35000000, 2, 5, 'M2 chip laptop'),"
                  "('AirPods Pro', 6000000, 3, 30, 'Wireless earbuds')");
        
        // Tags
        query.exec("INSERT INTO tags (name) VALUES "
                  "('New'), ('Bestseller'), ('Sale'), ('Premium')");
    }
    
    void setupUI() {
        // Central widget with tabs
        auto *centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        auto *mainLayout = new QVBoxLayout(centralWidget);
        
        // Search bar at top
        auto *searchLayout = new QHBoxLayout();
        globalSearchEdit = new QLineEdit();
        globalSearchEdit->setPlaceholderText("Tìm kiếm toàn cục...");
        searchLayout->addWidget(new QLabel("Tìm kiếm:"));
        searchLayout->addWidget(globalSearchEdit);
        mainLayout->addLayout(searchLayout);
        
        // Tab widget for different views
        tabWidget = new QTabWidget();
        
        // Tab 1: Product Management (SQL Models)
        tabWidget->addTab(createProductTab(), "Quản lý Sản phẩm");
        
        // Tab 2: Category Tree (Standard Item Model)
        tabWidget->addTab(createCategoryTab(), "Danh mục (Tree)");
        
        // Tab 3: Tags (String List Model)
        tabWidget->addTab(createTagsTab(), "Tags");
        
        // Tab 4: Reports (Query Model)
        tabWidget->addTab(createReportsTab(), "Báo cáo");
        
        mainLayout->addWidget(tabWidget);
        
        // Create menus and toolbars
        createMenus();
        createToolBar();
        
        // Create dock widgets
        createDockWidgets();
        
        setWindowTitle("Product Management System - All Models Demo");
        resize(1400, 900);
    }
    
    QWidget* createProductTab() {
        auto *widget = new QWidget();
        auto *layout = new QHBoxLayout(widget);
        
        // Left: Product table with filter
        auto *leftPanel = new QVBoxLayout();
        
        // Filter controls
        auto *filterGroup = new QGroupBox("Bộ lọc");
        auto *filterLayout = new QHBoxLayout(filterGroup);
        
        categoryFilterCombo = new QComboBox();
        categoryFilterCombo->addItem("Tất cả danh mục");
        
        priceFilterCombo = new QComboBox();
        priceFilterCombo->addItems({"Tất cả giá", "< 10 triệu", "10-30 triệu", "> 30 triệu"});
        
        filterLayout->addWidget(new QLabel("Danh mục:"));
        filterLayout->addWidget(categoryFilterCombo);
        filterLayout->addWidget(new QLabel("Giá:"));
        filterLayout->addWidget(priceFilterCombo);
        
        leftPanel->addWidget(filterGroup);
        
        // Product table
        productTableView = new QTableView();
        productTableView->setAlternatingRowColors(true);
        productTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        leftPanel->addWidget(productTableView);
        
        // Right: Product form
        auto *rightPanel = new QVBoxLayout();
        
        auto *formGroup = new QGroupBox("Chi tiết sản phẩm");
        auto *formLayout = new QFormLayout(formGroup);
        
        nameEdit = new QLineEdit();
        priceSpinBox = new QSpinBox();
        priceSpinBox->setRange(0, 999999999);
        priceSpinBox->setSingleStep(1000000);
        categoryCombo = new QComboBox();
        stockSpinBox = new QSpinBox();
        stockSpinBox->setRange(0, 9999);
        descriptionEdit = new QTextEdit();
        descriptionEdit->setMaximumHeight(100);
        
        formLayout->addRow("Tên:", nameEdit);
        formLayout->addRow("Giá:", priceSpinBox);
        formLayout->addRow("Danh mục:", categoryCombo);
        formLayout->addRow("Tồn kho:", stockSpinBox);
        formLayout->addRow("Mô tả:", descriptionEdit);
        
        rightPanel->addWidget(formGroup);
        
        // Form buttons
        auto *buttonLayout = new QHBoxLayout();
        saveButton = new QPushButton("Lưu");
        newButton = new QPushButton("Thêm mới");
        deleteButton = new QPushButton("Xóa");
        
        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(newButton);
        buttonLayout->addWidget(deleteButton);
        rightPanel->addLayout(buttonLayout);
        
        rightPanel->addStretch();
        
        // Splitter
        auto *splitter = new QSplitter(Qt::Horizontal);
        auto *leftWidget = new QWidget();
        leftWidget->setLayout(leftPanel);
        auto *rightWidget = new QWidget();
        rightWidget->setLayout(rightPanel);
        
        splitter->addWidget(leftWidget);
        splitter->addWidget(rightWidget);
        splitter->setStretchFactor(0, 2);
        splitter->setStretchFactor(1, 1);
        
        layout->addWidget(splitter);
        
        return widget;
    }
    
    QWidget* createCategoryTab() {
        auto *widget = new QWidget();
        auto *layout = new QVBoxLayout(widget);
        
        layout->addWidget(new QLabel("Category Tree với QStandardItemModel"));
        
        categoryTreeView = new QTreeView();
        categoryTreeView->setAlternatingRowColors(true);
        categoryTreeView->setAnimated(true);
        layout->addWidget(categoryTreeView);
        
        // Controls
        auto *controlLayout = new QHBoxLayout();
        addCategoryButton = new QPushButton("Thêm danh mục");
        removeCategoryButton = new QPushButton("Xóa danh mục");
        
        controlLayout->addWidget(addCategoryButton);
        controlLayout->addWidget(removeCategoryButton);
        controlLayout->addStretch();
        
        layout->addLayout(controlLayout);
        
        return widget;
    }
    
    QWidget* createTagsTab() {
        auto *widget = new QWidget();
        auto *layout = new QHBoxLayout(widget);
        
        // Left: Available tags
        auto *availableGroup = new QGroupBox("Tags có sẵn");
        auto *availableLayout = new QVBoxLayout(availableGroup);
        
        tagListView = new QListView();
        tagListView->setEditTriggers(QAbstractItemView::DoubleClicked);
        availableLayout->addWidget(tagListView);
        
        auto *tagControlLayout = new QHBoxLayout();
        tagEdit = new QLineEdit();
        tagEdit->setPlaceholderText("Tag mới...");
        addTagButton = new QPushButton("Thêm");
        removeTagButton = new QPushButton("Xóa");
        
        tagControlLayout->addWidget(tagEdit);
        tagControlLayout->addWidget(addTagButton);
        tagControlLayout->addWidget(removeTagButton);
        
        availableLayout->addLayout(tagControlLayout);
        
        // Right: Product tags
        auto *productTagsGroup = new QGroupBox("Tags của sản phẩm hiện tại");
        auto *productTagsLayout = new QVBoxLayout(productTagsGroup);
        
        productTagsView = new QListView();
        productTagsLayout->addWidget(productTagsView);
        
        layout->addWidget(availableGroup);
        layout->addWidget(productTagsGroup);
        
        return widget;
    }
    
    QWidget* createReportsTab() {
        auto *widget = new QWidget();
        auto *layout = new QVBoxLayout(widget);
        
        // Report selection
        auto *reportLayout = new QHBoxLayout();
        reportCombo = new QComboBox();
        reportCombo->addItems({
            "Tổng quan sản phẩm",
            "Sản phẩm theo danh mục",
            "Sản phẩm sắp hết hàng",
            "Top 10 sản phẩm đắt nhất"
        });
        
        executeReportButton = new QPushButton("Chạy báo cáo");
        
        reportLayout->addWidget(new QLabel("Chọn báo cáo:"));
        reportLayout->addWidget(reportCombo);
        reportLayout->addWidget(executeReportButton);
        reportLayout->addStretch();
        
        layout->addLayout(reportLayout);
        
        // Report view
        reportTableView = new QTableView();
        reportTableView->setAlternatingRowColors(true);
        layout->addWidget(reportTableView);
        
        return widget;
    }
    
    void createMenus() {
        // File menu
        QMenu *fileMenu = menuBar()->addMenu("&File");
        
        QAction *refreshAction = new QAction("&Refresh", this);
        refreshAction->setShortcut(QKeySequence::Refresh);
        connect(refreshAction, &QAction::triggered, [this]() {
            refreshAllModels();
            statusBar()->showMessage("Đã refresh dữ liệu", 2000);
        });
        fileMenu->addAction(refreshAction);
        
        fileMenu->addSeparator();
        
        QAction *exitAction = new QAction("&Exit", this);
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(exitAction);
        
        // View menu
        QMenu *viewMenu = menuBar()->addMenu("&View");
        viewMenu->addAction(logDock->toggleViewAction());
    }
    
    void createToolBar() {
        QToolBar *toolbar = addToolBar("Main");
        
        toolbar->addAction(QIcon(), "Refresh", [this]() {
            refreshAllModels();
        });
        
        toolbar->addSeparator();
        
        // Quick filter
        toolbar->addWidget(new QLabel(" Quick Filter: "));
        auto *quickFilterEdit = new QLineEdit();
        quickFilterEdit->setPlaceholderText("Filter products...");
        quickFilterEdit->setMaximumWidth(200);
        toolbar->addWidget(quickFilterEdit);
        
        connect(quickFilterEdit, &QLineEdit::textChanged, [this](const QString &text) {
            if (productProxyModel) {
                productProxyModel->setFilterFixedString(text);
            }
        });
    }
    
    void createDockWidgets() {
        // Log dock
        logDock = new QDockWidget("Event Log", this);
        logTextEdit = new QTextEdit();
        logTextEdit->setReadOnly(true);
        logTextEdit->setMaximumHeight(200);
        logDock->setWidget(logTextEdit);
        addDockWidget(Qt::BottomDockWidgetArea, logDock);
    }
    
    void setupModels() {
        // 1. Product table with relational model
        productModel = new QSqlRelationalTableModel(this, db);
        productModel->setTable("products");
        productModel->setRelation(3, QSqlRelation("categories", "id", "name"));
        productModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
        productModel->select();
        
        productModel->setHeaderData(1, Qt::Horizontal, "Tên sản phẩm");
        productModel->setHeaderData(2, Qt::Horizontal, "Giá");
        productModel->setHeaderData(3, Qt::Horizontal, "Danh mục");
        productModel->setHeaderData(4, Qt::Horizontal, "Tồn kho");
        productModel->setHeaderData(5, Qt::Horizontal, "Mô tả");
        
        // Proxy model for filtering
        productProxyModel = new QSortFilterProxyModel(this);
        productProxyModel->setSourceModel(productModel);
        productProxyModel->setFilterKeyColumn(-1);
        
        productTableView->setModel(productProxyModel);
        productTableView->hideColumn(0); // Hide ID
        productTableView->setItemDelegate(new QSqlRelationalDelegate(productTableView));
        
        // Data mapper for form
        mapper = new QDataWidgetMapper(this);
        mapper->setModel(productModel);
        mapper->setItemDelegate(new QSqlRelationalDelegate(this));
        mapper->addMapping(nameEdit, 1);
        mapper->addMapping(priceSpinBox, 2);
        mapper->addMapping(categoryCombo, 3);
        mapper->addMapping(stockSpinBox, 4);
        mapper->addMapping(descriptionEdit, 5);
        
        // 2. Category tree model
        categoryTreeModel = new QStandardItemModel(this);
        categoryTreeModel->setHorizontalHeaderLabels({"Danh mục", "Số sản phẩm"});
        
        categoryTreeView->setModel(categoryTreeModel);
        
        // 3. Tag list model
        tagListModel = new QStringListModel(this);
        tagListView->setModel(tagListModel);
        
        // 4. Query model for reports
        reportModel = new QSqlQueryModel(this);
        reportTableView->setModel(reportModel);
        
        // Load initial data
        refreshAllModels();
    }
    
    void setupConnections() {
        // Product selection
        connect(productTableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
                [this](const QModelIndex &current, const QModelIndex &previous) {
            if (!current.isValid()) return;
            
            QModelIndex sourceIndex = productProxyModel->mapToSource(current);
            mapper->setCurrentModelIndex(sourceIndex);
            
            // Log event
            QString productName = productModel->data(
                productModel->index(sourceIndex.row(), 1)).toString();
            log(QString("Đã chọn sản phẩm: %1").arg(productName));
        });
        
        // Save button
        connect(saveButton, &QPushButton::clicked, [this]() {
            mapper->submit();
            if (productModel->submitAll()) {
                log("Đã lưu thay đổi");
            } else {
                log("Lỗi: " + productModel->lastError().text());
                productModel->revertAll();
            }
        });
        
        // New button
        connect(newButton, &QPushButton::clicked, [this]() {
            int row = productModel->rowCount();
            productModel->insertRow(row);
            mapper->setCurrentIndex(row);
            
            nameEdit->setFocus();
            log("Thêm sản phẩm mới");
        });
        
        // Delete button
        connect(deleteButton, &QPushButton::clicked, [this]() {
            int row = mapper->currentIndex();
            if (row < 0) return;
            
            if (QMessageBox::question(this, "Xác nhận", 
                "Bạn có chắc muốn xóa sản phẩm này?") == QMessageBox::Yes) {
                productModel->removeRow(row);
                productModel->submitAll();
                log("Đã xóa sản phẩm");
            }
        });
        
        // Category filter
        connect(categoryFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this](int index) {
            if (index == 0) {
                productProxyModel->setFilterFixedString("");
            } else {
                QString category = categoryFilterCombo->currentText();
                productProxyModel->setFilterKeyColumn(3);
                productProxyModel->setFilterFixedString(category);
            }
        });
        
        // Price filter
        connect(priceFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this](int index) {
            // Custom filter implementation needed
            log(QString("Price filter changed to: %1").arg(priceFilterCombo->currentText()));
        });
        
        // Tags
        connect(addTagButton, &QPushButton::clicked, [this]() {
            QString tag = tagEdit->text().trimmed();
            if (tag.isEmpty()) return;
            
            QStringList tags = tagListModel->stringList();
            if (!tags.contains(tag)) {
                tags.append(tag);
                tagListModel->setStringList(tags);
                
                // Save to database
                QSqlQuery query;
                query.prepare("INSERT INTO tags (name) VALUES (?)");
                query.addBindValue(tag);
                query.exec();
                
                tagEdit->clear();
                log("Đã thêm tag: " + tag);
            }
        });
        
        // Reports
        connect(executeReportButton, &QPushButton::clicked, [this]() {
            executeReport();
        });
        
        // Global search
        connect(globalSearchEdit, &QLineEdit::textChanged, [this](const QString &text) {
            productProxyModel->setFilterFixedString(text);
            productProxyModel->setFilterKeyColumn(-1); // Search all columns
            log("Tìm kiếm: " + text);
        });
    }
    
    void loadCategories() {
        // Load categories for combo box
        categoryFilterCombo->clear();
        categoryFilterCombo->addItem("Tất cả danh mục");
        
        QSqlQuery query("SELECT id, name FROM categories");
        while (query.next()) {
            categoryFilterCombo->addItem(query.value(1).toString());
        }
        
        // Update category tree
        categoryTreeModel->clear();
        categoryTreeModel->setHorizontalHeaderLabels({"Danh mục", "Số sản phẩm"});
        
        auto *rootItem = categoryTreeModel->invisibleRootItem();
        
        query.exec("SELECT c.id, c.name, COUNT(p.id) "
                  "FROM categories c "
                  "LEFT JOIN products p ON c.id = p.category_id "
                  "GROUP BY c.id");
        
        while (query.next()) {
            auto *categoryItem = new QStandardItem(query.value(1).toString());
            categoryItem->setData(query.value(0), Qt::UserRole); // Store ID
            
            auto *countItem = new QStandardItem(query.value(2).toString());
            countItem->setEditable(false);
            
            rootItem->appendRow({categoryItem, countItem});
        }
        
        // Load tags
        QStringList tags;
        query.exec("SELECT name FROM tags");
        while (query.next()) {
            tags.append(query.value(0).toString());
        }
        tagListModel->setStringList(tags);
    }
    
    void executeReport() {
        int reportIndex = reportCombo->currentIndex();
        QString sql;
        
        switch (reportIndex) {
            case 0: // Overview
                sql = "SELECT p.name as 'Sản phẩm', c.name as 'Danh mục', "
                     "p.price as 'Giá', p.stock as 'Tồn kho' "
                     "FROM products p JOIN categories c ON p.category_id = c.id";
                break;
                
            case 1: // By category
                sql = "SELECT c.name as 'Danh mục', COUNT(*) as 'Số sản phẩm', "
                     "AVG(p.price) as 'Giá trung bình' "
                     "FROM categories c LEFT JOIN products p ON c.id = p.category_id "
                     "GROUP BY c.id";
                break;
                
            case 2: // Low stock
                sql = "SELECT name as 'Sản phẩm', stock as 'Tồn kho' "
                     "FROM products WHERE stock < 10 ORDER BY stock";
                break;
                
            case 3: // Top 10 expensive
                sql = "SELECT name as 'Sản phẩm', price as 'Giá' "
                     "FROM products ORDER BY price DESC LIMIT 10";
                break;
        }
        
        reportModel->setQuery(sql, db);
        log(QString("Đã chạy báo cáo: %1").arg(reportCombo->currentText()));
    }
    
    void refreshAllModels() {
        productModel->select();
        loadCategories();
        log("Đã refresh tất cả models");
    }
    
    void log(const QString &message) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
    }

private:
    // Database
    QSqlDatabase db;
    
    // Models
    QSqlRelationalTableModel *productModel;
    QSortFilterProxyModel *productProxyModel;
    QStandardItemModel *categoryTreeModel;
    QStringListModel *tagListModel;
    QSqlQueryModel *reportModel;
    
    // Views
    QTableView *productTableView;
    QTreeView *categoryTreeView;
    QListView *tagListView;
    QListView *productTagsView;
    QTableView *reportTableView;
    
    // UI Elements
    QTabWidget *tabWidget;
    QLineEdit *globalSearchEdit;
    
    // Product form
    QLineEdit *nameEdit;
    QSpinBox *priceSpinBox;
    QComboBox *categoryCombo;
    QSpinBox *stockSpinBox;
    QTextEdit *descriptionEdit;
    QPushButton *saveButton;
    QPushButton *newButton;
    QPushButton *deleteButton;
    QDataWidgetMapper *mapper;
    
    // Filters
    QComboBox *categoryFilterCombo;
    QComboBox *priceFilterCombo;
    
    // Category controls
    QPushButton *addCategoryButton;
    QPushButton *removeCategoryButton;
    
    // Tag controls
    QLineEdit *tagEdit;
    QPushButton *addTagButton;
    QPushButton *removeTagButton;
    
    // Report controls
    QComboBox *reportCombo;
    QPushButton *executeReportButton;
    
    // Log
    QDockWidget *logDock;
    QTextEdit *logTextEdit;
};

#include "6_complete_demo.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Complete Demo showcasing:
    // 1. QSqlRelationalTableModel - Product management with foreign keys
    // 2. QSortFilterProxyModel - Filtering and searching
    // 3. QStandardItemModel - Category tree structure
    // 4. QStringListModel - Simple tag management
    // 5. QSqlQueryModel - Reports and analytics
    // 6. QDataWidgetMapper - Form binding
    
    // Key patterns demonstrated:
    // - Model stacking (source -> proxy -> view)
    // - Multiple views of same data
    // - Form-table synchronization
    // - Mix of different model types
    // - Real-world CRUD operations
    
    ProductManagementApp window;
    window.show();
    
    return app.exec();
}