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

/**
 * ============================================
 * 📚 SUMMARY - TodoListModel Header
 * ============================================
 * 
 * CLASS HIERARCHY:
 * QObject → QAbstractItemModel → QAbstractListModel → TodoListModel
 * 
 * INHERITANCE:
 * - QAbstractListModel: Base class for 1D list models
 * - Provides: data(), rowCount(), etc.
 * 
 * Q_OBJECT MACRO:
 * - Enables Qt meta-object system
 * - Required for: signals, slots, properties, Q_INVOKABLE
 * - Processed by moc (Meta-Object Compiler)
 * 
 * Q_PROPERTY SYNTAX:
 * Q_PROPERTY(type name READ getter NOTIFY signal)
 * 
 * Example:
 * Q_PROPERTY(int count READ count NOTIFY countChanged)
 *   - type: int
 *   - name: count (accessible as todoModel.count in QML)
 *   - READ: count() getter function
 *   - NOTIFY: countChanged() signal (auto-updates QML)
 * 
 * Q_INVOKABLE:
 * - Makes C++ method callable from QML
 * - Alternative: use "public slots:"
 * - Example: todoModel.addTodo("text", 1) in QML
 * 
 * CUSTOM ROLES:
 * enum Roles {
 *     TextRole = Qt::UserRole + 1,  // 256
 *     CompletedRole,                 // 257
 *     PriorityRole,                  // 258
 *     CreatedDateRole                // 259
 * };
 * 
 * WHY Qt::UserRole + N?
 * - Qt reserves 0-255 for built-in roles
 * - Qt::UserRole = 256
 * - Custom roles start at 256+
 * 
 * DATA STRUCTURE:
 * struct Todo {
 *     QString text;
 *     bool completed;
 *     int priority;
 *     QString createdDate;
 * };
 * QList<Todo> m_todos;  // Storage
 * 
 * SIGNAL-SLOT CONNECTION:
 * C++:
 *   emit countChanged();
 *     ↓
 * QML:
 *   Label { text: todoModel.count }  // Auto-updates!
 * 
 * KEY POINTS:
 * ✅ Always use Q_OBJECT macro
 * ✅ Declare roles as enum
 * ✅ Override roleNames()
 * ✅ Use Q_PROPERTY for QML properties
 * ✅ Use Q_INVOKABLE for QML methods
 * ✅ Emit signals when data changes
 * 
 * ============================================
 */
