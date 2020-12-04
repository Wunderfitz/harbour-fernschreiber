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
import WerkWolf.Fernschreiber 1.0

Item {
    id: imagePreviewItem

    property ListItem messageListItem
    property MessageOverlayFlickable overlayFlickable
    property var rawMessage: messageListItem ? messageListItem.myMessage : overlayFlickable.overlayMessage
    property var photoData: rawMessage.content.photo
    readonly property int defaultHeight: Math.round(width * 2 / 3)
    property bool highlighted

    width: parent.width
    height: singleImage.visible ? Math.min(defaultHeight, singleImage.bestHeight + Theme.paddingSmall) : defaultHeight

    function clicked() {
        pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), {
            "photoData" : imagePreviewItem.photoData,
            "pictureFileInformation" : imageFile.fileInformation
        })
    }

    Component.onCompleted: {
        if (photoData) {
            // Check first which size fits best...
            var photo
            for (var i = 0; i < photoData.sizes.length; i++) {
                photo = photoData.sizes[i].photo
                if (photoData.sizes[i].width >= imagePreviewItem.width) {
                    break
                }
            }
            if (photo) {
                imageFile.fileInformation = photo
            }
        }
    }

    TDLibFile {
        id: imageFile
        tdlib: tdLibWrapper
        autoLoad: true
    }

    Image {
        id: singleImage
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        readonly property int bestHeight: (status === Image.Ready) ? Math.round(implicitHeight * width / implicitWidth) : 0
        anchors.centerIn: parent

        fillMode: Image.PreserveAspectCrop
        autoTransform: true
        asynchronous: true
        source: imageFile.isDownloadingCompleted ? imageFile.path : ""
        visible: status === Image.Ready
        opacity: visible ? 1 : 0
        Behavior on opacity { FadeAnimation {} }
        layer.enabled: imagePreviewItem.highlighted
        layer.effect: PressEffect { source: singleImage }
    }

    BackgroundImage {
        visible: singleImage.status !== Image.Ready
    }
}
