#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QTableView>

// 2. SQL Model: Dữ liệu nằm trong Database (SQLite, MySQL, v.v.)
// Không cần viết rowCount, data() vì Qt đã viết sẵn rồi.
void setupSqlModel() {
    // Kết nối DB
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("example.db");
    if (!db.open()) return;

    // Khởi tạo Model
    // READING TIP: QSqlTableModel liên kết trực tiếp với 1 bảng trong DB
    QSqlTableModel *model = new QSqlTableModel(nullptr, db);
    
    // Cấu hình bảng
    model->setTable("people");          // Tên bảng trong DB
    model->setEditStrategy(QSqlTableModel::OnFieldChange); // Lưu ngay khi sửa
    model->select();                    // Lệnh SELECT * FROM people

    // View
    QTableView *view = new QTableView;
    view->setModel(model); // View hiển thị dữ liệu từ DB
    view->show();
    
    // READING TIP:
    // QSqlQueryModel: Chỉ đọc (Read-only), dùng câu lệnh SQL tùy ý (SELECT * FROM a JOIN b...)
    // QSqlTableModel: Đọc/Ghi (Read/Write), chỉ 1 bảng đơn lẻ.
    // QSqlRelationalTableModel: 1 bảng + khóa ngoại (Foreign Key) hiển thị tên thay vì ID.
}
