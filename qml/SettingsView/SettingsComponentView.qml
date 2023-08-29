// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtQuick.Layouts

Column {
    id: rootItem

    property alias text: textItem.text
    property bool show: true

    default property alias contents: contentItem.children
    property real showHideAnimationSpeed: 200

    width: parent.width

    Component.onCompleted: {
        // Set initial open state
        contentItem.visible = rootItem.show
        contentItem.opacity = rootItem.show
        contentItemArea.height = rootItem.show ? contentItem.height : 0
    }

    Item {
        id: lightsSettings
        width: parent.width
        height: 30

        Rectangle {
            anchors.fill: parent
            color: "#3A3C3F"
            opacity: 0.4
        }

        Image {
            x: 8
            source: "/images/arrow-right.svg"
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width: 20
            fillMode: Image.PreserveAspectFit
            rotation: rootItem.show ? 90 : 0
            Behavior on rotation {
                NumberAnimation {
                    duration: showHideAnimationSpeed
                    easing.type: Easing.InOutCubic
                }
            }
        }

        Text {
            id: textItem
            x: 30
            anchors.verticalCenter: parent.verticalCenter
            color: "#f0f0f0"
            font.bold: true
            font.pixelSize: 16
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                rootItem.show = !rootItem.show
                if (rootItem.show) {
                    hideAnimation.stop()
                    showAnimation.start()
                } else {
                    showAnimation.stop()
                    hideAnimation.start()
                }
            }
        }
    }

    Item {
        width: 1
        height: 15
    }

    SequentialAnimation {
        id: showAnimation
        ScriptAction {
            script: contentItem.visible = true
        }
        ParallelAnimation {
            NumberAnimation {
                target: contentItemArea
                property: "height"
                to: contentItem.height
                duration: showHideAnimationSpeed
                easing.type: Easing.InOutQuad
            }
            SequentialAnimation {
                PauseAnimation {
                    duration: showHideAnimationSpeed / 2
                }
                NumberAnimation {
                    target: contentItem
                    property: "opacity"
                    to: 1.0
                    duration: showHideAnimationSpeed / 2
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    SequentialAnimation {
        id: hideAnimation
        ParallelAnimation {
            NumberAnimation {
                target: contentItemArea
                property: "height"
                to: 0
                duration: showHideAnimationSpeed
                easing.type: Easing.InOutQuad
            }
            SequentialAnimation {
                NumberAnimation {
                    target: contentItem
                    property: "opacity"
                    to: 0
                    duration: showHideAnimationSpeed / 2
                    easing.type: Easing.InOutQuad
                }
                PauseAnimation {
                    duration: showHideAnimationSpeed / 2
                }
            }
        }
        ScriptAction {
            script: contentItem.visible = false
        }
    }

    Item {
        id: contentItemArea
        width: parent.width

        Column {
            id: contentItem
            spacing: 8
            width: parent.width - this.leftPadding - this.rightPadding
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 15
        }
    }
}
