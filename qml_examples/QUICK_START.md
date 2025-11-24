# ⚡ QML Examples - Quick Start

## 🚀 5 Phút Chạy Được!

### Bước 1: Cài Đặt Qt Quick

```bash
# Ubuntu/Debian
sudo apt install qtdeclarative5-dev qml-module-qtquick-controls2

# macOS (Qt đã có Quick)
brew install qt@5

# Kiểm tra
qmake --version  # Phải có Qt Quick
```

### Bước 2: Build

```bash
cd /workspace/qml_examples
mkdir build && cd build
cmake ..
make
```

### Bước 3: Run

```bash
# Todo List
./qml_todo_list

# Student Table
./qml_student_table
```

---

## 📖 Hiểu Nhanh Kiến Trúc

### 3 Thành Phần Chính:

```
1. C++ MODEL (Business Logic)
   ↓
2. main.cpp (Kết nối)
   ↓
3. QML VIEW (UI)
```

### Example: Todo List

**1. Model (C++) - `models/TodoListModel.h`:**
```cpp
class TodoListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    Q_INVOKABLE void addTodo(const QString &text, int priority);
    // ... other methods
    
    QHash<int, QByteArray> roleNames() const override {
        return {{"text", TextRole}, {"completed", CompletedRole}};
    }
};
```

**2. Main (Kết nối) - `main_todo.cpp`:**
```cpp
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    TodoListModel todoModel;  // C++ model
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("todoModel", &todoModel);
    // ↑ Expose model sang QML với tên "todoModel"
    
    engine.load(QUrl("qrc:/qml/TodoListView.qml"));
    return app.exec();
}
```

**3. View (QML) - `qml/TodoListView.qml`:**
```qml
ListView {
    model: todoModel  // Access C++ model
    
    delegate: Rectangle {
        Text {
            text: model.text        // Access role "text"
        }
        CheckBox {
            checked: model.completed  // Access role "completed"
        }
    }
}

Button {
    onClicked: {
        todoModel.addTodo("New task", 1)  // Call C++ method
    }
}
```

---

## 🔑 Key Points

### 1. Q_PROPERTY
```cpp
Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
```
→ QML có thể access `todoModel.count`

### 2. Q_INVOKABLE
```cpp
Q_INVOKABLE void addTodo(const QString &text, int priority);
```
→ QML có thể gọi `todoModel.addTodo("text", 1)`

### 3. roleNames()
```cpp
QHash<int, QByteArray> roleNames() const override {
    return {{"text", TextRole}};
}
```
→ QML có thể access `model.text` trong delegate

### 4. setContextProperty()
```cpp
engine.rootContext()->setContextProperty("todoModel", &todoModel);
```
→ Expose C++ object sang QML với tên "todoModel"

---

## 📝 Tạo Model Mới

### Template:

**MyModel.h:**
```cpp
#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractListModel>

class MyModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ValueRole
    };
    
    explicit MyModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    Q_INVOKABLE void addItem(const QString &name, int value);
    
signals:
    void countChanged();
    
private:
    struct Item {
        QString name;
        int value;
    };
    QVector<Item> m_items;
};

#endif
```

**MyModel.cpp:**
```cpp
#include "MyModel.h"

MyModel::MyModel(QObject *parent) : QAbstractListModel(parent) {}

int MyModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.count();
}

QVariant MyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size())
        return QVariant();
    
    const Item &item = m_items.at(index.row());
    
    switch (role) {
        case NameRole: return item.name;
        case ValueRole: return item.value;
    }
    return QVariant();
}

QHash<int, QByteArray> MyModel::roleNames() const {
    return {
        {NameRole, "name"},
        {ValueRole, "value"}
    };
}

void MyModel::addItem(const QString &name, int value) {
    int row = m_items.count();
    beginInsertRows(QModelIndex(), row, row);
    m_items.append({name, value});
    endInsertRows();
    emit countChanged();
}
```

**main.cpp:**
```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MyModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    MyModel myModel;
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("myModel", &myModel);
    engine.load(QUrl("qrc:/MyView.qml"));
    
    return app.exec();
}
```

**MyView.qml:**
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 600
    
    Column {
        ListView {
            width: 400
            height: 500
            model: myModel
            
            delegate: Rectangle {
                width: 400
                height: 40
                
                Text {
                    text: model.name + ": " + model.value
                }
            }
        }
        
        Button {
            text: "Add Item"
            onClicked: myModel.addItem("Item", 100)
        }
    }
}
```

---

## 🎨 QML UI Basics

### Layout:
```qml
ColumnLayout {
    RowLayout {
        Label { text: "Name:" }
        TextField { id: nameField }
    }
    Button { text: "Submit" }
}
```

### ListView:
```qml
ListView {
    model: myModel
    delegate: Rectangle {
        Text { text: model.name }
    }
}
```

### Signals & Slots:
```qml
Button {
    onClicked: {
        console.log("Clicked!")
        myModel.doSomething()
    }
}
```

### Property Binding:
```qml
Label {
    text: "Count: " + myModel.count
    // Auto-updates when myModel.count changes!
}
```

---

## 🐛 Common Errors

### Error 1: "todoModel is not defined"

**Cause:** Forgot setContextProperty

**Fix:**
```cpp
engine.rootContext()->setContextProperty("todoModel", &todoModel);
```

### Error 2: "Cannot assign to non-existent property"

**Cause:** Role name typo

**Fix:**
```cpp
// C++
roles[TextRole] = "text";  // Lowercase!

// QML
Text { text: model.text }  // Match role name
```

### Error 3: Model doesn't update

**Cause:** Forgot emit signals

**Fix:**
```cpp
void addItem() {
    beginInsertRows(...);
    m_items.append(...);
    endInsertRows();
    emit countChanged();  // ← Don't forget!
}
```

---

## 📚 Next Steps

1. ✅ Run examples
2. ✅ Modify QML files (UI)
3. ✅ Add methods to models
4. ✅ Create your own model + view
5. → Read full documentation: `README.md`
6. → Compare with Qt Widgets: `QML_VS_WIDGETS_GUIDE.md`

---

## 💡 Tips

- **Hot reload:** Use `qmlscene MyView.qml` for fast iteration
- **Debug:** Add `console.log("value:", myVar)` in QML
- **Qt Creator:** Has QML editor with autocomplete
- **Documentation:** F1 in Qt Creator shows docs

---

**Start coding! 🎉**
