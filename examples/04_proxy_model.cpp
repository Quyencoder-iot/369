/**
 * VÍ DỤ 4: QSortFilterProxyModel - Filter và Sort
 * 
 * ĐẶC ĐIỂM:
 * - Wrapper cho source model
 * - Không thay đổi source data
 * - Filter: Ẩn/hiện items
 * - Sort: Sắp xếp items
 */

#include <QSortFilterProxyModel>
#include <QAbstractListModel>
#include <QListView>
#include <QApplication>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

// ============================================
// Source Model: Danh sách Sản phẩm
// ============================================
class ProductModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    struct Product {
        QString name;
        QString category; // "Electronics", "Books", "Clothing"
        double price;
        bool inStock;
    };
    
private:
    QVector<Product> m_products;
    
public:
    ProductModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        m_products = {
            {"Laptop Dell", "Electronics", 15000000, true},
            {"Áo sơ mi", "Clothing", 299000, true},
            {"Clean Code", "Books", 450000, false},
            {"iPhone 15", "Electronics", 25000000, true},
            {"Quần jeans", "Clothing", 599000, true},
            {"Design Patterns", "Books", 520000, true},
            {"Tai nghe Sony", "Electronics", 2500000, false},
            {"Giày thể thao", "Clothing", 1200000, true},
            {"Refactoring", "Books", 480000, true},
            {"MacBook Pro", "Electronics", 35000000, false}
        };
    }
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return parent.isValid() ? 0 : m_products.count();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_products.size())
            return QVariant();
        
        const Product &product = m_products.at(index.row());
        
        switch (role) {
            case Qt::DisplayRole:
                return QString("%1 - %2đ [%3]")
                    .arg(product.name)
                    .arg(product.price, 0, 'f', 0)
                    .arg(product.category);
                
            case Qt::UserRole: // name
                return product.name;
            case Qt::UserRole + 1: // category
                return product.category;
            case Qt::UserRole + 2: // price
                return product.price;
            case Qt::UserRole + 3: // inStock
                return product.inStock;
                
            case Qt::ForegroundRole:
                return product.inStock ? QColor(Qt::black) : QColor(Qt::gray);
                
            case Qt::FontRole: {
                QFont font;
                if (!product.inStock)
                    font.setStrikeOut(true);
                return font;
            }
        }
        
        return QVariant();
    }
    
    void addProduct(const QString &name, const QString &category, 
                   double price, bool inStock) {
        int row = m_products.count();
        beginInsertRows(QModelIndex(), row, row);
        m_products.append({name, category, price, inStock});
        endInsertRows();
    }
};

// ============================================
// Custom Proxy: Filter theo nhiều tiêu chí
// ============================================
class ProductFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
    
    QString m_searchText;
    QString m_categoryFilter;
    bool m_onlyInStock = false;
    double m_maxPrice = 999999999;
    
public:
    ProductFilterProxy(QObject *parent = nullptr) 
        : QSortFilterProxyModel(parent) {
        // Case insensitive sorting
        setSortCaseSensitivity(Qt::CaseInsensitive);
    }
    
    // ----------------------------------------
    // Filter logic
    // ----------------------------------------
    bool filterAcceptsRow(int sourceRow, 
                         const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        
        // Lấy data từ source model
        QString name = sourceModel()->data(index, Qt::UserRole).toString();
        QString category = sourceModel()->data(index, Qt::UserRole + 1).toString();
        double price = sourceModel()->data(index, Qt::UserRole + 2).toDouble();
        bool inStock = sourceModel()->data(index, Qt::UserRole + 3).toBool();
        
        // Filter 1: Search text
        if (!m_searchText.isEmpty()) {
            if (!name.contains(m_searchText, Qt::CaseInsensitive))
                return false;
        }
        
        // Filter 2: Category
        if (!m_categoryFilter.isEmpty() && m_categoryFilter != "All") {
            if (category != m_categoryFilter)
                return false;
        }
        
        // Filter 3: Only in stock
        if (m_onlyInStock && !inStock)
            return false;
        
        // Filter 4: Max price
        if (price > m_maxPrice)
            return false;
        
        return true; // Chấp nhận row này
    }
    
    // ----------------------------------------
    // Sort logic (optional override)
    // ----------------------------------------
    bool lessThan(const QModelIndex &left, 
                 const QModelIndex &right) const override {
        // Sort theo giá
        double leftPrice = sourceModel()->data(left, Qt::UserRole + 2).toDouble();
        double rightPrice = sourceModel()->data(right, Qt::UserRole + 2).toDouble();
        return leftPrice < rightPrice;
    }
    
    // ----------------------------------------
    // Public setters
    // ----------------------------------------
public slots:
    void setSearchText(const QString &text) {
        m_searchText = text;
        invalidateFilter(); // Refresh filter
    }
    
    void setCategoryFilter(const QString &category) {
        m_categoryFilter = category;
        invalidateFilter();
    }
    
    void setOnlyInStock(bool only) {
        m_onlyInStock = only;
        invalidateFilter();
    }
    
    void setMaxPrice(double price) {
        m_maxPrice = price;
        invalidateFilter();
    }
};

// ============================================
// Main Application
// ============================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // Source model
    ProductModel *sourceModel = new ProductModel;
    
    // Proxy model
    ProductFilterProxy *proxyModel = new ProductFilterProxy;
    proxyModel->setSourceModel(sourceModel);
    proxyModel->sort(0); // Enable sorting
    
    // View
    QListView *view = new QListView;
    view->setModel(proxyModel); // View dùng PROXY, không phải source!
    
    // ----------------------------------------
    // Filter controls
    // ----------------------------------------
    
    // Search box
    QLineEdit *searchBox = new QLineEdit;
    searchBox->setPlaceholderText("Tìm kiếm sản phẩm...");
    QObject::connect(searchBox, &QLineEdit::textChanged,
                    proxyModel, &ProductFilterProxy::setSearchText);
    
    // Category filter
    QComboBox *categoryCombo = new QComboBox;
    categoryCombo->addItems({"All", "Electronics", "Books", "Clothing"});
    QObject::connect(categoryCombo, &QComboBox::currentTextChanged,
                    proxyModel, &ProductFilterProxy::setCategoryFilter);
    
    // In stock only
    QCheckBox *inStockCheck = new QCheckBox("Chỉ còn hàng");
    QObject::connect(inStockCheck, &QCheckBox::toggled,
                    proxyModel, &ProductFilterProxy::setOnlyInStock);
    
    // Max price
    QLineEdit *maxPriceEdit = new QLineEdit;
    maxPriceEdit->setPlaceholderText("Giá tối đa");
    QObject::connect(maxPriceEdit, &QLineEdit::textChanged, [=](const QString &text) {
        double price = text.isEmpty() ? 999999999 : text.toDouble();
        proxyModel->setMaxPrice(price);
    });
    
    // Sort buttons
    QPushButton *sortAscBtn = new QPushButton("Giá tăng dần");
    QPushButton *sortDescBtn = new QPushButton("Giá giảm dần");
    
    QObject::connect(sortAscBtn, &QPushButton::clicked, [=]() {
        proxyModel->sort(0, Qt::AscendingOrder);
    });
    
    QObject::connect(sortDescBtn, &QPushButton::clicked, [=]() {
        proxyModel->sort(0, Qt::DescendingOrder);
    });
    
    // Count label
    QLabel *countLabel = new QLabel;
    auto updateCount = [=]() {
        int total = sourceModel->rowCount();
        int filtered = proxyModel->rowCount();
        countLabel->setText(QString("Hiển thị %1 / %2 sản phẩm")
                           .arg(filtered).arg(total));
    };
    
    QObject::connect(proxyModel, &QAbstractItemModel::modelReset, updateCount);
    QObject::connect(proxyModel, &QAbstractItemModel::rowsInserted, updateCount);
    QObject::connect(proxyModel, &QAbstractItemModel::rowsRemoved, updateCount);
    updateCount();
    
    // ----------------------------------------
    // Layout
    // ----------------------------------------
    layout->addWidget(new QLabel("<b>Filters:</b>"));
    layout->addWidget(searchBox);
    layout->addWidget(categoryCombo);
    layout->addWidget(inStockCheck);
    layout->addWidget(maxPriceEdit);
    
    QHBoxLayout *sortLayout = new QHBoxLayout;
    sortLayout->addWidget(sortAscBtn);
    sortLayout->addWidget(sortDescBtn);
    layout->addLayout(sortLayout);
    
    layout->addWidget(countLabel);
    layout->addWidget(view);
    
    window.resize(500, 600);
    window.setWindowTitle("Product Filter Demo");
    window.show();
    
    return app.exec();
}

#include "04_proxy_model.moc"

// ============================================
// CÁCH ĐỌC CODE PROXY MODEL:
// ============================================
// 1. Tìm source model: setSourceModel()
// 2. filterAcceptsRow(): 
//    - Nhận sourceRow
//    - Lấy data từ sourceModel()->data(...)
//    - Return true/false để hiện/ẩn row
// 3. lessThan(): Logic so sánh cho sorting
// 4. invalidateFilter(): Gọi để refresh filter
// 5. sort(): Trigger sorting
//
// MAPPING:
// - proxyModel->mapToSource(proxyIndex) → sourceIndex
// - proxyModel->mapFromSource(sourceIndex) → proxyIndex
//
// VIEW luôn dùng PROXY index, không dùng SOURCE index!
