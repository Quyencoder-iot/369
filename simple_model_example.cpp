/**
 * VÍ DỤ 1: MODEL-VIEW CỞ BẢN VỚI CON TRỎ
 * 
 * Mục tiêu: Hiểu cách tạo và kết nối Model-View bằng con trỏ
 * Thời gian: 10 phút
 */

#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>

class SimpleModelExample : public QWidget {
    Q_OBJECT
    
private:
    // ⭐ QUAN TRỌNG: Lưu con trỏ đến model và view
    QStandardItemModel* m_model;   // Con trỏ đến model
    QTableView* m_tableView;       // Con trỏ đến view
    QTableView* m_tableView2;      // Con trỏ đến view thứ 2
    
public:
    SimpleModelExample(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        demonstratePointers();
    }
    
    void setupUI() {
        // 📝 BƯỚC 1: TẠO MODEL với parent = this
        // → Qt sẽ tự động xóa model khi widget này bị hủy
        m_model = new QStandardItemModel(5, 3, this);
        //        ^^^                         ^^^^
        //        |                           |
        //        Con trỏ mới                Parent (quan trọng!)
        
        // Set header cho model
        m_model->setHorizontalHeaderItem(0, new QStandardItem("Tên"));
        m_model->setHorizontalHeaderItem(1, new QStandardItem("Tuổi"));
        m_model->setHorizontalHeaderItem(2, new QStandardItem("Điểm"));
        
        // 📝 BƯỚC 2: ĐIỀN DỮ LIỆU vào model
        populateModel();
        
        // 📝 BƯỚC 3: TẠO VIEW 1
        m_tableView = new QTableView(this);  // Parent = this
        
        // 🔑 ĐIỂM QUAN TRỌNG: Kết nối view với model
        // View LƯU CON TRỎ đến model, KHÔNG COPY dữ liệu!
        m_tableView->setModel(m_model);
        //           ^^^^^^^^ 
        //           View giữ địa chỉ của model
        
        qDebug() << "📍 Địa chỉ model:" << m_model;
        qDebug() << "📍 Model mà view1 đang dùng:" << m_tableView->model();
        // → Hai địa chỉ này GIỐNG NHAU!
        
        // 📝 BƯỚC 4: TẠO VIEW 2 - dùng CÙNG model
        m_tableView2 = new QTableView(this);
        m_tableView2->setModel(m_model);  // Cùng con trỏ model!
        //                     ^^^^^^^^
        //                     Cùng địa chỉ → cùng dữ liệu!
        
        qDebug() << "📍 Model mà view2 đang dùng:" << m_tableView2->model();
        // → Ba địa chỉ đều GIỐNG NHAU!
        
        // 📝 BƯỚC 5: Layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_tableView);
        layout->addWidget(m_tableView2);
        
        setWindowTitle("Qt Model-View với Con Trỏ - Ví dụ đơn giản");
        resize(600, 400);
    }
    
    void populateModel() {
        // Dữ liệu mẫu
        QStringList names = {"An", "Bình", "Chi", "Dũng", "Lan"};
        QList<int> ages = {20, 22, 19, 23, 21};
        QList<double> scores = {8.5, 7.0, 9.0, 6.5, 8.0};
        
        for (int row = 0; row < 5; ++row) {
            // Tạo items (cũng dùng con trỏ!)
            QStandardItem* nameItem = new QStandardItem(names[row]);
            QStandardItem* ageItem = new QStandardItem(QString::number(ages[row]));
            QStandardItem* scoreItem = new QStandardItem(QString::number(scores[row]));
            
            // Set item vào model (model sở hữu các items này)
            m_model->setItem(row, 0, nameItem);
            m_model->setItem(row, 1, ageItem);
            m_model->setItem(row, 2, scoreItem);
            //       ^^^^^^^ 
            //       Model nhận con trỏ và SỞ HỮU item
            //       → Không cần delete, model sẽ tự xóa!
        }
    }
    
    void demonstratePointers() {
        qDebug() << "\n🔍 DEMO: Hiểu về Con Trỏ trong Qt MVC\n";
        
        // Demo 1: Thay đổi model → tất cả views cập nhật
        qDebug() << "📝 Demo 1: Thay đổi model...";
        
        // Lấy con trỏ đến item
        QStandardItem* item = m_model->item(0, 0);
        //                    ^^^^^^^^^^^^^^^^
        //                    Model TRẢ VỀ con trỏ đến item
        
        if (item != nullptr) {  // Luôn kiểm tra nullptr!
            qDebug() << "   Giá trị cũ:" << item->text();
            item->setText("⭐ " + item->text());
            qDebug() << "   Giá trị mới:" << item->text();
            qDebug() << "   → Cả 2 views đều tự động cập nhật!";
        }
        
        // Demo 2: So sánh con trỏ
        qDebug() << "\n📝 Demo 2: So sánh địa chỉ con trỏ...";
        QAbstractItemModel* modelFromView1 = m_tableView->model();
        QAbstractItemModel* modelFromView2 = m_tableView2->model();
        
        qDebug() << "   View1.model() == View2.model()?" 
                 << (modelFromView1 == modelFromView2);
        qDebug() << "   → true: Cùng CON TRỎ đến cùng object!";
        
        // Demo 3: Model Index (không phải con trỏ thường)
        qDebug() << "\n📝 Demo 3: QModelIndex (đặc biệt)...";
        QModelIndex index = m_model->index(0, 0);
        //          ^^^^^
        //          Không phải con trỏ! Là value object nhẹ
        
        qDebug() << "   Index là:" << index;
        qDebug() << "   Dữ liệu:" << m_model->data(index).toString();
        qDebug() << "   → QModelIndex là 'handle' nhẹ, không phải con trỏ thực";
        
        // Demo 4: Pointer vs Reference
        qDebug() << "\n📝 Demo 4: Con trỏ vs Tham chiếu...";
        processModelByPointer(m_model);      // Truyền con trỏ
        processModelByReference(*m_model);   // Truyền tham chiếu
    }
    
    // Hàm nhận CON TRỎ model
    void processModelByPointer(QAbstractItemModel* model) {
        qDebug() << "   processModelByPointer(): Nhận con trỏ";
        qDebug() << "   Có thể check nullptr:" << (model != nullptr);
        if (model) {
            qDebug() << "   Số hàng:" << model->rowCount();
        }
    }
    
    // Hàm nhận THAM CHIẾU model
    void processModelByReference(QAbstractItemModel& model) {
        qDebug() << "   processModelByReference(): Nhận tham chiếu";
        qDebug() << "   Không cần check null, luôn valid";
        qDebug() << "   Số hàng:" << model.rowCount();
    }
};

// 🎓 BÀI TẬP:
// 1. Thêm một view thứ 3, kết nối với cùng model
// 2. Thay đổi một cell từ code, quan sát cả 3 views cập nhật
// 3. Thử tạo model KHÔNG có parent (new QStandardItemModel())
//    → Thêm delete model trong destructor
// 4. Print địa chỉ bộ nhớ của model và các items: qDebug() << static_cast<void*>(model);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "🚀 Bắt đầu ví dụ Qt Model-View với Con Trỏ\n";
    
    SimpleModelExample window;
    window.show();
    
    qDebug() << "\n💡 Quan sát: Khi thay đổi data ở view 1, view 2 cũng thay đổi";
    qDebug() << "    → Vì cả 2 đều dùng CÙNG CON TRỎ đến cùng model!\n";
    
    return app.exec();
}

#include "simple_model_example.moc"

/* 
📚 KIẾN THỨC RÚT RA:

1. MODEL-VIEW dùng CON TRỎ để chia sẻ dữ liệu:
   - Nhiều views có thể trỏ đến cùng 1 model
   - Thay đổi model → tất cả views cập nhật
   
2. PARENT-CHILD OWNERSHIP:
   - new QStandardItemModel(this) → this sở hữu model
   - Khi this bị delete → model tự động delete
   - Không cần delete thủ công!
   
3. POINTER vs INDEX:
   - Model, View, Delegate: dùng CON TRỎ (QAbstractItemModel*)
   - Items trong model: dùng QModelIndex (value object, không phải pointer)
   
4. NULL CHECK:
   - Luôn kiểm tra: if (pointer != nullptr)
   - Hoặc dùng: if (pointer) 
   
5. SO SÁNH CON TRỎ:
   - pointer1 == pointer2: so sánh địa chỉ
   - Cùng địa chỉ = cùng object
*/
