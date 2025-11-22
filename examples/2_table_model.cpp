// Ví dụ 2: QStandardItemModel - Table Model linh hoạt
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>

class TableModelExample : public QWidget {
    Q_OBJECT

public:
    TableModelExample(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupModel();
        connectSignals();
    }

private:
    void setupUI() {
        auto *mainLayout = new QVBoxLayout(this);
        
        // Title
        auto *titleLabel = new QLabel("QStandardItemModel - Table Example");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
        mainLayout->addWidget(titleLabel);
        
        // Table View
        tableView = new QTableView();
        tableView->setAlternatingRowColors(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSortingEnabled(true); // Cho phép sort bằng click header
        mainLayout->addWidget(tableView);
        
        // Control Panel
        auto *controlLayout = new QHBoxLayout();
        
        addRowButton = new QPushButton("Thêm hàng");
        removeRowButton = new QPushButton("Xóa hàng đã chọn");
        addColumnButton = new QPushButton("Thêm cột");
        
        // Custom data controls
        auto *customDataLayout = new QHBoxLayout();
        customDataCheckbox = new QCheckBox("Thêm custom data");
        customDataCheckbox->setChecked(true);
        
        customDataLayout->addWidget(new QLabel("Số lượng:"));
        quantitySpinBox = new QSpinBox();
        quantitySpinBox->setRange(1, 1000);
        quantitySpinBox->setValue(10);
        
        customDataLayout->addWidget(quantitySpinBox);
        customDataLayout->addWidget(customDataCheckbox);
        
        controlLayout->addWidget(addRowButton);
        controlLayout->addWidget(removeRowButton);
        controlLayout->addWidget(addColumnButton);
        controlLayout->addLayout(customDataLayout);
        controlLayout->addStretch();
        
        mainLayout->addLayout(controlLayout);
        
        // Info
        infoLabel = new QLabel("Tips: Click header để sort, Double-click cell để edit");
        infoLabel->setStyleSheet("color: #666; padding: 5px;");
        mainLayout->addWidget(infoLabel);
        
        setMinimumSize(800, 500);
        setWindowTitle("Table Model Demo");
    }
    
    void setupModel() {
        // Tạo model với headers
        model = new QStandardItemModel(4, 4, this); // 4 rows, 4 columns
        
        // Set headers
        model->setHorizontalHeaderLabels({"ID", "Tên sản phẩm", "Giá (VND)", "Còn hàng"});
        
        // Thêm dữ liệu mẫu
        QStringList products = {"Laptop Dell", "iPhone 14", "Samsung TV", "iPad Pro"};
        QList<int> prices = {15000000, 25000000, 20000000, 30000000};
        QList<bool> inStock = {true, true, false, true};
        
        for (int row = 0; row < 4; ++row) {
            // ID
            auto *idItem = new QStandardItem(QString::number(row + 1));
            idItem->setEditable(false); // ID không cho edit
            idItem->setTextAlignment(Qt::AlignCenter);
            model->setItem(row, 0, idItem);
            
            // Tên sản phẩm
            auto *nameItem = new QStandardItem(products[row]);
            
            // Thêm icon cho sản phẩm
            if (row == 0) nameItem->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
            else if (row == 1) nameItem->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogYesButton));
            
            // Custom data - có thể lưu thêm thông tin
            nameItem->setData(QVariant::fromValue(quantitySpinBox->value()), Qt::UserRole + 1);
            nameItem->setToolTip("Số lượng: " + QString::number(quantitySpinBox->value()));
            
            model->setItem(row, 1, nameItem);
            
            // Giá
            auto *priceItem = new QStandardItem();
            priceItem->setData(prices[row], Qt::EditRole);
            priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            model->setItem(row, 2, priceItem);
            
            // Còn hàng (checkbox)
            auto *stockItem = new QStandardItem();
            stockItem->setCheckable(true);
            stockItem->setCheckState(inStock[row] ? Qt::Checked : Qt::Unchecked);
            stockItem->setText(inStock[row] ? "Có" : "Hết");
            stockItem->setTextAlignment(Qt::AlignCenter);
            model->setItem(row, 3, stockItem);
            
            // Styling
            if (!inStock[row]) {
                // Highlight hết hàng
                for (int col = 0; col < model->columnCount(); ++col) {
                    model->item(row, col)->setBackground(QBrush(QColor(255, 200, 200)));
                }
            }
        }
        
        // Set model cho view
        tableView->setModel(model);
        
        // Điều chỉnh column width
        tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        tableView->setColumnWidth(0, 60);
        tableView->setColumnWidth(2, 120);
        tableView->setColumnWidth(3, 100);
    }
    
    void connectSignals() {
        // Thêm hàng mới
        connect(addRowButton, &QPushButton::clicked, [this]() {
            int row = model->rowCount();
            model->insertRow(row);
            
            // Thêm dữ liệu mặc định
            model->setItem(row, 0, new QStandardItem(QString::number(row + 1)));
            model->setItem(row, 1, new QStandardItem("Sản phẩm mới"));
            model->setItem(row, 2, new QStandardItem("0"));
            
            auto *stockItem = new QStandardItem();
            stockItem->setCheckable(true);
            stockItem->setCheckState(Qt::Checked);
            stockItem->setText("Có");
            model->setItem(row, 3, stockItem);
            
            updateInfo("Đã thêm hàng mới");
        });
        
        // Xóa hàng
        connect(removeRowButton, &QPushButton::clicked, [this]() {
            QModelIndexList selected = tableView->selectionModel()->selectedRows();
            if (selected.isEmpty()) {
                updateInfo("Vui lòng chọn hàng để xóa");
                return;
            }
            
            // Xóa từ cuối lên để không bị lỗi index
            std::sort(selected.begin(), selected.end(), 
                     [](const QModelIndex &a, const QModelIndex &b) {
                         return a.row() > b.row();
                     });
            
            for (const QModelIndex &index : selected) {
                model->removeRow(index.row());
            }
            
            updateInfo(QString("Đã xóa %1 hàng").arg(selected.size()));
        });
        
        // Thêm cột
        connect(addColumnButton, &QPushButton::clicked, [this]() {
            int col = model->columnCount();
            model->insertColumn(col);
            model->setHeaderData(col, Qt::Horizontal, QString("Cột %1").arg(col + 1));
            updateInfo("Đã thêm cột mới");
        });
        
        // Monitor item changes
        connect(model, &QStandardItemModel::itemChanged, [this](QStandardItem *item) {
            // Xử lý khi checkbox thay đổi
            if (item->column() == 3 && item->isCheckable()) {
                bool inStock = item->checkState() == Qt::Checked;
                item->setText(inStock ? "Có" : "Hết");
                
                // Update row color
                for (int col = 0; col < model->columnCount(); ++col) {
                    auto *cellItem = model->item(item->row(), col);
                    if (cellItem) {
                        cellItem->setBackground(inStock ? 
                            QBrush() : QBrush(QColor(255, 200, 200)));
                    }
                }
            }
            
            // Log custom data nếu có
            if (item->column() == 1) { // Cột tên sản phẩm
                QVariant customData = item->data(Qt::UserRole + 1);
                if (customData.isValid()) {
                    qDebug() << "Custom data (quantity):" << customData.toInt();
                }
            }
            
            updateInfo(QString("Cell [%1,%2] đã thay đổi").arg(item->row()).arg(item->column()));
        });
        
        // Double click to show details
        connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex &index) {
            auto *item = model->itemFromIndex(index);
            if (!item) return;
            
            QString details = QString("Chi tiết:\n");
            details += QString("Row: %1, Column: %2\n").arg(index.row()).arg(index.column());
            details += QString("Data: %1\n").arg(item->text());
            
            // Hiển thị custom data
            QVariant customData = item->data(Qt::UserRole + 1);
            if (customData.isValid()) {
                details += QString("Custom Data (Số lượng): %1").arg(customData.toInt());
            }
            
            QMessageBox::information(this, "Cell Details", details);
        });
    }
    
    void updateInfo(const QString &text) {
        infoLabel->setText(text);
        qDebug() << "Model info - Rows:" << model->rowCount() 
                 << "Columns:" << model->columnCount();
    }

private:
    QTableView *tableView;
    QStandardItemModel *model;
    QPushButton *addRowButton;
    QPushButton *removeRowButton;
    QPushButton *addColumnButton;
    QSpinBox *quantitySpinBox;
    QCheckBox *customDataCheckbox;
    QLabel *infoLabel;
};

#include "2_table_model.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // QStandardItemModel là model đa năng
    // Ưu điểm:
    // - Dễ sử dụng, không cần subclass
    // - Hỗ trợ cả list, table và tree
    // - Có thể lưu custom data với setData()
    // - Built-in checkboxes, icons, tooltips
    // - Hỗ trợ drag & drop
    
    // Nhược điểm:
    // - Không hiệu quả với data lớn
    // - Store toàn bộ data trong memory
    
    // Use cases:
    // - Table/tree với data vừa và nhỏ
    // - Cần edit trực tiếp
    // - Prototype nhanh
    
    TableModelExample window;
    window.show();
    
    return app.exec();
}