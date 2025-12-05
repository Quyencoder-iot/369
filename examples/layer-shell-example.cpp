// Ví dụ ứng dụng Qt sử dụng Layer Shell protocol
// Compile với: g++ -o layer-shell-example layer-shell-example.cpp -lQt5WaylandClient -lQt5Core -fPIC

#include <QtWaylandClient/QWaylandClientExtension>
#include <QtWaylandClient/QWaylandWindow>
#include <QtWaylandClient/QtWaylandClientVersion>
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

// Layer Shell protocol interface
// Cần wayland-scanner để generate từ .xml protocol file

class LayerShellExample : public QWidget
{
    Q_OBJECT

public:
    LayerShellExample(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setWindowTitle("Layer Shell Example");
        setMinimumSize(400, 300);
        
        // Thiết lập window flags cho overlay
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        
        // Layout
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *title = new QLabel("Layer Shell Example", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold;");
        layout->addWidget(title);
        
        QLabel *info = new QLabel(
            "Đây là ví dụ về ứng dụng sử dụng Layer Shell protocol.\n"
            "Ứng dụng này có thể hiển thị như một overlay panel.",
            this
        );
        info->setWordWrap(true);
        layout->addWidget(info);
        
        QPushButton *closeBtn = new QPushButton("Đóng", this);
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
        layout->addWidget(closeBtn);
        
        // Style
        setStyleSheet(
            "QWidget {"
            "    background-color: #1e1e2e;"
            "    color: #cdd6f4;"
            "    border-radius: 10px;"
            "    padding: 20px;"
            "}"
            "QLabel {"
            "    color: #cdd6f4;"
            "}"
            "QPushButton {"
            "    background-color: #89b4fa;"
            "    color: #1e1e2e;"
            "    border: none;"
            "    padding: 10px;"
            "    border-radius: 5px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #74c7ec;"
            "}"
        );
    }

protected:
    void showEvent(QShowEvent *event) override
    {
        QWidget::showEvent(event);
        
        // Thiết lập layer shell properties khi window được tạo
        // Trong thực tế, cần sử dụng QWaylandLayerShellSurface
        qDebug() << "Window shown - có thể thiết lập layer shell properties";
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Thiết lập platform plugin cho Wayland
    qputenv("QT_QPA_PLATFORM", "wayland");
    
    LayerShellExample window;
    window.show();
    
    return app.exec();
}

#include "layer-shell-example.moc"
