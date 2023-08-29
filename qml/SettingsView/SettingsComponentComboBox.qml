import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Column {
    id: rootItem

    property alias text: container.text
    property alias comboBox: comboBox

    signal toggled

    width: parent.width

    SettingsComponentContainer {
        id: container
        text: "label"

        ComboBox {
            id: comboBox
            Layout.fillWidth: true
        }
    }
}
