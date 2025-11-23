// pointer_manager.cpp - Implementation của pointer manager

#include "pointer_manager.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QTimer>
#include <QDebug>

PointerManager::PointerManager(QObject* parent)
    : QObject(parent)
{
    log("PointerManager created");
}

PointerManager::~PointerManager()
{
    log("PointerManager destructor - cleaning up...");
    
    // Clean raw pointers that don't have parents
    for (QObject* obj : m_rawPointers) {
        if (obj && !obj->parent()) {
            log(QString("Deleting orphan object: %1").arg(obj->objectName()));
            delete obj;
        }
    }
    
    // Smart pointers auto-cleanup
    log(QString("Smart pointers will auto-cleanup %1 objects").arg(m_smartPointers.size()));
    
    // Check QPointers
    int validQPointers = 0;
    for (const auto& ptr : m_safePointers) {
        if (ptr) validQPointers++;
    }
    log(QString("QPointers still valid: %1 of %2").arg(validQPointers).arg(m_safePointers.size()));
}

void PointerManager::demonstrateRawPointers()
{
    log("\n=== Raw Pointer Demo ===");
    
    // Create objects with raw pointers
    QObject* obj1 = new QObject();
    obj1->setObjectName("RawObject1");
    m_rawPointers.append(obj1);
    log(QString("Created raw pointer object: %1 at %2")
        .arg(obj1->objectName())
        .arg((qintptr)obj1, 0, 16));
    
    // Object with parent
    QObject* obj2 = new QObject(this);
    obj2->setObjectName("RawObject2_WithParent");
    m_rawPointers.append(obj2);
    log(QString("Created object with parent: %1").arg(obj2->objectName()));
    
    // Dangerous pattern - stack object address
    {
        QObject stackObj;
        stackObj.setObjectName("StackObject");
        // m_rawPointers.append(&stackObj); // DANGEROUS!
        log("Stack objects should NOT be stored in pointer containers!");
    }
    
    log("Raw pointers require manual memory management");
    log("Objects with parents will be auto-deleted");
}

void PointerManager::demonstrateSmartPointers()
{
    log("\n=== Smart Pointer Demo ===");
    
    // unique_ptr
    auto obj1 = std::make_unique<LifecycleObject>("UniquePtr_Object1");
    connect(obj1.get(), &LifecycleObject::destroyed, this, [this](const QString& name) {
        log(QString("Smart pointer object destroyed: %1").arg(name));
    });
    
    log(QString("Created unique_ptr object: %1").arg(obj1->name()));
    
    // Move to container
    m_smartPointers.push_back(std::move(obj1));
    
    // shared_ptr example
    auto shared1 = std::make_shared<LifecycleObject>("SharedPtr_Object1");
    auto shared2 = shared1; // Share ownership
    log(QString("Created shared_ptr with ref count: %1").arg(shared1.use_count()));
    
    // Custom deleter example
    auto customDeleter = [this](QObject* obj) {
        log(QString("Custom deleter called for: %1").arg(obj->objectName()));
        delete obj;
    };
    
    std::unique_ptr<QObject, decltype(customDeleter)> obj2(
        new QObject(), customDeleter);
    obj2->setObjectName("CustomDeleter_Object");
    
    log("Smart pointers provide automatic memory management");
}

void PointerManager::demonstrateQPointer()
{
    log("\n=== QPointer Demo ===");
    
    // Create object
    LifecycleObject* obj = new LifecycleObject("QPointer_Object");
    QPointer<LifecycleObject> safePtr = obj;
    m_safePointers.append(safePtr);
    
    log(QString("Created object: %1").arg(obj->name()));
    log(QString("QPointer valid: %1").arg(safePtr ? "true" : "false"));
    
    // Track it
    trackObject(obj, "QPointerDemo");
    
    // Delete object
    delete obj;
    
    // Check QPointer
    log("After deletion:");
    log(QString("QPointer valid: %1").arg(safePtr ? "true" : "false"));
    log("QPointer automatically becomes null when object is deleted!");
    
    // QPointer with model/view
    QStandardItemModel* model = new QStandardItemModel();
    QPointer<QStandardItemModel> modelPtr = model;
    
    QTableView* view = new QTableView();
    view->setModel(modelPtr);
    
    log("QPointer is perfect for model/view scenarios");
    
    // Cleanup
    delete view;
    delete model;
}

void PointerManager::demonstrateParentChild()
{
    log("\n=== Parent-Child Demo ===");
    
    // Create parent
    LifecycleObject* parent = new LifecycleObject("Parent");
    trackObject(parent, "ParentObject");
    
    // Create children
    LifecycleObject* child1 = new LifecycleObject("Child1", parent);
    LifecycleObject* child2 = new LifecycleObject("Child2", parent);
    LifecycleObject* grandchild = new LifecycleObject("Grandchild", child1);
    
    log(QString("Parent has %1 children").arg(parent->children().count()));
    log(QString("Child1 has %1 children").arg(child1->children().count()));
    
    // Demonstrate ownership transfer
    LifecycleObject* orphan = new LifecycleObject("Orphan");
    log(QString("Orphan parent: %1").arg(orphan->parent() ? "exists" : "null"));
    
    orphan->setParent(parent);
    log(QString("Orphan adopted - new parent: %1").arg(orphan->parent()->objectName()));
    
    // Delete parent - all children deleted
    log("\nDeleting parent - watch the cascade...");
    delete parent;
    
    // Check tracked object
    showTrackedObjects();
}

void PointerManager::demonstrateCircularReference()
{
    log("\n=== Circular Reference Demo ===");
    
    // Potential circular reference with Qt objects
    QObject* obj1 = new QObject();
    QObject* obj2 = new QObject();
    
    obj1->setObjectName("Circular1");
    obj2->setObjectName("Circular2");
    
    // Store references to each other (through properties)
    obj1->setProperty("buddy", QVariant::fromValue(obj2));
    obj2->setProperty("buddy", QVariant::fromValue(obj1));
    
    log("Created potential circular reference through properties");
    log("Qt's parent-child prevents true circular ownership");
    
    // In Qt, one must be parent of the other or both must have same parent
    obj2->setParent(obj1);
    log("Set parent-child to break circular dependency");
    
    // Clean up
    delete obj1; // obj2 deleted automatically
}

void PointerManager::demonstrateMemoryLeaks()
{
    log("\n=== Common Memory Leak Patterns ===");
    
    // Leak 1: Forgotten new without delete
    log("\nLeak Pattern 1: Forgotten allocation");
    log("  QObject* leak1 = new QObject();");
    log("  // Forget to delete - LEAK!");
    
    // Leak 2: Exception before delete
    log("\nLeak Pattern 2: Exception safety");
    log("  QObject* leak2 = new QObject();");
    log("  doSomethingThatMightThrow(); // Exception!");
    log("  delete leak2; // Never reached - LEAK!");
    
    // Leak 3: Container of pointers
    log("\nLeak Pattern 3: Container cleanup");
    log("  QList<QObject*> objects;");
    log("  objects.append(new QObject()); // Who deletes?");
    log("  objects.clear(); // Doesn't delete objects - LEAK!");
    
    // Leak 4: Disconnected model/view
    log("\nLeak Pattern 4: Model without parent");
    log("  QStandardItemModel* model = new QStandardItemModel();");
    log("  view->setModel(model);");
    log("  // View doesn't own model - must delete manually");
    
    // Solutions
    log("\n=== Solutions ===");
    log("1. Always set parent for Qt objects");
    log("2. Use smart pointers (unique_ptr, shared_ptr)");
    log("3. Use QPointer for safety checks");
    log("4. RAII - Resource Acquisition Is Initialization");
    log("5. Use tools: valgrind, address sanitizer, static analysis");
}

void PointerManager::trackObject(QObject* obj, const QString& name)
{
    if (!obj) return;
    
    m_trackedObjects[name] = obj;
    log(QString("Tracking object '%1' at %2")
        .arg(name)
        .arg((qintptr)obj, 0, 16));
    
    // Connect to track destruction
    connect(obj, &QObject::destroyed, this, [this, name]() {
        log(QString("Tracked object '%1' was destroyed").arg(name));
    });
}

void PointerManager::showTrackedObjects()
{
    log("\n=== Tracked Objects Status ===");
    log(QString("Total tracked: %1").arg(m_trackedObjects.size()));
    
    for (auto it = m_trackedObjects.begin(); it != m_trackedObjects.end(); ++it) {
        const QString& name = it.key();
        QPointer<QObject> obj = it.value();
        
        if (obj) {
            log(QString("  %1: ALIVE at %2")
                .arg(name)
                .arg((qintptr)obj.data(), 0, 16));
        } else {
            log(QString("  %1: DESTROYED (QPointer is null)")
                .arg(name));
        }
    }
}

void PointerManager::cleanupOrphans()
{
    log("\n=== Cleaning Up Orphan Objects ===");
    
    int cleaned = 0;
    
    // Check raw pointers
    for (int i = m_rawPointers.size() - 1; i >= 0; --i) {
        QObject* obj = m_rawPointers[i];
        if (obj && !obj->parent()) {
            log(QString("Deleting orphan: %1").arg(obj->objectName()));
            delete obj;
            m_rawPointers.removeAt(i);
            cleaned++;
        }
    }
    
    // Remove null QPointers
    m_safePointers.removeAll(QPointer<QObject>());
    
    // Clean tracked objects
    QStringList toRemove;
    for (auto it = m_trackedObjects.begin(); it != m_trackedObjects.end(); ++it) {
        if (!it.value()) {
            toRemove.append(it.key());
        }
    }
    
    for (const QString& key : toRemove) {
        m_trackedObjects.remove(key);
    }
    
    log(QString("Cleaned up %1 orphan objects").arg(cleaned));
    log(QString("Removed %1 null tracked references").arg(toRemove.size()));
}

void PointerManager::log(const QString& msg)
{
    emit logMessage(msg);
    qDebug() << "[PointerManager]" << msg;
}

// LifecycleObject implementation
LifecycleObject::LifecycleObject(const QString& name, QObject* parent)
    : QObject(parent), m_name(name)
{
    setObjectName(name);
    qDebug() << "LifecycleObject created:" << m_name << "at" << this;
    emit created(m_name);
}

LifecycleObject::~LifecycleObject()
{
    qDebug() << "LifecycleObject destroyed:" << m_name << "at" << this;
    emit destroyed(m_name);
}