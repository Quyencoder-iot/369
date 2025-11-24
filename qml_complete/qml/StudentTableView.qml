import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

/**
 * ============================================
 * EXAMPLE 2: STUDENT TABLE VIEW (QML)
 * ============================================
 * 
 * Model: StudentTableModel (C++ - QAbstractTableModel)
 * 
 * MỤC ĐÍCH:
 * - Hiển thị bảng sinh viên (6 columns)
 * - Add/Edit/Delete students
 * - Search/Filter
 * - Display statistics
 * 
 * KEY CONCEPTS:
 * 1. TableView - Hiển thị data dạng bảng
 * 2. HorizontalHeaderView - Headers cho columns
 * 3. columnWidthProvider - Custom column widths
 * 4. model.roleName - Access C++ roles
 * 5. get() method - Access data từ C++ model
 */

ApplicationWindow {
    id: root
    visible: true
    width: 1000
    height: 700
    title: qsTr("👨‍🎓 Student Table - QML View")
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // ============================================
        // HEADER
        // ============================================
        Label {
            text: qsTr("👨‍🎓 Student Management System")
            font.pixelSize: 24
            font.bold: true
        }
        
        Label {
            text: qsTr("Total students: ") + studentModel.rowCount
            color: "#666"
            font.pixelSize: 14
        }
        
        // ============================================
        // ADD STUDENT FORM
        // ============================================
        GroupBox {
            title: qsTr("➕ Add New Student")
            Layout.fillWidth: true
            
            GridLayout {
                anchors.fill: parent
                columns: 6
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
                }
                
                Label { text: qsTr("Grade:") }
                TextField {
                    id: gradeField
                    Layout.preferredWidth: 60
                    placeholderText: "A, B"
                }
                
                Label { text: qsTr("Email:") }
                TextField {
                    id: emailField
                    Layout.preferredWidth: 200
                    placeholderText: "email@example.com"
                }
                
                Label { text: qsTr("Phone:") }
                TextField {
                    id: phoneField
                    Layout.preferredWidth: 120
                    placeholderText: "0901234567"
                }
                
                Button {
                    text: qsTr("➕ Add")
                    highlighted: true
                    Layout.columnSpan: 2
                    enabled: nameField.text.length > 0
                    
                    onClicked: {
                        // ★ CALL C++ METHOD
                        studentModel.addStudent(
                            nameField.text,
                            ageSpinBox.value,
                            gradeField.text || "B",
                            emailField.text,
                            phoneField.text
                        )
                        
                        // Clear fields
                        nameField.text = ""
                        gradeField.text = ""
                        emailField.text = ""
                        phoneField.text = ""
                        ageSpinBox.value = 20
                    }
                }
            }
        }
        
        // ============================================
        // SEARCH BAR
        // ============================================
        RowLayout {
            Layout.fillWidth: true
            
            Label { text: qsTr("🔍 Search:") }
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search by name or email...")
            }
            Button {
                text: qsTr("Clear")
                onClicked: searchField.text = ""
            }
        }
        
        // ============================================
        // STATISTICS BAR
        // Property Binding: Auto-update from C++
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "#e8eaf6"
            radius: 6
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                
                Label {
                    text: qsTr("📊 Grade A: ") + studentModel.gradeACount()
                    color: "#4caf50"
                }
                Label {
                    text: qsTr("📊 Average Age: ") + studentModel.averageAge().toFixed(1)
                    color: "#2196f3"
                }
            }
        }
        
        // ============================================
        // TABLE VIEW
        // 
        // KEY CONCEPT:
        // - TableView khác ListView: 2D grid thay vì 1D list
        // - columnWidthProvider: Định nghĩa width cho mỗi column
        // - rowHeightProvider: Định nghĩa height cho mỗi row
        // - row/column properties trong delegate
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "#ddd"
            border.width: 1
            
            // ============================================
            // HORIZONTAL HEADER
            // ============================================
            HorizontalHeaderView {
                id: horizontalHeader
                anchors.left: tableView.left
                anchors.top: parent.top
                syncView: tableView
                clip: true
                
                delegate: Rectangle {
                    implicitWidth: getColumnWidth(index)
                    implicitHeight: 45
                    color: "#3f51b5"
                    
                    Label {
                        anchors.centerIn: parent
                        text: getColumnName(index)
                        color: "white"
                        font.bold: true
                        font.pixelSize: 14
                    }
                }
            }
            
            // ============================================
            // TABLE VIEW - Main data display
            // ============================================
            TableView {
                id: tableView
                anchors.top: horizontalHeader.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 1
                clip: true
                
                // ★ KẾT NỐI VỚI C++ MODEL
                model: studentModel
                
                // Column widths
                columnWidthProvider: function(column) {
                    return getColumnWidth(column)
                }
                
                rowHeightProvider: function(row) {
                    return 55
                }
                
                // ★ DELEGATE - Template cho mỗi cell
                delegate: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 55
                    border.color: "#e0e0e0"
                    border.width: 1
                    
                    // Background color
                    color: {
                        // Filter by search
                        if (searchField.text.length > 0) {
                            var name = studentModel.get(row, "name").toString()
                            var email = studentModel.get(row, "email").toString()
                            var search = searchField.text.toLowerCase()
                            
                            if (!name.toLowerCase().includes(search) &&
                                !email.toLowerCase().includes(search)) {
                                return "#f5f5f5"
                            }
                        }
                        
                        // Alternating colors
                        return (row % 2 === 0) ? "white" : "#fafafa"
                    }
                    
                    // Hover effect
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.color = "#e3f2fd"
                        onExited: parent.color = (row % 2 === 0) ? "white" : "#fafafa"
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 5
                        
                        // ★ ACCESS C++ MODEL ROLES
                        Label {
                            Layout.fillWidth: true
                            text: getCellText(row, column)
                            elide: Text.ElideRight
                            horizontalAlignment: getAlignment(column)
                            font.bold: column === 0
                            
                            // Grade color
                            color: {
                                if (column === 3) {  // Grade column
                                    var grade = model.grade.toString()
                                    if (grade.startsWith("A")) return "#4caf50"
                                    if (grade.startsWith("C") || grade.startsWith("D")) return "#f44336"
                                }
                                return "#000"
                            }
                        }
                        
                        // Delete button (last column only)
                        Button {
                            visible: column === 5
                            text: "🗑️"
                            flat: true
                            Layout.preferredWidth: 40
                            
                            onClicked: {
                                deleteDialog.studentRow = row
                                deleteDialog.studentName = model.name
                                deleteDialog.open()
                            }
                        }
                    }
                }
            }
        }
    }
    
    // ============================================
    // DELETE DIALOG
    // ============================================
    Dialog {
        id: deleteDialog
        title: qsTr("Confirm Delete")
        modal: true
        anchors.centerIn: parent
        
        property int studentRow: -1
        property string studentName: ""
        
        Label {
            text: qsTr("Delete student '%1'?").arg(deleteDialog.studentName)
        }
        
        standardButtons: Dialog.Yes | Dialog.No
        
        onAccepted: {
            // ★ CALL C++ METHOD
            studentModel.removeStudent(deleteDialog.studentRow)
        }
    }
    
    // ============================================
    // HELPER FUNCTIONS
    // ============================================
    function getColumnName(col) {
        switch (col) {
            case 0: return "ID"
            case 1: return "Tên"
            case 2: return "Tuổi"
            case 3: return "Điểm"
            case 4: return "Email"
            case 5: return "SĐT"
            default: return ""
        }
    }
    
    function getColumnWidth(col) {
        switch (col) {
            case 0: return 60   // ID
            case 1: return 200  // Name
            case 2: return 80   // Age
            case 3: return 80   // Grade
            case 4: return 250  // Email
            case 5: return 180  // Phone
            default: return 100
        }
    }
    
    function getCellText(row, col) {
        switch (col) {
            case 0: return model.studentId
            case 1: return model.name
            case 2: return model.age
            case 3: return model.grade
            case 4: return model.email
            case 5: return model.phone
            default: return ""
        }
    }
    
    function getAlignment(col) {
        return (col === 0 || col === 2) ? Text.AlignHCenter : Text.AlignLeft
    }
}

/**
 * ============================================
 * 📚 SUMMARY - STUDENT TABLE VIEW
 * ============================================
 * 
 * 1. TABLE MODEL DIFFERENCES:
 *    ✅ TableView thay vì ListView
 *    ✅ 2D grid (rows × columns)
 *    ✅ columnWidthProvider cho custom widths
 *    ✅ HorizontalHeaderView cho column headers
 * 
 * 2. ACCESSING DATA:
 *    ✅ model.roleName - Access roles trong delegate
 *    ✅ studentModel.get(row, "roleName") - Get data by row
 *    ✅ row & column properties trong delegate
 * 
 * 3. C++ METHODS CALLED:
 *    - addStudent(name, age, grade, email, phone)
 *    - removeStudent(row)
 *    - get(row, roleName) - Get specific cell data
 *    - gradeACount() - Statistics
 *    - averageAge() - Statistics
 * 
 * 4. C++ PROPERTIES:
 *    - rowCount (int) - Number of students
 * 
 * 5. ROLES USED:
 *    - studentId (IdRole) → int
 *    - name (NameRole) → QString
 *    - age (AgeRole) → int
 *    - grade (GradeRole) → QString
 *    - email (EmailRole) → QString
 *    - phone (PhoneRole) → QString
 * 
 * 6. KEY DIFFERENCES vs ListView:
 *    ListView (1D):
 *      - model[index].roleName
 *      - Chỉ có index
 *    
 *    TableView (2D):
 *      - model[row, column].roleName
 *      - Có row VÀ column
 *      - Cần columnWidthProvider
 *      - Cần HorizontalHeaderView
 * 
 * 7. SEARCH/FILTER:
 *    - Local QML implementation
 *    - Change background color khi không match
 *    - Không hide rows (khác với ListView filter)
 * 
 * ============================================
 * 💡 LEARNING POINTS:
 * ============================================
 * 
 * ★ TableView setup:
 *   TableView {
 *       model: cppTableModel
 *       columnWidthProvider: function(col) { return width }
 *       delegate: Rectangle { /* row, column available */ }
 *   }
 * 
 * ★ Access cell data:
 *   text: model.roleName  // Direct role access
 *   text: cppModel.get(row, "roleName")  // Method access
 * 
 * ★ Column headers:
 *   HorizontalHeaderView {
 *       syncView: tableView
 *       delegate: Rectangle { /* header cell */ }
 *   }
 * 
 * ★ 2D positioning:
 *   delegate có: row, column properties
 *   Dùng để xác định vị trí cell trong table
 * 
 * ============================================
 */
