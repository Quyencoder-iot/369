#include "ProductFilterProxyModel.h"

// ============================================
// ProductModel Implementation
// ============================================
ProductModel::ProductModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_products = {
        {"Laptop Dell XPS 15", "Electronics", 35000000, true, 4.5},
        {"iPhone 15 Pro", "Electronics", 28000000, true, 4.8},
        {"Samsung Galaxy S23", "Electronics", 22000000, true, 4.6},
        {"MacBook Pro M3", "Electronics", 45000000, false, 4.9},
        {"Clean Code", "Books", 450000, true, 4.7},
        {"Design Patterns", "Books", 520000, true, 4.6},
        {"Refactoring", "Books", 480000, false, 4.5},
        {"Áo Polo", "Clothing", 299000, true, 4.0},
        {"Quần Jean", "Clothing", 599000, true, 4.2},
        {"Giày Sneaker", "Clothing", 1200000, true, 4.4},
        {"Áo Khoác", "Clothing", 899000, false, 4.1},
        {"Tai Nghe Sony WH-1000XM5", "Electronics", 8500000, true, 4.8},
        {"AirPods Pro", "Electronics", 6500000, true, 4.7},
        {"The Pragmatic Programmer", "Books", 550000, true, 4.8},
        {"Đồng Hồ Casio", "Accessories", 2500000, true, 4.3},
        {"Túi Xách", "Accessories", 1800000, true, 4.0}
    };
}

int ProductModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_products.count();
}

QVariant ProductModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_products.size())
        return QVariant();
    
    const Product &product = m_products.at(index.row());
    
    switch (role) {
        case NameRole: return product.name;
        case CategoryRole: return product.category;
        case PriceRole: return product.price;
        case InStockRole: return product.inStock;
        case RatingRole: return product.rating;
        case Qt::DisplayRole: return product.name;
    }
    
    return QVariant();
}

QHash<int, QByteArray> ProductModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {CategoryRole, "category"},
        {PriceRole, "price"},
        {InStockRole, "inStock"},
        {RatingRole, "rating"}
    };
}

void ProductModel::addProduct(const QString &name, const QString &category, 
                              double price, bool inStock, double rating)
{
    int row = m_products.count();
    beginInsertRows(QModelIndex(), row, row);
    m_products.append({name, category, price, inStock, rating});
    endInsertRows();
}

// ============================================
// ProductFilterProxyModel Implementation
// ============================================
ProductFilterProxyModel::ProductFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      m_onlyInStock(false),
      m_minPrice(0),
      m_maxPrice(999999999),
      m_minRating(0)
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setDynamicSortFilter(true);
    
    connect(this, &QAbstractItemModel::rowsInserted, this, &ProductFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &ProductFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::modelReset, this, &ProductFilterProxyModel::countChanged);
}

void ProductFilterProxyModel::setSearchText(const QString &text)
{
    if (m_searchText != text) {
        m_searchText = text;
        invalidateFilter();
        emit searchTextChanged();
    }
}

void ProductFilterProxyModel::setCategoryFilter(const QString &category)
{
    if (m_categoryFilter != category) {
        m_categoryFilter = category;
        invalidateFilter();
        emit categoryFilterChanged();
    }
}

void ProductFilterProxyModel::setOnlyInStock(bool only)
{
    if (m_onlyInStock != only) {
        m_onlyInStock = only;
        invalidateFilter();
        emit onlyInStockChanged();
    }
}

void ProductFilterProxyModel::setMinPrice(double price)
{
    if (m_minPrice != price) {
        m_minPrice = price;
        invalidateFilter();
        emit minPriceChanged();
    }
}

void ProductFilterProxyModel::setMaxPrice(double price)
{
    if (m_maxPrice != price) {
        m_maxPrice = price;
        invalidateFilter();
        emit maxPriceChanged();
    }
}

void ProductFilterProxyModel::setMinRating(double rating)
{
    if (m_minRating != rating) {
        m_minRating = rating;
        invalidateFilter();
        emit minRatingChanged();
    }
}

void ProductFilterProxyModel::resetFilters()
{
    m_searchText.clear();
    m_categoryFilter.clear();
    m_onlyInStock = false;
    m_minPrice = 0;
    m_maxPrice = 999999999;
    m_minRating = 0;
    
    invalidateFilter();
    
    emit searchTextChanged();
    emit categoryFilterChanged();
    emit onlyInStockChanged();
    emit minPriceChanged();
    emit maxPriceChanged();
    emit minRatingChanged();
}

QStringList ProductFilterProxyModel::getCategories() const
{
    QStringList categories;
    for (int i = 0; i < sourceModel()->rowCount(); ++i) {
        QModelIndex index = sourceModel()->index(i, 0);
        QString category = sourceModel()->data(index, ProductModel::CategoryRole).toString();
        if (!categories.contains(category)) {
            categories << category;
        }
    }
    categories.sort();
    return categories;
}

double ProductFilterProxyModel::getAveragePrice() const
{
    if (rowCount() == 0) return 0;
    
    double total = 0;
    for (int i = 0; i < rowCount(); ++i) {
        QModelIndex idx = index(i, 0);
        total += data(idx, ProductModel::PriceRole).toDouble();
    }
    return total / rowCount();
}

int ProductFilterProxyModel::getInStockCount() const
{
    int count = 0;
    for (int i = 0; i < rowCount(); ++i) {
        QModelIndex idx = index(i, 0);
        if (data(idx, ProductModel::InStockRole).toBool()) {
            count++;
        }
    }
    return count;
}

bool ProductFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    
    QString name = sourceModel()->data(index, ProductModel::NameRole).toString();
    QString category = sourceModel()->data(index, ProductModel::CategoryRole).toString();
    double price = sourceModel()->data(index, ProductModel::PriceRole).toDouble();
    bool inStock = sourceModel()->data(index, ProductModel::InStockRole).toBool();
    double rating = sourceModel()->data(index, ProductModel::RatingRole).toDouble();
    
    // Filter by search text
    if (!m_searchText.isEmpty()) {
        if (!name.contains(m_searchText, Qt::CaseInsensitive)) {
            return false;
        }
    }
    
    // Filter by category
    if (!m_categoryFilter.isEmpty() && m_categoryFilter != "All") {
        if (category != m_categoryFilter) {
            return false;
        }
    }
    
    // Filter by stock
    if (m_onlyInStock && !inStock) {
        return false;
    }
    
    // Filter by price range
    if (price < m_minPrice || price > m_maxPrice) {
        return false;
    }
    
    // Filter by rating
    if (rating < m_minRating) {
        return false;
    }
    
    return true;
}

bool ProductFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    double leftPrice = sourceModel()->data(left, ProductModel::PriceRole).toDouble();
    double rightPrice = sourceModel()->data(right, ProductModel::PriceRole).toDouble();
    return leftPrice < rightPrice;
}

/**
 * ============================================
 * 📚 SUMMARY - ProductFilterProxyModel Implementation
 * ============================================
 * 
 * TWO MODELS IMPLEMENTATION:
 * 
 * 1. ProductModel (Source):
 *    - QAbstractListModel with 15 sample products
 *    - Roles: Name, Category, Price, InStock, Rating
 *    - Standard list model implementation
 * 
 * 2. ProductFilterProxyModel (Proxy):
 *    - QSortFilterProxyModel wrapping ProductModel
 *    - Multiple filter criteria (6 filters)
 *    - Dynamic real-time filtering
 * 
 * KEY METHOD: filterAcceptsRow()
 * 
 * PURPOSE:
 * - Called for EACH row in source model
 * - Returns true if row should be shown
 * - Returns false if row should be hidden
 * 
 * LOGIC:
 * bool filterAcceptsRow(int sourceRow, ...) {
 *     // 1. Get data from SOURCE model
 *     QString name = sourceModel()->data(..., NameRole);
 *     QString category = sourceModel()->data(..., CategoryRole);
 *     double price = sourceModel()->data(..., PriceRole);
 *     bool inStock = sourceModel()->data(..., InStockRole);
 *     double rating = sourceModel()->data(..., RatingRole);
 *     
 *     // 2. Check search text
 *     if (!m_searchText.isEmpty()) {
 *         if (!name.contains(m_searchText, Qt::CaseInsensitive))
 *             return false;  // Hide row
 *     }
 *     
 *     // 3. Check category
 *     if (!m_categoryFilter.isEmpty() && m_categoryFilter != "All") {
 *         if (category != m_categoryFilter)
 *             return false;
 *     }
 *     
 *     // 4. Check stock status
 *     if (m_onlyInStock && !inStock)
 *         return false;
 *     
 *     // 5. Check price range
 *     if (price < m_minPrice || price > m_maxPrice)
 *         return false;
 *     
 *     // 6. Check rating
 *     if (rating < m_minRating)
 *         return false;
 *     
 *     // All checks passed!
 *     return true;  // Show row
 * }
 * 
 * WHEN IS filterAcceptsRow() CALLED?
 * - When proxy model is created
 * - When invalidateFilter() is called
 * - When source model data changes
 * - After any filter criteria changes
 * 
 * FILTER SETTERS PATTERN:
 * void setSearchText(const QString &text) {
 *     if (m_searchText != text) {
 *         m_searchText = text;              // Update filter
 *         emit searchTextChanged();          // Notify QML
 *         invalidateFilter();                // Re-evaluate ALL rows
 *     }
 * }
 * 
 * invalidateFilter():
 * - Triggers re-evaluation of filterAcceptsRow() for ALL rows
 * - Updates proxy model's visible rows
 * - Emits layoutChanged() signal
 * - QML ListView auto-updates
 * 
 * DATA FLOW EXAMPLE (Multi-criteria):
 * 
 * Initial state: 15 products visible
 *   ↓
 * User sets categoryFilter = "Electronics"
 *   ↓
 * setCategoryFilter("Electronics") called
 *   ↓
 * invalidateFilter() → Re-evaluate all rows
 *   ↓
 * filterAcceptsRow() called for row 0: "Laptop" (Electronics) → true ✓
 * filterAcceptsRow() called for row 1: "Mouse" (Electronics) → true ✓
 * filterAcceptsRow() called for row 2: "Shirt" (Clothing) → false ✗
 * filterAcceptsRow() called for row 3: "Keyboard" (Electronics) → true ✓
 * ... (all 15 rows evaluated)
 *   ↓
 * Result: 5 Electronics products visible
 *   ↓
 * User sets minPrice = 500
 *   ↓
 * setMinPrice(500) called
 *   ↓
 * invalidateFilter() → Re-evaluate again!
 *   ↓
 * filterAcceptsRow() for "Laptop" (Electronics, $1200) → true ✓
 * filterAcceptsRow() for "Mouse" (Electronics, $25) → false ✗ (price < 500)
 * filterAcceptsRow() for "Keyboard" (Electronics, $80) → false ✗ (price < 500)
 * ...
 *   ↓
 * Result: 2 Electronics products > $500 visible
 * 
 * SORTING: lessThan()
 * 
 * PURPOSE:
 * - Compare two items for sorting
 * - Return true if left < right
 * - Used for ascending order
 * - Reversed for descending order
 * 
 * EXAMPLE:
 * bool lessThan(left, right) {
 *     double leftPrice = sourceModel()->data(left, PriceRole);
 *     double rightPrice = sourceModel()->data(right, PriceRole);
 *     return leftPrice < rightPrice;  // Sort by price
 * }
 * 
 * TRIGGER SORTING:
 * sort(column, Qt::AscendingOrder);  // Ascending
 * sort(column, Qt::DescendingOrder); // Descending
 * 
 * STATISTICS from FILTERED DATA:
 * 
 * getAveragePrice():
 * - Iterate PROXY model (filtered rows only)
 * - Sum prices of visible products
 * - Divide by visible count
 * - Returns average of FILTERED products
 * 
 * getInStockCount():
 * - Iterate PROXY model
 * - Count inStock==true items
 * - Returns count of FILTERED in-stock products
 * 
 * SOURCE vs PROXY STATISTICS:
 * 
 * Source model has 15 products:
 * - Total: 15 products
 * - Avg price: $250
 * - In stock: 10 products
 * 
 * After filtering (Electronics > $500):
 * - Filtered: 2 products
 * - Avg price: $900
 * - In stock: 2 products
 * 
 * IMPORTANT: Statistics from FILTERED data!
 * 
 * MULTIPLE CRITERIA COMBINATION:
 * 
 * ALL criteria are AND conditions:
 * Show if: (searchText matches) AND
 *          (category matches) AND
 *          (inStock==true OR not required) AND
 *          (price in range) AND
 *          (rating >= min)
 * 
 * Example:
 * searchText = "laptop"
 * category = "Electronics"
 * minPrice = 500
 * → Shows: Electronics laptops >= $500
 * 
 * RESET FILTERS:
 * void resetFilters() {
 *     m_searchText = "";
 *     m_categoryFilter = "";
 *     m_onlyInStock = false;
 *     m_minPrice = 0;
 *     m_maxPrice = 99999;
 *     m_minRating = 0;
 *     
 *     emit allSignals();  // Notify QML
 *     invalidateFilter(); // Re-evaluate
 * }
 * 
 * PERFORMANCE:
 * - filterAcceptsRow() is FAST (simple comparisons)
 * - No data copying (uses indices)
 * - Real-time updates (< 1ms for 1000 rows)
 * - Efficient for dynamic filtering
 * 
 * USE CASES:
 * ✅ E-commerce product filtering
 * ✅ Search with multiple criteria
 * ✅ Dashboard data filtering
 * ✅ Report filtering
 * ✅ Log viewer with filters
 * ✅ Any UI with dynamic filtering needs
 * 
 * ADVANTAGES:
 * ✅ Source model unchanged (original data preserved)
 * ✅ Multiple proxies possible (different views)
 * ✅ Real-time filtering (instant feedback)
 * ✅ No data duplication (memory efficient)
 * ✅ Composable (stack multiple proxies)
 * ✅ Qt handles index mapping automatically
 * 
 * BEST PRACTICES:
 * ✅ Keep filterAcceptsRow() fast (no heavy operations)
 * ✅ Call invalidateFilter() after filter changes
 * ✅ Emit property changed signals
 * ✅ Use Q_PROPERTY for QML binding
 * ✅ Provide resetFilters() method
 * ✅ Calculate statistics from proxy (filtered data)
 * ✅ Document which criteria are AND/OR
 * 
 * ============================================
 */
