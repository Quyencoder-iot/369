// Ví dụ về QSortFilterProxyModel - Lọc và sắp xếp dữ liệu
#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QHeaderView>
#include <QRandomGenerator>

// Custom Proxy Model với filtering logic phức tạp hơn
class EmployeeProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
    
public:
    explicit EmployeeProxyModel(QObject *parent = nullptr) 
        : QSortFilterProxyModel(parent), m_minSalary(0), m_maxSalary(999999999) {
        // Cho phép filter theo tất cả columns
        setFilterKeyColumn(-1);
    }
    
    // Setter cho salary range filter
    void setMinimumSalary(int salary) {
        m_minSalary = salary;
        invalidateFilter(); // Refresh filter
    }
    
    void setMaximumSalary(int salary) {
        m_maxSalary = salary;
        invalidateFilter();
    }
    
    // Setter cho department filter
    void setDepartmentFilter(const QString &dept) {
        m_departmentFilter = dept;
        invalidateFilter();
    }
    
protected:
    // Override filterAcceptsRow để custom filter logic
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        // Lấy index của các columns
        QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
        QModelIndex deptIndex = sourceModel()->index(sourceRow, 1, sourceParent);
        QModelIndex salaryIndex = sourceModel()->index(sourceRow, 2, sourceParent);
        
        // Filter theo text search (name hoặc department)
        QString name = sourceModel()->data(nameIndex).toString();
        QString dept = sourceModel()->data(deptIndex).toString();
        
        bool matchesText = filterRegularExpression().match(name).hasMatch() ||
                          filterRegularExpression().match(dept).hasMatch();
        
        // Filter theo department cụ thể
        bool matchesDept = m_departmentFilter.isEmpty() || 
                          dept == m_departmentFilter;
        
        // Filter theo salary range
        int salary = sourceModel()->data(salaryIndex).toInt();
        bool matchesSalary = salary >= m_minSalary && salary <= m_maxSalary;
        
        // Tất cả conditions phải true
        return matchesText && matchesDept && matchesSalary;
    }
    
    // Override lessThan để custom sort logic
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override {
        // Lấy data từ cả 2 sides
        QVariant leftData = sourceModel()->data(source_left);
        QVariant rightData = sourceModel()->data(source_right);
        
        // Special handling cho salary column (column 2)
        if (source_left.column() == 2) {
            return leftData.toInt() < rightData.toInt();
        }
        
        // Default string comparison cho các columns khác
        return QString::localeAwareCompare(leftData.toString(), 
                                          rightData.toString()) < 0;
    }
    
private:
    int m_minSalary;
    int m_maxSalary;
    QString m_departmentFilter;
};

// Widget demo chính
class ProxyModelDemo : public QWidget {
    Q_OBJECT
    
public:
    ProxyModelDemo() {
        setupModel();
        setupProxyModel();
        setupUI();
        connectSignals();
    }
    
private:
    void setupModel() {
        // Tạo source model với data nhân viên
        m_sourceModel = new QStandardItemModel(0, 4, this);
        m_sourceModel->setHorizontalHeaderLabels({"Tên", "Phòng ban", "Lương (VNĐ)", "Ngày vào"});
        
        // Thêm dữ liệu mẫu
        addEmployee("Nguyễn Văn A", "IT", 15000000, "2020-01-15");
        addEmployee("Trần Thị B", "HR", 12000000, "2019-05-20");
        addEmployee("Lê Văn C", "Sales", 18000000, "2021-03-10");
        addEmployee("Phạm Thị D", "IT", 20000000, "2018-11-30");
        addEmployee("Hoàng Văn E", "Finance", 16000000, "2020-07-22");
        addEmployee("Vũ Thị F", "Sales", 14000000, "2022-01-05");
        addEmployee("Đặng Văn G", "IT", 25000000, "2017-09-18");
        addEmployee("Bùi Thị H", "HR", 13000000, "2021-12-01");
        addEmployee("Cao Văn I", "Finance", 19000000, "2019-08-14");
        addEmployee("Dương Thị K", "Sales", 22000000, "2020-04-25");
    }
    
    void addEmployee(const QString &name, const QString &dept, int salary, const QString &date) {
        QList<QStandardItem*> row;
        row << new QStandardItem(name)
            << new QStandardItem(dept)
            << new QStandardItem(QString::number(salary))
            << new QStandardItem(date);
        m_sourceModel->appendRow(row);
    }
    
    void setupProxyModel() {
        // Tạo proxy model
        m_proxyModel = new EmployeeProxyModel(this);
        m_proxyModel->setSourceModel(m_sourceModel);
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    }
    
    void setupUI() {
        // Main layout
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(new QLabel("🔍 Demo QSortFilterProxyModel"));
        
        // Filter controls group
        auto *filterGroup = new QGroupBox("Bộ lọc");
        auto *filterLayout = new QVBoxLayout(filterGroup);
        
        // Text search
        auto *searchLayout = new QHBoxLayout;
        searchLayout->addWidget(new QLabel("Tìm kiếm:"));
        m_searchEdit = new QLineEdit;
        m_searchEdit->setPlaceholderText("Nhập tên hoặc phòng ban...");
        searchLayout->addWidget(m_searchEdit);
        filterLayout->addLayout(searchLayout);
        
        // Department filter
        auto *deptLayout = new QHBoxLayout;
        deptLayout->addWidget(new QLabel("Phòng ban:"));
        m_deptCombo = new QComboBox;
        m_deptCombo->addItems({"Tất cả", "IT", "HR", "Sales", "Finance"});
        deptLayout->addWidget(m_deptCombo);
        filterLayout->addLayout(deptLayout);
        
        // Salary range filter
        auto *salaryLayout = new QVBoxLayout;
        salaryLayout->addWidget(new QLabel("Khoảng lương:"));
        
        m_minSalarySlider = new QSlider(Qt::Horizontal);
        m_minSalarySlider->setRange(0, 30000000);
        m_minSalarySlider->setValue(0);
        m_minSalaryLabel = new QLabel("Min: 0 VNĐ");
        
        m_maxSalarySlider = new QSlider(Qt::Horizontal);
        m_maxSalarySlider->setRange(0, 30000000);
        m_maxSalarySlider->setValue(30000000);
        m_maxSalaryLabel = new QLabel("Max: 30,000,000 VNĐ");
        
        salaryLayout->addWidget(m_minSalaryLabel);
        salaryLayout->addWidget(m_minSalarySlider);
        salaryLayout->addWidget(m_maxSalaryLabel);
        salaryLayout->addWidget(m_maxSalarySlider);
        filterLayout->addLayout(salaryLayout);
        
        // Options
        m_caseSensitive = new QCheckBox("Phân biệt hoa/thường");
        m_regexFilter = new QCheckBox("Sử dụng Regular Expression");
        filterLayout->addWidget(m_caseSensitive);
        filterLayout->addWidget(m_regexFilter);
        
        mainLayout->addWidget(filterGroup);
        
        // Tables
        auto *tablesLayout = new QHBoxLayout;
        
        // Source table
        auto *sourceGroup = new QGroupBox("Dữ liệu gốc");
        auto *sourceLayout = new QVBoxLayout(sourceGroup);
        m_sourceView = new QTableView;
        m_sourceView->setModel(m_sourceModel);
        m_sourceView->setSortingEnabled(false);
        m_sourceView->horizontalHeader()->setStretchLastSection(true);
        sourceLayout->addWidget(m_sourceView);
        m_sourceCountLabel = new QLabel;
        sourceLayout->addWidget(m_sourceCountLabel);
        
        // Proxy table
        auto *proxyGroup = new QGroupBox("Dữ liệu sau khi lọc/sắp xếp");
        auto *proxyLayout = new QVBoxLayout(proxyGroup);
        m_proxyView = new QTableView;
        m_proxyView->setModel(m_proxyModel);
        m_proxyView->setSortingEnabled(true);
        m_proxyView->horizontalHeader()->setStretchLastSection(true);
        proxyLayout->addWidget(m_proxyView);
        m_proxyCountLabel = new QLabel;
        proxyLayout->addWidget(m_proxyCountLabel);
        
        tablesLayout->addWidget(sourceGroup);
        tablesLayout->addWidget(proxyGroup);
        mainLayout->addLayout(tablesLayout);
        
        // Action buttons
        auto *buttonLayout = new QHBoxLayout;
        auto *addBtn = new QPushButton("Thêm nhân viên ngẫu nhiên");
        auto *clearBtn = new QPushButton("Xóa bộ lọc");
        buttonLayout->addWidget(addBtn);
        buttonLayout->addWidget(clearBtn);
        mainLayout->addLayout(buttonLayout);
        
        // Connect buttons
        connect(addBtn, &QPushButton::clicked, this, &ProxyModelDemo::addRandomEmployee);
        connect(clearBtn, &QPushButton::clicked, this, &ProxyModelDemo::clearFilters);
        
        updateCounts();
    }
    
    void connectSignals() {
        // Text search
        connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
            if (m_regexFilter->isChecked()) {
                QRegularExpression regex(text);
                if (!regex.isValid()) {
                    m_searchEdit->setStyleSheet("background-color: #ffcccc;");
                    return;
                }
                m_searchEdit->setStyleSheet("");
                m_proxyModel->setFilterRegularExpression(regex);
            } else {
                m_proxyModel->setFilterWildcard(text);
            }
            updateCounts();
        });
        
        // Department filter
        connect(m_deptCombo, &QComboBox::currentTextChanged, this, [this](const QString &dept) {
            if (dept == "Tất cả") {
                m_proxyModel->setDepartmentFilter("");
            } else {
                m_proxyModel->setDepartmentFilter(dept);
            }
            updateCounts();
        });
        
        // Salary sliders
        connect(m_minSalarySlider, &QSlider::valueChanged, this, [this](int value) {
            m_minSalaryLabel->setText(QString("Min: %1 VNĐ").arg(QLocale().toString(value)));
            m_proxyModel->setMinimumSalary(value);
            updateCounts();
        });
        
        connect(m_maxSalarySlider, &QSlider::valueChanged, this, [this](int value) {
            m_maxSalaryLabel->setText(QString("Max: %1 VNĐ").arg(QLocale().toString(value)));
            m_proxyModel->setMaximumSalary(value);
            updateCounts();
        });
        
        // Options
        connect(m_caseSensitive, &QCheckBox::toggled, this, [this](bool checked) {
            m_proxyModel->setFilterCaseSensitivity(checked ? Qt::CaseSensitive : Qt::CaseInsensitive);
            updateCounts();
        });
        
        connect(m_regexFilter, &QCheckBox::toggled, this, [this](bool checked) {
            Q_UNUSED(checked);
            // Re-apply current filter with new mode
            m_searchEdit->textChanged(m_searchEdit->text());
        });
    }
    
    void updateCounts() {
        m_sourceCountLabel->setText(QString("Tổng: %1 nhân viên").arg(m_sourceModel->rowCount()));
        m_proxyCountLabel->setText(QString("Hiển thị: %1 nhân viên").arg(m_proxyModel->rowCount()));
    }
    
    void addRandomEmployee() {
        QStringList firstNames = {"Nguyễn", "Trần", "Lê", "Phạm", "Hoàng"};
        QStringList lastNames = {"Anh", "Bình", "Chi", "Dũng", "Hoa"};
        QStringList depts = {"IT", "HR", "Sales", "Finance"};
        
        QString name = firstNames[QRandomGenerator::global()->bounded(firstNames.size())] + " " +
                      lastNames[QRandomGenerator::global()->bounded(lastNames.size())];
        QString dept = depts[QRandomGenerator::global()->bounded(depts.size())];
        int salary = QRandomGenerator::global()->bounded(10000000, 30000000);
        QString date = QDate::currentDate().toString("yyyy-MM-dd");
        
        addEmployee(name, dept, salary, date);
        updateCounts();
    }
    
    void clearFilters() {
        m_searchEdit->clear();
        m_deptCombo->setCurrentIndex(0);
        m_minSalarySlider->setValue(0);
        m_maxSalarySlider->setValue(30000000);
        m_caseSensitive->setChecked(false);
        m_regexFilter->setChecked(false);
    }
    
private:
    // Models
    QStandardItemModel *m_sourceModel;
    EmployeeProxyModel *m_proxyModel;
    
    // Views
    QTableView *m_sourceView;
    QTableView *m_proxyView;
    
    // Filter controls
    QLineEdit *m_searchEdit;
    QComboBox *m_deptCombo;
    QSlider *m_minSalarySlider;
    QSlider *m_maxSalarySlider;
    QLabel *m_minSalaryLabel;
    QLabel *m_maxSalaryLabel;
    QCheckBox *m_caseSensitive;
    QCheckBox *m_regexFilter;
    
    // Info labels
    QLabel *m_sourceCountLabel;
    QLabel *m_proxyCountLabel;
};

#include "proxy_model_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ProxyModelDemo demo;
    demo.setWindowTitle("Proxy Model Example");
    demo.resize(1000, 800);
    demo.show();
    
    return app.exec();
}

/*
 * TỔNG KẾT VỀ QSortFilterProxyModel:
 * 
 * 1. Proxy Model KHÔNG chứa dữ liệu:
 *    - Chỉ là "filter" giữa source model và view
 *    - Mọi thay đổi ở source model tự động phản ánh
 * 
 * 2. Tính năng chính:
 *    - Filtering: Lọc dữ liệu theo điều kiện
 *    - Sorting: Sắp xếp không ảnh hưởng source
 *    - Mapping: Chuyển đổi index giữa proxy và source
 * 
 * 3. Methods quan trọng:
 *    - setSourceModel(): Kết nối với source model
 *    - filterAcceptsRow(): Override để custom filter
 *    - lessThan(): Override để custom sort
 *    - invalidateFilter(): Refresh filter
 * 
 * 4. Filter options:
 *    - setFilterRegularExpression(): Regex filter
 *    - setFilterWildcard(): Wildcard filter
 *    - setFilterFixedString(): Exact match
 *    - setFilterKeyColumn(): Column to filter (-1 = all)
 * 
 * 5. Index mapping:
 *    - mapToSource(): Proxy index -> Source index
 *    - mapFromSource(): Source index -> Proxy index
 * 
 * 6. Use cases:
 *    - Search functionality
 *    - Dynamic filtering UI
 *    - Multiple views của cùng data
 *    - Tạm thời ẩn/hiện rows
 */