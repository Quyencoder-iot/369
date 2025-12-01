#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QWaylandLayerShellV1>
#include <QWaylandLayerSurfaceV1>

/**
 * Ví dụ: Notification Overlay
 * 
 * Hiển thị thông báo ở góc trên bên phải màn hình
 * với hiệu ứng fade in/out
 */
class NotificationWindow : public QWidget {
    Q_OBJECT
    
public:
    NotificationWindow(const QString &title, const QString &message, 
                     QWidget *parent = nullptr)
        : QWidget(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Layout dọc
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        
        // Container với background
        auto *container = new QWidget(this);
        container->setStyleSheet(
            "QWidget {"
            "  background: rgba(30, 30, 30, 240);"
            "  border-radius: 10px;"
            "  border: 1px solid rgba(255, 255, 255, 50);"
            "}"
        );
        
        auto *containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(15, 15, 15, 15);
        containerLayout->setSpacing(8);
        
        // Title
        auto *titleLabel = new QLabel(title, container);
        titleLabel->setStyleSheet(
            "QLabel {"
            "  color: white;"
            "  font-size: 16px;"
            "  font-weight: bold;"
            "}"
        );
        containerLayout->addWidget(titleLabel);
        
        // Message
        auto *messageLabel = new QLabel(message, container);
        messageLabel->setStyleSheet(
            "QLabel {"
            "  color: rgba(255, 255, 255, 200);"
            "  font-size: 14px;"
            "}"
        );
        messageLabel->setWordWrap(true);
        containerLayout->addWidget(messageLabel);
        
        layout->addWidget(container);
        
        // Thiết lập kích thước
        setFixedSize(350, 120);
        
        // Thiết lập LayerShell
        setupLayerShell();
        
        // Hiệu ứng fade in
        fadeIn();
        
        // Auto-close sau 5 giây
        QTimer::singleShot(5000, this, [this]() {
            fadeOut();
        });
    }
    
private:
    void setupLayerShell() {
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) return;
        
        QWindow *window = windowHandle();
        if (!window) return;
        
        layerSurface = layerShell->getLayerSurface(window);
        if (!layerSurface) return;
        
        // Overlay layer ở trên cùng
        layerSurface->setLayer(QWaylandLayerShellV1::OverlayLayer);
        
        // Neo vào góc trên bên phải
        layerSurface->setAnchor(Qt::TopEdge | Qt::RightEdge);
        
        // Không chiếm exclusive zone
        layerSurface->setExclusiveZone(0);
        
        // Không nhận keyboard input
        layerSurface->setKeyboardInteractivity(
            QWaylandLayerShellV1::None
        );
        
        // Margin từ cạnh (20px từ trên và phải)
        layerSurface->setMargin(20, 20, 0, 0);
        
        layerSurface->commit();
        
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, [this](const QSize &size, uint32_t serial) {
                    // Giữ nguyên kích thước
                    layerSurface->ackConfigure(serial);
                });
    }
    
    void fadeIn() {
        auto *effect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(effect);
        
        auto *animation = new QPropertyAnimation(effect, "opacity", this);
        animation->setDuration(300);
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    void fadeOut() {
        auto *effect = graphicsEffect();
        if (!effect) {
            effect = new QGraphicsOpacityEffect(this);
            setGraphicsEffect(effect);
        }
        
        auto *animation = new QPropertyAnimation(effect, "opacity", this);
        animation->setDuration(300);
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);
        connect(animation, &QPropertyAnimation::finished, this, &QWidget::close);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qputenv("QT_QPA_PLATFORM", "wayland");
    
    // Tạo một số notification mẫu
    auto *notif1 = new NotificationWindow(
        "System Update",
        "New updates are available. Click to install."
    );
    notif1->show();
    
    QTimer::singleShot(2000, [&app]() {
        auto *notif2 = new NotificationWindow(
            "Battery Low",
            "Battery level is below 20%. Please plug in your charger."
        );
        notif2->show();
    });
    
    return app.exec();
}

#include "notification.moc"
