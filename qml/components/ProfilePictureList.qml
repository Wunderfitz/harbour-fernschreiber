/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

    This file is part of Fernschreiber.

    Fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fernschreiber is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fernschreiber. If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.6
import Sailfish.Silica 1.0

Item {
    id: profilePictureListItem
    visible: imageContainer.thumbnailVisible && bigProfilePictureList.count > 0
    property bool isActive: imageContainer.thumbnailActive
    readonly property int currentPictureIndex: bigProfilePictureList.currentIndex

    opacity: isActive ? 1.0 : 0.0
    Behavior on opacity { FadeAnimation {} }

    SlideshowView {
        id: bigProfilePictureList
        width: parent.width
        height: parent.height
        clip: true
        itemWidth: width
        itemHeight: height
        interactive: parent.isActive
        model: imageContainer.thumbnailModel
        delegate: Item {
            width: bigProfilePictureList.itemWidth
            height: bigProfilePictureList.itemHeight
            ProfileThumbnail {
                id: chatPictureDetail
                photoData: modelData.sizes[modelData.sizes.length - 1].photo
                replacementStringHint: ""
                radius: imageContainer.thumbnailRadius
                anchors.fill: parent
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), { "photoData" : modelData });
                }
            }
        }
    }

    Text {
        visible: bigProfilePictureList.count > 1
        width: parent.width - Theme.paddingSmall
        anchors {
            bottomMargin: Theme.paddingSmall
            bottom: parent.bottom
        }
        horizontalAlignment: Text.AlignHCenter
        property var baseString: new Array(bigProfilePictureList.count+1).join(" ○ ")
        text: (baseString.substring(0,bigProfilePictureList.currentIndex*3) + " ● " + baseString.substring((bigProfilePictureList.currentIndex+1)*3)).trim()
        font.pixelSize: Theme.fontSizeTiny
        color: Theme.primaryColor
        style: Text.Raised
        styleColor: Theme.highlightDimmerColor
    }

}
