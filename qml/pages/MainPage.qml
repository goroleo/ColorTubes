import QtQuick 2.0
import Sailfish.Silica 1.0
import ShadeLayer 1.0
import CtImageLayer 1.0
import TubeFlyer 1.0

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

        Label {
            id: levelLabel
            anchors.top: pulldown.bottom
            anchors.bottom: rect.top

            width: parent.width
            text: "Level 145"
            color: Theme.lightPrimaryColor
            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignVCenter

            font {
                bold: true
                pixelSize: Theme.fontSizeLarge
            }
        }


        Rectangle {
            id: rect

            width: 190
            height: 180+180+200

            scale: Math.min((parent.width) / width,
                            (parent.height - levelLabel.height) / height * (0.8) )

            border.width: 1
            border.color: "yellow"

            color: "steelblue"
            anchors.centerIn: parent

        }

        Item {

            id: tubeItem

            width: bottle.width
            height: bottle.height + 20 * bottle.scale
            anchors.top: levelLabel.bottom

            ShadeLayer {
                id: shade
                anchors.top: parent.top
                anchors.topMargin: 20 * bottle.scale
                shade: 3
            }

            CtImageLayer {
                id: bottle
                source: "bottle"
                anchors.top: parent.top
                anchors.topMargin: + 20 * scale
                scale: 1.0994
            }

            CtImageLayer {
                id: cork
                source: "cork"
                anchors.top: parent.top
                anchors.topMargin: 0
                visible: false
            }

//            TubeFlyer {
//                id: flyer
//            }

            MouseArea {
                width: parent.width
                height: parent.height
                onClicked: {
//                    if (!shade.visible)
//                        shade.shade = shade.shade + 1

///                    if (!shade.pulse)
//                        shade.shade = shade.shade + 1

//                    if (shade.shade == 4)
//                        shade.shade = 1

                    shade.visible ? shade.startHide() : shade.startShow()
                    cork.visible = shade.visible;
//                    shade.pulse ? shade.stopPulse() : shade.startPulse()
                }
            }

        }
    }
}
