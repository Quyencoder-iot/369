import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: "Qt Source Code Learning - " + (myData ? myData.name : "Loading...")

    // Access C++ object exposed via QQmlContext
    property var myData: null

    Component.onCompleted: {
        console.log("QML Component completed");
        console.log("myData object:", myData);
        console.log("myData.name:", myData ? myData.name : "null");
        console.log("myData.value:", myData ? myData.value : "null");
        console.log("APP_VERSION:", APP_VERSION);
    }

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                id: title
                text: "Qt Source Code Learning"
                font.pixelSize: 24
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                width: 400
                height: 200
                color: "white"
                border.color: "#ccc"
                border.width: 1
                radius: 5

                Column {
                    anchors.centerIn: parent
                    spacing: 15

                    Text {
                        text: "C++ Object Data (from QQmlContext):"
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: "Name: " + (myData ? myData.name : "N/A")
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: "Value: " + (myData ? myData.value : "N/A")
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    // Button to update C++ object
                    Button {
                        text: "Update Value"
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: {
                            if (myData) {
                                myData.setValue(myData.value + 1);
                                console.log("Value updated to:", myData.value);
                            }
                        }
                    }
                }
            }

            Text {
                text: "APP_VERSION: " + APP_VERSION
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 14
                color: "#666"
            }

            Text {
                text: "Platform: " + (app ? app.platformName() : "N/A")
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 12
                color: "#999"
            }
        }
    }

    // Monitor property changes
    Connections {
        target: myData
        function onValueChanged() {
            console.log("QML: Value changed to", myData.value);
        }
        function onNameChanged() {
            console.log("QML: Name changed to", myData.name);
        }
    }
}
