# Qt Architecture Diagrams & Flow Charts

## 📋 Mục Lục
1. [Class Hierarchy](#class-hierarchy)
2. [Initialization Sequence](#initialization-sequence)
3. [Event Processing Flow](#event-processing-flow)
4. [QML Loading Pipeline](#qml-loading-pipeline)
5. [Context Property Resolution](#context-property-resolution)
6. [Signal-Slot Mechanism](#signal-slot-mechanism)
7. [Memory Management](#memory-management)

---

## Class Hierarchy

### QGuiApplication Hierarchy

```
┌────────────────────────────────────────────────┐
│              QObject (QtCore)                   │
│  - Meta-object system                           │
│  - Signal/slot mechanism                        │
│  - Property system                              │
│  - Parent-child relationship                    │
└─────────────────┬──────────────────────────────┘
                  │ inherits
┌─────────────────▼──────────────────────────────┐
│         QCoreApplication (QtCore)               │
│  - Event loop (exec())                          │
│  - Event posting/sending                        │
│  - QEventLoop management                        │
│  - QAbstractEventDispatcher                     │
│  - Plugin loading                               │
│  - Translation system                           │
└─────────────────┬──────────────────────────────┘
                  │ inherits
┌─────────────────▼──────────────────────────────┐
│         QGuiApplication (QtGui)                 │
│  - QPlatformIntegration (QPA layer)            │
│  - Window management (QWindow)                  │
│  - Screen management (QScreen)                  │
│  - Input handling (mouse, keyboard, touch)     │
│  - Clipboard, drag-and-drop                    │
│  - Font, palette management                     │
│  - High-DPI support                            │
│  - Session management                           │
└─────────────────┬──────────────────────────────┘
                  │ inherits (optional)
┌─────────────────▼──────────────────────────────┐
│         QApplication (QtWidgets)                │
│  - Widget-specific functionality               │
│  - QWidget support                             │
│  - QStyle system                               │
│  (Not used in QML-only apps)                   │
└────────────────────────────────────────────────┘
```

### QQmlEngine Hierarchy

```
┌────────────────────────────────────────────────┐
│              QJSEngine (QtQml)                  │
│  - V4 JavaScript engine                         │
│  - JS value types                               │
│  - Script evaluation                            │
│  - JS object/array creation                     │
└─────────────────┬──────────────────────────────┘
                  │ inherits
┌─────────────────▼──────────────────────────────┐
│           QQmlEngine (QtQml)                    │
│  - QML type system                              │
│  - Component loading/caching                    │
│  - Import path resolution                       │
│  - Context management                           │
│  - Network access manager                       │
│  - Image provider                               │
│  - URL interceptor                              │
└─────────────────┬──────────────────────────────┘
                  │ inherits
┌─────────────────▼──────────────────────────────┐
│      QQmlApplicationEngine (QtQml)              │
│  - Application-specific features                │
│  - Automatic root window creation              │
│  - Built-in quit() on last window closed       │
│  - Application properties                       │
└────────────────────────────────────────────────┘
```

### Supporting Classes

```
QQmlEngine ecosystem:
├── QQmlComponent
│   ├── Component loading
│   ├── Object instantiation
│   └── Status tracking
│
├── QQmlContext
│   ├── Property exposure
│   ├── Context hierarchy
│   └── Property resolution
│
├── QQmlProperty
│   ├── Property read/write
│   └── Binding creation
│
├── QQmlExpression
│   ├── Expression evaluation
│   └── Context-aware JS evaluation
│
├── QQmlIncubator
│   ├── Asynchronous object creation
│   └── Progress tracking
│
└── QQmlTypeLoader
    ├── File loading
    ├── Import resolution
    └── Component caching
```

---

## Initialization Sequence

### Complete Application Startup

```
┌─────────────────────────────────────────────────────────┐
│                    main() Entry Point                    │
└────────────────────────┬────────────────────────────────┘
                         │
        ┌────────────────▼─────────────────┐
        │ QGuiApplication app(argc, argv)  │
        └────────────────┬─────────────────┘
                         │
    ╔════════════════════▼════════════════════╗
    ║   QGuiApplicationPrivate::init()        ║
    ╠═════════════════════════════════════════╣
    ║ 1. Parse command line arguments         ║
    ║ 2. Setup platform theme                 ║
    ║ 3. createPlatformIntegration()          ║
    ║    ├─→ Windows: QWindowsIntegration     ║
    ║    ├─→ Linux: QXcbIntegration          ║
    ║    ├─→ macOS: QCocoaIntegration        ║
    ║    └─→ Android/iOS: respective impl    ║
    ║ 4. createEventDispatcher()             ║
    ║    ├─→ Glib (Linux with GTK+)          ║
    ║    ├─→ Windows event loop              ║
    ║    └─→ Default Qt event loop           ║
    ║ 5. Initialize screens (QScreen)        ║
    ║ 6. Initialize input devices             ║
    ║ 7. Register platform dialogs            ║
    ╚════════════════════════════════════════╝
                         │
        ┌────────────────▼─────────────────┐
        │  QQmlApplicationEngine engine    │
        └────────────────┬─────────────────┘
                         │
    ╔════════════════════▼════════════════════╗
    ║   QQmlEngine constructor                ║
    ╠═════════════════════════════════════════╣
    ║ 1. Initialize V4 JavaScript engine      ║
    ║    └─→ Create ExecutionEngine           ║
    ║ 2. Register built-in QML types          ║
    ║    ├─→ QtQuick types                    ║
    ║    ├─→ QtQml types                      ║
    ║    └─→ Basic types (int, string, etc)  ║
    ║ 3. Setup import paths                   ║
    ║    ├─→ Qt plugin directories            ║
    ║    ├─→ QML2_IMPORT_PATH env var        ║
    ║    └─→ Application directory            ║
    ║ 4. Create root context                  ║
    ║    └─→ QQmlContext with engine parent  ║
    ║ 5. Initialize type loaders              ║
    ║ 6. Setup network access manager         ║
    ╚════════════════════════════════════════╝
                         │
        ┌────────────────▼─────────────────────┐
        │ engine.rootContext()                  │
        │   ->setContextProperty("foo", bar)    │
        └────────────────┬─────────────────────┘
                         │
    ╔════════════════════▼════════════════════╗
    ║   Store property in context              ║
    ╠═════════════════════════════════════════╣
    ║ 1. Get QQmlContextData (internal)        ║
    ║ 2. Check if QObject or QVariant          ║
    ║ 3. Store in property map                 ║
    ║ 4. Setup property change notifications   ║
    ║ 5. If QObject: connect to destroyed()    ║
    ╚════════════════════════════════════════╝
                         │
        ┌────────────────▼─────────────────────┐
        │ engine.load(QUrl("qrc:/main.qml"))   │
        └────────────────┬─────────────────────┘
                         │
                    [See QML Loading Pipeline]
                         │
        ┌────────────────▼─────────────────┐
        │       app.exec()                  │
        └────────────────┬─────────────────┘
                         │
    ╔════════════════════▼════════════════════╗
    ║   Enter Event Loop                       ║
    ╠═════════════════════════════════════════╣
    ║ while (!exitFlag) {                      ║
    ║   1. Wait for events (select/poll)       ║
    ║   2. Process platform events             ║
    ║   3. Dispatch to Qt objects              ║
    ║   4. Execute posted events               ║
    ║   5. Process timers                      ║
    ║   6. Render frames (if needed)           ║
    ║ }                                        ║
    ╚════════════════════════════════════════╝
```

---

## Event Processing Flow

### Complete Event Lifecycle

```
┌─────────────────────────────────────────────────────────────┐
│                    Event Source                              │
│  - OS (mouse, keyboard, window events)                       │
│  - Qt (posted events, timers)                                │
│  - Application (custom events)                               │
└────────────────────┬────────────────────────────────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  QWindowSystemInterface         │
    │  (Platform → Qt boundary)       │
    │                                 │
    │  handleMouseEvent()             │
    │  handleKeyEvent()               │
    │  handleWheelEvent()             │
    │  handleTouchEvent()             │
    │  handleScreenAdded()            │
    │  etc.                           │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  Event Queue                    │
    │  (QWindowSystemEventList)       │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  Event Loop Iteration           │
    │  QEventLoop::processEvents()    │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼────────────────────────┐
    │  QGuiApplicationPrivate                  │
    │  ::processWindowSystemEvent()            │
    │                                          │
    │  Switch (event type):                    │
    │    Mouse → processMouseEvent()           │
    │    Key → processKeyEvent()               │
    │    Touch → processTouchEvent()           │
    │    etc.                                  │
    └────────────────┬────────────────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  Create QEvent subclass         │
    │  (QMouseEvent, QKeyEvent, etc)  │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼──────────────────────────┐
    │  QGuiApplication::notify(receiver, event)  │
    │                                            │
    │  - Can be overridden for custom handling   │
    │  - Point of event filtering                │
    └────────────────┬──────────────────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  Global Event Filters           │
    │  (installed via                 │
    │   installEventFilter())         │
    │                                 │
    │  If filter returns true:        │
    │    → Event consumed, STOP       │
    │  Else:                          │
    │    → Continue                   │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  QObject::event(event)          │
    │  (on receiver object)           │
    │                                 │
    │  Switch (event type):           │
    │    case MouseButtonPress:       │
    │      → mousePressEvent()        │
    │    case KeyPress:               │
    │      → keyPressEvent()          │
    │    case Timer:                  │
    │      → timerEvent()             │
    │    etc.                         │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼───────────────────┐
    │  Object Event Filters               │
    │  (installed on specific objects)    │
    │                                     │
    │  For each filter:                   │
    │    If returns true: STOP            │
    └────────────────┬───────────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  Specific Event Handler         │
    │  (mousePressEvent, etc.)        │
    │                                 │
    │  - Default implementation       │
    │  - Can be overridden            │
    │  - Call event->ignore() to      │
    │    propagate to parent          │
    └────────────────┬───────────────┘
                     │
    ┌────────────────▼───────────────┐
    │  Event Accepted?                │
    └────────┬────────────────┬──────┘
             │ No             │ Yes
             │                │
    ┌────────▼───────┐        │
    │ Propagate to   │        │
    │ Parent Object  │        │
    └────────────────┘        │
                              │
                    ┌─────────▼──────────┐
                    │  Event Processing   │
                    │  Complete           │
                    └────────────────────┘

Example: Mouse Click on a QPushButton in a QWidget

OS Mouse Click
  → QWindowSystemInterface::handleMouseEvent()
    → Event queued
      → QEventLoop processes queue
        → QGuiApplication::notify(button, mouseEvent)
          → button->event(mouseEvent)
            → button->mousePressEvent(mouseEvent)
              → emit clicked()
                → Connected slot executed
```

### Event Types and Priorities

```
Event Priority Levels (processed in order):
┌──────────────────────────────────────────────┐
│ 1. Posted Events (QCoreApplication::post)   │ ← Highest priority
│    - Deferred deletion (DeferredDelete)      │
│    - Paint events (when coalesced)           │
│    - Custom posted events                    │
├──────────────────────────────────────────────┤
│ 2. Window System Events                      │
│    - Mouse, keyboard, touch                  │
│    - Window state changes                    │
│    - Screen changes                          │
├──────────────────────────────────────────────┤
│ 3. Timer Events                              │
│    - QTimer timeout signals                  │
│    - Zero-timer events (QTimer::singleShot)  │
├──────────────────────────────────────────────┤
│ 4. Idle Processing                           │ ← Lowest priority
│    - DeferredDelete cleanup                  │
│    - Deferred component creation             │
└──────────────────────────────────────────────┘
```

---

## QML Loading Pipeline

### Detailed Component Loading Flow

```
engine.load(QUrl("qrc:/main.qml"))
    │
    ▼
┌─────────────────────────────────────────────┐
│  QQmlApplicationEngine::load()              │
│  - Create QQmlComponent                     │
│  - Set creation context (root context)      │
└────────────────┬────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────┐
│  QQmlComponent::loadUrl()                   │
│  - Check if URL is valid                    │
│  - Determine load mode (sync/async)         │
└────────────────┬────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────┐
│  QQmlTypeLoader::getType()                  │
│  - Check cache first                        │
│  - If cached: return cached component       │
│  - Else: create new load request            │
└────────────────┬────────────────────────────┘
                 │
                 ▼
╔═════════════════════════════════════════════╗
║  File Loading Phase                         ║
╠═════════════════════════════════════════════╣
║ 1. Resolve URL to file path                 ║
║    - qrc:/ → Qt resource system             ║
║    - file:/ → Local filesystem              ║
║    - http:/ → Network request               ║
║                                             ║
║ 2. Read file content                        ║
║    - QFile for local files                  ║
║    - QNetworkAccessManager for remote       ║
║                                             ║
║ 3. QQmlTypeData::dataReceived()            ║
║    - Store file content                     ║
║    - Proceed to parsing                     ║
╚════════════════┬════════════════════════════╝
                 │
                 ▼
╔═════════════════════════════════════════════╗
║  Parsing Phase                              ║
╠═════════════════════════════════════════════╣
║ 1. QQmlScript::Parser::parse()             ║
║    - Lexical analysis (tokenization)        ║
║    - Syntax analysis                        ║
║    - Build AST (Abstract Syntax Tree)       ║
║                                             ║
║ AST Structure Example:                      ║
║   UiProgram                                 ║
║   └─ UiObjectDefinition (Rectangle)         ║
║      ├─ UiScriptBinding (width: 100)       ║
║      ├─ UiScriptBinding (height: 100)      ║
║      └─ UiObjectDefinition (Text)          ║
║         └─ UiScriptBinding (text: "Hi")    ║
║                                             ║
║ 2. Error checking                           ║
║    - Syntax errors                          ║
║    - Unknown properties (detected later)    ║
╚════════════════┬════════════════════════════╝
                 │
                 ▼
╔═════════════════════════════════════════════╗
║  Import Resolution Phase                    ║
╠═════════════════════════════════════════════╣
║ 1. Process import statements                ║
║    import QtQuick 2.15                      ║
║    import "components"                      ║
║    import org.example.mymodule 1.0         ║
║                                             ║
║ 2. QQmlImports::resolveType()              ║
║    - Search import paths                    ║
║    - Load qmldir files                      ║
║    - Register imported types                ║
║                                             ║
║ 3. QQmlMetaType lookup                     ║
║    - Find C++ registered types              ║
║    - Find QML-defined types                 ║
║    - Build type metadata                    ║
╚════════════════┬════════════════════════════╝
                 │
                 ▼
╔═════════════════════════════════════════════╗
║  Compilation Phase                          ║
╠═════════════════════════════════════════════╣
║ 1. QQmlCompiler::compile()                 ║
║    - Walk AST                               ║
║    - Resolve property names                 ║
║    - Resolve type names                     ║
║    - Optimize bindings                      ║
║                                             ║
║ 2. Generate V4 bytecode                     ║
║    - Convert JS expressions to bytecode     ║
║    - Optimize property accesses             ║
║    - Generate binding functions             ║
║                                             ║
║ 3. Create QQmlCompiledData                 ║
║    - Store compiled bytecode                ║
║    - Store type references                  ║
║    - Store property bindings                ║
║                                             ║
║ 4. Cache compiled data                      ║
║    - Store in QQmlTypeLoader cache         ║
║    - Optionally write disk cache (.qmlc)   ║
╚════════════════┬════════════════════════════╝
                 │
                 ▼
┌─────────────────────────────────────────────┐
│  QQmlComponent::create()                    │
│  - Component is now ready                   │
│  - Create root object instance              │
└────────────────┬────────────────────────────┘
                 │
                 ▼
╔═════════════════════════════════════════════╗
║  Instantiation Phase                        ║
╠═════════════════════════════════════════════╣
║ QQmlObjectCreator::create()                ║
║                                             ║
║ For each object in tree:                    ║
║                                             ║
║ 1. Create C++ object                        ║
║    - Call QMetaObject::newInstance()       ║
║    - Or call registered creation function   ║
║                                             ║
║ 2. Set parent-child relationship            ║
║    - Set QObject parent                     ║
║    - Set visual parent (if Item)            ║
║                                             ║
║ 3. Set property values                      ║
║    - Set constant values                    ║
║    - Create property bindings               ║
║    - Connect signals                        ║
║                                             ║
║ 4. Resolve context properties               ║
║    - Look up context properties             ║
║    - Create context property accesses       ║
║                                             ║
║ 5. Call Component.onCompleted               ║
║    - Execute completion handlers            ║
║    - Object is now fully initialized        ║
╚═════════════════════════════════════════════╝
                 │
                 ▼
┌─────────────────────────────────────────────┐
│  Object tree created!                        │
│  - Root object returned                      │
│  - All children created                      │
│  - All bindings active                       │
│  - Ready for rendering                       │
└─────────────────────────────────────────────┘
```

### Caching Strategy

```
First Load:
  URL → Load file → Parse → Compile → Cache → Create object
              ⤷ Disk read      ⤷ CPU intensive

Second Load (same URL):
  URL → Check cache → Get cached → Create object
              ⤷ Memory lookup (fast!)

Cache Key: QUrl (normalized)

Cache Storage:
┌────────────────────────────────────────┐
│  QQmlTypeLoader::m_typeCache           │
│  QHash<QUrl, QQmlTypeData*>           │
│                                        │
│  Key: "qrc:/main.qml"                 │
│  Value: QQmlTypeData {                │
│    compiledData: QQmlCompiledData*    │
│    status: Complete                    │
│    ... }                              │
└────────────────────────────────────────┘
```

---

## Context Property Resolution

### Property Lookup Chain

```
QML Code:
    Text { text: myProperty }
                    │
                    ▼
┌──────────────────────────────────────────────┐
│  QML binding evaluation                       │
│  QV4::Runtime::get_property(name)            │
└──────────────────┬───────────────────────────┘
                   │
                   ▼
┌──────────────────────────────────────────────┐
│  1. Local object properties                   │
│     Check if "myProperty" exists on Text      │
│     → Not found (not a Text property)         │
└──────────────────┬───────────────────────────┘
                   │
                   ▼
┌──────────────────────────────────────────────┐
│  2. Context properties (current context)      │
│     QQmlContextData::propertyValue("myProp")  │
│     → Check current component context         │
└──────────────────┬───────────────────────────┘
                   │
        ┌──────────┴──────────┐
        │ Found?              │
        └──┬────────────────┬─┘
     Yes   │                │ No
           │                │
           │                ▼
           │    ┌──────────────────────────────────┐
           │    │  3. Parent context                │
           │    │     Move to parent context        │
           │    │     Repeat search                 │
           │    └──────────────┬───────────────────┘
           │                   │
           │        ┌──────────┴──────────┐
           │        │ Found?              │
           │        └──┬────────────────┬─┘
           │     Yes   │                │ No
           │           │                │
           │           │                ▼
           │           │    ┌──────────────────────────┐
           │           │    │  4. Root context          │
           │           │    │     Final lookup          │
           │           │    └──────────┬───────────────┘
           │           │               │
           │           │    ┌──────────┴──────────┐
           │           │    │ Found?              │
           │           │    └──┬────────────────┬─┘
           │           │ Yes   │                │ No
           │           │       │                │
           ▼           ▼       ▼                ▼
┌──────────────────────────────────┐    ┌─────────────┐
│  Property found!                  │    │  Error:     │
│  - Get value                      │    │  ReferenceE │
│  - Setup binding (if needed)      │    │  rror       │
│  - Monitor for changes            │    └─────────────┘
└───────────────────────────────────┘

Example Context Hierarchy:

┌─────────────────────────────────────┐
│  Root Context                        │
│  Properties:                         │
│    - appVersion: "1.0"              │
│    - dataModel: <QObject>           │
└──────────────┬──────────────────────┘
               │ parent
┌──────────────▼──────────────────────┐
│  Component Context (MyComponent)     │
│  Properties:                         │
│    - componentValue: 42             │
└──────────────┬──────────────────────┘
               │ parent
┌──────────────▼──────────────────────┐
│  Item Context (Rectangle)            │
│  Properties:                         │
│    - localValue: "local"            │
│  Object Properties:                  │
│    - width, height, color, ...      │
└─────────────────────────────────────┘

Lookup "appVersion" from Rectangle:
  Rectangle context → not found
  → Component context → not found
    → Root context → FOUND: "1.0"

Lookup "localValue" from Rectangle:
  Rectangle context → FOUND: "local"
```

### Property Binding Mechanism

```
QML Binding:
    width: parent.width * 0.5

Compilation:
┌────────────────────────────────────────────┐
│  QQmlBinding created                        │
│  - Expression: "parent.width * 0.5"        │
│  - Target: this object                     │
│  - Target property: "width"                │
│  - Compiled to V4 bytecode                 │
└────────────────┬───────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────┐
│  Dependency tracking during evaluation      │
│                                            │
│  Evaluation:                                │
│  1. Access "parent"                        │
│     → Subscribe to parent property changes  │
│  2. Access "parent.width"                  │
│     → Subscribe to width property changes   │
│  3. Calculate result: 100 * 0.5 = 50       │
│  4. Set this.width = 50                    │
└────────────────┬───────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────┐
│  Dependencies registered:                   │
│  - parent.width NOTIFY signal              │
│     → connected to binding update          │
└────────────────┬───────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────┐
│  When parent.width changes:                 │
│  1. Emit widthChanged() signal             │
│  2. Signal triggers binding re-evaluation   │
│  3. QQmlBinding::update()                  │
│  4. Re-run bytecode                        │
│  5. Update target property                 │
└────────────────────────────────────────────┘

Optimization: Binding Removal
  If binding result is constant:
    → Remove binding, set constant value
    → Save memory and CPU cycles
```

---

## Signal-Slot Mechanism

### Meta-Object System

```
C++ Declaration:
    class MyClass : public QObject {
        Q_OBJECT
    signals:
        void valueChanged(int newValue);
    public slots:
        void setValue(int value);
    };

MOC (Meta-Object Compiler) generates:
┌────────────────────────────────────────────┐
│  MyClass::staticMetaObject                  │
│  - Class name: "MyClass"                   │
│  - Super class: "QObject"                  │
│  - Methods: [valueChanged, setValue, ...]  │
│  - Properties: [...]                       │
│  - Signals: [valueChanged = index 0]       │
│  - Slots: [setValue = index 1]             │
└────────────────────────────────────────────┘

Connection: connect(obj, &MyClass::valueChanged, receiver, &Receiver::onValueChanged)
┌────────────────────────────────────────────┐
│  QMetaObject::Connection created            │
│  - Sender: obj                             │
│  - Signal: valueChanged (index 0)          │
│  - Receiver: receiver                      │
│  - Slot: onValueChanged                    │
│  - Connection type: Auto (Qt::AutoConnect) │
└────────────────┬───────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────┐
│  Connection stored in sender's connection   │
│  list (QObjectPrivate::connectionLists)    │
└────────────────────────────────────────────┘

Signal Emission: emit valueChanged(42)
                 ↓
    (Expands to: QMetaObject::activate(this, &staticMetaObject, 0, args))
                 │
                 ▼
┌────────────────────────────────────────────┐
│  QMetaObject::activate()                    │
│  1. Get connection list for signal index 0  │
│  2. For each connection:                   │
│     - Check if receiver still alive        │
│     - Check connection type (direct/queued)│
│     - Call slot or queue event             │
└────────────────┬───────────────────────────┘
                 │
        ┌────────┴─────────┐
        │ Connection Type? │
        └────┬────────────┬┘
      Direct │            │ Queued
             ▼            ▼
┌──────────────────┐  ┌──────────────────────┐
│  Call directly:  │  │  Post event:         │
│  receiver->      │  │  QCoreApplication::  │
│  onValueChanged()│  │  postEvent(receiver, │
│                  │  │  MetaCallEvent)      │
└──────────────────┘  └──────────────────────┘
```

---

## Memory Management

### QObject Ownership

```
Rule 1: Parent-Child Ownership
    QObject *parent = new QObject();
    QObject *child = new QObject(parent);
    
    Relationship:
    ┌─────────────────┐
    │     parent      │
    │  children: [    │
    │    child ───────┼──→ ┌─────────────┐
    │  ]              │    │    child    │
    └─────────────────┘    │ parent: ◄───┼─┐
                           └─────────────┘ │
                                           │
    delete parent;  ────────────────────────┘
        ↓                                   ↓
    Deletes child automatically         child deleted

Rule 2: QML Object Ownership
    Component {
        id: myComponent
        Rectangle { id: rect }
    }
    
    var obj = myComponent.createObject(parent)
    
    Ownership:
    ┌──────────────────────────────────┐
    │  QQmlEngine::JavaScriptOwnership  │  ← Default for createObject()
    │  - Owned by JS garbage collector │
    │  - Deleted when no JS references │
    └──────────────────────────────────┘
    
    vs
    
    ┌──────────────────────────────────┐
    │  QQmlEngine::CppOwnership         │  ← For C++ created objects
    │  - Owned by C++                  │
    │  - Deleted explicitly or by parent│
    └──────────────────────────────────┘

Rule 3: Context Property Lifetime
    DataModel *model = new DataModel();
    engine.rootContext()->setContextProperty("model", model);
    
    ⚠️  WARNING: Engine doesn't take ownership!
    ⚠️  YOU must ensure model outlives engine
    
    Solutions:
    1. Make model child of engine
       DataModel *model = new DataModel(&engine);
    
    2. Use QScopedPointer
       QScopedPointer<DataModel> model(new DataModel());
    
    3. Stack allocation (if possible)
       DataModel model;
       engine.rootContext()->setContextProperty("model", &model);

Memory Leak Example:
    void createWindow() {
        QQuickView *view = new QQuickView();  // ← NO PARENT!
        view->setSource(...);
        view->show();
    }  // ← view leaked! Window stays open but pointer lost
    
    Fix:
    QQuickView *view = new QQuickView();
    view->setAttribute(Qt::WA_DeleteOnClose);  // Delete when closed
    // Or:
    connect(view, &QQuickView::closing, view, &QQuickView::deleteLater);
```

---

## Summary Cheat Sheet

```
┌─────────────────────────────────────────────────────────────┐
│  Quick Reference                                             │
├─────────────────────────────────────────────────────────────┤
│  QGuiApplication                                            │
│  ├─ Entry point for GUI applications                        │
│  ├─ Manages event loop (app.exec())                        │
│  ├─ Platform integration (QPA)                             │
│  └─ Override notify() for custom event handling            │
├─────────────────────────────────────────────────────────────┤
│  QQmlApplicationEngine                                      │
│  ├─ Inherits QQmlEngine                                    │
│  ├─ load() to load QML files                              │
│  ├─ rootContext() for context properties                   │
│  └─ Handles window lifetime automatically                  │
├─────────────────────────────────────────────────────────────┤
│  QQmlContext                                                │
│  ├─ setContextProperty() to expose C++ to QML             │
│  ├─ Hierarchical: child → parent → root                   │
│  ├─ Use QObject* for better performance                    │
│  └─ Remember: context doesn't own the objects!            │
├─────────────────────────────────────────────────────────────┤
│  Common Patterns                                            │
│  ├─ DataModel with Q_PROPERTY and NOTIFY                  │
│  ├─ Q_INVOKABLE for calling C++ from QML                  │
│  ├─ Use QObject parent for automatic deletion              │
│  └─ Connect QML signals to C++ slots                      │
└─────────────────────────────────────────────────────────────┘
```

---

**Tips:**
- Vẽ diagrams khi đọc code để hiểu flow
- Sử dụng debugger để trace execution
- Đọc tests để hiểu expected behavior
- Tham khảo Qt documentation thường xuyên

Happy Learning! 📚
