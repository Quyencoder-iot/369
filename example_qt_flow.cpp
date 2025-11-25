/*
 * Ví dụ minh họa flow của QGuiApplication, QQmlApplicationEngine, QQmlContext
 * 
 * File này giúp bạn hiểu cách các lớp này tương tác với nhau
 * Khi đọc source code, trace theo flow này để hiểu rõ hơn
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

// Class để expose data vào QML
class AppData : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    
public:
    explicit AppData(QObject *parent = nullptr) : QObject(parent), m_message("Hello from C++!") {}
    
    QString message() const { return m_message; }
    void setMessage(const QString &msg) {
        if (m_message != msg) {
            m_message = msg;
            emit messageChanged();
        }
    }
    
signals:
    void messageChanged();
    
private:
    QString m_message;
};

int main(int argc, char *argv[])
{
    // ============================================
    // BƯỚC 1: QGuiApplication - Application Lifecycle
    // ============================================
    // Khi đọc qguiapplication.cpp, trace constructor này:
    // - Platform plugin loading
    // - Event dispatcher setup
    // - Application attributes
    QGuiApplication app(argc, argv);
    
    qDebug() << "=== QGuiApplication created ===";
    qDebug() << "Organization:" << app.organizationName();
    qDebug() << "Application Name:" << app.applicationName();
    
    // ============================================
    // BƯỚC 2: QQmlApplicationEngine - QML Engine
    // ============================================
    // Khi đọc qqmlapplicationengine.cpp, trace constructor:
    // - QQmlEngine initialization
    // - Root context creation
    QQmlApplicationEngine engine;
    
    qDebug() << "=== QQmlApplicationEngine created ===";
    
    // ============================================
    // BƯỚC 3: QQmlContext - Context Setup
    // ============================================
    // Khi đọc qmlcontext.cpp, hiểu cách setContextProperty hoạt động:
    // - Property storage
    // - Lookup mechanism
    // - Change notification
    
    // Tạo data object
    AppData appData;
    
    // Lấy root context từ engine
    // Trace: QQmlApplicationEngine::rootContext()
    //        -> QQmlEngine::rootContext()
    QQmlContext *rootContext = engine.rootContext();
    
    qDebug() << "=== Root Context obtained ===";
    qDebug() << "Context object:" << rootContext;
    
    // Set context property - đây là cách expose C++ data vào QML
    // Trace: QQmlContext::setContextProperty()
    //        -> QQmlContextData::setContextProperty()
    rootContext->setContextProperty("appData", &appData);
    
    qDebug() << "=== Context property set ===";
    qDebug() << "Property 'appData' available in QML";
    
    // ============================================
    // BƯỚC 4: Load QML - Component Creation
    // ============================================
    // Khi đọc qqmlapplicationengine.cpp::load(), trace:
    // - QUrl resolution
    // - QQmlComponent::loadUrl()
    // - Component compilation
    // - Object creation
    // - Root object setup
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    
    qDebug() << "=== Loading QML file ===";
    qDebug() << "URL:" << url;
    
    // Trace load() method:
    // QQmlApplicationEngine::load(url)
    //   -> QQmlComponent::loadUrl(url, QQmlEngine::qmlContext())
    //      -> Component compilation
    //      -> QQmlComponent::create()
    //         -> QML object tree created
    engine.load(url);
    
    // ============================================
    // BƯỚC 5: Check Root Objects
    // ============================================
    // Sau khi load, engine có root objects
    // Trace: QQmlApplicationEngine::rootObjects()
    QList<QObject*> rootObjects = engine.rootObjects();
    
    qDebug() << "=== Root objects created ===";
    qDebug() << "Number of root objects:" << rootObjects.size();
    
    if (rootObjects.isEmpty()) {
        qDebug() << "ERROR: No root objects! Check QML file.";
        return -1;
    }
    
    // ============================================
    // BƯỚC 6: Event Loop - Application Execution
    // ============================================
    // Khi đọc qcoreapplication.cpp::exec(), trace:
    // - QEventLoop::exec()
    // - Event processing
    // - Signal/slot delivery
    // - QML binding updates
    
    qDebug() << "=== Starting event loop ===";
    qDebug() << "Application will process events until quit() is called";
    
    // Event loop chạy ở đây
    // Mọi events, signals, QML bindings được xử lý trong loop này
    int exitCode = app.exec();
    
    qDebug() << "=== Event loop ended ===";
    qDebug() << "Exit code:" << exitCode;
    
    return exitCode;
}

/*
 * ============================================
 * TRACE EXECUTION FLOW - Đọc source theo thứ tự này:
 * ============================================
 * 
 * 1. QGuiApplication::QGuiApplication()
 *    File: qtbase/src/gui/kernel/qguiapplication.cpp
 *    - Gọi QCoreApplication constructor
 *    - Initialize platform integration
 *    - Setup event dispatcher
 * 
 * 2. QQmlApplicationEngine::QQmlApplicationEngine()
 *    File: qtdeclarative/src/qml/qml/qqmlapplicationengine.cpp
 *    - Gọi QQmlEngine constructor
 *    - Create root context
 * 
 * 3. QQmlEngine::rootContext()
 *    File: qtdeclarative/src/qml/qml/qqmlengine.cpp
 *    - Return hoặc create root context
 * 
 * 4. QQmlContext::setContextProperty()
 *    File: qtdeclarative/src/qml/qml/qmlcontext.cpp
 *    - Store property trong QQmlContextData
 *    - Setup change notification
 * 
 * 5. QQmlApplicationEngine::load()
 *    File: qtdeclarative/src/qml/qml/qqmlapplicationengine.cpp
 *    - Resolve QUrl
 *    - Create QQmlComponent
 *    - Load và compile QML
 *    - Create QML objects
 * 
 * 6. QCoreApplication::exec()
 *    File: qtbase/src/corelib/kernel/qcoreapplication.cpp
 *    - Create QEventLoop
 *    - Call QEventLoop::exec()
 *    - Process events until quit()
 * 
 * ============================================
 * QML SIDE - Khi QML code chạy:
 * ============================================
 * 
 * Trong QML file (main.qml):
 * 
 * import QtQuick 2.15
 * 
 * Item {
 *     // QML engine tìm "appData" trong context
 *     // Trace: QQmlContext::contextProperty("appData")
 *     //        -> QQmlContextData::property()
 *     //        -> Traverse parent contexts nếu không tìm thấy
 *     
 *     Text {
 *         text: appData.message  // Đọc từ context
 *     }
 *     
 *     // Khi appData.message thay đổi:
 *     // - Signal messageChanged() được emit
 *     // - QML binding được trigger
 *     // - Text.text được update
 * }
 */

#include "example_qt_flow.moc"
