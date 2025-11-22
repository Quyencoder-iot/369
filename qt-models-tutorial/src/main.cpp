#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("Qt Models Tutorial");
    app.setOrganizationName("Qt Learning");
    
    // Create and show main window
    MainWindow window;
    window.setWindowTitle("Qt Model/View Architecture Tutorial");
    window.resize(1200, 800);
    window.show();
    
    return app.exec();
}