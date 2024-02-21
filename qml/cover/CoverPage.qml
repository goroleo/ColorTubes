import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    id: bg

    Image {
        id: icon
//        source: "image://bottle"
        source: StandardPaths.home + "/.colortubes/shade-green.png"

        anchors.centerIn: parent
    }

    Label {
        id: label
        anchors.centerIn: parent
        text: qsTr("Color Tubes")
    }
}
