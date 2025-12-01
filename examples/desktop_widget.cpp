/**
 * LayerShellQt Example: Desktop Widget
 * 
 * Ví dụ tạo một desktop widget (đồng hồ) với:
 * - Layer: Background (dưới tất cả cửa sổ)
 * - Anchors: Bottom + Right (góc dưới phải)
 * - Exclusive Zone: 0
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QTimer>
#include <QLocale>
#include <QWindow>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class DesktopClockWidget : public QWidget {
    Q_OBJECT
    
public:
    DesktopClockWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupTimer();
        
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(350, 180);
    }

private:
    void setupUI() {
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(20, 15, 20, 15);
        layout->setSpacing(5);
        
        // Thời gian
        m_timeLabel = new QLabel();
        m_timeLabel->setAlignment(Qt::AlignCenter);
        m_timeLabel->setStyleSheet(R"(
            font-size: 64px;
            font-weight: 100;
            color: rgba(255, 255, 255, 0.85);
            font-family: 'Segoe UI Light', 'SF Pro Display', sans-serif;
        )");
        
        // Ngày
        m_dateLabel = new QLabel();
        m_dateLabel->setAlignment(Qt::AlignCenter);
        m_dateLabel->setStyleSheet(R"(
            font-size: 18px;
            color: rgba(255, 255, 255, 0.6);
            font-family: 'Segoe UI', 'SF Pro Text', sans-serif;
        )");
        
        // Weather placeholder
        m_weatherLabel = new QLabel("☀️ 25°C - Trời nắng");
        m_weatherLabel->setAlignment(Qt::AlignCenter);
        m_weatherLabel->setStyleSheet(R"(
            font-size: 14px;
            color: rgba(255, 255, 255, 0.5);
            margin-top: 8px;
        )");
        
        layout->addWidget(m_timeLabel);
        layout->addWidget(m_dateLabel);
        layout->addWidget(m_weatherLabel);
        
        // Widget style - transparent với subtle border
        setStyleSheet(R"(
            DesktopClockWidget {
                background-color: rgba(0, 0, 0, 0.3);
                border-radius: 16px;
                border: 1px solid rgba(255, 255, 255, 0.1);
            }
        )");
    }
    
    void setupTimer() {
        auto *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &DesktopClockWidget::updateDisplay);
        timer->start(1000);
        updateDisplay();
    }
    
private slots:
    void updateDisplay() {
        auto now = QDateTime::currentDateTime();
        
        m_timeLabel->setText(now.toString("HH:mm"));
        
        // Format ngày tháng theo locale
        QLocale locale(QLocale::Vietnamese);
        QString dayName = locale.dayName(now.date().dayOfWeek(), QLocale::LongFormat);
        m_dateLabel->setText(QString("%1, ngày %2")
            .arg(dayName)
            .arg(now.toString("d MMMM yyyy")));
    }
    
private:
    QLabel *m_timeLabel;
    QLabel *m_dateLabel;
    QLabel *m_weatherLabel;
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isLayerShellSupported()) {
        qWarning() << "❌ Layer Shell không được hỗ trợ!";
        return 1;
    }
    
    DesktopClockWidget widget;
    widget.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(widget.windowHandle())) {
        // BACKGROUND layer: dưới tất cả các cửa sổ, như phần của desktop
        lsWindow->setLayer(LayerShellQt::Window::LayerBackground);
        
        // Neo vào góc dưới phải
        lsWindow->setAnchors(
            LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorRight
        );
        
        // Không chiếm không gian
        lsWindow->setExclusiveZone(0);
        
        // Margin từ các cạnh
        lsWindow->setMargins(QMargins(0, 0, 40, 40));
        
        // Widget không cần keyboard
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
        
        qDebug() << "✅ Desktop Widget đã được cấu hình trên Background layer";
    }
    
    widget.show();
    
    return app.exec();
}

#include "desktop_widget.moc"
