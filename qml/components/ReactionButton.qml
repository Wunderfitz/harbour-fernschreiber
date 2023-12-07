import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: button

    height: Theme.itemSizeExtraSmall
    width: Theme.itemSizeExtraSmall
    color: Theme.rgba(Theme.primaryColor, 0.4)
    radius: width / 2

    signal clicked()

    IconButton {
        icon.source: "image://theme/icon-s-favorite"
        anchors.centerIn: parent
        onClicked: button.clicked()
    }
}
