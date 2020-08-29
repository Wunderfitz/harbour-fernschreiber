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
import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0
import "../components"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Row {
    id: inReplyToRow
    spacing: Theme.paddingSmall
    width: parent.width

    property string myUserId;
    property variant inReplyToMessage: "";

    onInReplyToMessageChanged: {
        if (typeof inReplyToRow.inReplyToMessage === "object") {
            inReplyToUserText.text = (inReplyToRow.inReplyToMessage.sender_user_id !== inReplyToRow.myUserId) ? Emoji.emojify(Functions.getUserName(tdLibWrapper.getUserInformation(inReplyToRow.inReplyToMessage.sender_user_id)), inReplyToUserText.font.pixelSize) : qsTr("You");
            inReplyToMessageText.text = Emoji.emojify(Functions.getMessageText(inReplyToRow.inReplyToMessage, true), inReplyToMessageText.font.pixelSize);
        } else {
            inReplyToUserText.text = "";
            inReplyToMessageText.text = "";
        }
    }

    Rectangle {
        id: inReplyToMessageRectangle
        height: inReplyToMessageColumn.height
        width: Theme.paddingSmall
        color: Theme.secondaryHighlightColor
        border.width: 0
    }

    Column {
        id: inReplyToMessageColumn
        spacing: Theme.paddingSmall
        width: parent.width - Theme.paddingSmall - inReplyToMessageRectangle.width

        Text {
            id: inReplyToUserText

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
            id: inReplyToMessageText
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            width: parent.width
            elide: Text.ElideRight
            textFormat: Text.StyledText
            onTruncatedChanged: {
                // There is obviously a bug in QML in truncating text with images.
                // We simply remove Emojis then...
                if (truncated) {
                    text = text.replace(/\<img [^>]+\/\>/g, "");
                }
            }
        }
    }

}
