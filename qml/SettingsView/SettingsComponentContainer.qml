import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Column {
    id: rootItem
    width: parent.width
    property alias text: textItem.text
    default property alias contents: contentItem.children

    RowLayout {
        width: parent.width

        Text {
            id: textItem
            text: "label"

            color: "#f0f0f0"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 100
        }

        ColumnLayout {
            id: contentItem
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
    }
}
