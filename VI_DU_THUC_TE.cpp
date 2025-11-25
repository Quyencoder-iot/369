/*
 * Ví Dụ Thực Tế: QGuiApplication, QQmlApplicationEngine, QQmlContext
 * 
 * File này minh họa cách các lớp Qt hoạt động cùng nhau
 * Sử dụng để trace và hiểu execution flow
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QTimer>

// Custom C++ object để expose vào QML
class MyDataObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit MyDataObject(QObject *parent = nullptr)
        : QObject(parent), m_name("Default"), m_value(0)
    {
        qDebug() << "MyDataObject::MyDataObject() - Constructor called";
    }

    QString name() const { return m_name; }
    void setName(const QString &name) {
        if (m_name != name) {
            m_name = name;
            emit nameChanged();
            qDebug() << "MyDataObject::setName() - Name changed to:" << name;
        }
    }

    int value() const { return m_value; }
    void setValue(int value) {
        if (m_value != value) {
            m_value = value;
            emit valueChanged();
            qDebug() << "MyDataObject::setValue() - Value changed to:" << value;
        }
    }

signals:
    void nameChanged();
    void valueChanged();

private:
    QString m_name;
    int m_value;
};

// Event filter để monitor events
class EventMonitor : public QObject
{
public:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        // Log important events
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::KeyPress ||
            event->type() == QEvent::Paint) {
            qDebug() << "EventMonitor::eventFilter() - Event type:" 
                     << event->type() << "for object:" << obj;
        }
        return QObject::eventFilter(obj, event);
    }
};

int main(int argc, char *argv[])
{
    qDebug() << "=== Application Start ===";
    
    // ============================================
    // BƯỚC 1: Tạo QGuiApplication
    // ============================================
    qDebug() << "\n--- Step 1: Creating QGuiApplication ---";
    QGuiApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Qt Source Code Learning");
    app.setOrganizationName("Learning");
    
    qDebug() << "QGuiApplication created";
    qDebug() << "Platform name:" << app.platformName();
    qDebug() << "Application name:" << app.applicationName();
    
    // Install event filter để monitor events
    EventMonitor *monitor = new EventMonitor();
    app.installEventFilter(monitor);
    qDebug() << "Event filter installed";
    
    // ============================================
    // BƯỚC 2: Tạo QQmlApplicationEngine
    // ============================================
    qDebug() << "\n--- Step 2: Creating QQmlApplicationEngine ---";
    QQmlApplicationEngine engine;
    
    qDebug() << "QQmlApplicationEngine created";
    qDebug() << "Engine root context:" << engine.rootContext();
    
    // ============================================
    // BƯỚC 3: Setup QQmlContext và Expose Objects
    // ============================================
    qDebug() << "\n--- Step 3: Setting up QQmlContext ---";
    
    // Lấy root context
    QQmlContext *rootContext = engine.rootContext();
    qDebug() << "Root context obtained:" << rootContext;
    qDebug() << "Root context parent:" << rootContext->parentContext();
    qDebug() << "Root context base URL:" << rootContext->baseUrl();
    
    // Tạo và expose C++ object vào QML
    MyDataObject *myData = new MyDataObject();
    rootContext->setContextProperty("myData", myData);
    qDebug() << "Context property 'myData' set";
    
    // Expose application object
    rootContext->setContextProperty("app", &app);
    qDebug() << "Context property 'app' set";
    
    // Expose một số constants
    rootContext->setContextProperty("APP_VERSION", QString("1.0.0"));
    qDebug() << "Context property 'APP_VERSION' set";
    
    // ============================================
    // BƯỚC 4: Load QML File
    // ============================================
    qDebug() << "\n--- Step 4: Loading QML file ---";
    
    // Monitor object creation
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     [](QObject *obj, const QUrl &url) {
                         qDebug() << "QQmlApplicationEngine::objectCreated() - "
                                  << "Object:" << obj 
                                  << "from URL:" << url;
                         if (!obj) {
                             qDebug() << "ERROR: Failed to create object from" << url;
                         }
                     });
    
    // Load QML file
    // Option 1: Load from file system
    engine.load(QUrl::fromLocalFile("main.qml"));
    
    // Option 2: Load from resource (if using qrc)
    // engine.load(QUrl("qrc:/main.qml"));
    
    // Option 3: Load from URL
    // engine.load(QUrl("http://example.com/app.qml"));
    
    qDebug() << "QML file load requested";
    
    // ============================================
    // BƯỚC 5: Start Event Loop
    // ============================================
    qDebug() << "\n--- Step 5: Starting event loop ---";
    qDebug() << "Calling QGuiApplication::exec()";
    qDebug() << "Event loop will process events until quit() is called";
    
    // Tạo timer để demo event processing
    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [&app, myData, timer]() {
        static int count = 0;
        count++;
        qDebug() << "Timer fired - Count:" << count;
        
        // Update exposed object
        myData->setValue(count);
        
        // Quit after 5 iterations
        if (count >= 5) {
            qDebug() << "Quitting application...";
            timer->stop();
            app.quit();
        }
    });
    timer->start(1000); // Fire every second
    
    // Start event loop
    int result = app.exec();
    
    qDebug() << "\n=== Application End ===";
    qDebug() << "Event loop exited with code:" << result;
    
    // Cleanup
    delete myData;
    delete monitor;
    delete timer;
    
    return result;
}

// Include MOC file for Q_OBJECT macro
#include "VI_DU_THUC_TE.moc"
