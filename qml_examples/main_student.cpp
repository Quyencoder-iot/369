/**
 * main_student.cpp - Student Table với QML View + C++ Model
 * 
 * Kết nối:
 * - C++ StudentTableModel
 * - QML StudentTableView
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "models/StudentTableModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // Create C++ model
    StudentTableModel studentModel;
    
    // Setup QML engine
    QQmlApplicationEngine engine;
    
    // Expose model to QML
    engine.rootContext()->setContextProperty("studentModel", &studentModel);
    
    // Load QML
    const QUrl url(QStringLiteral("qrc:/qml/StudentTableView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}
