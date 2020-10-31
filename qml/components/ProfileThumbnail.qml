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

    id: profileThumbnail

    property variant photoData;
    property string replacementStringHint: "X"
    property bool forceElementUpdate: false
    property int radius: width / 2
    property int imageStatus: -1

    function getReplacementString() {
        if (replacementStringHint.length > 2) {
            // Remove all emoji images
            var strippedText = replacementStringHint.replace(/\<[^>]+\>/g, "").trim();
            if (strippedText.length > 0) {
                var textElements = strippedText.split(" ");
                if (textElements.length > 1) {
                    return textElements[0].charAt(0) + textElements[textElements.length - 1].charAt(0);
                } else {
                    return textElements[0].charAt(0);
                }
            }
        }
        return replacementStringHint;
    }

    function updatePicture() {
        profileImageLoader.active = false;
        replacementThumbnailItem.visible = true;
        if (typeof photoData === "object") {
            if (photoData.local.is_downloading_completed) {
                profileImageLoader.active = true;
                replacementThumbnailItem.visible = false;
            } else {
                tdLibWrapper.downloadFile(photoData.id);
            }
        }
    }

    Timer {
        id: updatePictureTimer
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            updatePicture();
        }
    }

    Component.onCompleted: {
        updatePictureTimer.start();
    }

    onPhotoDataChanged: {
        if (profileThumbnail.forceElementUpdate) {
            updatePictureTimer.stop();
            updatePictureTimer.start();
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (typeof photoData !== "undefined" && fileId === photoData.id) {
                console.log("File updated, completed? " + fileInformation.local.is_downloading_completed);
                if (fileInformation.local.is_downloading_completed) {
                    photoData = fileInformation;
                    profileImageLoader.active = true;
                    replacementThumbnailItem.visible = false;
                }
            }
        }
    }

    Component {
        id: profileImageComponent
        Item {
            width: parent.width
            height: width

            Image {
                id: singleImage
                width: parent.width - Theme.paddingSmall
                height: parent.height - Theme.paddingSmall
                anchors.centerIn: parent
                source: profileThumbnail.photoData.local.path

                fillMode: Image.PreserveAspectCrop
                autoTransform: true
                asynchronous: true
                visible: false
                onStatusChanged: {
                    profileThumbnail.imageStatus = status
                }
            }

            Rectangle {
                id: profileThumbnailMask
                width: parent.width - Theme.paddingSmall
                height: parent.height - Theme.paddingSmall
                color: Theme.primaryColor
                radius: profileThumbnail.radius
                anchors.centerIn: singleImage
                visible: false
            }

            OpacityMask {
                id: maskedThumbnail
                source: singleImage
                maskSource: profileThumbnailMask
                anchors.fill: singleImage
                visible: singleImage.status === Image.Ready ? true : false
                opacity: singleImage.status === Image.Ready ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
            }
        }
    }

    Loader {
        id: profileImageLoader
        active: false
        asynchronous: true
        width: parent.width
        sourceComponent: profileImageComponent
    }

    Item {
        id: replacementThumbnailItem
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        visible: true

        Rectangle {
            id: replacementThumbnailBackground
            anchors.fill: parent
            color: (Theme.colorScheme === Theme.LightOnDark) ? Theme.darkSecondaryColor : Theme.lightSecondaryColor
            radius: parent.width / 2
            opacity: 0.8
        }

        Text {
            id: replacementThumbnailText
            anchors.centerIn: replacementThumbnailBackground
            text: getReplacementString()
            color: Theme.primaryColor
            font.bold: true
            font.pixelSize: ( profileThumbnail.height >= Theme.itemSizeSmall ) ? Theme.fontSizeLarge : Theme.fontSizeMedium
        }

    }

}
