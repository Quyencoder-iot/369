#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QStyle>
#include <QToolButton>
#include <QWaylandLayerShellV1>
#include <QWaylandLayerSurfaceV1>

/**
 * Ví dụ: Bottom Dock (Launcher)
 * 
 * Tạo một dock ở dưới cùng màn hình với các ứng dụng
 */
class BottomDock : public QWidget {
    Q_OBJECT
    
public:
    BottomDock(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Layout ngang
        auto *layout = new QHBoxLayout(this);
        layout->setSpacing(8);
        layout->setContentsMargins(15, 8, 15, 8);
        
        // Thêm các ứng dụng vào dock
        QStringList apps = {
            "Terminal", "Browser", "Editor", "Settings", "Files"
        };
        
        for (const QString &appName : apps) {
            auto *button = createAppButton(appName);
            layout->addWidget(button);
        }
        
        layout->addStretch();
        
        // Nút menu
        auto *menuBtn = new QPushButton("☰", this);
        menuBtn->setFixedSize(50, 50);
        menuBtn->setStyleSheet(getButtonStyle());
        connect(menuBtn, &QPushButton::clicked, this, &BottomDock::showMenu);
        layout->addWidget(menuBtn);
        
        // Thiết lập style cho dock
        setStyleSheet(
            "QWidget {"
            "  background: rgba(40, 40, 40, 220);"
            "  border-radius: 15px;"
            "}"
        );
        
        setupLayerShell();
    }
    
private slots:
    void showMenu() {
        qDebug() << "Menu clicked";
        // Implement menu logic here
    }
    
    void appClicked(const QString &appName) {
        qDebug() << "Launching:" << appName;
        // Implement app launcher logic here
    }
    
private:
    QPushButton* createAppButton(const QString &appName) {
        auto *button = new QPushButton(appName, this);
        button->setFixedSize(50, 50);
        button->setStyleSheet(getButtonStyle());
        
        connect(button, &QPushButton::clicked, 
                this, [this, appName]() { appClicked(appName); });
        
        return button;
    }
    
    QString getButtonStyle() {
        return QString(
            "QPushButton {"
            "  background: rgba(60, 60, 60, 200);"
            "  color: white;"
            "  border-radius: 10px;"
            "  font-size: 12px;"
            "}"
            "QPushButton:hover {"
            "  background: rgba(80, 80, 80, 255);"
            "}"
            "QPushButton:pressed {"
            "  background: rgba(100, 100, 100, 255);"
            "}"
        );
    }
    
    void setupLayerShell() {
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) {
            qWarning() << "LayerShell not available";
            return;
        }
        
        QWindow *window = windowHandle();
        if (!window) {
            qWarning() << "Window handle not available";
            return;
        }
        
        layerSurface = layerShell->getLayerSurface(window);
        if (!layerSurface) {
            qWarning() << "Failed to create layer surface";
            return;
        }
        
        // Bottom layer
        layerSurface->setLayer(QWaylandLayerShellV1::BottomLayer);
        
        // Neo vào cạnh dưới
        layerSurface->setAnchor(Qt::BottomEdge | Qt::LeftEdge | Qt::RightEdge);
        
        // Chiếm 66px chiều cao (50px button + 16px padding)
        layerSurface->setExclusiveZone(66);
        
        // Có thể nhận keyboard input khi focus
        layerSurface->setKeyboardInteractivity(
            QWaylandLayerShellV1::OnDemand
        );
        
        // Margin từ cạnh dưới
        layerSurface->setMargin(0, 0, 10, 0);
        
        layerSurface->commit();
        
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, &BottomDock::onConfigure);
        connect(layerSurface, &QWaylandLayerSurfaceV1::closed,
                this, &QWidget::close);
        
        // Thiết lập kích thước ban đầu
        resize(1920, 66);
    }
    
    void onConfigure(const QSize &size, uint32_t serial) {
        // Giữ chiều cao cố định, chỉ thay đổi chiều rộng
        resize(size.width(), 66);
        
        if (layerSurface) {
            layerSurface->ackConfigure(serial);
        }
    }
    
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qputenv("QT_QPA_PLATFORM", "wayland");
    
    BottomDock dock;
    dock.show();
    
    return app.exec();
}

#include "bottom_dock.moc"
