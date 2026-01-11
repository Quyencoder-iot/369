/**
 * @file main.cpp
 * @brief Main entry point cho Qt Dashboard Application
 */

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("AUTOSAR Dashboard");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("AUTOSAR Adaptive Course");
    
    qDebug() << "==============================================";
    qDebug() << "   AUTOSAR Adaptive - Qt Dashboard Client";
    qDebug() << "   Example 2: Advanced GUI Implementation";
    qDebug() << "==============================================";
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
