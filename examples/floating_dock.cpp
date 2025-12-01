/**
 * LayerShellQt Example: Floating Dock
 * 
 * Ví dụ tạo một floating dock với:
 * - Layer: Top
 * - Anchors: Chỉ Bottom (dock nổi, không full-width)
 * - Exclusive Zone: 0 (không chiếm không gian)
 * - Margins: 12px từ bottom
 */

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProcess>
#include <QWindow>
#include <QGraphicsDropShadowEffect>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class FloatingDock : public QWidget {
    Q_OBJECT
    
public:
    FloatingDock(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setFixedHeight(68);
        
        // Transparent background cho hiệu ứng nổi
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    void setupUI() {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(8);
        
        // Danh sách các ứng dụng
        struct AppItem {
            QString icon;
            QString name;
            QString command;
        };
        
        QVector<AppItem> apps = {
            {"📁", "Files", "dolphin"},
            {"🌐", "Browser", "firefox"},
            {"📝", "Editor", "kate"},
            {"🖥️", "Terminal", "konsole"},
            {"⚙️", "Settings", "systemsettings"},
            {"🎵", "Music", "elisa"},
            {"📷", "Photos", "gwenview"}
        };
        
        for (const auto &app : apps) {
            auto *btn = new QPushButton(app.icon);
            btn->setToolTip(app.name);
            btn->setFixedSize(44, 44);
            
            QString cmd = app.command;
            connect(btn, &QPushButton::clicked, [cmd]() {
                QProcess::startDetached(cmd, QStringList());
            });
            
            layout->addWidget(btn);
        }
        
        // Style cho dock
        setStyleSheet(R"(
            FloatingDock {
                background-color: rgba(30, 30, 40, 0.95);
                border-radius: 16px;
                border: 1px solid rgba(255, 255, 255, 0.1);
            }
            QPushButton {
                background-color: transparent;
                border: none;
                border-radius: 10px;
                font-size: 22px;
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.15);
                transform: translateY(-4px);
            }
            QPushButton:pressed {
                background-color: rgba(255, 255, 255, 0.25);
            }
        )");
    }
};

int main(int argc, char *argv[]) {
    LayerShellQt::Shell::useLayerShell();
    
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isLayerShellSupported()) {
        qWarning() << "❌ Layer Shell không được hỗ trợ!";
        return 1;
    }
    
    FloatingDock dock;
    dock.createWinId();
    
    if (auto *lsWindow = LayerShellQt::Window::get(dock.windowHandle())) {
        lsWindow->setLayer(LayerShellQt::Window::LayerTop);
        
        // CHỈ neo vào cạnh dưới -> dock nổi ở giữa
        lsWindow->setAnchors(LayerShellQt::Window::AnchorBottom);
        
        // KHÔNG chiếm exclusive zone -> dock nổi trên các cửa sổ
        lsWindow->setExclusiveZone(0);
        
        // Khoảng cách từ cạnh dưới màn hình
        lsWindow->setMargins(QMargins(0, 0, 0, 12));
        
        lsWindow->setKeyboardInteractivity(
            LayerShellQt::Window::KeyboardInteractivityNone
        );
        
        qDebug() << "✅ Floating Dock đã được cấu hình";
    }
    
    dock.show();
    
    return app.exec();
}

#include "floating_dock.moc"
