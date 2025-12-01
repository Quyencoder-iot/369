/**
 * Example 5: Multi-Monitor Panel
 * 
 * Creates a separate panel for each connected monitor
 * Handles monitor hotplug (add/remove)
 */

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QScreen>
#include <QHash>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

class Panel : public QWidget {
    Q_OBJECT
public:
    Panel(QScreen *screen) : targetScreen(screen) {
        setFixedHeight(35);
        
        setStyleSheet(R"(
            QWidget {
                background-color: #1e1e1e;
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
        
        // Screen name
        QString screenName = screen->name();
        QLabel *nameLabel = new QLabel("📺 " + screenName);
        layout->addWidget(nameLabel);
        
        // Screen info
        QRect geom = screen->geometry();
        QString info = QString("%1×%2 @ %3 DPI")
            .arg(geom.width())
            .arg(geom.height())
            .arg(int(screen->logicalDotsPerInch()));
        QLabel *infoLabel = new QLabel(info);
        infoLabel->setStyleSheet("color: #888;");
        layout->addWidget(infoLabel);
        
        layout->addStretch();
        
        // Clock
        clockLabel = new QLabel();
        updateClock();
        layout->addWidget(clockLabel);
        
        // Update clock
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Panel::updateClock);
        timer->start(1000);
        
        // Configure layer shell
        setupLayerShell();
    }
    
    QString getScreenName() const {
        return targetScreen->name();
    }
    
private:
    void setupLayerShell() {
        auto layerWindow = LayerShellQt::Window::get(windowHandle());
        
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(35);
        
        // KEY: Set scope to specific screen
        layerWindow->setScope(targetScreen->name());
    }
    
    void updateClock() {
        clockLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
    }
    
    QScreen *targetScreen;
    QLabel *clockLabel;
};

class MultiMonitorManager : public QObject {
    Q_OBJECT
public:
    MultiMonitorManager() {
        // Create panel for each existing screen
        for (QScreen *screen : QGuiApplication::screens()) {
            addScreen(screen);
        }
        
        // Handle screen hotplug
        connect(qApp, &QGuiApplication::screenAdded, 
                this, &MultiMonitorManager::addScreen);
        connect(qApp, &QGuiApplication::screenRemoved, 
                this, &MultiMonitorManager::removeScreen);
    }
    
    ~MultiMonitorManager() {
        qDeleteAll(panels);
    }
    
private slots:
    void addScreen(QScreen *screen) {
        if (panels.contains(screen->name())) {
            return;  // Already have panel for this screen
        }
        
        qDebug() << "Adding panel for screen:" << screen->name() 
                 << "geometry:" << screen->geometry();
        
        Panel *panel = new Panel(screen);
        panels.insert(screen->name(), panel);
        panel->show();
    }
    
    void removeScreen(QScreen *screen) {
        QString name = screen->name();
        
        qDebug() << "Removing panel for screen:" << name;
        
        Panel *panel = panels.take(name);
        if (panel) {
            panel->deleteLater();
        }
    }
    
private:
    QHash<QString, Panel*> panels;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    
    qDebug() << "Detected screens:";
    for (QScreen *screen : QGuiApplication::screens()) {
        qDebug() << " -" << screen->name() << screen->geometry();
    }
    
    MultiMonitorManager manager;
    
    return app.exec();
}

#include "05-multi-monitor-panel.moc"
