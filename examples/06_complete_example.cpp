/**
 * Ví dụ 6: Ví Dụ Hoàn Chỉnh - Kết Hợp Tất Cả
 * 
 * Mục đích: Xem cách các model hoạt động cùng nhau
 */

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QListView>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setupDatabase();
        setupUI();
    }

private slots:
    void onSearchTextChanged(const QString &text)
    {
        // Proxy model tự động lọc khi text thay đổi
        proxyModel->setFilterFixedString(text);
    }

    void onRefreshClicked()
    {
        // Reload dữ liệu từ database
        sqlModel->select();
        statusLabel->setText("Đã làm mới dữ liệu");
    }

    void onSaveClicked()
    {
        // Lưu tất cả thay đổi vào database
        if (sqlModel->submitAll()) {
            QMessageBox::information(this, "Thành công", "Đã lưu dữ liệu!");
            statusLabel->setText("Đã lưu dữ liệu");
        } else {
            QMessageBox::warning(this, "Lỗi", "Không thể lưu: " + sqlModel->lastError().text());
            statusLabel->setText("Lỗi: " + sqlModel->lastError().text());
        }
    }

private:
    void setupDatabase()
    {
        // Kết nối SQLite database
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:"); // Database trong memory

        if (!db.open()) {
            QMessageBox::critical(this, "Lỗi", "Không thể mở database!");
            return;
        }

        // Tạo bảng mẫu
        QSqlQuery query;
        query.exec("CREATE TABLE employees ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "code TEXT,"
                   "name TEXT,"
                   "department TEXT,"
                   "salary INTEGER)");

        // Thêm dữ liệu mẫu
        query.exec("INSERT INTO employees (code, name, department, salary) VALUES "
                   "('NV001', 'Nguyễn Văn A', 'IT', 5000000),"
                   "('NV002', 'Trần Thị B', 'HR', 4500000),"
                   "('NV003', 'Lê Văn C', 'IT', 5500000),"
                   "('NV004', 'Phạm Thị D', 'Sales', 4000000)");

        // Tạo SQL Model
        sqlModel = new QSqlTableModel(this);
        sqlModel->setTable("employees");
        sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        sqlModel->select();

        // Đặt tên header
        sqlModel->setHeaderData(0, Qt::Horizontal, "ID");
        sqlModel->setHeaderData(1, Qt::Horizontal, "Mã NV");
        sqlModel->setHeaderData(2, Qt::Horizontal, "Tên");
        sqlModel->setHeaderData(3, Qt::Horizontal, "Phòng ban");
        sqlModel->setHeaderData(4, Qt::Horizontal, "Lương");

        // Tạo Proxy Model
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(sqlModel);
        proxyModel->setFilterKeyColumn(2); // Lọc theo cột "Tên"
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    void setupUI()
    {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Thanh tìm kiếm
        QHBoxLayout *searchLayout = new QHBoxLayout();
        searchLayout->addWidget(new QLabel("Tìm kiếm:"));
        
        QLineEdit *searchBox = new QLineEdit();
        searchBox->setPlaceholderText("Nhập tên để tìm kiếm...");
        connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
        searchLayout->addWidget(searchBox);

        QPushButton *refreshBtn = new QPushButton("Làm mới");
        connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
        searchLayout->addWidget(refreshBtn);

        QPushButton *saveBtn = new QPushButton("Lưu");
        connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
        searchLayout->addWidget(saveBtn);

        mainLayout->addLayout(searchLayout);

        // Table View
        QTableView *tableView = new QTableView();
        tableView->setModel(proxyModel); // ← Quan trọng: dùng proxy, không dùng sqlModel trực tiếp
        tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->resizeColumnsToContents();
        mainLayout->addWidget(tableView);

        // Status label
        statusLabel = new QLabel("Sẵn sàng");
        mainLayout->addWidget(statusLabel);

        setWindowTitle("Ví Dụ Hoàn Chỉnh - Qt Model/View");
        resize(800, 600);
    }

    QSqlTableModel *sqlModel;
    QSortFilterProxyModel *proxyModel;
    QLabel *statusLabel;
};

/**
 * LUỒNG DỮ LIỆU TRONG VÍ DỤ NÀY:
 * 
 * Database (SQLite)
 *     ↓
 * QSqlTableModel (sqlModel)
 *     ↓
 * QSortFilterProxyModel (proxyModel)
 *     ↓
 * QTableView (tableView)
 * 
 * Khi user:
 * - Nhập tìm kiếm → proxyModel lọc dữ liệu
 * - Chỉnh sửa cell → sqlModel cập nhật (chưa lưu)
 * - Click "Lưu" → sqlModel submit vào database
 * - Click "Làm mới" → sqlModel select lại từ database
 */

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);
//     MainWindow window;
//     window.show();
//     return app.exec();
// }

#include "06_complete_example.moc"
