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
import WerkWolf.Fernschreiber 1.0

Item {

    id: profileThumbnail

    property alias photoData: file.fileInformation
    property string replacementStringHint: "X"
    property int radius: width / 2
    property int imageStatus: -1
    property bool optimizeImageSize: true

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

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        autoLoad: true
    }

    Component {
        id: profileImageComponent
        Item {
            width: parent.width
            height: width
            visible: opacity > 0
            opacity: singleImage.status === Image.Ready ? 1 : 0
            Behavior on opacity { FadeAnimation {} }

            Image {
                id: singleImage
                width: parent.width - Theme.paddingSmall
                height: width
                anchors.centerIn: parent
                source: file.path
                sourceSize.width: optimizeImageSize ? width : undefined
                sourceSize.height: optimizeImageSize ? height : undefined
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
                source: singleImage
                maskSource: profileThumbnailMask
                anchors.fill: singleImage
            }
        }
    }

    Loader {
        id: profileImageLoader
        active: file.isDownloadingCompleted
        asynchronous: true
        width: parent.width
        sourceComponent: profileImageComponent
    }

    Item {
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        visible: !profileImageLoader.item || !profileImageLoader.item.visible

        Rectangle {
            id: replacementThumbnailBackground
            anchors.fill: parent
            color: (Theme.colorScheme === Theme.LightOnDark) ? Theme.darkSecondaryColor : Theme.lightSecondaryColor
            radius: parent.width / 2
            opacity: 0.8
        }

        Text {
            anchors.centerIn: replacementThumbnailBackground
            text: getReplacementString()
            color: Theme.primaryColor
            font.bold: true
            font.pixelSize: ( profileThumbnail.height >= Theme.itemSizeSmall ) ? Theme.fontSizeLarge : Theme.fontSizeMedium
        }
    }
}
