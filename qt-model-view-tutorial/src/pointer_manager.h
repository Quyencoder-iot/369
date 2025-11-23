// pointer_manager.h - Utility class để demo pointer management patterns

#ifndef POINTER_MANAGER_H
#define POINTER_MANAGER_H

#include <QObject>
#include <QPointer>
#include <memory>
#include <QHash>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractItemView;
class QAbstractItemDelegate;
QT_END_NAMESPACE

// Demo class for various pointer management techniques
class PointerManager : public QObject
{
    Q_OBJECT
    
public:
    explicit PointerManager(QObject* parent = nullptr);
    ~PointerManager();
    
    // Demo different pointer patterns
    void demonstrateRawPointers();
    void demonstrateSmartPointers();
    void demonstrateQPointer();
    void demonstrateParentChild();
    void demonstrateCircularReference();
    void demonstrateMemoryLeaks();
    
    // Utility methods
    void trackObject(QObject* obj, const QString& name);
    void showTrackedObjects();
    void cleanupOrphans();
    
signals:
    void logMessage(const QString& message);
    
private:
    // Different pointer storage patterns
    QList<QObject*> m_rawPointers;
    QList<std::unique_ptr<QObject>> m_smartPointers;
    QList<QPointer<QObject>> m_safePointers;
    QHash<QString, QPointer<QObject>> m_trackedObjects;
    
    void log(const QString& msg);
};

// Helper class to demonstrate lifecycle
class LifecycleObject : public QObject
{
    Q_OBJECT
    
public:
    explicit LifecycleObject(const QString& name, QObject* parent = nullptr);
    ~LifecycleObject();
    
    QString name() const { return m_name; }
    
signals:
    void created(const QString& name);
    void destroyed(const QString& name);
    
private:
    QString m_name;
};

#endif // POINTER_MANAGER_H