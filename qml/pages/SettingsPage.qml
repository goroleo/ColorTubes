import QtQuick 2.6
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import Game 1.0
import "../items/"

Page {
    id: page
    property int questionNumber
    property bool lightTheme: Theme.colorScheme == Theme.DarkOnLight


    property string imageSource: Game.isSystemTheme? Theme.backgroundImage
                : (Game.isLightTheme? "qrc:/img/bg-light.png" : "qrc:/img/bg-dark.png")

    property color textColor: Game.isLightTheme? Theme.darkPrimaryColor : Theme.lightPrimaryColor

    function changeOrientation (mode) {
        Game.setOrientation(mode)
        page.allowedOrientations = Game.isSystemOrientation
            ? Orientation.All : (Game.isPortraitOrientation
                ? Orientation.PortraitMask : Orientation.LandscapeMask)
    }

    function changeTheme (mode) {
        Game.setTheme(mode)
        imageSource = Game.isSystemTheme? Theme.backgroundImage
                     : (Game.isLightTheme? "qrc:/img/bg-light.png" : "qrc:/img/bg-dark.png")
        textColor = Game.isLightTheme? Theme.darkPrimaryColor : Theme.lightPrimaryColor
    }
    
    function changeUndoMode (mode) {
        Game.setUndoMode(mode)
    }

    function resetGamePlayClick() {
        console.log("[ResetGamePlay] clicked.")
        questionNumber = 1
        messagePanel.messageText = qsTr("#wantToResetGamePlay")
        messagePanel.buttonText = qsTr("Yes")
        messagePanel.enabled = true
        console.log("Message panel shows the question", questionNumber)
    }

    function themeChanged() {
        lightTheme = Theme.colorScheme == Theme.DarkOnLight
        Game.isLightTheme = lightTheme
    }
    
    allowedOrientations: Game.isSystemOrientation
                         ? Orientation.All : (Game.isPortraitOrientation
                             ? Orientation.PortraitMask : Orientation.LandscapeMask)

    Image {
        id: backgroundImage
        source: imageSource
        fillMode: Image.Tile
        width: parent.width
        height: parent.height
    }

    SilicaFlickable {
        id: optionsPanel
        anchors.fill: parent
        contentHeight: contentColumn.height

        VerticalScrollDecorator { }

        Column {
            id: contentColumn
            width: parent.width

            PageHeader {
                id: pageHeader
                title: qsTr("Game settings")
            }

// -------- orientation

            SectionHeader {
                text: qsTr("Screen orientation")
                horizontalAlignment: Text.AlignLeft
            }

            TextSwitch {
                text: qsTr("System")
                _label.color: textColor
                automaticCheck: false
                checked: Game.isSystemOrientation
                onClicked: changeOrientation(0)
            }

            TextSwitch {
                text: qsTr("Portrait")
                _label.color: textColor
                automaticCheck: false
                checked: !Game.isSystemOrientation && Game.isPortraitOrientation
                onClicked: changeOrientation(1)
            }

            TextSwitch {
                text: qsTr("Landscape")
                _label.color: textColor
                height: Theme.itemSizeExtraSmall
                automaticCheck: false
                checked: !Game.isSystemOrientation && !Game.isPortraitOrientation
                onClicked: changeOrientation(2)
            }

// -------- appearance

            SectionHeader {
                text: qsTr("Appearance")
                horizontalAlignment: Text.AlignLeft
            }

            TextSwitch {
                text: qsTr("System")
                _label.color: textColor
                automaticCheck: false
                checked: Game.isSystemTheme
                onClicked: changeTheme(0)
            }

            TextSwitch {
                text: qsTr("Dark")
                _label.color: textColor
                automaticCheck: false
                checked: !Game.isSystemTheme && !Game.isLightTheme
                onClicked: changeTheme(1)
            }

            TextSwitch {
                text: qsTr("Light")
                _label.color: textColor
                automaticCheck: false
                checked: !Game.isSystemTheme && Game.isLightTheme
                onClicked: changeTheme(2)
            }

// -------- Undo button

            SectionHeader {
                text: qsTr("Undo button")
                horizontalAlignment: Text.AlignLeft
            }

            TextSwitch {
                text: qsTr("#oneMove")
                _label.color: textColor
                automaticCheck: false
                checked: !Game.isUndoUntilSolvable
                onClicked: changeUndoMode(0)
            }

            TextSwitch {
                text: qsTr("#untilSolve")
                _label.color: textColor
                automaticCheck: false
                checked: Game.isUndoUntilSolvable
                onClicked: changeUndoMode(1)
            }

// ------------ others

            SectionHeader {
                text: qsTr("Others")
                horizontalAlignment: Text.AlignLeft
            }

            TextField {
                readOnly: true
                color: textColor
                text: qsTr("Reset gameplay...")
                onClicked: resetGamePlayClick()
            }

            TextField {
                readOnly: true
                color: textColor
                text: qsTr("About...")
                onClicked: aboutPanel.enabled = true
            }

        } // column

    } // flickable

    MessagePanel {
        id: messagePanel
        onAccepted: {
            console.log("Question accepted.")
            switch (questionNumber) {
            case 1: Game.resetGameplay(); pageStack.pop(); break;      // Reset Gameplay
            default: break;
            }
        }
        onRejected: {
            console.log("Question rejected")
        }
    }

    AboutPanel {
        id: aboutPanel
    }

    Component.onCompleted: {
        Game.isLightTheme = lightTheme
        Theme.homeBackgroundImageChanged.connect(themeChanged)
    }
}
