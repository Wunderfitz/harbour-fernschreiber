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
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Column {

    id: webPagePreviewColumn

    property variant webPageData;
    property variant pictureFileInformation;
    property bool hasImage: false;

    spacing: Theme.paddingSmall

    Component.onCompleted: {
        updateWebPage();
    }

    function updateWebPage() {
        if (typeof webPageData === "object") {
            if (typeof webPageData.photo !== "undefined") {
                hasImage = true;
                // Check first which size fits best...
                for (var i = 0; i < webPageData.photo.sizes.length; i++) {
                    pictureFileInformation = webPageData.photo.sizes[i].photo;
                    if (webPageData.photo.sizes[i].width >= webPagePreviewColumn.width) {
                        break;
                    }
                }
            }

            if (pictureFileInformation.local.is_downloading_completed) {
                singleImage.source = pictureFileInformation.local.path;
            } else {
                tdLibWrapper.downloadFile(pictureFileInformation.id);
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
        text: Emoji.emojify(webPageData.site_name, font.pixelSize)
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
        text: Emoji.emojify(webPageData.title, font.pixelSize)
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
        text: Emoji.emojify(webPageData.description, font.pixelSize)
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

        Image {
            id: imageLoadingBackgroundImage
            source: "../../images/background" + ( Theme.colorScheme ? "-black" : "-white" ) + ".png"
            anchors {
                centerIn: parent
            }
            width: parent.width - Theme.paddingMedium
            height: parent.height - Theme.paddingMedium
            visible: hasImage && singleImage.status !== Image.Ready

            fillMode: Image.PreserveAspectFit
            opacity: 0.15
        }
    }

}
