# Tóm Tắt Các Code Examples

## 📚 Quick Reference

### Example 1: Custom QGuiApplication với Event Logging
**File:** Code-Examples.cpp, line 17-88

**Mục đích:** Hiểu event flow và monitoring

**Key Points:**
- Override `notify()` để intercept TẤT CẢ events
- Log event types, receivers, và performance
- Measure event processing time
- Detect slow events (> 1ms)

**Khi nào dùng:**
- Debug event issues
- Monitor application performance
- Implement custom event handling
- Track user interactions

**Code snippet:**
```cpp
class DebugApplication : public QGuiApplication {
    bool notify(QObject *receiver, QEvent *event) override {
        // Log events
        // Measure time
        return QGuiApplication::notify(receiver, event);
    }
};
```

---

### Example 2: DataModel với Q_PROPERTY
**File:** Code-Examples.cpp, line 90-179

**Mục đích:** Best practice cho C++ ↔ QML communication

**Key Points:**
- Q_PROPERTY với READ, WRITE, NOTIFY
- NOTIFY signal → QML auto-update
- Q_INVOKABLE methods → callable from QML
- Check value before emit (performance!)

**Khi nào dùng:**
- Expose data to QML (RECOMMENDED!)
- Dynamic UI updates
- Two-way data binding
- Any time you need C++ ↔ QML sync

**Code snippet:**
```cpp
class DataModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
signals:
    void countChanged();
public:
    void setCount(int c) {
        if (m_count != c) {
            m_count = c;
            emit countChanged(); // ← QML bindings update!
        }
    }
};
```

**QML usage:**
```qml
Text {
    text: "Count: " + dataModel.count  // Auto-updates!
}
Button {
    onClicked: dataModel.increment()   // Call C++ method
}
```

---

### Example 3: Instrumented QQmlApplicationEngine
**File:** Code-Examples.cpp, line 181-293

**Mục đích:** Monitor QML loading process

**Key Points:**
- Connect to `objectCreated()` signal
- Connect to `warnings()` signal
- Measure load time
- Debug loading failures

**Khi nào dùng:**
- Debug QML loading issues
- Performance profiling
- Track component creation
- Production error monitoring

**Code snippet:**
```cpp
class InstrumentedEngine : public QQmlApplicationEngine {
    InstrumentedEngine() {
        connect(this, &QQmlApplicationEngine::objectCreated,
                this, &InstrumentedEngine::onObjectCreated);
        connect(this, &QQmlApplicationEngine::warnings,
                this, &InstrumentedEngine::onWarnings);
    }
    
    void loadWithTiming(const QUrl &url) {
        QElapsedTimer timer;
        timer.start();
        load(url);
        qDebug() << "Loaded in:" << timer.elapsed() << "ms";
    }
};
```

---

### Example 4: Context Hierarchy
**File:** Code-Examples.cpp, line 295-366

**Mục đích:** Hiểu property lookup và scoping

**Key Points:**
- Root context → global properties
- Child contexts → local properties
- Property shadowing (override parent)
- Lookup chain: child → parent → root

**Khi nào dùng:**
- Component-specific data
- Override global properties locally
- Scope isolation
- Complex applications

**Visualization:**
```
Root Context
├─ globalValue: "I'm global"
└─ version: "1.0.0"
    ↓ parent
Child Context
├─ localValue: "I'm local"
└─ globalValue: "I override global"  ← Shadows parent!

Lookup từ child:
1. localValue → Found in child ✓
2. globalValue → Found in child (shadowed) ✓
3. version → Not in child, check parent → Found ✓
```

---

### Example 5: Performance Benchmark
**File:** Code-Examples.cpp, line 368-456

**Mục đích:** So sánh performance approaches

**Key Points:**
- setContextProperty(QVariant) → SLOW ❌
- QObject properties → FAST ✓
- Benchmark 10,000 iterations
- QObject approach ~5-10x faster!

**Khi nào dùng:**
- Before choosing context property approach
- Performance-critical applications
- Frequent property updates
- Learning best practices

**Results:**
```
setContextProperty(QVariant):  500µs total, 0.05µs/call
QObject property change:        50µs total, 0.005µs/call
→ QObject approach is 10x faster!
```

**Lesson:**
✅ **DO:** Use QObject with Q_PROPERTY  
❌ **DON'T:** Call setContextProperty repeatedly

---

### Example 6: Event Filter
**File:** Code-Examples.cpp, line 458-547

**Mục đích:** Intercept events before delivery

**Key Points:**
- `eventFilter()` called BEFORE `event()`
- Return true → block event
- Return false → allow event
- Can install globally or per-object

**Khi nào dùng:**
- Global shortcuts
- Input validation
- Event logging
- Block specific events

**Event flow:**
```
OS Event
  ↓
QGuiApplication::notify()
  ↓
eventFilter()  ← YOU ARE HERE
  ↓ true? → STOP
  ↓ false? → Continue
  ↓
object->event()
  ↓
mousePressEvent(), etc.
```

**Code snippet:**
```cpp
class EventLogger : public QObject {
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(event);
            if (ke->key() == Qt::Key_Escape)
                return true;  // Block Escape!
        }
        return false; // Allow other events
    }
};

// Install:
app.installEventFilter(&logger);  // Global
// or:
widget->installEventFilter(&logger);  // Per-object
```

---

### Example 7: Dynamic Component Loading
**File:** Code-Examples.cpp, line 549-660

**Mục đích:** Load QML components at runtime

**Key Points:**
- QQmlComponent = low-level loader
- Can create multiple instances
- Check `isError()` before `create()`
- Manual memory management!

**Khi nào dùng:**
- Lazy loading
- Dynamic UI creation
- Plugin systems
- Reusable components

**Comparison:**
```
QQmlApplicationEngine:
✓ High-level
✓ Auto-lifetime management
✓ One root component
✗ Less flexible

QQmlComponent:
✓ Low-level control
✓ Multiple instances
✓ Very flexible
✗ Manual memory management
```

**Code snippet:**
```cpp
QQmlComponent component(engine, QUrl("qrc:/MyComponent.qml"));
if (component.isError()) {
    // Handle errors
    return;
}

// Create instance
QObject *obj1 = component.create();  // Instance 1
QObject *obj2 = component.create();  // Instance 2
// Same component, different instances!

// Don't forget to delete!
delete obj1;
delete obj2;
```

---

### Example 8: Complete Application
**File:** Code-Examples.cpp, line 662-823

**Mục đích:** Integration của TẤT CẢ concepts

**Application Flow:**
```
1. Create QGuiApplication
   ↓
2. Create QQmlApplicationEngine
   ↓
3. Setup Context Properties
   - Constants: appVersion, buildDate
   - QObject: dataModel
   - Services: dynamicLoader
   ↓
4. Install Event Filters
   ↓
5. Load QML
   - Parse → Compile → Create objects
   ↓
6. Setup Runtime Actions
   - QTimer for updates
   ↓
7. Enter Event Loop
   - Process events until quit
```

**Key Insights:**
- **Order matters!** Context properties BEFORE load()
- QObject lifetime must outlive engine
- Event filters affect ALL events (performance!)
- app.exec() blocks until quit()

**Command line:**
```bash
./app           # Normal UI mode
./app --perf    # Performance benchmark only
```

---

## 🎯 Cheat Sheet: Khi Nào Dùng Example Nào?

| Problem | Example | Why |
|---------|---------|-----|
| UI không update khi C++ data thay đổi | #2 (DataModel) | Need NOTIFY signal |
| Event không đến target object | #1, #6 (Event logging/filtering) | Debug event flow |
| QML loading chậm | #3 (Instrumented engine) | Measure load time |
| QML error khó debug | #3 (Instrumented engine) | Connect to warnings() |
| Cần properties scope riêng | #4 (Context hierarchy) | Use child contexts |
| Performance kém | #5 (Benchmark) | Compare approaches |
| Cần global shortcuts | #6 (Event filter) | Intercept keyboard |
| Load components động | #7 (Dynamic loading) | Use QQmlComponent |
| Không biết bắt đầu từ đâu | #8 (Complete app) | See full integration |

---

## 💡 Best Practices Summary

### ✅ DO:
1. **Use QObject với Q_PROPERTY** cho C++ ↔ QML
2. **Emit NOTIFY signals** khi properties thay đổi
3. **Check value changed** trước khi emit (performance)
4. **Set context properties BEFORE load()**
5. **Use QElapsedTimer** để measure performance
6. **Connect to engine signals** để debug
7. **Install event filters carefully** (performance impact)
8. **Manage memory properly** với dynamic objects

### ❌ DON'T:
1. **Don't call setContextProperty repeatedly** với QVariant
2. **Don't forget NOTIFY signal** trong Q_PROPERTY
3. **Don't block event loop** với slow operations
4. **Don't install global filters** without good reason
5. **Don't create objects without tracking** them
6. **Don't override notify()** unless necessary
7. **Don't set context properties AFTER load()**
8. **Don't assume object lifetime** (check ownership!)

---

## 🔗 Cross-References

**Related Documents:**
- Main guide: `Qt-Core-Classes-Deep-Dive.md`
- Architecture: `Architecture-Diagrams.md`
- Debugging: `Debugging-Recipes.md`
- Full code: `Code-Examples.cpp`

**Qt Documentation:**
- QGuiApplication: https://doc.qt.io/qt-6/qguiapplication.html
- QQmlApplicationEngine: https://doc.qt.io/qt-6/qqmlapplicationengine.html
- QQmlContext: https://doc.qt.io/qt-6/qqmlcontext.html
- Q_PROPERTY: https://doc.qt.io/qt-6/properties.html

---

## 📝 Next Steps

1. **Read the code** với comments
2. **Try modifying** examples
3. **Build and run** examples
4. **Experiment** với different scenarios
5. **Debug** với breakpoints
6. **Measure** performance
7. **Apply** to your projects!

**Happy coding!** 🚀
