import QtQuick 2.0
import Sailfish.Silica 1.0
import GameBoard 1.0

CoverBackground {
    id: bg

    Label {
        id: label
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: Theme.horizontalPageMargin
        text: qsTrId("app_title")
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
    }

    Image {
        id: boardImage
        anchors.top: label.bottom
        anchors.topMargin: Theme.horizontalPageMargin
        anchors.left: parent.left
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.horizontalPageMargin
        source: "qrc:/img/icon-cover.png"
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
    }
}
