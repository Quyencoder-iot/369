import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * ============================================
 * EXAMPLE 5: PRODUCT FILTER PROXY VIEW (QML)
 * ============================================
 * 
 * Model: ProductFilterProxyModel (C++ - QSortFilterProxyModel)
 * Source: ProductModel (C++ - QAbstractListModel)
 * 
 * MỤC ĐÍCH:
 * - Filter products by category, price, stock
 * - Sort by name, price, stock
 * - Search by name
 * - Display statistics
 * - KHÔNG thay đổi source model
 * 
 * KEY CONCEPTS:
 * 1. QSortFilterProxyModel - Filter/sort wrapper
 * 2. Source model - Original data
 * 3. Proxy model - Filtered/sorted view
 * 4. Dynamic filtering - Update filters in real-time
 * 5. Statistics từ cả source và proxy model
 */

ApplicationWindow {
    id: root
    visible: true
    width: 900
    height: 750
    title: qsTr("🛍️ Product Filter Proxy - QML View")
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // ============================================
        // HEADER
        // ============================================
        Label {
            text: qsTr("🛍️ Product Inventory System")
            font.pixelSize: 24
            font.bold: true
        }
        
        // ============================================
        // STATISTICS
        // So sánh Source vs Proxy model
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            height: 70
            color: "#f3e5f5"
            radius: 8
            
            GridLayout {
                anchors.fill: parent
                anchors.margins: 10
                columns: 2
                rowSpacing: 5
                columnSpacing: 20
                
                // Source model stats
                Label {
                    text: qsTr("📦 Total Products (Source): ") + proxyModel.sourceRowCount
                    font.pixelSize: 14
                    color: "#666"
                }
                
                Label {
                    text: qsTr("💰 Total Value: $") + proxyModel.sourceTotalValue.toFixed(2)
                    font.pixelSize: 14
                    color: "#666"
                }
                
                // Proxy model stats (filtered)
                Label {
                    text: qsTr("✅ Filtered Products: ") + proxyModel.rowCount()
                    font.pixelSize: 15
                    font.bold: true
                    color: "#7b1fa2"
                }
                
                Label {
                    text: qsTr("💵 Filtered Value: $") + proxyModel.filteredTotalValue.toFixed(2)
                    font.pixelSize: 15
                    font.bold: true
                    color: "#7b1fa2"
                }
            }
        }
        
        // ============================================
        // FILTER CONTROLS
        // 
        // KEY CONCEPT:
        // - Thay đổi filters → ProxyModel tự động update
        // - ListView hiển thị PROXY model (filtered data)
        // - Source model không bị thay đổi
        // ============================================
        GroupBox {
            title: qsTr("🔍 Filters & Sort")
            Layout.fillWidth: true
            
            GridLayout {
                anchors.fill: parent
                columns: 6
                columnSpacing: 15
                rowSpacing: 10
                
                // ========== SEARCH ==========
                Label { text: qsTr("Search:") }
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Search products...")
                    
                    onTextChanged: {
                        // ★ UPDATE PROXY FILTER
                        proxyModel.setSearchText(text)
                    }
                }
                
                // ========== CATEGORY FILTER ==========
                Label { text: qsTr("Category:") }
                ComboBox {
                    id: categoryCombo
                    Layout.preferredWidth: 150
                    model: ["All", "Electronics", "Clothing", "Food", "Books"]
                    
                    onCurrentTextChanged: {
                        // ★ UPDATE PROXY FILTER
                        proxyModel.setFilterCategory(currentText === "All" ? "" : currentText)
                    }
                }
                
                // ========== MIN PRICE ==========
                Label { text: qsTr("Min $:") }
                SpinBox {
                    id: minPriceSpinBox
                    from: 0
                    to: 10000
                    stepSize: 10
                    value: 0
                    editable: true
                    
                    onValueChanged: {
                        // ★ UPDATE PROXY FILTER
                        proxyModel.setMinPrice(value)
                    }
                }
                
                // ========== MAX PRICE ==========
                Label { text: qsTr("Max $:") }
                SpinBox {
                    id: maxPriceSpinBox
                    from: 0
                    to: 10000
                    stepSize: 10
                    value: 10000
                    editable: true
                    
                    onValueChanged: {
                        // ★ UPDATE PROXY FILTER
                        proxyModel.setMaxPrice(value)
                    }
                }
                
                // ========== IN STOCK ONLY ==========
                Label { text: qsTr("In Stock:") }
                CheckBox {
                    id: inStockCheckBox
                    checked: false
                    
                    onCheckedChanged: {
                        // ★ UPDATE PROXY FILTER
                        proxyModel.setShowInStockOnly(checked)
                    }
                }
                
                // ========== SORT ==========
                Label { text: qsTr("Sort by:") }
                ComboBox {
                    id: sortCombo
                    Layout.preferredWidth: 150
                    model: ["Name ↑", "Name ↓", "Price ↑", "Price ↓", "Stock ↑", "Stock ↓"]
                    
                    onCurrentIndexChanged: {
                        // ★ UPDATE PROXY SORT
                        var ascending = (currentIndex % 2 === 0)
                        
                        if (currentIndex < 2) {
                            proxyModel.sortByName(ascending)
                        } else if (currentIndex < 4) {
                            proxyModel.sortByPrice(ascending)
                        } else {
                            proxyModel.sortByStock(ascending)
                        }
                    }
                }
                
                // ========== RESET BUTTON ==========
                Button {
                    text: qsTr("🔄 Reset Filters")
                    Layout.columnSpan: 2
                    
                    onClicked: {
                        searchField.text = ""
                        categoryCombo.currentIndex = 0
                        minPriceSpinBox.value = 0
                        maxPriceSpinBox.value = 10000
                        inStockCheckBox.checked = false
                        sortCombo.currentIndex = 0
                        
                        proxyModel.resetFilters()
                    }
                }
            }
        }
        
        // ============================================
        // PRODUCT LIST
        // 
        // KEY CONCEPT:
        // - ListView.model = PROXY model (not source!)
        // - Hiển thị filtered & sorted data
        // - Source model vẫn giữ nguyên data
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"
            border.color: "#ddd"
            border.width: 1
            radius: 8
            
            ListView {
                id: productListView
                anchors.fill: parent
                anchors.margins: 5
                spacing: 8
                clip: true
                
                // ★ KẾT NỐI VỚI PROXY MODEL
                model: proxyModel
                
                // ★ DELEGATE - Product card
                delegate: Rectangle {
                    width: productListView.width - 10
                    height: 110
                    color: "white"
                    border.width: 2
                    border.color: {
                        if (model.stock === 0) return "#f44336"  // Out of stock - Red
                        if (model.stock < 10) return "#ff9800"   // Low stock - Orange
                        return "#4caf50"  // In stock - Green
                    }
                    radius: 8
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 15
                        
                        // Product icon
                        Rectangle {
                            width: 80
                            height: 80
                            color: getCategoryColor(model.category)
                            radius: 40
                            
                            Label {
                                anchors.centerIn: parent
                                text: getCategoryIcon(model.category)
                                font.pixelSize: 36
                            }
                        }
                        
                        // Product info
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            
                            Label {
                                text: model.name
                                font.pixelSize: 18
                                font.bold: true
                            }
                            
                            Label {
                                text: qsTr("📁 ") + model.category
                                font.pixelSize: 13
                                color: "#666"
                            }
                            
                            Label {
                                text: model.description
                                font.pixelSize: 12
                                color: "#999"
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                        }
                        
                        // Price & Stock
                        ColumnLayout {
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            spacing: 8
                            
                            Label {
                                text: "$" + model.price.toFixed(2)
                                font.pixelSize: 22
                                font.bold: true
                                color: "#4caf50"
                                horizontalAlignment: Text.AlignRight
                            }
                            
                            Rectangle {
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 30
                                color: {
                                    if (model.stock === 0) return "#f44336"
                                    if (model.stock < 10) return "#ff9800"
                                    return "#4caf50"
                                }
                                radius: 15
                                
                                Label {
                                    anchors.centerIn: parent
                                    text: {
                                        if (model.stock === 0) return "OUT OF STOCK"
                                        if (model.stock < 10) return "LOW: " + model.stock
                                        return "STOCK: " + model.stock
                                    }
                                    color: "white"
                                    font.pixelSize: 11
                                    font.bold: true
                                }
                            }
                        }
                    }
                    
                    // Hover effect
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.NoButton
                        
                        onEntered: parent.color = "#f5f5f5"
                        onExited: parent.color = "white"
                    }
                }
                
                // Empty state
                Label {
                    anchors.centerIn: parent
                    visible: productListView.count === 0
                    text: qsTr("🔍 No products match your filters\nTry adjusting the criteria")
                    horizontalAlignment: Text.AlignHCenter
                    color: "#999"
                    font.pixelSize: 16
                }
            }
        }
        
        // ============================================
        // FILTER INFO BAR
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "#fff3e0"
            radius: 6
            visible: proxyModel.rowCount() < proxyModel.sourceRowCount
            
            Label {
                anchors.centerIn: parent
                text: qsTr("⚠️ Showing %1 of %2 products (filters active)")
                    .arg(proxyModel.rowCount())
                    .arg(proxyModel.sourceRowCount)
                font.pixelSize: 13
                color: "#e65100"
            }
        }
    }
    
    // ============================================
    // HELPER FUNCTIONS
    // ============================================
    function getCategoryIcon(category) {
        switch (category) {
            case "Electronics": return "💻"
            case "Clothing": return "👕"
            case "Food": return "🍔"
            case "Books": return "📚"
            default: return "📦"
        }
    }
    
    function getCategoryColor(category) {
        switch (category) {
            case "Electronics": return "#e3f2fd"
            case "Clothing": return "#fce4ec"
            case "Food": return "#fff3e0"
            case "Books": return "#e8f5e9"
            default: return "#f5f5f5"
        }
    }
}

/**
 * ============================================
 * 📚 SUMMARY - PRODUCT FILTER PROXY VIEW
 * ============================================
 * 
 * 1. PROXY MODEL CONCEPT:
 *    ✅ Wrapper around source model
 *    ✅ Filter & sort WITHOUT modifying source
 *    ✅ Multiple views of same data
 *    ✅ Real-time updates
 * 
 * 2. MODEL HIERARCHY:
 *    ProductModel (Source)
 *         ↓ (wraps)
 *    ProductFilterProxyModel (Proxy)
 *         ↓ (displays)
 *    ListView (QML View)
 * 
 * 3. C++ METHODS CALLED:
 *    FILTER METHODS:
 *    - setSearchText(text)
 *    - setFilterCategory(category)
 *    - setMinPrice(price)
 *    - setMaxPrice(price)
 *    - setShowInStockOnly(bool)
 *    - resetFilters()
 *    
 *    SORT METHODS:
 *    - sortByName(ascending)
 *    - sortByPrice(ascending)
 *    - sortByStock(ascending)
 * 
 * 4. C++ PROPERTIES:
 *    PROXY MODEL:
 *    - rowCount() - Filtered count
 *    - filteredTotalValue - Sum of filtered items
 *    
 *    SOURCE MODEL (via proxy):
 *    - sourceRowCount - Total items
 *    - sourceTotalValue - Sum of all items
 * 
 * 5. ROLES USED (from source model):
 *    - name (NameRole) → QString
 *    - category (CategoryRole) → QString
 *    - price (PriceRole) → double
 *    - stock (StockRole) → int
 *    - description (DescriptionRole) → QString
 * 
 * 6. FILTERING WORKFLOW:
 *    User changes filter (e.g., category)
 *         ↓
 *    proxyModel.setFilterCategory(category)
 *         ↓
 *    Proxy model re-evaluates filterAcceptsRow()
 *         ↓
 *    Model emits layoutChanged/dataChanged
 *         ↓
 *    QML ListView automatically updates
 *         ↓
 *    Only matching items displayed
 * 
 * 7. KEY ADVANTAGES:
 *    ✅ Source model UNCHANGED
 *       → Multiple filtered views possible
 *       → Original data preserved
 *    
 *    ✅ Real-time filtering
 *       → No manual refresh needed
 *       → Automatic UI updates
 *    
 *    ✅ Composable
 *       → Stack multiple proxy models
 *       → Combine different filters
 *    
 *    ✅ Efficient
 *       → No data copying
 *       → Filter logic in C++
 * 
 * 8. SORT vs FILTER:
 *    FILTER:
 *    - Hides/shows items
 *    - filterAcceptsRow() returns bool
 *    - Changes rowCount()
 *    
 *    SORT:
 *    - Reorders items
 *    - lessThan() compares items
 *    - rowCount() unchanged
 * 
 * 9. MULTIPLE CRITERIA:
 *    Proxy có thể combine nhiều filters:
 *    - Search text AND
 *    - Category AND
 *    - Price range AND
 *    - Stock status
 *    
 *    filterAcceptsRow() checks tất cả conditions
 * 
 * ============================================
 * 💡 LEARNING POINTS:
 * ============================================
 * 
 * ★ Proxy model setup (C++):
 *   QSortFilterProxyModel proxy;
 *   proxy.setSourceModel(sourceModel);
 *   proxy.setFilterRole(Qt::UserRole + 1);
 *   proxy.setFilterCaseSensitivity(Qt::CaseInsensitive);
 * 
 * ★ Custom filtering (C++):
 *   bool filterAcceptsRow(int row, const QModelIndex &parent) {
 *       // Check if row passes ALL filter criteria
 *       return matchesSearch && matchesCategory && matchesPrice;
 *   }
 * 
 * ★ Custom sorting (C++):
 *   bool lessThan(const QModelIndex &left, const QModelIndex &right) {
 *       // Compare items for sorting
 *       return leftValue < rightValue;
 *   }
 * 
 * ★ QML usage:
 *   ListView {
 *       model: proxyModel  // NOT sourceModel!
 *   }
 * 
 * ★ Update filters:
 *   proxyModel.setFilterRole(CustomRole);
 *   proxyModel.invalidate();  // Re-evaluate filters
 * 
 * ★ Access source model:
 *   int total = proxyModel.sourceModel()->rowCount();
 * 
 * ============================================
 * 🎯 USE CASES:
 * ============================================
 * 
 * ✅ Search/Filter UI
 * ✅ Multiple sorted views of same data
 * ✅ Filtering large datasets
 * ✅ Dashboard với different views
 * ✅ Report filtering
 * ✅ Product catalogs
 * ✅ Log viewers với filters
 * 
 * ============================================
 */
