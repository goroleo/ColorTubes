import QtQuick 2.6
import Sailfish.Silica 1.0
import Game 1.0
import GameBoard 1.0
import FlowerLayer 1.0
import "../items/"


Page {
    id: page
    allowedOrientations: Orientation.Portrait
    property int questionNumber
    property int level: Game.level
    property bool hasMoves: Game.hasMoves

    function showCongrats() {congratsPanel.enabled = true}

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
                enabled: hasMoves
                onClicked: {
                    if (!board.busy) {
                        board.hideSelection()
                        console.log("[NewGame] button pressed")
                        questionNumber = 1
                        messagePanel.messageText = qsTr("#wantToStartAgain")
                        messagePanel.buttonText = qsTr("Yes")
                        messagePanel.enabled = true
                    }
                }
            }

            IconButtonItem {
                id: btnUndoMove
                source: "qrc:/img/icon-undo.svg"
                enabled: hasMoves
                onClicked: {
                    if (!board.busy) {
                        board.hideSelection()
                        questionNumber = 2
                        Game.undoMove()
                    }
                }
            }

            IconButtonItem {
                id: btnSolve
                source: "qrc:/img/icon-dice.svg"
                enabled: true
                onClicked: {
                    if (!board.busy) {
                        board.hideSelection()
                        questionNumber = 3
                        console.log("[Solve] button clicked")
                        messagePanel.messageText = qsTr("#wantToSolve")
                        messagePanel.buttonText = qsTr("#unavailable")
                        messagePanel.enabled = true
                    }
                }
            }

            IconButtonItem {
                id: btnSettings
                source: "qrc:/img/icon-gear.svg"
                onClicked: {
                    board.hideSelection()
                    questionNumber = 4
                    messagePanel.messageText = qsTr("#settings")
                    messagePanel.buttonText = qsTr("I see")
                    messagePanel.enabled = true
                    console.log("[Settings] button clicked")
                }
            }
        }

        Text {
            id: levelNumber

            text: qsTr("#level") + " " + level

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
            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutBack
                }
            }
        }
    }

    CongratsPanel {
        id: congratsPanel
        onClicked: {
            Game.newLevel()
        }
    }

    MessagePanel {
        id: messagePanel
        onAccepted: {
            console.log("Question", questionNumber, "accepted.")
            switch (questionNumber) {
            case 1: Game.startAgain(); return;
            case 2: return;
            case 3: Game.solve(); return;
            case 4: return;
            }
        }
        onRejected: {
            console.log("Rejected")
        }
    }

    Component.onCompleted: {
        Game.solved.connect(showCongrats)
    }
}
