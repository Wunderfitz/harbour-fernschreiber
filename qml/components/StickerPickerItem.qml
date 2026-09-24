/*
    Copyright (C) 2026 Sebastian J. Wolf and other contributors

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
import WerkWolf.Fernschreiber 1.0
import "../js/twemoji.js" as Emoji

BackgroundItem {
    id: stickerPickerItem

    property var sticker
    readonly property bool animated: !!sticker && !!sticker.format && sticker.format["@type"] === "stickerFormatTgs" && appSettings.animateStickers
    readonly property bool animationVisible: animatedStickerLoader.item ? animatedStickerLoader.item.status === AnimatedImage.Ready : false

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        fileInformation: stickerPickerItem.animated ? stickerPickerItem.sticker.sticker : ({})
        autoLoad: stickerPickerItem.animated
    }

    // Shown until the animated sticker has been downloaded and can be played
    TDLibThumbnail {
        thumbnail: stickerPickerItem.sticker.thumbnail
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        highlighted: stickerPickerItem.highlighted
        visible: !stickerPickerItem.animationVisible
    }

    Loader {
        id: animatedStickerLoader
        anchors.fill: parent
        active: stickerPickerItem.animated && file.isDownloadingCompleted
        sourceComponent: Component {
            AnimatedImage {
                id: animatedSticker
                anchors.fill: parent
                source: file.path
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                paused: !Qt.application.active
                cache: false
                layer.enabled: stickerPickerItem.highlighted
                layer.effect: PressEffect { source: animatedSticker }
            }
        }
    }

    Label {
        font.pixelSize: Theme.fontSizeSmall
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: Emoji.emojify(stickerPickerItem.sticker.emoji, font.pixelSize)
    }
}
