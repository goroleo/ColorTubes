import QtQuick 2.6
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import Game 1.0
import GameBoard 1.0
import "../items/"

Page {
    id: page
    allowedOrientations: Game.isSystemOrientation
                         ? Orientation.All : (Game.isPortraitOrientation
                             ? Orientation.PortraitMask : Orientation.LandscapeMask)

    property int questionNumber
    property bool isSystemLightTheme: Theme.colorScheme == Theme.DarkOnLight

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

    function promptToEndAssistMode() {
        questionNumber = 7
        messagePanel.messageText = qsTr("#wantToEndAssistMode")
        messagePanel.buttonText = qsTr("Yes")
        showMessagePanel()
    }

    function solutionIsNotFound() {
        questionNumber = 6
        messagePanel.messageText = qsTr("#solutionIsNotFound")
        messagePanel.buttonText = qsTr("Ok")
        showMessagePanel()
    }

    function themeChanged() {
        isSystemLightTheme = Theme.colorScheme == Theme.DarkOnLight
        Game.isLightTheme = isSystemLightTheme
    }

    function startAgainButtonClick() {
        console.log("[StartAgain] button clicked.")
        questionNumber = 1
        messagePanel.messageText = qsTr("#wantToStartAgain")
        messagePanel.buttonText = qsTr("Yes")
        showMessagePanel()
    }

    function undoMoveButtonClick() {
        console.log("[Undo] button clicked.")
        if (!board.busy) {
            board.hideSelection()
            if (Game.isUndoUntilSolvable) {
                Game.undoUntilSolvable();
                console.log("Undoes until solvable")
            } else {
                Game.undoMove();
                console.log("Undoes one move")
            }
            Game.refreshNeeded();
        } else {
            console.log("Game board is busy now. Cannot undo the move.")
        }
    }

    function solveGameButtonClick() {
        console.log("[Solve] button clicked.")
        if (!board.busy) {
            board.hideSelection()
            Game.solve()
        } else {
            console.log("Game board is busy now. Cannot start solving process.")
        }
    }

    function settingsButtonClick() {
        console.log("[Settings] button clicked.")
        board.hideSelection()
        blur.enabled = true
        aboutPanel.enabled = true
    }

    Image {
        id: backgroundImage
        source: Game.isSystemTheme? Theme.backgroundImage
                : (Game.isLightTheme? "qrc:/img/bg-light.png" : "qrc:/img/bg-dark.png")
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
            id: horizontalToolBar

            width: parent.width - Theme.horizontalPageMargin * 2
            height: Theme.iconSizeMedium
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.topMargin: Theme.paddingLarge
            spacing: (width - (Theme.iconSizeMedium * 4)) / 3
            visible: page.isPortrait

            IconButtonItem {
                source: "qrc:/img/icon-star.svg"
                enabled: Game.movesMade && !(Game.level == 1)
                onClicked: startAgainButtonClick()
            }

            IconButtonItem {
                source: "qrc:/img/icon-undo.svg"
                enabled: Game.movesMade
                onClicked: undoMoveButtonClick()
            }

            IconButtonItem {
                source: "qrc:/img/icon-dice.svg"
                enabled: !Game.isAssistMode
                onClicked: solveGameButtonClick()
            }

            IconButtonItem {
                id: btnSetings
                source: "qrc:/img/icon-gear.svg"
                enabled: ! (Game.level == 1)
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        Column {
            id: verticalToolBar

            width: Theme.iconSizeMedium + Theme.paddingLarge
            height: gamePanel.height - Theme.horizontalPageMargin * 2
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.topMargin: Theme.paddingLarge
            spacing: (height - (Theme.iconSizeMedium * 4)) / 3
            visible: page.isLandscape

            IconButtonItem {
                source: "qrc:/img/icon-star.svg"
                enabled: Game.movesMade && !(Game.level == 1)
                onClicked: startAgainButtonClick()
            }

            IconButtonItem {
                source: "qrc:/img/icon-undo.svg"
                enabled: Game.movesMade
                onClicked: undoMoveButtonClick()
            }

            IconButtonItem {
                source: "qrc:/img/icon-dice.svg"
                enabled: !Game.isAssistMode
                onClicked: solveGameButtonClick()
            }

            IconButtonItem {
                source: "qrc:/img/icon-gear.svg"
                enabled: ! (Game.level == 1)
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        Text {
            id: levelNumber
            text: qsTr("#level") + " " + Game.level
            anchors {
                top: page.isPortrait ? horizontalToolBar.bottom: gamePanel.top
                topMargin: Theme.paddingLarge
                left: page.isLandscape ? verticalToolBar.right : gamePanel.left
                leftMargin: Theme.paddingLarge
                right: gamePanel.right
                rightMargin: Theme.paddingLarge
            }

            verticalAlignment: Text.AlignBottom
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: Theme.fontSizeLarge

            font.family: Theme.fontFamilyHeading
            color: Theme.highlightColor
        }

        GameBoard {
            id: board
            anchors {
                top: page.isPortrait ? levelNumber.bottom : levelNumber.top
                left: page.isLandscape ? verticalToolBar.right : gamePanel.left
                bottom: gamePanel.bottom
                right: gamePanel.right
            }
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
        Game.isLightTheme = isSystemLightTheme
        Game.levelDone.connect(showCongratsPanel)
        Game.solverSuccess.connect(promptToStartAssistMode)
        Game.solverError.connect(solutionIsNotFound)
        Game.assistModeError.connect(promptToEndAssistMode)
        Theme.homeBackgroundImageChanged.connect(themeChanged)
    }
}
