/**
 * ============================================
 * EXAMPLE 5: PRODUCT FILTER PROXY APPLICATION
 * ============================================
 * 
 * C++ Models:
 *   - ProductModel (source) - QAbstractListModel
 *   - ProductFilterProxyModel (proxy) - QSortFilterProxyModel
 * QML View: ProductFilterProxyView.qml
 * 
 * PURPOSE:
 * Demonstrate PROXY MODEL pattern with QML
 * 
 * KEY CONCEPTS:
 * - Source model (original data)
 * - Proxy model (filter/sort wrapper)
 * - Multiple views of same data
 * - Filter without modifying source
 * - Dynamic sorting
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "models/ProductFilterProxyModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // ============================================
    // Create QML Engine
    // ============================================
    QQmlApplicationEngine engine;
    
    // ============================================
    // Create Proxy Model
    // 
    // NOTE: ProxyModel internally creates and manages
    // the source ProductModel. This is one pattern.
    // 
    // Alternative pattern:
    //   ProductModel sourceModel;
    //   ProxyModel proxyModel;
    //   proxyModel.setSourceModel(&sourceModel);
    // ============================================
    ProductFilterProxyModel proxyModel;
    
    // Proxy model already has sample data from internal ProductModel
    qDebug() << "✅ Proxy model created with" << proxyModel.rowCount() << "products";
    
    // ============================================
    // Expose ONLY Proxy Model to QML
    // 
    // KEY POINT:
    // QML sees the proxy model, NOT the source model
    // All filtering/sorting happens transparently
    // ============================================
    engine.rootContext()->setContextProperty("proxyModel", &proxyModel);
    
    // ============================================
    // Load QML
    // ============================================
    const QUrl url(QStringLiteral("qrc:/qml/ProductFilterProxyView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

/**
 * ============================================
 * 📚 SUMMARY - PROXY MODEL INTEGRATION
 * ============================================
 * 
 * 1. PROXY MODEL PATTERN:
 *    
 *    ProductModel (Source)
 *    - Stores actual data
 *    - Provides: name, price, category, stock, etc.
 *    - Unchanged by filtering
 *         ↓ (wrapped by)
 *    ProductFilterProxyModel (Proxy)
 *    - Filters by: category, price, stock
 *    - Sorts by: name, price, stock
 *    - No data storage (references source)
 *         ↓ (displays)
 *    QML ListView
 *    - Shows filtered/sorted view
 *    - User sees subset of data
 * 
 * 2. WHY PROXY MODEL?
 *    ✅ Filter WITHOUT modifying source
 *       → Multiple filtered views possible
 *       → Original data preserved
 *    
 *    ✅ Sort WITHOUT reordering source
 *       → Different sort orders simultaneously
 *       → No data duplication
 *    
 *    ✅ Composable
 *       → Stack multiple proxies
 *       → Combine filters
 *    
 *    ✅ Separation of concerns
 *       → Source model: data storage
 *       → Proxy model: view logic
 * 
 * 3. PROXY MODEL SETUP (C++):
 *    
 *    // Create source
 *    ProductModel *sourceModel = new ProductModel(this);
 *    
 *    // Create proxy
 *    QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
 *    
 *    // Connect proxy to source
 *    proxy->setSourceModel(sourceModel);
 *    
 *    // Configure filtering
 *    proxy->setFilterRole(NameRole);
 *    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
 *    
 *    // Configure sorting
 *    proxy->setSortRole(PriceRole);
 *    proxy->sort(0, Qt::AscendingOrder);
 * 
 * 4. CUSTOM FILTERING:
 *    Override filterAcceptsRow():
 *    
 *    bool filterAcceptsRow(int row, const QModelIndex &parent) override {
 *        QModelIndex index = sourceModel()->index(row, 0, parent);
 *        
 *        // Get data from source
 *        QString name = index.data(NameRole).toString();
 *        double price = index.data(PriceRole).toDouble();
 *        QString category = index.data(CategoryRole).toString();
 *        int stock = index.data(StockRole).toInt();
 *        
 *        // Apply ALL filter criteria
 *        bool matchesSearch = name.contains(m_searchText, Qt::CaseInsensitive);
 *        bool matchesCategory = m_filterCategory.isEmpty() || 
 *                               category == m_filterCategory;
 *        bool matchesPrice = price >= m_minPrice && price <= m_maxPrice;
 *        bool matchesStock = !m_showInStockOnly || stock > 0;
 *        
 *        return matchesSearch && matchesCategory && matchesPrice && matchesStock;
 *    }
 * 
 * 5. CUSTOM SORTING:
 *    Override lessThan():
 *    
 *    bool lessThan(const QModelIndex &left, const QModelIndex &right) override {
 *        switch (m_sortColumn) {
 *            case SortByName:
 *                return left.data(NameRole).toString() < 
 *                       right.data(NameRole).toString();
 *            
 *            case SortByPrice:
 *                return left.data(PriceRole).toDouble() < 
 *                       right.data(PriceRole).toDouble();
 *            
 *            case SortByStock:
 *                return left.data(StockRole).toInt() < 
 *                       right.data(StockRole).toInt();
 *        }
 *        return false;
 *    }
 * 
 * 6. UPDATING FILTERS FROM QML:
 *    
 *    QML:
 *      TextField {
 *          onTextChanged: proxyModel.setSearchText(text)
 *      }
 *      
 *      SpinBox {
 *          onValueChanged: proxyModel.setMinPrice(value)
 *      }
 *    
 *    C++:
 *      void setSearchText(const QString &text) {
 *          m_searchText = text;
 *          invalidateFilter();  // Trigger re-filtering
 *      }
 *      
 *      void setMinPrice(double price) {
 *          m_minPrice = price;
 *          invalidateFilter();
 *      }
 * 
 * 7. ACCESSING SOURCE MODEL DATA:
 *    From proxy model (C++):
 *      QAbstractItemModel *source = sourceModel();
 *      int totalRows = source->rowCount();
 *    
 *    From proxy model (QML):
 *      proxyModel.sourceRowCount  // Custom Q_PROPERTY
 *      proxyModel.rowCount()      // Filtered count
 * 
 * 8. INDEX MAPPING:
 *    Proxy indices ≠ Source indices!
 *    
 *    Convert:
 *      // Proxy → Source
 *      QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
 *      
 *      // Source → Proxy
 *      QModelIndex proxyIndex = proxyModel->mapFromSource(sourceIndex);
 * 
 * 9. STATISTICS:
 *    Can calculate from BOTH models:
 *    
 *    Source model stats (all data):
 *      int totalProducts = sourceModel()->rowCount();
 *      double totalValue = calculateTotal(sourceModel());
 *    
 *    Proxy model stats (filtered data):
 *      int visibleProducts = proxyModel->rowCount();
 *      double visibleValue = calculateTotal(proxyModel);
 * 
 * 10. QML INTEGRATION:
 *     Expose ONLY proxy model:
 *       engine.rootContext()->setContextProperty("proxyModel", &proxy);
 *     
 *     QML uses it like any other model:
 *       ListView {
 *           model: proxyModel  // Not sourceModel!
 *           delegate: Rectangle { text: model.name }
 *       }
 *     
 *     QML calls filter methods:
 *       Button { onClicked: proxyModel.setFilterCategory("Electronics") }
 * 
 * ============================================
 * 💡 LEARNING POINTS
 * ============================================
 * 
 * ★ Proxy wraps source:
 *   QSortFilterProxyModel proxy;
 *   proxy.setSourceModel(sourceModel);
 * 
 * ★ Override for custom logic:
 *   - filterAcceptsRow() → Show/hide rows
 *   - lessThan() → Sort order
 * 
 * ★ Update filters:
 *   void setFilter() {
 *       m_criteria = value;
 *       invalidateFilter();  // Re-evaluate all rows
 *   }
 * 
 * ★ QML sees proxy only:
 *   ListView { model: proxyModel }
 *   → Automatically shows filtered/sorted data
 * 
 * ★ Proxy advantages:
 *   ✅ No data duplication
 *   ✅ Source unchanged
 *   ✅ Multiple views possible
 *   ✅ Real-time updates
 *   ✅ Efficient (indexes only)
 * 
 * ★ Common use cases:
 *   - Search/filter UI
 *   - Multiple sort orders
 *   - Dashboard views
 *   - Reports with different filters
 *   - Log viewers
 *   - Product catalogs
 * 
 * ============================================
 */
