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
import "../components"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Item {
    id: pinnedMessageItem

    property var pinnedMessage;
    signal requestShowMessage;
    signal requestCloseMessage;

    onPinnedMessageChanged: {
        if (pinnedMessage) {
            console.log("[ChatPage] Activating pinned message");
            var messageUserText = (pinnedMessage.sender_user_id !== chatPage.myUserId) ? Emoji.emojify(Functions.getUserName(tdLibWrapper.getUserInformation(pinnedMessage.sender_user_id)), pinnedMessageUserText.font.pixelSize) : qsTr("You");
            pinnedMessageUserText.text = (messageUserText === "" ? qsTr("Pinned Message") : messageUserText );
            pinnedMessageText.text = Emoji.emojify(Functions.getMessageText(pinnedMessage, true, pinnedMessage.sender_user_id === chatPage.myUserId), pinnedMessageText.font.pixelSize);
            pinnedMessageItem.visible = true;
        } else {
            pinnedMessageItem.visible = false;
        }
    }

    visible: false
    anchors.left: parent.left
    anchors.right: parent.right
    height: visible ? pinnedMessageRow.height : 0

    Rectangle {
        id: pinnedMessageBackground
        anchors.fill: parent
        opacity: 0.1
        color: Theme.secondaryColor
    }

    Row {
        id: pinnedMessageRow

        width: parent.width

        IconButton {
            id: pinnedMessageButton
            width: Theme.itemSizeLarge
            height: Theme.itemSizeLarge
            icon.source: "image://theme/icon-m-mark-unread"
            onClicked: {
                pinnedMessageItem.requestShowMessage();
            }
        }

        Item {
            width: parent.width - pinnedMessageButton.width - unpinMessageIconLoader.width - removePinnedMessageIconButton.width
            height: pinnedMessageColumn.height
            anchors.verticalCenter: parent.verticalCenter
            Column {
                id: pinnedMessageColumn
                spacing: Theme.paddingSmall
                width: parent.width

                Text {
                    id: pinnedMessageUserText

                    width: parent.width
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.weight: Font.ExtraBold
                    color: Theme.primaryColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    textFormat: Text.StyledText
                    horizontalAlignment: Text.AlignLeft
                }

                Text {
                    id: pinnedMessageText

                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    width: parent.width
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    textFormat: Text.StyledText
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    pinnedMessageItem.requestShowMessage();
                }
            }
        }

        Loader {
            id: unpinMessageIconLoader
            asynchronous: true
            active: canPinMessages()
            Behavior on opacity { FadeAnimation {} }
            width: active ? item.width : 0
            height: active ? item.height : 0
            anchors.verticalCenter: parent.verticalCenter
            sourceComponent: Component {
                IconButton {
                    id: unpinMessageIconButton
                    icon.source: "image://theme/icon-m-remove"
                    onClicked: {
                        Remorse.itemAction(pinnedMessageRow, qsTr("Message unpinned"), function() { tdLibWrapper.unpinMessage(chatPage.chatInformation.id);
                                                                                                     pinnedMessageItem.requestCloseMessage(); });

                    }
                }
            }
        }

        IconButton {
            id: removePinnedMessageIconButton
            icon.source: "image://theme/icon-m-clear"
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                pinnedMessageItem.requestCloseMessage();
                pinnedMessage = undefined;
            }
        }
    }
}
