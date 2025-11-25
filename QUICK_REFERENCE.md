# Quick Reference - Qt Source Code Reading Guide

## 🎯 Mục Tiêu Nhanh

### QGuiApplication
**Vai trò:** Quản lý application lifecycle, event loop, platform abstraction  
**File chính:** `qtbase/src/gui/kernel/qguiapplication.cpp`  
**Đọc trước:** Constructor → exec() → Platform integration

### QQmlApplicationEngine
**Vai trò:** Load và execute QML files, quản lý QML components  
**File chính:** `qtdeclarative/src/qml/qml/qqmlapplicationengine.cpp`  
**Đọc trước:** Constructor → load() → Component creation

### QQmlContext
**Vai trò:** Cung cấp data và properties cho QML components  
**File chính:** `qtdeclarative/src/qml/qml/qmlcontext.cpp`  
**Đọc trước:** Constructor → setContextProperty() → Property lookup

---

## 📍 Nơi Tìm Source Code

```bash
# Online
https://github.com/qt/qtbase          # QGuiApplication
https://github.com/qt/qtdeclarative   # QQmlApplicationEngine, QQmlContext
https://code.woboq.org/qt5/           # Browse với syntax highlighting

# Local (nếu đã cài Qt source)
qmake -query QT_INSTALL_PREFIX
# Thường ở: ~/Qt/5.x.x/Src/ hoặc /usr/include/qt5/
```

---

## 🔍 Key Methods Cần Đọc

### QGuiApplication
```cpp
// Constructor - initialization flow
QGuiApplication::QGuiApplication(int &argc, char **argv)

// Event loop - core mechanism
int QCoreApplication::exec()

// Platform abstraction
QPlatformIntegration *platformIntegration() const
```

### QQmlApplicationEngine
```cpp
// Constructor - engine setup
QQmlApplicationEngine::QQmlApplicationEngine(QObject *parent)

// Load QML - MOST IMPORTANT ⭐
void load(const QUrl &url)
void loadData(const QByteArray &data, const QUrl &url)

// Root objects
QList<QObject*> rootObjects() const
```

### QQmlContext
```cpp
// Constructor - context creation
QQmlContext::QQmlContext(QQmlEngine *engine, QObject *parent)

// Set property - MOST IMPORTANT ⭐
void setContextProperty(const QString &name, QObject *value)
void setContextProperty(const QString &name, const QVariant &value)

// Get property
QVariant contextProperty(const QString &name) const
```

---

## 🔄 Execution Flow

```
main()
  │
  ├─> QGuiApplication app(argc, argv)
  │     └─> Platform plugin loading
  │     └─> Event dispatcher setup
  │
  ├─> QQmlApplicationEngine engine
  │     └─> QQmlEngine initialization
  │     └─> Root context creation
  │
  ├─> engine.rootContext()->setContextProperty("data", object)
  │     └─> Property storage
  │
  ├─> engine.load("main.qml")
  │     └─> QUrl resolution
  │     └─> QQmlComponent::loadUrl()
  │     └─> Component compilation
  │     └─> Object creation
  │
  └─> app.exec()
        └─> QEventLoop::exec()
        └─> Event processing
        └─> QML binding updates
```

---

## 🧠 Key Concepts

### QGuiApplication
- **Singleton Pattern:** Chỉ một instance
- **Event-Driven:** Mọi thứ xoay quanh events
- **Platform Abstraction:** Không phụ thuộc OS cụ thể

### QQmlApplicationEngine
- **Component-Based:** QML compile thành components
- **Lazy Loading:** Components tạo khi cần
- **Root Context:** Mỗi engine có root context

### QQmlContext
- **Context Hierarchy:** Parent-child relationships
- **Property Resolution:** Lookup từ child lên parent
- **Binding Source:** QML bindings đọc từ context

---

## 📚 Reading Strategy

### Phương Pháp Top-Down (Khuyến Nghị)
1. Header file (.h) - Interface
2. Documentation - Comments
3. Constructor - Initialization
4. Key methods - Core functionality
5. Private implementation - Details

### Phương Pháp Bottom-Up
1. Entry point (main)
2. Trace execution
3. Understand components
4. Build complete picture

---

## 🛠️ Debugging Tips

```cpp
// Add logging để trace
qDebug() << "QGuiApplication created";
qDebug() << "Loading QML:" << url;
qDebug() << "Context property:" << context->contextProperty("data");

// Set breakpoints trong Qt source
// Use Qt Creator: F2, Ctrl+Click để navigate
```

---

## ⚡ Quick Lookup

### Tìm Constructor
```bash
# QGuiApplication
grep -r "QGuiApplication::QGuiApplication" qtbase/src/gui/kernel/

# QQmlApplicationEngine
grep -r "QQmlApplicationEngine::QQmlApplicationEngine" qtdeclarative/src/qml/qml/

# QQmlContext
grep -r "QQmlContext::QQmlContext" qtdeclarative/src/qml/qml/
```

### Tìm Key Methods
```bash
# Load method
grep -r "void.*load.*QUrl" qtdeclarative/src/qml/qml/qqmlapplicationengine.cpp

# setContextProperty
grep -r "setContextProperty" qtdeclarative/src/qml/qml/qmlcontext.cpp

# exec method
grep -r "int.*exec" qtbase/src/corelib/kernel/qcoreapplication.cpp
```

---

## 🎓 Learning Path

### Beginner (1-2 days)
- [ ] Đọc header files
- [ ] Đọc documentation
- [ ] Hiểu basic concepts
- [ ] Trace simple example

### Intermediate (1 week)
- [ ] Đọc implementation của key methods
- [ ] Hiểu execution flow
- [ ] Debug với breakpoints
- [ ] Create examples

### Advanced (2-3 weeks)
- [ ] Đọc toàn bộ implementation
- [ ] Hiểu design decisions
- [ ] Understand performance implications
- [ ] Contribute improvements

---

## 📖 Related Classes

### QGuiApplication Related
- `QCoreApplication` - Base class
- `QEventLoop` - Event processing
- `QPlatformIntegration` - Platform abstraction
- `QEventDispatcher` - Event delivery

### QQmlApplicationEngine Related
- `QQmlEngine` - Base class
- `QQmlComponent` - Component management
- `QQmlContext` - Context management
- `QQmlTypeLoader` - Type loading

### QQmlContext Related
- `QQmlEngine` - Engine association
- `QQmlContextData` - Private implementation
- `QQmlProperty` - Property access
- `QQmlBinding` - Binding evaluation

---

## 🔗 Useful Links

- **Qt Documentation:** https://doc.qt.io/
- **Qt Base Source:** https://github.com/qt/qtbase
- **Qt Declarative Source:** https://github.com/qt/qtdeclarative
- **Woboq Code Browser:** https://code.woboq.org/qt5/
- **Qt Blog:** https://www.qt.io/blog

---

## 💡 Pro Tips

1. **Start Small:** Đọc từng phần một, không cố đọc hết
2. **Use IDE:** Qt Creator có excellent code navigation
3. **Debug:** Set breakpoints và step through code
4. **Take Notes:** Ghi chép những điểm quan trọng
5. **Practice:** Tạo examples để test understanding
6. **Ask Questions:** Stack Overflow, Qt forums
7. **Read Comments:** Qt source có comments rất tốt

---

**Remember:** Đọc source code là một journey, không phải destination. Take your time và enjoy the process! 🚀
