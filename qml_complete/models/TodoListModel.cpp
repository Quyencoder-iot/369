#include "TodoListModel.h"
#include <QDate>

TodoListModel::TodoListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Sample data
    m_todos = {
        {"Học QAbstractListModel", false, 1, QDate::currentDate().toString("yyyy-MM-dd")},
        {"Hiểu roleNames()", false, 1, QDate::currentDate().toString("yyyy-MM-dd")},
        {"Implement Q_INVOKABLE methods", false, 2, QDate::currentDate().toString("yyyy-MM-dd")},
        {"Tạo QML view", false, 2, QDate::currentDate().toString("yyyy-MM-dd")},
        {"Test app", false, 3, QDate::currentDate().toString("yyyy-MM-dd")}
    };
}

int TodoListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_todos.count();
}

QVariant TodoListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_todos.size())
        return QVariant();
    
    const Todo &todo = m_todos.at(index.row());
    
    switch (role) {
        case TextRole:
            return todo.text;
        case CompletedRole:
            return todo.completed;
        case PriorityRole:
            return todo.priority;
        case CreatedDateRole:
            return todo.createdDate;
        case Qt::DisplayRole:
            return QString("[%1] %2").arg(todo.completed ? "✓" : " ").arg(todo.text);
    }
    
    return QVariant();
}

bool TodoListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_todos.size())
        return false;
    
    Todo &todo = m_todos[index.row()];
    
    switch (role) {
        case TextRole:
            todo.text = value.toString();
            break;
        case CompletedRole:
            todo.completed = value.toBool();
            break;
        case PriorityRole:
            todo.priority = value.toInt();
            break;
        default:
            return false;
    }
    
    emit dataChanged(index, index, {role});
    return true;
}

Qt::ItemFlags TodoListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> TodoListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[CompletedRole] = "completed";
    roles[PriorityRole] = "priority";
    roles[CreatedDateRole] = "createdDate";
    return roles;
}

void TodoListModel::addTodo(const QString &text, int priority)
{
    int row = m_todos.count();
    beginInsertRows(QModelIndex(), row, row);
    m_todos.append({text, false, priority, QDate::currentDate().toString("yyyy-MM-dd")});
    endInsertRows();
    emit countChanged();
}

void TodoListModel::removeTodo(int index)
{
    if (index < 0 || index >= m_todos.count())
        return;
    
    beginRemoveRows(QModelIndex(), index, index);
    m_todos.remove(index);
    endRemoveRows();
    emit countChanged();
}

void TodoListModel::toggleCompleted(int index)
{
    if (index < 0 || index >= m_todos.count())
        return;
    
    m_todos[index].completed = !m_todos[index].completed;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {CompletedRole});
}

void TodoListModel::updateText(int index, const QString &text)
{
    if (index < 0 || index >= m_todos.count())
        return;
    
    m_todos[index].text = text;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {TextRole});
}

void TodoListModel::clear()
{
    beginResetModel();
    m_todos.clear();
    endResetModel();
    emit countChanged();
}

int TodoListModel::completedCount() const
{
    int count = 0;
    for (const Todo &todo : m_todos) {
        if (todo.completed) count++;
    }
    return count;
}

int TodoListModel::incompleteCount() const
{
    return m_todos.count() - completedCount();
}

/**
 * ============================================
 * 📚 SUMMARY - TodoListModel Implementation
 * ============================================
 * 
 * MODEL TYPE: QAbstractListModel (1D list)
 * 
 * KEY METHODS IMPLEMENTED:
 * ✅ rowCount() - Số lượng items
 * ✅ data() - Lấy data theo role
 * ✅ setData() - Sửa data (optional, for editing)
 * ✅ roleNames() - Map roles → QML property names
 * ✅ flags() - Item flags (editable, selectable, etc.)
 * 
 * CRUD OPERATIONS (Q_INVOKABLE):
 * ✅ addTodo() - Thêm item
 * ✅ removeTodo() - Xóa item
 * ✅ toggleCompleted() - Update item
 * ✅ clear() - Xóa tất cả
 * 
 * Q_PROPERTY (for QML binding):
 * ✅ count - Total todos
 * ✅ completedCount - Completed todos
 * ✅ incompleteCount - Incomplete todos
 * 
 * SIGNALS (auto-update QML):
 * ✅ countChanged()
 * ✅ completedCountChanged()
 * ✅ incompleteCountChanged()
 * ✅ dataChanged() - From QAbstractListModel
 * 
 * DATA FLOW:
 * 1. User action in QML (e.g., button click)
 *    ↓
 * 2. Call Q_INVOKABLE method (e.g., addTodo)
 *    ↓
 * 3. beginInsertRows() - Notify view
 *    ↓
 * 4. Modify m_todos (QList<Todo>)
 *    ↓
 * 5. endInsertRows() - Finalize notification
 *    ↓
 * 6. emit signals (countChanged, etc.)
 *    ↓
 * 7. QML property bindings auto-update
 *    ↓
 * 8. QML ListView calls data() for new item
 *    ↓
 * 9. UI displays updated data
 * 
 * ROLES EXPLAINED:
 * - TextRole (Qt::UserRole + 1) → "text" in QML
 * - CompletedRole (Qt::UserRole + 2) → "completed" in QML
 * - PriorityRole (Qt::UserRole + 3) → "priority" in QML
 * - CreatedDateRole (Qt::UserRole + 4) → "createdDate" in QML
 * 
 * QML ACCESS:
 * ListView {
 *     model: todoModel
 *     delegate: Rectangle {
 *         Text { text: model.text }  ← TextRole
 *         CheckBox { checked: model.completed }  ← CompletedRole
 *     }
 * }
 * 
 * IMPORTANT CONCEPTS:
 * 1. beginInsertRows/endInsertRows - MUST be called
 * 2. beginRemoveRows/endRemoveRows - MUST be called
 * 3. emit dataChanged() - For in-place edits
 * 4. roleNames() - Qt::UserRole + N → string names
 * 5. Q_INVOKABLE - Makes C++ methods callable from QML
 * 6. Q_PROPERTY - Makes C++ properties accessible in QML
 * 7. NOTIFY signal - Auto-updates QML when property changes
 * 
 * ============================================
 */
