# Quick Reference: Qt Source Code Reading Guide

## 🎯 Mục Tiêu Nhanh

### QGuiApplication
- **Vai trò**: Event loop manager, platform abstraction
- **File chính**: `qtbase/src/gui/kernel/qguiapplication.{h,cpp}`
- **Đọc trước**: Constructor → `exec()` → `notify()`

### QQmlApplicationEngine  
- **Vai trò**: QML loader, component creator
- **File chính**: `qtdeclarative/src/qml/qml/qqmlapplicationengine.{h,cpp}`
- **Đọc trước**: Constructor → `load()` → `rootContext()`

### QQmlContext
- **Vai trò**: Scope provider, object exposure
- **File chính**: `qtdeclarative/src/qml/qml/qqmlcontext.{h,cpp}`
- **Đọc trước**: Constructor → `setContextProperty()` → Property resolution

---

## 📋 Checklist Đọc Mã Nhanh

### Bước 1: Đọc Header Files (15 phút)
- [ ] `qguiapplication.h` - Class declaration, public API
- [ ] `qqmlapplicationengine.h` - Inheritance, key methods
- [ ] `qqmlcontext.h` - Context API, property methods

### Bước 2: Trace Constructors (30 phút)
- [ ] `QGuiApplication::QGuiApplication()` - Platform init
- [ ] `QQmlApplicationEngine::QQmlApplicationEngine()` - Engine setup
- [ ] `QQmlContext::QQmlContext()` - Context creation

### Bước 3: Key Methods (45 phút)
- [ ] `QGuiApplication::exec()` - Event loop
- [ ] `QQmlApplicationEngine::load()` - QML loading
- [ ] `QQmlContext::setContextProperty()` - Property exposure

### Bước 4: Debug và Trace (30 phút)
- [ ] Set breakpoints tại constructors
- [ ] Trace execution flow
- [ ] Monitor object creation

---

## 🔍 Các Method Quan Trọng

### QGuiApplication

```cpp
// Constructor
QGuiApplication(int &argc, char **argv)

// Event loop
int exec()

// Event processing
bool notify(QObject *receiver, QEvent *event)

// Platform info
QString platformName() const
```

### QQmlApplicationEngine

```cpp
// Constructor
QQmlApplicationEngine(QObject *parent = nullptr)

// Load QML
void load(const QUrl &url)
void loadData(const QByteArray &data, const QUrl &url)

// Context access
QQmlContext *rootContext() const

// Signals
void objectCreated(QObject *object, const QUrl &url)
```

### QQmlContext

```cpp
// Constructor
QQmlContext(QQmlEngine *engine, QObject *parent = nullptr)

// Property exposure
void setContextProperty(const QString &name, QObject *value)
void setContextProperty(const QString &name, const QVariant &value)
void setContextObject(QObject *object)

// URL resolution
QUrl resolveUrl(const QUrl &url) const

// Context info
QQmlContext *parentContext() const
QQmlEngine *engine() const
QUrl baseUrl() const
```

---

## 🗺️ Execution Flow

```
1. main()
   ↓
2. QGuiApplication app(argc, argv)
   ├── Initialize platform plugin
   ├── Setup event dispatcher
   └── Create application instance
   ↓
3. QQmlApplicationEngine engine
   ├── Create QQmlEngine
   ├── Create root QQmlContext
   └── Initialize component cache
   ↓
4. engine.rootContext()->setContextProperty(...)
   ├── Store property in context
   └── Make available to QML
   ↓
5. engine.load("main.qml")
   ├── Parse QML file
   ├── Create QQmlComponent
   ├── Instantiate component with root context
   └── Create QML object tree
   ↓
6. app.exec()
   ├── Start event loop
   ├── Process events (mouse, keyboard, paint, timer)
   └── Continue until quit()
   ↓
7. app.quit() or window.close()
   └── Exit event loop
   ↓
8. return app.exec()
```

---

## 🛠️ Debugging Commands

### Enable Debug Output

```cpp
// QML debug
QLoggingCategory::setFilterRules("qt.qml.debug=true");
QLoggingCategory::setFilterRules("qt.qml.compiler=true");

// Platform debug
QLoggingCategory::setFilterRules("qt.qpa.*=true");

// All Qt debug
QLoggingCategory::setFilterRules("*.debug=true");
```

### GDB Commands

```bash
# Set breakpoints
(gdb) break QGuiApplication::QGuiApplication
(gdb) break QQmlApplicationEngine::load
(gdb) break QQmlContext::setContextProperty

# Print object info
(gdb) print *engine
(gdb) print *rootContext

# Step through
(gdb) step
(gdb) next
(gdb) finish
```

### Qt Creator Tips

- **F2**: Follow symbol under cursor
- **Alt+Shift+U**: Find usages
- **Ctrl+K**: Locate in file
- **F4**: Switch header/source
- **Ctrl+Shift+F**: Find in files

---

## 📚 Key Concepts

### Event Loop
- `exec()` starts event loop
- Events queued and processed
- `quit()` stops event loop
- Non-blocking I/O

### QML Loading
- Parse QML → AST
- Compile → JavaScript bytecode
- Create component
- Instantiate with context

### Context Hierarchy
- Root context (top-level)
- Child contexts (scoped)
- Property resolution: child → parent
- URL resolution relative to context

### Property Exposure
- `setContextProperty()`: Expose object
- Accessible in QML by name
- Property bindings work
- Signals/slots work

---

## 🔗 Dependencies

```
QGuiApplication
├── QCoreApplication
│   ├── QObject
│   └── QEventLoop
└── QPlatformIntegration

QQmlApplicationEngine
├── QQmlEngine
│   ├── QJSEngine
│   └── QQmlTypeLoader
└── QQmlContext

QQmlContext
├── QQmlEngine
└── QQmlContext (parent)
```

---

## 📖 Reading Order

### Beginner
1. Header files (understand API)
2. Simple example code
3. Constructor implementations
4. Basic method implementations

### Intermediate
1. Full class implementations
2. Private headers (`*_p.h`)
3. Platform integration code
4. Type system

### Advanced
1. Optimization techniques
2. Memory management
3. Thread safety
4. Performance critical paths

---

## 💡 Pro Tips

1. **Start Small**: Đọc một method tại một thời điểm
2. **Use Debugger**: Trace execution thay vì chỉ đọc code
3. **Read Comments**: Qt có documentation comments tốt
4. **Follow Examples**: Xem Qt examples để hiểu usage
5. **Ask Questions**: Tại sao code được viết như vậy?
6. **Take Notes**: Ghi chú những điều quan trọng
7. **Practice**: Viết code để test understanding

---

## 🎓 Learning Path

### Week 1: Basics
- [ ] Read header files
- [ ] Understand public API
- [ ] Write simple test app
- [ ] Trace with debugger

### Week 2: Implementation
- [ ] Read constructor code
- [ ] Understand initialization
- [ ] Read key methods
- [ ] Understand data flow

### Week 3: Advanced
- [ ] Read private headers
- [ ] Understand internal structures
- [ ] Platform integration
- [ ] Performance considerations

### Week 4: Mastery
- [ ] Contribute to Qt (optional)
- [ ] Write documentation
- [ ] Help others understand
- [ ] Apply knowledge to projects

---

## 📞 Resources

- **Qt Docs**: https://doc.qt.io/
- **Qt Source**: https://code.qt.io/
- **Qt Forum**: https://forum.qt.io/
- **Stack Overflow**: Tag `qt`, `qml`, `qguiapplication`

---

**Remember**: Đọc mã nguồn là một kỹ năng cần thời gian. Đừng vội vàng, từng bước một!
