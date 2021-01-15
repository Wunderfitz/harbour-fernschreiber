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
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji


Loader {
    id: botUserLoader
    active: !!message.via_bot_user_id
    width: parent.width
    asynchronous: true
    sourceComponent: Label {
        property var botUserInformation: tdLibWrapper.getUserInformation(message.via_bot_user_id)
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        text: qsTr("via %1", "message posted via bot user").arg("<a style=\"text-decoration: none; font-weight: bold; color:"+Theme.primaryColor+"\" href=\"userId://" + message.via_bot_user_id + "\">@" + Emoji.emojify(botUserInformation.username, font.pixelSize)+"</a>")
        textFormat: Text.RichText
        truncationMode: TruncationMode.Fade
        onLinkActivated: {
            if(link === "userId://" + message.via_bot_user_id && botUserInformation.type.is_inline) {
                newMessageTextField.text = "@"+botUserInformation.username+" "
                newMessageTextField.cursorPosition = newMessageTextField.text.length
                lostFocusTimer.start();
            }
            else {
                Functions.handleLink(link);
            }
        }
    }
    property var message
}
