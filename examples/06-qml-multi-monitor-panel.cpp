/**
 * Example 6: QML Multi-Monitor Panel
 * 
 * Model-View architecture with QML
 * Separate panels for each monitor
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QScreen>
#include <QAbstractListModel>
#include <QHash>
#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

// ============================================================================
// MODEL: PanelModel - Quản lý screens và panel data
// ============================================================================

class PanelModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    enum Roles {
        ScreenNameRole = Qt::UserRole + 1,
        ScreenWidthRole,
        ScreenHeightRole,
        ScreenDpiRole,
        IsPrimaryRole
    };
    
    explicit PanelModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        // Load initial screens
        refreshScreens();
        
        // Watch for screen changes
        connect(qApp, &QGuiApplication::screenAdded,
                this, &PanelModel::onScreenAdded);
        connect(qApp, &QGuiApplication::screenRemoved,
                this, &PanelModel::onScreenRemoved);
    }
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_screens.count();
    }
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_screens.count()) {
            return QVariant();
        }
        
        QScreen *screen = m_screens.at(index.row());
        
        switch (role) {
        case ScreenNameRole:
            return screen->name();
        case ScreenWidthRole:
            return screen->geometry().width();
        case ScreenHeightRole:
            return screen->geometry().height();
        case ScreenDpiRole:
            return qRound(screen->logicalDotsPerInch());
        case IsPrimaryRole:
            return screen == QGuiApplication::primaryScreen();
        default:
            return QVariant();
        }
    }
    
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[ScreenNameRole] = "screenName";
        roles[ScreenWidthRole] = "screenWidth";
        roles[ScreenHeightRole] = "screenHeight";
        roles[ScreenDpiRole] = "screenDpi";
        roles[IsPrimaryRole] = "isPrimary";
        return roles;
    }
    
    int count() const {
        return m_screens.count();
    }
    
    Q_INVOKABLE QString getScreenName(int index) const {
        if (index >= 0 && index < m_screens.count()) {
            return m_screens.at(index)->name();
        }
        return QString();
    }
    
signals:
    void countChanged();
    void screenAdded(const QString &name);
    void screenRemoved(const QString &name);
    
private slots:
    void onScreenAdded(QScreen *screen) {
        qDebug() << "Screen added:" << screen->name();
        
        beginInsertRows(QModelIndex(), m_screens.count(), m_screens.count());
        m_screens.append(screen);
        endInsertRows();
        
        emit countChanged();
        emit screenAdded(screen->name());
    }
    
    void onScreenRemoved(QScreen *screen) {
        qDebug() << "Screen removed:" << screen->name();
        
        int index = m_screens.indexOf(screen);
        if (index >= 0) {
            beginRemoveRows(QModelIndex(), index, index);
            m_screens.removeAt(index);
            endRemoveRows();
            
            emit countChanged();
            emit screenRemoved(screen->name());
        }
    }
    
private:
    void refreshScreens() {
        beginResetModel();
        m_screens = QGuiApplication::screens();
        endResetModel();
        emit countChanged();
    }
    
    QList<QScreen*> m_screens;
};

// ============================================================================
// VIEW MANAGER: Quản lý QML views cho mỗi screen
// ============================================================================

class PanelViewManager : public QObject {
    Q_OBJECT
    
public:
    explicit PanelViewManager(PanelModel *model, QObject *parent = nullptr)
        : QObject(parent), m_model(model) {
        
        // Create panel for each existing screen
        for (int i = 0; i < model->count(); ++i) {
            QString screenName = model->getScreenName(i);
            createPanelForScreen(screenName);
        }
        
        // Watch for screen changes
        connect(model, &PanelModel::screenAdded,
                this, &PanelViewManager::createPanelForScreen);
        connect(model, &PanelModel::screenRemoved,
                this, &PanelViewManager::removePanelForScreen);
    }
    
    ~PanelViewManager() {
        qDeleteAll(m_views);
    }
    
private slots:
    void createPanelForScreen(const QString &screenName) {
        if (m_views.contains(screenName)) {
            return; // Already exists
        }
        
        qDebug() << "Creating panel for screen:" << screenName;
        
        // Create QML view
        QQuickView *view = new QQuickView();
        
        // Expose model to QML
        view->rootContext()->setContextProperty("panelModel", m_model);
        view->rootContext()->setContextProperty("currentScreenName", screenName);
        
        // Load QML
        view->setSource(QUrl("qrc:/Panel.qml"));
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        
        // Configure layer shell
        auto layerWindow = LayerShellQt::Window::get(view);
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);
        layerWindow->setAnchors(
            LayerShellQt::Window::AnchorTop |
            LayerShellQt::Window::AnchorLeft |
            LayerShellQt::Window::AnchorRight
        );
        layerWindow->setExclusiveZone(40);
        layerWindow->setScope(screenName);  // Per-screen
        
        // Show
        view->show();
        
        // Store
        m_views.insert(screenName, view);
    }
    
    void removePanelForScreen(const QString &screenName) {
        qDebug() << "Removing panel for screen:" << screenName;
        
        QQuickView *view = m_views.take(screenName);
        if (view) {
            view->deleteLater();
        }
    }
    
private:
    PanelModel *m_model;
    QHash<QString, QQuickView*> m_views;
};

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char *argv[]) {
    // Enable high DPI
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // Check layer shell support
    if (!LayerShellQt::Shell::isAvailable()) {
        qFatal("Layer shell not available!");
        return 1;
    }
    
    // Create model
    PanelModel model;
    
    qDebug() << "Detected" << model.count() << "screen(s)";
    
    // Create view manager
    PanelViewManager viewManager(&model);
    
    return app.exec();
}

#include "06-qml-multi-monitor-panel.moc"
