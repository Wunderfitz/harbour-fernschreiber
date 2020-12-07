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

Item {
    id: messageListItem
    property var myMessage: display
    property var userInformation: tdLibWrapper.getUserInformation(myMessage.sender.user_id)
    property bool isOwnMessage: chatPage.myUserId === myMessage.sender.user_id
    height: backgroundRectangle.height + Theme.paddingMedium

    Rectangle {
        id: backgroundRectangle
        anchors.centerIn: parent
        height: messageText.height + Theme.paddingMedium * 2
        width: Math.min(messageText.implicitWidth, messageText.contentWidth) + Theme.paddingMedium * 2
        color: Theme.rgba(Theme.secondaryColor, 0.1)
        radius: parent.width / 50
    }
    Text {
        id: messageText
        width: parent.width - Theme.paddingMedium * 4 - Theme.horizontalPageMargin * 2
        anchors.centerIn: parent
        color: Theme.highlightColor
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        text: "<a style=\"text-decoration: none; font-weight: bold; color:"+Theme.primaryColor+"\" href=\"userId://" + messageListItem.userInformation.id + "\">" + (!messageListItem.isOwnMessage ? Emoji.emojify(Functions.getUserName(messageListItem.userInformation), font.pixelSize) : qsTr("You")) + "</a> " + Emoji.emojify(Functions.getMessageText(messageListItem.myMessage, false, messageListItem.isOwnMessage, false), font.pixelSize)
        textFormat: Text.RichText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        onLinkActivated: {
            Functions.handleLink(link);
        }
    }
}
