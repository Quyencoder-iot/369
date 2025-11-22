// Ví dụ tổng hợp: SQL Model → Proxy Model → View
// Minh họa mối quan hệ giữa các components
#include <QApplication>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // ============================================
    // BƯỚC 1: Kết nối Database (Data Source)
    // ============================================
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    
    if (!db.open()) {
        qDebug() << "Cannot open database";
        return -1;
    }
    
    // Tạo bảng và dữ liệu mẫu
    QSqlQuery query;
    query.exec("CREATE TABLE products ("
               "id INTEGER PRIMARY KEY, "
               "name TEXT NOT NULL, "
               "category TEXT, "
               "price REAL, "
               "stock INTEGER)");
    
    query.exec("INSERT INTO products VALUES "
               "(1, 'Laptop', 'Electronics', 999.99, 10), "
               "(2, 'Book', 'Education', 19.99, 50), "
               "(3, 'Mouse', 'Electronics', 29.99, 30), "
               "(4, 'Pen', 'Office', 2.99, 100), "
               "(5, 'Tablet', 'Electronics', 499.99, 15)");
    
    // ============================================
    // BƯỚC 2: Tạo SQL Model (Source Model)
    // ============================================
    QSqlTableModel *sqlModel = new QSqlTableModel();
    sqlModel->setTable("products");
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Set header names
    sqlModel->setHeaderData(0, Qt::Horizontal, "ID");
    sqlModel->setHeaderData(1, Qt::Horizontal, "Tên sản phẩm");
    sqlModel->setHeaderData(2, Qt::Horizontal, "Danh mục");
    sqlModel->setHeaderData(3, Qt::Horizontal, "Giá");
    sqlModel->setHeaderData(4, Qt::Horizontal, "Tồn kho");
    
    // Load dữ liệu từ database
    sqlModel->select();
    
    qDebug() << "SQL Model rows:" << sqlModel->rowCount();
    
    // ============================================
    // BƯỚC 3: Tạo Proxy Model (Filter/Sort)
    // ============================================
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(sqlModel);  // Kết nối với source model
    
    // Sắp xếp theo cột giá (giảm dần)
    proxyModel->sort(3, Qt::DescendingOrder);
    
    // Lọc theo danh mục (mặc định: Electronics)
    proxyModel->setFilterKeyColumn(2);  // Cột category
    proxyModel->setFilterRegExp(QRegExp("Electronics", Qt::CaseInsensitive));
    
    qDebug() << "Proxy Model rows:" << proxyModel->rowCount();
    
    // ============================================
    // BƯỚC 4: Tạo View (Hiển thị)
    // ============================================
    QTableView *view = new QTableView();
    view->setModel(proxyModel);  // Kết nối với proxy model (KHÔNG phải sqlModel!)
    
    // Cấu hình view
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setAlternatingRowColors(true);
    view->horizontalHeader()->setStretchLastSection(true);
    view->resizeColumnsToContents();
    
    // ============================================
    // BƯỚC 5: Tạo UI Controls
    // ============================================
    QLineEdit *filterEdit = new QLineEdit();
    filterEdit->setPlaceholderText("Lọc theo danh mục (ví dụ: Electronics, Office)...");
    filterEdit->setText("Electronics");  // Giá trị mặc định
    
    QPushButton *sortButton = new QPushButton("Sắp xếp theo giá");
    QPushButton *resetButton = new QPushButton("Reset filter");
    
    QLabel *infoLabel = new QLabel();
    infoLabel->setText(QString("Hiển thị %1/%2 sản phẩm")
                      .arg(proxyModel->rowCount())
                      .arg(sqlModel->rowCount()));
    
    // ============================================
    // BƯỚC 6: Kết nối Signals/Slots
    // ============================================
    // Filter khi người dùng nhập
    QObject::connect(filterEdit, &QLineEdit::textChanged, 
                     [proxyModel, infoLabel, sqlModel](const QString &text) {
        if (text.isEmpty()) {
            proxyModel->setFilterRegExp(QRegExp(".*"));
        } else {
            proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
        }
        infoLabel->setText(QString("Hiển thị %1/%2 sản phẩm")
                          .arg(proxyModel->rowCount())
                          .arg(sqlModel->rowCount()));
    });
    
    // Sắp xếp khi click button
    QObject::connect(sortButton, &QPushButton::clicked, [proxyModel]() {
        static bool ascending = true;
        proxyModel->sort(3, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
        ascending = !ascending;
    });
    
    // Reset filter
    QObject::connect(resetButton, &QPushButton::clicked, 
                     [filterEdit, proxyModel]() {
        filterEdit->clear();
        proxyModel->setFilterRegExp(QRegExp(".*"));
    });
    
    // ============================================
    // BƯỚC 7: Layout và hiển thị
    // ============================================
    QWidget window;
    window.setWindowTitle("Ví dụ Qt Models - SQL → Proxy → View");
    window.resize(800, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(new QLabel("Luồng dữ liệu: Database → SQL Model → Proxy Model → View"));
    layout->addWidget(filterEdit);
    layout->addWidget(sortButton);
    layout->addWidget(resetButton);
    layout->addWidget(infoLabel);
    layout->addWidget(view);
    
    window.show();
    
    return app.exec();
}

/*
TÓM TẮT KIẾN TRÚC:

┌─────────────────────────────────────────────────┐
│              DATABASE (SQLite)                  │
│  products: id, name, category, price, stock     │
└──────────────────┬──────────────────────────────┘
                   │
                   │ SELECT * FROM products
                   │
┌──────────────────▼──────────────────────────────┐
│         QSqlTableModel (Source Model)           │
│  - setTable("products")                         │
│  - select() → load từ DB                        │
│  - 5 rows                                       │
└──────────────────┬──────────────────────────────┘
                   │
                   │ setSourceModel(sqlModel)
                   │
┌──────────────────▼──────────────────────────────┐
│      QSortFilterProxyModel (Proxy Model)        │
│  - Filter: category = "Electronics"             │
│  - Sort: price DESC                             │
│  - 3 rows (sau khi filter)                      │
└──────────────────┬──────────────────────────────┘
                   │
                   │ setModel(proxyModel)
                   │
┌──────────────────▼──────────────────────────────┐
│            QTableView (View)                    │
│  - Hiển thị 3 rows                              │
│  - User có thể tương tác                        │
└─────────────────────────────────────────────────┘

QUAN TRỌNG:
- View chỉ biết về Proxy Model, không biết về SQL Model
- Proxy Model chuyển đổi index giữa proxy và source
- SQL Model quản lý kết nối với database
- Khi filter thay đổi → Proxy Model tự động cập nhật → View tự động refresh
*/
