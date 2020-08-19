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

    id: profileThumbnail

    property variant photoData;
    property string replacementStringHint: "X"

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

    Component.onCompleted: {
        if (typeof photoData === "object") {
            if (photoData.local.is_downloading_completed) {
                singleImage.source = photoData.local.path;
            } else {
                tdLibWrapper.downloadFile(photoData.id);
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (fileId === photoData.id) {
                console.log("File updated, completed? " + fileInformation.local.is_downloading_completed);
                if (fileInformation.local.is_downloading_completed) {
                    photoData = fileInformation;
                    singleImage.source = photoData.local.path;
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
        visible: false
    }

    Rectangle {
        id: profileThumbnailMask
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        color: Theme.primaryColor
        radius: parent.width / 2
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

    Item {
        id: replacementThumbnailItem
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        visible: singleImage.status !== Image.Ready

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
            font.pixelSize: Theme.fontSizeLarge
        }

    }

}
