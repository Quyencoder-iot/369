// Ví dụ 5: SQL Models - QSqlTableModel, QSqlQueryModel, QSqlRelationalTableModel
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QDebug>

class SqlModelsExample : public QWidget {
    Q_OBJECT

public:
    SqlModelsExample(QWidget *parent = nullptr) : QWidget(parent) {
        if (!createConnection()) {
            QMessageBox::critical(this, "Database Error", 
                "Không thể kết nối database!");
            return;
        }
        
        createTables();
        setupUI();
        setupModels();
        connectSignals();
    }

private:
    bool createConnection() {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:"); // In-memory database for demo
        
        if (!db.open()) {
            qDebug() << "Database error:" << db.lastError().text();
            return false;
        }
        
        return true;
    }
    
    void createTables() {
        QSqlQuery query;
        
        // Bảng categories
        query.exec("CREATE TABLE categories ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "name VARCHAR(50) NOT NULL)");
        
        // Bảng products  
        query.exec("CREATE TABLE products ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "name VARCHAR(100) NOT NULL,"
                  "price INTEGER NOT NULL,"
                  "category_id INTEGER,"
                  "stock INTEGER DEFAULT 0,"
                  "FOREIGN KEY(category_id) REFERENCES categories(id))");
        
        // Thêm data mẫu cho categories
        query.exec("INSERT INTO categories (name) VALUES "
                  "('Điện thoại'), ('Laptop'), ('Tablet'), "
                  "('Phụ kiện'), ('Smartwatch')");
        
        // Thêm data mẫu cho products
        query.exec("INSERT INTO products (name, price, category_id, stock) VALUES "
                  "('iPhone 14 Pro', 30000000, 1, 15),"
                  "('Samsung Galaxy S23', 25000000, 1, 20),"
                  "('MacBook Air M2', 35000000, 2, 8),"
                  "('Dell XPS 13', 28000000, 2, 12),"
                  "('iPad Pro 11', 20000000, 3, 10),"
                  "('AirPods Pro', 6000000, 4, 50),"
                  "('Apple Watch Series 8', 12000000, 5, 25)");
    }
    
    void setupUI() {
        auto *mainLayout = new QVBoxLayout(this);
        
        // Title
        auto *titleLabel = new QLabel("SQL Models Example");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
        mainLayout->addWidget(titleLabel);
        
        // Tab-like layout for different models
        auto *modelsLayout = new QGridLayout();
        
        // 1. QSqlTableModel
        auto *tableModelGroup = new QGroupBox("QSqlTableModel - Direct Table Access");
        auto *tableModelLayout = new QVBoxLayout(tableModelGroup);
        
        tableView = new QTableView();
        tableView->setAlternatingRowColors(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        auto *tableControlLayout = new QHBoxLayout();
        addRowButton = new QPushButton("Thêm hàng");
        deleteRowButton = new QPushButton("Xóa hàng");
        submitButton = new QPushButton("Lưu thay đổi");
        revertButton = new QPushButton("Hủy thay đổi");
        
        tableControlLayout->addWidget(addRowButton);
        tableControlLayout->addWidget(deleteRowButton);
        tableControlLayout->addWidget(submitButton);
        tableControlLayout->addWidget(revertButton);
        tableControlLayout->addStretch();
        
        tableModelLayout->addWidget(tableView);
        tableModelLayout->addLayout(tableControlLayout);
        
        modelsLayout->addWidget(tableModelGroup, 0, 0);
        
        // 2. QSqlQueryModel
        auto *queryModelGroup = new QGroupBox("QSqlQueryModel - Custom Query (Read-Only)");
        auto *queryModelLayout = new QVBoxLayout(queryModelGroup);
        
        queryView = new QTableView();
        queryView->setAlternatingRowColors(true);
        
        auto *queryControlLayout = new QHBoxLayout();
        queryEdit = new QLineEdit();
        queryEdit->setPlaceholderText("Enter SQL query...");
        queryEdit->setText("SELECT p.name, c.name as category, p.price, p.stock "
                          "FROM products p JOIN categories c ON p.category_id = c.id");
        
        executeQueryButton = new QPushButton("Execute Query");
        
        queryControlLayout->addWidget(queryEdit);
        queryControlLayout->addWidget(executeQueryButton);
        
        queryModelLayout->addWidget(queryView);
        queryModelLayout->addLayout(queryControlLayout);
        
        modelsLayout->addWidget(queryModelGroup, 0, 1);
        
        // 3. QSqlRelationalTableModel
        auto *relationalModelGroup = new QGroupBox("QSqlRelationalTableModel - With Foreign Keys");
        auto *relationalModelLayout = new QVBoxLayout(relationalModelGroup);
        
        relationalView = new QTableView();
        relationalView->setAlternatingRowColors(true);
        relationalView->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        auto *relationalControlLayout = new QHBoxLayout();
        categoryFilterCombo = new QComboBox();
        categoryFilterCombo->addItem("Tất cả danh mục");
        
        filterButton = new QPushButton("Lọc theo danh mục");
        clearFilterButton = new QPushButton("Xóa lọc");
        
        relationalControlLayout->addWidget(new QLabel("Danh mục:"));
        relationalControlLayout->addWidget(categoryFilterCombo);
        relationalControlLayout->addWidget(filterButton);
        relationalControlLayout->addWidget(clearFilterButton);
        relationalControlLayout->addStretch();
        
        relationalModelLayout->addWidget(relationalView);
        relationalModelLayout->addLayout(relationalControlLayout);
        
        modelsLayout->addWidget(relationalModelGroup, 1, 0, 1, 2);
        
        mainLayout->addLayout(modelsLayout);
        
        // Info area
        infoText = new QTextEdit();
        infoText->setReadOnly(true);
        infoText->setMaximumHeight(150);
        mainLayout->addWidget(infoText);
        
        setMinimumSize(1200, 800);
        setWindowTitle("SQL Models Demo");
    }
    
    void setupModels() {
        // 1. Setup QSqlTableModel
        tableModel = new QSqlTableModel(this, db);
        tableModel->setTable("products");
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        tableModel->select();
        
        // Set headers cho dễ đọc
        tableModel->setHeaderData(1, Qt::Horizontal, "Tên sản phẩm");
        tableModel->setHeaderData(2, Qt::Horizontal, "Giá (VND)");
        tableModel->setHeaderData(3, Qt::Horizontal, "ID Danh mục");
        tableModel->setHeaderData(4, Qt::Horizontal, "Tồn kho");
        
        tableView->setModel(tableModel);
        tableView->hideColumn(0); // Ẩn cột ID
        
        // 2. Setup QSqlQueryModel
        queryModel = new QSqlQueryModel(this);
        executeQuery();
        queryView->setModel(queryModel);
        
        // 3. Setup QSqlRelationalTableModel
        relationalModel = new QSqlRelationalTableModel(this, db);
        relationalModel->setTable("products");
        relationalModel->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);
        
        // Thiết lập quan hệ foreign key
        relationalModel->setRelation(3, QSqlRelation("categories", "id", "name"));
        
        relationalModel->setHeaderData(1, Qt::Horizontal, "Tên sản phẩm");
        relationalModel->setHeaderData(2, Qt::Horizontal, "Giá (VND)");
        relationalModel->setHeaderData(3, Qt::Horizontal, "Danh mục");
        relationalModel->setHeaderData(4, Qt::Horizontal, "Tồn kho");
        
        relationalModel->select();
        
        relationalView->setModel(relationalModel);
        relationalView->hideColumn(0); // Ẩn cột ID
        
        // Sử dụng delegate để có combo box cho foreign key
        relationalView->setItemDelegate(new QSqlRelationalDelegate(relationalView));
        
        // Populate category filter
        QSqlQuery query("SELECT DISTINCT name FROM categories");
        while (query.next()) {
            categoryFilterCombo->addItem(query.value(0).toString());
        }
        
        updateInfo("Models đã được khởi tạo");
    }
    
    void connectSignals() {
        // QSqlTableModel controls
        connect(addRowButton, &QPushButton::clicked, [this]() {
            int row = tableModel->rowCount();
            tableModel->insertRow(row);
            
            // Set default values
            tableModel->setData(tableModel->index(row, 1), "Sản phẩm mới");
            tableModel->setData(tableModel->index(row, 2), 0);
            tableModel->setData(tableModel->index(row, 3), 1);
            tableModel->setData(tableModel->index(row, 4), 0);
            
            updateInfo("Đã thêm hàng mới. Nhấn 'Lưu thay đổi' để commit.");
        });
        
        connect(deleteRowButton, &QPushButton::clicked, [this]() {
            QModelIndexList selected = tableView->selectionModel()->selectedRows();
            if (selected.isEmpty()) {
                updateInfo("Chọn hàng để xóa");
                return;
            }
            
            for (const QModelIndex &index : selected) {
                tableModel->removeRow(index.row());
            }
            
            updateInfo("Đã xóa hàng. Nhấn 'Lưu thay đổi' để commit.");
        });
        
        connect(submitButton, &QPushButton::clicked, [this]() {
            if (tableModel->submitAll()) {
                updateInfo("Đã lưu thay đổi vào database");
            } else {
                updateInfo("Lỗi: " + tableModel->lastError().text());
            }
        });
        
        connect(revertButton, &QPushButton::clicked, [this]() {
            tableModel->revertAll();
            updateInfo("Đã hủy thay đổi");
        });
        
        // QSqlQueryModel controls  
        connect(executeQueryButton, &QPushButton::clicked, [this]() {
            executeQuery();
        });
        
        connect(queryEdit, &QLineEdit::returnPressed, executeQueryButton, &QPushButton::click);
        
        // QSqlRelationalTableModel filters
        connect(filterButton, &QPushButton::clicked, [this]() {
            QString category = categoryFilterCombo->currentText();
            if (category == "Tất cả danh mục") {
                relationalModel->setFilter("");
            } else {
                // Note: phải dùng tên bảng.cột cho relation
                relationalModel->setFilter(QString("categories.name = '%1'").arg(category));
            }
            relationalModel->select();
            updateInfo("Đã lọc theo danh mục: " + category);
        });
        
        connect(clearFilterButton, &QPushButton::clicked, [this]() {
            relationalModel->setFilter("");
            relationalModel->select();
            categoryFilterCombo->setCurrentIndex(0);
            updateInfo("Đã xóa bộ lọc");
        });
        
        // Monitor changes in relational model
        connect(relationalModel, &QSqlRelationalTableModel::dataChanged,
                [this](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
            updateInfo("Dữ liệu đã được cập nhật tự động");
        });
        
        // Show SQL for learning
        connect(tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
                [this](const QModelIndex &current, const QModelIndex &previous) {
            if (!current.isValid()) return;
            
            QSqlRecord record = tableModel->record(current.row());
            QString info = "Selected Record:\n";
            for (int i = 0; i < record.count(); ++i) {
                info += QString("%1: %2\n").arg(record.fieldName(i))
                                           .arg(record.value(i).toString());
            }
            updateInfo(info);
        });
    }
    
    void executeQuery() {
        QString sql = queryEdit->text();
        queryModel->setQuery(sql, db);
        
        if (queryModel->lastError().isValid()) {
            updateInfo("Query Error: " + queryModel->lastError().text());
        } else {
            updateInfo(QString("Query executed. Rows: %1").arg(queryModel->rowCount()));
            
            // Update headers for custom query
            queryModel->setHeaderData(0, Qt::Horizontal, "Sản phẩm");
            queryModel->setHeaderData(1, Qt::Horizontal, "Danh mục");
            queryModel->setHeaderData(2, Qt::Horizontal, "Giá");
            queryModel->setHeaderData(3, Qt::Horizontal, "Tồn kho");
        }
    }
    
    void updateInfo(const QString &text) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        infoText->append(QString("[%1] %2").arg(timestamp).arg(text));
        
        // Auto scroll to bottom
        QTextCursor cursor = infoText->textCursor();
        cursor.movePosition(QTextCursor::End);
        infoText->setTextCursor(cursor);
        
        qDebug() << text;
    }

private:
    QSqlDatabase db;
    
    // Models
    QSqlTableModel *tableModel;
    QSqlQueryModel *queryModel;
    QSqlRelationalTableModel *relationalModel;
    
    // Views
    QTableView *tableView;
    QTableView *queryView;
    QTableView *relationalView;
    
    // Controls
    QPushButton *addRowButton;
    QPushButton *deleteRowButton;
    QPushButton *submitButton;
    QPushButton *revertButton;
    
    QLineEdit *queryEdit;
    QPushButton *executeQueryButton;
    
    QComboBox *categoryFilterCombo;
    QPushButton *filterButton;
    QPushButton *clearFilterButton;
    
    QTextEdit *infoText;
};

#include "5_sql_models.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // SQL Models trong Qt
    
    // 1. QSqlQueryModel
    // - Read-only model cho custom queries
    // - Flexible nhất nhưng không edit được
    // - Use case: Reports, complex joins
    
    // 2. QSqlTableModel  
    // - Read/write cho một table
    // - Support INSERT, UPDATE, DELETE
    // - 3 edit strategies:
    //   - OnFieldChange: Tự động save khi edit
    //   - OnRowChange: Save khi chuyển row
    //   - OnManualSubmit: Save thủ công
    
    // 3. QSqlRelationalTableModel
    // - Extends QSqlTableModel
    // - Handle foreign keys tự động
    // - Hiển thị lookup values thay vì IDs
    // - Perfect cho forms với dropdowns
    
    // Key concepts:
    // - submitAll(): Commit changes to DB
    // - revertAll(): Rollback changes
    // - setFilter(): WHERE clause
    // - setSort(): ORDER BY
    // - record(): Get QSqlRecord at row
    
    SqlModelsExample window;
    window.show();
    
    return app.exec();
}