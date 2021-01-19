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
import WerkWolf.Fernschreiber 1.0
import Sailfish.Silica 1.0

Item {
    id: tdLibPhoto
    property var photo
    property bool highlighted
    readonly property alias fileInformation: tdLibImage.fileInformation
    readonly property alias image: tdLibImage

    onWidthChanged: setImageFile()
    onPhotoChanged: setImageFile()

    function setImageFile() {
        if (photo) {
            var photoSize;
            for (var i = 0; i < photo.sizes.length; i++) {
                photoSize = photo.sizes[i].photo;
                if (photo.sizes[i].width >= width) {
                    break;
                }
            }
            if (photoSize && photoSize.id !== tdLibImage.fileInformation.id) {
                tdLibImage.fileInformation = photoSize;
            }
        }
    }

    Loader {
        anchors.fill: parent
        active: !!tdLibPhoto.photo.minithumbnail
        asynchronous: true
        sourceComponent: Component {
            Image {
                id: minithumbnail
                source: "data:image/jpg;base64,"+tdLibPhoto.photo.minithumbnail.data
                asynchronous: true
                fillMode: tdLibImage.fillMode
                smooth: false
                cache: false

                layer {
                    enabled: tdLibPhoto.highlighted
                    effect: PressEffect { source: minithumbnail }
                }
            }
        }
    }

    TDLibImage {
        id: tdLibImage
        width: parent.width //don't use anchors here for easier custom scaling
        height: parent.height
        cache: false
        highlighted: parent.highlighted
    }

    Component.onCompleted: setImageFile()
}
