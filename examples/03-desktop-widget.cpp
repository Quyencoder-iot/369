/**
 * Example 3: Desktop Widget
 * 
 * A desktop widget that shows system information
 * Appears in bottom-right corner, below normal windows
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class DesktopWidget : public QWidget {
    Q_OBJECT
public:
    DesktopWidget() {
        setFixedSize(250, 200);
        setAttribute(Qt::WA_TranslucentBackground);
        
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(30, 30, 30, 0.85);
                border: 1px solid rgba(255, 255, 255, 0.2);
                border-radius: 12px;
                color: white;
                font-family: monospace;
            }
            QLabel {
                padding: 5px;
            }
            QLabel#title {
                font-size: 16px;
                font-weight: bold;
                color: #4fc3f7;
            }
            QLabel#time {
                font-size: 24px;
                font-weight: bold;
            }
        )");
        
        // Layout
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(15, 15, 15, 15);
        layout->setSpacing(10);
        
        // Title
        QLabel *title = new QLabel("System Monitor");
        title->setObjectName("title");
        title->setAlignment(Qt::AlignCenter);
        layout->addWidget(title);
        
        // Time
        timeLabel = new QLabel();
        timeLabel->setObjectName("time");
        timeLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(timeLabel);
        
        // System info
        cpuLabel = new QLabel("CPU: --");
        memLabel = new QLabel("RAM: --");
        
        layout->addWidget(cpuLabel);
        layout->addWidget(memLabel);
        
        layout->addStretch();
        
        // Update timer
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &DesktopWidget::updateInfo);
        timer->start(2000);
        updateInfo();
        
        // Configure layer shell
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        // Bottom layer (below normal windows)
        layerWindow->setLayer(LayerShellQt::Window::LayerBottom);
        
        // Bottom-right corner
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorRight
        );
        
        // No exclusive zone
        layerWindow->setExclusiveZone(0);
        
        // Margins: 20px from bottom and right
        layerWindow->setMargins(QMargins(0, 0, 20, 20));
        
        // No keyboard
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
    }
    
    void updateInfo() {
        // Update time
        timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
        
        // Simple CPU load (Linux)
        QFile loadFile("/proc/loadavg");
        if (loadFile.open(QIODevice::ReadOnly)) {
            QString load = QString(loadFile.readAll()).split(' ')[0];
            cpuLabel->setText("CPU: " + load);
            loadFile.close();
        }
        
        // Memory usage (Linux)
        QFile memFile("/proc/meminfo");
        if (memFile.open(QIODevice::ReadOnly)) {
            QString content = memFile.readAll();
            QStringList lines = content.split('\n');
            
            auto extractValue = [&](const QString &line) -> int {
                return line.split(QRegExp("\\s+"))[1].toInt();
            };
            
            int total = extractValue(lines[0]);  // MemTotal
            int available = extractValue(lines[2]);  // MemAvailable
            int used = total - available;
            int percent = (used * 100) / total;
            
            memLabel->setText(QString("RAM: %1% (%2 MB)")
                .arg(percent)
                .arg(used / 1024));
            
            memFile.close();
        }
    }
    
    QLabel *timeLabel;
    QLabel *cpuLabel;
    QLabel *memLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    
    DesktopWidget widget;
    widget.show();
    
    return app.exec();
}

#include "03-desktop-widget.moc"
