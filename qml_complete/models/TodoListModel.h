#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>

/**
 * Example 1: QAbstractListModel - Todo List
 * 
 * LIST MODEL:
 * - 1 chiều (single column)
 * - Dùng cho danh sách đơn giản
 * - Chỉ cần implement: rowCount(), data()
 */
class TodoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY dataChanged)
    
public:
    enum TodoRoles {
        TextRole = Qt::UserRole + 1,
        CompletedRole,
        PriorityRole,
        CreatedDateRole
    };
    Q_ENUM(TodoRoles)
    
    explicit TodoListModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface - BẮT BUỘC
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Methods for QML
    Q_INVOKABLE void addTodo(const QString &text, int priority = 2);
    Q_INVOKABLE void removeTodo(int index);
    Q_INVOKABLE void toggleCompleted(int index);
    Q_INVOKABLE void updateText(int index, const QString &text);
    Q_INVOKABLE void clear();
    Q_INVOKABLE int completedCount() const;
    Q_INVOKABLE int incompleteCount() const;
    
signals:
    void countChanged();
    
private:
    struct Todo {
        QString text;
        bool completed;
        int priority; // 1=High, 2=Medium, 3=Low
        QString createdDate;
    };
    
    QVector<Todo> m_todos;
};

#endif // TODOLISTMODEL_H
