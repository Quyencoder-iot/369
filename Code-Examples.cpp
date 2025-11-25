/**
 * Code Examples for Qt Core Classes Deep Dive
 * 
 * Các ví dụ thực tế để thực hành và hiểu sâu về:
 * - QGuiApplication
 * - QQmlApplicationEngine  
 * - QQmlContext
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QDebug>
#include <QElapsedTimer>

// ============================================================================
// Example 1: Custom QGuiApplication với Event Logging
// ============================================================================
// 
// MỤC ĐÍCH: Hiểu cách events được xử lý trong Qt
// 
// GIẢI THÍCH:
// - QGuiApplication::notify() là điểm trung tâm xử lý TẤT CẢ events
// - Mọi event (mouse, keyboard, timer) đều đi qua hàm này
// - Override notify() cho phép bạn:
//   + Log/monitor tất cả events
//   + Filter/block events
//   + Measure performance
//   + Debug event flow
// 
// CÁCH HOẠT ĐỘNG:
// 1. Event từ OS → QWindowSystemInterface
// 2. → Event queue → Event loop
// 3. → notify() (điểm này!) → receiver->event()
// 4. → Specific handler (mousePressEvent, keyPressEvent, etc.)
//
// HỌC ĐƯỢC GÌ:
// - Event flow trong Qt
// - Cách intercept events
// - Performance monitoring
// - Event types và priorities
// ============================================================================

class DebugApplication : public QGuiApplication {
public:
    DebugApplication(int &argc, char **argv) 
        : QGuiApplication(argc, argv) 
    {
        qDebug() << "DebugApplication constructed";
    }
    
    bool notify(QObject *receiver, QEvent *event) override {
        // GIẢI THÍCH: Hàm này được gọi cho EVERY SINGLE EVENT trong app!
        // Cẩn thận: đừng làm gì nặng ở đây vì ảnh hưởng performance
        
        static int eventCount = 0;
        
        // Log chỉ một số event types (nếu log tất cả → quá nhiều!)
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::KeyPress ||
            event->type() == QEvent::Timer) {
            
            qDebug() << "[Event" << ++eventCount << "]"
                     << "Type:" << event->type()              // Event type enum
                     << "Receiver:" << receiver->objectName() // Object nhận event
                     << "Class:" << receiver->metaObject()->className(); // C++ class name
        }
        
        // PERFORMANCE MONITORING: Đo thời gian xử lý event
        QElapsedTimer timer;
        timer.start();
        
        // Call base implementation để xử lý event bình thường
        // QUAN TRỌNG: Phải gọi base class, nếu không events sẽ không được xử lý!
        bool result = QGuiApplication::notify(receiver, event);
        
        // Check nếu event xử lý chậm (> 1ms)
        qint64 elapsed = timer.nsecsElapsed();
        if (elapsed > 1000000) { // 1ms = 1,000,000 nanoseconds
            qWarning() << "⚠️ Slow event processing:" << elapsed / 1000000.0 << "ms"
                      << "Type:" << event->type();
            // Trong production: có thể log to file, send metrics, etc.
        }
        
        return result; // true = event handled, false = event ignored
    }
};

// ============================================================================
// Example 2: Context Property Object với Signals
// ============================================================================
//
// MỤC ĐÍCH: Hiểu cách expose C++ objects to QML hiệu quả
//
// GIẢI THÍCH:
// - Cách TỐT NHẤT để share data giữa C++ và QML
// - Sử dụng Q_PROPERTY với NOTIFY signal → QML tự động update
// - Q_INVOKABLE methods → có thể gọi từ QML
// - Không dùng setContextProperty với QVariant (chậm!)
//
// Q_PROPERTY ANATOMY:
// Q_PROPERTY(Type name READ getter WRITE setter NOTIFY signal)
//            ↓    ↓    ↓          ↓           ↓
//         int  count count()  setCount()  countChanged()
//
// CÁCH HOẠT ĐỘNG:
// 1. C++ property thay đổi → emit NOTIFY signal
// 2. QML binding subscribe to signal
// 3. Signal emitted → QML re-evaluate binding
// 4. UI tự động update!
//
// HỌC ĐƯỢC GÌ:
// - Meta-object system (Q_OBJECT, Q_PROPERTY)
// - Signal/slot mechanism
// - Property binding system
// - C++ ↔ QML communication
// ============================================================================

class DataModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    
public:
    explicit DataModel(QObject *parent = nullptr) 
        : QObject(parent)
        , m_count(0)
        , m_message("Initial message")
    {
    }
    
    // GETTER: Simple, chỉ return value
    int count() const { return m_count; }
    
    // SETTER: Quan trọng! Phải emit signal để QML update
    void setCount(int count) {
        // BEST PRACTICE: Check if value thực sự thay đổi
        // Tránh emit signal không cần thiết → tối ưu performance
        if (m_count != count) {
            m_count = count;
            qDebug() << "DataModel::count changed to:" << count;
            
            // CRITICAL: Emit NOTIFY signal
            // QML bindings subscribe to signal này
            // Khi signal emit → QML re-evaluate bindings → UI update!
            emit countChanged();
        }
    }
    
    QString message() const { return m_message; }
    
    void setMessage(const QString &msg) {
        if (m_message != msg) {
            m_message = msg;
            qDebug() << "DataModel::message changed to:" << msg;
            emit messageChanged(); // Tương tự như trên
        }
    }
    
    // Q_INVOKABLE: Cho phép gọi method từ QML
    // Syntax trong QML: dataModel.increment()
    Q_INVOKABLE void increment() {
        setCount(m_count + 1); // Reuse setter → signal auto-emitted
    }
    
    // Method với parameters: QML có thể pass arguments
    // QML: dataModel.formatMessage("Prefix")
    Q_INVOKABLE QString formatMessage(const QString &prefix) {
        return prefix + ": " + m_message;
    }
    
signals:
    void countChanged();
    void messageChanged();
    
private:
    int m_count;
    QString m_message;
};

// ============================================================================
// Example 3: Custom QQmlApplicationEngine với Loading Callbacks
// ============================================================================
//
// MỤC ĐÍCH: Debug và monitor QML loading process
//
// GIẢI THÍCH:
// - QQmlApplicationEngine kế thừa QQmlEngine
// - Emit signals khi loading QML:
//   + objectCreated() - khi QML object được tạo
//   + warnings() - khi có QML errors/warnings
// - Connect to signals để track loading progress
//
// QML LOADING FLOW:
// load(url) → Load file → Parse → Compile → Create object
//                ↓          ↓        ↓          ↓
//              Read QML   Build AST  Bytecode  C++ objects
//
// TẠI SAO CẦN:
// - Debug loading failures
// - Measure load time (performance)
// - Track which components loaded
// - Catch QML warnings early
//
// HỌC ĐƯỢC GÌ:
// - QML loading pipeline
// - Signal-based monitoring
// - Performance measurement
// - Error handling in QML
// ============================================================================

class InstrumentedEngine : public QQmlApplicationEngine {
    Q_OBJECT
    
public:
    explicit InstrumentedEngine(QObject *parent = nullptr)
        : QQmlApplicationEngine(parent)
    {
        // SETUP MONITORING: Connect to engine signals
        // Signal này emit khi QML object được tạo (hoặc fail)
        connect(this, &QQmlApplicationEngine::objectCreated,
                this, &InstrumentedEngine::onObjectCreated);
        
        // Signal này emit khi có QML warnings/errors
        connect(this, &QQmlApplicationEngine::warnings,
                this, &InstrumentedEngine::onWarnings);
    }
    
    void loadWithTiming(const QUrl &url) {
        qDebug() << "=== Loading QML:" << url;
        
        // Start timer TRƯỚC khi load
        QElapsedTimer timer;
        timer.start();
        
        // Load QML file
        // Flow: Load file → Parse → Compile → Create objects
        // Trong quá trình này, signals trên sẽ được emit
        load(url);
        
        // Measure total time
        qint64 elapsed = timer.elapsed();
        qDebug() << "=== Loading completed in:" << elapsed << "ms";
        
        // TYPICAL TIMES:
        // - Simple QML: < 100ms
        // - Complex app: 200-500ms
        // - Very complex: > 1s (consider optimization!)
    }
    
private slots:
    void onObjectCreated(QObject *object, const QUrl &url) {
        // CALLBACK: Được gọi SAU KHI load() hoàn thành
        // object != nullptr → success
        // object == nullptr → failed (check warnings!)
        
        if (object) {
            qDebug() << "✓ Object created successfully"
                     << "\n  URL:" << url  // Source QML file
                     << "\n  Type:" << object->metaObject()->className() // C++ class (e.g., QQuickWindow)
                     << "\n  ObjectName:" << object->objectName(); // QML objectName property
            
            // Có thể làm thêm:
            // - Store object reference
            // - Connect to object's signals
            // - Initialize object state
        } else {
            qWarning() << "✗ Failed to create object from:" << url;
            // Check warnings signal để biết lý do!
        }
    }
    
    void onWarnings(const QList<QQmlError> &warnings) {
        // CALLBACK: Được gọi khi có QML errors/warnings
        // Có thể có nhiều warnings cùng lúc
        
        qWarning() << "⚠ QML Warnings:";
        for (const QQmlError &warning : warnings) {
            // QQmlError chứa:
            // - File, line, column
            // - Error message
            // - Error description
            qWarning() << "  " << warning.toString();
        }
        
        // COMMON ERRORS:
        // - Unknown property
        // - Type not found
        // - Syntax error
        // - Binding loop
    }
};

// ============================================================================
// Example 4: Multiple Contexts - Parent/Child Hierarchy
// ============================================================================
//
// MỤC ĐÍCH: Hiểu context hierarchy và property lookup
//
// GIẢI THÍCH:
// - QQmlContext có thể có parent-child relationship
// - Property lookup: current → parent → grandparent → root
// - Child context có thể "shadow" parent properties
//
// CONTEXT TREE:
//     Root Context (globalValue, version)
//          ↓ parent
//     Child Context (localValue, globalValue-overridden)
//
// PROPERTY LOOKUP từ child:
// 1. Tìm "localValue" → Found trong child ✓
// 2. Tìm "globalValue" → Found trong child (shadowed!) ✓
// 3. Tìm "version" → Not in child, tìm parent → Found ✓
//
// USE CASES:
// - Component-specific data (không ảnh hưởng global)
// - Override global properties locally
// - Scope isolation
//
// HỌC ĐƯỢC GÌ:
// - Context hierarchy
// - Property resolution chain
// - Scope shadowing
// - When to use multiple contexts
// ============================================================================

void demonstrateContextHierarchy(QQmlApplicationEngine *engine) {
    qDebug() << "\n=== Context Hierarchy Demo ===";
    
    // ROOT CONTEXT: Mọi engine đều có root context
    // Properties ở đây accessible từ EVERYWHERE trong QML
    QQmlContext *rootCtx = engine->rootContext();
    rootCtx->setContextProperty("globalValue", "I'm global");
    rootCtx->setContextProperty("version", "1.0.0");
    
    // CHILD CONTEXT: Tạo context con
    // Constructor: QQmlContext(parent, owner)
    // - parent: context cha (cho property lookup)
    // - owner: QObject owner (cho memory management)
    QQmlContext *childCtx = new QQmlContext(rootCtx, engine);
    childCtx->setContextProperty("localValue", "I'm local");
    
    // SHADOWING: Child có thể override parent properties
    childCtx->setContextProperty("globalValue", "I override global");
    
    // VERIFY: Test property lookup
    qDebug() << "Root context 'globalValue':" 
             << rootCtx->contextProperty("globalValue"); // "I'm global"
    
    qDebug() << "Child context 'globalValue':"
             << childCtx->contextProperty("globalValue")  // "I override global" (shadowed!)
             << "\nChild context 'version':"
             << childCtx->contextProperty("version");     // "1.0.0" (inherited from root)
    
    // LOOKUP ALGORITHM từ QML trong childCtx:
    // Text { text: someProperty }
    //   1. Tìm trong object properties (Text.text, Text.width, etc.)
    //   2. Tìm trong childCtx properties → Found "localValue", "globalValue"
    //   3. Nếu không có, tìm trong rootCtx → Found "version"
    //   4. Nếu vẫn không có → ReferenceError!
    
    // USE CASE: Component-specific data
    // Ví dụ: Dialog component có own context với dialogData
    // Không ảnh hưởng global scope
}

// ============================================================================
// Example 5: Performance Measurement - Context Property Updates
// ============================================================================
//
// MỤC ĐÍCH: So sánh performance của 2 cách expose data to QML
//
// GIẢI THÍCH:
// Cách 1: setContextProperty(name, QVariant)
//   - Mỗi lần update phải notify QML engine
//   - QVariant boxing/unboxing overhead
//   - CHẬM! ❌
//
// Cách 2: setContextProperty(name, QObject*)
//   - Direct property access via meta-object
//   - Automatic NOTIFY signal
//   - NHANH! ✓
//
// BENCHMARK:
// Test 10,000 updates và so sánh thời gian
//
// KẾT QUẢ EXPECTED:
// QObject approach nhanh hơn 5-10x!
//
// BEST PRACTICE:
// ✓ Dùng QObject với Q_PROPERTY
// ✗ Tránh setContextProperty với primitives
//
// HỌC ĐƯỢC GÌ:
// - Performance implications
// - QVariant overhead
// - Meta-object efficiency
// - Profiling techniques
// ============================================================================

void measureContextPropertyPerformance() {
    qDebug() << "\n=== Performance Measurement ===";
    
    // Create minimal app (không cần UI cho benchmark)
    QGuiApplication app(0, nullptr);
    QQmlApplicationEngine engine;
    
    const int iterations = 10000;
    QElapsedTimer timer;
    
    // ========================================
    // TEST 1: setContextProperty với QVariant
    // ========================================
    // Mỗi call phải:
    // 1. Box value vào QVariant
    // 2. Notify QML engine về change
    // 3. Update internal structures
    // → OVERHEAD cao!
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        engine.rootContext()->setContextProperty("testValue", i);
    }
    qint64 elapsed1 = timer.nsecsElapsed();
    qDebug() << "setContextProperty(QVariant) x" << iterations << ":"
             << elapsed1 / 1000.0 << "µs total,"
             << elapsed1 / iterations / 1000.0 << "µs per call";
    
    // ========================================
    // TEST 2: QObject property (RECOMMENDED!)
    // ========================================
    // Set QObject 1 LẦN, sau đó update properties
    // QML bindings tự động track qua NOTIFY signals
    // → EFFICIENT!
    
    DataModel model;
    engine.rootContext()->setContextProperty("model", &model);
    
    timer.restart();
    for (int i = 0; i < iterations; ++i) {
        model.setCount(i);  // Direct property change qua meta-object
        // Chỉ emit signal, không phải notify engine!
    }
    qint64 elapsed2 = timer.nsecsElapsed();
    qDebug() << "QObject property change x" << iterations << ":"
             << elapsed2 / 1000.0 << "µs total,"
             << elapsed2 / iterations / 1000.0 << "µs per call";
    
    // RESULTS: QObject thường nhanh hơn 5-10x!
    qDebug() << "→ QObject approach is" 
             << (double)elapsed1 / elapsed2 << "x faster";
    
    // LESSON: LUÔN dùng QObject với Q_PROPERTY
    // KHÔNG dùng setContextProperty nhiều lần cho same property!
}

// ============================================================================
// Example 6: Event Filter - Intercept và Modify Events
// ============================================================================
//
// MỤC ĐÍCH: Intercept events TRƯỚC KHI chúng đến target object
//
// GIẢI THÍCH:
// - eventFilter() được gọi TRƯỚC object->event()
// - Return true → event bị block (không đến target)
// - Return false → event tiếp tục đến target
//
// EVENT FLOW với filter:
// OS → QGuiApplication::notify()
//   → eventFilter() ← (điểm này!)
//      → true? STOP ✋
//      → false? Continue
//         → object->event()
//            → mousePressEvent() etc.
//
// USE CASES:
// - Logging/monitoring events
// - Block specific events (như Escape key)
// - Modify events before delivery
// - Global shortcuts
// - Input validation
//
// INSTALL FILTER:
// object->installEventFilter(filter); // Trên specific object
// app->installEventFilter(filter);    // Global (tất cả events!)
//
// HỌC ĐƯỢC GÌ:
// - Event filtering mechanism
// - Event interception
// - Global vs local filters
// - Event blocking
// ============================================================================

class EventLogger : public QObject {
    Q_OBJECT
    
public:
    explicit EventLogger(QObject *parent = nullptr) : QObject(parent) {}
    
protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        // IMPORTANT: eventFilter được gọi TRƯỚC watched->event()
        // Đây là cơ hội để intercept hoặc block events
        
        // ========================================
        // MOUSE EVENTS
        // ========================================
        if (event->type() == QEvent::MouseButtonPress) {
            // Cast to specific event type để access details
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            qDebug() << "🖱️ Mouse press at:" << mouseEvent->pos()
                     << "button:" << mouseEvent->button()
                     << "on object:" << watched->objectName();
            
            // DECISION:
            // return true  → Event BLOCKED, watched không nhận được
            // return false → Event continues to watched object
            return false; // Allow event
        }
        
        // ========================================
        // KEYBOARD EVENTS
        // ========================================
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            qDebug() << "⌨️ Key pressed:" << keyEvent->text()
                     << "Key code:" << keyEvent->key()
                     << "Modifiers:" << keyEvent->modifiers();
            
            // EXAMPLE: Block Escape key (common use case)
            if (keyEvent->key() == Qt::Key_Escape) {
                qDebug() << "  → Escape key blocked!";
                return true;  // FILTER (block) this event
            }
            
            // Could also implement:
            // - Global shortcuts (Ctrl+S, etc.)
            // - Input validation (only allow numbers)
            // - Custom key handling
        }
        
        // DEFAULT: Pass to base class
        // Base class typically returns false (allow event)
        return QObject::eventFilter(watched, event);
    }
};

// ============================================================================
// Example 7: QML Component Dynamic Loading
// ============================================================================
//
// MỤC ĐÍCH: Load QML components động tại runtime
//
// GIẢI THÍCH:
// - QQmlComponent: Low-level interface để load QML
// - Có thể load từ file, string, hoặc URL
// - Create multiple instances từ 1 component
// - Cho phép lazy loading (chỉ load khi cần)
//
// QQmlComponent LIFECYCLE:
// 1. Create component với URL/file
// 2. Check isError() → có lỗi không?
// 3. create() → tạo instance (có thể tạo nhiều lần!)
// 4. Object ready to use
//
// ENGINE vs COMPONENT:
// QQmlApplicationEngine:
//   - High-level, load root component
//   - Automatic lifetime management
//   - One root window
//
// QQmlComponent:
//   - Low-level, flexible
//   - Manual lifetime management
//   - Multiple instances
//   - Dynamic loading
//
// USE CASES:
// - Lazy loading (load khi cần)
// - Dynamic UI (create objects on demand)
// - Reusable components
// - Plugin systems
//
// HỌC ĐƯỢC GÌ:
// - QQmlComponent API
// - Dynamic object creation
// - Error handling
// - Memory management
// ============================================================================

class DynamicLoader : public QObject {
    Q_OBJECT
    
public:
    explicit DynamicLoader(QQmlApplicationEngine *engine, QObject *parent = nullptr)
        : QObject(parent)
        , m_engine(engine)
    {
    }
    
    Q_INVOKABLE void loadComponent(const QString &qmlFile) {
        // Q_INVOKABLE → có thể gọi từ QML
        // QML: dynamicLoader.loadComponent("qrc:/MyComponent.qml")
        
        qDebug() << "Loading component dynamically:" << qmlFile;
        
        // CREATE COMPONENT
        // QQmlComponent = low-level interface để load QML
        // Constructor: QQmlComponent(engine, url)
        QQmlComponent component(m_engine, QUrl(qmlFile));
        
        // CHECK STATUS
        // Component có thể có 3 states:
        // - Loading (async load in progress)
        // - Ready (loaded successfully)
        // - Error (failed to load)
        if (component.isError()) {
            qWarning() << "❌ Component has errors:";
            for (const QQmlError &error : component.errors()) {
                qWarning() << "  " << error.toString();
            }
            return;
        }
        
        // CREATE INSTANCE
        // create() có thể gọi NHIỀU lần → tạo nhiều instances!
        // Mỗi lần gọi = 1 object mới
        QObject *object = component.create();
        if (object) {
            qDebug() << "✓ Component created:" << object->metaObject()->className();
            
            // MEMORY MANAGEMENT
            // Object không có parent → phải track và delete manually!
            m_dynamicObjects.append(object);
            
            // ALTERNATIVE: Set parent để auto-cleanup
            // object->setParent(someParent);
        } else {
            qWarning() << "✗ Failed to create component";
            // Check component.errors() để biết lý do
        }
    }
    
    Q_INVOKABLE void cleanupDynamicObjects() {
        // Clean up tất cả dynamic objects
        qDebug() << "Cleaning up" << m_dynamicObjects.size() << "dynamic objects";
        
        // qDeleteAll: Qt helper để delete all items trong container
        qDeleteAll(m_dynamicObjects);
        m_dynamicObjects.clear();
        
        // IMPORTANT: Nếu objects có parent, đừng delete manually!
        // Parent sẽ tự động delete children
    }
    
private:
    QQmlApplicationEngine *m_engine;
    QList<QObject*> m_dynamicObjects;
};

// ============================================================================
// Example 8: Main Function - Tích Hợp Tất Cả
// ============================================================================
//
// MỤC ĐÍCH: Demo COMPLETE Qt/QML application với tất cả concepts
//
// GIẢI THÍCH:
// Ứng dụng này tích hợp TẤT CẢ các examples trước:
// ✓ Custom QGuiApplication (Example 1)
// ✓ DataModel với properties (Example 2)
// ✓ Instrumented engine (Example 3)
// ✓ Context hierarchy (Example 4)
// ✓ Event filters (Example 6)
// ✓ Dynamic loader (Example 7)
//
// APPLICATION FLOW:
// 1. Create DebugApplication
//    → Override notify() để log events
//
// 2. Create InstrumentedEngine
//    → Monitor QML loading
//
// 3. Setup context properties
//    → Simple values: appVersion, buildDate
//    → QObject: dataModel (với NOTIFY signals)
//    → Services: dynamicLoader
//
// 4. Install event filter
//    → Log mouse, keyboard events globally
//
// 5. Load QML
//    → Parse, compile, create objects
//
// 6. Enter event loop (app.exec())
//    → Process events until quit
//
// 7. QTimer demo
//    → Update dataModel after 2 seconds
//    → Watch QML automatically update!
//
// COMMAND LINE OPTIONS:
// --perf : Chạy performance benchmark (không load UI)
//
// HỌC ĐƯỢC GÌ:
// - Complete application structure
// - Integration của tất cả components
// - Initialization order (quan trọng!)
// - Best practices
// - Real-world patterns
// ============================================================================

int main(int argc, char *argv[])
{
    qDebug() << "=== Qt Core Classes Deep Dive Examples ===\n";
    
    // ========================================
    // STEP 1: Create QGuiApplication
    // ========================================
    // QUAN TRỌNG: Phải tạo QGuiApplication TRƯỚC bất kỳ Qt GUI code nào
    // Application object:
    // - Initialize Qt platform integration
    // - Setup event loop
    // - Manage application-wide settings
    DebugApplication app(argc, argv); // Using our custom class!
    
    // ========================================
    // COMMAND LINE: Performance benchmark
    // ========================================
    // Run: ./app --perf
    if (argc > 1 && QString(argv[1]) == "--perf") {
        measureContextPropertyPerformance();
        return 0; // Exit without UI
    }
    
    // ========================================
    // STEP 2: Create QML Engine
    // ========================================
    InstrumentedEngine engine; // Custom engine với monitoring
    
    // ========================================
    // STEP 3: Setup Context Properties
    // ========================================
    // TIMING: Phải set TRƯỚC khi load QML!
    // QML code sẽ reference các properties này
    
    qDebug() << "\n1. Setting up context properties...";
    
    // SIMPLE VALUES: OK cho constants
    // NOT recommended cho frequently-changing values
    engine.rootContext()->setContextProperty("appVersion", "1.0.0");
    engine.rootContext()->setContextProperty("buildDate", __DATE__);
    
    // QOBJECT: BEST PRACTICE!
    // Properties tự động sync với QML qua NOTIFY signals
    DataModel dataModel;
    engine.rootContext()->setContextProperty("dataModel", &dataModel);
    // ⚠️ WARNING: Engine KHÔNG own dataModel!
    //    Phải ensure dataModel outlives engine
    
    // SERVICE OBJECTS: Expose functionality to QML
    DynamicLoader loader(&engine);
    engine.rootContext()->setContextProperty("dynamicLoader", &loader);
    
    // DEMO: Context hierarchy
    demonstrateContextHierarchy(&engine);
    
    // ========================================
    // STEP 4: Install Event Filters
    // ========================================
    qDebug() << "\n2. Installing event filter...";
    EventLogger eventLogger;
    app.installEventFilter(&eventLogger);
    // Global filter → nhận TẤT CẢ events trong app
    // Cẩn thận với performance!
    
    // ========================================
    // STEP 5: Load QML
    // ========================================
    qDebug() << "\n3. Loading QML...";
    engine.loadWithTiming(QUrl(QStringLiteral("qrc:/main.qml")));
    // Flow: Load file → Parse → Compile → Create objects
    // objectCreated signal sẽ emit khi done
    
    // CHECK: Verify object created successfully
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "❌ No root objects - exiting";
        return -1;
    }
    
    // ========================================
    // STEP 6: Setup Runtime Updates
    // ========================================
    // QTimer: Schedule future action
    // Demo: Update model sau 2 giây
    QTimer::singleShot(2000, [&]() {
        qDebug() << "\n=== Updating data model ===";
        dataModel.setCount(42);
        dataModel.setMessage("Updated from C++");
        // → countChanged() signal emitted
        //   → QML bindings re-evaluated
        //     → UI automatically updates!
    });
    
    // ========================================
    // STEP 7: Enter Event Loop
    // ========================================
    qDebug() << "\n4. Entering event loop...\n";
    
    // EVENT LOOP:
    // while (not quit) {
    //   1. Wait for events (OS, timers, posted events)
    //   2. Dispatch events via notify()
    //   3. Process GUI updates
    //   4. Render frames
    // }
    return app.exec(); // Blocks until quit() called
    
    // CLEANUP: Automatic!
    // - app destructor deletes children
    // - engine destructor deletes QML objects
    // - dataModel, loader = stack objects, auto-deleted
}

// ============================================================================
// Example QML File (main.qml) để dùng với code trên
// ============================================================================

/*
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    visible: true
    width: 640
    height: 480
    title: "Qt Core Classes Deep Dive - Example App"
    
    Column {
        anchors.centerIn: parent
        spacing: 20
        
        // Context properties từ C++
        Text {
            text: "App Version: " + appVersion
            font.pixelSize: 16
        }
        
        Text {
            text: "Build Date: " + buildDate
            font.pixelSize: 16
        }
        
        // QObject properties với binding
        Text {
            text: "Data Model Count: " + dataModel.count
            font.pixelSize: 20
            font.bold: true
            color: "blue"
        }
        
        Text {
            text: "Message: " + dataModel.message
            font.pixelSize: 16
        }
        
        // Button để test invokable methods
        Button {
            text: "Increment Count"
            onClicked: {
                dataModel.increment()
                console.log("Count incremented from QML")
            }
        }
        
        Button {
            text: "Change Message"
            onClicked: {
                dataModel.message = "Updated from QML at " + new Date().toLocaleTimeString()
            }
        }
        
        // Test dynamic loading
        Button {
            text: "Load Dynamic Component"
            onClicked: {
                dynamicLoader.loadComponent("qrc:/DynamicComponent.qml")
            }
        }
        
        // Test formatMessage invokable
        Text {
            text: dataModel.formatMessage("Formatted")
            font.pixelSize: 14
            color: "green"
        }
    }
    
    // Component để test event handling
    MouseArea {
        anchors.fill: parent
        z: -1  // Behind other items
        onPressed: {
            console.log("Window MouseArea pressed at:", mouse.x, mouse.y)
        }
    }
    
    // Keyboard event handling
    focus: true
    Keys.onPressed: {
        console.log("Key pressed in QML:", event.key, event.text)
        if (event.key === Qt.Key_Space) {
            dataModel.increment()
            event.accepted = true
        }
    }
}
*/

#include "Code-Examples.moc"
