/*
    Copyright (C) 2020 Sebastian J. Wolf

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
import "../"

Item {
    visible: imageContainer.tweenFactor > 0.8 && bigProfilePictureList.count > 0
    property bool isActive: imageContainer.tweenFactor === 1.0

    opacity: isActive ? 1.0 : 0.0
    Behavior on opacity { NumberAnimation {} }

    SlideshowView {
        id: bigProfilePictureList
        property bool isActive: imageContainer.tweenFactor === 1.0
        width: parent.width
        height: parent.height
        clip: true
        itemWidth: width
        itemHeight: height
        interactive: parent.isActive
        model: chatInformationPage.chatPartnerProfilePhotos
        delegate: Item {
            width: bigProfilePictureList.itemWidth
            height: bigProfilePictureList.itemHeight
            ProfileThumbnail {
                id: chatPictureDetail
                photoData: modelData.sizes[modelData.sizes.length - 1].photo
                replacementStringHint: ""
                radius: chatPictureThumbnail.radius
                anchors.fill: parent
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../../pages/ImagePage.qml"), { "photoData" : modelData });
                }
            }
        }
    }

    Text {
        visible: bigProfilePictureList.count > 1
        width: parent.width
        anchors {
            bottomMargin: Theme.paddingSmall
            bottom: parent.bottom
        }
        horizontalAlignment: Text.AlignHCenter
        property var baseString: new Array(bigProfilePictureList.count+1).join(" ○ ")
        text: baseString.substring(0,bigProfilePictureList.currentIndex*3) + " ● " + baseString.substring((bigProfilePictureList.currentIndex+1)*3)
        color: Theme.primaryColor
        style: Text.Raised
        styleColor: Theme.highlightDimmerColor
    }
}
