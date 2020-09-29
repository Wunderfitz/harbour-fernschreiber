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

function getMessageText(message, simple, myself) {
    if (message.content['@type'] === 'messageText') {
        if (simple) {
            return message.content.text.text;
        } else {
            return enhanceMessageText(message.content.text);
        }
    }
    if (message.content['@type'] === 'messageSticker') {
        return simple ? qsTr("Sticker: %1").arg(message.content.sticker.emoji) : "";
    }
    if (message.content['@type'] === 'messagePhoto') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Picture: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption)
        } else {
            return simple ? (myself ? qsTr("sent a picture", "myself") : qsTr("sent a picture")) : "";
        }
    }
    if (message.content['@type'] === 'messageVideo') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Video: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption)
        } else {
            return simple ? (myself ? qsTr("sent a video", "myself") : qsTr("sent a video")) : "";
        }
    }
    if (message.content['@type'] === 'messageAnimation') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Animation: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption)
        } else {
            return simple ? (myself ? qsTr("sent an animation", "myself") : qsTr("sent an animation")) : "";
        }
    }
    if (message.content['@type'] === 'messageAudio') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Audio: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption)
        } else {
            return simple ? (myself ? qsTr("sent an audio", "myself") : qsTr("sent an audio")) : "";
        }
    }
    if (message.content['@type'] === 'messageVoiceNote') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Voice Note: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption)
        } else {
            return simple ? (myself ? qsTr("sent a voice note", "myself") : qsTr("sent a voice note")) : "";
        }
    }
    if (message.content['@type'] === 'messageDocument') {
        if (message.content.document.file_name !== "") {
            return simple ? qsTr("Document: %1").arg(message.content.document.file_name) : message.content.document.file_name
        } else {
            return simple ? (myself ? qsTr("sent a document", "myself") : qsTr("sent a document")) : "";
        }
    }
    if (message.content['@type'] === 'messageLocation') {
        return simple ? (myself ? qsTr("sent a location", "myself") : qsTr("sent a location")) : "";
    }
    if (message.content['@type'] === 'messageVenue') {
        return simple ? (myself ? qsTr("sent a venue", "myself") : qsTr("sent a venue")) : ( "<b>" + message.content.venue.title + "</b>, " + message.content.venue.address );
    }
    if (message.content['@type'] === 'messageContactRegistered') {
        return myself ? qsTr("have registered with Telegram") : qsTr("has registered with Telegram");
    }
    if (message.content['@type'] === 'messageChatJoinByLink') {
        return myself ? qsTr("joined this chat", "myself") : qsTr("joined this chat");
    }
    if (message.content['@type'] === 'messageChatAddMembers') {
        return myself ? qsTr("were added to this chat", "myself") : qsTr("was added to this chat");
    }
    if (message.content['@type'] === 'messageChatDeleteMember') {
        return myself ? qsTr("left this chat", "myself") : qsTr("left this chat");
    }
    return qsTr("Unsupported message: %1").arg(message.content['@type'].substring(7));
}
function getShortenedCount(count) {
    if (count >= 1000000) {
        return qsTr("%1M").arg((count / 1000000).toLocaleString(Qt.locale(), 'f', 0));
    } else if (count >= 1000 ) {
        return qsTr("%1K").arg((count / 1000).toLocaleString(Qt.locale(), 'f', 0));
    } else {
        return count;
    }
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
    messageText = messageText.replace("<", "&lt;");
    messageText = messageText.replace(">", "&gt;");
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
        if (entityType === "textEntityTypeUrl") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>" ));
        }
        if (entityType === "textEntityTypeCode") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<pre>" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</pre>" ));
        }
        if (entityType === "textEntityTypeEmailAddress") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"mailto:" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>" ));
        }
        if (entityType === "textEntityTypeItalic") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<i>" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</i>" ));
        }
        if (entityType === "textEntityTypeMention") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"user:" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>" ));
        }
        if (entityType === "textEntityTypeMentionName") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"userId://" + formattedText.entities[i].type.user_id + "\">" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>" ));
        }
        if (entityType === "textEntityTypePhoneNumber") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"tel:" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>" ));
        }
        if (entityType === "textEntityTypePre") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<pre>" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</pre>" ));
        }
        if (entityType === "textEntityTypePreCode") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<pre>" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</pre>" ));
        }
        if (entityType === "textEntityTypeTextUrl") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"" + formattedText.entities[i].type.url + "\">" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>" ));
        }
        if (entityType === "textEntityTypeUnderline") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<u>" ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</u>" ));
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

function handleLink(link) {
    if (link.indexOf("user://") === 0) {
        //pageStack.push(Qt.resolvedUrl("../pages/UserPage.qml"), {"userName": link.substring(7)});
    } else if (link.indexOf("userid://") === 0) {
        //pageStack.push(Qt.resolvedUrl("../pages/UserPage.qml"), {"userId": link.substring(9)});
    }  else {
        Qt.openUrlExternally(link);
    }
}

function getVideoHeight(videoWidth, videoData) {
    if (typeof videoData !== "undefined") {
        if (videoData.height === 0) {
            return videoWidth;
        } else {
            var aspectRatio = videoData.height / videoData.width;
            return Math.round(videoWidth * aspectRatio);
        }
    } else {
        return 1;
    }
}
