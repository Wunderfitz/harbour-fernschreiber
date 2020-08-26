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

    id: stickerPreviewItem

    property variant stickerData;
    property int usedFileId;

    width: stickerData.width + Theme.paddingSmall
    height: stickerData.height + Theme.paddingSmall

    Component.onCompleted: {
        updateSticker();
    }

    function updateSticker() {
        if (typeof stickerData === "object") {
            if (stickerData.is_animated) {
                // Use thumbnail until we can decode TGS files
                usedFileId = stickerData.thumbnail.photo.id;
                if (stickerData.thumbnail.photo.local.is_downloading_completed) {
                    singleImage.source = stickerData.thumbnail.photo.local.path;
                } else {
                    tdLibWrapper.downloadFile(usedFileId);
                }
            } else {
                usedFileId = stickerData.sticker.id;
                if (stickerData.sticker.local.is_downloading_completed) {
                    singleImage.source = stickerData.sticker.local.path;
                } else {
                    tdLibWrapper.downloadFile(usedFileId);
                }
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (typeof stickerData === "object") {
                if (fileId === usedFileId) {
                    console.log("File updated, completed? " + fileInformation.local.is_downloading_completed);
                    if (fileInformation.local.is_downloading_completed) {                        
                        if (stickerData.is_animated) {
                            stickerData.thumbnail.photo = fileInformation;
                        } else {
                            stickerData.sticker = fileInformation;
                        }
                        singleImage.source = fileInformation.local.path;
                    }
                }
            }
        }
    }

    Image {
        id: singleImage
        width: ( ( parent.width - Theme.paddingSmall ) >= stickerData.width ) ? stickerData.width : ( parent.width - Theme.paddingSmall )
        height: ( ( parent.height - Theme.paddingSmall ) >= stickerData.height ) ? stickerData.height : ( parent.height - Theme.paddingSmall )
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
                //pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), { "photoData" : imagePreviewItem.photoData, "pictureFileInformation" : imagePreviewItem.pictureFileInformation });
            }
        }
    }

    Image {
        id: imageLoadingBackgroundImage
        source: "../../images/background" + ( Theme.colorScheme ? "-black" : "-white" ) + ".png"
        anchors {
            centerIn: parent
        }
        width: ( ( parent.width - Theme.paddingSmall ) >= stickerData.width ) ? stickerData.width : ( parent.width - Theme.paddingSmall )
        height: ( ( parent.height - Theme.paddingSmall ) >= stickerData.height ) ? stickerData.height : ( parent.height - Theme.paddingSmall )
        visible: singleImage.status !== Image.Ready

        fillMode: Image.PreserveAspectFit
        opacity: 0.15
    }

}
