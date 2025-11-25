/*
 * QML file tương ứng với example_qt_flow.cpp
 * 
 * File này minh họa cách QML sử dụng context properties
 * Khi đọc source code, hiểu cách QML engine resolve "appData"
 */

import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: "Qt Source Code Understanding Example"

    /*
     * ============================================
     * CONTEXT PROPERTY ACCESS
     * ============================================
     * 
     * Khi QML code truy cập "appData", engine sẽ:
     * 
     * 1. Tìm trong current component's context
     * 2. Nếu không có, tìm trong parent context
     * 3. Tiếp tục lên root context
     * 4. Tìm thấy "appData" trong root context
     * 
     * Trace trong source:
     * - QQmlContext::contextProperty()
     * - QQmlContextData::property()
     * - Property resolution chain
     */
    
    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            id: title
            text: "Understanding Qt Source Code"
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
        }

        /*
         * Đây là nơi QML truy cập context property
         * 
         * Trace: QQmlBinding::evaluate()
         *        -> QQmlContext::contextProperty("appData")
         *        -> Return QObject pointer
         *        -> Access property "message"
         */
        Text {
            id: messageText
            text: appData.message  // ← Context property access
            font.pixelSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
            
            /*
             * Binding này được update khi:
             * - appData.messageChanged() signal được emit
             * - QML binding system detect change
             * - Text.text được update
             * 
             * Trace: QQmlPropertyBinding::update()
             */
        }

        Button {
            text: "Change Message"
            anchors.horizontalCenter: parent.horizontalCenter
            
            /*
             * Khi click, gọi method trên context object
             * 
             * Trace: QQmlPropertyBinding::write()
             *        -> QObject::setProperty() hoặc direct method call
             */
            onClicked: {
                appData.setMessage("Message changed from QML!")
            }
        }
    }

    /*
     * ============================================
     * CONTEXT HIERARCHY EXAMPLE
     * ============================================
     * 
     * Nếu có nested components, mỗi component có context riêng:
     * 
     * Root Context (from QQmlApplicationEngine)
     *   └──> appData property
     * 
     * Component Context (for MyComponent)
     *   └──> parent = Root Context
     *         └──> Can access appData from parent
     */
    
    // Component {
    //     id: myComponent
    //     
    //     // Component có context riêng, nhưng có thể access parent context
    //     Text {
    //         text: appData.message  // Tìm trong parent context
    //     }
    // }
}
