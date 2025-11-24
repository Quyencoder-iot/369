/**
 * main_todo.cpp - Todo List với QML View + C++ Model
 * 
 * Kết nối:
 * - C++ TodoListModel
 * - QML TodoListView
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
    
    // Create C++ model
    TodoListModel todoModel;
    
    // Setup QML engine
    QQmlApplicationEngine engine;
    
    // Expose C++ model to QML
    engine.rootContext()->setContextProperty("todoModel", &todoModel);
    
    // Load QML file
    const QUrl url(QStringLiteral("qrc:/qml/TodoListView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}
