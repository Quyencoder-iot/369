// Ví dụ: Nhiều View dùng chung 1 Model
// Compile: g++ -fPIC -std=c++17 multi_view_example.cpp -o multi_view_example $(pkg-config --cflags --libs Qt6Core Qt6Widgets)

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QStandardItemModel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Ví Dụ: Multi-View với Shared Model");
        resize(900, 600);
        
        // ✅ BƯỚC 1: Tạo 1 Model duy nhất
        // Model được MainWindow sở hữu
        m_sharedModel = new QStandardItemModel(this);
        setupModel();
        
        // ✅ BƯỚC 2: Tạo nhiều View khác nhau
        // Tất cả view đều có parent là MainWindow
        m_tableView = new QTableView(this);
        m_treeView = new QTreeView(this);
        m_listView = new QListView(this);
        
        // ✅ BƯỚC 3: Tất cả view dùng CHUNG 1 model
        // View KHÔNG sở hữu model, chỉ tham chiếu
        m_tableView->setModel(m_sharedModel);
        m_treeView->setModel(m_sharedModel);
        m_listView->setModel(m_sharedModel);
        
        // ✅ Kết nối signal để thấy sự đồng bộ
        connect(m_sharedModel, &QStandardItemModel::dataChanged,
                this, &MainWindow::onModelDataChanged);
        
        connect(m_tableView, &QTableView::clicked,
                this, &MainWindow::onTableViewClicked);
        
        // Setup UI
        setupUI();
        
        qDebug() << "✅ 1 Model được share bởi 3 View";
        qDebug() << "✅ Khi model thay đổi, tất cả view tự động cập nhật";
    }
    
private slots:
    void onModelDataChanged(const QModelIndex& topLeft, 
                           const QModelIndex& bottomRight) {
        qDebug() << "📢 Model data changed! All views updated automatically";
        qDebug() << "   Changed range:" << topLeft.row() << topLeft.column()
                 << "to" << bottomRight.row() << bottomRight.column();
    }
    
    void onTableViewClicked(const QModelIndex& index) {
        if (!index.isValid()) return;
        
        QVariant data = m_sharedModel->data(index, Qt::DisplayRole);
        qDebug() << "✅ TableView clicked:" << index.row() << index.column()
                 << "Data:" << data.toString();
        qDebug() << "   TreeView and ListView show the same data!";
    }
    
    void addNewRow() {
        // ✅ Thêm dòng mới vào model
        // Tất cả view sẽ tự động hiển thị dòng mới
        QList<QStandardItem*> items;
        items << new QStandardItem("Item " + QString::number(m_sharedModel->rowCount() + 1));
        items << new QStandardItem("Value " + QString::number(m_sharedModel->rowCount() + 1));
        items << new QStandardItem("Extra " + QString::number(m_sharedModel->rowCount() + 1));
        
        m_sharedModel->appendRow(items);
        
        qDebug() << "✅ New row added to model - all views updated!";
    }
    
private:
    void setupModel() {
        m_sharedModel->setHorizontalHeaderLabels({"Column 1", "Column 2", "Column 3"});
        
        // Thêm dữ liệu mẫu
        for (int i = 0; i < 5; ++i) {
            QList<QStandardItem*> items;
            items << new QStandardItem(QString("Row %1 Col 1").arg(i + 1));
            items << new QStandardItem(QString("Row %1 Col 2").arg(i + 1));
            items << new QStandardItem(QString("Row %1 Col 3").arg(i + 1));
            m_sharedModel->appendRow(items);
        }
    }
    
    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Button để test
        QPushButton* addButton = new QPushButton("Thêm Dòng Mới", this);
        connect(addButton, &QPushButton::clicked, this, &MainWindow::addNewRow);
        mainLayout->addWidget(addButton);
        
        // Splitter chứa các view
        QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(m_tableView);
        splitter->addWidget(m_treeView);
        splitter->addWidget(m_listView);
        
        splitter->setSizes({300, 300, 300});
        mainLayout->addWidget(splitter);
    }
    
    // ✅ 1 Model được share
    QStandardItemModel* m_sharedModel;
    
    // ✅ 3 View khác nhau
    QTableView* m_tableView;
    QTreeView* m_treeView;
    QListView* m_listView;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "multi_view_example.moc"
