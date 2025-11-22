/**
 * VÍ DỤ 1: QAbstractListModel - Danh sách Todo
 * 
 * ĐẶC ĐIỂM:
 * - Dữ liệu 1 chiều (list)
 * - Chỉ cần implement rowCount() và data()
 * - Editable với setData()
 */

#include <QAbstractListModel>
#include <QListView>
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>

// ============================================
// BƯỚC 1: Định nghĩa Model
// ============================================
class TodoListModel : public QAbstractListModel {
    Q_OBJECT
    
    // Cấu trúc dữ liệu
    struct Todo {
        QString text;
        bool completed;
        int priority; // 1=High, 2=Medium, 3=Low
    };
    
    QVector<Todo> m_todos; // Lưu data ở đây
    
public:
    // Constructor
    TodoListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        // Khởi tạo data mẫu
        m_todos = {
            {"Học Qt Models", false, 1},
            {"Đọc documentation", false, 2},
            {"Làm bài tập", false, 3}
        };
    }
    
    // ----------------------------------------
    // BẮT BUỘC: Trả về số lượng items
    // ----------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        // List model không có children, nên nếu parent valid thì return 0
        if (parent.isValid())
            return 0;
        
        return m_todos.count();
    }
    
    // ----------------------------------------
    // BẮT BUỘC: Trả về data cho mỗi item
    // ----------------------------------------
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        // Kiểm tra index hợp lệ
        if (!index.isValid() || index.row() >= m_todos.size())
            return QVariant();
        
        const Todo &todo = m_todos.at(index.row());
        
        // Xử lý theo role
        switch (role) {
            case Qt::DisplayRole: // Text hiển thị
                return QString("[%1] %2")
                    .arg(todo.completed ? "✓" : " ")
                    .arg(todo.text);
                
            case Qt::EditRole: // Text khi edit
                return todo.text;
                
            case Qt::CheckStateRole: // Checkbox state
                return todo.completed ? Qt::Checked : Qt::Unchecked;
                
            case Qt::ForegroundRole: // Màu text
                if (todo.completed)
                    return QColor(Qt::gray);
                if (todo.priority == 1)
                    return QColor(Qt::red);
                return QColor(Qt::black);
                
            case Qt::ToolTipRole: // Tooltip
                return QString("Priority: %1").arg(todo.priority);
                
            case Qt::UserRole: // Custom data
                return todo.priority;
                
            default:
                return QVariant();
        }
    }
    
    // ----------------------------------------
    // Cho phép edit
    // ----------------------------------------
    bool setData(const QModelIndex &index, const QVariant &value, 
                 int role = Qt::EditRole) override {
        if (!index.isValid() || index.row() >= m_todos.size())
            return false;
        
        Todo &todo = m_todos[index.row()];
        
        switch (role) {
            case Qt::EditRole:
                todo.text = value.toString();
                break;
                
            case Qt::CheckStateRole:
                todo.completed = (value.toInt() == Qt::Checked);
                break;
                
            default:
                return false;
        }
        
        // Thông báo view rằng data đã thay đổi
        emit dataChanged(index, index, {role});
        return true;
    }
    
    // ----------------------------------------
    // Flags: Cho biết item có thể làm gì
    // ----------------------------------------
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        
        return QAbstractListModel::flags(index) 
             | Qt::ItemIsEditable          // Có thể edit
             | Qt::ItemIsUserCheckable;    // Có thể check
    }
    
    // ----------------------------------------
    // Thêm todo mới
    // ----------------------------------------
    void addTodo(const QString &text, int priority = 2) {
        // Thông báo TRƯỚC khi insert
        int row = m_todos.count();
        beginInsertRows(QModelIndex(), row, row);
        
        // Insert data
        m_todos.append({text, false, priority});
        
        // Thông báo SAU khi insert
        endInsertRows();
    }
    
    // ----------------------------------------
    // Xóa todo
    // ----------------------------------------
    bool removeTodo(int row) {
        if (row < 0 || row >= m_todos.count())
            return false;
        
        // Thông báo TRƯỚC khi remove
        beginRemoveRows(QModelIndex(), row, row);
        
        // Remove data
        m_todos.remove(row);
        
        // Thông báo SAU khi remove
        endRemoveRows();
        
        return true;
    }
};

// ============================================
// BƯỚC 2: Sử dụng Model
// ============================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Tạo window
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // Tạo model
    TodoListModel *model = new TodoListModel;
    
    // Tạo view
    QListView *view = new QListView;
    view->setModel(model); // KẾT NỐI MODEL VỚI VIEW
    
    // Tạo input
    QLineEdit *input = new QLineEdit;
    input->setPlaceholderText("Nhập todo mới...");
    
    QPushButton *addBtn = new QPushButton("Thêm");
    QPushButton *removeBtn = new QPushButton("Xóa item được chọn");
    
    // Thêm todo
    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        if (!input->text().isEmpty()) {
            model->addTodo(input->text());
            input->clear();
        }
    });
    
    // Xóa todo
    QObject::connect(removeBtn, &QPushButton::clicked, [=]() {
        QModelIndex current = view->currentIndex();
        if (current.isValid()) {
            model->removeTodo(current.row());
        }
    });
    
    // Layout
    layout->addWidget(view);
    layout->addWidget(input);
    layout->addWidget(addBtn);
    layout->addWidget(removeBtn);
    
    window.show();
    return app.exec();
}

#include "01_list_model.moc"

// ============================================
// CÁCH ĐỌC CODE LIST MODEL:
// ============================================
// 1. Tìm data storage: m_todos (QVector<Todo>)
// 2. rowCount(): return m_todos.count()
// 3. data(): switch(role) → trả về gì cho mỗi role
// 4. setData(): update m_todos → emit dataChanged()
// 5. addTodo(): beginInsertRows() → append() → endInsertRows()
// 6. removeTodo(): beginRemoveRows() → remove() → endRemoveRows()
