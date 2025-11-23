// delegate_pointer_example.cpp - Ví dụ về con trỏ với Qt Delegate

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>
#include <QPointer>
#include <memory>

// Custom Delegate để demo lifecycle và pointer management
class ColorDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
private:
    // Cache các editor được tạo
    mutable QPointer<QWidget> m_lastEditor;
    
public:
    ColorDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {
        qDebug() << "ColorDelegate created at" << this << "with parent" << parent;
    }
    
    ~ColorDelegate() {
        qDebug() << "ColorDelegate destroyed at" << this;
    }
    
    // Paint method - sử dụng painter pointer
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        if (!painter) return; // Always check pointer
        
        // Lưu state của painter
        painter->save();
        
        // Custom painting
        if (index.column() == 0) {
            painter->fillRect(option.rect, Qt::lightGray);
        }
        
        // Gọi base implementation
        QStyledItemDelegate::paint(painter, option, index);
        
        // Restore painter state
        painter->restore();
        
        qDebug() << "Painted index" << index << "with painter" << painter;
    }
    
    // Create editor - return widget pointer
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override {
        qDebug() << "Creating editor for" << index << "parent:" << parent;
        
        QWidget* editor = nullptr;
        
        if (index.column() == 0) {
            // ComboBox cho cột đầu
            QComboBox* combo = new QComboBox(parent);
            combo->addItems({"Red", "Green", "Blue", "Yellow"});
            editor = combo;
        } else if (index.column() == 1) {
            // SpinBox cho cột 2
            QSpinBox* spin = new QSpinBox(parent);
            spin->setRange(0, 100);
            editor = spin;
        } else {
            // Default editor
            editor = QStyledItemDelegate::createEditor(parent, option, index);
        }
        
        // Lưu reference đến editor mới nhất
        m_lastEditor = editor;
        
        qDebug() << "Created editor:" << editor << "type:" << editor->metaObject()->className();
        return editor;
    }
    
    // Set data từ model vào editor
    void setEditorData(QWidget* editor, const QModelIndex& index) const override {
        if (!editor) return;
        
        qDebug() << "Setting editor data for" << index << "editor:" << editor;
        
        // Cast an toàn với qobject_cast
        if (QComboBox* combo = qobject_cast<QComboBox*>(editor)) {
            combo->setCurrentText(index.data().toString());
        } else if (QSpinBox* spin = qobject_cast<QSpinBox*>(editor)) {
            spin->setValue(index.data().toInt());
        } else {
            QStyledItemDelegate::setEditorData(editor, index);
        }
    }
    
    // Set data từ editor vào model
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                     const QModelIndex& index) const override {
        if (!editor || !model) return;
        
        qDebug() << "Setting model data from editor" << editor << "to index" << index;
        
        if (QComboBox* combo = qobject_cast<QComboBox*>(editor)) {
            model->setData(index, combo->currentText());
        } else if (QSpinBox* spin = qobject_cast<QSpinBox*>(editor)) {
            model->setData(index, spin->value());
        } else {
            QStyledItemDelegate::setModelData(editor, model, index);
        }
    }
    
    // Kiểm tra editor còn valid không
    bool isEditorValid() const {
        return m_lastEditor && m_lastEditor->isVisible();
    }
};

// Complex Delegate với multiple editors
class MultiEditorDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
private:
    // Map để track editors theo index
    mutable QHash<QPersistentModelIndex, QPointer<QWidget>> m_editors;
    
public:
    MultiEditorDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                         const QModelIndex& index) const override {
        QWidget* editor = nullptr;
        
        // Tạo editor dựa trên data type
        QVariant data = index.data();
        
        switch (data.type()) {
            case QVariant::Int:
                editor = new QSpinBox(parent);
                break;
                
            case QVariant::Double:
                editor = new QDoubleSpinBox(parent);
                break;
                
            case QVariant::Date:
                editor = new QDateEdit(parent);
                break;
                
            case QVariant::Bool:
                editor = new QCheckBox(parent);
                break;
                
            default:
                editor = new QLineEdit(parent);
                break;
        }
        
        // Track editor
        m_editors[QPersistentModelIndex(index)] = editor;
        
        // Connect để cleanup khi editor bị destroy
        connect(editor, &QObject::destroyed, [this, index]() {
            qDebug() << "Editor destroyed for index" << index;
            m_editors.remove(QPersistentModelIndex(index));
        });
        
        return editor;
    }
    
    void clearEditors() {
        qDebug() << "Clearing" << m_editors.size() << "tracked editors";
        m_editors.clear();
    }
    
    int activeEditorCount() const {
        int count = 0;
        for (auto it = m_editors.begin(); it != m_editors.end(); ++it) {
            if (it.value()) count++;
        }
        return count;
    }
};

// Demo window cho Delegate patterns
class DelegatePointerDemo : public QMainWindow {
    Q_OBJECT
    
private:
    // Model
    QStandardItemModel* m_model;
    
    // View
    QTableView* m_tableView;
    
    // Delegates - different ownership patterns
    ColorDelegate* m_ownedDelegate;          // Owned by this
    std::unique_ptr<MultiEditorDelegate> m_smartDelegate;  // Smart pointer
    QPointer<QStyledItemDelegate> m_safeDelegate;         // Safe pointer
    
public:
    DelegatePointerDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupModel();
        setupUI();
        demonstrateDelegatePatterns();
    }
    
    void setupModel() {
        m_model = new QStandardItemModel(10, 4, this);
        m_model->setHorizontalHeaderLabels({"Color", "Value", "Text", "Date"});
        
        // Populate với different data types
        for (int row = 0; row < 10; ++row) {
            m_model->setItem(row, 0, new QStandardItem("Red"));
            m_model->setItem(row, 1, new QStandardItem(QString::number(row * 10)));
            m_model->setItem(row, 2, new QStandardItem(QString("Text %1").arg(row)));
            m_model->setItem(row, 3, new QStandardItem(QDate::currentDate().toString()));
        }
    }
    
    void setupUI() {
        QWidget* central = new QWidget(this);
        setCentralWidget(central);
        
        QVBoxLayout* layout = new QVBoxLayout(central);
        
        // Table view
        m_tableView = new QTableView(central);
        m_tableView->setModel(m_model);
        layout->addWidget(m_tableView);
        
        // Control buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        
        auto addButton = [&](const QString& text, auto slot) {
            QPushButton* btn = new QPushButton(text);
            connect(btn, &QPushButton::clicked, this, slot);
            buttonLayout->addWidget(btn);
            return btn;
        };
        
        addButton("Set Color Delegate", &DelegatePointerDemo::setColorDelegate);
        addButton("Set Multi Delegate", &DelegatePointerDemo::setMultiDelegate);
        addButton("Remove Delegate", &DelegatePointerDemo::removeDelegate);
        addButton("Test Delegate Lifecycle", &DelegatePointerDemo::testDelegateLifecycle);
        addButton("Show Delegate Info", &DelegatePointerDemo::showDelegateInfo);
        
        layout->addLayout(buttonLayout);
    }
    
    void demonstrateDelegatePatterns() {
        qDebug() << "\n=== Delegate Pointer Patterns ===";
        
        // Pattern 1: Delegate với parent
        m_ownedDelegate = new ColorDelegate(this);
        
        // Pattern 2: Smart pointer delegate
        m_smartDelegate = std::make_unique<MultiEditorDelegate>();
        
        // Set default delegate
        setColorDelegate();
    }
    
private slots:
    void setColorDelegate() {
        qDebug() << "\n=== Setting Color Delegate ===";
        
        // Lấy delegate cũ trước khi set mới
        QAbstractItemDelegate* oldDelegate = m_tableView->itemDelegate();
        qDebug() << "Old delegate:" << oldDelegate;
        
        // Set delegate mới
        m_tableView->setItemDelegate(m_ownedDelegate);
        qDebug() << "New delegate set:" << m_ownedDelegate;
        
        // Delegate cũ KHÔNG được view delete tự động!
        // Phải tự quản lý lifecycle
    }
    
    void setMultiDelegate() {
        qDebug() << "\n=== Setting Multi Editor Delegate ===";
        
        m_tableView->setItemDelegate(m_smartDelegate.get());
        qDebug() << "Smart pointer delegate set:" << m_smartDelegate.get();
    }
    
    void removeDelegate() {
        qDebug() << "\n=== Removing Custom Delegate ===";
        
        // Set về default delegate
        m_tableView->setItemDelegate(nullptr);
        
        // Hoặc tạo default delegate mới
        QStyledItemDelegate* defaultDelegate = new QStyledItemDelegate(m_tableView);
        m_tableView->setItemDelegate(defaultDelegate);
        
        qDebug() << "Reverted to default delegate";
    }
    
    void testDelegateLifecycle() {
        qDebug() << "\n=== Testing Delegate Lifecycle ===";
        
        // Test 1: Temporary delegate
        {
            QStyledItemDelegate tempDelegate;
            m_tableView->setItemDelegateForColumn(0, &tempDelegate);
            qDebug() << "Set temporary delegate for column 0";
        } // tempDelegate destroyed here!
        // DANGEROUS - view still has pointer!
        
        // Test 2: Safe approach
        QStyledItemDelegate* columnDelegate = new QStyledItemDelegate(m_tableView);
        m_tableView->setItemDelegateForColumn(1, columnDelegate);
        qDebug() << "Set owned delegate for column 1";
        
        // Test 3: Row delegate
        QStyledItemDelegate* rowDelegate = new QStyledItemDelegate(this);
        m_tableView->setItemDelegateForRow(0, rowDelegate);
        qDebug() << "Set delegate for row 0";
    }
    
    void showDelegateInfo() {
        qDebug() << "\n=== Current Delegate Information ===";
        
        // Main delegate
        QAbstractItemDelegate* mainDelegate = m_tableView->itemDelegate();
        qDebug() << "Main delegate:" << mainDelegate;
        if (mainDelegate) {
            qDebug() << "  Type:" << mainDelegate->metaObject()->className();
            qDebug() << "  Parent:" << mainDelegate->parent();
        }
        
        // Column delegates
        for (int col = 0; col < m_model->columnCount(); ++col) {
            QAbstractItemDelegate* colDelegate = m_tableView->itemDelegateForColumn(col);
            if (colDelegate && colDelegate != mainDelegate) {
                qDebug() << "Column" << col << "delegate:" << colDelegate;
            }
        }
        
        // Row delegates  
        for (int row = 0; row < std::min(5, m_model->rowCount()); ++row) {
            QAbstractItemDelegate* rowDelegate = m_tableView->itemDelegateForRow(row);
            if (rowDelegate && rowDelegate != mainDelegate) {
                qDebug() << "Row" << row << "delegate:" << rowDelegate;
            }
        }
        
        // Check smart delegate
        if (m_smartDelegate) {
            qDebug() << "\nMulti-editor delegate stats:";
            qDebug() << "  Active editors:" << m_smartDelegate->activeEditorCount();
        }
    }
};

// Best practices và common patterns
void demonstrateDelegateBestPractices() {
    qDebug() << "\n=== Delegate Best Practices ===";
    
    // Best Practice 1: Delegate ownership
    {
        qDebug() << "\n1. Proper Delegate Ownership:";
        QTableView view;
        
        // GOOD: Delegate with same parent as view
        QStyledItemDelegate* delegate = new QStyledItemDelegate(&view);
        view.setItemDelegate(delegate);
        // Both destroyed together
    }
    
    // Best Practice 2: Editor management
    {
        qDebug() << "\n2. Editor Widget Management:";
        // Editors are created with view's viewport as parent
        // View automatically manages editor lifecycle
        // Don't store permanent pointers to editors
    }
    
    // Best Practice 3: Type-safe casting
    {
        qDebug() << "\n3. Safe Widget Casting:";
        // Always use qobject_cast for safety
        QWidget* editor = new QSpinBox();
        
        if (QSpinBox* spin = qobject_cast<QSpinBox*>(editor)) {
            // Safe to use spin
            spin->setValue(42);
        }
        
        delete editor;
    }
    
    // Anti-pattern: Stack allocated delegate
    {
        qDebug() << "\n[ANTI-PATTERN] Stack Allocated Delegate:";
        QTableView view;
        
        {
            QStyledItemDelegate stackDelegate;
            view.setItemDelegate(&stackDelegate);
        } // stackDelegate destroyed!
        // view has dangling pointer!
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Show best practices
    demonstrateDelegateBestPractices();
    
    // Create main demo
    DelegatePointerDemo demo;
    demo.setWindowTitle("Qt Delegate Pointer Demo");
    demo.resize(800, 600);
    demo.show();
    
    return app.exec();
}

#include "delegate_pointer_example.moc"