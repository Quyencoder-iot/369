# Qt SQL + QML - Ví Dụ Thực Tế (C++ Model + QML View)

## 📱 Modern Qt Applications: C++ Backend + QML Frontend

> **Architecture:** SQL Model (C++) ↔ Qt Meta-Object System ↔ QML View

---

## 🎯 Table of Contents

1. [Basic Setup: Expose Model to QML](#example-1-basic-setup)
2. [QSqlTableModel + QML ListView](#example-2-qsqltablemodel--qml)
3. [Custom C++ Model for QML](#example-3-custom-c-model-exposed-to-qml)
4. [Complete CRUD App (C++ + QML)](#example-4-complete-crud-app)
5. [Real-time Updates with Signals](#example-5-real-time-updates)
6. [Advanced Filtering and Sorting](#example-6-advanced-filtering-sorting)
7. [Multi-page QML App](#example-7-multi-page-app)
8. [Production-Ready Architecture](#example-8-production-architecture)

---

## Example 1: Basic Setup - Expose QSqlTableModel to QML

### 📁 Project Structure
```
basic-sql-qml/
├── main.cpp
├── main.qml
├── basic-sql-qml.pro
└── database.db (auto-created)
```

### 💻 main.cpp (C++ Backend)

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDebug>

bool setupDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    
    if (!db.open()) {
        qCritical() << "Cannot open database:" << db.lastError().text();
        return false;
    }
    
    // Create table
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "email TEXT, "
               "age INTEGER)");
    
    // Add sample data if empty
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO users (name, email, age) VALUES "
                   "('Alice Johnson', 'alice@example.com', 28), "
                   "('Bob Smith', 'bob@example.com', 35), "
                   "('Charlie Brown', 'charlie@example.com', 42)");
    }
    
    return true;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Setup database
    if (!setupDatabase()) {
        return -1;
    }
    
    // Create model
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("users");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    
    // Set headers
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Age");
    
    // Setup QML engine
    QQmlApplicationEngine engine;
    
    // Expose model to QML
    engine.rootContext()->setContextProperty("userModel", model);
    
    // Load QML
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
```

### 📱 main.qml (QML View)

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "User Database - QML + SQL"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // Header
        Label {
            text: "User Management System"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Table View
        TableView {
            id: tableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            model: userModel
            
            delegate: Rectangle {
                implicitWidth: 200
                implicitHeight: 40
                border.width: 1
                border.color: "#ddd"
                
                Text {
                    anchors.centerIn: parent
                    text: display
                    font.pixelSize: 14
                }
            }
        }
        
        // Button row
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: "Refresh"
                onClicked: userModel.select()
            }
            
            Button {
                text: "Add Row"
                onClicked: {
                    userModel.insertRow(userModel.rowCount())
                }
            }
            
            Button {
                text: "Delete Selected"
                enabled: tableView.currentRow >= 0
                onClicked: {
                    userModel.removeRow(tableView.currentRow)
                }
            }
            
            Button {
                text: "Save Changes"
                highlighted: true
                onClicked: {
                    if (userModel.submitAll()) {
                        statusLabel.text = "Changes saved successfully!"
                    } else {
                        statusLabel.text = "Error: " + userModel.lastError().text
                    }
                }
            }
            
            Button {
                text: "Cancel"
                onClicked: {
                    userModel.revertAll()
                    statusLabel.text = "Changes cancelled"
                }
            }
            
            Item { Layout.fillWidth: true }
        }
        
        // Status bar
        Label {
            id: statusLabel
            text: "Ready"
            Layout.fillWidth: true
            color: "#666"
        }
    }
}
```

### 📝 basic-sql-qml.pro

```qmake
QT += quick sql
CONFIG += c++17

SOURCES += main.cpp

RESOURCES += qml.qrc

# QML files
DISTFILES += \
    main.qml
```

### 📦 qml.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>main.qml</file>
    </qresource>
</RCC>
```

### 🔨 Build & Run

```bash
qmake
make
./basic-sql-qml
```

---

## Example 2: QSqlTableModel + QML ListView (Better Performance)

### 💻 main.cpp

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>

class DatabaseManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSqlTableModel* userModel READ userModel CONSTANT)
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    
public:
    explicit DatabaseManager(QObject *parent = nullptr) : QObject(parent)
    {
        setupDatabase();
        m_userModel = new QSqlTableModel(this);
        m_userModel->setTable("users");
        m_userModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        m_userModel->select();
        
        connect(m_userModel, &QSqlTableModel::dataChanged,
                this, &DatabaseManager::rowCountChanged);
    }
    
    QSqlTableModel* userModel() { return m_userModel; }
    int rowCount() const { return m_userModel->rowCount(); }
    
    // Expose methods to QML
    Q_INVOKABLE bool addUser(const QString &name, const QString &email, int age)
    {
        int row = m_userModel->rowCount();
        if (!m_userModel->insertRow(row))
            return false;
        
        m_userModel->setData(m_userModel->index(row, 1), name);
        m_userModel->setData(m_userModel->index(row, 2), email);
        m_userModel->setData(m_userModel->index(row, 3), age);
        
        return m_userModel->submitAll();
    }
    
    Q_INVOKABLE bool deleteUser(int row)
    {
        if (row < 0 || row >= m_userModel->rowCount())
            return false;
        
        m_userModel->removeRow(row);
        return m_userModel->submitAll();
    }
    
    Q_INVOKABLE bool updateUser(int row, const QString &name, 
                                const QString &email, int age)
    {
        if (row < 0 || row >= m_userModel->rowCount())
            return false;
        
        m_userModel->setData(m_userModel->index(row, 1), name);
        m_userModel->setData(m_userModel->index(row, 2), email);
        m_userModel->setData(m_userModel->index(row, 3), age);
        
        return m_userModel->submitAll();
    }
    
    Q_INVOKABLE QString getUserName(int row) const
    {
        return m_userModel->data(m_userModel->index(row, 1)).toString();
    }
    
    Q_INVOKABLE QString getUserEmail(int row) const
    {
        return m_userModel->data(m_userModel->index(row, 2)).toString();
    }
    
    Q_INVOKABLE int getUserAge(int row) const
    {
        return m_userModel->data(m_userModel->index(row, 3)).toInt();
    }
    
    Q_INVOKABLE void setFilter(const QString &filter)
    {
        m_userModel->setFilter(filter);
        m_userModel->select();
    }
    
    Q_INVOKABLE void clearFilter()
    {
        m_userModel->setFilter("");
        m_userModel->select();
    }
    
signals:
    void rowCountChanged();
    
private:
    void setupDatabase()
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("users.db");
        
        if (!db.open()) {
            qCritical() << "Cannot open database";
            return;
        }
        
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT NOT NULL, "
                   "email TEXT, "
                   "age INTEGER)");
    }
    
    QSqlTableModel *m_userModel;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    DatabaseManager dbManager;
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("dbManager", &dbManager);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}

#include "main.moc"
```

### 📱 main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 900
    height: 700
    title: "User Management - Advanced"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // Header
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "#3498db"
            radius: 5
            
            Label {
                anchors.centerIn: parent
                text: "User Database Manager"
                font.pixelSize: 26
                font.bold: true
                color: "white"
            }
        }
        
        // Search bar
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search by name or email..."
                onTextChanged: {
                    if (text.length > 0) {
                        dbManager.setFilter(
                            "name LIKE '%" + text + "%' OR " +
                            "email LIKE '%" + text + "%'"
                        )
                    } else {
                        dbManager.clearFilter()
                    }
                }
            }
            
            Button {
                text: "Clear"
                onClicked: {
                    searchField.text = ""
                    dbManager.clearFilter()
                }
            }
        }
        
        // User list
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            model: dbManager.userModel
            spacing: 5
            clip: true
            
            delegate: Rectangle {
                width: listView.width
                height: 80
                color: index % 2 === 0 ? "#ecf0f1" : "#ffffff"
                radius: 5
                border.color: "#bdc3c7"
                border.width: 1
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: listView.currentIndex = index
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15
                    
                    // ID badge
                    Rectangle {
                        width: 50
                        height: 50
                        color: "#3498db"
                        radius: 25
                        
                        Label {
                            anchors.centerIn: parent
                            text: model.id
                            color: "white"
                            font.bold: true
                            font.pixelSize: 18
                        }
                    }
                    
                    // User info
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        
                        Label {
                            text: model.name
                            font.pixelSize: 18
                            font.bold: true
                            color: "#2c3e50"
                        }
                        
                        Label {
                            text: "📧 " + model.email
                            font.pixelSize: 14
                            color: "#7f8c8d"
                        }
                    }
                    
                    // Age
                    Label {
                        text: model.age + " years"
                        font.pixelSize: 16
                        color: "#34495e"
                    }
                    
                    // Actions
                    RowLayout {
                        spacing: 5
                        
                        Button {
                            text: "✏️ Edit"
                            onClicked: {
                                editDialog.editIndex = index
                                editDialog.editName = model.name
                                editDialog.editEmail = model.email
                                editDialog.editAge = model.age
                                editDialog.open()
                            }
                        }
                        
                        Button {
                            text: "🗑️ Delete"
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
                text: "No users found\nClick 'Add User' to create one"
                visible: listView.count === 0
                font.pixelSize: 16
                color: "#95a5a6"
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Statistics
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "#ecf0f1"
            radius: 5
            
            Label {
                anchors.centerIn: parent
                text: "Total Users: " + dbManager.rowCount
                font.pixelSize: 16
            }
        }
        
        // Action buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: "➕ Add User"
                highlighted: true
                Layout.fillWidth: true
                onClicked: addDialog.open()
            }
            
            Button {
                text: "🔄 Refresh"
                Layout.fillWidth: true
                onClicked: dbManager.userModel.select()
            }
        }
    }
    
    // Add User Dialog
    Dialog {
        id: addDialog
        title: "Add New User"
        width: 400
        standardButtons: Dialog.Save | Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            TextField {
                id: newName
                Layout.fillWidth: true
                placeholderText: "Name"
            }
            
            TextField {
                id: newEmail
                Layout.fillWidth: true
                placeholderText: "Email"
            }
            
            SpinBox {
                id: newAge
                Layout.fillWidth: true
                from: 1
                to: 150
                value: 25
            }
        }
        
        onAccepted: {
            if (dbManager.addUser(newName.text, newEmail.text, newAge.value)) {
                statusLabel.text = "User added successfully!"
                statusLabel.color = "green"
                newName.text = ""
                newEmail.text = ""
                newAge.value = 25
            } else {
                statusLabel.text = "Failed to add user"
                statusLabel.color = "red"
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
        property string editName: ""
        property string editEmail: ""
        property int editAge: 0
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            TextField {
                id: editNameField
                Layout.fillWidth: true
                text: editDialog.editName
            }
            
            TextField {
                id: editEmailField
                Layout.fillWidth: true
                text: editDialog.editEmail
            }
            
            SpinBox {
                id: editAgeField
                Layout.fillWidth: true
                from: 1
                to: 150
                value: editDialog.editAge
            }
        }
        
        onAccepted: {
            if (dbManager.updateUser(editIndex, editNameField.text,
                                    editEmailField.text, editAgeField.value)) {
                statusLabel.text = "User updated successfully!"
                statusLabel.color = "green"
            } else {
                statusLabel.text = "Failed to update user"
                statusLabel.color = "red"
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
        property string deleteName: ""
        
        Label {
            text: "Are you sure you want to delete user:\n" + deleteDialog.deleteName + "?"
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            if (dbManager.deleteUser(deleteIndex)) {
                statusLabel.text = "User deleted successfully!"
                statusLabel.color = "green"
            } else {
                statusLabel.text = "Failed to delete user"
                statusLabel.color = "red"
            }
        }
    }
    
    // Status bar
    footer: ToolBar {
        Label {
            id: statusLabel
            anchors.centerIn: parent
            text: "Ready"
        }
    }
}
```

---

## Example 3: Custom C++ Model Exposed to QML

### 💻 usermodel.h

```cpp
#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

struct User {
    int id;
    QString name;
    QString email;
    int age;
};

class UserModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    enum UserRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        EmailRole,
        AgeRole
    };
    
    explicit UserModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Custom methods exposed to QML
    Q_INVOKABLE bool addUser(const QString &name, const QString &email, int age);
    Q_INVOKABLE bool removeUser(int index);
    Q_INVOKABLE bool updateUser(int index, const QString &name, 
                                const QString &email, int age);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void filterByAge(int minAge, int maxAge);
    Q_INVOKABLE void clearFilter();
    
signals:
    void countChanged();
    void errorOccurred(const QString &message);
    
private:
    void loadUsers();
    bool setupDatabase();
    
    QList<User> m_users;
    QSqlDatabase m_database;
    QString m_filter;
};

#endif // USERMODEL_H
```

### 💻 usermodel.cpp

```cpp
#include "usermodel.h"
#include <QDebug>

UserModel::UserModel(QObject *parent)
    : QAbstractListModel(parent)
{
    if (setupDatabase()) {
        loadUsers();
    }
}

bool UserModel::setupDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("users.db");
    
    if (!m_database.open()) {
        emit errorOccurred("Cannot open database: " + m_database.lastError().text());
        return false;
    }
    
    QSqlQuery query(m_database);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "email TEXT, "
                    "age INTEGER)")) {
        emit errorOccurred("Cannot create table: " + query.lastError().text());
        return false;
    }
    
    return true;
}

void UserModel::loadUsers()
{
    beginResetModel();
    m_users.clear();
    
    QString sql = "SELECT id, name, email, age FROM users";
    if (!m_filter.isEmpty()) {
        sql += " WHERE " + m_filter;
    }
    sql += " ORDER BY name";
    
    QSqlQuery query(m_database);
    if (query.exec(sql)) {
        while (query.next()) {
            User user;
            user.id = query.value(0).toInt();
            user.name = query.value(1).toString();
            user.email = query.value(2).toString();
            user.age = query.value(3).toInt();
            m_users.append(user);
        }
    } else {
        emit errorOccurred("Query failed: " + query.lastError().text());
    }
    
    endResetModel();
    emit countChanged();
}

int UserModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_users.count();
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_users.count())
        return QVariant();
    
    const User &user = m_users[index.row()];
    
    switch (role) {
    case IdRole:
        return user.id;
    case NameRole:
        return user.name;
    case EmailRole:
        return user.email;
    case AgeRole:
        return user.age;
    }
    
    return QVariant();
}

QHash<int, QByteArray> UserModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[EmailRole] = "email";
    roles[AgeRole] = "age";
    return roles;
}

bool UserModel::addUser(const QString &name, const QString &email, int age)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO users (name, email, age) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(age);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to add user: " + query.lastError().text());
        return false;
    }
    
    loadUsers();
    return true;
}

bool UserModel::removeUser(int index)
{
    if (index < 0 || index >= m_users.count())
        return false;
    
    int userId = m_users[index].id;
    
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to delete user: " + query.lastError().text());
        return false;
    }
    
    loadUsers();
    return true;
}

bool UserModel::updateUser(int index, const QString &name, 
                           const QString &email, int age)
{
    if (index < 0 || index >= m_users.count())
        return false;
    
    int userId = m_users[index].id;
    
    QSqlQuery query(m_database);
    query.prepare("UPDATE users SET name = ?, email = ?, age = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(age);
    query.addBindValue(userId);
    
    if (!query.exec()) {
        emit errorOccurred("Failed to update user: " + query.lastError().text());
        return false;
    }
    
    loadUsers();
    return true;
}

void UserModel::refresh()
{
    loadUsers();
}

void UserModel::filterByAge(int minAge, int maxAge)
{
    m_filter = QString("age BETWEEN %1 AND %2").arg(minAge).arg(maxAge);
    loadUsers();
}

void UserModel::clearFilter()
{
    m_filter.clear();
    loadUsers();
}
```

### 💻 main.cpp

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "usermodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Register type to QML (alternative to context property)
    qmlRegisterType<UserModel>("com.myapp.models", 1, 0, "UserModel");
    
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
```

### 📱 main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.myapp.models 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Custom Model Example"
    
    // Instantiate custom model
    UserModel {
        id: userModel
        
        onErrorOccurred: function(message) {
            errorDialog.text = message
            errorDialog.open()
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        
        // Filter controls
        GroupBox {
            title: "Filter by Age"
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                
                Label { text: "Min:" }
                SpinBox {
                    id: minAge
                    from: 0
                    to: 150
                    value: 18
                }
                
                Label { text: "Max:" }
                SpinBox {
                    id: maxAge
                    from: 0
                    to: 150
                    value: 65
                }
                
                Button {
                    text: "Apply Filter"
                    onClicked: userModel.filterByAge(minAge.value, maxAge.value)
                }
                
                Button {
                    text: "Clear Filter"
                    onClicked: userModel.clearFilter()
                }
            }
        }
        
        // User list with custom model
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            model: userModel
            spacing: 10
            
            delegate: Rectangle {
                width: listView.width
                height: 60
                color: "#f0f0f0"
                radius: 5
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    
                    Label {
                        text: model.name
                        font.bold: true
                        font.pixelSize: 16
                        Layout.fillWidth: true
                    }
                    
                    Label {
                        text: model.email
                        color: "#666"
                    }
                    
                    Label {
                        text: model.age + " years"
                        color: "#333"
                    }
                    
                    Button {
                        text: "Delete"
                        onClicked: userModel.removeUser(index)
                    }
                }
            }
        }
        
        // Status
        Label {
            text: "Total: " + userModel.count + " users"
            Layout.alignment: Qt.AlignHCenter
        }
    }
    
    // Error dialog
    Dialog {
        id: errorDialog
        title: "Error"
        property alias text: errorLabel.text
        
        Label {
            id: errorLabel
            wrapMode: Text.WordWrap
        }
        
        standardButtons: Dialog.Ok
    }
}
```

---

## Example 4: Complete CRUD App - Production Quality

### 📁 Project Structure
```
crud-app/
├── src/
│   ├── main.cpp
│   ├── databasemanager.h
│   ├── databasemanager.cpp
│   ├── usermodel.h
│   └── usermodel.cpp
├── qml/
│   ├── main.qml
│   ├── UserList.qml
│   ├── UserForm.qml
│   └── ConfirmDialog.qml
├── resources.qrc
└── crud-app.pro
```

### 💻 databasemanager.h

```cpp
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    
public:
    static DatabaseManager& instance();
    
    bool initialize();
    bool isConnected() const { return m_connected; }
    QSqlDatabase database() { return m_database; }
    
    Q_INVOKABLE bool executeQuery(const QString &query);
    
signals:
    void connectedChanged();
    void errorOccurred(const QString &message);
    
private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    QSqlDatabase m_database;
    bool m_connected;
};

#endif
```

### 💻 databasemanager.cpp

```cpp
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), m_connected(false)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::initialize()
{
    // Use application data directory
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    
    QString dbPath = dataPath + "/app_database.db";
    
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        emit errorOccurred("Cannot open database: " + 
                          m_database.lastError().text());
        m_connected = false;
        emit connectedChanged();
        return false;
    }
    
    // Create tables
    QSqlQuery query(m_database);
    
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "email TEXT UNIQUE, "
        "age INTEGER, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP)");
    
    if (!success) {
        emit errorOccurred("Cannot create table: " + query.lastError().text());
        m_connected = false;
    } else {
        m_connected = true;
    }
    
    emit connectedChanged();
    return success;
}

bool DatabaseManager::executeQuery(const QString &queryStr)
{
    QSqlQuery query(m_database);
    if (!query.exec(queryStr)) {
        emit errorOccurred("Query failed: " + query.lastError().text());
        return false;
    }
    return true;
}
```

### 💻 main.cpp

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "databasemanager.h"
#include "usermodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set app info
    app.setOrganizationName("MyCompany");
    app.setApplicationName("CRUD App");
    
    // Set QML style
    QQuickStyle::setStyle("Material");
    
    // Initialize database
    if (!DatabaseManager::instance().initialize()) {
        qCritical() << "Failed to initialize database";
        return -1;
    }
    
    // Register types
    qmlRegisterType<UserModel>("com.myapp.models", 1, 0, "UserModel");
    
    QQmlApplicationEngine engine;
    
    // Expose database manager
    engine.rootContext()->setContextProperty("dbManager", 
                                            &DatabaseManager::instance());
    
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    engine.load(url);
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
```

### 📱 qml/main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import com.myapp.models 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 768
    title: "CRUD Application - Production"
    
    Material.theme: Material.Light
    Material.accent: Material.Blue
    
    UserModel {
        id: userModel
        
        onErrorOccurred: function(message) {
            errorSnackbar.text = message
            errorSnackbar.open()
        }
    }
    
    header: ToolBar {
        Material.background: Material.Blue
        
        RowLayout {
            anchors.fill: parent
            
            Label {
                text: "User Management System"
                font.pixelSize: 20
                font.bold: true
                color: "white"
                Layout.fillWidth: true
            }
            
            ToolButton {
                icon.name: "help-about"
                text: "About"
                onClicked: aboutDialog.open()
            }
        }
    }
    
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: userListPage
    }
    
    Component {
        id: userListPage
        
        UserList {
            userModel: userModel
            
            onAddRequested: {
                stackView.push(userFormPage, {
                    "mode": "add"
                })
            }
            
            onEditRequested: function(index) {
                stackView.push(userFormPage, {
                    "mode": "edit",
                    "editIndex": index
                })
            }
        }
    }
    
    Component {
        id: userFormPage
        
        UserForm {
            userModel: userModel
            
            onSaved: {
                stackView.pop()
                successSnackbar.text = "User saved successfully!"
                successSnackbar.open()
            }
            
            onCancelled: {
                stackView.pop()
            }
        }
    }
    
    // Success snackbar
    Snackbar {
        id: successSnackbar
        Material.background: Material.Green
    }
    
    // Error snackbar
    Snackbar {
        id: errorSnackbar
        Material.background: Material.Red
    }
    
    // About dialog
    Dialog {
        id: aboutDialog
        title: "About"
        standardButtons: Dialog.Ok
        
        ColumnLayout {
            Label {
                text: "CRUD Application v1.0"
                font.bold: true
            }
            Label {
                text: "Built with Qt " + Qt.version
            }
            Label {
                text: "© 2025 MyCompany"
            }
        }
    }
}

// Custom Snackbar component
Component {
    id: snackbarComponent
    
    Rectangle {
        id: snackbar
        
        property alias text: label.text
        
        width: parent.width
        height: 50
        color: Material.background
        y: parent.height
        
        Label {
            id: label
            anchors.centerIn: parent
            color: "white"
        }
        
        function open() {
            animation.start()
        }
        
        SequentialAnimation {
            id: animation
            NumberAnimation {
                target: snackbar
                property: "y"
                to: parent.height - snackbar.height
                duration: 300
            }
            PauseAnimation { duration: 2000 }
            NumberAnimation {
                target: snackbar
                property: "y"
                to: parent.height
                duration: 300
            }
        }
    }
}
```

### 📱 qml/UserList.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    
    property var userModel
    
    signal addRequested()
    signal editRequested(int index)
    
    header: Pane {
        RowLayout {
            anchors.fill: parent
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search..."
                onTextChanged: {
                    // Implement search
                }
            }
            
            Button {
                text: "Add User"
                highlighted: true
                onClicked: root.addRequested()
            }
        }
    }
    
    ListView {
        id: listView
        anchors.fill: parent
        model: root.userModel
        spacing: 5
        
        delegate: ItemDelegate {
            width: listView.width
            height: 80
            
            contentItem: RowLayout {
                spacing: 15
                
                Rectangle {
                    width: 50
                    height: 50
                    radius: 25
                    color: Material.accent
                    
                    Label {
                        anchors.centerIn: parent
                        text: model.name.charAt(0)
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }
                
                ColumnLayout {
                    Layout.fillWidth: true
                    
                    Label {
                        text: model.name
                        font.pixelSize: 18
                        font.bold: true
                    }
                    
                    Label {
                        text: model.email
                        color: Material.color(Material.Grey)
                    }
                }
                
                Label {
                    text: model.age + " years"
                }
                
                Button {
                    text: "Edit"
                    flat: true
                    onClicked: root.editRequested(index)
                }
                
                Button {
                    text: "Delete"
                    flat: true
                    onClicked: {
                        deleteDialog.deleteIndex = index
                        deleteDialog.deleteName = model.name
                        deleteDialog.open()
                    }
                }
            }
        }
    }
    
    ConfirmDialog {
        id: deleteDialog
        
        property int deleteIndex: -1
        property string deleteName: ""
        
        title: "Confirm Delete"
        text: "Delete user: " + deleteName + "?"
        
        onAccepted: {
            userModel.removeUser(deleteIndex)
        }
    }
}
```

### 📱 qml/UserForm.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    
    property var userModel
    property string mode: "add" // "add" or "edit"
    property int editIndex: -1
    
    signal saved()
    signal cancelled()
    
    title: mode === "add" ? "Add User" : "Edit User"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        TextField {
            id: nameField
            Layout.fillWidth: true
            placeholderText: "Name *"
            
            Component.onCompleted: {
                if (mode === "edit" && editIndex >= 0) {
                    text = userModel.data(
                        userModel.index(editIndex, 0), 
                        UserModel.NameRole)
                }
            }
        }
        
        TextField {
            id: emailField
            Layout.fillWidth: true
            placeholderText: "Email *"
            
            Component.onCompleted: {
                if (mode === "edit" && editIndex >= 0) {
                    text = userModel.data(
                        userModel.index(editIndex, 0),
                        UserModel.EmailRole)
                }
            }
        }
        
        SpinBox {
            id: ageField
            Layout.fillWidth: true
            from: 1
            to: 150
            value: 25
            
            Component.onCompleted: {
                if (mode === "edit" && editIndex >= 0) {
                    value = userModel.data(
                        userModel.index(editIndex, 0),
                        UserModel.AgeRole)
                }
            }
        }
        
        Item { Layout.fillHeight: true }
        
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Cancel"
                Layout.fillWidth: true
                onClicked: root.cancelled()
            }
            
            Button {
                text: "Save"
                highlighted: true
                Layout.fillWidth: true
                enabled: nameField.text.length > 0 && 
                        emailField.text.length > 0
                
                onClicked: {
                    if (mode === "add") {
                        userModel.addUser(nameField.text, 
                                        emailField.text, 
                                        ageField.value)
                    } else {
                        userModel.updateUser(editIndex, 
                                           nameField.text,
                                           emailField.text, 
                                           ageField.value)
                    }
                    root.saved()
                }
            }
        }
    }
}
```

### 📱 qml/ConfirmDialog.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: root
    
    property alias text: contentLabel.text
    
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Yes | Dialog.No
    
    Label {
        id: contentLabel
        wrapMode: Text.WordWrap
    }
}
```

---

## 🔨 Build Configuration

### crud-app.pro

```qmake
QT += quick sql quickcontrols2
CONFIG += c++17

SOURCES += \
    src/main.cpp \
    src/databasemanager.cpp \
    src/usermodel.cpp

HEADERS += \
    src/databasemanager.h \
    src/usermodel.h

RESOURCES += resources.qrc

# Additional import path
QML_IMPORT_PATH = $$PWD/qml

# Output
TARGET = crud-app
TEMPLATE = app

# Install
target.path = /usr/local/bin
INSTALLS += target
```

### resources.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file>qml/main.qml</file>
        <file>qml/UserList.qml</file>
        <file>qml/UserForm.qml</file>
        <file>qml/ConfirmDialog.qml</file>
    </qresource>
</RCC>
```

---

## 🚀 Build & Run

```bash
# Using qmake
qmake
make
./crud-app

# Using CMake (alternative)
cmake -B build
cmake --build build
./build/crud-app
```

---

## 📚 Key Concepts Summary

### C++ Side (Model/Backend):
- ✅ QSqlTableModel - Direct SQL table access
- ✅ QAbstractListModel - Custom model implementation
- ✅ Q_PROPERTY - Expose properties to QML
- ✅ Q_INVOKABLE - Expose methods to QML
- ✅ signals/slots - Communication with QML
- ✅ qmlRegisterType - Register C++ types in QML

### QML Side (View/Frontend):
- ✅ ListView - Display data
- ✅ TableView - Tabular display
- ✅ Dialog - User interactions
- ✅ StackView - Navigation
- ✅ Material theme - Modern UI
- ✅ Data binding - Automatic updates

### Best Practices:
- ✅ Tách biệt Model (C++) và View (QML)
- ✅ Singleton pattern cho DatabaseManager
- ✅ Error handling với signals
- ✅ RAII pattern cho resources
- ✅ Q_PROPERTY cho reactive binding
- ✅ Custom roles cho ListView
- ✅ Validation trước khi save

---

---

## Example 5: Real-time Updates với Signals/Slots

### 💻 realtimemodel.h

```cpp
#ifndef REALTIMEMODEL_H
#define REALTIMEMODEL_H

#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QTimer>

class RealtimeModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int updateInterval READ updateInterval 
               WRITE setUpdateInterval NOTIFY updateIntervalChanged)
    Q_PROPERTY(bool autoRefresh READ autoRefresh 
               WRITE setAutoRefresh NOTIFY autoRefreshChanged)
    
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        ValueRole,
        TimestampRole
    };
    
    explicit RealtimeModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    int updateInterval() const { return m_updateInterval; }
    void setUpdateInterval(int interval);
    
    bool autoRefresh() const { return m_autoRefresh; }
    void setAutoRefresh(bool enabled);
    
    Q_INVOKABLE void manualRefresh();
    
signals:
    void updateIntervalChanged();
    void autoRefreshChanged();
    void dataUpdated();
    
private slots:
    void onTimerTimeout();
    
private:
    void loadData();
    
    struct DataItem {
        int id;
        QString name;
        double value;
        QString timestamp;
    };
    
    QList<DataItem> m_data;
    QSqlDatabase m_db;
    QTimer *m_timer;
    int m_updateInterval;
    bool m_autoRefresh;
};

#endif
```

### 💻 realtimemodel.cpp

```cpp
#include "realtimemodel.h"
#include <QSqlQuery>
#include <QDateTime>

RealtimeModel::RealtimeModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_timer(new QTimer(this))
    , m_updateInterval(1000)
    , m_autoRefresh(false)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("realtime.db");
    m_db.open();
    
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS realtime_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "value REAL, "
               "timestamp TEXT)");
    
    connect(m_timer, &QTimer::timeout, this, &RealtimeModel::onTimerTimeout);
    
    loadData();
}

int RealtimeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant RealtimeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_data.count())
        return QVariant();
    
    const DataItem &item = m_data[index.row()];
    
    switch (role) {
    case IdRole: return item.id;
    case NameRole: return item.name;
    case ValueRole: return item.value;
    case TimestampRole: return item.timestamp;
    }
    
    return QVariant();
}

QHash<int, QByteArray> RealtimeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    roles[TimestampRole] = "timestamp";
    return roles;
}

void RealtimeModel::setUpdateInterval(int interval)
{
    if (m_updateInterval != interval) {
        m_updateInterval = interval;
        if (m_autoRefresh) {
            m_timer->setInterval(interval);
        }
        emit updateIntervalChanged();
    }
}

void RealtimeModel::setAutoRefresh(bool enabled)
{
    if (m_autoRefresh != enabled) {
        m_autoRefresh = enabled;
        if (enabled) {
            m_timer->start(m_updateInterval);
        } else {
            m_timer->stop();
        }
        emit autoRefreshChanged();
    }
}

void RealtimeModel::manualRefresh()
{
    loadData();
}

void RealtimeModel::onTimerTimeout()
{
    loadData();
}

void RealtimeModel::loadData()
{
    beginResetModel();
    m_data.clear();
    
    QSqlQuery query(m_db);
    query.exec("SELECT id, name, value, timestamp FROM realtime_data "
               "ORDER BY timestamp DESC LIMIT 50");
    
    while (query.next()) {
        DataItem item;
        item.id = query.value(0).toInt();
        item.name = query.value(1).toString();
        item.value = query.value(2).toDouble();
        item.timestamp = query.value(3).toString();
        m_data.append(item);
    }
    
    endResetModel();
    emit dataUpdated();
}
```

### 📱 main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import com.myapp.models 1.0

ApplicationWindow {
    visible: true
    width: 1000
    height: 700
    title: "Real-time Data Monitor"
    
    RealtimeModel {
        id: realtimeModel
        autoRefresh: true
        updateInterval: 2000
        
        onDataUpdated: {
            updateChart()
            lastUpdateLabel.text = "Last update: " + new Date().toLocaleString()
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        
        // Controls
        RowLayout {
            Layout.fillWidth: true
            
            Switch {
                text: "Auto Refresh"
                checked: realtimeModel.autoRefresh
                onToggled: realtimeModel.autoRefresh = checked
            }
            
            Label {
                text: "Update Interval (ms):"
            }
            
            SpinBox {
                from: 100
                to: 10000
                stepSize: 100
                value: realtimeModel.updateInterval
                onValueChanged: realtimeModel.updateInterval = value
            }
            
            Button {
                text: "Manual Refresh"
                onClicked: realtimeModel.manualRefresh()
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                id: lastUpdateLabel
                text: "Last update: -"
                color: "#666"
            }
        }
        
        // Chart
        ChartView {
            id: chartView
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true
            
            LineSeries {
                id: lineSeries
                name: "Value"
                axisX: ValueAxis {
                    min: 0
                    max: 50
                }
                axisY: ValueAxis {
                    min: 0
                    max: 100
                }
            }
        }
        
        // Data list
        ListView {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            model: realtimeModel
            
            delegate: ItemDelegate {
                width: parent.width
                text: model.name + ": " + model.value.toFixed(2) + 
                      " (" + model.timestamp + ")"
            }
        }
    }
    
    function updateChart() {
        lineSeries.clear()
        for (let i = 0; i < realtimeModel.rowCount; i++) {
            lineSeries.append(i, realtimeModel.data(
                realtimeModel.index(i, 0), RealtimeModel.ValueRole))
        }
    }
}
```

---

## Example 6: Advanced Filtering and Sorting

### 💻 advancedmodel.h

```cpp
#ifndef ADVANCEDMODEL_H
#define ADVANCEDMODEL_H

#include <QSortFilterProxyModel>
#include <QSqlTableModel>

class AdvancedModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterText READ filterText 
               WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(QString sortColumn READ sortColumn 
               WRITE setSortColumn NOTIFY sortColumnChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending 
               WRITE setSortAscending NOTIFY sortAscendingChanged)
    
public:
    explicit AdvancedModel(QObject *parent = nullptr);
    
    QString filterText() const { return m_filterText; }
    void setFilterText(const QString &text);
    
    QString sortColumn() const { return m_sortColumn; }
    void setSortColumn(const QString &column);
    
    bool sortAscending() const { return m_sortAscending; }
    void setSortAscending(bool ascending);
    
    Q_INVOKABLE void applyComplexFilter(const QVariantMap &criteria);
    Q_INVOKABLE void clearAllFilters();
    
signals:
    void filterTextChanged();
    void sortColumnChanged();
    void sortAscendingChanged();
    
protected:
    bool filterAcceptsRow(int sourceRow, 
                         const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, 
                 const QModelIndex &right) const override;
    
private:
    QSqlTableModel *m_sourceModel;
    QString m_filterText;
    QString m_sortColumn;
    bool m_sortAscending;
    QVariantMap m_complexFilter;
};

#endif
```

### 📱 FilterView.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: root
    
    property var model
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        // Text search
        GroupBox {
            title: "Text Search"
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: "Search in all fields..."
                    onTextChanged: model.filterText = text
                }
                
                Button {
                    text: "Clear"
                    onClicked: searchField.text = ""
                }
            }
        }
        
        // Sort options
        GroupBox {
            title: "Sort Options"
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                
                Label { text: "Sort by:" }
                
                ComboBox {
                    id: sortCombo
                    Layout.fillWidth: true
                    model: ["Name", "Email", "Age", "Created Date"]
                    onCurrentTextChanged: {
                        model.sortColumn = currentText.toLowerCase()
                    }
                }
                
                RadioButton {
                    text: "Ascending"
                    checked: true
                    onCheckedChanged: {
                        if (checked) model.sortAscending = true
                    }
                }
                
                RadioButton {
                    text: "Descending"
                    onCheckedChanged: {
                        if (checked) model.sortAscending = false
                    }
                }
            }
        }
        
        // Advanced filters
        GroupBox {
            title: "Advanced Filters"
            Layout.fillWidth: true
            
            GridLayout {
                anchors.fill: parent
                columns: 2
                
                Label { text: "Age Range:" }
                RowLayout {
                    SpinBox {
                        id: minAge
                        from: 0
                        to: 150
                        value: 0
                    }
                    Label { text: "to" }
                    SpinBox {
                        id: maxAge
                        from: 0
                        to: 150
                        value: 150
                    }
                }
                
                Label { text: "Email Domain:" }
                ComboBox {
                    id: domainCombo
                    Layout.fillWidth: true
                    model: ["All", "gmail.com", "yahoo.com", "hotmail.com"]
                }
                
                Button {
                    text: "Apply Filters"
                    Layout.columnSpan: 2
                    highlighted: true
                    onClicked: {
                        root.model.applyComplexFilter({
                            "minAge": minAge.value,
                            "maxAge": maxAge.value,
                            "domain": domainCombo.currentText
                        })
                    }
                }
            }
        }
        
        // Stats
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "#f0f0f0"
            radius: 5
            
            Label {
                anchors.centerIn: parent
                text: "Showing " + root.model.rowCount + " results"
                font.bold: true
            }
        }
    }
}
```

---

## Example 7: Multi-page QML App with Navigation

### 📁 Project Structure
```
multipage-app/
├── src/
│   ├── main.cpp
│   └── models/
│       ├── usermodel.h
│       └── usermodel.cpp
├── qml/
│   ├── main.qml
│   ├── pages/
│   │   ├── HomePage.qml
│   │   ├── UsersPage.qml
│   │   ├── SettingsPage.qml
│   │   └── StatisticsPage.qml
│   └── components/
│       ├── NavigationDrawer.qml
│       └── UserCard.qml
└── resources.qrc
```

### 📱 qml/main.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "pages"
import "components"

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 800
    title: "Multi-page Application"
    
    property int currentPage: 0
    
    // Drawer navigation
    Drawer {
        id: drawer
        width: 250
        height: root.height
        
        NavigationDrawer {
            anchors.fill: parent
            currentIndex: currentPage
            
            onPageSelected: function(index) {
                currentPage = index
                stackView.replace(getPage(index))
                drawer.close()
            }
        }
    }
    
    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            
            ToolButton {
                icon.name: "menu"
                onClicked: drawer.open()
            }
            
            Label {
                text: stackView.currentItem ? 
                      stackView.currentItem.title : "App"
                font.pixelSize: 20
                Layout.fillWidth: true
            }
            
            ToolButton {
                icon.name: "search"
                onClicked: searchDialog.open()
            }
            
            ToolButton {
                icon.name: "more-vert"
                onClicked: optionsMenu.open()
                
                Menu {
                    id: optionsMenu
                    MenuItem { text: "Settings" }
                    MenuItem { text: "About" }
                    MenuSeparator {}
                    MenuItem { text: "Exit" }
                }
            }
        }
    }
    
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: HomePage {}
    }
    
    function getPage(index) {
        switch(index) {
        case 0: return homeComponent
        case 1: return usersComponent
        case 2: return statsComponent
        case 3: return settingsComponent
        default: return homeComponent
        }
    }
    
    Component { id: homeComponent; HomePage {} }
    Component { id: usersComponent; UsersPage {} }
    Component { id: statsComponent; StatisticsPage {} }
    Component { id: settingsComponent; SettingsPage {} }
    
    Dialog {
        id: searchDialog
        title: "Search"
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        TextField {
            id: searchField
            width: 300
            placeholderText: "Search..."
        }
    }
}
```

### 📱 qml/components/NavigationDrawer.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: root
    
    property int currentIndex: 0
    signal pageSelected(int index)
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // Header
        Pane {
            Layout.fillWidth: true
            Material.background: Material.primary
            
            ColumnLayout {
                anchors.fill: parent
                
                Image {
                    source: "qrc:/images/avatar.png"
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    Layout.alignment: Qt.AlignHCenter
                }
                
                Label {
                    text: "User Name"
                    color: "white"
                    font.pixelSize: 18
                    Layout.alignment: Qt.AlignHCenter
                }
                
                Label {
                    text: "user@example.com"
                    color: "white"
                    opacity: 0.7
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
        
        // Navigation items
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            model: ListModel {
                ListElement { title: "Home"; icon: "home" }
                ListElement { title: "Users"; icon: "people" }
                ListElement { title: "Statistics"; icon: "bar-chart" }
                ListElement { title: "Settings"; icon: "settings" }
            }
            
            delegate: ItemDelegate {
                width: parent.width
                highlighted: root.currentIndex === index
                
                contentItem: RowLayout {
                    spacing: 15
                    
                    Label {
                        text: "📍" // Replace with actual icon
                        font.pixelSize: 20
                    }
                    
                    Label {
                        text: model.title
                        font.pixelSize: 16
                    }
                }
                
                onClicked: root.pageSelected(index)
            }
        }
        
        // Footer
        Pane {
            Layout.fillWidth: true
            
            Label {
                anchors.centerIn: parent
                text: "Version 1.0"
                color: "#999"
                font.pixelSize: 12
            }
        }
    }
}
```

### 📱 qml/pages/UsersPage.qml

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.myapp.models 1.0
import "../components"

Page {
    id: root
    title: "Users"
    
    UserModel {
        id: userModel
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // Toolbar
        Pane {
            Layout.fillWidth: true
            Material.elevation: 2
            
            RowLayout {
                anchors.fill: parent
                
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Search users..."
                }
                
                Button {
                    text: "Filter"
                    flat: true
                    onClicked: filterDrawer.open()
                }
                
                Button {
                    text: "Add User"
                    highlighted: true
                    onClicked: addUserDialog.open()
                }
            }
        }
        
        // User grid
        GridView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 300
            cellHeight: 200
            
            model: userModel
            
            delegate: UserCard {
                width: 280
                height: 180
                userName: model.name
                userEmail: model.email
                userAge: model.age
                
                onEditClicked: {
                    editUserDialog.editIndex = index
                    editUserDialog.open()
                }
                
                onDeleteClicked: {
                    userModel.removeUser(index)
                }
            }
        }
    }
    
    Drawer {
        id: filterDrawer
        edge: Qt.RightEdge
        width: 300
        height: root.height
        
        FilterView {
            anchors.fill: parent
            model: userModel
        }
    }
}
```

---

## Example 8: Production Architecture with DI

### 💻 Dependency Injection Container

```cpp
// servicelocator.h
#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include <QObject>
#include <QHash>
#include <memory>

class ServiceLocator
{
public:
    static ServiceLocator& instance();
    
    template<typename T>
    void registerService(T* service) {
        services[typeid(T).name()] = service;
    }
    
    template<typename T>
    T* getService() {
        auto it = services.find(typeid(T).name());
        if (it != services.end()) {
            return static_cast<T*>(*it);
        }
        return nullptr;
    }
    
private:
    ServiceLocator() = default;
    QHash<QString, QObject*> services;
};

#endif
```

### 💻 Repository Pattern

```cpp
// userrepository.h
#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <QObject>
#include <QList>
#include "user.h"

class IUserRepository
{
public:
    virtual ~IUserRepository() = default;
    virtual QList<User> findAll() = 0;
    virtual User findById(int id) = 0;
    virtual bool save(const User &user) = 0;
    virtual bool remove(int id) = 0;
};

class UserRepository : public QObject, public IUserRepository
{
    Q_OBJECT
    
public:
    explicit UserRepository(QObject *parent = nullptr);
    
    QList<User> findAll() override;
    User findById(int id) override;
    bool save(const User &user) override;
    bool remove(int id) override;
    
signals:
    void dataChanged();
    
private:
    QSqlDatabase m_db;
};

#endif
```

### 💻 ViewModel Pattern

```cpp
// userviewmodel.h
#ifndef USERVIEWMODEL_H
#define USERVIEWMODEL_H

#include <QObject>
#include "userrepository.h"

class UserViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList users READ users NOTIFY usersChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    
public:
    explicit UserViewModel(IUserRepository *repo, QObject *parent = nullptr);
    
    QVariantList users() const { return m_users; }
    bool loading() const { return m_loading; }
    QString error() const { return m_error; }
    
    Q_INVOKABLE void loadUsers();
    Q_INVOKABLE void addUser(const QVariantMap &userData);
    Q_INVOKABLE void updateUser(int id, const QVariantMap &userData);
    Q_INVOKABLE void deleteUser(int id);
    
signals:
    void usersChanged();
    void loadingChanged();
    void errorChanged();
    
private:
    void setLoading(bool loading);
    void setError(const QString &error);
    
    IUserRepository *m_repository;
    QVariantList m_users;
    bool m_loading;
    QString m_error;
};

#endif
```

### 💻 Main Setup

```cpp
// main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "servicelocator.h"
#include "databasemanager.h"
#include "userrepository.h"
#include "userviewmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Setup services
    auto &locator = ServiceLocator::instance();
    
    // Register database manager
    auto dbManager = new DatabaseManager;
    locator.registerService(dbManager);
    
    // Register repository
    auto userRepo = new UserRepository;
    locator.registerService<IUserRepository>(userRepo);
    
    // Create ViewModel
    auto viewModel = new UserViewModel(userRepo);
    
    // Setup QML
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("userViewModel", viewModel);
    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    
    return app.exec();
}
```

---

## 🎯 Summary & Best Practices

### Architecture Principles

1. **Separation of Concerns**
   - C++ handles: Business logic, data access, validation
   - QML handles: UI, user interaction, presentation

2. **Communication Patterns**
   - C++ → QML: Q_PROPERTY, signals
   - QML → C++: Q_INVOKABLE methods, property bindings

3. **Data Flow**
   ```
   Database → Repository → Model → QML View
                ↓
           ViewModel (optional)
   ```

### Performance Tips

- ✅ Use QSqlTableModel cho simple cases
- ✅ Custom QAbstractListModel cho complex logic
- ✅ QSortFilterProxyModel cho filtering/sorting
- ✅ Lazy loading cho large datasets
- ✅ Batch updates thay vì individual signals

### Security

- ✅ Luôn dùng prepared statements
- ✅ Validate input từ QML
- ✅ Không expose sensitive data qua Q_PROPERTY
- ✅ Use transactions cho multi-step operations

### Testing

```cpp
// Unit test example
class UserModelTest : public QObject
{
    Q_OBJECT
    
private slots:
    void testAddUser() {
        UserModel model;
        QVERIFY(model.addUser("Test", "test@test.com", 25));
        QCOMPARE(model.rowCount(), 1);
    }
};
```

---

## 📦 Complete Project Templates

### Minimal Template

```bash
my-app/
├── main.cpp
├── main.qml
└── my-app.pro
```

### Standard Template

```bash
my-app/
├── src/
│   ├── main.cpp
│   └── models/
├── qml/
│   ├── main.qml
│   └── pages/
├── resources.qrc
└── my-app.pro
```

### Production Template

```bash
my-app/
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── database/
│   │   └── services/
│   ├── models/
│   ├── viewmodels/
│   └── repositories/
├── qml/
│   ├── main.qml
│   ├── pages/
│   ├── components/
│   └── dialogs/
├── tests/
├── resources.qrc
├── CMakeLists.txt
└── README.md
```

---

## 🚀 Next Steps

1. **Học thêm về:**
   - Qt Quick Controls 2
   - Qt Quick Layouts
   - Material Design với QML
   - QML Debugging tools

2. **Thực hành với:**
   - Build một TODO app
   - Build một contact manager
   - Build một expense tracker

3. **Nâng cao:**
   - Unit testing với QTest
   - CI/CD cho Qt apps
   - Cross-platform deployment
   - Qt for Mobile (Android/iOS)

---

**Chúc bạn thành công với Qt SQL + QML! 🎉**
