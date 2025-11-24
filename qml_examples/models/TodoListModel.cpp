#include "TodoListModel.h"

TodoListModel::TodoListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Sample data
    m_todos = {
        {"Học Qt Models", false, 1},
        {"Học QML", false, 1},
        {"Tạo ứng dụng", false, 2},
        {"Deploy app", false, 3}
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
    return roles;
}

void TodoListModel::addTodo(const QString &text, int priority)
{
    int row = m_todos.count();
    beginInsertRows(QModelIndex(), row, row);
    m_todos.append({text, false, priority});
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

QString TodoListModel::getText(int index) const
{
    if (index < 0 || index >= m_todos.count())
        return QString();
    return m_todos.at(index).text;
}

bool TodoListModel::getCompleted(int index) const
{
    if (index < 0 || index >= m_todos.count())
        return false;
    return m_todos.at(index).completed;
}

int TodoListModel::getPriority(int index) const
{
    if (index < 0 || index >= m_todos.count())
        return 2;
    return m_todos.at(index).priority;
}
