# Qt Debugging Recipes & Tips

## 📋 Mục Lục
1. [Setup Environment](#setup-environment)
2. [GDB/LLDB Tips](#gdb-lldb-tips)
3. [Qt Creator Debugging](#qt-creator-debugging)
4. [Common Debugging Scenarios](#common-debugging-scenarios)
5. [Performance Profiling](#performance-profiling)
6. [Memory Debugging](#memory-debugging)
7. [QML Debugging](#qml-debugging)

---

## Setup Environment

### Build Qt với Debug Symbols

```bash
# Clone Qt source
git clone https://code.qt.io/qt/qt5.git
cd qt5
./init-repository --module-subset=qtbase,qtdeclarative

# Configure với debug symbols
mkdir build && cd build
../configure -developer-build -debug -opensource -confirm-license \
    -nomake examples -nomake tests \
    -prefix /opt/qt-debug

# Build (sẽ mất vài giờ)
cmake --build . --parallel $(nproc)
cmake --install .
```

### Setup Qt Creator

**Tools → Options → Debugger → GDB:**
```
Additional Startup Commands:
source /path/to/qt5/qtbase/share/qtcreator/debugger/qttypes.py
set print pretty on
set print object on
set print static-members on
```

**Debug Qt Sources:**
```
Tools → Options → Debugger → General
☑ Use debug info of Qt framework
Qt source path: /path/to/qt5/qtbase/src
```

---

## GDB/LLDB Tips

### Basic Qt Type Inspection

```gdb
# QString
(gdb) p myString
$1 = {d = 0x7fff12345678}

# Pretty print QString
(gdb) call (QString)myString.toLatin1().data()
$2 = "Hello World"

# Hoặc với Qt helper:
(gdb) p/q myString
"Hello World"

# QList/QVector
(gdb) p myList
$3 = QList of length 5 = {1, 2, 3, 4, 5}

# QObject hierarchy
(gdb) call myObject->dumpObjectTree()
QObject::MyObject
  QObject::ChildObject1
  QObject::ChildObject2

# QObject properties
(gdb) call myObject->dumpObjectInfo()
```

### D-Pointer Inspection

```gdb
# Access private implementation
(gdb) p this
$1 = (QGuiApplication *) 0x7fffffffe000

# Get d-pointer
(gdb) p this->d_func()
$2 = (QGuiApplicationPrivate *) 0x555555600000

# Access private members
(gdb) p this->d_func()->eventDispatcher
$3 = (QAbstractEventDispatcher *) 0x555555700000
```

### Breakpoint Tricks

```gdb
# Break on all QObject signal emissions
(gdb) break QMetaObject::activate

# Break on specific signal
(gdb) break MyClass::mySignal

# Break when property changes
(gdb) watch myObject->d_ptr->propertyValue

# Conditional breakpoint
(gdb) break QGuiApplication::notify if event->type() == 2
# 2 = QEvent::KeyPress

# Break on QML warnings
(gdb) break QQmlEngine::warning
```

### Call Stack Navigation

```gdb
# Backtrace với frame numbers
(gdb) bt

# Select specific frame
(gdb) frame 5

# Info về current frame
(gdb) info locals
(gdb) info args

# Move up/down stack
(gdb) up
(gdb) down
```

### Qt-Specific Commands

```gdb
# Define custom commands in ~/.gdbinit

# Print QString content
define pqs
    if $argc == 1
        call (void)qDebug() << *((QString*)$arg0)
    else
        print "Usage: pqs <QString*>"
    end
end

# Print QVariant
define pqv
    if $argc == 1
        call (void)qDebug() << *((QVariant*)$arg0)
    else
        print "Usage: pqv <QVariant*>"
    end
end

# Print QObject name and class
define pqo
    if $argc == 1
        printf "Object: %s, Class: %s\n", \
            ((QObject*)$arg0)->objectName().toLatin1().data(), \
            ((QObject*)$arg0)->metaObject()->className()
    else
        print "Usage: pqo <QObject*>"
    end
end
```

---

## Qt Creator Debugging

### Debug Output Visualization

**Application Output pane:**
```cpp
// Color-coded debug output
qDebug() << "Normal message";
qInfo() << "Info message";
qWarning() << "Warning message";
qCritical() << "Critical message";

// Custom categories
Q_LOGGING_CATEGORY(myCategory, "app.mycategory")
qCDebug(myCategory) << "Debug in category";
qCInfo(myCategory) << "Info in category";
```

**Environment variables:**
```bash
# Enable specific logging categories
QT_LOGGING_RULES="app.mycategory.debug=true"

# Disable all debug output
QT_LOGGING_RULES="*.debug=false"

# Complex rules
QT_LOGGING_RULES="qt.*.debug=false;app.*.debug=true"
```

### Locals and Expressions Window

**Watch expressions:**
```cpp
// In Locals & Expressions pane, add:
this->d_func()
event->type()
sender()->objectName()
m_privateVar

// Complex expressions
dynamic_cast<QQuickWindow*>(rootObjects().first())
rootContext()->contextProperty("myValue")
```

### Debugger Console

**Run Qt commands:**
```
# Qt Creator debugger console
(gdb) call qApp->dumpObjectTree()
(gdb) call myObject->setProperty("visible", QVariant(true))
(gdb) call engine->clearComponentCache()
```

---

## Common Debugging Scenarios

### Scenario 1: Trace Event Delivery

**Problem:** Mouse click không đến được target widget

**Debug steps:**
```cpp
// 1. Breakpoint tại entry point
Breakpoint: QGuiApplication::notify()

// 2. Check event type và receiver
(gdb) p event->type()
$1 = 2  // QEvent::MouseButtonPress

(gdb) p receiver->objectName()
$2 = "myButton"

// 3. Step through event handlers
(gdb) n  // Next line
(gdb) s  // Step into

// 4. Check if event was accepted
(gdb) p event->isAccepted()
$3 = true

// 5. Check event filters
(gdb) call qApp->dumpObjectTree()
# Look for objects với eventFilter()
```

**Common issues:**
- Event filter blocking events
- Wrong object in focus
- Event được consume bởi parent widget

### Scenario 2: QML Context Property Not Updating

**Problem:** Context property thay đổi nhưng QML không update

**Debug steps:**
```cpp
// 1. Verify property has NOTIFY signal
class MyClass : public QObject {
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    // ↑ MUST have NOTIFY!
signals:
    void valueChanged();  // ← MUST emit this
};

// 2. Breakpoint tại setter
Breakpoint: MyClass::setValue()

// 3. Verify signal emission
(gdb) b MyClass::setValue
(gdb) commands
> silent
> printf "setValue called with: %d\n", value
> call this->valueChanged()  // Manual emit for testing
> continue
> end

// 4. Check if signal connected
(gdb) call QObject::dumpObjectInfo()
# Look for connections to valueChanged signal
```

**Common issues:**
- Missing NOTIFY signal
- Signal not emitted after value change
- Context property set AFTER QML loaded

### Scenario 3: Crash in QML Binding

**Problem:** App crashes khi evaluate QML binding

**Debug steps:**
```cpp
// 1. Enable QML debugging
QLoggingCategory::setFilterRules(QStringLiteral("qml.binding.removal.info=true"));

// 2. Catch exception
(gdb) catch throw
(gdb) catch signal SIGSEGV

// 3. Examine binding
Breakpoint: QQmlBinding::update()

// 4. Check binding expression
(gdb) p m_expression
$1 = "parent.width * 0.5"

// 5. Check binding source object
(gdb) p m_target
$2 = (QObject *) 0x0  // ← NULL pointer! Problem found!
```

**Common issues:**
- Binding references destroyed object
- Circular binding dependency
- Type mismatch trong binding

### Scenario 4: QML Component Loading Failed

**Problem:** QQmlApplicationEngine.load() không tạo được object

**Debug steps:**
```cpp
// 1. Connect to warnings signal
QObject::connect(&engine, &QQmlApplicationEngine::warnings,
    [](const QList<QQmlError> &warnings) {
        for (const auto &w : warnings) {
            qDebug() << w.toString();
        }
    });

// 2. Check if object created
QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
    [](QObject *obj, const QUrl &url) {
        if (!obj)
            qDebug() << "Failed to create from:" << url;
    });

// 3. Breakpoint at component creation
Breakpoint: QQmlComponent::create()

// 4. Check component status
(gdb) p this->status()
$1 = QQmlComponent::Error

(gdb) call this->errors()
$2 = QList of length 1 = {
    QQmlError("Unknown property: 'myProperty'", line 10)
}
```

### Scenario 5: Performance Issue - Slow Rendering

**Problem:** UI laggy, slow frame rate

**Debug steps:**
```cpp
// 1. Enable Qt Quick profiling
QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software); // Test

// 2. Profile frame rendering
qputenv("QSG_RENDER_TIMING", "1");

// 3. Breakpoint tại paint
Breakpoint: QQuickWindow::update()

// 4. Measure time
(gdb) b QQuickWindow::update
(gdb) commands
> silent
> shell date +%s%N
> continue
> end

// 5. Check for binding loops
QML_DISABLE_OPTIMIZER=1
```

---

## Performance Profiling

### Qt Built-in Profilers

#### QML Profiler (Qt Creator)

```bash
# 1. Build với debug info
qmake CONFIG+=debug CONFIG+=qml_debug

# 2. Run trong Qt Creator với QML Profiler
Analyze → QML Profiler

# 3. Look for:
# - Binding loops
# - Expensive JavaScript evaluations
# - Slow component creation
```

#### Scene Graph Rendering

```cpp
// Enable rendering stats
qputenv("QSG_RENDER_TIMING", "1");

// Output format:
// Frame: 16.67ms (60 FPS)
//   - Sync: 2.5ms
//   - Render: 12.1ms
//   - Swap: 2.0ms
```

### Valgrind

```bash
# Memory profiling
valgrind --tool=massif --massif-out-file=massif.out ./myapp
ms_print massif.out

# Callgrind (CPU profiling)
valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./myapp
kcachegrind callgrind.out

# Memory leaks
valgrind --leak-check=full --show-leak-kinds=all ./myapp
```

### Perf (Linux)

```bash
# Record performance data
perf record -g ./myapp

# Analyze
perf report

# Find hot spots
perf top -p $(pidof myapp)
```

### Qt Performance Logging

```cpp
// Enable performance logging
QLoggingCategory::setFilterRules(
    "qt.qml.binding.removal.info=true\n"
    "qt.quick.timing=true\n"
    "qt.scenegraph.time.renderloop=true"
);

// Custom timing
QElapsedTimer timer;
timer.start();

// ... code to measure ...

qint64 elapsed = timer.nsecsElapsed();
qDebug() << "Operation took:" << elapsed / 1000000.0 << "ms";
```

---

## Memory Debugging

### QObject Memory Management

```cpp
// Rule 1: QObject với parent tự động deleted
QObject *child = new QObject(parent);
// ↑ Don't need to delete, parent will delete

// Rule 2: Without parent, YOU must delete
QObject *obj = new QObject();
// ... use obj ...
delete obj;  // ← YOUR responsibility

// Rule 3: Use QScopedPointer for safety
QScopedPointer<QObject> obj(new QObject());
// Auto-deleted when out of scope
```

### Detect Memory Leaks

```cpp
// Method 1: Track QObject creation/destruction
class ObjectTracker : public QObject {
public:
    ObjectTracker() {
        QObject::connect(qApp, &QCoreApplication::aboutToQuit,
            this, &ObjectTracker::checkLeaks);
    }
    
    void checkLeaks() {
        auto allObjects = qApp->findChildren<QObject*>();
        qDebug() << "Objects alive:" << allObjects.size();
        for (auto obj : allObjects) {
            qDebug() << "  -" << obj->metaObject()->className()
                     << obj->objectName();
        }
    }
};

// Method 2: Use AddressSanitizer
// Compile with: -fsanitize=address
```

### Qt's Memory Pool

```cpp
// Some Qt classes use memory pools
// Check allocations:
qDebug() << "QList allocations:" << QListData::shared_null.ref.load();
```

---

## QML Debugging

### Enable QML Debugging

```bash
# Build with QML debugging support
qmake CONFIG+=qml_debug

# Run with debugger port
./myapp -qmljsdebugger=port:3768
```

### Qt Creator QML Debugger

**Features:**
1. **Breakpoints in QML**: Set breakpoints trong .qml files
2. **Inspect QML objects**: View property values trong runtime
3. **Expression evaluation**: Evaluate QML expressions
4. **QML Profiler**: Profile QML performance

**Debug console commands:**
```javascript
// In QML debugger console
print(myObject.width)
myObject.color = "red"
console.log("Value:", myValue)
```

### QML Logging

```qml
// In QML files
Item {
    Component.onCompleted: {
        console.log("Component completed")
        console.log("Width:", width, "Height:", height)
    }
    
    onWidthChanged: {
        console.trace()  // Print stack trace
        console.time("resize")  // Start timer
        // ... resize code ...
        console.timeEnd("resize")  // End timer
    }
}
```

### QML Assertions

```qml
Rectangle {
    width: 100
    height: 100
    
    onWidthChanged: {
        console.assert(width > 0, "Width must be positive!")
    }
}
```

---

## Advanced Debugging Techniques

### Reverse Debugging (GDB)

```gdb
# Record execution
(gdb) target record-full

# Run forward
(gdb) continue

# Go backwards!
(gdb) reverse-continue
(gdb) reverse-step
(gdb) reverse-next

# Find when variable changed
(gdb) watch -l myVariable
(gdb) reverse-continue
```

### Conditional Logging

```cpp
#define DEBUG_EVENT(event) \
    do { \
        if (qEnvironmentVariableIsSet("DEBUG_EVENTS")) { \
            qDebug() << "Event:" << event->type() \
                     << "Time:" << QTime::currentTime().toString(); \
        } \
    } while (0)
```

### Custom Qt Message Handler

```cpp
void myMessageHandler(QtMsgType type, const QMessageLogContext &context,
                     const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[DEBUG] %1 (%2:%3)").arg(msg).arg(context.file).arg(context.line);
        break;
    case QtWarningMsg:
        txt = QString("[WARN] %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("[CRITICAL] %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("[FATAL] %1").arg(msg);
        abort();
    }
    
    // Write to file
    QFile outFile("debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
       << txt << endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);
    // ...
}
```

---

## Useful Environment Variables

```bash
# Qt Debugging
QT_DEBUG_PLUGINS=1              # Plugin loading debug
QT_LOGGING_RULES="*.debug=true" # Enable all debug output
QML_DISABLE_OPTIMIZER=1         # Disable QML optimizer
QSG_INFO=1                      # Scene Graph info
QSG_RENDER_TIMING=1             # Rendering timing
QT_FATAL_WARNINGS=1             # Treat warnings as fatal

# QML Debugging
QML_IMPORT_TRACE=1              # Trace import loading
QML_DISABLE_DISK_CACHE=1        # Disable QML cache

# Graphics
QT_QPA_PLATFORM=xcb             # Force platform
QT_SCALE_FACTOR=2               # High DPI scaling
QT_AUTO_SCREEN_SCALE_FACTOR=1   # Auto DPI scaling

# Performance
QT_LOGGING_TO_CONSOLE=1         # Log to console
```

---

## Debugging Checklist

### Before You Start
- [ ] Build with debug symbols (-g flag)
- [ ] Enable assertions (don't use -DQT_NO_DEBUG)
- [ ] Install Qt sources for stepping
- [ ] Setup GDB/LLDB pretty printers
- [ ] Enable Qt Creator debugger helpers

### During Debugging
- [ ] Reproduce issue reliably
- [ ] Note exact steps to reproduce
- [ ] Check Qt documentation for class
- [ ] Search Qt bug tracker
- [ ] Minimize test case
- [ ] Use logging before breakpoints
- [ ] Check memory ownership
- [ ] Verify thread safety

### After Finding Bug
- [ ] Write test case
- [ ] Document fix
- [ ] Check for similar issues
- [ ] Consider upstream fix
- [ ] Update documentation

---

## Resources

### Documentation
- https://doc.qt.io/qt-6/debug.html
- https://wiki.qt.io/How_to_Use_the_QML_Debugger

### Tools
- Qt Creator Debugger: https://doc.qt.io/qtcreator/creator-debugger-engines.html
- GDB Dashboard: https://github.com/cyrus-and/gdb-dashboard
- Valgrind: https://valgrind.org/docs/manual/quick-start.html

### Community
- Qt Forum: https://forum.qt.io/
- Qt Mailing List: https://lists.qt-project.org/
- Stack Overflow: [qt] tag

---

Happy Debugging! 🐛🔧
