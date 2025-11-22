// Ví dụ về QAbstractTableModel - Model dạng bảng
#include <QApplication>
#include <QTableView>
#include <QAbstractTableModel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialog>

// Struct đại diện cho một sản phẩm
struct Product {
    QString name;
    double price;
    int quantity;
    QString category;
    
    // Tính tổng giá trị
    double totalValue() const { return price * quantity; }
};

// Custom Table Model để quản lý danh sách sản phẩm
class ProductTableModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    // Enum cho các cột
    enum Column {
        NameColumn = 0,
        PriceColumn,
        QuantityColumn,
        CategoryColumn,
        TotalColumn,
        ColumnCount // Số lượng cột
    };
    
    explicit ProductTableModel(QObject *parent = nullptr)
        : QAbstractTableModel(parent) {
        // Khởi tạo dữ liệu mẫu
        m_products << Product{"Laptop Dell", 15000000, 5, "Điện tử"}
                   << Product{"iPhone 15", 25000000, 3, "Điện thoại"}
                   << Product{"Chuột Logitech", 500000, 20, "Phụ kiện"}
                   << Product{"Bàn phím cơ", 2000000, 10, "Phụ kiện"}
                   << Product{"Màn hình 27\"", 8000000, 7, "Điện tử"};
    }
    
    // 1. PHƯƠNG THỨC BẮT BUỘC: Số lượng hàng
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_products.size();
    }
    
    // 2. PHƯƠNG THỨC BẮT BUỘC: Số lượng cột
    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return ColumnCount;
    }
    
    // 3. PHƯƠNG THỨC BẮT BUỘC: Trả về dữ liệu
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_products.size())
            return QVariant();
            
        const Product &product = m_products.at(index.row());
        
        // Xử lý các roles khác nhau
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
            case NameColumn:
                return product.name;
            case PriceColumn:
                return QString::number(product.price, 'f', 0) + " VNĐ";
            case QuantityColumn:
                return product.quantity;
            case CategoryColumn:
                return product.category;
            case TotalColumn:
                return QString::number(product.totalValue(), 'f', 0) + " VNĐ";
            }
        }
        // Căn chỉnh văn bản
        else if (role == Qt::TextAlignmentRole) {
            switch (index.column()) {
            case PriceColumn:
            case QuantityColumn:
            case TotalColumn:
                return Qt::AlignRight + Qt::AlignVCenter;
            default:
                return Qt::AlignLeft + Qt::AlignVCenter;
            }
        }
        // Màu nền cho hàng chẵn/lẻ
        else if (role == Qt::BackgroundRole) {
            if (index.row() % 2 == 0)
                return QColor(245, 245, 245);
        }
        // Tooltip
        else if (role == Qt::ToolTipRole) {
            return QString("Sản phẩm: %1\nGiá: %2\nSố lượng: %3")
                .arg(product.name)
                .arg(product.price)
                .arg(product.quantity);
        }
        
        return QVariant();
    }
    
    // 4. PHƯƠNG THỨC TÙY CHỌN: Header của bảng
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case NameColumn:
                return "Tên sản phẩm";
            case PriceColumn:
                return "Giá";
            case QuantityColumn:
                return "Số lượng";
            case CategoryColumn:
                return "Danh mục";
            case TotalColumn:
                return "Tổng giá trị";
            }
        }
        return QVariant();
    }
    
    // 5. PHƯƠNG THỨC TÙY CHỌN: Cho phép edit
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {
        if (!index.isValid() || role != Qt::EditRole)
            return false;
            
        Product &product = m_products[index.row()];
        
        switch (index.column()) {
        case NameColumn:
            product.name = value.toString();
            break;
        case PriceColumn:
            product.price = value.toDouble();
            break;
        case QuantityColumn:
            product.quantity = value.toInt();
            break;
        case CategoryColumn:
            product.category = value.toString();
            break;
        default:
            return false;
        }
        
        // Emit signal để update cả hàng (vì total value có thể thay đổi)
        emit dataChanged(index.sibling(index.row(), 0), 
                        index.sibling(index.row(), ColumnCount - 1));
        return true;
    }
    
    // 6. PHƯƠNG THỨC TÙY CHỌN: Flags
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
            
        Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        
        // Không cho edit cột tổng giá trị
        if (index.column() != TotalColumn) {
            flags |= Qt::ItemIsEditable;
        }
        
        return flags;
    }
    
    // 7. CUSTOM METHODS: Thêm/xóa/cập nhật sản phẩm
    void addProduct(const Product &product) {
        int row = m_products.size();
        beginInsertRows(QModelIndex(), row, row);
        m_products.append(product);
        endInsertRows();
    }
    
    void removeProduct(int row) {
        if (row < 0 || row >= m_products.size())
            return;
            
        beginRemoveRows(QModelIndex(), row, row);
        m_products.removeAt(row);
        endRemoveRows();
    }
    
    // Lấy tổng giá trị kho
    double getTotalInventoryValue() const {
        double total = 0;
        for (const auto &product : m_products) {
            total += product.totalValue();
        }
        return total;
    }
    
    // Sort theo cột
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override {
        beginResetModel();
        
        std::sort(m_products.begin(), m_products.end(),
            [column, order](const Product &a, const Product &b) {
                bool less = false;
                switch (column) {
                case NameColumn:
                    less = a.name < b.name;
                    break;
                case PriceColumn:
                    less = a.price < b.price;
                    break;
                case QuantityColumn:
                    less = a.quantity < b.quantity;
                    break;
                case CategoryColumn:
                    less = a.category < b.category;
                    break;
                case TotalColumn:
                    less = a.totalValue() < b.totalValue();
                    break;
                }
                return order == Qt::AscendingOrder ? less : !less;
            });
            
        endResetModel();
    }
    
private:
    QList<Product> m_products;
};

// Dialog để thêm sản phẩm mới
class AddProductDialog : public QDialog {
    Q_OBJECT
    
public:
    AddProductDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Thêm sản phẩm mới");
        
        m_nameEdit = new QLineEdit;
        m_priceSpinBox = new QSpinBox;
        m_priceSpinBox->setMaximum(999999999);
        m_priceSpinBox->setSuffix(" VNĐ");
        m_quantitySpinBox = new QSpinBox;
        m_quantitySpinBox->setMaximum(9999);
        m_categoryEdit = new QLineEdit;
        
        auto *formLayout = new QFormLayout;
        formLayout->addRow("Tên sản phẩm:", m_nameEdit);
        formLayout->addRow("Giá:", m_priceSpinBox);
        formLayout->addRow("Số lượng:", m_quantitySpinBox);
        formLayout->addRow("Danh mục:", m_categoryEdit);
        
        auto *okButton = new QPushButton("OK");
        auto *cancelButton = new QPushButton("Hủy");
        
        auto *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(formLayout);
        mainLayout->addLayout(buttonLayout);
        
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    }
    
    Product getProduct() const {
        return Product{
            m_nameEdit->text(),
            static_cast<double>(m_priceSpinBox->value()),
            m_quantitySpinBox->value(),
            m_categoryEdit->text()
        };
    }
    
private:
    QLineEdit *m_nameEdit;
    QSpinBox *m_priceSpinBox;
    QSpinBox *m_quantitySpinBox;
    QLineEdit *m_categoryEdit;
};

// Widget demo chính
class TableModelDemo : public QWidget {
    Q_OBJECT
    
public:
    TableModelDemo() {
        // Tạo model
        m_model = new ProductTableModel(this);
        
        // Tạo view
        m_tableView = new QTableView;
        m_tableView->setModel(m_model);
        m_tableView->setSortingEnabled(true); // Cho phép sort bằng click header
        m_tableView->setAlternatingRowColors(true);
        m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_tableView->horizontalHeader()->setStretchLastSection(true);
        
        // Labels
        m_totalLabel = new QLabel;
        updateTotalLabel();
        
        // Buttons
        auto *addBtn = new QPushButton("Thêm sản phẩm");
        auto *removeBtn = new QPushButton("Xóa sản phẩm");
        auto *refreshBtn = new QPushButton("Cập nhật tổng");
        
        // Layout
        auto *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(addBtn);
        buttonLayout->addWidget(removeBtn);
        buttonLayout->addWidget(refreshBtn);
        
        auto *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("📊 Demo QAbstractTableModel"));
        layout->addWidget(m_tableView);
        layout->addWidget(m_totalLabel);
        layout->addLayout(buttonLayout);
        
        // Connections
        connect(addBtn, &QPushButton::clicked, this, &TableModelDemo::addProduct);
        connect(removeBtn, &QPushButton::clicked, this, &TableModelDemo::removeProduct);
        connect(refreshBtn, &QPushButton::clicked, this, &TableModelDemo::updateTotalLabel);
        
        // Update total khi dữ liệu thay đổi
        connect(m_model, &QAbstractTableModel::dataChanged, 
                this, &TableModelDemo::updateTotalLabel);
        connect(m_model, &QAbstractTableModel::rowsInserted, 
                this, &TableModelDemo::updateTotalLabel);
        connect(m_model, &QAbstractTableModel::rowsRemoved, 
                this, &TableModelDemo::updateTotalLabel);
    }
    
private slots:
    void addProduct() {
        AddProductDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            m_model->addProduct(dialog.getProduct());
        }
    }
    
    void removeProduct() {
        auto indexes = m_tableView->selectionModel()->selectedRows();
        if (!indexes.isEmpty()) {
            m_model->removeProduct(indexes.first().row());
        }
    }
    
    void updateTotalLabel() {
        double total = m_model->getTotalInventoryValue();
        m_totalLabel->setText(QString("💰 Tổng giá trị kho: %1 VNĐ")
            .arg(QString::number(total, 'f', 0)));
    }
    
private:
    ProductTableModel *m_model;
    QTableView *m_tableView;
    QLabel *m_totalLabel;
};

#include "table_model_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TableModelDemo demo;
    demo.setWindowTitle("Table Model Example");
    demo.resize(800, 600);
    demo.show();
    
    return app.exec();
}

/*
 * TỔNG KẾT VỀ QAbstractTableModel:
 * 
 * 1. So với List Model, cần thêm:
 *    - columnCount(): Số lượng cột
 *    - headerData(): Tiêu đề cột/hàng
 * 
 * 2. Index có cả row và column:
 *    - index.row(): Hàng
 *    - index.column(): Cột
 *    - index.sibling(): Lấy ô khác cùng hàng/cột
 * 
 * 3. Tips cho Table Model:
 *    - Dùng enum cho columns để dễ maintain
 *    - Có thể custom hiển thị theo role (alignment, color...)
 *    - setSortingEnabled(true) cho phép sort tự động
 * 
 * 4. Khi update dữ liệu:
 *    - Nếu 1 ô thay đổi ảnh hưởng ô khác, emit dataChanged cho cả range
 *    - beginResetModel/endResetModel khi thay đổi lớn
 */