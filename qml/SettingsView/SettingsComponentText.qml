import QtQuick
//import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Universal

Column {
    id: rootItem

    property alias text: container.text
    property alias textArea: textArea

    width: parent.width

    SettingsComponentContainer {
        id: container
        text: "label"

        TextField {
            id: textArea
            text: ""
            Layout.fillWidth: true

            color: "white"

            background: Rectangle {
                implicitWidth: 60
                implicitHeight: 28
                border.width: 2
                color: !textArea.enabled ? textArea.Universal.baseLowColor : "#17171a"
                border.color: !textArea.enabled ? textArea.Universal.baseLowColor : textArea.activeFocus ? globalColor.primaryColor : textArea.hovered ? globalColor.borderHoverdColor : globalColor.borderColor
            }
        }
    }
}
