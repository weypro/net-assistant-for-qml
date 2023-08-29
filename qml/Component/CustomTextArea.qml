import QtQuick 2.15
import QtQuick.Controls.Universal

TextArea {
    id: rootItem

    color: "white"

    background: Rectangle {
        implicitWidth: 60
        implicitHeight: 28
        border.width: 2
        color: !rootItem.enabled ? rootItem.Universal.baseLowColor : "#17171a"
        border.color: !rootItem.enabled ? rootItem.Universal.baseLowColor : rootItem.activeFocus ? globalColor.primaryColor : rootItem.hovered ? globalColor.borderHoverdColor : globalColor.borderColor
    }
}
