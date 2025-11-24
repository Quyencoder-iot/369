#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>

/**
 * TodoListModel - C++ Model cho QML
 * 
 * Expose sang QML với:
 * - Q_INVOKABLE methods
 * - Custom roles
 */
class TodoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    // Custom roles cho QML
    enum TodoRoles {
        TextRole = Qt::UserRole + 1,
        CompletedRole,
        PriorityRole
    };
    Q_ENUM(TodoRoles)
    
    explicit TodoListModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Methods cho QML (Q_INVOKABLE)
    Q_INVOKABLE void addTodo(const QString &text, int priority = 2);
    Q_INVOKABLE void removeTodo(int index);
    Q_INVOKABLE void toggleCompleted(int index);
    Q_INVOKABLE QString getText(int index) const;
    Q_INVOKABLE bool getCompleted(int index) const;
    Q_INVOKABLE int getPriority(int index) const;
    
signals:
    void countChanged();
    
private:
    struct Todo {
        QString text;
        bool completed;
        int priority; // 1=High, 2=Medium, 3=Low
    };
    
    QVector<Todo> m_todos;
};

#endif // TODOLISTMODEL_H
