import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

/**
 * ============================================
 * EXAMPLE 4: EMPLOYEE SQL VIEW (QML)
 * ============================================
 * 
 * Model: EmployeeSqlModel (C++ - QSqlRelationalTableModel)
 * 
 * MỤC ĐÍCH:
 * - Hiển thị data từ SQL database
 * - CRUD operations (Create, Read, Update, Delete)
 * - Foreign key relationships (Department)
 * - Auto-save to database
 * 
 * KEY CONCEPTS:
 * 1. QSqlTableModel - Direct database mapping
 * 2. QSqlRelationalTableModel - Foreign key support
 * 3. setData() - Edit cells trực tiếp
 * 4. submit() - Commit changes to database
 * 5. revert() - Rollback changes
 */

ApplicationWindow {
    id: root
    visible: true
    width: 1100
    height: 700
    title: qsTr("👔 Employee Database - QML View")
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // ============================================
        // HEADER
        // ============================================
        Label {
            text: qsTr("👔 Employee Management System (SQL)")
            font.pixelSize: 24
            font.bold: true
        }
        
        // ============================================
        // DATABASE INFO
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "#e1f5fe"
            radius: 6
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20
                
                Label {
                    text: qsTr("💾 Database: In-Memory SQLite")
                    font.pixelSize: 13
                }
                
                Label {
                    text: qsTr("📊 Total Employees: ") + employeeModel.rowCount()
                    font.pixelSize: 13
                }
                
                Label {
                    text: qsTr("💰 Average Salary: $") + employeeModel.averageSalary().toFixed(0)
                    font.pixelSize: 13
                    color: "#4caf50"
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: employeeModel.hasUnsavedChanges ? "⚠️ Unsaved changes" : "✅ Saved"
                    color: employeeModel.hasUnsavedChanges ? "#ff9800" : "#4caf50"
                    font.pixelSize: 13
                }
            }
        }
        
        // ============================================
        // ADD EMPLOYEE FORM
        // ============================================
        GroupBox {
            title: qsTr("➕ Add New Employee")
            Layout.fillWidth: true
            
            GridLayout {
                anchors.fill: parent
                columns: 8
                columnSpacing: 10
                rowSpacing: 10
                
                Label { text: qsTr("Name:") }
                TextField {
                    id: nameField
                    Layout.preferredWidth: 150
                    placeholderText: "John Doe"
                }
                
                Label { text: qsTr("Position:") }
                TextField {
                    id: positionField
                    Layout.preferredWidth: 120
                    placeholderText: "Developer"
                }
                
                Label { text: qsTr("Dept:") }
                ComboBox {
                    id: deptCombo
                    Layout.preferredWidth: 120
                    model: employeeModel.departments
                }
                
                Label { text: qsTr("Salary:") }
                SpinBox {
                    id: salarySpinBox
                    from: 10000
                    to: 500000
                    stepSize: 5000
                    value: 50000
                    editable: true
                }
                
                Button {
                    text: qsTr("➕ Add")
                    highlighted: true
                    Layout.columnSpan: 2
                    enabled: nameField.text.length > 0 && positionField.text.length > 0
                    
                    onClicked: {
                        // ★ CALL C++ METHOD
                        employeeModel.addEmployee(
                            nameField.text,
                            positionField.text,
                            deptCombo.currentIndex + 1,  // Department ID
                            salarySpinBox.value
                        )
                        
                        // Clear fields
                        nameField.text = ""
                        positionField.text = ""
                        salarySpinBox.value = 50000
                    }
                }
            }
        }
        
        // ============================================
        // ACTION BUTTONS
        // ============================================
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: qsTr("💾 Save Changes")
                highlighted: true
                enabled: employeeModel.hasUnsavedChanges
                
                onClicked: {
                    // ★ COMMIT TO DATABASE
                    employeeModel.submit()
                    statusLabel.text = "✅ Saved successfully!"
                    statusLabel.color = "#4caf50"
                }
            }
            
            Button {
                text: qsTr("↶ Revert Changes")
                enabled: employeeModel.hasUnsavedChanges
                
                onClicked: {
                    // ★ ROLLBACK CHANGES
                    employeeModel.revert()
                    statusLabel.text = "↶ Changes reverted"
                    statusLabel.color = "#ff9800"
                }
            }
            
            Button {
                text: qsTr("🔄 Refresh")
                
                onClicked: {
                    employeeModel.select()
                    statusLabel.text = "🔄 Refreshed"
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                id: statusLabel
                text: ""
                font.pixelSize: 13
            }
        }
        
        // ============================================
        // EMPLOYEE TABLE
        // 
        // KEY CONCEPT:
        // - TableView với SQL model
        // - Editable cells via setData()
        // - Auto-commit hoặc manual submit()
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "#ddd"
            border.width: 1
            
            // ============================================
            // HEADER
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
                    color: "#1976d2"
                    
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
            // TABLE VIEW
            // ============================================
            TableView {
                id: tableView
                anchors.top: horizontalHeader.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 1
                clip: true
                
                // ★ KẾT NỐI VỚI SQL MODEL
                model: employeeModel
                
                columnWidthProvider: function(column) {
                    return getColumnWidth(column)
                }
                
                rowHeightProvider: function(row) {
                    return 60
                }
                
                // ★ DELEGATE - Editable cells
                delegate: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 60
                    border.color: "#e0e0e0"
                    border.width: 1
                    color: (row % 2 === 0) ? "white" : "#fafafa"
                    
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.color = "#fff9c4"
                        onExited: parent.color = (row % 2 === 0) ? "white" : "#fafafa"
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 5
                        
                        // ★ EDITABLE FIELD
                        // Chỉ cho phép edit Name, Position, Salary
                        TextField {
                            visible: column >= 1 && column <= 3
                            Layout.fillWidth: true
                            text: getCellText(row, column)
                            
                            // Read-only for department (foreign key)
                            readOnly: column === 3
                            
                            onEditingFinished: {
                                // ★ UPDATE DATABASE
                                if (text !== getCellText(row, column)) {
                                    employeeModel.setData(row, column, text)
                                    statusLabel.text = "⚠️ Unsaved changes"
                                    statusLabel.color = "#ff9800"
                                }
                            }
                        }
                        
                        // Non-editable fields (ID)
                        Label {
                            visible: column === 0
                            Layout.fillWidth: true
                            text: getCellText(row, column)
                            horizontalAlignment: Text.AlignHCenter
                            font.bold: true
                        }
                        
                        // Salary with formatting
                        Label {
                            visible: column === 4
                            Layout.fillWidth: true
                            text: "$" + parseInt(getCellText(row, column)).toLocaleString()
                            horizontalAlignment: Text.AlignRight
                            color: parseInt(getCellText(row, column)) > 70000 ? "#4caf50" : "#000"
                            font.bold: parseInt(getCellText(row, column)) > 70000
                        }
                        
                        // Delete button
                        Button {
                            visible: column === 5
                            text: "🗑️"
                            flat: true
                            
                            onClicked: {
                                deleteDialog.employeeRow = row
                                deleteDialog.employeeName = model.name
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
        
        property int employeeRow: -1
        property string employeeName: ""
        
        Label {
            text: qsTr("Delete employee '%1'?").arg(deleteDialog.employeeName)
        }
        
        standardButtons: Dialog.Yes | Dialog.No
        
        onAccepted: {
            employeeModel.removeEmployee(deleteDialog.employeeRow)
            statusLabel.text = "✅ Deleted"
            statusLabel.color = "#4caf50"
        }
    }
    
    // ============================================
    // HELPER FUNCTIONS
    // ============================================
    function getColumnName(col) {
        switch (col) {
            case 0: return "ID"
            case 1: return "Name"
            case 2: return "Position"
            case 3: return "Department"
            case 4: return "Salary"
            case 5: return "Actions"
            default: return ""
        }
    }
    
    function getColumnWidth(col) {
        switch (col) {
            case 0: return 60   // ID
            case 1: return 200  // Name
            case 2: return 180  // Position
            case 3: return 180  // Department
            case 4: return 150  // Salary
            case 5: return 100  // Actions
            default: return 100
        }
    }
    
    function getCellText(row, col) {
        switch (col) {
            case 0: return model.id
            case 1: return model.name
            case 2: return model.position
            case 3: return model.department
            case 4: return model.salary
            default: return ""
        }
    }
}

/**
 * ============================================
 * 📚 SUMMARY - EMPLOYEE SQL VIEW
 * ============================================
 * 
 * 1. SQL MODEL KEY FEATURES:
 *    ✅ Direct database mapping (table → model)
 *    ✅ Editable cells (setData)
 *    ✅ Transaction support (submit/revert)
 *    ✅ Foreign key relationships (QSqlRelationalTableModel)
 *    ✅ Auto-sync với database
 * 
 * 2. C++ METHODS CALLED:
 *    - addEmployee(name, position, deptId, salary)
 *    - removeEmployee(row)
 *    - setData(row, column, value) - Edit cell
 *    - submit() - Save to database
 *    - revert() - Rollback changes
 *    - select() - Refresh from database
 *    - averageSalary() - Statistics
 * 
 * 3. C++ PROPERTIES:
 *    - hasUnsavedChanges (bool) - Dirty flag
 *    - departments (QStringList) - For ComboBox
 *    - rowCount() (int)
 * 
 * 4. ROLES USED:
 *    - id (IdRole) → int
 *    - name (NameRole) → QString
 *    - position (PositionRole) → QString
 *    - department (DepartmentRole) → QString (from foreign key)
 *    - salary (SalaryRole) → int
 * 
 * 5. EDITING WORKFLOW:
 *    User edits TextField
 *         ↓
 *    onEditingFinished
 *         ↓
 *    employeeModel.setData(row, col, value)
 *         ↓
 *    Model marks as dirty (hasUnsavedChanges = true)
 *         ↓
 *    User clicks "Save"
 *         ↓
 *    employeeModel.submit()
 *         ↓
 *    SQL UPDATE executed
 *         ↓
 *    Database updated
 * 
 * 6. FOREIGN KEY HANDLING:
 *    - departments table linked to employees table
 *    - QSqlRelationalTableModel auto-resolves foreign keys
 *    - Display department name instead of ID
 *    - ComboBox populated from departments list
 * 
 * 7. TRANSACTION SUPPORT:
 *    submit() → COMMIT changes to database
 *    revert() → ROLLBACK to last saved state
 *    select() → Re-query database
 * 
 * 8. KEY DIFFERENCES vs Non-SQL Models:
 *    Non-SQL Models:
 *      - Data in memory (QList, QVector)
 *      - Manual save logic
 *    
 *    SQL Models:
 *      - Data in database
 *      - Auto-sync with database
 *      - Transaction support
 *      - Foreign key relationships
 * 
 * ============================================
 * 💡 LEARNING POINTS:
 * ============================================
 * 
 * ★ SQL Model setup (C++):
 *   QSqlRelationalTableModel model;
 *   model.setTable("employees");
 *   model.setRelation(3, QSqlRelation("departments", "id", "name"));
 *   model.select();  // Load data
 * 
 * ★ Edit cells:
 *   TextField {
 *       onEditingFinished: sqlModel.setData(row, col, text)
 *   }
 * 
 * ★ Save changes:
 *   Button { onClicked: sqlModel.submit() }
 * 
 * ★ Foreign keys:
 *   QSqlRelation("foreign_table", "foreign_key", "display_field")
 *   → Shows human-readable text instead of IDs
 * 
 * ★ Transaction control:
 *   submit() → Save all changes
 *   revert() → Discard all changes
 * 
 * ============================================
 */
