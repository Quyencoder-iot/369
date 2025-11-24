#ifndef PRODUCTFILTERPROXYMODEL_H
#define PRODUCTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QAbstractListModel>
#include <QVector>

/**
 * Product Model - Source model cho Proxy
 */
class ProductModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    enum ProductRoles {
        NameRole = Qt::UserRole + 1,
        CategoryRole,
        PriceRole,
        InStockRole,
        RatingRole
    };
    Q_ENUM(ProductRoles)
    
    explicit ProductModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    Q_INVOKABLE void addProduct(const QString &name, const QString &category, 
                               double price, bool inStock, double rating);
    
private:
    struct Product {
        QString name;
        QString category;
        double price;
        bool inStock;
        double rating;
    };
    
    QVector<Product> m_products;
};

/**
 * Example 5: QSortFilterProxyModel - Product Filter
 * 
 * PROXY MODEL:
 * - Wrapper cho source model
 * - Filter & Sort mà KHÔNG thay đổi source
 * - Multiple filter criteria
 */
class ProductFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(QString categoryFilter READ categoryFilter WRITE setCategoryFilter NOTIFY categoryFilterChanged)
    Q_PROPERTY(bool onlyInStock READ onlyInStock WRITE setOnlyInStock NOTIFY onlyInStockChanged)
    Q_PROPERTY(double minPrice READ minPrice WRITE setMinPrice NOTIFY minPriceChanged)
    Q_PROPERTY(double maxPrice READ maxPrice WRITE setMaxPrice NOTIFY maxPriceChanged)
    Q_PROPERTY(double minRating READ minRating WRITE setMinRating NOTIFY minRatingChanged)
    Q_PROPERTY(int filteredCount READ rowCount NOTIFY countChanged)
    
public:
    explicit ProductFilterProxyModel(QObject *parent = nullptr);
    
    QString searchText() const { return m_searchText; }
    QString categoryFilter() const { return m_categoryFilter; }
    bool onlyInStock() const { return m_onlyInStock; }
    double minPrice() const { return m_minPrice; }
    double maxPrice() const { return m_maxPrice; }
    double minRating() const { return m_minRating; }
    
    void setSearchText(const QString &text);
    void setCategoryFilter(const QString &category);
    void setOnlyInStock(bool only);
    void setMinPrice(double price);
    void setMaxPrice(double price);
    void setMinRating(double rating);
    
    Q_INVOKABLE void resetFilters();
    Q_INVOKABLE QStringList getCategories() const;
    Q_INVOKABLE double getAveragePrice() const;
    Q_INVOKABLE int getInStockCount() const;
    
signals:
    void searchTextChanged();
    void categoryFilterChanged();
    void onlyInStockChanged();
    void minPriceChanged();
    void maxPriceChanged();
    void minRatingChanged();
    void countChanged();
    
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    
private:
    QString m_searchText;
    QString m_categoryFilter;
    bool m_onlyInStock;
    double m_minPrice;
    double m_maxPrice;
    double m_minRating;
};

#endif // PRODUCTFILTERPROXYMODEL_H

/**
 * ============================================
 * 📚 SUMMARY - ProductFilterProxyModel Header
 * ============================================
 * 
 * CLASS HIERARCHY:
 * QObject → QAbstractProxyModel → QSortFilterProxyModel → ProductFilterProxyModel
 * 
 * TWO MODELS:
 * 1. ProductModel (Source) - Stores actual data
 * 2. ProductFilterProxyModel (Proxy) - Filters/sorts ProductModel
 * 
 * PROXY PATTERN:
 * ProductModel (Source)
 *     ↓ wraps
 * ProductFilterProxyModel (Proxy)
 *     ↓ displays
 * QML ListView
 * 
 * KEY CONCEPT: KHÔNG THAY ĐỔI SOURCE
 * ✅ Source model data KHÔNG bị sửa
 * ✅ Proxy chỉ hiển thị filtered/sorted view
 * ✅ Nhiều proxy có thể wrap cùng source
 * ✅ Real-time updates khi filter thay đổi
 * 
 * Q_PROPERTY (Filter Criteria):
 * ✅ searchText - Search by product name
 * ✅ categoryFilter - Filter by category
 * ✅ onlyInStock - Show only in-stock items
 * ✅ minPrice/maxPrice - Price range filter
 * ✅ minRating - Minimum rating filter
 * ✅ filteredCount - Number of filtered items (READ rowCount)
 * 
 * OVERRIDE METHODS:
 * ✅ filterAcceptsRow() - Decide if row passes filters
 * ✅ lessThan() - Compare rows for sorting
 * 
 * filterAcceptsRow() LOGIC:
 * bool filterAcceptsRow(int sourceRow, ...) {
 *     Get product from source model
 *     Check ALL criteria:
 *       - Search text matches name?
 *       - Category matches filter?
 *       - Price in range?
 *       - Rating >= minRating?
 *       - In stock (if onlyInStock)?
 *     Return true if ALL pass, false otherwise
 * }
 * 
 * lessThan() LOGIC:
 * bool lessThan(left, right) {
 *     Compare left and right items
 *     Return true if left < right
 *     Used for sorting (ascending/descending)
 * }
 * 
 * Q_INVOKABLE METHODS:
 * ✅ resetFilters() - Clear all filters
 * ✅ getCategories() - Get unique categories list
 * ✅ getAveragePrice() - Calculate filtered avg price
 * ✅ getInStockCount() - Count filtered in-stock items
 * 
 * DATA FLOW (Filtering):
 * User sets categoryFilter = "Electronics"
 *   ↓
 * setCategoryFilter("Electronics")
 *   ↓
 * m_categoryFilter = "Electronics"
 *   ↓
 * emit categoryFilterChanged()
 *   ↓
 * invalidateFilter()  // Trigger re-filtering
 *   ↓
 * Qt calls filterAcceptsRow() for EACH row
 *   ↓
 * Only matching rows pass
 *   ↓
 * emit countChanged()
 *   ↓
 * QML ListView auto-updates
 *   ↓
 * Shows only "Electronics" products
 * 
 * ADVANTAGES:
 * ✅ Source data unchanged
 * ✅ Multiple views possible
 * ✅ Real-time filtering
 * ✅ No data duplication
 * ✅ Efficient (indexes only)
 * ✅ Composable (stack proxies)
 * 
 * ============================================
 */
