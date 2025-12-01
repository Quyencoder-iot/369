/**
 * LayerShellQt Example: Notification Popup
 * 
 * Ví dụ tạo một notification popup với:
 * - Layer: Overlay (trên tất cả, kể cả fullscreen apps)
 * - Anchors: Top + Right (góc trên phải)
 * - Auto-dismiss sau vài giây
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QWindow>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class NotificationPopup : public QWidget {
    Q_OBJECT
    
public:
    NotificationPopup(const QString &appName, 
                      const QString &title, 
                      const QString &message,
                      int durationMs = 5000,
                      QWidget *parent = nullptr) 
        : QWidget(parent)
        , m_duration(durationMs)
    {
        setupUI(appName, title, message);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedWidth(360);
        
        // Auto dismiss sau duration
        if (m_duration > 0) {
            QTimer::singleShot(m_duration, this, &QWidget::close);
        }
    }

private:
    void setupUI(const QString &appName, 
                 const QString &title, 
                 const QString &message) {
        auto *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(16, 14, 16, 14);
        mainLayout->setSpacing(14);
        
        // Icon
        auto *iconLabel = new QLabel("🔔");
        iconLabel->setStyleSheet("font-size: 28px;");
        iconLabel->setAlignment(Qt::AlignTop);
        
        // Content
        auto *contentLayout = new QVBoxLayout();
        contentLayout->setSpacing(4);
        
        // App name
        auto *appLabel = new QLabel(appName);
        appLabel->setStyleSheet(R"(
            font-size: 11px;
            color: rgba(255, 255, 255, 0.5);
            text-transform: uppercase;
            letter-spacing: 1px;
        )");
        
        // Title
        auto *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet(R"(
            font-size: 14px;
            font-weight: bold;
            color: white;
        )");
        
        // Message
        auto *messageLabel = new QLabel(message);
        messageLabel->setStyleSheet(R"(
            font-size: 13px;
            color: rgba(255, 255, 255, 0.8);
        )");
        messageLabel->setWordWrap(true);
        
        contentLayout->addWidget(appLabel);
        contentLayout->addWidget(titleLabel);
        contentLayout->addWidget(messageLabel);
        
        // Close button
        auto *closeBtn = new QPushButton("✕");
        closeBtn->setFixedSize(24, 24);
        closeBtn->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                border: none;
                color: rgba(255, 255, 255, 0.5);
                font-size: 14px;
            }
            QPushButton:hover {
                color: white;
                background: rgba(255, 255, 255, 0.1);
                border-radius: 12px;
            }
        )");
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
        
        auto *closeLayout = new QVBoxLayout();
        closeLayout->addWidget(closeBtn);
        closeLayout->addStretch();
        
        mainLayout->addWidget(iconLabel);
        mainLayout->addLayout(contentLayout, 1);
        mainLayout->addLayout(closeLayout);
        
        // Notification style
        setStyleSheet(R"(
            NotificationPopup {
                background-color: rgba(35, 35, 45, 0.95);
                border-radius: 12px;
                border: 1px solid rgba(255, 255, 255, 0.1);
            }
        )");
    }
    
private:
    int m_duration;
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isLayerShellSupported()) {
        qWarning() << "❌ Layer Shell không được hỗ trợ!";
        return 1;
    }
    
    // Tạo notification
    NotificationPopup notification(
        "System",
        "LayerShellQt Notification",
        "Đây là một ví dụ notification sử dụng LayerShellQt. "
        "Notification này sẽ tự động biến mất sau 5 giây.",
        5000  // 5 giây
    );
    
    notification.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(notification.windowHandle())) {
        // OVERLAY layer: trên tất cả, kể cả fullscreen applications
        lsWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        
        // Neo vào góc trên phải
        lsWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorRight
        );
        
        // Không chiếm exclusive zone
        lsWindow->setExclusiveZone(0);
        
        // Margin từ góc
        lsWindow->setMargins(QMargins(0, 20, 20, 0));
        
        // Notification không cần keyboard
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
        
        qDebug() << "✅ Notification đã được cấu hình trên Overlay layer";
    }
    
    notification.show();
    
    // Tự động thoát sau khi notification đóng
    QObject::connect(&notification, &QWidget::destroyed, &app, &QApplication::quit);
    
    return app.exec();
}

#include "notification.moc"
