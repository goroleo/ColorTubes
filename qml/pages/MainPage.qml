import QtQuick 2.6
import Sailfish.Silica 1.0
import GameBoard 1.0
import FlowerLayer 1.0
import "../items/"


Page {
    id: page
    allowedOrientations: Orientation.Portrait
    property int questionNumber

    Image {
        id: backgroundImage
        source: "qrc:/img/bg.png"
        fillMode: Image.Tile
        width: parent.width
        height: parent.height
    }

    Rectangle {
        id: gamePanel
        width: parent.width
        height: parent.height
        opacity: enabled ? 1.0 : 0.0
        color: "#00000000"

        Row {
            id: topMenu
            width: parent.width - Theme.horizontalPageMargin * 2
            height: btnNewGame.height
            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingLarge
            spacing: (width - (Theme.iconSizeMedium * 4)) / 3

            IconButtonItem {
                id: btnNewGame
                source: "qrc:/img/icon-star.svg"
                enabled: board.hasMoves
                onClicked: {
                    console.log("[NewGame] button pressed")
                    questionNumber = 1
                    messagePanel.messageText
                            = qsTr("Do you want to start this level again?")
                    messagePanel.buttonText
                            = qsTr("Yes")
                    messagePanel.enabled = true
                }
            }

            IconButtonItem {
                id: btnUndoMove
                source: "qrc:/img/icon-undo.svg"
                enabled: board.hasMoves
                onClicked: {
                    questionNumber = 2
                    board.undoMove()
                }
            }

            IconButtonItem {
                id: btnSolve
                source: "qrc:/img/icon-dice.svg"
                enabled: true
                onClicked: {
                    questionNumber = 3
                    console.log("[Solve] button clicked")
                    messagePanel.messageText
                            = qsTr("I will try to find a solution to this game, but it can take some time.\nDo you want to get started?")
                    messagePanel.buttonText
                            = qsTr("Unavailable now")
                    messagePanel.enabled = true
                }
            }

            IconButtonItem {
                id: btnSettings
                source: "qrc:/img/icon-gear.svg"
                onClicked: {
                    questionNumber = 4
                    messagePanel.messageText
                            = qsTr("Application settings will be here")
                    messagePanel.buttonText
                            = qsTr("I see")
                    messagePanel.enabled = true
                    console.log("[Settings] button clicked")
                }
            }
        }

        Text {
            id: levelNumber

            text: qsTr("Level") + " " + "145"

            width: parent.width
            anchors.top: topMenu.bottom
            anchors.topMargin: Theme.paddingLarge

            verticalAlignment: Text.AlignBottom
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: Theme.fontSizeLarge

            font.family: Theme.fontFamilyHeading
            color: Theme.highlightColor
        }

        GameBoard {
            id: board
            anchors.top: levelNumber.bottom
            anchors.bottom: parent.bottom
            width: parent.width
            enabled: true
            opacity: enabled ? 1.0 : 0.0
            Behavior on opacity { FadeAnimator {} }
            onSolved: {
                congratsPanel.enabled = true
            }
        }
    }

    CongratsPanel {
        id: congratsPanel
    }

    MessagePanel {
        id: messagePanel
        onAccepted: {
            console.log("Accepted.", "question=", questionNumber)
            switch (questionNumber) {
            case 1: board.startAgain(); return;
            case 2: board.undoMove(); return;
            case 3:
            case 4:
            }

        }
        onRejected: {
            console.log("Rejected")
        }
    }

}
