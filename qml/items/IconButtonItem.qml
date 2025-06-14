import QtQuick 2.0
import Sailfish.Silica 1.0
import Game 1.0

Icon {
    id: iconButtonItem
    signal clicked

    height: Theme.iconSizeMedium
    width: Theme.iconSizeMedium
    opacity: enabled? 1.0 : Theme.opacityLow
    //    color: (Theme.colorScheme == Theme.DarkOnLight)? Theme.darkPrimaryColor : Theme.lightPrimaryColor
    color: (Game.isLightTheme)? Theme.darkPrimaryColor : Theme.lightPrimaryColor
    highlightColor: Theme.highlightColor

    MouseArea {
        anchors.fill: parent
        onPressedChanged:
            iconButtonItem.highlighted = pressed;
        onClicked:
            iconButtonItem.clicked()
    }
}
