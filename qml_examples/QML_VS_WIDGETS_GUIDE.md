# 🎨 QML vs Qt Widgets - Complete Comparison

## 📊 Side-by-Side Comparison

### Todo List Example

#### Qt Widgets Approach (C++ only):

```cpp
// All in C++
class TodoListModel : public QAbstractListModel {
    // Model code
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Create model
    TodoListModel *model = new TodoListModel;
    
    // Create view (C++)
    QListView *view = new QListView;
    view->setModel(model);
    
    // Create widgets (C++)
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    QLineEdit *input = new QLineEdit;
    QPushButton *addBtn = new QPushButton("Add");
    
    // Connect signals
    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        model->addTodo(input->text());
    });
    
    // Layout
    layout->addWidget(view);
    layout->addWidget(input);
    layout->addWidget(addBtn);
    
    window.show();
    return app.exec();
}
```

**Pros:**
- ✅ Fast compilation
- ✅ Type-safe
- ✅ Good for desktop
- ✅ Mature, stable

**Cons:**
- ❌ Verbose UI code
- ❌ Hard to animate
- ❌ Poor mobile support
- ❌ No hot reload

---

#### QML Approach (Model C++ + View QML):

**C++ Model:**
```cpp
// models/TodoListModel.h
class TodoListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    Q_INVOKABLE void addTodo(const QString &text, int priority);
    
    QHash<int, QByteArray> roleNames() const override {
        return {
            {TextRole, "text"},
            {CompletedRole, "completed"}
        };
    }
    
signals:
    void countChanged();
};
```

**main.cpp:**
```cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    TodoListModel model;
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("todoModel", &model);
    engine.load(QUrl("qrc:/qml/TodoListView.qml"));
    
    return app.exec();
}
```

**QML View (TodoListView.qml):**
```qml
ApplicationWindow {
    ColumnLayout {
        // Input
        RowLayout {
            TextField {
                id: input
                placeholderText: "New todo..."
            }
            Button {
                text: "Add"
                onClicked: todoModel.addTodo(input.text, 1)
            }
        }
        
        // List
        ListView {
            model: todoModel
            delegate: Rectangle {
                RowLayout {
                    CheckBox {
                        checked: model.completed
                        onClicked: todoModel.toggleCompleted(index)
                    }
                    Label {
                        text: model.text
                    }
                }
            }
        }
        
        // Statistics
        Label {
            text: "Total: " + todoModel.count
        }
    }
}
```

**Pros:**
- ✅ Declarative UI (easy to read)
- ✅ Built-in animations
- ✅ Hot reload (during development)
- ✅ Great for mobile
- ✅ Modern look & feel
- ✅ Less code for UI

**Cons:**
- ❌ Learning curve (new syntax)
- ❌ Runtime errors (not compile-time)
- ❌ Debugging can be harder

---

## 🔄 Data Flow Comparison

### Qt Widgets:
```
User clicks button
    ↓
Signal emitted
    ↓
Slot in C++ called
    ↓
Model updated
    ↓
View notified (dataChanged signal)
    ↓
View re-paints
```

### QML:
```
User clicks Button
    ↓
onClicked handler (QML)
    ↓
Calls C++ Q_INVOKABLE method
    ↓
Model updated (C++)
    ↓
Signal emitted (C++)
    ↓
QML property binding updates automatically
    ↓
View re-renders
```

---

## 📏 Code Size Comparison

### Creating a Simple Form:

**Qt Widgets (60 lines C++):**
```cpp
QWidget *window = new QWidget;
QFormLayout *layout = new QFormLayout(window);

QLineEdit *nameEdit = new QLineEdit;
layout->addRow("Name:", nameEdit);

QSpinBox *ageSpinBox = new QSpinBox;
ageSpinBox->setRange(0, 100);
layout->addRow("Age:", ageSpinBox);

QComboBox *gradeCombo = new QComboBox;
gradeCombo->addItems({"A", "B", "C", "D"});
layout->addRow("Grade:", gradeCombo);

QPushButton *submitBtn = new QPushButton("Submit");
layout->addRow(submitBtn);

QObject::connect(submitBtn, &QPushButton::clicked, [=]() {
    QString name = nameEdit->text();
    int age = ageSpinBox->value();
    QString grade = gradeCombo->currentText();
    // Process...
});

window->show();
```

**QML (25 lines):**
```qml
Window {
    FormLayout {
        TextField {
            id: nameField
            placeholderText: "Name"
        }
        
        SpinBox {
            id: ageSpinBox
            from: 0
            to: 100
        }
        
        ComboBox {
            id: gradeCombo
            model: ["A", "B", "C", "D"]
        }
        
        Button {
            text: "Submit"
            onClicked: {
                myModel.submit(nameField.text, ageSpinBox.value, gradeCombo.currentText)
            }
        }
    }
}
```

**Winner:** QML (58% less code for UI)

---

## 🎨 Animation Comparison

### Fade-in Animation:

**Qt Widgets:**
```cpp
QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
widget->setGraphicsEffect(effect);

QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
animation->setDuration(1000);
animation->setStartValue(0.0);
animation->setEndValue(1.0);
animation->start();
```

**QML:**
```qml
Rectangle {
    opacity: 0
    
    OpacityAnimator on opacity {
        from: 0
        to: 1
        duration: 1000
    }
}

// Or even simpler:
Rectangle {
    Behavior on opacity {
        NumberAnimation { duration: 1000 }
    }
}
```

**Winner:** QML (much easier)

---

## 🔧 Model Integration

### Exposing C++ Model:

**Qt Widgets:**
```cpp
// Just set model
view->setModel(model);
// Done!
```

**QML:**
```cpp
// Need to expose to QML context
engine.rootContext()->setContextProperty("myModel", &model);

// AND implement roleNames()
QHash<int, QByteArray> MyModel::roleNames() const {
    return {
        {TextRole, "text"},
        {ValueRole, "value"}
    };
}

// AND use Q_INVOKABLE for methods
Q_INVOKABLE void addItem(const QString &text);
```

**Winner:** Qt Widgets (simpler)

---

## 📱 Platform Support

| Platform | Qt Widgets | QML |
|----------|------------|-----|
| **Windows Desktop** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **macOS Desktop** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Linux Desktop** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Android** | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **iOS** | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Embedded** | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **WebAssembly** | ⭐⭐ | ⭐⭐⭐⭐ |

---

## 🏆 When to Use What?

### Use Qt Widgets When:

✅ Building traditional desktop application
✅ Complex forms with many controls
✅ Need native OS look & feel
✅ Target is primarily Windows/macOS/Linux desktop
✅ Team already knows Qt Widgets
✅ Need widgets like QTreeWidget, QDockWidget
✅ Integrating with existing Qt Widgets codebase

**Example Apps:**
- IDE (Qt Creator)
- Office suites
- CAD software
- Database tools
- System utilities

---

### Use QML When:

✅ Building mobile app (Android/iOS)
✅ Modern, animated UIs
✅ Touch interfaces
✅ Rapid prototyping
✅ Cross-platform (desktop + mobile)
✅ Custom, unique designs
✅ Games or media apps
✅ Embedded systems

**Example Apps:**
- Mobile apps
- Media players
- Smart home controls
- Car dashboards
- Kiosks
- Games

---

## 💰 Development Cost

### Qt Widgets:
- **Learning Time:** 1-2 weeks (if know C++)
- **Development Speed:** Medium
- **Maintenance:** Medium
- **Designer Tools:** Qt Designer
- **Hot Reload:** ❌ No (need rebuild)

### QML:
- **Learning Time:** 1 week (QML syntax easy)
- **Development Speed:** Fast (declarative)
- **Maintenance:** Easy (UI separated)
- **Designer Tools:** Qt Design Studio
- **Hot Reload:** ✅ Yes (qmlscene)

---

## 🔀 Hybrid Approach

You can mix both!

```cpp
// Main window: Qt Widgets
QMainWindow *window = new QMainWindow;

// Embed QML view
QQuickWidget *qmlWidget = new QQuickWidget;
qmlWidget->setSource(QUrl("qrc:/MyView.qml"));
window->setCentralWidget(qmlWidget);

// Expose C++ to QML
qmlWidget->rootContext()->setContextProperty("cppModel", &model);
```

**Best of both worlds:**
- Use Qt Widgets for main application frame
- Use QML for dynamic, animated content areas

---

## 📊 Performance Comparison

### Rendering:

| Operation | Qt Widgets | QML |
|-----------|------------|-----|
| Static UI | Fast | Fast |
| Animations | Medium | Very Fast (GPU) |
| Complex graphics | Slow | Fast (Scene Graph) |
| Startup time | Fast | Medium |
| Memory usage | Low | Medium |

### Model Operations:

**Same performance!** Both use same C++ models underneath.

---

## 🎓 Learning Resources

### Qt Widgets:
- [Qt Widgets Documentation](https://doc.qt.io/qt-5/qtwidgets-index.html)
- Book: "C++ GUI Programming with Qt 5"
- Mature, lots of examples

### QML:
- [QML Tutorial](https://doc.qt.io/qt-5/qmlfirststeps.html)
- [Qt Quick Controls](https://doc.qt.io/qt-5/qtquickcontrols-index.html)
- Interactive examples in Qt Creator

---

## 📝 Migration Guide

### From Qt Widgets to QML:

1. **Keep Models in C++**
   ```cpp
   // No change needed!
   class MyModel : public QAbstractListModel { ... };
   ```

2. **Add Q_INVOKABLE and roleNames()**
   ```cpp
   Q_INVOKABLE void addItem(const QString &text);
   QHash<int, QByteArray> roleNames() const override;
   ```

3. **Rewrite Views in QML**
   ```qml
   ListView {
       model: myModel
       delegate: Text { text: model.name }
   }
   ```

4. **Update main()**
   ```cpp
   QGuiApplication app(argc, argv);
   QQmlApplicationEngine engine;
   engine.rootContext()->setContextProperty("myModel", &model);
   engine.load(QUrl("qrc:/main.qml"));
   ```

---

## 🎯 Recommendation

### For Your Qt Models Learning:

**Best Approach:** Learn BOTH!

1. **Start with Qt Widgets** (current examples)
   - Understand Models deeply
   - Practice with familiar C++
   - Good for desktop apps

2. **Then Learn QML** (qml_examples/)
   - Keep same models
   - Create modern UIs
   - Prepare for mobile

3. **Final Project:** Combine both
   - C++ Models (business logic)
   - QML Views (UI)
   - Best of both worlds!

---

## 📦 Project Structure Recommendation

```
my_app/
├── src/
│   ├── models/          # C++ Models (shared)
│   │   ├── TodoModel.h
│   │   └── TodoModel.cpp
│   │
│   ├── widgets/         # Qt Widgets version
│   │   └── main_widgets.cpp
│   │
│   └── qml/             # QML version
│       ├── main_qml.cpp
│       └── views/
│           └── TodoView.qml
│
└── CMakeLists.txt       # Build both versions
```

**Same models, different UIs!**

---

**Choose wisely based on your target platform and requirements! 🎯**
