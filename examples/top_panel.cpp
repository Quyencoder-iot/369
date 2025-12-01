#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QWaylandLayerShellV1>
#include <QWaylandLayerSurfaceV1>

/**
 * Ví dụ đơn giản: Top Panel với clock
 * 
 * Panel này sẽ hiển thị ở trên cùng màn hình với:
 * - Clock ở giữa
 * - Các nút điều khiển ở bên phải
 */
class TopPanel : public QWidget {
    Q_OBJECT
    
public:
    TopPanel(QWidget *parent = nullptr) : QWidget(parent) {
        // Thiết lập cửa sổ không viền và trong suốt
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Tạo layout ngang
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 5, 10, 5);
        layout->setSpacing(10);
        
        // Label hiển thị thời gian
        timeLabel = new QLabel(this);
        timeLabel->setStyleSheet(
            "QLabel {"
            "  background: rgba(0, 0, 0, 150);"
            "  color: white;"
            "  padding: 5px 15px;"
            "  border-radius: 5px;"
            "  font-size: 14px;"
            "}"
        );
        layout->addWidget(timeLabel);
        
        layout->addStretch();
        
        // Nút đóng
        auto *closeBtn = new QPushButton("×", this);
        closeBtn->setFixedSize(30, 30);
        closeBtn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(255, 0, 0, 200);"
            "  color: white;"
            "  border-radius: 15px;"
            "  font-size: 18px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "  background: rgba(255, 0, 0, 255);"
            "}"
        );
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
        layout->addWidget(closeBtn);
        
        // Cập nhật thời gian mỗi giây
        auto *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TopPanel::updateTime);
        timer->start(1000);
        updateTime();
        
        // Thiết lập LayerShell
        setupLayerShell();
    }
    
private slots:
    void updateTime() {
        QDateTime now = QDateTime::currentDateTime();
        timeLabel->setText(now.toString("hh:mm:ss - dddd, MMMM dd, yyyy"));
    }
    
private:
    void setupLayerShell() {
        // Kiểm tra platform
        if (QGuiApplication::platformName() != "wayland") {
            qWarning() << "Not running on Wayland!";
            return;
        }
        
        // Lấy instance của LayerShell
        auto *layerShell = QWaylandLayerShellV1::instance();
        if (!layerShell) {
            qWarning() << "LayerShell not available";
            return;
        }
        
        // Lấy QWindow từ widget
        QWindow *window = windowHandle();
        if (!window) {
            qWarning() << "Window handle not available";
            return;
        }
        
        // Tạo layer surface
        layerSurface = layerShell->getLayerSurface(window);
        if (!layerSurface) {
            qWarning() << "Failed to create layer surface";
            return;
        }
        
        // Thiết lập thuộc tính
        layerSurface->setLayer(QWaylandLayerShellV1::TopLayer);
        layerSurface->setAnchor(Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge);
        layerSurface->setExclusiveZone(40); // 40px chiều cao
        layerSurface->setKeyboardInteractivity(
            QWaylandLayerShellV1::OnDemand
        );
        
        // Kết nối signals
        connect(layerSurface, &QWaylandLayerSurfaceV1::configureRequested,
                this, &TopPanel::onConfigure);
        connect(layerSurface, &QWaylandLayerSurfaceV1::closed,
                this, &QWidget::close);
        
        // Commit
        layerSurface->commit();
        
        // Thiết lập kích thước ban đầu
        resize(1920, 40); // Giả sử màn hình 1920px
    }
    
    void onConfigure(const QSize &size, uint32_t serial) {
        // Cập nhật kích thước theo yêu cầu của compositor
        resize(size.width(), 40); // Giữ chiều cao cố định
        
        // ACK configure (quan trọng!)
        if (layerSurface) {
            layerSurface->ackConfigure(serial);
        }
    }
    
    QLabel *timeLabel = nullptr;
    QWaylandLayerSurfaceV1 *layerSurface = nullptr;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Đảm bảo sử dụng Wayland
    qputenv("QT_QPA_PLATFORM", "wayland");
    
    TopPanel panel;
    panel.show();
    
    return app.exec();
}

#include "top_panel.moc"
