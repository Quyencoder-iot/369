import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * ============================================
 * EXAMPLE 1: TODO LIST VIEW (QML)
 * ============================================
 * 
 * Model: TodoListModel (C++ - QAbstractListModel)
 * 
 * MỤC ĐÍCH:
 * - Hiển thị danh sách todos
 * - Add/Remove/Edit todos
 * - Toggle completed status
 * - Filter theo trạng thái
 * - Hiển thị statistics
 * 
 * KEY QML CONCEPTS:
 * 1. ListView - Hiển thị list từ model
 * 2. delegate - Template cho mỗi item
 * 3. model.xxx - Access C++ model roles
 * 4. modelObject.method() - Call C++ methods
 * 5. Property binding - Auto-update khi data thay đổi
 */

ApplicationWindow {
    id: root
    visible: true
    width: 500
    height: 700
    title: qsTr("📝 Todo List - QML View")
    
    // ============================================
    // FILTER STATE
    // ============================================
    property string filterMode: "all"  // all, incomplete, completed
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // ============================================
        // HEADER
        // ============================================
        Label {
            text: qsTr("📝 Todo List Application")
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
        }
        
        // ============================================
        // STATISTICS BAR
        // Property Binding: Tự động update khi model thay đổi
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "#e3f2fd"
            radius: 8
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20
                
                Label {
                    text: qsTr("📊 Tổng: ") + todoModel.count
                    font.pixelSize: 14
                }
                
                Label {
                    text: qsTr("✅ Hoàn thành: ") + todoModel.completedCount
                    font.pixelSize: 14
                    color: "#4caf50"
                }
                
                Label {
                    text: qsTr("⏳ Còn lại: ") + todoModel.incompleteCount
                    font.pixelSize: 14
                    color: "#ff9800"
                }
            }
        }
        
        // ============================================
        // INPUT AREA - Thêm todo mới
        // ============================================
        GroupBox {
            title: qsTr("➕ Thêm Todo Mới")
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                spacing: 10
                
                // Text input
                TextField {
                    id: inputField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Nhập todo mới...")
                    
                    // Enter key → Add
                    onAccepted: addButton.clicked()
                    
                    // Focus khi app mở
                    Component.onCompleted: forceActiveFocus()
                }
                
                // Priority selector
                ComboBox {
                    id: priorityCombo
                    model: [
                        {text: "🔴 High", value: 1},
                        {text: "🟠 Medium", value: 2},
                        {text: "🟢 Low", value: 3}
                    ]
                    textRole: "text"
                    currentIndex: 1
                    Layout.preferredWidth: 130
                }
                
                // Add button
                Button {
                    id: addButton
                    text: qsTr("➕")
                    highlighted: true
                    enabled: inputField.text.length > 0
                    
                    onClicked: {
                        // ★ CALL C++ METHOD
                        todoModel.addTodo(
                            inputField.text, 
                            priorityCombo.model[priorityCombo.currentIndex].value
                        )
                        inputField.text = ""
                        inputField.forceActiveFocus()
                    }
                }
            }
        }
        
        // ============================================
        // FILTER BUTTONS
        // ============================================
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: qsTr("📋 Tất cả")
                flat: true
                checked: filterMode === "all"
                onClicked: filterMode = "all"
            }
            
            Button {
                text: qsTr("⏳ Chưa xong")
                flat: true
                checked: filterMode === "incomplete"
                onClicked: filterMode = "incomplete"
            }
            
            Button {
                text: qsTr("✅ Đã xong")
                flat: true
                checked: filterMode === "completed"
                onClicked: filterMode = "completed"
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: qsTr("🗑️ Clear All")
                flat: true
                onClicked: clearDialog.open()
            }
        }
        
        // ============================================
        // TODO LIST - ListView
        // 
        // KEY CONCEPT:
        // - model: Kết nối với C++ model (todoModel)
        // - delegate: Template cho mỗi item
        // - model.xxx: Access roles từ C++ (text, completed, priority)
        // - index: Row number của item
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f5f5f5"
            border.color: "#ddd"
            border.width: 1
            radius: 8
            
            ListView {
                id: listView
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5
                clip: true
                
                // ★ KẾT NỐI VỚI C++ MODEL
                model: todoModel
                
                // ★ DELEGATE - Template cho mỗi todo item
                delegate: Rectangle {
                    width: listView.width
                    height: visible ? 70 : 0
                    color: model.completed ? "#e8f5e9" : "white"
                    border.width: 2
                    border.color: {
                        switch (model.priority) {
                            case 1: return "#f44336"  // High - Red
                            case 2: return "#ff9800"  // Medium - Orange
                            case 3: return "#4caf50"  // Low - Green
                            default: return "#999"
                        }
                    }
                    radius: 6
                    
                    // ★ FILTER LOGIC
                    visible: {
                        if (filterMode === "all") return true
                        if (filterMode === "completed") return model.completed
                        if (filterMode === "incomplete") return !model.completed
                        return true
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        
                        // Checkbox
                        CheckBox {
                            checked: model.completed
                            onClicked: {
                                // ★ CALL C++ METHOD với index
                                todoModel.toggleCompleted(index)
                            }
                        }
                        
                        // Todo text
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            
                            Label {
                                text: model.text
                                font.pixelSize: 16
                                font.strikeout: model.completed
                                color: model.completed ? "#888" : "#000"
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                            
                            Label {
                                text: qsTr("📅 ") + model.createdDate
                                font.pixelSize: 11
                                color: "#666"
                            }
                        }
                        
                        // Priority badge
                        Rectangle {
                            width: 60
                            height: 28
                            color: {
                                switch (model.priority) {
                                    case 1: return "#f44336"
                                    case 2: return "#ff9800"
                                    case 3: return "#4caf50"
                                    default: return "#999"
                                }
                            }
                            radius: 14
                            
                            Label {
                                anchors.centerIn: parent
                                text: {
                                    switch (model.priority) {
                                        case 1: return "HIGH"
                                        case 2: return "MED"
                                        case 3: return "LOW"
                                        default: return ""
                                    }
                                }
                                color: "white"
                                font.pixelSize: 10
                                font.bold: true
                            }
                        }
                        
                        // Delete button
                        Button {
                            text: "🗑️"
                            flat: true
                            
                            onClicked: {
                                // ★ CALL C++ METHOD
                                todoModel.removeTodo(index)
                            }
                        }
                    }
                    
                    // Hover effect
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.NoButton
                        
                        onEntered: parent.color = model.completed ? "#c8e6c9" : "#f0f0f0"
                        onExited: parent.color = model.completed ? "#e8f5e9" : "white"
                    }
                }
                
                // Empty state
                Label {
                    anchors.centerIn: parent
                    visible: listView.count === 0
                    text: qsTr("📝 Chưa có todo nào\nThêm todo đầu tiên!")
                    horizontalAlignment: Text.AlignHCenter
                    color: "#999"
                    font.pixelSize: 18
                }
            }
        }
    }
    
    // ============================================
    // CLEAR DIALOG
    // ============================================
    Dialog {
        id: clearDialog
        title: qsTr("Xác nhận")
        modal: true
        anchors.centerIn: parent
        
        Label {
            text: qsTr("Xóa tất cả todos?")
        }
        
        standardButtons: Dialog.Yes | Dialog.No
        
        onAccepted: {
            todoModel.clear()
        }
    }
}

/**
 * ============================================
 * 📚 SUMMARY - TODO LIST VIEW
 * ============================================
 * 
 * 1. QML → C++ COMMUNICATION:
 *    ✅ Access model: todoModel (set từ main.cpp)
 *    ✅ Access roles: model.text, model.completed, model.priority
 *    ✅ Call methods: todoModel.addTodo(), todoModel.removeTodo()
 *    ✅ Access properties: todoModel.count, todoModel.completedCount
 * 
 * 2. KEY QML COMPONENTS:
 *    ✅ ListView - Hiển thị list items
 *    ✅ delegate - Template cho mỗi item
 *    ✅ Property binding - Auto-update UI (todoModel.count)
 *    ✅ Signals - Button.onClicked, TextField.onAccepted
 * 
 * 3. DATA FLOW:
 *    User clicks Add Button
 *         ↓
 *    onClicked handler (QML)
 *         ↓
 *    todoModel.addTodo() (C++ method)
 *         ↓
 *    C++ model updates data
 *         ↓
 *    C++ emits countChanged signal
 *         ↓
 *    QML property binding updates automatically
 *         ↓
 *    ListView re-renders with new item
 * 
 * 4. FILTERING:
 *    - Local QML property: filterMode
 *    - delegate.visible controls which items show
 *    - NO changes to C++ model
 * 
 * 5. ROLES USED:
 *    - text (TextRole) → Todo text
 *    - completed (CompletedRole) → bool
 *    - priority (PriorityRole) → int (1,2,3)
 *    - createdDate (CreatedDateRole) → string
 * 
 * 6. C++ METHODS CALLED:
 *    - addTodo(text, priority)
 *    - removeTodo(index)
 *    - toggleCompleted(index)
 *    - clear()
 * 
 * 7. C++ PROPERTIES ACCESSED:
 *    - count (read-only)
 *    - completedCount (read-only)
 *    - incompleteCount (read-only)
 * 
 * ============================================
 * 💡 LEARNING POINTS:
 * ============================================
 * 
 * ★ ListView với C++ model:
 *   ListView { model: cppModelObject }
 * 
 * ★ Access roles trong delegate:
 *   delegate: Rectangle { Text { text: model.roleName } }
 * 
 * ★ Call C++ methods:
 *   Button { onClicked: cppModel.method(params) }
 * 
 * ★ Property binding (auto-update):
 *   Label { text: "Count: " + cppModel.count }
 * 
 * ★ Use index trong delegate:
 *   Button { onClicked: cppModel.remove(index) }
 * 
 * ============================================
 */
