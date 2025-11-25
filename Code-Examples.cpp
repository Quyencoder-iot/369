/**
 * Code Examples for Qt Core Classes Deep Dive
 * 
 * Các ví dụ thực tế để thực hành và hiểu sâu về:
 * - QGuiApplication
 * - QQmlApplicationEngine  
 * - QQmlContext
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QDebug>
#include <QElapsedTimer>

// ============================================================================
// Example 1: Custom QGuiApplication với Event Logging
// ============================================================================

class DebugApplication : public QGuiApplication {
public:
    DebugApplication(int &argc, char **argv) 
        : QGuiApplication(argc, argv) 
    {
        qDebug() << "DebugApplication constructed";
    }
    
    bool notify(QObject *receiver, QEvent *event) override {
        // Log tất cả events (warning: rất nhiều!)
        static int eventCount = 0;
        
        // Chỉ log một số event types quan trọng
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::KeyPress ||
            event->type() == QEvent::Timer) {
            
            qDebug() << "[Event" << ++eventCount << "]"
                     << "Type:" << event->type()
                     << "Receiver:" << receiver->objectName()
                     << "Class:" << receiver->metaObject()->className();
        }
        
        // Measure event processing time
        QElapsedTimer timer;
        timer.start();
        
        bool result = QGuiApplication::notify(receiver, event);
        
        qint64 elapsed = timer.nsecsElapsed();
        if (elapsed > 1000000) { // > 1ms
            qWarning() << "Slow event processing:" << elapsed / 1000000.0 << "ms"
                      << "Type:" << event->type();
        }
        
        return result;
    }
};

// ============================================================================
// Example 2: Context Property Object với Signals
// ============================================================================

class DataModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    
public:
    explicit DataModel(QObject *parent = nullptr) 
        : QObject(parent)
        , m_count(0)
        , m_message("Initial message")
    {
    }
    
    int count() const { return m_count; }
    void setCount(int count) {
        if (m_count != count) {
            m_count = count;
            qDebug() << "DataModel::count changed to:" << count;
            emit countChanged();
        }
    }
    
    QString message() const { return m_message; }
    void setMessage(const QString &msg) {
        if (m_message != msg) {
            m_message = msg;
            qDebug() << "DataModel::message changed to:" << msg;
            emit messageChanged();
        }
    }
    
    // Invokable methods - có thể gọi từ QML
    Q_INVOKABLE void increment() {
        setCount(m_count + 1);
    }
    
    Q_INVOKABLE QString formatMessage(const QString &prefix) {
        return prefix + ": " + m_message;
    }
    
signals:
    void countChanged();
    void messageChanged();
    
private:
    int m_count;
    QString m_message;
};

// ============================================================================
// Example 3: Custom QQmlApplicationEngine với Loading Callbacks
// ============================================================================

class InstrumentedEngine : public QQmlApplicationEngine {
    Q_OBJECT
    
public:
    explicit InstrumentedEngine(QObject *parent = nullptr)
        : QQmlApplicationEngine(parent)
    {
        // Connect signals để track loading
        connect(this, &QQmlApplicationEngine::objectCreated,
                this, &InstrumentedEngine::onObjectCreated);
        
        connect(this, &QQmlApplicationEngine::warnings,
                this, &InstrumentedEngine::onWarnings);
    }
    
    void loadWithTiming(const QUrl &url) {
        qDebug() << "=== Loading QML:" << url;
        
        QElapsedTimer timer;
        timer.start();
        
        load(url);
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "=== Loading completed in:" << elapsed << "ms";
    }
    
private slots:
    void onObjectCreated(QObject *object, const QUrl &url) {
        if (object) {
            qDebug() << "✓ Object created successfully"
                     << "\n  URL:" << url
                     << "\n  Type:" << object->metaObject()->className()
                     << "\n  ObjectName:" << object->objectName();
        } else {
            qWarning() << "✗ Failed to create object from:" << url;
        }
    }
    
    void onWarnings(const QList<QQmlError> &warnings) {
        qWarning() << "⚠ QML Warnings:";
        for (const QQmlError &warning : warnings) {
            qWarning() << "  " << warning.toString();
        }
    }
};

// ============================================================================
// Example 4: Multiple Contexts - Parent/Child Hierarchy
// ============================================================================

void demonstrateContextHierarchy(QQmlApplicationEngine *engine) {
    qDebug() << "\n=== Context Hierarchy Demo ===";
    
    // Root context - global properties
    QQmlContext *rootCtx = engine->rootContext();
    rootCtx->setContextProperty("globalValue", "I'm global");
    rootCtx->setContextProperty("version", "1.0.0");
    
    // Child context - component-specific properties
    QQmlContext *childCtx = new QQmlContext(rootCtx, engine);
    childCtx->setContextProperty("localValue", "I'm local");
    childCtx->setContextProperty("globalValue", "I override global"); // Shadows parent
    
    qDebug() << "Root context properties:" 
             << rootCtx->contextProperty("globalValue");
    qDebug() << "Child context properties:"
             << childCtx->contextProperty("globalValue")  // Overridden
             << childCtx->contextProperty("version");     // Inherited
    
    // Trong QML, property lookup sẽ:
    // 1. Tìm trong current context (childCtx)
    // 2. Nếu không có, tìm trong parent context (rootCtx)
    // 3. Tiếp tục lên root context
}

// ============================================================================
// Example 5: Performance Measurement - Context Property Updates
// ============================================================================

void measureContextPropertyPerformance() {
    qDebug() << "\n=== Performance Measurement ===";
    
    QGuiApplication app(0, nullptr);
    QQmlApplicationEngine engine;
    
    const int iterations = 10000;
    QElapsedTimer timer;
    
    // Test 1: Set context property nhiều lần
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        engine.rootContext()->setContextProperty("testValue", i);
    }
    qint64 elapsed1 = timer.nsecsElapsed();
    qDebug() << "setContextProperty x" << iterations << ":"
             << elapsed1 / 1000.0 << "µs total,"
             << elapsed1 / iterations / 1000.0 << "µs per call";
    
    // Test 2: Context property với QObject (better performance)
    DataModel model;
    engine.rootContext()->setContextProperty("model", &model);
    
    timer.restart();
    for (int i = 0; i < iterations; ++i) {
        model.setCount(i);  // Direct property change
    }
    qint64 elapsed2 = timer.nsecsElapsed();
    qDebug() << "QObject property change x" << iterations << ":"
             << elapsed2 / 1000.0 << "µs total,"
             << elapsed2 / iterations / 1000.0 << "µs per call";
    
    qDebug() << "→ QObject approach is" 
             << (double)elapsed1 / elapsed2 << "x faster";
}

// ============================================================================
// Example 6: Event Filter - Intercept và Modify Events
// ============================================================================

class EventLogger : public QObject {
    Q_OBJECT
    
public:
    explicit EventLogger(QObject *parent = nullptr) : QObject(parent) {}
    
protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        // Filter mouse events
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            qDebug() << "Mouse press at:" << mouseEvent->pos()
                     << "on object:" << watched->objectName();
            
            // Return true to filter (không cho event đi tiếp)
            // Return false to cho phép event propagate
            return false;
        }
        
        // Filter keyboard events
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            qDebug() << "Key pressed:" << keyEvent->text()
                     << "Key:" << keyEvent->key();
            
            // Example: Block Escape key
            if (keyEvent->key() == Qt::Key_Escape) {
                qDebug() << "  → Escape key blocked!";
                return true;  // Filter event
            }
        }
        
        return QObject::eventFilter(watched, event);
    }
};

// ============================================================================
// Example 7: QML Component Dynamic Loading
// ============================================================================

class DynamicLoader : public QObject {
    Q_OBJECT
    
public:
    explicit DynamicLoader(QQmlApplicationEngine *engine, QObject *parent = nullptr)
        : QObject(parent)
        , m_engine(engine)
    {
    }
    
    Q_INVOKABLE void loadComponent(const QString &qmlFile) {
        qDebug() << "Loading component dynamically:" << qmlFile;
        
        QQmlComponent component(m_engine, QUrl(qmlFile));
        
        // Check for errors
        if (component.isError()) {
            qWarning() << "Component has errors:";
            for (const QQmlError &error : component.errors()) {
                qWarning() << "  " << error.toString();
            }
            return;
        }
        
        // Create instance
        QObject *object = component.create();
        if (object) {
            qDebug() << "✓ Component created:" << object->metaObject()->className();
            m_dynamicObjects.append(object);
        } else {
            qWarning() << "✗ Failed to create component";
        }
    }
    
    Q_INVOKABLE void cleanupDynamicObjects() {
        qDebug() << "Cleaning up" << m_dynamicObjects.size() << "dynamic objects";
        qDeleteAll(m_dynamicObjects);
        m_dynamicObjects.clear();
    }
    
private:
    QQmlApplicationEngine *m_engine;
    QList<QObject*> m_dynamicObjects;
};

// ============================================================================
// Example 8: Main Function - Tích Hợp Tất Cả
// ============================================================================

int main(int argc, char *argv[])
{
    qDebug() << "=== Qt Core Classes Deep Dive Examples ===\n";
    
    // Use custom application
    DebugApplication app(argc, argv);
    
    // Performance test (không cần UI)
    if (argc > 1 && QString(argv[1]) == "--perf") {
        measureContextPropertyPerformance();
        return 0;
    }
    
    // Create instrumented engine
    InstrumentedEngine engine;
    
    // Setup context properties
    qDebug() << "\n1. Setting up context properties...";
    
    // Simple value
    engine.rootContext()->setContextProperty("appVersion", "1.0.0");
    engine.rootContext()->setContextProperty("buildDate", __DATE__);
    
    // QObject with properties
    DataModel dataModel;
    engine.rootContext()->setContextProperty("dataModel", &dataModel);
    
    // Dynamic loader
    DynamicLoader loader(&engine);
    engine.rootContext()->setContextProperty("dynamicLoader", &loader);
    
    // Demonstrate context hierarchy
    demonstrateContextHierarchy(&engine);
    
    // Install event filter
    qDebug() << "\n2. Installing event filter...";
    EventLogger eventLogger;
    app.installEventFilter(&eventLogger);
    
    // Load QML with timing
    qDebug() << "\n3. Loading QML...";
    engine.loadWithTiming(QUrl(QStringLiteral("qrc:/main.qml")));
    
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "No root objects - exiting";
        return -1;
    }
    
    // Update data model after 2 seconds
    QTimer::singleShot(2000, [&]() {
        qDebug() << "\n=== Updating data model ===";
        dataModel.setCount(42);
        dataModel.setMessage("Updated from C++");
    });
    
    qDebug() << "\n4. Entering event loop...\n";
    return app.exec();
}

// ============================================================================
// Example QML File (main.qml) để dùng với code trên
// ============================================================================

/*
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    visible: true
    width: 640
    height: 480
    title: "Qt Core Classes Deep Dive - Example App"
    
    Column {
        anchors.centerIn: parent
        spacing: 20
        
        // Context properties từ C++
        Text {
            text: "App Version: " + appVersion
            font.pixelSize: 16
        }
        
        Text {
            text: "Build Date: " + buildDate
            font.pixelSize: 16
        }
        
        // QObject properties với binding
        Text {
            text: "Data Model Count: " + dataModel.count
            font.pixelSize: 20
            font.bold: true
            color: "blue"
        }
        
        Text {
            text: "Message: " + dataModel.message
            font.pixelSize: 16
        }
        
        // Button để test invokable methods
        Button {
            text: "Increment Count"
            onClicked: {
                dataModel.increment()
                console.log("Count incremented from QML")
            }
        }
        
        Button {
            text: "Change Message"
            onClicked: {
                dataModel.message = "Updated from QML at " + new Date().toLocaleTimeString()
            }
        }
        
        // Test dynamic loading
        Button {
            text: "Load Dynamic Component"
            onClicked: {
                dynamicLoader.loadComponent("qrc:/DynamicComponent.qml")
            }
        }
        
        // Test formatMessage invokable
        Text {
            text: dataModel.formatMessage("Formatted")
            font.pixelSize: 14
            color: "green"
        }
    }
    
    // Component để test event handling
    MouseArea {
        anchors.fill: parent
        z: -1  // Behind other items
        onPressed: {
            console.log("Window MouseArea pressed at:", mouse.x, mouse.y)
        }
    }
    
    // Keyboard event handling
    focus: true
    Keys.onPressed: {
        console.log("Key pressed in QML:", event.key, event.text)
        if (event.key === Qt.Key_Space) {
            dataModel.increment()
            event.accepted = true
        }
    }
}
*/

#include "Code-Examples.moc"
