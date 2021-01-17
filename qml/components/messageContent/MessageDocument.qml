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
import "../../js/twemoji.js" as Emoji

MessageContentFileInfoBase {
    id: contentItem
    fileInformation: rawMessage.content.document.document

    primaryText: Emoji.emojify(rawMessage.content.document.file_name || "", primaryLabel.font.pixelSize)
    secondaryText: Emoji.emojify(Functions.enhanceMessageText(rawMessage.content.caption) || "", secondaryLabel.font.pixelSize)

    minithumbnail: rawMessage.content.document.minithumbnail
    thumbnail: rawMessage.content.document.thumbnail

    leftButton {
        icon.source: Theme.iconForMimeType(rawMessage.content.document.mime_type)
        onClicked: {
            if(file.isDownloadingCompleted) {
                // in this case, the MouseArea should take over
                tdLibWrapper.openFileOnDevice(file.path);
            } else if(!file.isDownloadingActive) {
                file.load();
            } else {
                file.cancel()
            }
        }
    }

    states: [
        State {
            when: file.isDownloadingCompleted
            PropertyChanges { target: openMouseArea; enabled: true }
            PropertyChanges {
                target: primaryLabel
                color: (contentItem.highlighted || openMouseArea.pressed) ? Theme.highlightColor : Theme.primaryColor
            }
            PropertyChanges {
                target: secondaryLabel
                color: (contentItem.highlighted || openMouseArea.pressed) ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }
            PropertyChanges {
                target: tertiaryLabel
                color: (contentItem.highlighted || openMouseArea.pressed) ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }
            PropertyChanges {
                target: leftButton
                highlighted: contentItem.highlighted || openMouseArea.pressed
            }
        }

    ]
    MouseArea {
        id: openMouseArea
        enabled: file.isDownloadingCompleted
        visible: enabled
        anchors {
            fill: primaryItem
            rightMargin: copyButton.width
        }
        onClicked: {
            tdLibWrapper.openFileOnDevice(file.path);
        }
    }
}
