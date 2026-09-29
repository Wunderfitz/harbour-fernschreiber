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

Dialog {
    id: pollOptionPage
    allowedOrientations: Orientation.All

    property string chatId
    property string messageId
    property string pollQuestion

    canAccept: optionTextField.charactersLeft >= 0 && optionTextField.text.trim().length > 0

    DialogHeader {
        id: header
        dialog: pollOptionPage
        title: qsTr("Add an Option", "Dialog Header")
    }

    Label {
        id: subHeaderLabel
        anchors {
            verticalCenter: header.bottom
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        color: Theme.secondaryHighlightColor
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("to %1", "After dialog header… Add an Option to [poll question]").arg(Emoji.emojify(pollOptionPage.pollQuestion, font.pixelSize))
        font.pixelSize: Theme.fontSizeSmall
    }

    Column {
        anchors {
            top: subHeaderLabel.bottom
            topMargin: Theme.paddingLarge
            left: parent.left
            right: parent.right
        }

        TextField {
            id: optionTextField
            width: parent.width
            placeholderText: qsTr("Enter an answer here")
            property int charactersLeft: 100 - text.length
            color: charactersLeft < 0 ? Theme.errorColor : Theme.highlightColor
            label: qsTr("Answer (%Ln characters left)", "", charactersLeft)
            EnterKey.iconSource: "image://theme/icon-m-enter-close"
            EnterKey.onClicked: focus = false
            Component.onCompleted: forceActiveFocus()
        }

        Label {
            width: parent.width - Theme.horizontalPageMargin * 2
            x: Theme.horizontalPageMargin
            topPadding: Theme.paddingLarge
            color: Theme.secondaryHighlightColor
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: qsTr("Everybody in this chat can see who added which answer.")
        }
    }

    onAccepted: {
        tdLibWrapper.addPollOption(pollOptionPage.chatId, pollOptionPage.messageId, optionTextField.text.trim());
    }
}
