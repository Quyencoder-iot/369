/**
 * Example 1: Simple Top Panel
 * 
 * A minimal top panel with a label and clock
 * Demonstrates basic LayerShellQt usage
 */

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class SimplePanel : public QWidget {
    Q_OBJECT
public:
    SimplePanel() {
        // Set fixed height
        setFixedHeight(30);
        
        // Style
        setStyleSheet(R"(
            QWidget {
                background-color: #2d2d2d;
                color: white;
                font-family: sans-serif;
            }
            QLabel {
                padding: 0 10px;
            }
        )");
        
        // Layout
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 0, 10, 0);
        layout->setSpacing(5);
        
        // Add title
        QLabel *title = new QLabel("My Panel");
        layout->addWidget(title);
        
        layout->addStretch();
        
        // Add clock
        clockLabel = new QLabel();
        updateClock();
        layout->addWidget(clockLabel);
        
        // Update clock every second
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SimplePanel::updateClock);
        timer->start(1000);
        
        // Configure layer shell
        setupLayerShell();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        // Top layer (above normal windows)
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // Anchor to top edge, span full width
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        
        // Reserve 30px from top (exclusive zone)
        layerWindow->setExclusiveZone(30);
        
        // Enable keyboard input (if you add interactive elements)
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityExclusive
        );
    }
    
    void updateClock() {
        clockLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
    }
    
    QLabel *clockLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Check if layer shell is available
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available! Are you running on a supported Wayland compositor?");
        return 1;
    }
    
    SimplePanel panel;
    panel.show();
    
    return app.exec();
}

#include "01-simple-panel.moc"
