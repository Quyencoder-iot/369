/**
 * Qt Models - Practical Code Examples
 * Các ví dụ code thực tế để học Qt Model/View
 */

#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QSqlTableModel>
#include <QListView>
#include <QTableView>
#include <QTreeView>

// ============================================================================
// EXAMPLE 1: Simple List Model (QAbstractListModel)
// ============================================================================

class SimpleListModel : public QAbstractListModel {
    Q_OBJECT
    QStringList m_items;
    
public:
    SimpleListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
    
    // ---- REQUIRED METHODS ----
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_items.count();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_items.size())
            return QVariant();
        
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_items.at(index.row());
        
        return QVariant();
    }
    
    // ---- EDITABLE SUPPORT ----
    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (!index.isValid() || role != Qt::EditRole)
            return false;
        
        m_items[index.row()] = value.toString();
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        return true;
    }
    
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
    }
    
    // ---- INSERT/REMOVE SUPPORT ----
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
        beginInsertRows(parent, row, row + count - 1);
        for (int i = 0; i < count; ++i)
            m_items.insert(row, QString("New Item"));
        endInsertRows();
        return true;
    }
    
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override {
        if (row < 0 || row + count > m_items.size())
            return false;
        
        beginRemoveRows(parent, row, row + count - 1);
        for (int i = 0; i < count; ++i)
            m_items.removeAt(row);
        endRemoveRows();
        return true;
    }
    
    // ---- CUSTOM API ----
    void addItem(const QString &item) {
        insertRow(m_items.size());
        setData(index(m_items.size() - 1), item, Qt::EditRole);
    }
};

// Usage:
void example1() {
    SimpleListModel *model = new SimpleListModel;
    model->addItem("Item 1");
    model->addItem("Item 2");
    model->addItem("Item 3");
    
    QListView *view = new QListView;
    view->setModel(model);
    view->show();
}


// ============================================================================
// EXAMPLE 2: Table Model with Custom Data (QAbstractTableModel)
// ============================================================================

struct Person {
    QString name;
    int age;
    QString email;
};

class PersonTableModel : public QAbstractTableModel {
    Q_OBJECT
    QVector<Person> m_people;
    
public:
    PersonTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}
    
    // ---- REQUIRED METHODS ----
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_people.size();
    }
    
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 3; // Name, Age, Email
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_people.size())
            return QVariant();
        
        const Person &person = m_people.at(index.row());
        
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
                case 0: return person.name;
                case 1: return person.age;
                case 2: return person.email;
            }
        }
        
        // Color coding for age
        if (role == Qt::BackgroundRole && index.column() == 1) {
            if (person.age < 18)
                return QColor(Qt::lightGray);
            else if (person.age >= 65)
                return QColor(Qt::yellow);
        }
        
        return QVariant();
    }
    
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
            return QVariant();
        
        switch (section) {
            case 0: return "Name";
            case 1: return "Age";
            case 2: return "Email";
        }
        return QVariant();
    }
    
    // ---- EDITABLE SUPPORT ----
    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (!index.isValid() || role != Qt::EditRole)
            return false;
        
        Person &person = m_people[index.row()];
        
        switch (index.column()) {
            case 0:
                person.name = value.toString();
                break;
            case 1:
                person.age = value.toInt();
                break;
            case 2:
                person.email = value.toString();
                break;
            default:
                return false;
        }
        
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole, Qt::BackgroundRole});
        return true;
    }
    
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
    
    // ---- CUSTOM API ----
    void addPerson(const Person &person) {
        beginInsertRows(QModelIndex(), m_people.size(), m_people.size());
        m_people.append(person);
        endInsertRows();
    }
    
    void removePerson(int row) {
        if (row < 0 || row >= m_people.size())
            return;
        
        beginRemoveRows(QModelIndex(), row, row);
        m_people.remove(row);
        endRemoveRows();
    }
};

// Usage:
void example2() {
    PersonTableModel *model = new PersonTableModel;
    model->addPerson({"Alice", 25, "alice@example.com"});
    model->addPerson({"Bob", 70, "bob@example.com"});
    model->addPerson({"Charlie", 17, "charlie@example.com"});
    
    QTableView *view = new QTableView;
    view->setModel(model);
    view->show();
}


// ============================================================================
// EXAMPLE 3: Tree Model (QAbstractItemModel)
// ============================================================================

class TreeItem {
public:
    TreeItem(const QString &data, TreeItem *parent = nullptr)
        : m_data(data), m_parent(parent) {}
    
    ~TreeItem() { qDeleteAll(m_children); }
    
    void appendChild(TreeItem *child) {
        m_children.append(child);
    }
    
    TreeItem *child(int row) {
        return m_children.value(row);
    }
    
    int childCount() const {
        return m_children.count();
    }
    
    int row() const {
        if (m_parent)
            return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));
        return 0;
    }
    
    TreeItem *parent() {
        return m_parent;
    }
    
    QString data() const {
        return m_data;
    }
    
private:
    QString m_data;
    TreeItem *m_parent;
    QList<TreeItem*> m_children;
};

class TreeModel : public QAbstractItemModel {
    Q_OBJECT
    TreeItem *m_rootItem;
    
public:
    TreeModel(QObject *parent = nullptr) : QAbstractItemModel(parent) {
        m_rootItem = new TreeItem("Root");
    }
    
    ~TreeModel() {
        delete m_rootItem;
    }
    
    // ---- REQUIRED METHODS ----
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        TreeItem *parentItem;
        if (!parent.isValid())
            parentItem = m_rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());
        
        return parentItem->childCount();
    }
    
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1; // Single column tree
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || role != Qt::DisplayRole)
            return QVariant();
        
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data();
    }
    
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (!hasIndex(row, column, parent))
            return QModelIndex();
        
        TreeItem *parentItem;
        if (!parent.isValid())
            parentItem = m_rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());
        
        TreeItem *childItem = parentItem->child(row);
        if (childItem)
            return createIndex(row, column, childItem);
        
        return QModelIndex();
    }
    
    QModelIndex parent(const QModelIndex &child) const override {
        if (!child.isValid())
            return QModelIndex();
        
        TreeItem *childItem = static_cast<TreeItem*>(child.internalPointer());
        TreeItem *parentItem = childItem->parent();
        
        if (parentItem == m_rootItem)
            return QModelIndex();
        
        return createIndex(parentItem->row(), 0, parentItem);
    }
    
    // ---- CUSTOM API ----
    TreeItem* rootItem() { return m_rootItem; }
};

// Usage:
void example3() {
    TreeModel *model = new TreeModel;
    
    // Build tree structure
    TreeItem *root = model->rootItem();
    TreeItem *folder1 = new TreeItem("Documents", root);
    root->appendChild(folder1);
    
    TreeItem *file1 = new TreeItem("file1.txt", folder1);
    TreeItem *file2 = new TreeItem("file2.txt", folder1);
    folder1->appendChild(file1);
    folder1->appendChild(file2);
    
    TreeItem *folder2 = new TreeItem("Pictures", root);
    root->appendChild(folder2);
    
    QTreeView *view = new QTreeView;
    view->setModel(model);
    view->expandAll();
    view->show();
}


// ============================================================================
// EXAMPLE 4: Sort/Filter Proxy Model
// ============================================================================

class CustomFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
    int m_minAge = 0;
    int m_maxAge = 100;
    
public:
    CustomFilterProxy(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
    
    void setAgeRange(int minAge, int maxAge) {
        m_minAge = minAge;
        m_maxAge = maxAge;
        invalidateFilter(); // Trigger re-filtering
    }
    
protected:
    // Custom filter logic
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        // Get age from column 1 (assuming PersonTableModel)
        QModelIndex ageIndex = sourceModel()->index(source_row, 1, source_parent);
        int age = sourceModel()->data(ageIndex).toInt();
        
        if (age < m_minAge || age > m_maxAge)
            return false;
        
        // Also apply text filter
        QModelIndex nameIndex = sourceModel()->index(source_row, 0, source_parent);
        QString name = sourceModel()->data(nameIndex).toString();
        
        return name.contains(filterRegExp());
    }
    
    // Custom sort logic
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        // Custom sorting for age column
        if (left.column() == 1) {
            int leftAge = sourceModel()->data(left).toInt();
            int rightAge = sourceModel()->data(right).toInt();
            return leftAge < rightAge;
        }
        
        // Default string comparison for other columns
        return QSortFilterProxyModel::lessThan(left, right);
    }
};

// Usage:
void example4() {
    PersonTableModel *sourceModel = new PersonTableModel;
    sourceModel->addPerson({"Alice", 25, "alice@example.com"});
    sourceModel->addPerson({"Bob", 70, "bob@example.com"});
    sourceModel->addPerson({"Charlie", 17, "charlie@example.com"});
    sourceModel->addPerson({"David", 35, "david@example.com"});
    
    CustomFilterProxy *proxyModel = new CustomFilterProxy;
    proxyModel->setSourceModel(sourceModel);
    proxyModel->setFilterKeyColumn(0); // Filter on name column
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    
    // Filter: Show only people aged 18-65
    proxyModel->setAgeRange(18, 65);
    
    QTableView *view = new QTableView;
    view->setModel(proxyModel);
    view->setSortingEnabled(true); // Enable column header sorting
    view->show();
    
    // Connect to search box
    QLineEdit *searchBox = new QLineEdit;
    QObject::connect(searchBox, &QLineEdit::textChanged, [proxyModel](const QString &text) {
        proxyModel->setFilterFixedString(text);
    });
}


// ============================================================================
// EXAMPLE 5: SQL Model
// ============================================================================

void example5_sql() {
    // Setup database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("employees.db");
    
    if (!db.open()) {
        qWarning() << "Failed to open database";
        return;
    }
    
    // Create table
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS employees ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "department TEXT, "
               "salary INTEGER)");
    
    // Insert sample data
    query.exec("INSERT INTO employees (name, department, salary) VALUES ('Alice', 'Engineering', 75000)");
    query.exec("INSERT INTO employees (name, department, salary) VALUES ('Bob', 'Marketing', 60000)");
    query.exec("INSERT INTO employees (name, department, salary) VALUES ('Charlie', 'Engineering', 80000)");
    
    // Create SQL table model
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("employees");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select(); // Load data from database
    
    // Set headers
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Department");
    model->setHeaderData(3, Qt::Horizontal, "Salary");
    
    // Add sorting/filtering with proxy
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(2); // Filter on department
    
    QTableView *view = new QTableView;
    view->setModel(proxy);
    view->setSortingEnabled(true);
    view->hideColumn(0); // Hide ID column
    view->show();
    
    // Edit data
    QModelIndex index = model->index(0, 3); // First row, salary column
    model->setData(index, 78000);
    
    // Save changes to database
    if (!model->submitAll()) {
        qWarning() << "Failed to submit changes:" << model->lastError().text();
        model->revertAll();
    }
}


// ============================================================================
// EXAMPLE 6: Custom Roles for Advanced Data
// ============================================================================

class AdvancedListModel : public QAbstractListModel {
    Q_OBJECT
    
    struct Item {
        QString title;
        QString description;
        QColor color;
        QIcon icon;
        int priority;
    };
    
    QList<Item> m_items;
    
public:
    enum CustomRoles {
        TitleRole = Qt::UserRole + 1,
        DescriptionRole,
        ColorRole,
        IconRole,
        PriorityRole
    };
    
    AdvancedListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_items.count();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_items.size())
            return QVariant();
        
        const Item &item = m_items.at(index.row());
        
        switch (role) {
            case Qt::DisplayRole:
            case TitleRole:
                return item.title;
            
            case DescriptionRole:
                return item.description;
            
            case Qt::DecorationRole:
            case IconRole:
                return item.icon;
            
            case Qt::BackgroundRole:
            case ColorRole:
                return item.color;
            
            case PriorityRole:
                return item.priority;
            
            case Qt::ToolTipRole:
                return QString("%1\n%2\nPriority: %3")
                    .arg(item.title)
                    .arg(item.description)
                    .arg(item.priority);
        }
        
        return QVariant();
    }
    
    // Expose custom roles to QML
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "title";
        roles[DescriptionRole] = "description";
        roles[ColorRole] = "color";
        roles[IconRole] = "icon";
        roles[PriorityRole] = "priority";
        return roles;
    }
    
    void addItem(const Item &item) {
        beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
        m_items.append(item);
        endInsertRows();
    }
};

// Usage:
void example6() {
    AdvancedListModel *model = new AdvancedListModel;
    
    model->addItem({
        "High Priority Task",
        "This needs to be done ASAP",
        QColor(Qt::red),
        QIcon(":/icons/urgent.png"),
        10
    });
    
    model->addItem({
        "Normal Task",
        "Regular work item",
        QColor(Qt::white),
        QIcon(":/icons/normal.png"),
        5
    });
    
    QListView *view = new QListView;
    view->setModel(model);
    view->show();
}


// ============================================================================
// EXAMPLE 7: Model Index Mapping (Proxy to Source)
// ============================================================================

void example7_index_mapping() {
    // Create source model
    PersonTableModel *sourceModel = new PersonTableModel;
    sourceModel->addPerson({"Alice", 25, "alice@example.com"});
    sourceModel->addPerson({"Bob", 70, "bob@example.com"});
    sourceModel->addPerson({"Charlie", 17, "charlie@example.com"});
    
    // Create proxy
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
    proxy->setSourceModel(sourceModel);
    proxy->sort(0); // Sort by name
    
    // Get index from proxy
    QModelIndex proxyIndex = proxy->index(0, 0);
    QString name = proxy->data(proxyIndex).toString();
    qDebug() << "First person in sorted view:" << name; // "Alice"
    
    // Map proxy index to source index
    QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
    qDebug() << "Source row:" << sourceIndex.row(); // Original position
    
    // Map source index to proxy index
    QModelIndex sourceIndex2 = sourceModel->index(2, 0); // Charlie
    QModelIndex proxyIndex2 = proxy->mapFromSource(sourceIndex2);
    qDebug() << "Proxy row:" << proxyIndex2.row(); // Position after sorting
    
    // IMPORTANT: Always use proxy indices when working with proxy models!
    // View selection indices are PROXY indices, not source indices
}


// ============================================================================
// DEBUGGING TIPS
// ============================================================================

void debugging_tips() {
    // 1. Override data() and add logging
    /*
    QVariant data(const QModelIndex &index, int role) const override {
        qDebug() << "data() called:"
                 << "row =" << index.row()
                 << "col =" << index.column()
                 << "role =" << role;
        // ... implementation
    }
    */
    
    // 2. Test model independently
    /*
    MyModel model;
    qDebug() << "Row count:" << model.rowCount();
    qDebug() << "Column count:" << model.columnCount();
    
    for (int i = 0; i < model.rowCount(); ++i) {
        QModelIndex index = model.index(i, 0);
        qDebug() << "Row" << i << ":" << model.data(index).toString();
    }
    */
    
    // 3. Use QAbstractItemModelTester (Qt 5.11+)
    /*
    #include <QAbstractItemModelTester>
    
    MyModel *model = new MyModel;
    QAbstractItemModelTester *tester = new QAbstractItemModelTester(
        model,
        QAbstractItemModelTester::FailureReportingMode::Fatal
    );
    // Tester will automatically check model consistency
    */
    
    // 4. Check signal emissions
    /*
    QObject::connect(model, &QAbstractItemModel::dataChanged,
        [](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
            qDebug() << "dataChanged emitted:"
                     << "rows:" << topLeft.row() << "-" << bottomRight.row()
                     << "roles:" << roles;
        });
    */
}


// ============================================================================
// PERFORMANCE TIPS
// ============================================================================

void performance_tips() {
    // 1. Batch updates with layoutAboutToBeChanged/layoutChanged
    /*
    emit layoutAboutToBeChanged();
    // ... make many changes to internal data structure
    emit layoutChanged();
    */
    
    // 2. Use beginInsertRows/endInsertRows instead of layoutChanged
    /*
    // GOOD:
    beginInsertRows(QModelIndex(), position, position + count - 1);
    // insert data
    endInsertRows();
    
    // BAD (slow):
    // insert data
    emit layoutChanged();
    */
    
    // 3. Implement canFetchMore/fetchMore for lazy loading
    /*
    bool canFetchMore(const QModelIndex &parent) const override {
        return m_items.size() < m_totalItemsInDatabase;
    }
    
    void fetchMore(const QModelIndex &parent) override {
        int remainder = m_totalItemsInDatabase - m_items.size();
        int itemsToFetch = qMin(100, remainder);
        
        beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + itemsToFetch - 1);
        // Load next batch from database
        endInsertRows();
    }
    */
    
    // 4. Cache expensive computations
    /*
    mutable QHash<QPersistentModelIndex, QVariant> m_cache;
    
    QVariant data(const QModelIndex &index, int role) const override {
        if (role == ExpensiveRole) {
            QPersistentModelIndex persistentIndex(index);
            if (!m_cache.contains(persistentIndex)) {
                m_cache[persistentIndex] = computeExpensiveValue(index);
            }
            return m_cache[persistentIndex];
        }
        // ...
    }
    */
}

// ============================================================================
// MAIN (for testing)
// ============================================================================

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Uncomment to test each example:
    // example1();
    // example2();
    // example3();
    // example4();
    // example5_sql();
    // example6();
    // example7_index_mapping();
    
    return app.exec();
}

#include "Qt_Models_Examples.moc"
