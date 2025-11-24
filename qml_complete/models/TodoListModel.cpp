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
