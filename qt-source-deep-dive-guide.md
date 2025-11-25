# Hướng Dẫn Đọc Hiểu Sâu Code Nguồn Qt Core

## Mục lục
1. [Phương pháp tiếp cận](#1-phương-pháp-tiếp-cận)
2. [Kiến trúc tổng quan](#2-kiến-trúc-tổng-quan)
3. [QGuiApplication - Deep Dive](#3-qguiapplication---deep-dive)
4. [QQmlApplicationEngine - Deep Dive](#4-qqmlapplicationengine---deep-dive)
5. [QQmlContext - Deep Dive](#5-qqmlcontext---deep-dive)
6. [Mối quan hệ giữa các class](#6-mối-quan-hệ-giữa-các-class)
7. [Kỹ thuật debug và trace](#7-kỹ-thuật-debug-và-trace)
8. [Bài tập thực hành](#8-bài-tập-thực-hành)

---

## 1. Phương pháp tiếp cận

### 1.1 Nguyên tắc đọc code Qt

```
┌─────────────────────────────────────────────────────────────┐
│                    CHIẾN LƯỢC ĐỌC CODE                      │
├─────────────────────────────────────────────────────────────┤
│  1. Top-Down: Hiểu mục đích → API → Implementation          │
│  2. D-Pointer Pattern: Class → Class_p (private impl)       │
│  3. Signal-Slot: Theo dõi luồng event                       │
│  4. Inheritance: Base class trước, derived sau              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Cấu trúc file trong Qt Source

```
qt/
├── qtbase/
│   └── src/
│       ├── gui/
│       │   └── kernel/
│       │       ├── qguiapplication.h          # Public header
│       │       ├── qguiapplication.cpp        # Implementation
│       │       └── qguiapplication_p.h        # Private header (D-pointer)
│       └── corelib/
│           └── kernel/
│               ├── qcoreapplication.h
│               └── qcoreapplication.cpp
├── qtdeclarative/
│   └── src/
│       ├── qml/
│       │   └── qml/
│       │       ├── qqmlcontext.h
│       │       ├── qqmlcontext.cpp
│       │       └── qqmlcontext_p.h
│       └── qmlmodels/
│           ├── qqmlapplicationengine.h
│           └── qqmlapplicationengine.cpp
```

### 1.3 D-Pointer Pattern (Pimpl Idiom)

Qt sử dụng D-Pointer để:
- Ẩn implementation details
- Duy trì binary compatibility
- Giảm compile time

```cpp
// Public class (qguiapplication.h)
class Q_GUI_EXPORT QGuiApplication : public QCoreApplication
{
    Q_DECLARE_PRIVATE(QGuiApplication)  // Macro tạo d_func()
public:
    QGuiApplication(int &argc, char **argv);
    // ... public API
};

// Private implementation (qguiapplication_p.h)
class QGuiApplicationPrivate : public QCoreApplicationPrivate
{
    Q_DECLARE_PUBLIC(QGuiApplication)  // Macro tạo q_func()
public:
    static QGuiApplicationPrivate *self;
    
    QPlatformIntegration *platform_integration;
    QList<QWindow *> windowList;
    // ... private data members
};
```

**Cách đọc:** Luôn mở song song file `.h`, `_p.h` và `.cpp`

---

## 2. Kiến trúc tổng quan

### 2.1 Class Hierarchy

```
                    ┌─────────────┐
                    │   QObject   │
                    └──────┬──────┘
                           │
              ┌────────────┴────────────┐
              │                         │
    ┌─────────┴─────────┐    ┌─────────┴─────────┐
    │ QCoreApplication  │    │    QQmlContext    │
    └─────────┬─────────┘    └───────────────────┘
              │
    ┌─────────┴─────────┐
    │  QGuiApplication  │
    └─────────┬─────────┘
              │
    ┌─────────┴─────────┐
    │   QApplication    │  (QtWidgets)
    └───────────────────┘


    ┌─────────────────┐
    │    QJSEngine    │
    └────────┬────────┘
             │
    ┌────────┴────────┐
    │    QQmlEngine   │
    └────────┬────────┘
             │
    ┌────────┴─────────────────┐
    │  QQmlApplicationEngine   │
    └──────────────────────────┘
```

### 2.2 Luồng khởi tạo ứng dụng Qt/QML

```
main()
    │
    ▼
QGuiApplication app(argc, argv)
    │
    ├──► QCoreApplication::init()
    ├──► QPlatformIntegration khởi tạo
    └──► Event loop setup
    │
    ▼
QQmlApplicationEngine engine
    │
    ├──► QQmlEngine::init()
    ├──► Root QQmlContext tạo ra
    └──► Import paths setup
    │
    ▼
engine.load("main.qml")
    │
    ├──► QQmlComponent tạo ra
    ├──► QML parsing & compilation
    ├──► QQmlContext cho component
    └──► QObject tree tạo ra
    │
    ▼
app.exec()
    │
    └──► Event loop chạy
```

---

## 3. QGuiApplication - Deep Dive

### 3.1 Vai trò chính

```cpp
// File: qtbase/src/gui/kernel/qguiapplication.h

class Q_GUI_EXPORT QGuiApplication : public QCoreApplication
{
    Q_OBJECT
    Q_PROPERTY(QIcon windowIcon READ windowIcon WRITE setWindowIcon)
    Q_PROPERTY(QString applicationDisplayName READ applicationDisplayName 
               WRITE setApplicationDisplayName NOTIFY applicationDisplayNameChanged)
    Q_PROPERTY(QString desktopFileName READ desktopFileName 
               WRITE setDesktopFileName)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection 
               WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    Q_PROPERTY(QString platformName READ platformName STORED false)
    Q_PROPERTY(bool quitOnLastWindowClosed READ quitOnLastWindowClosed 
               WRITE setQuitOnLastWindowClosed)
    Q_PROPERTY(QScreen *primaryScreen READ primaryScreen 
               NOTIFY primaryScreenChanged)
    // ...
};
```

### 3.2 Các thành phần quan trọng cần đọc

#### A. Constructor - Điểm khởi đầu

```cpp
// File: qguiapplication.cpp

QGuiApplication::QGuiApplication(int &argc, char **argv, int flags)
    : QCoreApplication(*new QGuiApplicationPrivate(argc, argv, flags))
{
    d_func()->init();  // ← QUAN TRỌNG: Đọc hàm init()
}

// Trong QGuiApplicationPrivate::init()
void QGuiApplicationPrivate::init()
{
    // 1. Khởi tạo platform integration
    initPlatformIntegration();
    
    // 2. Khởi tạo palette, font
    initPalette();
    
    // 3. Kết nối các signal system
    QObject::connect(QGuiApplication::instance(), 
                     &QGuiApplication::lastWindowClosed,
                     QGuiApplication::instance(), 
                     &QCoreApplication::quit);
}
```

#### B. Platform Integration - Cầu nối với OS

```cpp
// File: qguiapplication_p.h

class Q_GUI_EXPORT QGuiApplicationPrivate : public QCoreApplicationPrivate
{
public:
    // Platform abstraction - ĐÂY LÀ ĐIỂM QUAN TRỌNG
    static QPlatformIntegration *platform_integration;
    
    // Window management
    static QList<QWindow *> windowList;
    static QWindow *focus_window;
    
    // Input handling
    static QInputMethod *inputMethod;
    static QPlatformTheme *platform_theme;
    
    // Screen management  
    static QList<QScreen *> screen_list;
};
```

#### C. Event Processing - Xử lý sự kiện

```cpp
// Đây là hàm CỐT LÕI của Qt GUI
bool QGuiApplication::notify(QObject *object, QEvent *event)
{
    // Tất cả events đều đi qua đây
    switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        // Xử lý keyboard events
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        // Xử lý mouse events
        break;
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        // Xử lý touch events
        break;
    // ... nhiều event types khác
    }
    return QCoreApplication::notify(object, event);
}
```

### 3.3 Sơ đồ luồng xử lý Event

```
┌──────────────────────────────────────────────────────────────┐
│                     OS (Linux/Windows/macOS)                 │
└───────────────────────────┬──────────────────────────────────┘
                            │ Native events
                            ▼
┌──────────────────────────────────────────────────────────────┐
│                   QPlatformIntegration                       │
│  (Xcb/Cocoa/Windows platform plugin)                         │
└───────────────────────────┬──────────────────────────────────┘
                            │ QPA events
                            ▼
┌──────────────────────────────────────────────────────────────┐
│               QGuiApplicationPrivate                         │
│  processMouseEvent(), processKeyEvent(), etc.                │
└───────────────────────────┬──────────────────────────────────┘
                            │ QEvent objects
                            ▼
┌──────────────────────────────────────────────────────────────┐
│            QGuiApplication::notify()                         │
│  Central event dispatcher                                    │
└───────────────────────────┬──────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│               QWindow / QQuickWindow                         │
│  event() → specific handlers                                 │
└──────────────────────────────────────────────────────────────┘
```

### 3.4 Checklist đọc QGuiApplication

- [ ] `QGuiApplication::QGuiApplication()` - Constructor
- [ ] `QGuiApplicationPrivate::init()` - Initialization sequence
- [ ] `QGuiApplicationPrivate::processMouseEvent()` - Mouse handling
- [ ] `QGuiApplicationPrivate::processKeyEvent()` - Keyboard handling
- [ ] `QGuiApplication::notify()` - Event dispatch
- [ ] `QGuiApplication::exec()` - Event loop (inherited)
- [ ] Platform plugins: `src/plugins/platforms/`

---

## 4. QQmlApplicationEngine - Deep Dive

### 4.1 Vai trò chính

`QQmlApplicationEngine` là convenience class kết hợp:
- `QQmlEngine` (JavaScript engine + QML runtime)
- Automatic window loading
- Translation support
- Application lifecycle management

### 4.2 Cấu trúc class

```cpp
// File: qtdeclarative/src/qml/qml/qqmlapplicationengine.h

class Q_QML_EXPORT QQmlApplicationEngine : public QQmlEngine
{
    Q_OBJECT
public:
    QQmlApplicationEngine(QObject *parent = nullptr);
    QQmlApplicationEngine(const QUrl &url, QObject *parent = nullptr);
    QQmlApplicationEngine(const QString &filePath, QObject *parent = nullptr);
    ~QQmlApplicationEngine();

    QList<QObject*> rootObjects() const;  // ← Truy cập root objects

public Q_SLOTS:
    void load(const QUrl &url);           // ← Load QML file
    void loadFromModule(const QString &uri, const QString &typeName);
    void setInitialProperties(const QVariantMap &initialProperties);
    void setExtraFileSelectors(const QStringList &extraFileSelectors);

Q_SIGNALS:
    void objectCreated(QObject *object, const QUrl &url);
    void objectCreationFailed(const QUrl &url);  // ← Error handling
};
```

### 4.3 Luồng Load QML File

```cpp
// File: qqmlapplicationengine.cpp

void QQmlApplicationEngine::load(const QUrl &url)
{
    Q_D(QQmlApplicationEngine);
    
    // 1. Tạo QQmlComponent để parse/compile QML
    QQmlComponent component(this, url);
    
    // 2. Kiểm tra errors
    if (component.isLoading()) {
        // Async loading - chờ
        QObject::connect(&component, &QQmlComponent::statusChanged,
                         this, [this, &component, url]() {
            d->finishLoad(component, url);
        });
    } else {
        // Sync loading
        d->finishLoad(component, url);
    }
}

void QQmlApplicationEnginePrivate::finishLoad(QQmlComponent &component, 
                                               const QUrl &url)
{
    if (component.isError()) {
        // Emit error signal
        emit q_func()->objectCreationFailed(url);
        return;
    }
    
    // 3. Tạo QML object tree
    QObject *object = component.create();  // ← QUAN TRỌNG
    
    if (object) {
        // 4. Store root object
        rootObjects.append(object);
        
        // 5. Nếu là Window, show nó
        if (QWindow *window = qobject_cast<QWindow *>(object)) {
            window->show();
        }
        
        emit q_func()->objectCreated(object, url);
    }
}
```

### 4.4 Sơ đồ luồng Load QML

```
engine.load("main.qml")
           │
           ▼
┌──────────────────────────────────────┐
│    QQmlComponent(engine, url)        │
│    - Parse QML file                  │
│    - Resolve imports                 │
│    - Compile to bytecode             │
└───────────────────┬──────────────────┘
                    │
                    ▼
┌──────────────────────────────────────┐
│    component.create()                │
│    - Allocate QObjects               │
│    - Set properties                  │
│    - Create bindings                 │
│    - Connect signals                 │
└───────────────────┬──────────────────┘
                    │
                    ▼
┌──────────────────────────────────────┐
│    QML Object Tree                   │
│    ┌─────────────┐                   │
│    │ Window      │ (root)            │
│    │  ├─ Item    │                   │
│    │  │   └─ Text│                   │
│    │  └─ Button  │                   │
│    └─────────────┘                   │
└──────────────────────────────────────┘
```

### 4.5 QQmlEngine - Base class quan trọng

```cpp
// File: qqmlengine.h

class Q_QML_EXPORT QQmlEngine : public QJSEngine
{
    Q_OBJECT
public:
    // Import paths - nơi tìm QML modules
    QStringList importPathList() const;
    void addImportPath(const QString &dir);
    
    // Plugin paths
    QStringList pluginPathList() const;
    void addPluginPath(const QString &dir);
    
    // Root context - QUAN TRỌNG
    QQmlContext *rootContext() const;
    
    // Object ownership
    enum ObjectOwnership { CppOwnership, JavaScriptOwnership };
    static void setObjectOwnership(QObject *, ObjectOwnership);
    
    // Network access
    QNetworkAccessManager *networkAccessManager() const;
    
    // Singleton instance registration
    template<typename T>
    static QObject *singletonInstance(int qmlTypeId);
};
```

### 4.6 Checklist đọc QQmlApplicationEngine

- [ ] `QQmlApplicationEngine::QQmlApplicationEngine()` - Constructor
- [ ] `QQmlApplicationEngine::load()` - Main entry point
- [ ] `QQmlApplicationEnginePrivate::finishLoad()` - Object creation
- [ ] `QQmlEngine::rootContext()` - Context access
- [ ] `QQmlComponent::create()` - Object instantiation
- [ ] Error handling signals

---

## 5. QQmlContext - Deep Dive

### 5.1 Vai trò chính

`QQmlContext` là:
- Scope cho property bindings
- Bridge giữa C++ và QML
- Hierarchical context tree

### 5.2 Cấu trúc class

```cpp
// File: qtdeclarative/src/qml/qml/qqmlcontext.h

class Q_QML_EXPORT QQmlContext : public QObject
{
    Q_OBJECT
public:
    QQmlContext(QQmlEngine *parent, QObject *objParent = nullptr);
    QQmlContext(QQmlContext *parent, QObject *objParent = nullptr);
    ~QQmlContext();

    bool isValid() const;
    
    QQmlEngine *engine() const;
    QQmlContext *parentContext() const;
    
    // ĐÂY LÀ CÁC HÀM QUAN TRỌNG NHẤT
    
    // Expose C++ object to QML
    void setContextObject(QObject *object);
    QObject *contextObject() const;
    
    // Expose C++ property to QML by name
    void setContextProperty(const QString &name, QObject *value);
    void setContextProperty(const QString &name, const QVariant &value);
    QVariant contextProperty(const QString &name) const;
    
    // Name resolution
    QString nameForObject(const QObject *object) const;
    QObject *objectForName(const QString &name) const;

    QUrl resolvedUrl(const QUrl &url) const;
    void setBaseUrl(const QUrl &baseUrl);
    QUrl baseUrl() const;
};
```

### 5.3 Context Hierarchy

```
┌─────────────────────────────────────────────────────────────┐
│                    Root Context                              │
│  (QQmlEngine::rootContext())                                │
│                                                              │
│  Context Properties:                                         │
│    - "applicationName" → QString                             │
│    - "backend" → BackendController*                          │
└───────────────────────────┬─────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        ▼                   ▼                   ▼
┌───────────────┐   ┌───────────────┐   ┌───────────────┐
│ Component A   │   │ Component B   │   │ Component C   │
│ Context       │   │ Context       │   │ Context       │
│               │   │               │   │               │
│ Properties:   │   │ Properties:   │   │ Properties:   │
│  - "model"    │   │  - "index"    │   │  - "item"     │
└───────────────┘   └───────────────┘   └───────────────┘
```

### 5.4 Name Resolution Process

```cpp
// Khi QML truy cập một property name:

// main.qml
Text {
    text: someProperty  // Làm sao Qt tìm "someProperty"?
}
```

```
Lookup "someProperty":
           │
           ▼
┌─────────────────────────────────┐
│  1. Current context properties  │ ──► Found? Return value
└───────────────────┬─────────────┘
                    │ Not found
                    ▼
┌─────────────────────────────────┐
│  2. Context object properties   │ ──► Found? Return value
│     (via QMetaObject)           │
└───────────────────┬─────────────┘
                    │ Not found
                    ▼
┌─────────────────────────────────┐
│  3. Parent context (recursive)  │ ──► Found? Return value
└───────────────────┬─────────────┘
                    │ Not found
                    ▼
┌─────────────────────────────────┐
│  4. Root context                │ ──► Found? Return value
└───────────────────┬─────────────┘
                    │ Not found
                    ▼
┌─────────────────────────────────┐
│  5. Global object (Math, etc.)  │ ──► Found? Return value
└───────────────────┬─────────────┘
                    │ Not found
                    ▼
            ReferenceError!
```

### 5.5 Private Implementation

```cpp
// File: qqmlcontext_p.h

class QQmlContextPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QQmlContext)
public:
    // Engine reference
    QQmlEngine *engine;
    
    // Parent context in hierarchy
    QQmlContext *parent;
    
    // Children contexts
    QList<QQmlContext *> childContexts;
    
    // Context object - all its properties accessible in QML
    QPointer<QObject> contextObject;
    
    // Explicitly set context properties
    // Key: property name, Value: property value
    QHash<QString, QVariant> propertyValues;
    QHash<QString, QPointer<QObject>> propertyObjects;
    
    // URL for relative path resolution
    QUrl baseUrl;
    
    // Property index cache for fast lookup
    QQmlPropertyCache *propertyCache;
    
    // Expressions bound to this context
    QQmlContextData *contextData;
};
```

### 5.6 Ví dụ sử dụng Context

```cpp
// main.cpp
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    
    // 1. Set context property on ROOT context
    // Có thể truy cập từ BẤT KỲ QML file nào
    engine.rootContext()->setContextProperty("appVersion", "1.0.0");
    
    // 2. Expose C++ object
    BackendController backend;
    engine.rootContext()->setContextProperty("backend", &backend);
    
    // 3. Set context object (all Q_PROPERTY accessible)
    AppSettings settings;
    engine.rootContext()->setContextObject(&settings);
    
    engine.load(QUrl("qrc:/main.qml"));
    return app.exec();
}

// BackendController.h
class BackendController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
public:
    Q_INVOKABLE void doSomething();  // Callable from QML
signals:
    void statusChanged();
};
```

```qml
// main.qml
Window {
    Text {
        // Truy cập context property
        text: "Version: " + appVersion
    }
    
    Button {
        onClicked: {
            // Gọi C++ method
            backend.doSomething()
            
            // Truy cập context object property
            console.log(status)  // từ AppSettings
        }
    }
}
```

### 5.7 Checklist đọc QQmlContext

- [ ] `QQmlContext::setContextProperty()` - Property injection
- [ ] `QQmlContext::setContextObject()` - Object injection
- [ ] `QQmlContextPrivate` - Internal data structures
- [ ] `QQmlContextData` - Lower-level context data
- [ ] Name resolution in `qqmlengine.cpp`
- [ ] Property binding evaluation

---

## 6. Mối quan hệ giữa các class

### 6.1 Sơ đồ tổng hợp

```
┌─────────────────────────────────────────────────────────────────────────┐
│                              Application                                 │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     QGuiApplication                              │   │
│  │  - Event loop management                                         │   │
│  │  - Platform integration                                          │   │
│  │  - Input handling                                                │   │
│  │  - Screen management                                             │   │
│  └────────────────────────────────┬────────────────────────────────┘   │
│                                   │ creates/manages                     │
│                                   ▼                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                  QQmlApplicationEngine                           │   │
│  │  ┌───────────────────────────────────────────────────────────┐  │   │
│  │  │                      QQmlEngine                            │  │   │
│  │  │  - QML/JS runtime                                          │  │   │
│  │  │  - Import resolution                                       │  │   │
│  │  │  - Type registration                                       │  │   │
│  │  │                                                            │  │   │
│  │  │  ┌───────────────────────────────────────────────────┐    │  │   │
│  │  │  │              Root QQmlContext                      │    │  │   │
│  │  │  │  - Global context properties                       │    │  │   │
│  │  │  │  - C++ ↔ QML bridge                               │    │  │   │
│  │  │  │                                                    │    │  │   │
│  │  │  │  ┌─────────────┐  ┌─────────────┐                 │    │  │   │
│  │  │  │  │ Component   │  │ Component   │                 │    │  │   │
│  │  │  │  │ Context     │  │ Context     │                 │    │  │   │
│  │  │  │  └─────────────┘  └─────────────┘                 │    │  │   │
│  │  │  └───────────────────────────────────────────────────┘    │  │   │
│  │  └───────────────────────────────────────────────────────────┘  │   │
│  │                                                                  │   │
│  │  rootObjects() → [QWindow, ...]                                  │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 6.2 Sequence Diagram - Typical App Startup

```
main()          QGuiApplication    QQmlApplicationEngine    QQmlContext
  │                   │                     │                    │
  │  new()            │                     │                    │
  ├──────────────────►│                     │                    │
  │                   │ init()              │                    │
  │                   ├──────►              │                    │
  │                   │                     │                    │
  │  new()            │                     │                    │
  ├─────────────────────────────────────────►                    │
  │                   │                     │ create root ctx    │
  │                   │                     ├────────────────────►
  │                   │                     │                    │
  │  setContextProperty()                   │                    │
  ├─────────────────────────────────────────────────────────────►│
  │                   │                     │                    │
  │  load("main.qml") │                     │                    │
  ├─────────────────────────────────────────►                    │
  │                   │                     │ create child ctx   │
  │                   │                     ├────────────────────►
  │                   │                     │                    │
  │                   │                     │ create QWindow     │
  │                   │                     ├──────►             │
  │                   │                     │                    │
  │  exec()           │                     │                    │
  ├──────────────────►│                     │                    │
  │                   │ event loop          │                    │
  │                   │◄────────────────────┼────────────────────┤
  │                   │      running        │                    │
```

---

## 7. Kỹ thuật debug và trace

### 7.1 Qt Debug Categories

```cpp
// Bật debug output cho QML
QLoggingCategory::setFilterRules("qt.qml.binding=true");
QLoggingCategory::setFilterRules("qt.qml.general=true");

// Hoặc qua environment variable
// QT_LOGGING_RULES="qt.qml.*=true"
```

### 7.2 Useful Environment Variables

```bash
# QML debugging
export QML_IMPORT_TRACE=1          # Trace import resolution
export QT_DEBUG_PLUGINS=1          # Plugin loading info
export QSG_INFO=1                  # Scene graph info
export QSG_RENDER_LOOP=basic       # Force basic render loop

# Platform debugging  
export QT_QPA_PLATFORM=xcb         # Force X11
export QT_QPA_PLATFORMTHEME=gtk3   # Force GTK theme

# Event debugging
export QT_EVENT_DISPATCHER_DEBUG=1
```

### 7.3 GDB/LLDB Breakpoints

```gdb
# Useful breakpoints for understanding flow

# App initialization
break QGuiApplication::QGuiApplication
break QGuiApplicationPrivate::init

# QML loading
break QQmlApplicationEngine::load
break QQmlComponent::create

# Context operations
break QQmlContext::setContextProperty
break QQmlContextPrivate::propertyValues

# Event handling
break QGuiApplication::notify
break QGuiApplicationPrivate::processMouseEvent
```

### 7.4 Qt Creator Debug Views

1. **QML Debugger** - Inspect QML object tree
2. **Locals and Expressions** - Watch Q_PROPERTY values
3. **Qt Internal Modules** - Load debug symbols

### 7.5 Tự thêm debug output

```cpp
// Trong code của bạn
#include <QDebug>

// Simple debug
qDebug() << "Context property set:" << name << "=" << value;

// Conditional debug
#ifdef QT_DEBUG
    qDebug() << Q_FUNC_INFO << "Entering function";
#endif

// Category-based debug
Q_LOGGING_CATEGORY(lcMyApp, "myapp.core")
qCDebug(lcMyApp) << "Detailed message";
```

---

## 8. Bài tập thực hành

### 8.1 Bài tập 1: Trace App Startup

**Mục tiêu:** Hiểu sequence khởi tạo

```cpp
// Thêm qDebug() vào các vị trí sau và chạy app:
// 1. QGuiApplication constructor
// 2. QGuiApplicationPrivate::init()
// 3. QQmlApplicationEngine constructor
// 4. QQmlApplicationEngine::load()
// 5. QQmlContext::setContextProperty()
```

### 8.2 Bài tập 2: Custom Context Property

**Mục tiêu:** Hiểu QQmlContext

```cpp
// Tạo một class với Q_PROPERTY
// Set làm context property
// Truy cập từ QML
// Observe khi property thay đổi
```

### 8.3 Bài tập 3: Event Tracing

**Mục tiêu:** Hiểu event flow

```cpp
// Override QGuiApplication::notify()
// Log tất cả events
// Phân tích event flow khi click một button
```

### 8.4 Bài tập 4: Context Hierarchy

**Mục tiêu:** Hiểu name resolution

```qml
// Tạo nested components
// Set context properties ở các levels khác nhau
// Test name shadowing và resolution
```

---

## 9. Resources

### 9.1 Source Code Locations

| Class | Repository | Path |
|-------|------------|------|
| QGuiApplication | qt/qtbase | src/gui/kernel/qguiapplication.* |
| QCoreApplication | qt/qtbase | src/corelib/kernel/qcoreapplication.* |
| QQmlEngine | qt/qtdeclarative | src/qml/qml/qqmlengine.* |
| QQmlApplicationEngine | qt/qtdeclarative | src/qml/qml/qqmlapplicationengine.* |
| QQmlContext | qt/qtdeclarative | src/qml/qml/qqmlcontext.* |
| QQmlComponent | qt/qtdeclarative | src/qml/qml/qqmlcomponent.* |

### 9.2 Documentation Links

- [Qt Source Code (code.qt.io)](https://code.qt.io)
- [Qt Documentation](https://doc.qt.io)
- [Qt Wiki - D-Pointer](https://wiki.qt.io/D-Pointer)
- [KDAB Qt Training Materials](https://www.kdab.com/qt-training/)

### 9.3 Recommended Reading Order

1. **QObject** (foundation of everything)
2. **QCoreApplication** (event loop basics)
3. **QGuiApplication** (GUI specifics)
4. **QJSEngine** (JavaScript runtime)
5. **QQmlEngine** (QML runtime)
6. **QQmlContext** (C++/QML bridge)
7. **QQmlApplicationEngine** (convenience wrapper)
8. **QQmlComponent** (QML loading/compilation)

---

## 10. Quick Reference Card

```
┌─────────────────────────────────────────────────────────────────────┐
│                     QUICK REFERENCE                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  D-Pointer Access:                                                   │
│    Q_D(ClassName)  → d (QClassNamePrivate*)                         │
│    Q_Q(ClassName)  → q (QClassName*)                                │
│    d_func()        → returns d pointer                              │
│    q_func()        → returns q pointer                              │
│                                                                      │
│  Common Macros:                                                      │
│    Q_DECLARE_PRIVATE(Class)  - in public class                      │
│    Q_DECLARE_PUBLIC(Class)   - in private class                     │
│    Q_OBJECT                  - enables meta-object features         │
│    Q_PROPERTY(...)           - declares meta-property               │
│    Q_INVOKABLE               - callable from QML                    │
│                                                                      │
│  Key Static Members:                                                 │
│    QGuiApplication::instance()     → app singleton                  │
│    QGuiApplicationPrivate::self    → private singleton              │
│    QCoreApplication::eventDispatcher() → event dispatcher           │
│                                                                      │
│  File Naming Convention:                                             │
│    qclassname.h      - public header                                │
│    qclassname_p.h    - private header                               │
│    qclassname.cpp    - implementation                               │
│    qclassname_p.cpp  - private implementation (rare)                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

*Tài liệu này được tạo để hướng dẫn đọc hiểu code nguồn Qt. Để có hiểu biết sâu nhất, hãy kết hợp đọc tài liệu này với việc debug step-by-step trong Qt Creator.*
