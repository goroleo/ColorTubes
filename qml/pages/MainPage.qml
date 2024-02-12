import QtQuick 2.0
import Sailfish.Silica 1.0
import TubeItem 1.0

Page {
    id: page
    allowedOrientations: Orientation.Portrait

    Image {
        id: backgroundImage

        source: "qrc:/img/bg-pattern.png"
        fillMode: Image.Tile
        width: parent.width
        height: parent.height
    }

    SilicaFlickable {
        id: mySilica

        contentWidth: page.width - Theme.horizontalPageMargin*2;
        contentHeight: page.height
        anchors.fill: parent
        topMargin: Theme.paddingLarge
        leftMargin: Theme.horizontalPageMargin
        rightMargin: Theme.horizontalPageMargin

        PullDownMenu {
            id: pulldown

            MenuItem {

                Row {
                    id: topMenu

                    spacing: (parent.width - (Theme.iconSizeMedium * 4)) / 3

                    IconButton {
                        id: btnNewGame

                        height: Theme.iconSizeMedium
                        width: Theme.iconSizeMedium
                        icon.scale: Theme.iconSizeMedium / icon.height

                        icon.source: "qrc:/img/icon-star.svg?"
                                     + (pressed
                                        ? Theme.lightHighlightColor
                                        : Theme.lightPrimaryColor)
                        onClicked: {
                            console.log("[NewGameButton] pressed")
                            pulldown.close(false)
                        }
                    }

                    IconButton {
                        id: btnUndoMove

                        height: Theme.iconSizeMedium
                        width: Theme.iconSizeMedium
                        icon.scale: Theme.iconSizeMedium / icon.height

                        icon.source: "qrc:/img/icon-undo.svg?"
                                     + (pressed
                                        ? Theme.lightHighlightColor
                                        : Theme.lightPrimaryColor)
                        onClicked: {
                            console.log("[UndoButton] pressed")
                            pulldown.close(false)
                        }
                    }

                    IconButton {
                        id: btnSolve

                        height: Theme.iconSizeMedium
                        width: Theme.iconSizeMedium
                        icon.scale: Theme.iconSizeMedium / icon.height

                        icon.source: "qrc:/img/icon-dice.svg?"
                                     + (pressed
                                        ? Theme.lightHighlightColor
                                        : Theme.lightPrimaryColor)
                        onClicked: {
                            console.log("[SolveButton] pressed")
                            pulldown.close(false)
                        }
                    }

                    IconButton {
                        id: btnSettings

                        height: Theme.iconSizeMedium
                        width: Theme.iconSizeMedium
                        icon.scale: Theme.iconSizeMedium / icon.height

                        icon.source: "qrc:/img/icon-gear.svg?"
                                     + (pressed
                                        ? Theme.lightHighlightColor
                                        : Theme.lightPrimaryColor)
                        onClicked: {
                            console.log("[SettingsButton] pressed")
                            pulldown.close(false)
                        }
                    }
                }
            }
        }

       TubeItem {
            id: tube1

            scale: 1.678
            shade: 0
        }
    }
}
