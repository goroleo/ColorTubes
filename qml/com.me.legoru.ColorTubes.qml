import QtQuick 2.0
import Sailfish.Silica 1.0

ApplicationWindow
{
    initialPage: Qt.resolvedUrl("pages/MainPage.qml")
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: Screen.sizeCategory >= Screen.Large ? Orientation.All : Orientation.Portrait
}
