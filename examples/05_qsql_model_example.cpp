/**
 * Ví dụ 5: QSQL Models - Làm Việc Với Database
 * 
 * Mục đích: Hiểu cách sử dụng QSqlQueryModel và QSqlTableModel
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QSqlError>
#include <QDebug>

/**
 * 1. QSqlQueryModel - Chỉ đọc dữ liệu từ query
 */
void exampleSqlQueryModel()
{
    // Kết nối database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("employees.db");
    
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        return;
    }

    // Tạo model từ query
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT id, name, department, salary FROM employees WHERE salary > 5000000");

    // Kiểm tra lỗi
    if (model->lastError().isValid()) {
        qDebug() << "Query error:" << model->lastError();
    }

    // Hiển thị
    QTableView *view = new QTableView();
    view->setModel(model);
    view->show();

    // Đặc điểm:
    // - Chỉ đọc, không thể chỉnh sửa
    // - Dữ liệu được load từ query
    // - Phù hợp cho báo cáo, thống kê
}

/**
 * 2. QSqlTableModel - Đọc và ghi dữ liệu vào 1 bảng
 */
void exampleSqlTableModel()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("employees.db");
    db.open();

    // Tạo model cho bảng "employees"
    QSqlTableModel *model = new QSqlTableModel();
    model->setTable("employees");
    
    // Thiết lập edit strategy
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // Hoặc OnFieldChange, OnRowChange
    
    // Lọc dữ liệu (tùy chọn)
    model->setFilter("department = 'IT'");
    
    // Sắp xếp (tùy chọn)
    model->setSort(3, Qt::DescendingOrder); // Sắp xếp theo cột 3 (salary)
    
    // Load dữ liệu
    if (!model->select()) {
        qDebug() << "Select error:" << model->lastError();
        return;
    }

    // Đặt tên header
    model->setHeaderData(0, Qt::Horizontal, "Mã NV");
    model->setHeaderData(1, Qt::Horizontal, "Tên");
    model->setHeaderData(2, Qt::Horizontal, "Phòng ban");
    model->setHeaderData(3, Qt::Horizontal, "Lương");

    QTableView *view = new QTableView();
    view->setModel(model);
    view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    view->show();

    // Lưu thay đổi
    // model->submitAll();  // Lưu tất cả thay đổi
    // model->revertAll(); // Hủy tất cả thay đổi

    // Đặc điểm:
    // - Có thể chỉnh sửa (CRUD)
    // - Tự động sync với database
    // - Làm việc với 1 bảng
}

/**
 * 3. QSqlRelationalTableModel - Model với quan hệ giữa các bảng
 */
void exampleSqlRelationalTableModel()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("company.db");
    db.open();

    // Tạo model cho bảng "employees" có quan hệ với bảng "departments"
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel();
    model->setTable("employees");
    
    // Thiết lập quan hệ: cột "department_id" tham chiếu đến bảng "departments"
    model->setRelation(2, QSqlRelation("departments", "id", "name"));
    // Cột 2 (department_id) → hiển thị "name" từ bảng "departments"

    model->select();

    QTableView *view = new QTableView();
    view->setModel(model);
    view->show();

    // Đặc điểm:
    // - Hỗ trợ foreign key
    // - Hiển thị dữ liệu từ bảng liên quan
    // - Phức tạp hơn nhưng mạnh mẽ hơn
}

/**
 * 4. Kết Hợp QSqlTableModel với QSortFilterProxyModel
 */
void exampleSqlWithProxy()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("employees.db");
    db.open();

    // Model gốc từ database
    QSqlTableModel *sqlModel = new QSqlTableModel();
    sqlModel->setTable("employees");
    sqlModel->select();

    // Proxy để lọc và sắp xếp (không cần query lại database)
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(sqlModel);
    proxyModel->setFilterKeyColumn(1); // Lọc theo cột "name"
    proxyModel->sort(0, Qt::AscendingOrder);

    QTableView *view = new QTableView();
    view->setModel(proxyModel); // View kết nối với proxy
    view->show();

    // Lợi ích:
    // - Lọc/sắp xếp nhanh (trong memory)
    // - Không cần query lại database
    // - User có thể tương tác trực tiếp
}

/**
 * SO SÁNH CÁC SQL MODELS:
 * 
 * QSqlQueryModel:
 *   - Chỉ đọc
 *   - Từ query tùy ý
 *   - Nhanh, đơn giản
 *   - Dùng cho: báo cáo, thống kê
 * 
 * QSqlTableModel:
 *   - Đọc và ghi
 *   - 1 bảng
 *   - Tự động sync
 *   - Dùng cho: CRUD operations, form quản lý
 * 
 * QSqlRelationalTableModel:
 *   - Đọc và ghi
 *   - Nhiều bảng có quan hệ
 *   - Hỗ trợ foreign key
 *   - Dùng cho: database phức tạp, có quan hệ
 */
