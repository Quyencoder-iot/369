/**
 * LayerShellQt Example: Top Panel
 * 
 * Ví dụ tạo một top panel đơn giản với:
 * - Layer: Top (trên các cửa sổ thường)
 * - Anchors: Top + Left + Right (thanh ngang dọc cạnh trên)
 * - Exclusive Zone: 36px (các cửa sổ sẽ không che panel)
 */

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QTimer>
#include <QWindow>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class TopPanel : public QWidget {
    Q_OBJECT
    
public:
    TopPanel(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupTimer();
        setFixedHeight(36);
    }

private:
    void setupUI() {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(12, 0, 12, 0);
        layout->setSpacing(16);
        
        // Menu button
        auto *menuBtn = new QPushButton("☰ Menu");
        menuBtn->setFlat(true);
        menuBtn->setStyleSheet("color: white; font-size: 13px; padding: 6px 12px;");
        
        // Title
        auto *title = new QLabel("LayerShellQt Demo Panel");
        title->setStyleSheet("color: white; font-weight: bold; font-size: 13px;");
        
        // Spacer
        auto *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        // System indicators
        auto *indicators = new QLabel("🔊 📶 🔋 85%");
        indicators->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 12px;");
        
        // Clock
        m_clockLabel = new QLabel();
        m_clockLabel->setStyleSheet("color: white; font-size: 13px;");
        
        layout->addWidget(menuBtn);
        layout->addWidget(title);
        layout->addWidget(spacer);
        layout->addWidget(indicators);
        layout->addWidget(m_clockLabel);
        
        // Panel style
        setStyleSheet(R"(
            TopPanel {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #3a3a4a, stop:1 #2a2a3a);
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.1);
                border-radius: 4px;
            }
        )");
    }
    
    void setupTimer() {
        auto *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TopPanel::updateClock);
        timer->start(1000);
        updateClock();
    }
    
private slots:
    void updateClock() {
        m_clockLabel->setText(
            QDateTime::currentDateTime().toString("HH:mm:ss  -  ddd, dd MMM yyyy")
        );
    }
    
private:
    QLabel *m_clockLabel;
};

int main(int argc, char *argv[]) {
    // ⚠️ QUAN TRỌNG: Phải gọi TRƯỚC QApplication
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    // Kiểm tra hỗ trợ Layer Shell
    if (!LayerShellQt::Shell::isLayerShellSupported()) {
        qWarning() << "❌ Layer Shell không được hỗ trợ!";
        qWarning() << "   Có thể bạn đang chạy trên X11 hoặc GNOME Wayland";
        return 1;
    }
    
    TopPanel panel;
    
    // Tạo window handle trước khi cấu hình
    panel.createWinId();
    
    // Cấu hình Layer Shell
    if (auto *lsWindow = LayerShellQt::Window::get(panel.windowHandle())) {
        // Layer Top: hiển thị trên các cửa sổ thường
        lsWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // Neo vào cạnh trên, trải dài từ trái sang phải
        lsWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        
        // Chiếm 36px phía trên (các cửa sổ maximize sẽ không che panel)
        lsWindow->setExclusiveZone(36);
        
        // Panel không cần nhận keyboard focus
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
        
        qDebug() << "✅ Panel đã được cấu hình với Layer Shell";
    }
    
    panel.show();
    
    return app.exec();
}

#include "top_panel.moc"
