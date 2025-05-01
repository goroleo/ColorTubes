import QtQuick 2.6
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import Game 1.0
import GameBoard 1.0
import "../items/"


Page {
    id: page
    allowedOrientations: Orientation.All
    property int questionNumber     // a.k.a. Question Number

    function showCongratsPanel() {
        blur.enabled = true
        congratsPanel.enabled = true
    }

    function showMessagePanel() {
        if (!board.busy) {
            board.hideSelection()
            blur.enabled = true
            messagePanel.enabled = true
            console.log("Message panel shows the question", questionNumber)
        } else {
            console.log("Game board is busy now. Cannot show the message.")
        }
    }

    function promptToStartAssistMode() {
        questionNumber = 5
        messagePanel.messageText = qsTr("#wantToStartAssistMode")
        messagePanel.buttonText = qsTr("Yes")
        showMessagePanel()
    }

    function solutionIsNotFound() {
        questionNumber = 6
        messagePanel.messageText = qsTr("#solutionIsNotFound")
        messagePanel.buttonText = qsTr("Ok")
        showMessagePanel()
    }

    function promptToEndAssistMode() {
        questionNumber = 7
        messagePanel.messageText = qsTr("#wantToEndAssistMode")
        messagePanel.buttonText = qsTr("Yes")
        showMessagePanel()
    }

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
        color: "#00000000"

        Row {
            id: topMenu
            anchors.horizontalCenter: parent.horizontalCenter
            height: btnStartAgain.height
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingLarge
            spacing: 100 // Theme.iconSizeMedium * 4 + 100 * 3 < width

            IconButtonItem {
                id: btnStartAgain
                source: "qrc:/img/icon-star.svg"
                enabled: Game.movesMade
                onClicked: {
                    console.log("[StartAgain] button clicked.")
                    questionNumber = 1
                    messagePanel.messageText = qsTr("#wantToStartAgain")
                    messagePanel.buttonText = qsTr("Yes")
                    showMessagePanel()
                }
            }

            IconButtonItem {
                id: btnUndoMove
                source: "qrc:/img/icon-undo.svg"
                enabled: Game.movesMade
                onClicked: {
                    console.log("[Undo] button clicked.")
                    if (!board.busy) {
                        board.hideSelection()
                        Game.undoMove()
                        Game.refreshNeeded();
                    } else {
                        console.log("Game board is busy now. Cannot undo the move.")
                    }
                }
            }

            IconButtonItem {
                id: btnSolve
                source: "qrc:/img/icon-dice.svg"
                enabled: !Game.isAssistMode
                onClicked: {
                    console.log("[Solve] button clicked.")
                    if (!board.busy) {
                        board.hideSelection()
                        Game.solve()
                    } else {
                        console.log("Game board is busy now. Cannot start solving process.")
                    }
                }
            }

            IconButtonItem {
                id: btnSetings
                source: "qrc:/img/icon-gear.svg"
                onClicked: {
                    console.log("[Settings] button clicked.")
                    board.hideSelection()
                    blur.enabled = true
                    aboutPanel.enabled = true
                }
            }
        }

        Text {
            id: levelNumber

            text: qsTr("#level") + " " + Game.level

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
            anchors.horizontalCenter: parent.horizontalCenter
            width: isLandscape ? parent.height : parent.width
            enabled: true
        }

        FastBlur {
            id: blur
            anchors.fill: board
            source: board
            cached: false
            enabled: false
            visible: enabled
            radius: enabled ? 64.0 : 0.0
            Behavior on radius {
                NumberAnimation {
                    duration: 20
                    easing.type: Easing.Linear
                }
            }
        }
    }

    CongratsPanel {
        id: congratsPanel
        onClicked: {
            if (!board.busy) {
                congratsPanel.enabled = false
                blur.enabled = false
                Game.newLevel()
            } else {
                console.log("Game board is busy now. Cannot close the panel.")
            }
        }
    }

    MessagePanel {
        id: messagePanel
        onAccepted: {
            blur.enabled = false
            console.log("Question", questionNumber, "accepted.")
            switch (questionNumber) {
            case 1: Game.startAgain(); break;      // Restart button
            case 5: Game.startAssistMode(); break; // Assist mode
            case 7: Game.endAssistMode(); break;   // Assist mode error
            default: break;
            }
            Game.refreshNeeded();
        }
        onRejected: {
            blur.enabled = false
            console.log("Question", questionNumber, "rejected")
            switch (questionNumber) {
            case 5: Game.endAssistMode(); break;
            default: break;
            }
            Game.refreshNeeded();
        }
    }

    AboutPanel {
        id: aboutPanel
        onClosed:
            blur.enabled = false;

    }

    Component.onCompleted: {
        Game.levelDone.connect(showCongratsPanel)
        Game.solverSuccess.connect(promptToStartAssistMode)
        Game.solverError.connect(solutionIsNotFound)
        Game.assistModeError.connect(promptToEndAssistMode)
    }
}
