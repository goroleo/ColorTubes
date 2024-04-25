import QtQuick 2.6

import Sailfish.Silica 1.0

import GameBoard 1.0
import FlowerLayer 1.0

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

            text: qsTrId("level") + " " + "145"

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
                flower.startRotate()
            }
        }
    }

    Rectangle {
        id: congratsPanel

        width: parent.width
        height: parent.height
        color: "#448888ff"
        enabled: true
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator {} }

        FlowerLayer {
            id: flower
            anchors.fill: parent
            opacity: 0.5
        }

        Image {
            id: badge
            source: "qrc:/img/badge.svg"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            property real iHeight: badge.sourceSize.height * badge.width / badge.sourceSize.width
        }

        Text {
            id: congratsText
            text: qsTrId("victory")

            x: badge.width * 0.25
            y: congratsPanel.height / 2 - badge.iHeight * 0.25
            width: badge.width * 0.5
            height: badge.iHeight * 0.5
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font.capitalization: Font.AllUppercase
            font.weight: Font.DemiBold
            font.pixelSize: badge.iHeight * 0.3

            color: "#fffa60"
            clip: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                flower.stopRotate()
                congratsPanel.enabled = false
            }
        }
    }



}
