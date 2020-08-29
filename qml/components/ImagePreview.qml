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
import QtQuick 2.5
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Item {

    id: imagePreviewItem

    property variant photoData;
    property variant pictureFileInformation;

    Component.onCompleted: {
        updatePicture();
    }

    function updatePicture() {
        if (photoData) {
            // Check first which size fits best...
            for (var i = 0; i < photoData.sizes.length; i++) {
                imagePreviewItem.pictureFileInformation = photoData.sizes[i].photo;
                if (photoData.sizes[i].width >= imagePreviewItem.width) {
                    break;
                }
            }

            if (imagePreviewItem.pictureFileInformation.local.is_downloading_completed) {
                singleImage.source = imagePreviewItem.pictureFileInformation.local.path;
            } else {
                tdLibWrapper.downloadFile(imagePreviewItem.pictureFileInformation.id);
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (imagePreviewItem.pictureFileInformation) {
                if (fileId === imagePreviewItem.pictureFileInformation.id && fileInformation.local.is_downloading_completed) {
                    imagePreviewItem.pictureFileInformation = fileInformation;
                    singleImage.source = fileInformation.local.path;
                }
            }
        }
    }

    Image {
        id: singleImage
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        anchors.centerIn: parent

        fillMode: Image.PreserveAspectCrop
        autoTransform: true
        asynchronous: true
        visible: status === Image.Ready
        opacity: status === Image.Ready ? 1 : 0
        Behavior on opacity { NumberAnimation {} }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), { "photoData" : imagePreviewItem.photoData, "pictureFileInformation" : imagePreviewItem.pictureFileInformation });
            }
        }
    }

    Image {
        id: imageLoadingBackgroundImage
        source: "../../images/background-" + ( Theme.colorScheme ? "black" : "white" ) + "-small.png"
        anchors {
            centerIn: parent
        }
        width: parent.width - Theme.paddingMedium
        height: parent.height - Theme.paddingMedium
        visible: singleImage.status !== Image.Ready
        asynchronous: true

        fillMode: Image.PreserveAspectFit
        opacity: 0.15
    }

}
