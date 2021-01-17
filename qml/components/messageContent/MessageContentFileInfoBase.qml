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
import Sailfish.Silica 1.0
import QtMultimedia 5.6
import WerkWolf.Fernschreiber 1.0
import QtGraphicalEffects 1.0
import "../"
import "../../js/functions.js" as Functions
import "../../js/twemoji.js" as Emoji
import "../../js/debug.js" as Debug

MessageContentBase {
    id: contentItem
    height: childrenRect.height
    property alias fileInformation: file.fileInformation
    property alias primaryLabel: primaryLabel
    property alias primaryText: primaryLabel.text
    property alias secondaryLabel: secondaryLabel
    property alias secondaryText: secondaryLabel.text
    property alias tertiaryLabel: tertiaryLabel
    property alias tertiaryText: tertiaryLabel.text
    property var thumbnail
    property var minithumbnail

    readonly property alias file: file
    readonly property alias primaryItem: primaryItem
    readonly property alias leftButton: leftButton
    readonly property alias labelsColumn: labelsColumn
    readonly property alias copyButton: copyButton
//    readonly property alias downloadNeededIndicatorIcon: downloadNeededIndicatorIcon

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        autoLoad: false
    }

    Item {
        id: primaryItem
        width: parent.width
        height: Theme.itemSizeLarge
        Loader {
            active: contentItem.thumbnail || contentItem.minithumbnail
            visible: active
            anchors.fill: leftButton
            sourceComponent: Component {
                TDLibThumbnail {
                    opacity: 0.3
                    thumbnail: contentItem.thumbnail
                    minithumbnail: contentItem.minithumbnail
                }
            }
        }

        IconButton {
            id: leftButton
            highlighted: down || contentItem.highlighted
            anchors.verticalCenter: parent.verticalCenter
            icon {
                asynchronous: true
            }

            ProgressCircle {
                value: file.downloadedSize / file.expectedSize
                progressColor: Theme.highlightColor
                backgroundColor: Theme.highlightDimmerColor
                width: Theme.iconSizeMedium
                height: Theme.iconSizeMedium
                visible: opacity > 0
                opacity: file.isDownloadingActive ? 1.0 : 0.0
                anchors.centerIn: parent
                Behavior on opacity { FadeAnimation {} }
            }
            Rectangle {
                anchors.centerIn: downloadNeededIndicatorIcon
                width: downloadNeededIndicatorIcon.width + Theme.paddingMedium
                height: width

                color: Theme.rgba(Theme.overlayBackgroundColor, 0.2)
                opacity: file.isDownloadingActive ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
                visible: opacity > 0
                radius: width/2
            }

            Icon {
                id: downloadNeededIndicatorIcon
                source: file.isDownloadingActive || file.isDownloadingCompleted ? "image://theme/icon-s-clear-opaque-cross" : "image://theme/icon-s-cloud-download"
                asynchronous: true
                width: Theme.iconSizeExtraSmall
                height: width
                visible: opacity > 0
                sourceSize.width: width
                sourceSize.height: height
                opacity: file.isDownloadingCompleted ? 0.0 : 1.0
                Behavior on opacity { FadeAnimation {} }
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: Theme.paddingSmall
                }
            }
        }

        Column {
            id: labelsColumn
            anchors {
                left: leftButton.right
                leftMargin: Theme.paddingSmall
                right: copyButton.left
                verticalCenter: leftButton.verticalCenter
            }

            Label {
                id: primaryLabel
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
                fontSizeMode: Text.HorizontalFit
                minimumPixelSize: Theme.fontSizeTiny
                color: Theme.highlightColor
                visible: text.length > 0
                truncationMode: TruncationMode.Fade
            }

            Label {
                id: secondaryLabel
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                fontSizeMode: Text.HorizontalFit
                minimumPixelSize: Theme.fontSizeTiny
                color: Theme.secondaryHighlightColor
                visible: text.length > 0
                truncationMode: TruncationMode.Fade
            }
            Item {
                height: sizeLabel.height
                width: parent.width
                Label {
                    id: tertiaryLabel
                    font.pixelSize: Theme.fontSizeTiny
                    color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    visible: text.length > 0
                    truncationMode: TruncationMode.Fade
                }
                Label {
                    id: sizeLabel
                    anchors.right: parent.right
                    font.pixelSize: Theme.fontSizeTiny
                    color: tertiaryLabel.color
                    text: Format.formatFileSize(file.size || file.expectedSize)
                    visible: (file.size || file.expectedSize) > 0
                    truncationMode: TruncationMode.Fade
                }
            }
        }
        IconButton {
            id: copyButton
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            opacity: file.isDownloadingCompleted ? 1.0 : 0.0
            width: file.isDownloadingCompleted ? Theme.itemSizeMedium : 0
            visible: opacity > 0

            Behavior on opacity { FadeAnimation {} }
            Behavior on width { NumberAnimation { duration: 200 } }
            icon {
                asynchronous: true
                source: "../../../images/icon-m-copy-to-folder.svg"
                sourceSize {
                    width: Theme.iconSizeMedium
                    height: Theme.iconSizeMedium
                }
            }
            onClicked: {
                tdLibWrapper.copyFileToDownloads(file.path);
                // not persistent:
                opacity = 0;
                width = 0;
            }
        }
    }
}
