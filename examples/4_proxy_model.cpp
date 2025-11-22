// Ví dụ 4: QSortFilterProxyModel - Lọc và sắp xếp dữ liệu
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QTableView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QDebug>
#include <QRegularExpression>
#include <random>

// Custom proxy model với logic lọc phức tạp
class CustomFilterProxyModel : public QSortFilterProxyModel {
public:
    CustomFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {
        minPrice = 0;
        maxPrice = 999999999;
        showInStockOnly = false;
    }
    
    void setMinPrice(int price) {
        minPrice = price;
        invalidateFilter(); // Trigger re-filter
    }
    
    void setMaxPrice(int price) {
        maxPrice = price;
        invalidateFilter();
    }
    
    void setShowInStockOnly(bool inStock) {
        showInStockOnly = inStock;
        invalidateFilter();
    }

protected:
    // Override để custom filter logic
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        QModelIndex priceIndex = sourceModel()->index(sourceRow, 2, sourceParent);
        QModelIndex stockIndex = sourceModel()->index(sourceRow, 3, sourceParent);
        
        // Check giá
        int price = sourceModel()->data(priceIndex, Qt::EditRole).toInt();
        if (price < minPrice || price > maxPrice) {
            return false;
        }
        
        // Check stock
        if (showInStockOnly) {
            QString stockStatus = sourceModel()->data(stockIndex).toString();
            if (stockStatus != "Còn hàng") {
                return false;
            }
        }
        
        // Apply text filter từ parent class
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    }
    
    // Override để custom sort
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        // Special handling cho cột giá
        if (left.column() == 2) { // Cột giá
            int leftPrice = sourceModel()->data(left, Qt::EditRole).toInt();
            int rightPrice = sourceModel()->data(right, Qt::EditRole).toInt();
            return leftPrice < rightPrice;
        }
        
        return QSortFilterProxyModel::lessThan(left, right);
    }

private:
    int minPrice;
    int maxPrice;
    bool showInStockOnly;
};

class ProxyModelExample : public QWidget {
    Q_OBJECT

public:
    ProxyModelExample(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupModel();
        setupProxyModel();
        connectSignals();
        
        // Update initial info
        updateInfo();
    }

private:
    void setupUI() {
        auto *mainLayout = new QHBoxLayout(this);
        
        // Left panel - Controls
        auto *controlPanel = new QVBoxLayout();
        
        auto *titleLabel = new QLabel("Proxy Model - Filter & Sort Demo");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
        controlPanel->addWidget(titleLabel);
        
        // Search filter
        auto *searchGroup = new QGroupBox("Tìm kiếm");
        auto *searchLayout = new QVBoxLayout(searchGroup);
        
        searchEdit = new QLineEdit();
        searchEdit->setPlaceholderText("Tìm kiếm sản phẩm...");
        
        searchColumnCombo = new QComboBox();
        searchColumnCombo->addItems({"Tất cả cột", "Tên sản phẩm", "Danh mục", "Nhà sản xuất"});
        
        caseSensitiveCheck = new QCheckBox("Phân biệt hoa thường");
        useRegexCheck = new QCheckBox("Sử dụng Regex");
        
        searchLayout->addWidget(new QLabel("Tìm trong:"));
        searchLayout->addWidget(searchColumnCombo);
        searchLayout->addWidget(searchEdit);
        searchLayout->addWidget(caseSensitiveCheck);
        searchLayout->addWidget(useRegexCheck);
        
        controlPanel->addWidget(searchGroup);
        
        // Price filter
        auto *priceGroup = new QGroupBox("Lọc theo giá");
        auto *priceLayout = new QVBoxLayout(priceGroup);
        
        minPriceSpinBox = new QSpinBox();
        minPriceSpinBox->setRange(0, 100000000);
        minPriceSpinBox->setSingleStep(1000000);
        minPriceSpinBox->setPrefix("₫");
        minPriceSpinBox->setSuffix("");
        
        maxPriceSpinBox = new QSpinBox();
        maxPriceSpinBox->setRange(0, 100000000);
        maxPriceSpinBox->setValue(100000000);
        maxPriceSpinBox->setSingleStep(1000000);
        maxPriceSpinBox->setPrefix("₫");
        
        priceLayout->addWidget(new QLabel("Giá tối thiểu:"));
        priceLayout->addWidget(minPriceSpinBox);
        priceLayout->addWidget(new QLabel("Giá tối đa:"));
        priceLayout->addWidget(maxPriceSpinBox);
        
        controlPanel->addWidget(priceGroup);
        
        // Stock filter
        inStockOnlyCheck = new QCheckBox("Chỉ hiện sản phẩm còn hàng");
        controlPanel->addWidget(inStockOnlyCheck);
        
        // Sort options
        auto *sortGroup = new QGroupBox("Sắp xếp");
        auto *sortLayout = new QVBoxLayout(sortGroup);
        
        sortColumnCombo = new QComboBox();
        sortColumnCombo->addItems({"ID", "Tên sản phẩm", "Giá", "Trạng thái"});
        
        sortOrderCombo = new QComboBox();
        sortOrderCombo->addItems({"Tăng dần", "Giảm dần"});
        
        sortButton = new QPushButton("Áp dụng sắp xếp");
        
        sortLayout->addWidget(new QLabel("Sắp xếp theo:"));
        sortLayout->addWidget(sortColumnCombo);
        sortLayout->addWidget(sortOrderCombo);
        sortLayout->addWidget(sortButton);
        
        controlPanel->addWidget(sortGroup);
        
        // Actions
        resetButton = new QPushButton("Reset Filters");
        addRandomButton = new QPushButton("Thêm sản phẩm ngẫu nhiên");
        
        controlPanel->addWidget(resetButton);
        controlPanel->addWidget(addRandomButton);
        
        // Info
        infoLabel = new QLabel();
        infoLabel->setStyleSheet("color: #666; padding: 5px;");
        controlPanel->addWidget(infoLabel);
        
        controlPanel->addStretch();
        
        // Right panel - Views
        auto *viewPanel = new QVBoxLayout();
        
        // Tab widget for different views
        auto *viewsLayout = new QGridLayout();
        
        // Original view
        auto *originalLabel = new QLabel("Source Model (Original Data):");
        originalLabel->setStyleSheet("font-weight: bold;");
        viewsLayout->addWidget(originalLabel, 0, 0);
        
        sourceView = new QTableView();
        sourceView->setAlternatingRowColors(true);
        viewsLayout->addWidget(sourceView, 1, 0);
        
        // Filtered view
        auto *filteredLabel = new QLabel("Proxy Model (Filtered & Sorted):");
        filteredLabel->setStyleSheet("font-weight: bold;");
        viewsLayout->addWidget(filteredLabel, 0, 1);
        
        proxyView = new QTableView();
        proxyView->setAlternatingRowColors(true);
        proxyView->setSortingEnabled(true);
        viewsLayout->addWidget(proxyView, 1, 1);
        
        viewPanel->addLayout(viewsLayout);
        
        // Layout assembly
        mainLayout->addLayout(controlPanel, 1);
        mainLayout->addLayout(viewPanel, 3);
        
        setMinimumSize(1200, 700);
        setWindowTitle("Proxy Model Demo");
    }
    
    void setupModel() {
        // Source model với data mẫu
        sourceModel = new QStandardItemModel(this);
        sourceModel->setHorizontalHeaderLabels({"ID", "Tên sản phẩm", "Giá (VND)", "Trạng thái", "Danh mục", "Nhà sản xuất"});
        
        // Thêm data mẫu
        QStringList products = {
            "iPhone 14 Pro Max", "Samsung Galaxy S23 Ultra", "MacBook Pro 16\"",
            "Dell XPS 15", "iPad Pro 12.9\"", "Surface Pro 9",
            "AirPods Pro", "Sony WH-1000XM5", "Apple Watch Series 8",
            "Samsung Galaxy Watch 5", "LG OLED TV 55\"", "Sony Bravia 65\""
        };
        
        QList<int> prices = {
            30000000, 28000000, 65000000,
            45000000, 25000000, 35000000,
            6000000, 10000000, 12000000,
            8000000, 35000000, 45000000
        };
        
        QStringList categories = {
            "Điện thoại", "Điện thoại", "Laptop",
            "Laptop", "Tablet", "Tablet",
            "Phụ kiện", "Phụ kiện", "Đồng hồ",
            "Đồng hồ", "TV", "TV"
        };
        
        QStringList manufacturers = {
            "Apple", "Samsung", "Apple",
            "Dell", "Apple", "Microsoft",
            "Apple", "Sony", "Apple",
            "Samsung", "LG", "Sony"
        };
        
        for (int i = 0; i < products.size(); ++i) {
            QList<QStandardItem*> row;
            
            // ID
            auto *idItem = new QStandardItem(QString::number(i + 1));
            idItem->setEditable(false);
            row.append(idItem);
            
            // Tên
            row.append(new QStandardItem(products[i]));
            
            // Giá
            auto *priceItem = new QStandardItem();
            priceItem->setData(prices[i], Qt::EditRole);
            priceItem->setData(prices[i], Qt::DisplayRole); // For display formatting
            row.append(priceItem);
            
            // Trạng thái
            bool inStock = (i % 3) != 2; // 2/3 còn hàng
            row.append(new QStandardItem(inStock ? "Còn hàng" : "Hết hàng"));
            
            // Danh mục
            row.append(new QStandardItem(categories[i]));
            
            // Nhà sản xuất
            row.append(new QStandardItem(manufacturers[i]));
            
            sourceModel->appendRow(row);
        }
        
        sourceView->setModel(sourceModel);
        sourceView->resizeColumnsToContents();
    }
    
    void setupProxyModel() {
        // Sử dụng custom proxy model
        proxyModel = new CustomFilterProxyModel(this);
        proxyModel->setSourceModel(sourceModel);
        
        // Configure proxy
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setFilterKeyColumn(-1); // Filter all columns by default
        
        proxyView->setModel(proxyModel);
        proxyView->resizeColumnsToContents();
    }
    
    void connectSignals() {
        // Text search
        connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
            if (useRegexCheck->isChecked()) {
                QRegularExpression regex(text);
                if (!regex.isValid()) {
                    searchEdit->setStyleSheet("background-color: #ffcccc;");
                    return;
                }
                searchEdit->setStyleSheet("");
                proxyModel->setFilterRegularExpression(regex);
            } else {
                searchEdit->setStyleSheet("");
                proxyModel->setFilterFixedString(text);
            }
            updateInfo();
        });
        
        // Search column
        connect(searchColumnCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this](int index) {
            if (index == 0) {
                proxyModel->setFilterKeyColumn(-1); // All columns
            } else if (index == 1) {
                proxyModel->setFilterKeyColumn(1); // Name
            } else if (index == 2) {
                proxyModel->setFilterKeyColumn(4); // Category
            } else if (index == 3) {
                proxyModel->setFilterKeyColumn(5); // Manufacturer
            }
            updateInfo();
        });
        
        // Case sensitivity
        connect(caseSensitiveCheck, &QCheckBox::toggled, [this](bool checked) {
            proxyModel->setFilterCaseSensitivity(checked ? Qt::CaseSensitive : Qt::CaseInsensitive);
            updateInfo();
        });
        
        // Regex mode
        connect(useRegexCheck, &QCheckBox::toggled, [this](bool checked) {
            searchEdit->textChanged(searchEdit->text()); // Re-trigger filter
        });
        
        // Price filters
        connect(minPriceSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [this](int value) {
            proxyModel->setMinPrice(value);
            updateInfo();
        });
        
        connect(maxPriceSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [this](int value) {
            proxyModel->setMaxPrice(value);
            updateInfo();
        });
        
        // Stock filter
        connect(inStockOnlyCheck, &QCheckBox::toggled, [this](bool checked) {
            proxyModel->setShowInStockOnly(checked);
            updateInfo();
        });
        
        // Sort button
        connect(sortButton, &QPushButton::clicked, [this]() {
            int column = sortColumnCombo->currentIndex();
            Qt::SortOrder order = sortOrderCombo->currentIndex() == 0 ? 
                Qt::AscendingOrder : Qt::DescendingOrder;
            
            proxyModel->sort(column, order);
            updateInfo();
        });
        
        // Reset filters
        connect(resetButton, &QPushButton::clicked, [this]() {
            searchEdit->clear();
            searchColumnCombo->setCurrentIndex(0);
            caseSensitiveCheck->setChecked(false);
            useRegexCheck->setChecked(false);
            minPriceSpinBox->setValue(0);
            maxPriceSpinBox->setValue(100000000);
            inStockOnlyCheck->setChecked(false);
            proxyModel->invalidate();
            updateInfo();
        });
        
        // Add random product
        connect(addRandomButton, &QPushButton::clicked, [this]() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<> priceDist(5000000, 50000000);
            static std::uniform_int_distribution<> stockDist(0, 1);
            
            QStringList randomProducts = {
                "Xiaomi Redmi Note", "ASUS ROG Phone", "Google Pixel",
                "OnePlus Nord", "Realme GT", "OPPO Find"
            };
            
            QStringList randomCategories = {"Điện thoại", "Laptop", "Tablet", "Phụ kiện"};
            QStringList randomManufacturers = {"Xiaomi", "ASUS", "Google", "OnePlus"};
            
            int id = sourceModel->rowCount() + 1;
            QString product = randomProducts[gen() % randomProducts.size()] + 
                            " " + QString::number(gen() % 10);
            
            QList<QStandardItem*> row;
            row.append(new QStandardItem(QString::number(id)));
            row.append(new QStandardItem(product));
            
            auto *priceItem = new QStandardItem();
            int price = priceDist(gen);
            priceItem->setData(price, Qt::EditRole);
            row.append(priceItem);
            
            row.append(new QStandardItem(stockDist(gen) ? "Còn hàng" : "Hết hàng"));
            row.append(new QStandardItem(randomCategories[gen() % randomCategories.size()]));
            row.append(new QStandardItem(randomManufacturers[gen() % randomManufacturers.size()]));
            
            sourceModel->appendRow(row);
            updateInfo("Đã thêm: " + product);
        });
        
        // Monitor proxy model changes
        connect(proxyModel, &QSortFilterProxyModel::layoutChanged, [this]() {
            proxyView->resizeColumnsToContents();
        });
    }
    
    void updateInfo(const QString &extraInfo = "") {
        int sourceRows = sourceModel->rowCount();
        int filteredRows = proxyModel->rowCount();
        
        QString info = QString("Tổng: %1 | Hiển thị: %2")
            .arg(sourceRows).arg(filteredRows);
        
        if (!extraInfo.isEmpty()) {
            info += " | " + extraInfo;
        }
        
        infoLabel->setText(info);
        
        // Debug output
        qDebug() << "\n=== Proxy Model Info ===";
        qDebug() << "Source rows:" << sourceRows;
        qDebug() << "Filtered rows:" << filteredRows;
        qDebug() << "Filter pattern:" << proxyModel->filterRegularExpression().pattern();
        qDebug() << "Filter column:" << proxyModel->filterKeyColumn();
    }

private:
    // Models
    QStandardItemModel *sourceModel;
    CustomFilterProxyModel *proxyModel;
    
    // Views
    QTableView *sourceView;
    QTableView *proxyView;
    
    // Controls
    QLineEdit *searchEdit;
    QComboBox *searchColumnCombo;
    QCheckBox *caseSensitiveCheck;
    QCheckBox *useRegexCheck;
    QSpinBox *minPriceSpinBox;
    QSpinBox *maxPriceSpinBox;
    QCheckBox *inStockOnlyCheck;
    QComboBox *sortColumnCombo;
    QComboBox *sortOrderCombo;
    QPushButton *sortButton;
    QPushButton *resetButton;
    QPushButton *addRandomButton;
    QLabel *infoLabel;
};

#include "4_proxy_model.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // QSortFilterProxyModel
    // Ưu điểm:
    // - Không thay đổi source model
    // - Có thể chain nhiều proxy models
    // - Filter và sort hiệu quả
    // - Support regex
    // - Dễ dàng toggle filter on/off
    
    // Key concepts:
    // - setSourceModel(): Kết nối với source
    // - mapToSource(): Convert proxy index to source index  
    // - mapFromSource(): Convert source index to proxy index
    // - invalidateFilter(): Re-run filter
    
    // Use cases:
    // - Search/filter trong list lớn
    // - Sort mà không ảnh hưởng data gốc
    // - Multiple views với filters khác nhau
    // - Temporary filters (undo được)
    
    ProxyModelExample window;
    window.show();
    
    return app.exec();
}