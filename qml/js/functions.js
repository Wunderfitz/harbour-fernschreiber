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

function getSimpleMessageText(message) {
    if (message.content['@type'] === 'messageText') {
        return message.content.text.text;
    }
    if (message.content['@type'] === 'messageSticker') {
        return qsTr("Sticker: %1").arg(message.content.sticker.emoji);
    }
    if (message.content['@type'] === 'messagePhoto') {
        return (message.content.caption.text !== "") ? qsTr("Picture: %1").arg(message.content.caption.text) : qsTr("shared a picture");
    }
    if (message.content['@type'] === 'messageVideo') {
        return (message.content.caption.text !== "") ? qsTr("Video: %1").arg(message.content.caption.text) : qsTr("shared a video");
    }
    if (message.content['@type'] === 'messageAudio') {
        return (message.content.caption.text !== "") ? qsTr("Audio: %1").arg(message.content.caption.text) : qsTr("shared an audio");
    }
    if (message.content['@type'] === 'messageVoiceNote') {
        return (message.content.caption.text !== "") ? qsTr("Voice Note: %1").arg(message.content.caption.text) : qsTr("shared a voice note");
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
