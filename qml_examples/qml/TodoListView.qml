import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * TodoListView - QML View cho TodoListModel
 * 
 * Hiển thị danh sách todos với khả năng:
 * - Add/Remove todos
 * - Toggle completed
 * - Filter theo completed status
 */
ApplicationWindow {
    id: root
    visible: true
    width: 480
    height: 640
    title: qsTr("📝 Todo List - QML + C++ Model")
    
    // Access to C++ model (set from main.cpp)
    property var todoModel
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // ===================================
        // Header
        // ===================================
        Label {
            text: qsTr("Todo List Application")
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
        }
        
        Label {
            text: qsTr("Total: %1 items").arg(todoModel ? todoModel.count : 0)
            color: "#666"
            Layout.fillWidth: true
        }
        
        // ===================================
        // Input Area
        // ===================================
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            TextField {
                id: inputField
                Layout.fillWidth: true
                placeholderText: qsTr("Nhập todo mới...")
                onAccepted: addButton.clicked()
            }
            
            ComboBox {
                id: priorityCombo
                model: [
                    {text: "High", value: 1},
                    {text: "Medium", value: 2},
                    {text: "Low", value: 3}
                ]
                textRole: "text"
                currentIndex: 1
                Layout.preferredWidth: 100
            }
            
            Button {
                id: addButton
                text: qsTr("➕ Thêm")
                highlighted: true
                enabled: inputField.text.length > 0
                
                onClicked: {
                    if (inputField.text.length > 0) {
                        todoModel.addTodo(
                            inputField.text,
                            priorityCombo.model[priorityCombo.currentIndex].value
                        )
                        inputField.text = ""
                        inputField.focus = true
                    }
                }
            }
        }
        
        // ===================================
        // Filter Buttons
        // ===================================
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: qsTr("📋 Tất cả")
                flat: true
                checked: listView.filterMode === "all"
                onClicked: listView.filterMode = "all"
            }
            
            Button {
                text: qsTr("⏳ Chưa xong")
                flat: true
                checked: listView.filterMode === "incomplete"
                onClicked: listView.filterMode = "incomplete"
            }
            
            Button {
                text: qsTr("✅ Đã xong")
                flat: true
                checked: listView.filterMode === "completed"
                onClicked: listView.filterMode = "completed"
            }
        }
        
        // ===================================
        // Todo List
        // ===================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f5f5f5"
            border.color: "#ddd"
            border.width: 1
            radius: 5
            
            ListView {
                id: listView
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5
                clip: true
                
                property string filterMode: "all"
                
                model: todoModel
                
                // Custom delegate
                delegate: Rectangle {
                    width: listView.width
                    height: 60
                    color: model.completed ? "#e8f5e9" : "white"
                    border.color: getPriorityColor(model.priority)
                    border.width: 2
                    radius: 5
                    
                    // Priority color
                    function getPriorityColor(priority) {
                        switch (priority) {
                            case 1: return "#f44336" // Red - High
                            case 2: return "#ff9800" // Orange - Medium
                            case 3: return "#4caf50" // Green - Low
                            default: return "#999"
                        }
                    }
                    
                    // Filter logic
                    visible: {
                        if (listView.filterMode === "all") return true
                        if (listView.filterMode === "completed") return model.completed
                        if (listView.filterMode === "incomplete") return !model.completed
                        return true
                    }
                    
                    height: visible ? 60 : 0
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        
                        // Checkbox
                        CheckBox {
                            checked: model.completed
                            onClicked: todoModel.toggleCompleted(index)
                        }
                        
                        // Text
                        Label {
                            Layout.fillWidth: true
                            text: model.text
                            font.pixelSize: 16
                            font.strikeout: model.completed
                            color: model.completed ? "#888" : "#000"
                            wrapMode: Text.WordWrap
                        }
                        
                        // Priority badge
                        Rectangle {
                            width: 60
                            height: 24
                            color: getPriorityColor(model.priority)
                            radius: 12
                            
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
                            
                            onClicked: todoModel.removeTodo(index)
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
                    font.pixelSize: 16
                }
            }
        }
        
        // ===================================
        // Statistics
        // ===================================
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "#e3f2fd"
            radius: 5
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20
                
                Label {
                    text: qsTr("📊 Thống kê:")
                    font.bold: true
                }
                
                Label {
                    text: qsTr("Tổng: %1").arg(todoModel ? todoModel.count : 0)
                }
                
                Label {
                    id: completedLabel
                    text: {
                        var completed = 0
                        if (todoModel) {
                            for (var i = 0; i < todoModel.count; i++) {
                                if (todoModel.getCompleted(i)) {
                                    completed++
                                }
                            }
                        }
                        return qsTr("✅ Hoàn thành: %1").arg(completed)
                    }
                    color: "#4caf50"
                }
                
                Label {
                    text: {
                        var incomplete = 0
                        if (todoModel) {
                            for (var i = 0; i < todoModel.count; i++) {
                                if (!todoModel.getCompleted(i)) {
                                    incomplete++
                                }
                            }
                        }
                        return qsTr("⏳ Còn lại: %1").arg(incomplete)
                    }
                    color: "#ff9800"
                }
            }
        }
    }
}
