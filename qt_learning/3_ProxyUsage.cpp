#include <QSortFilterProxyModel>
#include "1_CustomTableModel.h"
#include <QTableView>

// 3. Proxy Model: Lớp trung gian để LỌC (Filter) hoặc SẮP XẾP (Sort)
// Nó đứng giữa:  Source Model (Gốc) -> Proxy Model -> View (Hiển thị)

void setupProxy() {
    // Model Gốc
    PersonModel *sourceModel = new PersonModel();
    
    // Proxy Model
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    
    // KẾT NỐI: Quan trọng nhất là dòng này
    proxyModel->setSourceModel(sourceModel);

    // Cấu hình Lọc: Chỉ hiện người tên có chữ "A"
    proxyModel->setFilterKeyColumn(0); // Cột 0 là Name
    proxyModel->setFilterRegularExpression("A.*"); // Regex
    
    // Cấu hình Sắp xếp
    proxyModel->setDynamicSortFilter(true); // Tự động sắp xếp khi dữ liệu đổi

    // View
    QTableView *view = new QTableView;
    
    // LƯU Ý QUAN TRỌNG KHI ĐỌC CODE:
    // View setModel là proxy, KHÔNG phải sourceModel
    view->setModel(proxyModel); 
    view->setSortingEnabled(true); // Cho phép click header để sort
    view->show();
    
    // MAPPING:
    // Nếu bạn có QModelIndex từ View (đang là index của Proxy), 
    // muốn lấy dữ liệu gốc phải dùng: mapToSource(proxyIndex)
}
