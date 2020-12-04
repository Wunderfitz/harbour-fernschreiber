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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Item {

    id: imagePreviewItem

    property ListItem messageListItem
    property MessageOverlayFlickable overlayFlickable
    property var rawMessage: messageListItem ? messageListItem.myMessage : overlayFlickable.overlayMessage

    property var locationData : ( rawMessage.content['@type'] === "messageLocation" ) ?  rawMessage.content.location : ( ( rawMessage.content['@type'] === "messageVenue" ) ? rawMessage.content.venue.location : "" )

    property string chatId: rawMessage.chat_id
    property var pictureFileInformation;
    width: parent.width
    height: width / 2

    Component.onCompleted: {
        updatePicture();
    }
    function clicked(){
        if(!processLauncher.launchProgram('harbour-pure-maps', ["geo:"+locationData.latitude+","+locationData.longitude])) {
            imageNotification.show(qsTr("Install Pure Maps to inspect this location."));
        }
    }
    function updatePicture() {
        imagePreviewItem.pictureFileInformation = null;
        if (locationData) {
            tdLibWrapper.getMapThumbnailFile(chatId, locationData.latitude, locationData.longitude, Math.round(imagePreviewItem.width), Math.round(imagePreviewItem.height));
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            // we do not have a way of knowing if this is the correct file, so we have to guess the first new one should be right.
            if(!imagePreviewItem.pictureFileInformation) {
                imagePreviewItem.pictureFileInformation = fileInformation;
                tdLibWrapper.downloadFile(imagePreviewItem.pictureFileInformation.id);
            } else if(imagePreviewItem.pictureFileInformation && fileInformation.id === imagePreviewItem.pictureFileInformation.id) {
                imagePreviewItem.pictureFileInformation = fileInformation;
                singleImage.source = fileInformation.local.path;
            }
        }
    }

    AppNotification {
        id: imageNotification
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
        Item {
            anchors.centerIn: parent
            width: markerImage.width
            height: markerImage.height * 1.75 // 0.875 (vertical pin point) * 2
            Image {
                id: markerImage
                source: 'image://theme/icon-m-location'
            }

            DropShadow {
                anchors.fill: markerImage
                horizontalOffset: 3
                verticalOffset: 3
                radius: 8.0
                samples: 17
                color: Theme.colorScheme ? Theme.lightPrimaryColor : Theme.darkPrimaryColor
                source: markerImage
            }
        }
    }

    BackgroundImage {
        visible: singleImage.status !== Image.Ready
    }

}
