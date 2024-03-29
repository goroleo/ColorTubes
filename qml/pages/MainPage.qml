import QtQuick 2.0
import Sailfish.Silica 1.0
import GameBoard 1.0

Page {
    id: page
    allowedOrientations: Orientation.Portrait

    Image {
        id: backgroundImage

        source: "qrc:/img/bg.png"
        fillMode: Image.Tile
        width: parent.width
        height: parent.height
    }

    Row {
        id: topMenu
        width: parent.width - Theme.horizontalPageMargin * 2
        height: btnNewGame.height
        anchors.left: parent.left
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge

        spacing: (width - (Theme.iconSizeMedium * 4)) / 3

        Icon {
            id: btnNewGame
            source: "qrc:/img/icon-star.svg"
            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            opacity: enabled? 1.0 : Theme.opacityLow
            color: Theme.lightPrimaryColor
            highlightColor: Theme.highlightColor

            MouseArea {
                width: parent.width
                height: parent.height

                onPressedChanged: {
                    btnNewGame.highlighted = pressed;
                }
                onClicked: {
                    console.log("[NewGame] button pressed")
                }
            }
        }

        Icon {
            id: btnUndoMove
            source: "qrc:/img/icon-undo.svg"
            enabled: false

            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            opacity: enabled? 1.0 : Theme.opacityLow
            color: Theme.lightPrimaryColor
            highlightColor: Theme.highlightColor

            MouseArea {
                width: parent.width
                height: parent.height

                onPressedChanged: {
                    btnUndoMove.highlighted = pressed;
                }

                onClicked: {
                    console.log("[UndoMove] button clicked")
                }
            }
        }

        Icon {
            id: btnSolve
            source: "qrc:/img/icon-dice.svg"

            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            opacity: enabled? 1.0 : Theme.opacityLow
            color: Theme.lightPrimaryColor
            highlightColor: Theme.highlightColor

            MouseArea {
                width: parent.width
                height: parent.height

                onPressedChanged: {
                    btnSolve.highlighted = pressed;
                }

                onClicked: {
                    console.log("[Solve] button clicked")
                }
            }
        }

        Icon {
            id: btnSettings
            source: "qrc:/img/icon-gear.svg"

            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            opacity: enabled? 1.0 : Theme.opacityLow

            color: Theme.lightPrimaryColor
            highlightColor: Theme.highlightColor

            MouseArea {
                width: parent.width
                height: parent.height

                onPressedChanged: {
                    btnSettings.highlighted = pressed;
                }

                onClicked: {
                    console.log("[Settings] button clicked")
                    btnUndoMove.enabled = !btnUndoMove.enabled
                }
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

        MouseArea {
            width: parent.width
            height: parent.height
            onClicked: {
                console.log("fontSize "
                            + "Large " + Theme.fontSizeLarge + ", "
                            + "ExtraLarge " + Theme.fontSizeExtraLarge + ", "
                            + "Huge " + Theme.fontSizeHuge + " "
                            )
                console.log("pixel size: " + levelNumber.font.pixelSize + ", point size: " + levelNumber.font.pointSize)
            }
        }
    }

    GameBoard {
        id: board
        anchors.top: levelNumber.bottom
        anchors.bottom: parent.bottom
        width: parent.width
    }

}
