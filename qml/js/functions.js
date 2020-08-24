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

function getUserName(userInformation) {
    var firstName = typeof userInformation.first_name !== "undefined" ? userInformation.first_name : "";
    var lastName = typeof userInformation.last_name !== "undefined" ? userInformation.last_name : "";
    return (firstName + " " + lastName).trim();
}

function getMessageText(message, simple) {
    if (message.content['@type'] === 'messageText') {
        if (simple) {
            return message.content.text.text;
        } else {
            return enhanceMessageText(message.content.text);
        }
    }
    if (message.content['@type'] === 'messageSticker') {
        return qsTr("Sticker: %1").arg(message.content.sticker.emoji);
    }
    if (message.content['@type'] === 'messagePhoto') {
        if (message.content.caption.text !== "") {
            return qsTr("Picture: %1").arg(simple ? message.content.caption.text : enhanceMessageText(message.content.caption))
        } else {
            return qsTr("shared a picture");
        }
    }
    if (message.content['@type'] === 'messageVideo') {
        if (message.content.caption.text !== "") {
            return qsTr("Video: %1").arg(simple ? message.content.caption.text : enhanceMessageText(message.content.caption))
        } else {
            return qsTr("shared a video");
        }
    }
    if (message.content['@type'] === 'messageAudio') {
        if (message.content.caption.text !== "") {
            return qsTr("Audio: %1").arg(simple ? message.content.caption.text : enhanceMessageText(message.content.caption))
        } else {
            return qsTr("shared an audio");
        }
    }
    if (message.content['@type'] === 'messageVoiceNote') {
        if (message.content.caption.text !== "") {
            return qsTr("Voice Note: %1").arg(simple ? message.content.caption.text : enhanceMessageText(message.content.caption))
        } else {
            return qsTr("shared a voice note");
        }
    }
    if (message.content['@type'] === 'messageLocation') {
        return qsTr("shared their location");
    }
    if (message.content['@type'] === 'messageContactRegistered') {
        return qsTr("has registered with Telegram");
    }
    if (message.content['@type'] === 'messageChatJoinByLink') {
        return qsTr("joined this chat.");
    }
    if (message.content['@type'] === 'messageChatAddMembers') {
        return qsTr("was added to this chat.");
    }
    if (message.content['@type'] === 'messageChatDeleteMember') {
        return qsTr("left this chat.");
    }
    return "?";
}

function getDateTimeElapsed(timestamp) {
    return Format.formatDate(new Date(timestamp * 1000), Formatter.DurationElapsed);
}

function MessageInsertion(offset, insertionString) {
    this.offset = offset;
    this.insertionString = insertionString;
}

MessageInsertion.prototype.toString = function insertionToString() {
    return "Offset: " + this.offset + ", Insertion String: " + this.insertionString;
}

function enhanceMessageText(formattedText) {

    var messageText = formattedText.text;
    var messageInsertions = [];

    for (var i = 0; i < formattedText.entities.length; i++) {
        if (formattedText.entities[i]['@type'] !== "textEntity") {
            continue;
        }
        var entityType = formattedText.entities[i].type['@type'];
        if (entityType === "textEntityTypeBold") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<b>" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</b>" ));
        }
    }

    messageInsertions.sort( function(a, b) { return b.offset - a.offset } );

    for (var z = 0; z < messageInsertions.length; z++) {
        messageText = messageText.substring(0, messageInsertions[z].offset) + messageInsertions[z].insertionString + messageText.substring(messageInsertions[z].offset);
    }

    messageText = messageText.replace(new RegExp("\r?\n", "g"), "<br>");

    var spaceRegex = /\s{2,}/g;
    function spaceReplacer(match, p1, offset, string) {
        var replaceString = "";
        for (var i = 0; i < match.length; i++) {
            replaceString += "&nbsp;";
        }
        return replaceString;
    }
    messageText = messageText.replace(spaceRegex, spaceReplacer);

    return messageText;

}
