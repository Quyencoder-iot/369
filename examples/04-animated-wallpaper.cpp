/**
 * Example 4: Animated Wallpaper
 * 
 * An animated wallpaper with moving gradient circles
 * Uses background layer
 */

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>
#include <cmath>

class AnimatedWallpaper : public QWidget {
    Q_OBJECT
public:
    AnimatedWallpaper() {
        setAttribute(Qt::WA_OpaquePaintEvent);
        
        // Animate at 60 FPS
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
        timer->start(16);  // ~60 FPS
        
        // Configure layer shell
        setupLayerShell();
    }
    
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Gradient background
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0.0, QColor(15, 32, 39));    // Dark blue-grey
        gradient.setColorAt(0.5, QColor(32, 43, 51));    // Mid grey
        gradient.setColorAt(1.0, QColor(44, 62, 80));    // Blue-grey
        painter.fillRect(rect(), gradient);
        
        // Get current time for animation
        double time = QDateTime::currentMSecsSinceEpoch() / 1000.0;
        
        // Draw animated circles
        const int numCircles = 8;
        for (int i = 0; i < numCircles; i++) {
            double angle = time * 0.3 + i * (2.0 * M_PI / numCircles);
            double radius = 300 + 100 * std::sin(time * 0.5 + i * 0.5);
            
            double x = width() / 2 + radius * std::cos(angle);
            double y = height() / 2 + radius * std::sin(angle);
            double size = 80 + 40 * std::sin(time + i);
            
            // Color with hue rotation
            int hue = int(time * 20 + i * 45) % 360;
            QColor color;
            color.setHsv(hue, 150, 80, 60);  // Semi-transparent
            
            painter.setBrush(color);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(x, y), size, size);
        }
        
        // Draw center glow
        QRadialGradient centerGlow(width() / 2, height() / 2, 
                                   400 + 100 * std::sin(time * 0.5));
        centerGlow.setColorAt(0.0, QColor(255, 255, 255, 30));
        centerGlow.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter.setBrush(centerGlow);
        painter.drawRect(rect());
        
        // Optional: Draw time
        painter.setPen(QColor(255, 255, 255, 100));
        painter.setFont(QFont("sans-serif", 48, QFont::Light));
        painter.drawText(rect(), Qt::AlignCenter, 
            QTime::currentTime().toString("hh:mm"));
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        // Background layer (lowest)
        layerWindow->setLayer(LayerShellQt::Window::LayerBackground);
        
        // Full screen
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        
        // No exclusive zone
        layerWindow->setExclusiveZone(0);
        
        // No keyboard
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    
    AnimatedWallpaper wallpaper;
    wallpaper.showFullScreen();
    
    return app.exec();
}

#include "04-animated-wallpaper.moc"
