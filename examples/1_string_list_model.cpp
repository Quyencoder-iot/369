// Ví dụ 1: QStringListModel - List Model đơn giản nhất
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListView>
#include <QStringListModel>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

class StringListExample : public QWidget {
    Q_OBJECT

public:
    StringListExample(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupModel();
        connectSignals();
    }

private:
    void setupUI() {
        auto *mainLayout = new QVBoxLayout(this);
        
        // Title
        auto *titleLabel = new QLabel("QStringListModel Example");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
        mainLayout->addWidget(titleLabel);
        
        // List View
        listView = new QListView();
        listView->setAlternatingRowColors(true);
        mainLayout->addWidget(listView);
        
        // Input area
        auto *inputLayout = new QHBoxLayout();
        inputEdit = new QLineEdit();
        inputEdit->setPlaceholderText("Nhập text mới...");
        
        addButton = new QPushButton("Thêm");
        removeButton = new QPushButton("Xóa item đã chọn");
        sortButton = new QPushButton("Sắp xếp A-Z");
        
        inputLayout->addWidget(inputEdit);
        inputLayout->addWidget(addButton);
        inputLayout->addWidget(removeButton);
        inputLayout->addWidget(sortButton);
        
        mainLayout->addLayout(inputLayout);
        
        // Info label
        infoLabel = new QLabel("Tip: Double-click để edit item");
        infoLabel->setStyleSheet("color: #666; padding: 5px;");
        mainLayout->addWidget(infoLabel);
        
        setMinimumSize(500, 400);
        setWindowTitle("String List Model Demo");
    }
    
    void setupModel() {
        // Khởi tạo model với dữ liệu mẫu
        QStringList list;
        list << "Apple" << "Banana" << "Cherry" << "Date" << "Elderberry";
        
        model = new QStringListModel(list, this);
        
        // Gán model cho view
        listView->setModel(model);
        
        // Cho phép edit bằng double-click
        listView->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    
    void connectSignals() {
        // Thêm item mới
        connect(addButton, &QPushButton::clicked, [this]() {
            QString text = inputEdit->text().trimmed();
            if (text.isEmpty()) return;
            
            // Cách 1: Lấy list hiện tại, thêm item, set lại
            QStringList list = model->stringList();
            list.append(text);
            model->setStringList(list);
            
            // Cách 2: Dùng insertRows() và setData()
            // int row = model->rowCount();
            // model->insertRows(row, 1);
            // QModelIndex index = model->index(row);
            // model->setData(index, text);
            
            inputEdit->clear();
            updateInfo("Đã thêm: " + text);
        });
        
        // Xóa item đã chọn
        connect(removeButton, &QPushButton::clicked, [this]() {
            QModelIndex index = listView->currentIndex();
            if (!index.isValid()) {
                updateInfo("Vui lòng chọn item để xóa");
                return;
            }
            
            QString itemText = model->data(index).toString();
            model->removeRow(index.row());
            updateInfo("Đã xóa: " + itemText);
        });
        
        // Sắp xếp
        connect(sortButton, &QPushButton::clicked, [this]() {
            model->sort(0, Qt::AscendingOrder);
            updateInfo("Đã sắp xếp A-Z");
        });
        
        // Enter để thêm
        connect(inputEdit, &QLineEdit::returnPressed, addButton, &QPushButton::click);
        
        // Monitor data changes
        connect(model, &QStringListModel::dataChanged, 
                [this](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
            QString newValue = model->data(topLeft).toString();
            updateInfo(QString("Item đã thay đổi thành: %1").arg(newValue));
        });
        
        // Selection changed
        connect(listView->selectionModel(), &QItemSelectionModel::currentChanged,
                [this](const QModelIndex &current, const QModelIndex &previous) {
            if (current.isValid()) {
                QString item = model->data(current).toString();
                updateInfo(QString("Đã chọn: %1 (row %2)").arg(item).arg(current.row()));
            }
        });
    }
    
    void updateInfo(const QString &text) {
        infoLabel->setText(text);
        // Log để học cách model hoạt động
        qDebug() << "Model rowCount:" << model->rowCount();
        qDebug() << "Current list:" << model->stringList();
    }

private:
    QListView *listView;
    QStringListModel *model;
    QLineEdit *inputEdit;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *sortButton;
    QLabel *infoLabel;
};

#include "1_string_list_model.moc"

// Main function với comments giải thích
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // QStringListModel là model đơn giản nhất
    // Ưu điểm:
    // - Dễ sử dụng cho list các string
    // - Tự động hỗ trợ edit
    // - Built-in sort
    
    // Nhược điểm:
    // - Chỉ chứa string, không chứa data phức tạp
    // - Không có nhiều columns
    
    // Use cases:
    // - Danh sách đơn giản (tên, tags, ...)
    // - ComboBox items
    // - Quick prototyping
    
    StringListExample window;
    window.show();
    
    return app.exec();
}