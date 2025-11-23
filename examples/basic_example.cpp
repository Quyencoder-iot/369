// Ví dụ cơ bản: Model-View với con trỏ
// Compile: g++ -fPIC -std=c++17 basic_example.cpp -o basic_example $(pkg-config --cflags --libs Qt6Core Qt6Widgets)

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Ví Dụ Cơ Bản: Model-View");
        resize(600, 400);
        
        // ✅ BƯỚC 1: Tạo Model với parent là 'this'
        // Model sẽ tự động bị delete khi MainWindow bị destroy
        m_model = new QStandardItemModel(this);
        
        // Setup header
        m_model->setHorizontalHeaderLabels({"Tên", "Tuổi", "Thành Phố"});
        
        // Thêm dữ liệu
        addRow("Nguyễn Văn A", "25", "Hà Nội");
        addRow("Trần Thị B", "30", "TP.HCM");
        addRow("Lê Văn C", "28", "Đà Nẵng");
        
        // ✅ BƯỚC 2: Tạo View với parent là 'this'
        m_view = new QTableView(this);
        
        // ✅ BƯỚC 3: Set model cho view
        // View KHÔNG sở hữu model, chỉ tham chiếu
        m_view->setModel(m_model);
        
        // Tùy chỉnh view
        m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_view->horizontalHeader()->setStretchLastSection(true);
        
        // ✅ BƯỚC 4: Set central widget
        setCentralWidget(m_view);
        
        // ✅ Kết nối signal để xem cách con trỏ hoạt động
        connect(m_view, &QTableView::clicked, this, &MainWindow::onCellClicked);
        
        qDebug() << "✅ Model và View đã được tạo với parent-child relationship";
        qDebug() << "✅ Khi MainWindow bị destroy, tất cả sẽ tự động cleanup";
    }
    
    // ✅ KHÔNG CẦN destructor!
    // Qt sẽ tự động delete m_model và m_view vì chúng có parent
    
private slots:
    void onCellClicked(const QModelIndex& index) {
        // ✅ Luôn kiểm tra index hợp lệ
        if (!index.isValid()) {
            qDebug() << "❌ Index không hợp lệ";
            return;
        }
        
        // ✅ Lấy dữ liệu từ model thông qua index
        QVariant data = m_model->data(index, Qt::DisplayRole);
        qDebug() << "Cell clicked:" << index.row() << index.column() 
                 << "Data:" << data.toString();
    }
    
private:
    void addRow(const QString& name, const QString& age, const QString& city) {
        QList<QStandardItem*> items;
        items << new QStandardItem(name);
        items << new QStandardItem(age);
        items << new QStandardItem(city);
        
        // ✅ QStandardItem được model sở hữu
        m_model->appendRow(items);
    }
    
    // ✅ Con trỏ thành viên - được quản lý bởi parent
    QStandardItemModel* m_model;
    QTableView* m_view;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "basic_example.moc"
