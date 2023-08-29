import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import module.net
import "../Component"

Item {
    id: rootItem

    property bool show: true
    property real showAnimation: show ? 1 : 0
    property NetConn netConn

    function resetSettings() {}

    width: parent.width

    Behavior on showAnimation {
        NumberAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    // Background
    Rectangle {
        anchors.fill: scrollView
        opacity: showAnimation ? 0.6 : 0
        visible: opacity
        color: globalColor.bgColorLighter
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        contentWidth: parent.width // 如果没有该属性则子元素都没有width
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.interactive: true
        clip: true

        ScrollBar.vertical.width: globalColor.scrollBarWidth

        Column {
            id: settingsArea
            anchors.fill: parent
            opacity: showAnimation
            visible: opacity

            Item {
                width: 1
                height: 20
            }

            SettingsComponentView {
                id: settingsViewGeneral
                text: qsTr("General")
                show: true

                SettingsComponentComboBox {
                    text: "Protocol"

                    comboBox {
                        id: connTypeComboBox
                        model: netConn.connTypeStrList()
                        onActivated: {
                            netConn.setConnType(connTypeComboBox.currentIndex)
                        }
                    }
                }

                SettingsComponentText {
                    text: "IP"

                    textArea {
                        text: netConn.serverAddress()

                        onEditingFinished: {
                            netConn.setServerAddress(textArea.text)
                        }
                    }
                }

                SettingsComponentText {
                    text: "Port"

                    textArea {
                        id: portInput
                        text: netConn.port()

                        onEditingFinished: {
                            netConn.setServerPort(textArea.text)
                        }
                    }
                }

                Button {
                    id: connectBtn
                    text: "Connect"
                    width: parent.width
                    property color defaultColor: connectBtn.background.color
                    onClicked: {
                        netConn.connectBtnClicked()
                    }

                    function getColor(state) {
                        switch (state) {
                        case ConnEnum.Connected:
                            return globalColor.primaryColor
                        default:
                            return "#4B4C4E"
                        }
                    }
                }
            }

            SettingsComponentView {
                text: qsTr("Receive")
                show: true

                SettingsComponentCheckBox {
                    text: "Receive Enable"
                    checked: netConn.receiveEnableState()
                    width: parent.width
                    onToggled: {
                        netConn.setReceiveEnabledState(checked)
                    }
                }

                SettingsComponentCheckBox {
                    text: "Show timestamp"
                    width: parent.width
                    checked: netConn.timeStampEnableState()
                    onToggled: {
                        netConn.setTimeStampEnableState(checked)
                    }
                }

                Button {
                    id: clearRecvBtn
                    text: "Clear"
                    width: parent.width

                    onClicked: {
                        historyArea.text = ""
                    }
                }
            }
            SettingsComponentView {
                text: qsTr("Send")
                show: true

                Button {
                    id: clearSendBtn
                    text: "Clear"
                    width: parent.width
                    onClicked: {
                        messageArea.text = ""
                    }
                }
            }
        }
    }

    // 统一处理状态改变
    Connections {
        target: netConn
        function onStateChanged(state) {
            //            console.log("onStateChanged", state)
            connectBtn.background.color = connectBtn.getColor(state)

            // 在未连接状态下点击连接，会触发连接动作，同时按钮无法使用，直到状态更新
            if (state === ConnEnum.Connecting)
                connectBtn.enabled = false
            else
                connectBtn.enabled = true
        }
    }
}
