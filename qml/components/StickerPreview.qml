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
import QtQuick 2.5
import Sailfish.Silica 1.0

Item {

    property ListItem messageListItem
    property variant rawMessage: messageListItem.myMessage

    property variant stickerData: rawMessage.content.sticker;
    property int usedFileId;

    width: stickerData.width
    height: stickerData.height

    Component.onCompleted: {
        if (stickerData) {
            if (stickerData.is_animated) {
                // Use thumbnail until we can decode TGS files
                usedFileId = stickerData.thumbnail.photo.id;
                if (stickerData.thumbnail.photo.local.is_downloading_completed) {
                    stickerImage.source = stickerData.thumbnail.photo.local.path;
                } else {
                    tdLibWrapper.downloadFile(usedFileId);
                }
            } else {
                usedFileId = stickerData.sticker.id;
                if (stickerData.sticker.local.is_downloading_completed) {
                    stickerImage.source = stickerData.sticker.local.path;
                } else {
                    tdLibWrapper.downloadFile(usedFileId);
                }
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (stickerData) {
                if (fileId === usedFileId && fileInformation.local.is_downloading_completed) {
                    if (stickerData.is_animated) {
                        stickerData.thumbnail.photo = fileInformation;
                    } else {
                        stickerData.sticker = fileInformation;
                    }
                    stickerImage.source = fileInformation.local.path;
                }
            }
        }
    }

    Image {
        id: stickerImage
        anchors.fill: parent

        fillMode: Image.PreserveAspectFit
        autoTransform: true
        asynchronous: true
        visible: opacity > 0
        opacity: status === Image.Ready ? 1 : 0
        Behavior on opacity { FadeAnimation {} }
    }

    Loader {
        anchors.fill: parent
        sourceComponent: Component {
            BackgroundImage {}
        }

        active: opacity > 0
        opacity: !stickerImage.visible && !placeHolderDelayTimer.running ? 0.15 : 0
        Behavior on opacity { FadeAnimation {} }
    }

    Timer {
        id: placeHolderDelayTimer
        interval: 1000
        running: true
    }
}
