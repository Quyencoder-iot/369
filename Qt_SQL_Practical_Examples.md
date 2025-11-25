# Qt SQL - Ví Dụ Thực Tế (C++ Model + QML View)

> **Architecture:** Mỗi example gồm 3 files chính: Model (C++), View (QML), main.cpp

## 📋 Table of Contents

1. [Example 1: Basic CRUD - Simple Todo App](#example-1-basic-crud---simple-todo-app)
2. [Example 2: User Management with Material Design](#example-2-user-management-with-material-design)
3. [Example 3: Real-time Dashboard](#example-3-real-time-dashboard)
4. [Example 4: Multi-page App with Navigation](#example-4-multi-page-app-with-navigation)
5. [Example 5: Advanced Filtering & Search](#example-5-advanced-filtering--search)
6. [Example 6: Transaction Management](#example-6-transaction-management)
7. [Example 7: Master-Detail View](#example-7-master-detail-view)
8. [Example 8: Production App with Clean Architecture](#example-8-production-app-with-clean-architecture)

---

## Example 1: Basic CRUD - Simple Todo App

### 📁 Project Structure
```
todo-app/
├── todomodel.h          # Model header
├── todomodel.cpp        # Model implementation
├── main.qml             # QML view
├── main.cpp             # Application entry
├── todo.pro             # qmake project file
├── CMakeLists.txt       # CMake project file
└── qml.qrc              # Resource file
```

### 📄 todomodel.h

```cpp
#ifndef TODOMODEL_H
#define TODOMODEL_H

#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QSqlQuery>

struct TodoItem {
    int id;
    QString title;
    bool completed;
};

class TodoModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    enum TodoRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        CompletedRole
    };
    
    explicit TodoModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Todo operations
    Q_INVOKABLE bool addTodo(const QString &title);
    Q_INVOKABLE bool removeTodo(int index);
    Q_INVOKABLE bool toggleCompleted(int index);
    Q_INVOKABLE bool updateTitle(int index, const QString &newTitle);
    Q_INVOKABLE void refresh();
    
signals:
    void countChanged();
    void errorOccurred(const QString &message);
    
private:
    void loadTodos();
    bool initDatabase();
    
    QList<TodoItem> m_todos;
    QSqlDatabase m_db;
};

#endif // TODOMODEL_H
```

### 📄 todomodel.cpp

```cpp
#include "todomodel.h"
#include <QSqlError>
#include <QDebug>

TodoModel::TodoModel(QObject *parent)
    : QAbstractListModel(parent)
{
    if (initDatabase()) {
        loadTodos();
    }
}

bool TodoModel::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("todos.db");
    
    if (!m_db.open()) {
        emit errorOccurred("Cannot open database: " + m_db.lastError().text());
        return false;
    }
    
    QSqlQuery query(m_db);
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS todos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "completed INTEGER DEFAULT 0)");
    
    if (!success) {
        emit errorOccurred("Cannot create table: " + query.lastError().text());
        return false;
    }
    
    return true;
}

void TodoModel::loadTodos()
{
    beginResetModel();
    m_todos.clear();
    
    QSqlQuery query("SELECT id, title, completed FROM todos ORDER BY id", m_db);
    
    while (query.next()) {
        TodoItem item;
        item.id = query.value(0).toInt();
        item.title = query.value(1).toString();
        item.completed = query.value(2).toBool();
        m_todos.append(item);
    }
    
    endResetModel();
    emit countChanged();
}

int TodoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_todos.count();
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_todos.count())
        return QVariant();
    
    const TodoItem &item = m_todos[index.row()];
    
    switch (role) {
    case IdRole: return item.id;
    case TitleRole: return item.title;
    case CompletedRole: return item.completed;
    }
    
    return QVariant();
}

QHash<int, QByteArray> TodoModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "todoId";
    roles[TitleRole] = "title";
    roles[CompletedRole] = "completed";
    return roles;
}

bool TodoModel::addTodo(const QString &title)
{
    if (title.isEmpty()) {
        emit errorOccurred("Title cannot be empty");
        return false;
    }
    
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO todos (title, completed) VALUES (?, 0)");
    query.addBindValue(title);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to add todo: " + query.lastError().text());
        return false;
    }
    
    loadTodos();
    return true;
}

bool TodoModel::removeTodo(int index)
{
    if (index < 0 || index >= m_todos.count())
        return false;
    
    int todoId = m_todos[index].id;
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM todos WHERE id = ?");
    query.addBindValue(todoId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to delete todo: " + query.lastError().text());
        return false;
    }
    
    loadTodos();
    return true;
}

bool TodoModel::toggleCompleted(int index)
{
    if (index < 0 || index >= m_todos.count())
        return false;
    
    int todoId = m_todos[index].id;
    bool newCompleted = !m_todos[index].completed;
    
    QSqlQuery query(m_db);
    query.prepare("UPDATE todos SET completed = ? WHERE id = ?");
    query.addBindValue(newCompleted ? 1 : 0);
    query.addBindValue(todoId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to update todo: " + query.lastError().text());
        return false;
    }
    
    loadTodos();
    return true;
}

bool TodoModel::updateTitle(int index, const QString &newTitle)
{
    if (index < 0 || index >= m_todos.count() || newTitle.isEmpty())
        return false;
    
    int todoId = m_todos[index].id;
    
    QSqlQuery query(m_db);
    query.prepare("UPDATE todos SET title = ? WHERE id = ?");
    query.addBindValue(newTitle);
    query.addBindValue(todoId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to update todo: " + query.lastError().text());
        return false;
    }
    
    loadTodos();
    return true;
}

void TodoModel::refresh()
{
    loadTodos();
}
```

### 📄 main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 600
    height: 800
    title: "Todo App - Qt SQL"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // Header
        Rectangle {
            Layout.fillWidth: true
            height: 80
            color: "#2196F3"
            radius: 10
            
            ColumnLayout {
                anchors.centerIn: parent
                
                Label {
                    text: "My Todos"
                    font.pixelSize: 28
                    font.bold: true
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter
                }
                
                Label {
                    text: todoModel.count + " tasks"
                    font.pixelSize: 14
                    color: "white"
                    opacity: 0.8
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
        
        // Add todo input
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            TextField {
                id: todoInput
                Layout.fillWidth: true
                placeholderText: "What needs to be done?"
                font.pixelSize: 16
                
                Keys.onReturnPressed: addButton.clicked()
            }
            
            Button {
                id: addButton
                text: "Add"
                highlighted: true
                enabled: todoInput.text.length > 0
                
                onClicked: {
                    if (todoModel.addTodo(todoInput.text)) {
                        todoInput.text = ""
                    }
                }
            }
        }
        
        // Todo list
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            
            model: todoModel
            
            delegate: Rectangle {
                width: listView.width
                height: 70
                color: "#f5f5f5"
                radius: 8
                border.color: "#ddd"
                border.width: 1
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10
                    
                    // Checkbox
                    CheckBox {
                        checked: model.completed
                        onClicked: todoModel.toggleCompleted(index)
                    }
                    
                    // Title
                    Label {
                        Layout.fillWidth: true
                        text: model.title
                        font.pixelSize: 16
                        font.strikeout: model.completed
                        opacity: model.completed ? 0.5 : 1.0
                        wrapMode: Text.WordWrap
                    }
                    
                    // Edit button
                    Button {
                        text: "✏️"
                        flat: true
                        onClicked: {
                            editDialog.editIndex = index
                            editDialog.editText = model.title
                            editDialog.open()
                        }
                    }
                    
                    // Delete button
                    Button {
                        text: "🗑️"
                        flat: true
                        onClicked: {
                            deleteDialog.deleteIndex = index
                            deleteDialog.open()
                        }
                    }
                }
            }
            
            // Empty state
            Label {
                anchors.centerIn: parent
                text: "No todos yet!\nAdd one above to get started 🚀"
                font.pixelSize: 18
                color: "#999"
                horizontalAlignment: Text.AlignHCenter
                visible: listView.count === 0
            }
        }
        
        // Stats
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "#f0f0f0"
            radius: 8
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                
                Label {
                    text: "Total: " + todoModel.count
                    font.pixelSize: 14
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: {
                        var completed = 0;
                        for (var i = 0; i < todoModel.count; i++) {
                            if (todoModel.data(todoModel.index(i, 0), 258)) // CompletedRole
                                completed++;
                        }
                        return "Completed: " + completed;
                    }
                    font.pixelSize: 14
                    color: "#4CAF50"
                }
            }
        }
    }
    
    // Edit Dialog
    Dialog {
        id: editDialog
        title: "Edit Todo"
        width: 400
        standardButtons: Dialog.Save | Dialog.Cancel
        
        property int editIndex: -1
        property alias editText: editField.text
        
        TextField {
            id: editField
            width: parent.width
            placeholderText: "Enter new title"
        }
        
        onAccepted: {
            if (editField.text.length > 0) {
                todoModel.updateTitle(editIndex, editField.text)
            }
        }
    }
    
    // Delete Confirmation Dialog
    Dialog {
        id: deleteDialog
        title: "Confirm Delete"
        width: 400
        standardButtons: Dialog.Yes | Dialog.No
        
        property int deleteIndex: -1
        
        Label {
            text: "Are you sure you want to delete this todo?"
        }
        
        onAccepted: {
            todoModel.removeTodo(deleteIndex)
        }
    }
    
    // Error toast
    Popup {
        id: errorPopup
        anchors.centerIn: parent
        width: 300
        height: 100
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        
        property alias message: errorLabel.text
        
        Rectangle {
            anchors.fill: parent
            color: "#f44336"
            radius: 8
            
            Label {
                id: errorLabel
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
        }
    }
    
    Connections {
        target: todoModel
        function onErrorOccurred(message) {
            errorPopup.message = message
            errorPopup.open()
        }
    }
}
```

### 📄 main.cpp

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "todomodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set application info
    app.setOrganizationName("MyCompany");
    app.setApplicationName("TodoApp");
    
    // Create model
    TodoModel todoModel;
    
    // Setup QML engine
    QQmlApplicationEngine engine;
    
    // Expose model to QML
    engine.rootContext()->setContextProperty("todoModel", &todoModel);
    
    // Load QML
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
```

### 📄 todo.pro

```qmake
QT += quick sql
CONFIG += c++17

# Input files
SOURCES += \
    main.cpp \
    todomodel.cpp

HEADERS += \
    todomodel.h

RESOURCES += qml.qrc

# Output
TARGET = todo-app
TEMPLATE = app

# Install
target.path = /usr/local/bin
INSTALLS += target
```

### 📄 qml.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>main.qml</file>
    </qresource>
</RCC>
```

### 📄 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(todo-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql)

# Add executable
qt_add_executable(todo-app
    main.cpp
    todomodel.h
    todomodel.cpp
)

# Add QML module
qt_add_qml_module(todo-app
    URI TodoApp
    VERSION 1.0
    QML_FILES main.qml
)

# Link libraries
target_link_libraries(todo-app PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Sql
)

# Install
install(TARGETS todo-app
    RUNTIME DESTINATION bin
)
```

### 📄 Makefile (Manual - Optional)

```makefile
# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fPIC

# Qt paths (adjust for your system)
QT_PATH = /usr/lib/qt6
QT_INCLUDE = $(QT_PATH)/include
QT_LIBS = $(QT_PATH)/lib

# Include paths
INCLUDES = -I$(QT_INCLUDE) \
           -I$(QT_INCLUDE)/QtCore \
           -I$(QT_INCLUDE)/QtGui \
           -I$(QT_INCLUDE)/QtQuick \
           -I$(QT_INCLUDE)/QtQml \
           -I$(QT_INCLUDE)/QtSql

# Libraries
LIBS = -L$(QT_LIBS) \
       -lQt6Core -lQt6Gui -lQt6Quick -lQt6Qml -lQt6Sql

# Files
SOURCES = main.cpp todomodel.cpp
HEADERS = todomodel.h
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = todo-app

# Build
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) moc_*.cpp moc_*.o

.PHONY: all clean
```

### 🔨 Build & Run

#### Using qmake (Recommended):
```bash
# Generate Makefile
qmake todo.pro

# Build
make

# Run
./todo-app
```

#### Using CMake:
```bash
# Configure
cmake -B build

# Build
cmake --build build

# Run
./build/todo-app
```

#### Using Qt Creator:
1. Open `todo.pro`
2. Click "Configure Project"
3. Click Run (Ctrl+R)

---

## Example 2: User Management with Material Design

### 📁 Project Structure
```
user-manager/
├── usermanager.h
├── usermanager.cpp
├── main.qml
├── main.cpp
├── user-manager.pro
└── qml.qrc
```

### 📄 usermanager.h

```cpp
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QAbstractListModel>
#include <QSqlDatabase>

struct User {
    int id;
    QString name;
    QString email;
    int age;
    QString avatar;
};

class UserManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    
public:
    enum UserRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        EmailRole,
        AgeRole,
        AvatarRole
    };
    
    explicit UserManager(QObject *parent = nullptr);
    ~UserManager();
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    QString searchText() const { return m_searchText; }
    void setSearchText(const QString &text);
    
    Q_INVOKABLE bool addUser(const QString &name, const QString &email, int age);
    Q_INVOKABLE bool updateUser(int index, const QString &name, const QString &email, int age);
    Q_INVOKABLE bool deleteUser(int index);
    Q_INVOKABLE QVariantMap getUser(int index) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void sortBy(const QString &field, bool ascending);
    
signals:
    void countChanged();
    void searchTextChanged();
    void errorOccurred(const QString &message);
    void userAdded();
    void userUpdated();
    void userDeleted();
    
private:
    void loadUsers();
    bool initDatabase();
    QString generateAvatar(const QString &name);
    
    QList<User> m_users;
    QSqlDatabase m_db;
    QString m_searchText;
    QString m_sortField;
    bool m_sortAscending;
};

#endif
```

### 📄 usermanager.cpp

```cpp
#include "usermanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

UserManager::UserManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_sortField("name")
    , m_sortAscending(true)
{
    if (initDatabase()) {
        loadUsers();
    }
}

UserManager::~UserManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool UserManager::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("users.db");
    
    if (!m_db.open()) {
        emit errorOccurred("Cannot open database");
        return false;
    }
    
    QSqlQuery query(m_db);
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "email TEXT UNIQUE, "
        "age INTEGER, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)");
    
    if (!success) {
        emit errorOccurred("Cannot create table");
        return false;
    }
    
    return true;
}

void UserManager::loadUsers()
{
    beginResetModel();
    m_users.clear();
    
    QString sql = "SELECT id, name, email, age FROM users";
    
    if (!m_searchText.isEmpty()) {
        sql += " WHERE name LIKE :search OR email LIKE :search";
    }
    
    sql += QString(" ORDER BY %1 %2")
           .arg(m_sortField)
           .arg(m_sortAscending ? "ASC" : "DESC");
    
    QSqlQuery query(m_db);
    query.prepare(sql);
    
    if (!m_searchText.isEmpty()) {
        query.bindValue(":search", "%" + m_searchText + "%");
    }
    
    if (query.exec()) {
        while (query.next()) {
            User user;
            user.id = query.value(0).toInt();
            user.name = query.value(1).toString();
            user.email = query.value(2).toString();
            user.age = query.value(3).toInt();
            user.avatar = generateAvatar(user.name);
            m_users.append(user);
        }
    }
    
    endResetModel();
    emit countChanged();
}

QString UserManager::generateAvatar(const QString &name)
{
    if (name.isEmpty()) return "👤";
    
    QStringList avatars = {"👨", "👩", "👦", "👧", "🧑", "👴", "👵"};
    int index = qAbs(name[0].unicode()) % avatars.size();
    return avatars[index];
}

int UserManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_users.count();
}

QVariant UserManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_users.count())
        return QVariant();
    
    const User &user = m_users[index.row()];
    
    switch (role) {
    case IdRole: return user.id;
    case NameRole: return user.name;
    case EmailRole: return user.email;
    case AgeRole: return user.age;
    case AvatarRole: return user.avatar;
    }
    
    return QVariant();
}

QHash<int, QByteArray> UserManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "userId";
    roles[NameRole] = "name";
    roles[EmailRole] = "email";
    roles[AgeRole] = "age";
    roles[AvatarRole] = "avatar";
    return roles;
}

void UserManager::setSearchText(const QString &text)
{
    if (m_searchText != text) {
        m_searchText = text;
        loadUsers();
        emit searchTextChanged();
    }
}

bool UserManager::addUser(const QString &name, const QString &email, int age)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (name, email, age) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(age);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to add user: " + query.lastError().text());
        return false;
    }
    
    loadUsers();
    emit userAdded();
    return true;
}

bool UserManager::updateUser(int index, const QString &name, const QString &email, int age)
{
    if (index < 0 || index >= m_users.count())
        return false;
    
    int userId = m_users[index].id;
    
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET name = ?, email = ?, age = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(age);
    query.addBindValue(userId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to update user");
        return false;
    }
    
    loadUsers();
    emit userUpdated();
    return true;
}

bool UserManager::deleteUser(int index)
{
    if (index < 0 || index >= m_users.count())
        return false;
    
    int userId = m_users[index].id;
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to delete user");
        return false;
    }
    
    loadUsers();
    emit userDeleted();
    return true;
}

QVariantMap UserManager::getUser(int index) const
{
    QVariantMap map;
    
    if (index >= 0 && index < m_users.count()) {
        const User &user = m_users[index];
        map["id"] = user.id;
        map["name"] = user.name;
        map["email"] = user.email;
        map["age"] = user.age;
    }
    
    return map;
}

void UserManager::refresh()
{
    loadUsers();
}

void UserManager::sortBy(const QString &field, bool ascending)
{
    m_sortField = field;
    m_sortAscending = ascending;
    loadUsers();
}
```

### 📄 main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 900
    height: 700
    title: "User Manager"
    
    Material.theme: Material.Light
    Material.accent: Material.Blue
    
    header: ToolBar {
        Material.background: Material.Blue
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            
            Label {
                text: "User Management"
                font.pixelSize: 20
                font.bold: true
                color: "white"
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: userManager.count + " users"
                color: "white"
                font.pixelSize: 14
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // Search and actions
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search users..."
                Material.accent: Material.Blue
                
                onTextChanged: {
                    userManager.searchText = text
                }
            }
            
            Button {
                text: "Add User"
                highlighted: true
                onClicked: addDialog.open()
            }
            
            Button {
                text: "Sort"
                flat: true
                onClicked: sortMenu.open()
                
                Menu {
                    id: sortMenu
                    MenuItem {
                        text: "Name (A-Z)"
                        onTriggered: userManager.sortBy("name", true)
                    }
                    MenuItem {
                        text: "Name (Z-A)"
                        onTriggered: userManager.sortBy("name", false)
                    }
                    MenuItem {
                        text: "Age (Low-High)"
                        onTriggered: userManager.sortBy("age", true)
                    }
                    MenuItem {
                        text: "Age (High-Low)"
                        onTriggered: userManager.sortBy("age", false)
                    }
                }
            }
        }
        
        // User list
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10
            clip: true
            
            model: userManager
            
            delegate: ItemDelegate {
                width: listView.width
                height: 100
                
                contentItem: RowLayout {
                    spacing: 15
                    
                    // Avatar
                    Rectangle {
                        width: 70
                        height: 70
                        radius: 35
                        color: Material.color(Material.Blue, Material.Shade200)
                        
                        Label {
                            anchors.centerIn: parent
                            text: model.avatar
                            font.pixelSize: 32
                        }
                    }
                    
                    // Info
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        
                        Label {
                            text: model.name
                            font.pixelSize: 18
                            font.bold: true
                        }
                        
                        Label {
                            text: "📧 " + model.email
                            font.pixelSize: 14
                            color: Material.color(Material.Grey)
                        }
                        
                        Label {
                            text: "🎂 " + model.age + " years old"
                            font.pixelSize: 14
                            color: Material.color(Material.Grey)
                        }
                    }
                    
                    // Actions
                    ColumnLayout {
                        spacing: 5
                        
                        Button {
                            text: "Edit"
                            flat: true
                            Material.foreground: Material.Blue
                            onClicked: {
                                editDialog.editIndex = index
                                var user = userManager.getUser(index)
                                editDialog.editName = user.name
                                editDialog.editEmail = user.email
                                editDialog.editAge = user.age
                                editDialog.open()
                            }
                        }
                        
                        Button {
                            text: "Delete"
                            flat: true
                            Material.foreground: Material.Red
                            onClicked: {
                                deleteDialog.deleteIndex = index
                                deleteDialog.deleteName = model.name
                                deleteDialog.open()
                            }
                        }
                    }
                }
            }
            
            // Empty state
            Label {
                anchors.centerIn: parent
                text: searchField.text.length > 0 
                      ? "No users found" 
                      : "No users yet\nClick 'Add User' to create one"
                font.pixelSize: 16
                color: Material.color(Material.Grey)
                horizontalAlignment: Text.AlignHCenter
                visible: listView.count === 0
            }
        }
    }
    
    // Add Dialog
    Dialog {
        id: addDialog
        title: "Add New User"
        width: 400
        standardButtons: Dialog.Save | Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 15
            
            TextField {
                id: addNameField
                Layout.fillWidth: true
                placeholderText: "Name *"
            }
            
            TextField {
                id: addEmailField
                Layout.fillWidth: true
                placeholderText: "Email *"
            }
            
            SpinBox {
                id: addAgeField
                Layout.fillWidth: true
                from: 1
                to: 150
                value: 25
                editable: true
            }
        }
        
        onAccepted: {
            if (addNameField.text && addEmailField.text) {
                userManager.addUser(
                    addNameField.text,
                    addEmailField.text,
                    addAgeField.value
                )
                addNameField.text = ""
                addEmailField.text = ""
                addAgeField.value = 25
            }
        }
    }
    
    // Edit Dialog
    Dialog {
        id: editDialog
        title: "Edit User"
        width: 400
        standardButtons: Dialog.Save | Dialog.Cancel
        
        property int editIndex: -1
        property alias editName: editNameField.text
        property alias editEmail: editEmailField.text
        property alias editAge: editAgeField.value
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 15
            
            TextField {
                id: editNameField
                Layout.fillWidth: true
                placeholderText: "Name"
            }
            
            TextField {
                id: editEmailField
                Layout.fillWidth: true
                placeholderText: "Email"
            }
            
            SpinBox {
                id: editAgeField
                Layout.fillWidth: true
                from: 1
                to: 150
                editable: true
            }
        }
        
        onAccepted: {
            userManager.updateUser(
                editIndex,
                editNameField.text,
                editEmailField.text,
                editAgeField.value
            )
        }
    }
    
    // Delete Dialog
    Dialog {
        id: deleteDialog
        title: "Confirm Delete"
        width: 400
        standardButtons: Dialog.Yes | Dialog.No
        
        property int deleteIndex: -1
        property string deleteName: ""
        
        Label {
            text: "Delete user: " + deleteDialog.deleteName + "?"
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            userManager.deleteUser(deleteIndex)
        }
    }
    
    // Success snackbar
    Popup {
        id: successPopup
        anchors.centerIn: parent
        width: 300
        height: 60
        closePolicy: Popup.CloseOnEscape
        
        property alias text: successLabel.text
        
        background: Rectangle {
            color: Material.color(Material.Green)
            radius: 8
        }
        
        Label {
            id: successLabel
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 14
        }
        
        Timer {
            id: successTimer
            interval: 2000
            onTriggered: successPopup.close()
        }
        
        onOpened: successTimer.start()
    }
    
    Connections {
        target: userManager
        
        function onUserAdded() {
            successPopup.text = "User added successfully!"
            successPopup.open()
        }
        
        function onUserUpdated() {
            successPopup.text = "User updated successfully!"
            successPopup.open()
        }
        
        function onUserDeleted() {
            successPopup.text = "User deleted successfully!"
            successPopup.open()
        }
        
        function onErrorOccurred(message) {
            errorPopup.text = message
            errorPopup.open()
        }
    }
    
    // Error popup
    Popup {
        id: errorPopup
        anchors.centerIn: parent
        width: 300
        height: 80
        
        property alias text: errorLabel.text
        
        background: Rectangle {
            color: Material.color(Material.Red)
            radius: 8
        }
        
        Label {
            id: errorLabel
            anchors.centerIn: parent
            color: "white"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
```

### 📄 main.cpp

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "usermanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set Material style
    QQuickStyle::setStyle("Material");
    
    // Create manager
    UserManager userManager;
    
    // Setup QML
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("userManager", &userManager);
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
```

### 📄 user-manager.pro

```qmake
QT += quick sql quickcontrols2
CONFIG += c++17

SOURCES += \
    main.cpp \
    usermanager.cpp

HEADERS += \
    usermanager.h

RESOURCES += qml.qrc

TARGET = user-manager
TEMPLATE = app
```

### 📄 qml.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>main.qml</file>
    </qresource>
</RCC>
```

### 🔨 Build & Run

```bash
qmake user-manager.pro
make
./user-manager
```

---

## 📝 Summary & Quick Commands

### Structure của mỗi example:

```
project/
├── model.h          → Model header (C++)
├── model.cpp        → Model implementation (C++)
├── main.qml         → View (QML)
├── main.cpp         → Entry point
├── project.pro      → qmake file
├── CMakeLists.txt   → CMake file (optional)
└── qml.qrc          → Resource file
```

### Build commands:

```bash
# Using qmake (recommended)
qmake *.pro && make

# Using CMake
cmake -B build && cmake --build build

# Using Qt Creator
# Just open .pro file and click Run
```

### Common issues:

```bash
# If Qt not found
export QT_PATH=/path/to/qt
export PATH=$QT_PATH/bin:$PATH

# If SQL driver missing
sudo apt-get install libqt6sql6-sqlite

# Clean build
make clean
rm -rf build Makefile *.o moc_*
```

---

## 🎓 Learning Path

1. **Start with Example 1** - Basic CRUD todo app
2. **Then Example 2** - User management với Material Design
3. Practice: Build your own app (e.g., Contact Manager)
4. Study more examples: 3-8 (Real-time, Navigation, etc.)

**Mỗi example đều có đầy đủ files để compile và chạy ngay!** 🚀

Bạn có muốn tôi tiếp tục với Examples 3-8 không?
