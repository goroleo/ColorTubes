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

/*        Item {

            id: tubeItem

            width: colors.width
            height: colors.height
            anchors.top: levelLabel.bottom
            anchors.horizontalCenter: parent.Center

            ShadeLayer {
                id: shade
                anchors.top: parent.top
                anchors.topMargin: 20 * bottle.scale
                anchors.left: parent.left
                anchors.leftMargin: 100 * bottle.scale

                shade: 3
            }

            BottleLayer {
                id: bottle_back
                source: "back"
                anchors.top: parent.top
                anchors.topMargin: 20 * bottle.scale
                anchors.left: parent.left
                anchors.leftMargin: 100 * bottle.scale
            }

            ColorsLayer {
                id: colors
                anchors.top: parent.top
                anchors.left: parent.left
                angle: 0
            }

            BottleLayer {
                id: bottle
                source: "bottle"
                anchors.top: parent.top
                anchors.topMargin: 35.5 * scale
                anchors.left: parent.left
                anchors.leftMargin: 100 * scale
                scale: 1.894
            }

            CorkLayer {
                id: cork
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 100 * scale
                visible: false
            }

//            TubeFlyer {
//                id: flyer
//            }

            MouseArea {
                anchors.leftMargin: 100 * bottle.scale;
                anchors.topMargin: 20 * bottle.scale;
                width: bottle.width
                height: bottle.height
                onClicked: {
//                    if (colors.count < 4)
//                       colors.fillColors(11, 1)
//                    else
//                        colors.count = 0
                    if (colors.count == 0)
                        colors.count = 4
                    else
                        colors.dropColors(1);


//                    if (!shade.visible)
//                        shade.shade = shade.shade + 1

                    if (!shade.pulse)
                       shade.shade = shade.shade + 1

                    if (shade.shade == 4)
                        shade.shade = 1

//                    colors.setAngle(colors.angle + 5/180*3.1415926)
//                    shade.visible ? shade.startHide() : shade.startShow()
                    shade.pulse ? shade.stopPulse() : shade.startPulse()
                    cork.visible = ((shade.visible || shade.pulse) && (shade.shade == 3))
                }
            }

        }
    }

    */

        TubeItem {
            id: tube1

            scale: 1.5
            shade: 2
        }
    }
}
