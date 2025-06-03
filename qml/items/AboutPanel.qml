import QtQuick 2.6
import Sailfish.Silica 1.0

Rectangle {
    id: aboutPanel

    signal closed

    property color dialogColor: "#1c1127"

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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("About panel pressed.")
            aboutPanel.enabled = false
            aboutPanel.closed()
        }
    }

    Rectangle {
        id: msgBox

        radius: Theme.horizontalPageMargin
        width: parent.width - Theme.paddingLarge * 2
        height: Theme.horizontalPageMargin * 2 + Theme.paddingLarge * 4
                + msgButton.height + msgTextArea.height + msgCaptionArea.height
        anchors.centerIn: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.highlightBackgroundColor }
            GradientStop { position: Theme.paddingLarge * 3 / height; color: dialogColor }
            GradientStop { position: 1.0; color: dialogColor }
        }

        scale: parent.enabled ? 1.0 : 0.0
        Behavior on scale {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutBack
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {}
        }

        Column {
            spacing: Theme.paddingLarge * 2
            anchors.fill: parent
            anchors.margins: Theme.horizontalPageMargin

            Row {
                spacing: Theme.horizontalPageMargin
                anchors.left: parent.left
                anchors.right: parent.right

                Rectangle {
                    id: msgCaptionArea
                    radius: Theme.horizontalPageMargin / 2
                    height: Theme.horizontalPageMargin + msgCaptionText.height
                    width: parent.width - btnCloseArea.width - Theme.horizontalPageMargin
                    color: dialogColor

                    Text {
                        id: msgCaptionText
                        text: qsTr("#ColorTubes")
                        color: Theme.highlightColor
                        anchors.centerIn: parent
                        font.family: Theme.fontFamilyHeading
                        font.pixelSize: Theme.fontSizeMedium
                    }
                }

                Rectangle {
                    id: btnCloseArea
                    radius: Theme.horizontalPageMargin / 2
                    width: msgCaptionArea.height
                    height: msgCaptionArea.height
                    border.color: Theme.highlightBackgroundColor
                    color: dialogColor

                    Icon {
                        id: btnCloseIcon
                        source: "image://theme/icon-m-cancel"
                        anchors.centerIn: parent
                        opacity: enabled? 1.0 : Theme.opacityLow
                        color: Theme.lightPrimaryColor
                        highlightColor: Theme.highlightColor
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressedChanged: {
                            btnCloseIcon.highlighted = pressed;
                        }
                        onClicked: {
                            console.log("Close button pressed.")
                            aboutPanel.enabled = false
                            aboutPanel.closed()
                        }
                    }
                }
            }

            Rectangle {
                id: msgTextArea
                width: parent.width
                height: abText.height
                color: dialogColor

                Column {
                    id: abText
                    width: parent.width
                    spacing: Theme.paddingLarge

                    Text {
                        id: abText1
                        text: qsTr("#aboutText1")
                        wrapMode: Text.WordWrap
                        color: Theme.lightPrimaryColor
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                    }
                    Text {
                        id: abText2
                        text: qsTr("#aboutText2")
                        wrapMode: Text.WordWrap
                        color: Theme.rgba(Theme.lightPrimaryColor, 0.75)
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                        font.pointSize: Theme.fontSizeTinyBase
                    }
                    Text {
                        id: abText3
                        text: qsTr("#aboutText3")
                        wrapMode: Text.WordWrap
                        color: Theme.rgba(Theme.lightPrimaryColor, 0.75)
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                        font.pointSize: Theme.fontSizeTinyBase
                    }
                    Text {
                        id: abText4
                        text: qsTr("#aboutText4")
                        wrapMode: Text.WordWrap
                        color: Theme.rgba(Theme.lightPrimaryColor, 0.75)
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                        font.pointSize: Theme.fontSizeTinyBase
                    }
                }
            }

            Button {
                id: msgButton
                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                text: qsTr("Ok")
                color: Theme.darkPrimaryColor
                backgroundColor: Theme.highlightBackgroundColor
                onClicked: {
                    console.log("Message button pressed")
                    aboutPanel.enabled = false
                    aboutPanel.closed()
                }
            }
        }
    }
}
