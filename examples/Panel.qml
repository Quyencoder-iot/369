import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// ============================================================================
// VIEW: Panel UI in QML
// ============================================================================

Rectangle {
    id: root
    
    // Panel dimensions
    width: 1920
    height: 40
    
    // Styling
    color: "#1e1e1e"
    
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 15
        
        // ====================================================================
        // Left Section: Screen Info
        // ====================================================================
        
        RowLayout {
            spacing: 10
            
            // Monitor icon
            Text {
                text: "🖥️"
                font.pixelSize: 20
            }
            
            // Screen name
            Text {
                id: screenNameText
                text: currentScreenName || "Unknown"
                color: "#4fc3f7"
                font.pixelSize: 14
                font.bold: true
            }
            
            // Screen info from model
            Text {
                text: {
                    for (var i = 0; i < panelModel.count; i++) {
                        if (panelModel.getScreenName(i) === currentScreenName) {
                            var width = panelModel.data(panelModel.index(i, 0), 257); // ScreenWidthRole
                            var height = panelModel.data(panelModel.index(i, 0), 258); // ScreenHeightRole
                            var dpi = panelModel.data(panelModel.index(i, 0), 259); // ScreenDpiRole
                            return width + "×" + height + " @ " + dpi + " DPI";
                        }
                    }
                    return "";
                }
                color: "#888888"
                font.pixelSize: 12
            }
        }
        
        // ====================================================================
        // Center Section: Spacer
        // ====================================================================
        
        Item {
            Layout.fillWidth: true
        }
        
        // ====================================================================
        // Right Section: System Info
        // ====================================================================
        
        RowLayout {
            spacing: 20
            
            // Total screens counter
            Rectangle {
                width: 80
                height: 25
                color: "#2d2d2d"
                radius: 5
                
                RowLayout {
                    anchors.centerIn: parent
                    spacing: 5
                    
                    Text {
                        text: "📺"
                        font.pixelSize: 14
                    }
                    
                    Text {
                        text: panelModel.count + " screens"
                        color: "white"
                        font.pixelSize: 11
                    }
                }
            }
            
            // Clock
            Text {
                id: clockText
                color: "white"
                font.pixelSize: 14
                font.family: "monospace"
                
                function updateTime() {
                    var date = new Date();
                    clockText.text = Qt.formatTime(date, "hh:mm:ss");
                }
                
                Component.onCompleted: updateTime()
                
                Timer {
                    interval: 1000
                    running: true
                    repeat: true
                    onTriggered: clockText.updateTime()
                }
            }
            
            // Settings button
            Button {
                text: "⚙️"
                flat: true
                
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 16
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                background: Rectangle {
                    color: parent.hovered ? "#3d3d3d" : "transparent"
                    radius: 5
                }
                
                onClicked: {
                    console.log("Settings clicked on", currentScreenName);
                }
            }
        }
    }
    
    // ====================================================================
    // Screen info display (tooltip-like)
    // ====================================================================
    
    Rectangle {
        id: infoPopup
        width: 200
        height: infoColumn.height + 20
        color: "#2d2d2d"
        border.color: "#4fc3f7"
        border.width: 1
        radius: 8
        visible: false
        
        anchors.top: parent.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 15
        
        Column {
            id: infoColumn
            anchors.centerIn: parent
            spacing: 5
            
            Text {
                text: "Screen Information"
                color: "#4fc3f7"
                font.bold: true
                font.pixelSize: 12
            }
            
            Text {
                text: "Name: " + currentScreenName
                color: "white"
                font.pixelSize: 10
            }
            
            Text {
                text: {
                    for (var i = 0; i < panelModel.count; i++) {
                        if (panelModel.getScreenName(i) === currentScreenName) {
                            var isPrimary = panelModel.data(panelModel.index(i, 0), 260); // IsPrimaryRole
                            return isPrimary ? "Primary: Yes" : "Primary: No";
                        }
                    }
                    return "Primary: Unknown";
                }
                color: "white"
                font.pixelSize: 10
            }
        }
    }
    
    // Show info on hover
    MouseArea {
        anchors.left: parent.left
        anchors.top: parent.top
        width: 200
        height: parent.height
        hoverEnabled: true
        
        onEntered: infoPopup.visible = true
        onExited: infoPopup.visible = false
    }
    
    // ====================================================================
    // Connections to model signals
    // ====================================================================
    
    Connections {
        target: panelModel
        
        function onCountChanged() {
            console.log("Screen count changed:", panelModel.count);
        }
        
        function onScreenAdded(name) {
            console.log("Screen added:", name);
        }
        
        function onScreenRemoved(name) {
            console.log("Screen removed:", name);
        }
    }
}
