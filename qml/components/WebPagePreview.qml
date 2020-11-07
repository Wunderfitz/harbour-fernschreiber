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
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Column {

    id: webPagePreviewColumn

    property var webPageData;
    property var pictureFileInformation;
    property bool hasImage: false;

    spacing: Theme.paddingSmall

    Component.onCompleted: {
        updateWebPage();
    }

    function updateWebPage() {
        if (webPageData) {
            if (typeof webPageData.photo !== "undefined") {
                hasImage = true;
                // Check first which size fits best...
                for (var i = 0; i < webPageData.photo.sizes.length; i++) {
                    pictureFileInformation = webPageData.photo.sizes[i].photo;
                    if (webPageData.photo.sizes[i].width >= webPagePreviewColumn.width) {
                        break;
                    }
                }
                if (pictureFileInformation.local.is_downloading_completed) {
                    singleImage.source = pictureFileInformation.local.path;
                } else {
                    tdLibWrapper.downloadFile(pictureFileInformation.id);
                }
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (typeof pictureFileInformation !== "undefined" && fileId === pictureFileInformation.id) {
                if (fileInformation.local.is_downloading_completed) {
                    pictureFileInformation = fileInformation;
                    singleImage.source = fileInformation.local.path;
                }
            }
        }
    }

    Text {
        id: siteNameText

        width: parent.width
        text: webPageData.site_name ? Emoji.emojify(webPageData.site_name, font.pixelSize) : ""
        font.pixelSize: Theme.fontSizeExtraSmall
        font.bold: true
        color: Theme.secondaryHighlightColor
        elide: Text.ElideRight
        maximumLineCount: 1
        textFormat: Text.StyledText
        visible: (text !== "")
    }

    Text {
        id: titleText

        width: parent.width
        text: webPageData.title ? Emoji.emojify(webPageData.title, font.pixelSize) : ""
        font.pixelSize: Theme.fontSizeExtraSmall
        font.bold: true
        color: Theme.primaryColor
        elide: Text.ElideRight
        wrapMode: Text.Wrap
        maximumLineCount: 2
        textFormat: Text.StyledText
        visible: (text !== "")
    }

    Text {
        id: descriptionText

        width: parent.width
        text: webPageData.description ? Emoji.emojify(webPageData.description, font.pixelSize) : ""
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.primaryColor
        elide: Text.ElideRight
        wrapMode: Text.Wrap
        maximumLineCount: 3
        textFormat: Text.StyledText
        visible: (text !== "")
    }

    Item {
        id: webPagePreviewImageItem
        width: parent.width
        height: width * 2 / 3
        visible: hasImage

        Image {
            id: singleImage
            width: parent.width - Theme.paddingSmall
            height: parent.height - Theme.paddingSmall
            anchors.centerIn: parent

            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectCrop
            autoTransform: true
            asynchronous: true
            visible: hasImage && status === Image.Ready
            opacity: hasImage && status === Image.Ready ? 1 : 0
            Behavior on opacity { NumberAnimation {} }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), { "photoData" : webPageData.photo, "pictureFileInformation" : pictureFileInformation });
                }
            }
        }

        BackgroundImage {
            visible: hasImage && singleImage.status !== Image.Ready
        }
    }

}
