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
    property variant locationData;
    property int chatId;
    property variant pictureFileInformation;

    Component.onCompleted: {
        updatePicture();
    }

    function updatePicture() {
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

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(!processLauncher.launchProgram('harbour-pure-maps', ["geo:"+locationData.latitude+","+locationData.longitude])) {
                    imageNotification.show(qsTr("Install Pure Maps to inspect this location."));
                }
            }
            Rectangle {
                anchors.fill: parent
                color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                opacity: parent.pressed ? 1.0 : 0.0
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
