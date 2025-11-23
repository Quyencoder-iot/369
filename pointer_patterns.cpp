/**
 * VÍ DỤ 3: CÁC PATTERN CON TRỎ THƯỜNG DÙNG TRONG QT MVC
 * 
 * Mục tiêu: Tổng hợp các pattern con trỏ hay gặp
 * Thời gian: 10 phút
 */

#include <QApplication>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QPointer>
#include <QSharedPointer>
#include <QDebug>
#include <QTimer>

/**
 * 🎯 PATTERN 1: CHIA SẺ MODEL GIỮA NHIỀU VIEWS
 */
void pattern1_SharedModel() {
    qDebug() << "\n📝 PATTERN 1: Một Model - Nhiều Views\n";
    
    // Tạo model
    QStandardItemModel* model = new QStandardItemModel(3, 2);
    model->setData(model->index(0, 0), "Data 1");
    model->setData(model->index(1, 0), "Data 2");
    model->setData(model->index(2, 0), "Data 3");
    
    // Tạo nhiều views với CÙNG model pointer
    QTableView* tableView = new QTableView();
    QListView* listView = new QListView();
    QTreeView* treeView = new QTreeView();
    
    // Tất cả đều trỏ đến cùng model
    tableView->setModel(model);
    listView->setModel(model);
    treeView->setModel(model);
    
    qDebug() << "Model address:" << model;
    qDebug() << "TableView's model:" << tableView->model();
    qDebug() << "ListView's model:" << listView->model();
    qDebug() << "TreeView's model:" << treeView->model();
    qDebug() << "→ Tất cả đều CÙNG địa chỉ!";
    
    // Thay đổi model → tất cả views cập nhật
    model->setData(model->index(0, 0), "⭐ Modified!");
    qDebug() << "Sau khi sửa model, tất cả views đều thấy thay đổi";
    
    // Cleanup
    delete tableView;
    delete listView;
    delete treeView;
    delete model;
    
    qDebug() << "✅ Pattern 1 hoàn thành\n";
}

/**
 * 🎯 PATTERN 2: PROXY MODEL (Chain of Pointers)
 */
void pattern2_ProxyModel() {
    qDebug() << "\n📝 PATTERN 2: Proxy Model - Chuỗi Con Trỏ\n";
    
    // Source model
    QStandardItemModel* sourceModel = new QStandardItemModel(5, 1);
    for (int i = 0; i < 5; ++i) {
        sourceModel->setData(sourceModel->index(i, 0), 
                            QString("Item %1").arg(i + 1));
    }
    
    // Proxy model - lọc và sắp xếp
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
    
    // 🔑 Proxy GIỮ CON TRỎ đến source model
    proxyModel->setSourceModel(sourceModel);
    //          ^^^^^^^^^^^^^^
    //          Proxy trỏ đến source
    
    // View trỏ đến proxy (không trực tiếp đến source!)
    QTableView* view = new QTableView();
    view->setModel(proxyModel);
    
    // Chuỗi con trỏ: View → Proxy → Source
    qDebug() << "View -> model:" << view->model();
    qDebug() << "Proxy -> sourceModel:" << proxyModel->sourceModel();
    qDebug() << "Source model:" << sourceModel;
    qDebug() << "→ Chuỗi con trỏ: View → Proxy → Source";
    
    // Lọc dữ liệu
    proxyModel->setFilterRegularExpression(".*[135].*");  // Chỉ hiện Item 1, 3, 5
    qDebug() << "Sau khi filter, view chỉ hiển thị items được lọc";
    qDebug() << "Nhưng source model vẫn giữ đủ dữ liệu!";
    
    // Cleanup
    delete view;
    delete proxyModel;
    delete sourceModel;
    
    qDebug() << "✅ Pattern 2 hoàn thành\n";
}

/**
 * 🎯 PATTERN 3: QPOINTER - Con Trỏ An Toàn
 */
void pattern3_QPointer() {
    qDebug() << "\n📝 PATTERN 3: QPointer - Con Trỏ An Toàn\n";
    
    // ❌ Vấn đề với RAW POINTER
    QTableView* rawPtr = new QTableView();
    qDebug() << "Raw pointer:" << rawPtr;
    
    delete rawPtr;
    // rawPtr vẫn giữ địa chỉ cũ (DANGLING POINTER!)
    qDebug() << "Sau delete, raw pointer vẫn có giá trị:" << rawPtr;
    qDebug() << "→ NGUY HIỂM! Dùng rawPtr bây giờ sẽ CRASH";
    
    // ✅ Giải pháp: QPointer
    QPointer<QTableView> safePtr = new QTableView();
    qDebug() << "\nQPointer:" << safePtr.data();
    qDebug() << "isNull():" << safePtr.isNull();
    
    delete safePtr.data();
    // QPointer TỰ ĐỘNG set về nullptr!
    qDebug() << "Sau delete, QPointer:" << safePtr.data();
    qDebug() << "isNull():" << safePtr.isNull();
    qDebug() << "→ AN TOÀN! safePtr tự động = nullptr";
    
    // Cách dùng an toàn
    if (safePtr) {
        safePtr->show();  // Chỉ gọi nếu còn tồn tại
    } else {
        qDebug() << "Object đã bị xóa, không làm gì cả";
    }
    
    qDebug() << "✅ Pattern 3 hoàn thành\n";
}

/**
 * 🎯 PATTERN 4: QSHAREDPOINTER - Quản Lý Tự Động
 */
void pattern4_QSharedPointer() {
    qDebug() << "\n📝 PATTERN 4: QSharedPointer - Reference Counting\n";
    
    // Tạo shared pointer
    QSharedPointer<QStandardItemModel> sharedModel(new QStandardItemModel(3, 2));
    qDebug() << "SharedPointer created";
    qDebug() << "Reference count:" << sharedModel.use_count();
    
    {
        // Scope mới: tạo thêm shared pointer
        QSharedPointer<QStandardItemModel> sharedModel2 = sharedModel;
        qDebug() << "\nTrong scope:";
        qDebug() << "  Reference count:" << sharedModel.use_count();
        qDebug() << "  → 2 pointers cùng trỏ đến object";
        
        // Cả 2 đều trỏ đến cùng object
        qDebug() << "  sharedModel:" << sharedModel.data();
        qDebug() << "  sharedModel2:" << sharedModel2.data();
        qDebug() << "  → Cùng địa chỉ!";
    }
    // sharedModel2 ra khỏi scope → ref count giảm
    
    qDebug() << "\nRa khỏi scope:";
    qDebug() << "Reference count:" << sharedModel.use_count();
    qDebug() << "→ sharedModel2 hủy, nhưng object vẫn còn";
    
    // Khi sharedModel hủy, object tự động delete
    qDebug() << "✅ Pattern 4 hoàn thành\n";
}

/**
 * 🎯 PATTERN 5: PARENT-CHILD OWNERSHIP
 */
void pattern5_ParentChildOwnership() {
    qDebug() << "\n📝 PATTERN 5: Parent-Child Ownership\n";
    
    // Tạo parent widget
    QWidget* parent = new QWidget();
    qDebug() << "Parent created:" << parent;
    
    // Tạo children với parent
    QTableView* child1 = new QTableView(parent);
    QTableView* child2 = new QTableView(parent);
    QStandardItemModel* model = new QStandardItemModel(parent);
    
    qDebug() << "Children created:";
    qDebug() << "  child1:" << child1;
    qDebug() << "  child2:" << child2;
    qDebug() << "  model:" << model;
    
    // Kiểm tra parent
    qDebug() << "child1->parent():" << child1->parent();
    qDebug() << "→ parent() trả về parent pointer";
    
    // 🔑 Xóa parent → tất cả children TỰ ĐỘNG xóa!
    qDebug() << "\nĐang xóa parent...";
    delete parent;
    // child1, child2, model đều đã bị xóa!
    
    qDebug() << "→ Tất cả children tự động bị xóa";
    qDebug() << "→ KHÔNG cần delete child1, child2, model!";
    qDebug() << "✅ Pattern 5 hoàn thành\n";
}

/**
 * 🎯 PATTERN 6: NULL CHECK & VALIDATION
 */
void pattern6_NullCheckPatterns() {
    qDebug() << "\n📝 PATTERN 6: Null Check Patterns\n";
    
    QTableView* view = new QTableView();
    
    // Pattern 6a: Explicit null check
    QAbstractItemModel* model = view->model();
    if (model != nullptr) {
        qDebug() << "Model exists:" << model;
    } else {
        qDebug() << "No model set → model is nullptr";
    }
    
    // Pattern 6b: Implicit bool conversion
    if (model) {
        qDebug() << "This won't print (model is null)";
    } else {
        qDebug() << "Model is null (implicit check)";
    }
    
    // Pattern 6c: Set model và kiểm tra lại
    QStandardItemModel* newModel = new QStandardItemModel(view);
    view->setModel(newModel);
    
    model = view->model();
    if (model) {
        qDebug() << "Now model exists:" << model;
        qDebug() << "Row count:" << model->rowCount();
    }
    
    // Pattern 6d: qobject_cast với null check
    QStandardItemModel* standardModel = qobject_cast<QStandardItemModel*>(model);
    if (standardModel) {
        qDebug() << "Cast successful → model is QStandardItemModel";
    } else {
        qDebug() << "Cast failed → model is different type";
    }
    
    delete view;  // Tự động xóa newModel (vì parent = view)
    
    qDebug() << "✅ Pattern 6 hoàn thành\n";
}

/**
 * 🎯 PATTERN 7: GETTING POINTERS FROM MODEL/VIEW
 */
void pattern7_GettingPointers() {
    qDebug() << "\n📝 PATTERN 7: Lấy Con Trỏ từ Model/View\n";
    
    QTableView* view = new QTableView();
    QStandardItemModel* model = new QStandardItemModel(3, 2, view);
    view->setModel(model);
    
    // Điền dữ liệu
    for (int i = 0; i < 3; ++i) {
        QStandardItem* item = new QStandardItem(QString("Item %1").arg(i));
        model->setItem(i, 0, item);
    }
    
    // Pattern 7a: Lấy model từ view
    QAbstractItemModel* retrievedModel = view->model();
    qDebug() << "Model from view:" << retrievedModel;
    qDebug() << "Original model:" << model;
    qDebug() << "→ Same pointer:" << (retrievedModel == model);
    
    // Pattern 7b: Lấy item từ model
    QStandardItem* item = model->item(0, 0);
    qDebug() << "\nItem pointer:" << item;
    if (item) {
        qDebug() << "Item text:" << item->text();
        // Modify qua pointer
        item->setText("⭐ " + item->text());
        qDebug() << "Modified text:" << item->text();
    }
    
    // Pattern 7c: Lấy delegate từ view
    QAbstractItemDelegate* delegate = view->itemDelegate();
    qDebug() << "\nDefault delegate:" << delegate;
    qDebug() << "→ View luôn có default delegate";
    
    // Pattern 7d: Lấy selection model
    QItemSelectionModel* selectionModel = view->selectionModel();
    qDebug() << "Selection model:" << selectionModel;
    qDebug() << "→ Mỗi view có riêng selection model";
    
    delete view;
    
    qDebug() << "✅ Pattern 7 hoàn thành\n";
}

/**
 * 🎯 PATTERN 8: POINTER COMPARISONS
 */
void pattern8_PointerComparisons() {
    qDebug() << "\n📝 PATTERN 8: So Sánh Con Trỏ\n";
    
    QStandardItemModel* model1 = new QStandardItemModel();
    QStandardItemModel* model2 = new QStandardItemModel();
    QStandardItemModel* model3 = model1;  // Alias
    
    qDebug() << "model1:" << model1;
    qDebug() << "model2:" << model2;
    qDebug() << "model3:" << model3;
    
    // So sánh địa chỉ
    qDebug() << "\nSo sánh:";
    qDebug() << "model1 == model2:" << (model1 == model2);  // false
    qDebug() << "model1 == model3:" << (model1 == model3);  // true
    qDebug() << "→ So sánh CON TRỎ = so sánh địa chỉ";
    
    // So sánh với nullptr
    QAbstractItemModel* nullModel = nullptr;
    qDebug() << "\nNull check:";
    qDebug() << "nullModel == nullptr:" << (nullModel == nullptr);
    qDebug() << "model1 == nullptr:" << (model1 == nullptr);
    
    // Cast và so sánh
    QAbstractItemModel* basePtr = model1;
    qDebug() << "\nSau khi cast lên base class:";
    qDebug() << "basePtr == model1:" << (basePtr == model1);
    qDebug() << "→ Vẫn cùng địa chỉ dù khác type";
    
    delete model1;
    delete model2;
    // Không delete model3 (vì cùng với model1)
    
    qDebug() << "✅ Pattern 8 hoàn thành\n";
}

/**
 * 📊 MAIN - Chạy tất cả patterns
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "🚀 BẮT ĐẦU: Các Pattern Con Trỏ trong Qt MVC\n";
    qDebug() << "=" << QString("=").repeated(60);
    
    pattern1_SharedModel();
    pattern2_ProxyModel();
    pattern3_QPointer();
    pattern4_QSharedPointer();
    pattern5_ParentChildOwnership();
    pattern6_NullCheckPatterns();
    pattern7_GettingPointers();
    pattern8_PointerComparisons();
    
    qDebug() << "=" << QString("=").repeated(60);
    qDebug() << "\n✅ HOÀN THÀNH: Tất cả patterns\n";
    
    qDebug() << "💡 TÓM TẮT:";
    qDebug() << "1. Một model có thể chia sẻ cho nhiều views";
    qDebug() << "2. Proxy model tạo chuỗi con trỏ";
    qDebug() << "3. QPointer tự động null khi object bị xóa";
    qDebug() << "4. QSharedPointer đếm reference tự động";
    qDebug() << "5. Parent tự động xóa children";
    qDebug() << "6. Luôn null check trước khi dùng pointer";
    qDebug() << "7. Có thể lấy lại pointers từ view/model";
    qDebug() << "8. So sánh pointer = so sánh địa chỉ bộ nhớ";
    
    return 0;  // Không cần show window, chỉ chạy demos
}

/* 
📚 TỔNG KẾT: CÁC PATTERN CON TRỎ QUAN TRỌNG

┌─────────────────────────────────────────────────────────────┐
│ PATTERN                 │ CÁCH DÙNG                         │
├─────────────────────────────────────────────────────────────┤
│ 1. Shared Model         │ view1->setModel(m); view2->setModel(m) │
│ 2. Proxy Chain          │ view->setModel(proxy); proxy->setSourceModel(source) │
│ 3. QPointer             │ QPointer<T> p = new T(); if(p) {...} │
│ 4. QSharedPointer       │ QSharedPointer<T> p(new T()); │
│ 5. Parent-Child         │ new Widget(parent); // parent sở hữu │
│ 6. Null Check           │ if (ptr != nullptr) {...} │
│ 7. Get Pointers         │ model = view->model(); │
│ 8. Pointer Comparison   │ if (ptr1 == ptr2) {...} │
└─────────────────────────────────────────────────────────────┘

KHI NÀO DÙNG GÌ?
├─ Raw Pointer (T*)       → Khi có parent quản lý
├─ QPointer<T>            → Khi lo ngại dangling pointer
├─ QSharedPointer<T>      → Khi cần chia sẻ ownership
└─ Unique_ptr<T>          → Khi chỉ có 1 owner (hiếm trong Qt)

LƯU Ý QUAN TRỌNG:
⚠️ Luôn set parent khi tạo QObject
⚠️ Không delete pointer mà Qt quản lý
⚠️ Luôn null check trước khi dereference
⚠️ Dùng qobject_cast thay vì dynamic_cast
⚠️ So sánh pointer trước khi delete

CODE NÀY RẤT HỮU ÍCH:
- Chạy và đọc debug output
- Thử modify code để thử nghiệm
- Đặt breakpoint và xem memory addresses
- Thêm qDebug() để hiểu rõ hơn
*/
