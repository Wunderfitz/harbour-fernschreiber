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
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Item {
    id: messageListItem
    property var myMessage: display
    property bool senderIsUser: myMessage.sender_id["@type"] === "messageSenderUser"
    property var userInformation: senderIsUser ? tdLibWrapper.getUserInformation(myMessage.sender_id.user_id) : null
    property bool isOwnMessage: senderIsUser && chatPage.myUserId === myMessage.sender_id.user_id
    property var linkedMessage
    height: backgroundRectangle.height + Theme.paddingMedium

    Rectangle {
        id: backgroundRectangle
        anchors.centerIn: parent
        height: messageText.height + Theme.paddingMedium * 2
        width: Math.min(messageText.implicitWidth, messageText.contentWidth) + Theme.paddingMedium * 2
        color: Theme.colorScheme === Theme.LightOnDark ? Theme.rgba(Theme.secondaryColor, 0.1) : Theme.rgba(Theme.overlayBackgroundColor, 0.1)
        radius: parent.width / 50
    }
    Text {
        id: messageText
        width: parent.width - Theme.paddingMedium * 4 - Theme.horizontalPageMargin * 2
        anchors.centerIn: parent
        color: Theme.highlightColor
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        property string messageContentText: Functions.getMessageText(messageListItem.myMessage, false, chatPage.myUserId, false)
        text: (messageListItem.senderIsUser
               ? "<a style=\"text-decoration: none; font-weight: bold; color:"+Theme.primaryColor+"\" href=\"userId://" + messageListItem.userInformation.id + "\">" + (!messageListItem.isOwnMessage ? Emoji.emojify(Functions.getUserName(messageListItem.userInformation), font.pixelSize) : qsTr("You")) + "</a> "
               :  "<a style=\"text-decoration: none; font-weight: bold; color:"+Theme.secondaryHighlightColor+"\">" +  Emoji.emojify(chatPage.chatInformation.title || "") + "</a> ")
            + Emoji.emojify(messageContentText, font.pixelSize)
        textFormat: Text.RichText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        onLinkActivated: {
            if(link === "linkedmessage" && linkedMessage) {
                messageOverlayLoader.overlayMessage = linkedMessage;
                messageOverlayLoader.active = true;
            } else {
                Functions.handleLink(link);
            }

        }
    }
    Loader {
        id: gameScoreInfoLoader
        active: myMessage.content["@type"] === "messageGameScore"
        asynchronous: true
        sourceComponent: Component {
            Connections {
                target: tdLibWrapper
                onReceivedMessage: {
                    if(chatId === chatPage.chatInformation.id && messageId === myMessage.content.game_message_id) {
                        messageListItem.linkedMessage = message;
                        messageText.messageContentText = messageListItem.isOwnMessage ?
                                    qsTr("scored %Ln points in %2", "myself", myMessage.content.score).arg("<a href=\"linkedmessage\" style=\"text-decoration: none; color:"+Theme.primaryColor+"\">"+message.content.game.title+"</a>") :

                                    qsTr("scored %Ln points in %2", "", myMessage.content.score).arg("<a href=\"linkedmessage\" style=\"text-decoration: none; color:"+Theme.primaryColor+"\" >"+message.content.game.title+"</a>");
                    }
                }
                Component.onCompleted: {
                    tdLibWrapper.getMessage(chatPage.chatInformation.id, myMessage.content.game_message_id);
                }
            }
        }
    }
}
