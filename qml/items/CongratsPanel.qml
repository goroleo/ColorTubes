import QtQuick 2.6
import Sailfish.Silica 1.0
import FlowerLayer 1.0

Rectangle {
    id: congratsPanel
    signal clicked

    width: parent.width
    height: parent.height
    color: Theme.rgba(Theme.highlightBackgroundColor, 0.2)
    enabled: false
    opacity: enabled ? 1.0 : 0.0
    Behavior on opacity {
        NumberAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    FlowerLayer {
        id: congratsFlower
        anchors.fill: parent
        opacity: 0.5
    }

    Rectangle {
        id: congratsText
        color: "#00000000"
        width: parent.width
        height: ( width / badge.sourceSize.width ) * badge.sourceSize.height
        anchors.centerIn: parent

        scale: parent.enabled ? 1.0 : 0.0
        Behavior on scale {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutBack
            }
        }

        Image {
            id: badge
            source: "qrc:/img/badge.svg"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }

        Text {
            text: qsTr("#victory")

            width: parent.width * 0.5
            height: parent.height * 0.5
            anchors.centerIn: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font.capitalization: Font.AllUppercase
            font.weight: Font.DemiBold
            font.pixelSize: parent.height * 0.3

            color: "#fffa60"
            clip: true
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            congratsPanel.enabled = false
            congratsPanel.clicked()
        }
    }
}

