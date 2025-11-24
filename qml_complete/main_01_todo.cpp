/**
 * ============================================
 * EXAMPLE 1: TODO LIST APPLICATION
 * ============================================
 * 
 * C++ Model: TodoListModel (QAbstractListModel)
 * QML View: TodoListView.qml
 * 
 * PURPOSE:
 * Demonstrate how to integrate a C++ LIST MODEL with QML
 * 
 * KEY INTEGRATION STEPS:
 * 1. Create QML engine
 * 2. Create C++ model instance
 * 3. Expose model to QML via setContextProperty
 * 4. Load QML file
 * 5. QML automatically connects to C++ model
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "models/TodoListModel.h"

int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // ============================================
    // STEP 1: Create QML Engine
    // ============================================
    QQmlApplicationEngine engine;
    
    // ============================================
    // STEP 2: Create C++ Model
    // ============================================
    TodoListModel todoModel;
    
    // Add some sample data
    todoModel.addTodo("Learn Qt Models", 1);  // High priority
    todoModel.addTodo("Create QML Views", 2);  // Medium priority
    todoModel.addTodo("Write documentation", 3);  // Low priority
    todoModel.addTodo("Test application", 2);
    
    // ============================================
    // STEP 3: Expose model to QML
    // 
    // KEY CONCEPT:
    // - setContextProperty makes C++ object available in QML
    // - "todoModel" is the name used in QML
    // - &todoModel is the C++ object pointer
    // ============================================
    engine.rootContext()->setContextProperty("todoModel", &todoModel);
    
    // ============================================
    // STEP 4: Load QML file
    // ============================================
    const QUrl url(QStringLiteral("qrc:/qml/TodoListView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    // ============================================
    // STEP 5: Run application
    // ============================================
    return app.exec();
}

/**
 * ============================================
 * 📚 SUMMARY - TODO LIST INTEGRATION
 * ============================================
 * 
 * 1. C++ → QML DATA FLOW:
 *    TodoListModel (C++)
 *         ↓ setContextProperty
 *    QML Context
 *         ↓ property binding
 *    ListView.model = todoModel
 *         ↓ data() calls
 *    Display in QML UI
 * 
 * 2. QML → C++ METHOD CALLS:
 *    Button.onClicked in QML
 *         ↓ Q_INVOKABLE
 *    todoModel.addTodo() (C++ method)
 *         ↓ QList modification
 *    emit dataChanged()
 *         ↓ signal propagation
 *    QML ListView auto-updates
 * 
 * 3. KEY COMPONENTS:
 *    ✅ QGuiApplication - GUI app lifecycle
 *    ✅ QQmlApplicationEngine - QML runtime
 *    ✅ QQmlContext - C++/QML bridge
 *    ✅ setContextProperty - Expose C++ to QML
 *    ✅ qrc: - Qt Resource System
 * 
 * 4. MODEL ROLES:
 *    C++ (roleNames):
 *      TextRole → "text"
 *      CompletedRole → "completed"
 *      PriorityRole → "priority"
 *    
 *    QML (access):
 *      model.text
 *      model.completed
 *      model.priority
 * 
 * 5. WHY setContextProperty?
 *    ✅ Simple for single objects
 *    ✅ Available globally in QML
 *    ✅ No need for import statements
 *    
 *    Alternative: qmlRegisterType()
 *      → For creating multiple instances in QML
 *      → Requires import statement
 * 
 * 6. QML FILE LOADING:
 *    qrc:/qml/TodoListView.qml
 *      ↑    ↑         ↑
 *      |    |         └─ QML filename
 *      |    └─────────── Directory in qml.qrc
 *      └──────────────── Qt Resource System prefix
 * 
 * 7. ERROR HANDLING:
 *    objectCreated signal checks if QML loaded successfully
 *    If fail → exit with error code -1
 * 
 * ============================================
 * 💡 LEARNING POINTS
 * ============================================
 * 
 * ★ Expose C++ object to QML:
 *   engine.rootContext()->setContextProperty("name", &object);
 * 
 * ★ Model MUST inherit QAbstractListModel
 * ★ Model MUST override: rowCount(), data(), roleNames()
 * ★ Model MUST emit signals: dataChanged(), rowsInserted(), etc.
 * ★ Methods called from QML MUST be Q_INVOKABLE or slots
 * ★ Properties accessed from QML MUST be Q_PROPERTY
 * 
 * ============================================
 */
