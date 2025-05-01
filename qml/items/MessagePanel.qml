import QtQuick 2.6
import Sailfish.Silica 1.0

Rectangle {
    id: msgPanel

    signal accepted
    signal rejected

    property string caption: qsTr("#ColorTubes")
    property string messageText: qsTr("#someMessage")
    property string buttonText: qsTr("Ok")
    property color dialogColor: "#1c1127"
    readonly property int dialogWidth: Math.min(width, height)

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
            console.log("Message panel pressed. Answer is negative.")
            msgPanel.enabled = false
            msgPanel.rejected()
        }
    }

    Rectangle {
        id: msgBox

        radius: Theme.horizontalPageMargin
        width: dialogWidth - Theme.paddingLarge * 2
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
                        text: caption
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
                            console.log("Close button pressed. Answer is negative.")
                            msgPanel.enabled = false
                            msgPanel.rejected()
                        }
                    }
                }
            }

            Text {
                id: msgTextArea
                width: parent.width
                text: messageText
                color: Theme.lightPrimaryColor
                wrapMode: Text.WordWrap
            }

            Button {
                id: msgButton
                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                text: buttonText
                color: Theme.darkPrimaryColor
                backgroundColor: Theme.highlightBackgroundColor
                onClicked: {
                    console.log("Message button pressed. Answer is positive.")
                    msgPanel.enabled = false
                    msgPanel.accepted()
                }
            }
        }
    }
}
