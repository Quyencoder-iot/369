/**
 * VÍ DỤ 2: CUSTOM DELEGATE VỚI CON TRỎ
 * 
 * Mục tiêu: Hiểu cách delegate dùng con trỏ để vẽ và tạo editor
 * Thời gian: 15 phút
 */

#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QSpinBox>
#include <QProgressBar>
#include <QDebug>

/**
 * 🎨 CUSTOM DELEGATE 1: Vẽ Progress Bar
 * 
 * Mục đích: Hiểu cách delegate nhận CON TRỎ QPainter để vẽ
 */
class ProgressBarDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    // Constructor: nhận parent pointer (để Qt quản lý bộ nhớ)
    ProgressBarDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {
        qDebug() << "✅ ProgressBarDelegate created at:" << this;
    }
    
    ~ProgressBarDelegate() {
        qDebug() << "❌ ProgressBarDelegate destroyed at:" << this;
    }
    
    // 🔑 HÀM PAINT: Nhận CON TRỎ QPainter
    void paint(QPainter* painter,                    // ← CON TRỎ!
               const QStyleOptionViewItem& option,   // Tham chiếu
               const QModelIndex& index) const       // Tham chiếu
               override {
        
        // 📝 Kiểm tra painter pointer
        if (!painter) {
            qDebug() << "⚠️ painter is nullptr!";
            return;
        }
        
        // Lấy giá trị từ model (số từ 0-100)
        int progress = index.data().toInt();
        
        // 🎨 Sử dụng CON TRỎ painter với operator ->
        painter->save();  // Lưu trạng thái
        //      ^^^
        //      Dùng -> để gọi method qua con trỏ!
        
        // Vẽ background
        painter->fillRect(option.rect, Qt::lightGray);
        
        // Vẽ progress bar
        int progressWidth = (option.rect.width() * progress) / 100;
        QRect progressRect = option.rect;
        progressRect.setWidth(progressWidth);
        
        // Chọn màu theo giá trị
        QColor barColor = progress < 50 ? Qt::red : 
                         progress < 80 ? Qt::yellow : 
                         Qt::green;
        painter->fillRect(progressRect, barColor);
        
        // Vẽ text
        QString text = QString("%1%").arg(progress);
        painter->setPen(Qt::black);
        painter->drawText(option.rect, Qt::AlignCenter, text);
        
        painter->restore();  // Khôi phục trạng thái
        
        // 💡 LƯU Ý: Không delete painter! 
        // Qt quản lý và sẽ tự xóa sau khi vẽ xong
    }
};

/**
 * 🎨 CUSTOM DELEGATE 2: Custom Editor
 * 
 * Mục đích: Hiểu cách delegate TẠO và TRẢ VỀ con trỏ widget
 */
class SpinBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    SpinBoxDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {}
    
    // 🔑 TẠO EDITOR: Trả về CON TRỎ QWidget
    QWidget* createEditor(QWidget* parent,              // ← CON TRỎ parent
                         const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override {
        
        qDebug() << "🔨 createEditor() được gọi";
        qDebug() << "   Parent widget:" << parent;
        
        // 📝 Tạo editor widget với PARENT
        // → Parent sẽ tự động xóa editor khi không cần
        QSpinBox* editor = new QSpinBox(parent);
        //        ^^^^^^^                ^^^^^
        //        Con trỏ mới            Parent (quan trọng!)
        
        editor->setMinimum(0);
        editor->setMaximum(100);
        editor->setSingleStep(5);
        
        qDebug() << "   Editor tạo tại:" << editor;
        
        // 🔑 TRẢ VỀ CON TRỎ
        // Qt/View sẽ quản lý editor này
        return editor;  // Trả về raw pointer
    }
    
    // 🔑 SET DỮ LIỆU vào editor: Nhận CON TRỎ editor
    void setEditorData(QWidget* editor,           // ← CON TRỎ!
                      const QModelIndex& index) const override {
        
        qDebug() << "📝 setEditorData() được gọi";
        qDebug() << "   Editor pointer:" << editor;
        
        // 📝 CAST con trỏ về đúng type
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
        //        ^^^^^^^                           ^^^^^^
        //        Con trỏ cụ thể                    Cast từ con trỏ base
        
        if (spinBox) {  // Kiểm tra cast thành công
            int value = index.data().toInt();
            spinBox->setValue(value);
            qDebug() << "   Set value:" << value;
        } else {
            qDebug() << "   ⚠️ Cast thất bại!";
        }
    }
    
    // 🔑 LẤY DỮ LIỆU từ editor: Nhận CON TRỎ editor
    void setModelData(QWidget* editor,                // ← CON TRỎ!
                     QAbstractItemModel* model,       // ← CON TRỎ!
                     const QModelIndex& index) const override {
        
        qDebug() << "💾 setModelData() được gọi";
        
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
        if (spinBox && model) {  // Kiểm tra cả 2 pointers
            int value = spinBox->value();
            
            // Dùng con trỏ model để set data
            model->setData(index, value);
            //    ^
            //    Gọi method qua con trỏ
            
            qDebug() << "   Saved value:" << value;
        }
    }
};

/**
 * 📊 MAIN WINDOW
 */
class DelegateExample : public QWidget {
    Q_OBJECT
    
private:
    QStandardItemModel* m_model;
    QTableView* m_tableView;
    ProgressBarDelegate* m_progressDelegate;  // Lưu con trỏ delegate
    SpinBoxDelegate* m_spinBoxDelegate;       // Lưu con trỏ delegate
    
public:
    DelegateExample(QWidget* parent = nullptr) : QWidget(parent) {
        setupModel();
        setupView();
        demonstrateDelegatePointers();
        
        setWindowTitle("Custom Delegate với Con Trỏ");
        resize(600, 400);
    }
    
    void setupModel() {
        // Tạo model
        m_model = new QStandardItemModel(5, 2, this);
        m_model->setHorizontalHeaderItem(0, new QStandardItem("Tên"));
        m_model->setHorizontalHeaderItem(1, new QStandardItem("Tiến độ %"));
        
        // Điền dữ liệu
        QStringList names = {"Task 1", "Task 2", "Task 3", "Task 4", "Task 5"};
        QList<int> progress = {25, 50, 75, 90, 100};
        
        for (int i = 0; i < 5; ++i) {
            m_model->setItem(i, 0, new QStandardItem(names[i]));
            m_model->setItem(i, 1, new QStandardItem(QString::number(progress[i])));
        }
    }
    
    void setupView() {
        m_tableView = new QTableView(this);
        m_tableView->setModel(m_model);
        
        // 🔑 TẠO VÀ GÁN DELEGATES với CON TRỎ
        
        // Delegate 1: Cho cột 1 (Progress Bar)
        m_progressDelegate = new ProgressBarDelegate(this);
        //                   ^^^
        //                   Tạo với parent = this
        
        m_tableView->setItemDelegateForColumn(1, m_progressDelegate);
        //           ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //           View LƯU CON TRỎ đến delegate
        
        qDebug() << "📍 ProgressDelegate address:" << m_progressDelegate;
        qDebug() << "📍 Delegate từ view:" 
                 << m_tableView->itemDelegateForColumn(1);
        // → Cùng địa chỉ!
        
        // Delegate 2: Cho cột 0 (SpinBox Editor)
        m_spinBoxDelegate = new SpinBoxDelegate(this);
        m_tableView->setItemDelegateForColumn(0, m_spinBoxDelegate);
        
        // Layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_tableView);
    }
    
    void demonstrateDelegatePointers() {
        qDebug() << "\n🔍 DEMO: Con Trỏ trong Delegate\n";
        
        // Demo 1: Lấy delegate từ view
        qDebug() << "📝 Demo 1: Lấy delegate pointer từ view...";
        QAbstractItemDelegate* delegate = m_tableView->itemDelegateForColumn(1);
        //                     ^^^^^^^^^
        //                     Con trỏ base class!
        
        qDebug() << "   Delegate pointer:" << delegate;
        qDebug() << "   So sánh với m_progressDelegate:" 
                 << (delegate == m_progressDelegate);
        // → true!
        
        // Demo 2: Dynamic cast
        qDebug() << "\n📝 Demo 2: Dynamic cast delegate pointer...";
        ProgressBarDelegate* progressDel = 
            qobject_cast<ProgressBarDelegate*>(delegate);
        
        if (progressDel) {
            qDebug() << "   Cast thành công!" << progressDel;
            qDebug() << "   → Có thể gọi methods của ProgressBarDelegate";
        } else {
            qDebug() << "   Cast thất bại!";
        }
        
        // Demo 3: Default delegate
        qDebug() << "\n📝 Demo 3: Default delegate...";
        QAbstractItemDelegate* defaultDel = m_tableView->itemDelegate();
        qDebug() << "   Default delegate:" << defaultDel;
        qDebug() << "   → View luôn có một default delegate";
    }
};

// 🎓 BÀI TẬP:
// 1. Tạo delegate vẽ màu nền theo giá trị (đỏ: 0-33, vàng: 34-66, xanh: 67-100)
// 2. Tạo delegate với QComboBox editor thay vì QSpinBox
// 3. Thử KHÔNG truyền parent khi tạo delegate
//    → Quan sát memory leak với valgrind
// 4. Override updateEditorGeometry() để custom vị trí editor

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "🚀 Bắt đầu ví dụ Custom Delegate với Con Trỏ\n";
    
    DelegateExample window;
    window.show();
    
    qDebug() << "\n💡 Hướng dẫn:";
    qDebug() << "   - Double click vào cell để edit";
    qDebug() << "   - Cột 'Tiến độ %' hiển thị progress bar";
    qDebug() << "   - Quan sát debug log để thấy con trỏ hoạt động\n";
    
    return app.exec();
}

#include "custom_delegate_example.moc"

/* 
📚 KIẾN THỨC RÚT RA VỀ CON TRỎ TRONG DELEGATE:

1. DELEGATE NHẬN CON TRỎ trong các methods:
   - paint(QPainter* painter, ...)
   - createEditor(...) → trả về QWidget*
   - setEditorData(QWidget* editor, ...)
   - setModelData(QWidget* editor, QAbstractItemModel* model, ...)
   
2. CON TRỎ PAINTER:
   - Qt truyền painter pointer vào paint()
   - Dùng painter->method() để vẽ
   - KHÔNG delete painter! Qt tự quản lý
   
3. EDITOR WIDGET:
   - createEditor() tạo widget với PARENT
   - Trả về raw pointer
   - View/Qt sở hữu và tự xóa editor
   
4. QOBJECT_CAST:
   - Cast an toàn giữa QObject pointers
   - Trả về nullptr nếu cast thất bại
   - Luôn kiểm tra kết quả!
   
5. DELEGATE OWNERSHIP:
   - Tạo delegate với parent = view hoặc window
   - View LƯU CON TRỎ đến delegate
   - Parent tự động xóa delegate
   
6. CON TRỎ BASE CLASS:
   - View->itemDelegate() trả QAbstractItemDelegate*
   - Dùng qobject_cast<> để cast về derived class
   - Cho phép đa hình (polymorphism)
*/
