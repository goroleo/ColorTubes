import QtQuick 2.0
import Sailfish.Silica 1.0

Icon {
    id: iconButtonItem
    signal clicked

    height: Theme.iconSizeMedium
    width: Theme.iconSizeMedium
    opacity: enabled? 1.0 : Theme.opacityLow
    color: Theme.lightPrimaryColor
    highlightColor: Theme.highlightColor

    MouseArea {
        anchors.fill: parent
        onPressedChanged:
            iconButtonItem.highlighted = pressed;
        onClicked:
            iconButtonItem.clicked()
    }
}
