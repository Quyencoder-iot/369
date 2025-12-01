/**
 * Example 2: Notification Window
 * 
 * A notification that appears in the top-right corner
 * Auto-hides after 5 seconds
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class Notification : public QWidget {
    Q_OBJECT
public:
    Notification(const QString &title, const QString &message, int duration = 5000) {
        setFixedSize(350, 100);
        setAttribute(Qt::WA_TranslucentBackground);
        
        setStyleSheet(R"(
            QWidget {
                background-color: rgba(45, 45, 45, 0.95);
                border: 2px solid #555;
                border-radius: 10px;
                color: white;
                font-family: sans-serif;
            }
            QLabel#title {
                font-size: 14px;
                font-weight: bold;
                padding: 5px;
            }
            QLabel#message {
                font-size: 12px;
                color: #ccc;
                padding: 5px;
            }
        )");
        
        // Layout
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(15, 10, 15, 10);
        layout->setSpacing(5);
        
        // Title
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setObjectName("title");
        layout->addWidget(titleLabel);
        
        // Message
        QLabel *msgLabel = new QLabel(message);
        msgLabel->setObjectName("message");
        msgLabel->setWordWrap(true);
        layout->addWidget(msgLabel);
        
        // Configure layer shell
        setupLayerShell();
        
        // Auto-hide after duration
        QTimer::singleShot(duration, this, &Notification::fadeOut);
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        // Top layer
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // Top-right corner
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorRight
        );
        
        // No exclusive zone (overlay mode - apps can go under it)
        layerWindow->setExclusiveZone(0);
        
        // Margins: 10px from top and right
        layerWindow->setMargins(QMargins(0, 10, 10, 0));
        
        // No keyboard interaction
        layerWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
    }
    
    void fadeOut() {
        QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
        anim->setDuration(300);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, &QWidget::close);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    
    // Create and show notification
    Notification *notif = new Notification(
        "System Update",
        "Your system has been updated successfully. Restart to complete."
    );
    notif->setAttribute(Qt::WA_DeleteOnClose);
    notif->show();
    
    // Create another notification after 1 second
    QTimer::singleShot(1000, [&]() {
        Notification *notif2 = new Notification(
            "New Message",
            "You have 3 unread messages."
        );
        notif2->setAttribute(Qt::WA_DeleteOnClose);
        notif2->show();
    });
    
    return app.exec();
}

#include "02-notification.moc"
