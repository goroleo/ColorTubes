import QtQuick 2.0
import ShadeLayer 1.0

Item {
    id: tubeItem
    width: shade.width
    height: shade.height

    ShadeLayer {
        id: shade
        shade: 1
        visible: false
    }

    MouseArea {
        width: parent
        height: parent
    }

}
