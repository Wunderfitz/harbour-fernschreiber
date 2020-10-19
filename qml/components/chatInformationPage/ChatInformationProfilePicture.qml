/*
    Copyright (C) 2020 Sebastian J. Wolf and other contributors

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
    visible: imageContainer.tweenFactor > 0.8 && chatPictureDetail.imageStatus === Image.Ready
    property bool isActive: imageContainer.tweenFactor === 1.0
    opacity: isActive ? 1.0 : 0.0
    Behavior on opacity { NumberAnimation {} }
    ProfileThumbnail {
        id: chatPictureDetail
        anchors.fill: parent
        photoData: (typeof chatInformation.photo !== "undefined") ? chatInformation.photo.big : ""
        replacementStringHint: ""
        radius: chatPictureThumbnail.radius
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            var preparedPhotoData = {sizes:[{width:640,height: 640,photo:chatPictureDetail.photoData}]};
            pageStack.push(Qt.resolvedUrl("../../pages/ImagePage.qml"), { "photoData" : preparedPhotoData });
        }
    }
}
