// main.cpp - Entry point cho Qt Model-View-Delegate Pointer Demo

#include <QApplication>
#include <QDebug>
#include "mainwindow.h"

// Hàm helper để in thông tin về pointer management
void printPointerManagementIntro() {
    qDebug() << "================================================";
    qDebug() << "Qt Model-View-Delegate Pointer Management Demo";
    qDebug() << "================================================";
    qDebug() << "\nHướng dẫn sử dụng:";
    qDebug() << "1. Khám phá các tab khác nhau để học về:";
    qDebug() << "   - Model Pointers: Cách quản lý model và items";
    qDebug() << "   - View Pointers: Multiple views, selection models";
    qDebug() << "   - Delegate Pointers: Custom delegates và editors";
    qDebug() << "   - Memory Management: Best practices và pitfalls";
    qDebug() << "\n2. Sử dụng các nút để test các scenarios khác nhau";
    qDebug() << "3. Xem console output để hiểu lifecycle của objects";
    qDebug() << "================================================\n";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("Qt MVC Pointer Tutorial");
    app.setApplicationDisplayName("Qt Model-View-Delegate Con Trỏ Demo");
    app.setOrganizationName("Qt Tutorial");
    
    // Print introduction
    printPointerManagementIntro();
    
    // Create and show main window
    MainWindow window;
    window.setWindowTitle("Qt Model-View-Delegate - Quản Lý Con Trỏ");
    window.resize(1200, 800);
    window.show();
    
    // Connect để print info khi app shutdown
    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        qDebug() << "\n=== Application Shutting Down ===";
        qDebug() << "All Qt objects with parents will be automatically deleted";
        qDebug() << "Smart pointers will clean up their resources";
        qDebug() << "Check for any memory leaks or dangling pointers!";
    });
    
    return app.exec();
}