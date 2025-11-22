/**
 * Ví dụ 4: QSortFilterProxyModel - Proxy Model để Lọc và Sắp Xếp
 * 
 * Mục đích: Hiểu cách sử dụng Proxy Model
 */

#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QTableView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

// Model gốc (giống ví dụ 2)
class EmployeeTableModel : public QAbstractTableModel
{
    // ... (code giống ví dụ 2)
};

// Sử dụng Proxy Model:
void setupProxyExample()
{
    // 1. Tạo model gốc
    EmployeeTableModel *sourceModel = new EmployeeTableModel();

    // 2. Tạo Proxy Model
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    
    // 3. Kết nối Proxy với Model gốc
    proxyModel->setSourceModel(sourceModel);

    // 4. Thiết lập Proxy
    proxyModel->setFilterKeyColumn(1);        // Lọc theo cột "Tên"
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->sort(0, Qt::AscendingOrder);  // Sắp xếp theo cột đầu tiên

    // 5. Kết nối Proxy với View (KHÔNG phải model gốc!)
    QTableView *view = new QTableView();
    view->setModel(proxyModel);  // ← Quan trọng: dùng proxy, không dùng sourceModel

    // 6. Tạo ô tìm kiếm để lọc
    QLineEdit *searchBox = new QLineEdit();
    QObject::connect(searchBox, &QLineEdit::textChanged,
                     proxyModel, &QSortFilterProxyModel::setFilterFixedString);
}

/**
 * LUỒNG DỮ LIỆU:
 * 
 * Source Model → Proxy Model → View
 *     ↓              ↓           ↓
 *  Dữ liệu      Lọc/Sắp xếp   Hiển thị
 * 
 * Khi user nhập vào searchBox:
 * 1. searchBox::textChanged() → proxyModel::setFilterFixedString()
 * 2. Proxy lọc dữ liệu từ Source Model
 * 3. Proxy cập nhật View
 */

/**
 * CÁC LOẠI PROXY MODEL KHÁC:
 */

// 1. QSortFilterProxyModel - Lọc và sắp xếp (phổ biến nhất)
QSortFilterProxyModel *sortFilterProxy = new QSortFilterProxyModel();
sortFilterProxy->setSourceModel(sourceModel);
sortFilterProxy->setFilterRegExp(QRegExp("pattern"));
sortFilterProxy->sort(0, Qt::AscendingOrder);

// 2. QIdentityProxyModel - Giữ nguyên cấu trúc, chỉ thêm logic
class CustomProxy : public QIdentityProxyModel {
    // Override data() để thêm logic xử lý
    QVariant data(const QModelIndex &index, int role) const override {
        QVariant value = sourceModel()->data(index, role);
        // Thêm logic xử lý ở đây
        if (role == Qt::DisplayRole && index.column() == 0) {
            return "Prefix: " + value.toString();
        }
        return value;
    }
};

// 3. Xếp chồng nhiều Proxy
/*
QSortFilterProxyModel *proxy1 = new QSortFilterProxyModel();
proxy1->setSourceModel(sourceModel);

CustomProxy *proxy2 = new CustomProxy();
proxy2->setSourceModel(proxy1);  // Proxy2 lấy dữ liệu từ Proxy1

view->setModel(proxy2);  // View kết nối với Proxy cuối cùng
*/
