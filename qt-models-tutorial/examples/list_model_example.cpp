// Ví dụ về QAbstractListModel - Model danh sách đơn giản
#include <QApplication>
#include <QListView>
#include <QAbstractListModel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QWidget>
#include <QDebug>

// Định nghĩa một Task với tiêu đề và trạng thái
struct Task {
    QString title;
    bool completed = false;
};

// Custom List Model để quản lý danh sách tasks
class TaskListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    // Định nghĩa custom roles
    enum TaskRoles {
        TitleRole = Qt::UserRole + 1,
        CompletedRole
    };
    
    explicit TaskListModel(QObject *parent = nullptr) 
        : QAbstractListModel(parent) {
        // Khởi tạo một số tasks mẫu
        m_tasks << Task{"Học Qt Model/View", false}
                << Task{"Viết code ví dụ", false}
                << Task{"Đọc documentation", true};
    }
    
    // 1. PHƯƠNG THỨC BẮT BUỘC: Trả về số lượng items
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent); // List model không có parent
        return m_tasks.size();
    }
    
    // 2. PHƯƠNG THỨC BẮT BUỘC: Trả về dữ liệu cho từng role
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        // Kiểm tra index hợp lệ
        if (!index.isValid() || index.row() >= m_tasks.size())
            return QVariant();
            
        const Task &task = m_tasks.at(index.row());
        
        switch (role) {
        case Qt::DisplayRole:
        case TitleRole:
            return task.title;
            
        case Qt::CheckStateRole:
            return task.completed ? Qt::Checked : Qt::Unchecked;
            
        case CompletedRole:
            return task.completed;
            
        case Qt::DecorationRole:
            // Có thể return QIcon ở đây
            return QVariant();
            
        default:
            return QVariant();
        }
    }
    
    // 3. PHƯƠNG THỨC TÙY CHỌN: Cho phép edit dữ liệu
    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (!index.isValid() || index.row() >= m_tasks.size())
            return false;
            
        Task &task = m_tasks[index.row()];
        
        switch (role) {
        case Qt::EditRole:
        case TitleRole:
            task.title = value.toString();
            break;
            
        case Qt::CheckStateRole:
        case CompletedRole:
            task.completed = value.toBool();
            break;
            
        default:
            return false;
        }
        
        // QUAN TRỌNG: Phải emit signal này để view update
        emit dataChanged(index, index, {role});
        return true;
    }
    
    // 4. PHƯƠNG THỨC TÙY CHỌN: Khai báo flags cho items
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
            
        // Cho phép select, enable và check
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }
    
    // 5. CUSTOM METHODS: Thêm/xóa tasks
    void addTask(const QString &title) {
        // QUAN TRỌNG: Phải gọi begin/end khi thay đổi rows
        int row = m_tasks.size();
        beginInsertRows(QModelIndex(), row, row);
        m_tasks.append(Task{title, false});
        endInsertRows();
    }
    
    void removeTask(int row) {
        if (row < 0 || row >= m_tasks.size())
            return;
            
        // QUAN TRỌNG: Phải gọi begin/end khi xóa rows
        beginRemoveRows(QModelIndex(), row, row);
        m_tasks.removeAt(row);
        endRemoveRows();
    }
    
    // 6. PHƯƠNG THỨC HỮU ÍCH: Role names cho QML
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "title";
        roles[CompletedRole] = "completed";
        return roles;
    }
    
private:
    QList<Task> m_tasks; // Container lưu dữ liệu
};

// Widget demo
class ListModelDemo : public QWidget {
    Q_OBJECT
    
public:
    ListModelDemo() {
        // Tạo model
        m_model = new TaskListModel(this);
        
        // Tạo view
        m_listView = new QListView;
        m_listView->setModel(m_model);
        
        // Tạo controls
        m_input = new QLineEdit;
        m_input->setPlaceholderText("Nhập task mới...");
        
        auto *addBtn = new QPushButton("Thêm Task");
        auto *removeBtn = new QPushButton("Xóa Task Đã Chọn");
        
        // Layout
        auto *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("📝 Demo QAbstractListModel"));
        layout->addWidget(m_listView);
        layout->addWidget(m_input);
        layout->addWidget(addBtn);
        layout->addWidget(removeBtn);
        
        // Connections
        connect(addBtn, &QPushButton::clicked, this, [this]() {
            if (!m_input->text().isEmpty()) {
                m_model->addTask(m_input->text());
                m_input->clear();
            }
        });
        
        connect(removeBtn, &QPushButton::clicked, this, [this]() {
            auto indexes = m_listView->selectionModel()->selectedIndexes();
            if (!indexes.isEmpty()) {
                m_model->removeTask(indexes.first().row());
            }
        });
        
        // Double click để toggle completed
        connect(m_listView, &QListView::doubleClicked, this, [this](const QModelIndex &index) {
            bool completed = index.data(TaskListModel::CompletedRole).toBool();
            m_model->setData(index, !completed, Qt::CheckStateRole);
        });
    }
    
private:
    TaskListModel *m_model;
    QListView *m_listView;
    QLineEdit *m_input;
};

// Include moc file
#include "list_model_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ListModelDemo demo;
    demo.setWindowTitle("List Model Example");
    demo.resize(400, 500);
    demo.show();
    
    return app.exec();
}

/*
 * TỔNG KẾT VỀ QAbstractListModel:
 * 
 * 1. Phải implement ít nhất:
 *    - rowCount(): Số lượng items
 *    - data(): Trả về dữ liệu cho mỗi role
 * 
 * 2. Nên implement:
 *    - setData(): Cho phép edit
 *    - flags(): Định nghĩa behavior của items
 *    - roleNames(): Cho QML
 * 
 * 3. Luôn nhớ:
 *    - Gọi beginInsertRows/endInsertRows khi thêm
 *    - Gọi beginRemoveRows/endRemoveRows khi xóa
 *    - Emit dataChanged khi update dữ liệu
 * 
 * 4. Tips:
 *    - Dùng Q_UNUSED cho params không dùng
 *    - Kiểm tra index.isValid() trước khi dùng
 *    - Custom roles bắt đầu từ Qt::UserRole + 1
 */