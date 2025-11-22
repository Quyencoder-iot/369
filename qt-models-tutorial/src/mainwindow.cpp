#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QProcess>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFont>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createMenuItems();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Central widget
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Main layout với splitter
    auto *mainLayout = new QHBoxLayout(centralWidget);
    
    // Splitter để resize được
    auto *splitter = new QSplitter(Qt::Horizontal);
    
    // Left panel - Menu
    auto *leftPanel = new QWidget;
    auto *leftLayout = new QVBoxLayout(leftPanel);
    
    auto *menuLabel = new QLabel("📚 Danh sách ví dụ");
    menuLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    leftLayout->addWidget(menuLabel);
    
    m_menuList = new QListWidget;
    m_menuList->setAlternatingRowColors(true);
    leftLayout->addWidget(m_menuList);
    
    // Right panel - Content
    m_contentStack = new QStackedWidget;
    
    // Welcome widget
    m_welcomeWidget = new QWidget;
    auto *welcomeLayout = new QVBoxLayout(m_welcomeWidget);
    
    auto *welcomeLabel = new QLabel("🎯 Qt Model/View Tutorial");
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLayout->addWidget(welcomeLabel);
    
    auto *instructionText = new QTextEdit;
    instructionText->setReadOnly(true);
    instructionText->setHtml(
        "<h2>Hướng dẫn sử dụng:</h2>"
        "<ol>"
        "<li>Chọn một ví dụ từ menu bên trái</li>"
        "<li>Click nút 'Chạy ví dụ' để xem demo</li>"
        "<li>Đọc mô tả để hiểu cách hoạt động</li>"
        "</ol>"
        "<h2>Tài liệu:</h2>"
        "<ul>"
        "<li>📖 Xem file <b>docs/HUONG_DAN_QT_MODELS.md</b> để học lý thuyết</li>"
        "<li>🔗 Xem file <b>docs/MOI_QUAN_HE_MODELS.md</b> để hiểu relationships</li>"
        "<li>💻 Source code trong thư mục <b>examples/</b></li>"
        "</ul>"
        "<h2>Tips học nhanh:</h2>"
        "<ul>"
        "<li>🎯 Bắt đầu với <b>QAbstractListModel</b> (đơn giản nhất)</li>"
        "<li>📊 Sau đó học <b>QAbstractTableModel</b></li>"
        "<li>🌳 Tiếp theo là <b>Tree Model</b> (phức tạp hơn)</li>"
        "<li>🔍 Cuối cùng là <b>Proxy</b> và <b>SQL Models</b></li>"
        "</ul>"
    );
    welcomeLayout->addWidget(instructionText);
    
    m_contentStack->addWidget(m_welcomeWidget);
    
    // Add panels to splitter
    splitter->addWidget(leftPanel);
    splitter->addWidget(m_contentStack);
    splitter->setSizes({300, 700}); // Initial sizes
    
    mainLayout->addWidget(splitter);
    
    // Connect signals
    connect(m_menuList, &QListWidget::itemClicked, this, &MainWindow::onMenuItemClicked);
}

void MainWindow::createMenuItems()
{
    addMenuItem("QAbstractListModel", 
                "Model cho danh sách 1 chiều. Ví dụ về Task List với add/remove/update.",
                "list_model_example");
    
    addMenuItem("QAbstractTableModel",
                "Model cho dữ liệu dạng bảng 2 chiều. Ví dụ về quản lý sản phẩm với sort và custom display.",
                "table_model_example");
    
    addMenuItem("QAbstractItemModel (Tree)",
                "Model cho cấu trúc cây phân cấp. Ví dụ về file system với folders và files.",
                "tree_model_example");
    
    addMenuItem("QSortFilterProxyModel",
                "Proxy model để lọc và sắp xếp. Ví dụ về filtering nhân viên với nhiều tiêu chí.",
                "proxy_model_example");
    
    addMenuItem("QSql Models",
                "Các models làm việc với database. Demo QSqlTableModel, QSqlRelationalTableModel và QSqlQueryModel.",
                "sql_model_example");
}

void MainWindow::addMenuItem(const QString &title, const QString &description, const QString &executable)
{
    ExampleItem item{title, description, executable};
    m_examples.append(item);
    
    auto *listItem = new QListWidgetItem(title);
    listItem->setData(Qt::UserRole, m_examples.size() - 1);
    
    QFont font = listItem->font();
    font.setPointSize(11);
    listItem->setFont(font);
    
    m_menuList->addItem(listItem);
}

void MainWindow::onMenuItemClicked()
{
    auto *item = m_menuList->currentItem();
    if (!item) return;
    
    int index = item->data(Qt::UserRole).toInt();
    const ExampleItem &example = m_examples[index];
    
    // Create example widget nếu chưa có
    if (m_contentStack->count() == 1) {
        auto *exampleWidget = new QWidget;
        auto *layout = new QVBoxLayout(exampleWidget);
        
        // Title
        auto *titleLabel = new QLabel;
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; padding: 10px;");
        layout->addWidget(titleLabel);
        
        // Description
        auto *descLabel = new QLabel;
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border-radius: 5px;");
        layout->addWidget(descLabel);
        
        // Run button
        auto *runBtn = new QPushButton("▶️ Chạy ví dụ");
        runBtn->setStyleSheet("padding: 10px; font-size: 14px;");
        layout->addWidget(runBtn);
        
        // Source preview
        auto *sourceLabel = new QLabel("📝 Source code:");
        sourceLabel->setStyleSheet("font-weight: bold; margin-top: 20px;");
        layout->addWidget(sourceLabel);
        
        auto *sourceText = new QTextEdit;
        sourceText->setReadOnly(true);
        sourceText->setFont(QFont("Consolas", 9));
        layout->addWidget(sourceText);
        
        m_contentStack->addWidget(exampleWidget);
    }
    
    // Update content
    auto *exampleWidget = m_contentStack->widget(1);
    auto *titleLabel = exampleWidget->findChild<QLabel*>();
    auto *descLabel = exampleWidget->findChildren<QLabel*>()[1];
    auto *runBtn = exampleWidget->findChild<QPushButton*>();
    auto *sourceText = exampleWidget->findChild<QTextEdit*>();
    
    titleLabel->setText(example.title);
    descLabel->setText(example.description);
    
    // Disconnect previous connections
    disconnect(runBtn, nullptr, nullptr, nullptr);
    connect(runBtn, &QPushButton::clicked, this, [this, example]() {
        runExample(example.executable);
    });
    
    // Load source preview
    QString sourcePath = QString("examples/%1.cpp").arg(example.executable);
    QFile file(sourcePath);
    if (file.open(QIODevice::ReadOnly)) {
        QString content = file.readAll();
        // Chỉ show 50 dòng đầu
        QStringList lines = content.split('\n');
        if (lines.size() > 50) {
            lines = lines.mid(0, 50);
            lines.append("\n... (xem full code trong file " + sourcePath + ")");
        }
        sourceText->setPlainText(lines.join('\n'));
    }
    
    m_contentStack->setCurrentIndex(1);
}

void MainWindow::runExample(const QString &exampleName)
{
    QString program = "./" + exampleName;
    
    QProcess *process = new QProcess(this);
    process->start(program);
    
    if (!process->waitForStarted()) {
        QMessageBox::warning(this, "Lỗi", 
            QString("Không thể chạy %1.\n"
                    "Hãy chắc chắn bạn đã build project với CMake.").arg(exampleName));
    }
}