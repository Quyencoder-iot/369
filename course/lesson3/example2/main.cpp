#include <QApplication>
#include "dashboard/dashboardwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("Vehicle Dashboard");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("AUTOSAR Adaptive Course");
    
    DashboardWindow window;
    window.show();
    
    return app.exec();
}
