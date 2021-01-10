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
import WerkWolf.Fernschreiber 1.0
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Column {
    id: gamePreviewItem

    property ListItem messageListItem
    property MessageOverlayFlickable overlayFlickable
    property var rawMessage: messageListItem ? messageListItem.myMessage : overlayFlickable.overlayMessage
    property bool highlighted

    width: parent.width
    height: childrenRect.height


    Label {
        width: parent.width
        font.bold: true
        font.pixelSize: Theme.fontSizeSmall
        text: Emoji.emojify(rawMessage.content.game.title || "", font.pixelSize)
        truncationMode: TruncationMode.Fade
        textFormat: Text.StyledText
        wrapMode: Text.Wrap
    }
    Label {
        width: parent.width
        font.pixelSize: Theme.fontSizeExtraSmall
        text: Emoji.emojify(rawMessage.content.game.description || "", font.pixelSize)
        truncationMode: TruncationMode.Fade
        textFormat: Text.StyledText
        wrapMode: Text.Wrap
    }
    Label {
        width: parent.width
        font.pixelSize: Theme.fontSizeExtraSmall
        text: Emoji.emojify(Functions.enhanceMessageText(rawMessage.content.game.text) || "", font.pixelSize)
        truncationMode: TruncationMode.Fade
        wrapMode: Text.Wrap
        textFormat: Text.StyledText
        onLinkActivated: {
            var chatCommand = Functions.handleLink(link);
            if(chatCommand) {
                tdLibWrapper.sendTextMessage(chatInformation.id, chatCommand);
            }
        }
    }
    Item {
        width: parent.width
        height: Theme.paddingLarge
    }

    Image {
        id: thumbnail
        source: thumbnailFile.isDownloadingCompleted ? thumbnailFile.path : ""
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        width: visible ? parent.width : 0
//        height: width
        opacity: status === Image.Ready ? 1.0 : 0.0

        Behavior on opacity { FadeAnimation {} }
        layer.enabled: queryResultItem.pressed
        layer.effect: PressEffect { source: thumbnail }

        TDLibFile {
            id: thumbnailFile
            tdlib: tdLibWrapper
            autoLoad: true
        }
        Rectangle {
            width: Theme.iconSizeMedium
            height: width
            anchors {
                top: parent.top
                topMargin: Theme.paddingSmall
                left: parent.left
                leftMargin: Theme.paddingSmall
            }

            color: Theme.rgba(Theme.overlayBackgroundColor, 0.2)
            radius: Theme.paddingSmall
            Icon {
                id: icon
                source: "image://theme/icon-m-game-controller"
                asynchronous: true
            }
        }
    }

    Component.onCompleted: {
        if (rawMessage.content.game.photo) {
            // Check first which size fits best...
            var photo
            for (var i = 0; i < rawMessage.content.game.photo.sizes.length; i++) {
                photo = rawMessage.content.game.photo.sizes[i].photo
                if (rawMessage.content.game.photo.sizes[i].width >= gamePreviewItem.width) {
                    break
                }
            }
            if (photo) {
                thumbnailFile.fileInformation = photo
            }
        }
    }
}
