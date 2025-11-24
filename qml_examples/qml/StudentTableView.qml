import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

/**
 * StudentTableView - QML View cho StudentTableModel
 * 
 * Hiển thị bảng sinh viên với:
 * - TableView
 * - Add/Edit/Delete operations
 * - Search filter
 */
ApplicationWindow {
    id: root
    visible: true
    width: 900
    height: 600
    title: qsTr("👨‍🎓 Student Table - QML + C++ Model")
    
    property var studentModel
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // ===================================
        // Header
        // ===================================
        Label {
            text: qsTr("Student Management")
            font.pixelSize: 24
            font.bold: true
        }
        
        Label {
            text: qsTr("Total students: %1").arg(studentModel ? studentModel.rowCount : 0)
            color: "#666"
        }
        
        // ===================================
        // Add Student Form
        // ===================================
        GroupBox {
            title: qsTr("➕ Add New Student")
            Layout.fillWidth: true
            
            GridLayout {
                anchors.fill: parent
                columns: 5
                columnSpacing: 10
                rowSpacing: 10
                
                Label { text: qsTr("Name:") }
                TextField {
                    id: nameField
                    Layout.preferredWidth: 150
                    placeholderText: "Nguyễn Văn A"
                }
                
                Label { text: qsTr("Age:") }
                SpinBox {
                    id: ageSpinBox
                    from: 15
                    to: 100
                    value: 20
                    Layout.preferredWidth: 100
                }
                
                Button {
                    text: qsTr("➕ Add")
                    highlighted: true
                    Layout.rowSpan: 2
                    enabled: nameField.text.length > 0
                    
                    onClicked: {
                        studentModel.addStudent(
                            nameField.text,
                            ageSpinBox.value,
                            gradeField.text || "B",
                            emailField.text
                        )
                        
                        // Clear fields
                        nameField.text = ""
                        gradeField.text = ""
                        emailField.text = ""
                        ageSpinBox.value = 20
                        nameField.focus = true
                    }
                }
                
                Label { text: qsTr("Grade:") }
                TextField {
                    id: gradeField
                    Layout.preferredWidth: 80
                    placeholderText: "A, B, C"
                }
                
                Label { text: qsTr("Email:") }
                TextField {
                    id: emailField
                    Layout.preferredWidth: 200
                    placeholderText: "email@example.com"
                }
            }
        }
        
        // ===================================
        // Search Bar
        // ===================================
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Label {
                text: qsTr("🔍 Search:")
            }
            
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Tìm theo tên hoặc email...")
            }
            
            Button {
                text: qsTr("Clear")
                onClicked: searchField.text = ""
            }
        }
        
        // ===================================
        // Table View
        // ===================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "#ddd"
            border.width: 1
            
            HorizontalHeaderView {
                id: horizontalHeader
                anchors.left: tableView.left
                anchors.top: parent.top
                syncView: tableView
                clip: true
                
                delegate: Rectangle {
                    implicitWidth: {
                        switch (index) {
                            case 0: return 60   // ID
                            case 1: return 200  // Name
                            case 2: return 80   // Age
                            case 3: return 80   // Grade
                            case 4: return 250  // Email
                            default: return 100
                        }
                    }
                    implicitHeight: 40
                    color: "#3f51b5"
                    
                    Label {
                        anchors.centerIn: parent
                        text: {
                            switch (index) {
                                case 0: return "ID"
                                case 1: return "Tên"
                                case 2: return "Tuổi"
                                case 3: return "Điểm"
                                case 4: return "Email"
                                default: return ""
                            }
                        }
                        color: "white"
                        font.bold: true
                    }
                }
            }
            
            TableView {
                id: tableView
                anchors.top: horizontalHeader.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 1
                clip: true
                
                model: studentModel
                
                columnWidthProvider: function(column) {
                    switch (column) {
                        case 0: return 60
                        case 1: return 200
                        case 2: return 80
                        case 3: return 80
                        case 4: return 250
                        default: return 100
                    }
                }
                
                rowHeightProvider: function(row) {
                    return 50
                }
                
                delegate: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 50
                    border.color: "#e0e0e0"
                    border.width: 1
                    color: {
                        // Search filter
                        if (searchField.text.length > 0) {
                            var name = studentModel.get(row, "name").toString()
                            var email = studentModel.get(row, "email").toString()
                            var searchText = searchField.text.toLowerCase()
                            
                            if (!name.toLowerCase().includes(searchText) &&
                                !email.toLowerCase().includes(searchText)) {
                                return "#f0f0f0"
                            }
                        }
                        
                        // Alternating colors
                        return (row % 2 === 0) ? "white" : "#f9f9f9"
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.color = "#e3f2fd"
                        onExited: parent.color = (row % 2 === 0) ? "white" : "#f9f9f9"
                        
                        onClicked: {
                            tableView.currentRow = row
                        }
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: 5
                        
                        // Display text
                        Label {
                            Layout.fillWidth: true
                            text: {
                                switch (column) {
                                    case 0: return model.studentId
                                    case 1: return model.name
                                    case 2: return model.age
                                    case 3: return model.grade
                                    case 4: return model.email
                                    default: return ""
                                }
                            }
                            elide: Text.ElideRight
                            horizontalAlignment: column === 0 || column === 2 ? Text.AlignHCenter : Text.AlignLeft
                            font.bold: column === 0
                            
                            // Grade color
                            color: {
                                if (column === 3) {
                                    var grade = model.grade.toString()
                                    if (grade.startsWith("A")) return "#4caf50"
                                    if (grade.startsWith("C") || grade.startsWith("D")) return "#f44336"
                                }
                                return "#000"
                            }
                        }
                        
                        // Delete button (last column only)
                        Button {
                            visible: column === 4
                            text: "🗑️"
                            flat: true
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 30
                            
                            onClicked: {
                                deleteDialog.studentRow = row
                                deleteDialog.studentName = model.name
                                deleteDialog.open()
                            }
                        }
                    }
                }
                
                property int currentRow: -1
            }
        }
        
        // ===================================
        // Statistics
        // ===================================
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "#e8eaf6"
            radius: 5
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20
                
                Label {
                    text: qsTr("📊 Statistics:")
                    font.bold: true
                }
                
                Label {
                    text: {
                        var gradeA = 0
                        if (studentModel) {
                            for (var i = 0; i < studentModel.rowCount; i++) {
                                var grade = studentModel.get(i, "grade").toString()
                                if (grade.startsWith("A")) gradeA++
                            }
                        }
                        return qsTr("Grade A: %1").arg(gradeA)
                    }
                    color: "#4caf50"
                }
                
                Label {
                    text: {
                        var total = 0
                        var count = 0
                        if (studentModel) {
                            for (var i = 0; i < studentModel.rowCount; i++) {
                                var age = studentModel.get(i, "age").toInt()
                                total += age
                                count++
                            }
                        }
                        var avg = count > 0 ? (total / count).toFixed(1) : 0
                        return qsTr("Average Age: %1").arg(avg)
                    }
                    color: "#2196f3"
                }
            }
        }
    }
    
    // ===================================
    // Delete Confirmation Dialog
    // ===================================
    Dialog {
        id: deleteDialog
        title: qsTr("Confirm Delete")
        modal: true
        anchors.centerIn: parent
        
        property int studentRow: -1
        property string studentName: ""
        
        ColumnLayout {
            Label {
                text: qsTr("Delete student '%1'?").arg(deleteDialog.studentName)
            }
        }
        
        standardButtons: Dialog.Yes | Dialog.No
        
        onAccepted: {
            studentModel.removeStudent(deleteDialog.studentRow)
        }
    }
}
