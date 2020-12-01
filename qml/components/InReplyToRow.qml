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

Row {
    id: inReplyToRow
    spacing: Theme.paddingSmall
    width: parent.width
    height: inReplyToMessageColumn.height

    property string myUserId;
    property var inReplyToMessage;
    property bool editable: false;

    signal clearRequested()

    onInReplyToMessageChanged: {
        if (inReplyToMessage) {
            inReplyToUserText.text = (inReplyToRow.inReplyToMessage.sender_user_id !== inReplyToRow.myUserId) ? Emoji.emojify(Functions.getUserName(tdLibWrapper.getUserInformation(inReplyToRow.inReplyToMessage.sender_user_id)), inReplyToUserText.font.pixelSize) : qsTr("You");
            inReplyToMessageText.text = Emoji.emojify(Functions.getMessageText(inReplyToRow.inReplyToMessage, true, inReplyToRow.inReplyToMessage.sender_user_id === inReplyToRow.myUserId, false), inReplyToMessageText.font.pixelSize);
        }
    }

    Rectangle {
        id: inReplyToMessageRectangle
        height: inReplyToMessageColumn.height
        width: Theme.paddingSmall
        color: Theme.secondaryHighlightColor
        border.width: 0
    }

    Row {
        width: parent.width - Theme.paddingSmall - inReplyToMessageRectangle.width
        spacing: Theme.paddingSmall

        Column {
            id: inReplyToMessageColumn
            spacing: Theme.paddingSmall
            width: parent.width - ( inReplyToRow.editable ? ( Theme.paddingSmall + removeInReplyToIconButton.width ) : 0 )

            Label {
                id: inReplyToUserText

                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                font.weight: Font.ExtraBold
                maximumLineCount: 1
                truncationMode: TruncationMode.Fade
                textFormat: Text.StyledText
                horizontalAlignment: Text.AlignLeft
            }

            Label {
                id: inReplyToMessageText
                font.pixelSize: Theme.fontSizeExtraSmall
                width: parent.width
                textFormat: Text.StyledText
                truncationMode: TruncationMode.Fade
                maximumLineCount: 1
            }
        }

        IconButton {
            id: removeInReplyToIconButton
            icon.source: "image://theme/icon-m-clear"
            visible: inReplyToRow.editable
            onClicked: {
                inReplyToRow.clearRequested();
            }
        }
    }

}
