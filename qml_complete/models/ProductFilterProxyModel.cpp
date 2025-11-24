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
