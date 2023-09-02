import QtQuick
import QtQuick.Window
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs

import module.net
import "./qml"
import "./qml/SettingsView"
import "./qml/Component"

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: qsTr("Net Assistant With QML")

    Universal.theme: Universal.Dark
    Universal.accent: globalColor.primaryColor
    Universal.foreground: "#000000"

    GlobalColor {
        id: globalColor
    }

    NetConn {
        id: globalNetConn
        Component.onCompleted: {
            globalNetConn.init()
        }
    }

    // 菜单栏
    menuBar: MenuBar {
        Menu {
            title: qsTr("&Help")
            Action {
                id: aboutAction
                text: qsTr("&About")
                onTriggered: {
                    console.info("点击了About")
                }
            }
        }

        background: Rectangle {
            anchors.fill: parent
            color: globalColor.bgColorLighter
        }
    }

    Rectangle {
        id: mainWindowContent
        anchors.fill: parent

        color: "#1E1F22"

        Settings {
            id: settings
            onReseted: {
                settingsView.resetSettings()
            }
        }

        Settings {
            id: defaultSettings
        }

        // 用split隔开，左设置，右主区域
        SplitView {
            id: splitView
            anchors.fill: parent
            orientation: Qt.Horizontal

            // 自定义分隔栏，修改宽度和颜色
            handle: Rectangle {
                id: handleDelegate
                implicitWidth: 2
                color: SplitHandle.pressed ? "#434343" : (SplitHandle.hovered ? Qt.lighter("#525252", 1.1) : "#525252")
            }

            //设置
            ColumnLayout {
                SplitView.preferredWidth: 300
                SplitView.minimumWidth: 100
                SplitView.fillHeight: true

                SettingsView {
                    id: settingsView
                    Layout.fillWidth: true // 填充宽度
                    Layout.fillHeight: true // 填充高度
                    //            anchors.margins: 20
                    visible: settings.showSettingsView
                    netConn: globalNetConn
                    Component.onCompleted: {
                        settings.reset()
                    }
                }
            }

            // 主区域，包括历史消息框、发送文本框、发送按钮
            Item {
                id: mainArea
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                ColumnLayout {
                    anchors.fill: parent // 填充父元素的大小

                    // 历史消息文本框
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: 0.8 * parent.height // 首选高度占80%
                        ScrollBar.vertical.width: globalColor.scrollBarWidth
                        CustomTextArea {
                            id: historyArea
                            readOnly: true // 只读模式，不可编辑
                            wrapMode: TextArea.WrapAnywhere
                            cursorPosition: text.length
                        }
                    }
                    // 需要发送的消息文本框和发送按钮
                    RowLayout {
                        id: inputSendArea
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: 0.2 * parent.height // 首选高度占20%
                        Layout.bottomMargin: 5 // 下外边距5个像素

                        ScrollView {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            ScrollBar.vertical.width: globalColor.scrollBarWidth
                            CustomTextArea {
                                id: messageArea
                                text: "Network test message"
                                placeholderText: "Input the message"
                                wrapMode: TextArea.WrapAnywhere
                            }
                        }

                        Button {
                            id: sendButton
                            text: "Send"
                            Layout.minimumWidth: 100
                            Layout.preferredWidth: 100
                            Layout.maximumWidth: 100
                            Layout.fillHeight: true
                            width: 150

                            onClicked: {
                                var message = messageArea.text
                                if (message.length > 0) {
                                    globalNetConn.sendMessage(messageArea.text)
                                    //                                    messageArea.text = "Network test message"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 状态栏
    footer: ToolBar {

        height: 32
        RowLayout {
            anchors.fill: parent
            Item {
                id: fillitem
                Layout.fillWidth: true
            }
            RowLayout {
                spacing: 10
                RowLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    //接收计数 单位：字节
                    RowLayout {
                        Layout.preferredWidth: 50
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Label {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter // 水平居左垂直居中对齐
                            text: "RX:"
                        }

                        Label {
                            id: receivedCountLabel
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter // 水平居左垂直居中对齐
                            text: "0"
                        }
                    }

                    //发送计数  单位：字节
                    RowLayout {
                        Layout.preferredWidth: 50
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Label {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter // 水平居左垂直居中对齐
                            text: "TX:"
                        }
                        Label {
                            id: sendCountLabel
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter // 水平居左垂直居中对齐
                            text: "0"
                        }
                    }
                }

                Button {
                    id: refreshCountButton
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter // 水平居左垂直居中对齐
                    text: "Rst"

                    onClicked: {
                        globalNetConn.resetCount()
                    }
                }

                Button {
                    id: countButton
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter // 水平居左垂直居中对齐
                    Layout.rightMargin: 10
                    text: "Cnt"
                    checkable: true
                    checked: true

                    onClicked: {
                        globalNetConn.setCountEnabledState(checked)
                    }
                }
            }
        }

        background: Rectangle {
            anchors.fill: parent
            color: globalColor.bgColorLighter
        }
    }

    // 错误提示框
    MessageDialog {
        id: messageDialog
        title: "error"
        text: ""
        buttons: MessageDialog.Ok
        Component.onCompleted: visible = false
    }

    // 统一处理状态改变
    Connections {
        target: globalNetConn
        function onMessageChanged(message) {
            //            console.log(message)
            historyArea.text += message
        }

        function onStatisticsChanged(recvCountStr, sendCountStr) {
            //            console.log("r", recvCountStr, "s", sendCountStr)
            receivedCountLabel.text = recvCountStr
            sendCountLabel.text = sendCountStr
        }

        function onErrorOccurred(message) {
            messageDialog.text = message
            messageDialog.open()
        }
    }
}
