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
import "../js/functions.js" as Functions

Column {
    id: webPagePreviewColumn

    property var webPageData;
    property bool largerFontSize: false;
    property bool highlighted
    readonly property bool hasImage: picture.fileId !== 0
    readonly property int fontSize: largerFontSize ? Theme.fontSizeSmall : Theme.fontSizeExtraSmall

    spacing: Theme.paddingSmall

    Component.onCompleted: updatePhoto()

    onWebPageDataChanged: updatePhoto()

    function updatePhoto() {
        if (webPageData) {
            if (webPageData.photo) {
                // Check first which size fits best...
                var photo
                for (var i = 0; i < webPageData.photo.sizes.length; i++) {
                    photo = webPageData.photo.sizes[i].photo;
                    if (webPageData.photo.sizes[i].width >= webPagePreviewColumn.width) {
                        break;
                    }
                }
                if (photo) {
                    picture.fileInformation = photo
                }
            }
        }
    }

    function clicked() {
        descriptionText.toggleMaxLineCount()
    }

    TDLibFile {
        id: picture
        tdlib: tdLibWrapper
        autoLoad: true
    }

    MultilineEmojiLabel {
        id: siteNameText

        width: parent.width
        rawText: webPageData.site_name ? webPageData.site_name : ""
        font.pixelSize: webPagePreviewColumn.fontSize
        font.bold: true
        color: Theme.secondaryHighlightColor
        visible: (rawText !== "")
        maxLineCount: 1
    }

    MultilineEmojiLabel {
        id: titleText

        width: parent.width
        rawText: webPageData.title ? webPageData.title : ""
        font.pixelSize: webPagePreviewColumn.fontSize
        font.bold: true
        visible: (rawText !== "")
        maxLineCount: 2
    }

    MultilineEmojiLabel {
        id: descriptionText

        width: parent.width
        rawText: webPageData.description ? Functions.enhanceMessageText(webPageData.description) : ""
        font.pixelSize: webPagePreviewColumn.fontSize
        visible: (rawText !== "")
        readonly property int defaultMaxLineCount: 3
        maxLineCount: defaultMaxLineCount
        linkColor: Theme.highlightColor
        onLinkActivated: {
            Functions.handleLink(link);
        }
        function toggleMaxLineCount() {
            maxLineCount = maxLineCount > 0 ? 0 : defaultMaxLineCount
        }
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
            source: picture.isDownloadingCompleted ? picture.path : ""
            visible: opacity > 0
            opacity: hasImage && status === Image.Ready ? 1 : 0
            layer.enabled: webPagePreviewColumn.highlighted
            layer.effect: PressEffect { source: singleImage }
            Behavior on opacity { FadeAnimation {} }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), { "photoData" : webPageData.photo, "pictureFileInformation" : picture.fileInformation });
                }
            }
        }

        BackgroundImage {
            id: backgroundImage
            visible: hasImage && singleImage.status !== Image.Ready
            layer.enabled: webPagePreviewColumn.highlighted
            layer.effect: PressEffect { source: backgroundImage }
        }
    }

    Label {
        width: parent.width
        text: qsTr("Preview not supported for this link...")
        font.pixelSize: webPagePreviewColumn.largerFontSize ? Theme.fontSizeExtraSmall : Theme.fontSizeTiny
        font.italic: true
        color: Theme.secondaryColor
        truncationMode: TruncationMode.Fade
        visible: !siteNameText.visible && !titleText.visible && !descriptionText.visible && !webPagePreviewImageItem.visible
    }

}
