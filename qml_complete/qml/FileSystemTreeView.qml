import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * ============================================
 * EXAMPLE 3: FILE SYSTEM TREE VIEW (QML)
 * ============================================
 * 
 * Model: FileSystemTreeModel (C++ - QAbstractItemModel)
 * 
 * MỤC ĐÍCH:
 * - Hiển thị cấu trúc phân cấp (hierarchical data)
 * - Expand/Collapse nodes
 * - Display file info (name, size, type)
 * - Add/Remove files/folders
 * 
 * KEY CONCEPTS:
 * 1. TreeView - Hiển thị hierarchical data
 * 2. QModelIndex - Reference to tree node
 * 3. model.index() - Access child nodes
 * 4. model.hasChildren() - Check if node has children
 * 5. Recursive delegate - Tree structure
 */

ApplicationWindow {
    id: root
    visible: true
    width: 700
    height: 600
    title: qsTr("📁 File System Tree - QML View")
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // ============================================
        // HEADER
        // ============================================
        Label {
            text: qsTr("📁 File System Browser")
            font.pixelSize: 24
            font.bold: true
        }
        
        // ============================================
        // STATISTICS
        // Property Binding từ C++
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: "#e8f5e9"
            radius: 6
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20
                
                Label {
                    text: qsTr("📊 Total Items: ") + treeModel.itemCount
                    font.pixelSize: 14
                }
                
                Label {
                    text: qsTr("📂 Folders: ") + treeModel.folderCount
                    font.pixelSize: 14
                    color: "#ff9800"
                }
                
                Label {
                    text: qsTr("📄 Files: ") + treeModel.fileCount
                    font.pixelSize: 14
                    color: "#2196f3"
                }
            }
        }
        
        // ============================================
        // ADD NEW ITEM FORM
        // ============================================
        GroupBox {
            title: qsTr("➕ Add New Item")
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                spacing: 10
                
                TextField {
                    id: nameField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Enter name...")
                }
                
                ComboBox {
                    id: typeCombo
                    model: ["📄 File", "📁 Folder"]
                    currentIndex: 0
                }
                
                Button {
                    text: qsTr("Add to Root")
                    highlighted: true
                    enabled: nameField.text.length > 0
                    
                    onClicked: {
                        // ★ CALL C++ METHOD
                        var isFolder = (typeCombo.currentIndex === 1)
                        treeModel.addItem(nameField.text, isFolder, treeModel.rootIndex())
                        nameField.text = ""
                    }
                }
            }
        }
        
        // ============================================
        // TREE VIEW
        // 
        // KEY CONCEPT:
        // - TreeView trong Qt 6.2+ là TreeView component mới
        // - Trong Qt 5, dùng ListView với recursive delegate
        // - model cung cấp hierarchical structure
        // - Expand/collapse state
        // ============================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"
            border.color: "#ddd"
            border.width: 1
            radius: 6
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 5
                clip: true
                
                // ============================================
                // TREE LISTVIEW
                // Sử dụng ListView với recursive delegate
                // ============================================
                ListView {
                    id: treeView
                    
                    // ★ KẾT NỐI VỚI C++ TREE MODEL
                    model: treeModel
                    
                    // ★ DELEGATE - Template cho mỗi tree node
                    delegate: TreeNodeDelegate {}
                    
                    // Empty state
                    Label {
                        anchors.centerIn: parent
                        visible: treeView.count === 0
                        text: qsTr("📁 Empty tree\nAdd some items!")
                        horizontalAlignment: Text.AlignHCenter
                        color: "#999"
                        font.pixelSize: 16
                    }
                }
            }
        }
    }
    
    // ============================================
    // TREE NODE DELEGATE (Reusable Component)
    // 
    // KEY CONCEPT:
    // - Recursive component cho tree structure
    // - Hiển thị node và children
    // - Handle expand/collapse
    // ============================================
    Component {
        id: TreeNodeDelegate
        
        Column {
            width: treeView.width
            
            // ============================================
            // NODE ROW
            // ============================================
            Rectangle {
                id: nodeRect
                width: parent.width
                height: 45
                color: nodeMouseArea.containsMouse ? "#f0f0f0" : "white"
                
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: model.depth * 25 + 10  // Indent by depth
                    anchors.rightMargin: 10
                    spacing: 8
                    
                    // Expand/Collapse button
                    Button {
                        visible: model.hasChildren
                        text: model.isExpanded ? "▼" : "▶"
                        flat: true
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 30
                        
                        onClicked: {
                            // ★ CALL C++ METHOD để toggle expand
                            treeModel.toggleExpanded(model.index)
                        }
                    }
                    
                    // Icon
                    Label {
                        text: model.isFolder ? "📁" : "📄"
                        font.pixelSize: 20
                    }
                    
                    // Name
                    Label {
                        text: model.name
                        font.pixelSize: 14
                        font.bold: model.isFolder
                        Layout.fillWidth: true
                    }
                    
                    // Size (files only)
                    Label {
                        visible: !model.isFolder
                        text: formatSize(model.size)
                        font.pixelSize: 12
                        color: "#666"
                    }
                    
                    // Modified date
                    Label {
                        text: model.modifiedDate
                        font.pixelSize: 11
                        color: "#999"
                        Layout.preferredWidth: 150
                    }
                    
                    // Add child button (folders only)
                    Button {
                        visible: model.isFolder
                        text: "➕"
                        flat: true
                        
                        onClicked: {
                            addChildDialog.parentIndex = model.index
                            addChildDialog.parentName = model.name
                            addChildDialog.open()
                        }
                    }
                    
                    // Delete button
                    Button {
                        text: "🗑️"
                        flat: true
                        
                        onClicked: {
                            deleteDialog.itemIndex = model.index
                            deleteDialog.itemName = model.name
                            deleteDialog.open()
                        }
                    }
                }
                
                MouseArea {
                    id: nodeMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.NoButton
                }
            }
            
            // ============================================
            // CHILDREN (Recursive)
            // Hiển thị nếu node được expand
            // ============================================
            Repeater {
                model: model.hasChildren && model.isExpanded ? model.childCount : 0
                
                delegate: Loader {
                    width: parent.width
                    sourceComponent: TreeNodeDelegate
                    
                    // ★ PASS CHILD MODEL INDEX
                    property var childModel: treeModel.getChild(model.index, index)
                    
                    onLoaded: {
                        item.model = childModel
                    }
                }
            }
        }
    }
    
    // ============================================
    // ADD CHILD DIALOG
    // ============================================
    Dialog {
        id: addChildDialog
        title: qsTr("Add Item to '%1'").arg(parentName)
        modal: true
        anchors.centerIn: parent
        width: 400
        
        property var parentIndex: null
        property string parentName: ""
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            Label { text: qsTr("Name:") }
            TextField {
                id: childNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Enter name...")
            }
            
            Label { text: qsTr("Type:") }
            ComboBox {
                id: childTypeCombo
                Layout.fillWidth: true
                model: ["📄 File", "📁 Folder"]
            }
        }
        
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        onAccepted: {
            if (childNameField.text.length > 0) {
                var isFolder = (childTypeCombo.currentIndex === 1)
                treeModel.addItem(childNameField.text, isFolder, addChildDialog.parentIndex)
                childNameField.text = ""
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
        
        property var itemIndex: null
        property string itemName: ""
        
        Label {
            text: qsTr("Delete '%1'?").arg(deleteDialog.itemName)
        }
        
        standardButtons: Dialog.Yes | Dialog.No
        
        onAccepted: {
            treeModel.removeItem(deleteDialog.itemIndex)
        }
    }
    
    // ============================================
    // HELPER FUNCTIONS
    // ============================================
    function formatSize(bytes) {
        if (bytes < 1024) return bytes + " B"
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + " KB"
        return (bytes / (1024 * 1024)).toFixed(1) + " MB"
    }
}

/**
 * ============================================
 * 📚 SUMMARY - FILE SYSTEM TREE VIEW
 * ============================================
 * 
 * 1. TREE MODEL CONCEPTS:
 *    ✅ Hierarchical data (parent-child relationships)
 *    ✅ QModelIndex - Reference to nodes
 *    ✅ Recursive structure - Nodes chứa nodes
 *    ✅ Expand/Collapse state
 * 
 * 2. QML TREE IMPLEMENTATION:
 *    ✅ ListView with recursive delegate
 *    ✅ Indent by depth: leftMargin = depth * 25
 *    ✅ Repeater for children nodes
 *    ✅ Loader for recursive components
 * 
 * 3. C++ METHODS CALLED:
 *    - addItem(name, isFolder, parentIndex)
 *    - removeItem(index)
 *    - toggleExpanded(index)
 *    - getChild(parentIndex, childRow) - Get child node
 *    - rootIndex() - Get root node index
 * 
 * 4. C++ PROPERTIES:
 *    - itemCount (total items)
 *    - folderCount
 *    - fileCount
 * 
 * 5. ROLES USED:
 *    - name (NameRole) → QString
 *    - isFolder (IsFolderRole) → bool
 *    - size (SizeRole) → int (bytes)
 *    - modifiedDate (ModifiedDateRole) → QString
 *    - depth (DepthRole) → int (tree level)
 *    - hasChildren (HasChildrenRole) → bool
 *    - isExpanded (IsExpandedRole) → bool
 *    - childCount (ChildCountRole) → int
 * 
 * 6. KEY DIFFERENCES vs List/Table:
 *    List/Table:
 *      - Flat structure (1D hoặc 2D)
 *      - Simple iteration
 *    
 *    Tree:
 *      - Hierarchical (N levels)
 *      - Parent-child relationships
 *      - Recursive navigation
 *      - QModelIndex for node reference
 * 
 * 7. RECURSIVE DELEGATE:
 *    Component {
 *        Column {
 *            Rectangle { /* Node display */ }
 *            Repeater {
 *                delegate: Loader {
 *                    sourceComponent: TreeNodeDelegate  // Recursion!
 *                }
 *            }
 *        }
 *    }
 * 
 * 8. EXPAND/COLLAPSE:
 *    - State lưu trong C++ model
 *    - toggleExpanded(index) thay đổi state
 *    - QML delegate re-renders children khi state changes
 * 
 * ============================================
 * 💡 LEARNING POINTS:
 * ============================================
 * 
 * ★ QModelIndex concept:
 *   - Unique reference đến tree node
 *   - Pass giữa QML ↔ C++
 *   - Dùng để identify parent/child
 * 
 * ★ Recursive delegate:
 *   Component gọi chính nó để hiển thị children
 *   → Tạo tree structure trong QML
 * 
 * ★ Depth-based indent:
 *   leftMargin: model.depth * INDENT_SIZE
 *   → Visual hierarchy
 * 
 * ★ Expand/Collapse logic:
 *   - C++ model stores state
 *   - QML shows/hides children based on state
 *   - Button calls toggleExpanded()
 * 
 * ★ Parent-Child operations:
 *   addItem(name, isFolder, parentIndex)
 *   → Add child to specific parent
 * 
 * ============================================
 */
