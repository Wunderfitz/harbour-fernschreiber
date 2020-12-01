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

.pragma library
.import "debug.js" as Debug
.import Sailfish.Silica 1.0 as Silica

var tdLibWrapper;
var appNotification;
function setGlobals(globals) {
    tdLibWrapper = globals.tdLibWrapper;
    appNotification = globals.appNotification;
}

function getUserName(userInformation) {
    var firstName = typeof userInformation.first_name !== "undefined" ? userInformation.first_name : "";
    var lastName = typeof userInformation.last_name !== "undefined" ? userInformation.last_name : "";
    return (firstName + " " + lastName).trim();
}

function getMessageText(message, simple, myself, ignoreEntities) {
    if (message.content['@type'] === 'messageText') {
        if (simple) {
            return message.content.text.text;
        } else {
            return enhanceMessageText(message.content.text, ignoreEntities);
        }
    }
    if (message.content['@type'] === 'messageSticker') {
        return simple ? qsTr("Sticker: %1").arg(message.content.sticker.emoji) : "";
    }
    if (message.content['@type'] === 'messagePhoto') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Picture: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent a picture", "myself") : qsTr("sent a picture")) : "";
        }
    }
    if (message.content['@type'] === 'messageVideo') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Video: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent a video", "myself") : qsTr("sent a video")) : "";
        }
    }
    if (message.content['@type'] === 'messageVideoNote') {
        return simple ? (myself ? qsTr("sent a video note", "myself") : qsTr("sent a video note")) : "";
    }
    if (message.content['@type'] === 'messageAnimation') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Animation: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent an animation", "myself") : qsTr("sent an animation")) : "";
        }
    }
    if (message.content['@type'] === 'messageAudio') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Audio: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent an audio", "myself") : qsTr("sent an audio")) : "";
        }
    }
    if (message.content['@type'] === 'messageVoiceNote') {
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Voice Note: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent a voice note", "myself") : qsTr("sent a voice note")) : "";
        }
    }
    if (message.content['@type'] === 'messageDocument') {
        if (message.content.document.file_name !== "") {
            return simple ? qsTr("Document: %1").arg(message.content.document.file_name) : (message.content.document.file_name + ( message.content.caption.text !== "" ? ("<br />" + enhanceMessageText(message.content.caption, ignoreEntities) ) : "")).trim();
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
    if (message.content['@type'] === 'messageChatChangeTitle') {
        return myself ? qsTr("changed the chat title to %1", "myself").arg(message.content.title) : qsTr("changed the chat title to %1").arg(message.content.title);
    }
    if (message.content['@type'] === 'messagePoll') {
        if (message.content.poll.type['@type'] === "pollTypeQuiz") {
            if (message.content.poll.is_anonymous) {
                return simple ? (myself ? qsTr("sent an anonymous quiz", "myself") : qsTr("sent an anonymous quiz")) : ("<b>" + qsTr("Anonymous Quiz") + "</b>");
            }
            return simple ? (myself ? qsTr("sent a quiz", "myself") : qsTr("sent a quiz")) : ("<b>" + qsTr("Quiz") + "</b>");
        }
        if (message.content.poll.is_anonymous) {
            return simple ? (myself ? qsTr("sent an anonymous poll", "myself") : qsTr("sent an anonymous poll")) : ("<b>" + qsTr("Anonymous Poll") + "</b>");
        }
        return simple ? (myself ? qsTr("sent a poll", "myself") : qsTr("sent a poll")) : ("<b>" + qsTr("Poll") + "</b>");
    }
    if (message.content['@type'] === 'messageBasicGroupChatCreate' || message.content['@type'] === 'messageSupergroupChatCreate') {
        return myself ? qsTr("created this group", "myself") : qsTr("created this group");
    }
    if (message.content['@type'] === 'messageChatChangePhoto') {
        return myself ? qsTr("changed the chat photo", "myself") : qsTr("changed the chat photo");
    }
    if (message.content['@type'] === 'messageChatDeletePhoto') {
        return myself ? qsTr("deleted the chat photo", "myself") : qsTr("deleted the chat photo");
    }
    if (message.content['@type'] === 'messageChatSetTtl') {
        return myself ? qsTr("changed the secret chat TTL setting", "myself; TTL = Time To Live") : qsTr("changed the secret chat TTL setting", "TTL = Time To Live");
    }

    if (message.content['@type'] === 'messageChatUpgradeFrom' || message.content['@type'] === 'messageChatUpgradeTo' ) {
        return myself ? qsTr("upgraded this group to a supergroup", "myself") : qsTr("upgraded this group to a supergroup");
    }
    if (message.content['@type'] === 'messageCustomServiceAction') {
        return message.content.text;
    }
    if (message.content['@type'] === 'messagePinMessage') {
        return myself ? qsTr("changed the pinned message", "myself") : qsTr("changed the pinned message");
    }
    if (message.content['@type'] === 'messageExpiredPhoto') {
        return myself ? qsTr("sent a self-destructing photo that is expired", "myself") : qsTr("sent a self-destructing photo that is expired");
    }
    if (message.content['@type'] === 'messageExpiredVideo') {
        return myself ? qsTr("sent a self-destructing video that is expired", "myself") : qsTr("sent a self-destructing video that is expired");
    }
    if (message.content['@type'] === 'messageScreenshotTaken') {
        return myself ? qsTr("created a screenshot in this chat", "myself") : qsTr("created a screenshot in this chat");
    }
    if (message.content['@type'] === 'messageUnsupported') {
        return myself ? qsTr("sent an unsupported message", "myself") : qsTr("sent an unsupported message");
    }

    return myself ? qsTr("sent an unsupported message: %1", "myself; %1 is message type").arg(message.content['@type'].substring(7)) : qsTr("sent an unsupported message: %1", "%1 is message type").arg(message.content['@type'].substring(7));
}

function getChatPartnerStatusText(statusType, was_online) {
    switch(statusType) {
    case "userStatusEmpty":
        return qsTr("never online");
    case "userStatusLastMonth":
        return qsTr("last month");
    case "userStatusLastWeek":
        return qsTr("last week");
    case "userStatusOffline":
        return (%1) (getDateTimeElapsed(was_online));
    case "userStatusOnline":
        return qsTr("online");
    case "userStatusRecently":
        return qsTr("offline, was recently online");
    }
}

function getSecretChatStatus(secretChatDetails) {
    switch (secretChatDetails.state["@type"]) {
    case "secretChatStateClosed":
        return "<b>" + qsTr("Closed!") + "</b>";
    case "secretChatStatePending":
        return qsTr("Pending acknowledgement");
    case "secretChatStateReady":
        return "";
    }
}

function getChatMemberStatusText(statusType) {
//    chatMemberStatusAdministrator, chatMemberStatusBanned, chatMemberStatusCreator, chatMemberStatusLeft, chatMemberStatusMember, and chatMemberStatusRestricted.
    switch(statusType) {
    case "chatMemberStatusAdministrator":
        return qsTr("Admin", "channel user role");
    case "chatMemberStatusBanned":
        return qsTr("Banned", "channel user role");
    case "chatMemberStatusCreator":
        return qsTr("Creator", "channel user role");
    case "chatMemberStatusRestricted":
        return qsTr("Restricted", "channel user role");
    case "chatMemberStatusLeft":
    case "chatMemberStatusMember":
        return ""
    }
    return statusType || "";
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
    return Silica.Format.formatDate(new Date(timestamp * 1000), Silica.Formatter.DurationElapsed);
}

function getDateTimeTranslated(timestamp) {
    return new Date(timestamp * 1000).toLocaleString();
}

function MessageInsertion(offset, insertionString, removeLength) {
    this.offset = offset;
    this.insertionString = insertionString;
    this.removeLength = removeLength;
}

MessageInsertion.prototype.toString = function insertionToString() {
    return "Offset: " + this.offset + ", Insertion String: " + this.insertionString + ", Remove Length: " + this.removeLength;
}

function handleHtmlEntity(messageText, messageInsertions, originalString, replacementString) {
    var continueSearch = true;
    var fromIndex = 0;
    var nextIndex = 0;
    while (continueSearch) {
        nextIndex = messageText.indexOf(originalString, fromIndex);
        if (nextIndex < 0) {
            continueSearch = false;
        } else {
            messageInsertions.push(new MessageInsertion(nextIndex, replacementString, originalString.length));
            fromIndex = nextIndex + 1;
        }
    }
}

function enhanceHtmlEntities(simpleText) {

    var messageInsertions = [];
    var messageText = simpleText;

    handleHtmlEntity(messageText, messageInsertions, "&", "&amp;");
    handleHtmlEntity(messageText, messageInsertions, "<", "&lt;");
    handleHtmlEntity(messageText, messageInsertions, ">", "&gt;");

    messageInsertions.sort( function(a, b) { return (b.offset+b.removeLength) - (a.offset+a.removeLength) } );

    for (var z = 0; z < messageInsertions.length; z++) {
        messageText = messageText.substring(0, messageInsertions[z].offset) + messageInsertions[z].insertionString + messageText.substring(messageInsertions[z].offset + messageInsertions[z].removeLength);
    }

    return messageText;

}

function enhanceMessageText(formattedText, ignoreEntities) {

    var messageInsertions = [];
    var messageText = formattedText.text;

    if (ignoreEntities) {
        return messageText;
    }

    handleHtmlEntity(messageText, messageInsertions, "&", "&amp;");
    handleHtmlEntity(messageText, messageInsertions, "<", "&lt;");
    handleHtmlEntity(messageText, messageInsertions, ">", "&gt;");

    for (var i = 0; i < formattedText.entities.length; i++) {
        if (formattedText.entities[i]['@type'] !== "textEntity") {
            continue;
        }
        var entityType = formattedText.entities[i].type['@type'];
        if (entityType === "textEntityTypeBold") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<b>", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</b>", 0 ));
        }
        if (entityType === "textEntityTypeUrl") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>", 0 ));
        }
        if (entityType === "textEntityTypeCode") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<pre>", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</pre>", 0 ));
        }
        if (entityType === "textEntityTypeEmailAddress") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"mailto:" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>", 0 ));
        }
        if (entityType === "textEntityTypeItalic") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<i>", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</i>", 0 ));
        }
        if (entityType === "textEntityTypeMention") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"user://" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>", 0 ));
        }
        if (entityType === "textEntityTypeMentionName") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"userId://" + formattedText.entities[i].type.user_id + "\">", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>", 0 ));
        }
        if (entityType === "textEntityTypePhoneNumber") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"tel:" + messageText.substring(formattedText.entities[i].offset, ( formattedText.entities[i].offset + formattedText.entities[i].length )) + "\">", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>", 0 ));
        }
        if (entityType === "textEntityTypePre") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<pre>", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</pre>", 0 ));
        }
        if (entityType === "textEntityTypePreCode") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<pre>", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</pre>", 0 ));
        }
        if (entityType === "textEntityTypeTextUrl") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<a href=\"" + formattedText.entities[i].type.url + "\">", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</a>", 0 ));
        }
        if (entityType === "textEntityTypeUnderline") {
            messageInsertions.push(new MessageInsertion(formattedText.entities[i].offset, "<u>", 0 ));
            messageInsertions.push(new MessageInsertion((formattedText.entities[i].offset + formattedText.entities[i].length), "</u>", 0 ));
        }
    }

    messageInsertions.sort( function(a, b) { return (b.offset+b.removeLength) - (a.offset+a.removeLength) } );

    for (var z = 0; z < messageInsertions.length; z++) {
        messageText = messageText.substring(0, messageInsertions[z].offset) + messageInsertions[z].insertionString + messageText.substring(messageInsertions[z].offset + messageInsertions[z].removeLength);
    }

    messageText = messageText.replace(new RegExp("\r?\n", "g"), "<br>");

    return messageText;

}

function handleLink(link) {
    var tMePrefix = tdLibWrapper.getOptionString("t_me_url");
    if (link.indexOf("user://") === 0) {
        var userName = link.substring(8);
        var userInformation = tdLibWrapper.getUserInformationByName(userName);
        if (typeof userInformation.id === "undefined") {
            appNotification.show(qsTr("Unable to find user %1").arg(userName));
        } else {
            tdLibWrapper.createPrivateChat(userInformation.id);
        }
    } else if (link.indexOf("userId://") === 0) {
        tdLibWrapper.createPrivateChat(link.substring(9));
    } else if (link.indexOf("tg://") === 0) {
        Debug.log("Special TG link: ", link);
        if (link.indexOf("tg://join?invite=") === 0) {
            tdLibWrapper.joinChatByInviteLink(tMePrefix + "joinchat/" + link.substring(17));
        } else if (link.indexOf("tg://resolve?domain=") === 0) {
            tdLibWrapper.searchPublicChat(link.substring(20));
        }
    }  else {
        if (link.indexOf(tMePrefix) === 0) {
            if (link.indexOf("joinchat") !== -1) {
                Debug.log("Joining Chat: ", link);
                tdLibWrapper.joinChatByInviteLink(link);
                // Do the necessary stuff to open the chat if successful
                // Fail with nice error message if it doesn't work
            } else {
                Debug.log("Search public chat: ", link.substring(tMePrefix.length));
                tdLibWrapper.searchPublicChat(link.substring(tMePrefix.length));
                // Check responses for updateBasicGroup or updateSupergroup
                // Fire createBasicGroupChat or createSupergroupChat
                // Do the necessary stuff to open the chat
                // Fail with nice error message if chat can't be found
            }
        } else {
            Qt.openUrlExternally(link);
        }
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
function replaceUrlsWithLinks(string) {
    return string.replace(/((\w+):\/\/[\w?=&.\/-;#~%-]+(?![\w\s?&.\/;#~%"=-]*>))/g, "<a href=\"$1\">$1</a>");
}

function sortMessagesArrayByDate(messages) {
    messages.sort(function(a, b) {
      return a.date - b.date;
    });
}

function getMessagesArrayIds(messages) {
    sortMessagesArrayByDate(messages);
    return messages.map(function(message){return message.id.toString()});
}

function getMessagesArrayText(messages) {
    sortMessagesArrayByDate(messages);
    var lastSenderName = "";
    var lines = [];
    for(var i = 0; i < messages.length; i += 1) {
        var senderName = getUserName(tdLibWrapper.getUserInformation(messages[i].sender_user_id));
        if(senderName !== lastSenderName) {
            lines.push(senderName);
        }
        lastSenderName = senderName;
        lines.push(getMessageText(messages[i], true, false));
        lines.push("");
    }
    return lines.join("\n");
}

function handleErrorMessage(code, message) {
    if (code === 404) {
        // Silently ignore 404 Not Found messages (occur sometimes, without clear context...)
        return;
    }
    if (message === "USER_ALREADY_PARTICIPANT") {
        appNotification.show(qsTr("You are already a member of this chat."));
    } else {
        appNotification.show(message);
    }
}

function getMessagesNeededForwardPermissions(messages) {
    var neededPermissions = ["can_send_messages"]

    var mediaMessageTypes = ["messageAudio", "messageDocument", "messagePhoto", "messageVideo", "messageVideoNote", "messageVoiceNote"]
    var otherMessageTypes = ["messageAnimation", "messageGame", "messageSticker"]
    for(var i = 0; i < messages.length && neededPermissions.length < 3; i += 1) {
        var type = messages[i]["content"]["@type"]
        var permission = ""
        if(type === "messageText") {
            continue
        } else if(type === "messagePoll") {
            permission = "can_send_polls"
        } else if(mediaMessageTypes.indexOf(type) > -1) {
            permission = "can_send_media_messages"
        } else if(otherMessageTypes.indexOf(type) > -1) {
            permission = "can_send_other_messages"
        }

        if(permission !== "" && neededPermissions.indexOf(permission) === -1) {
            neededPermissions.push(permission)
        }
    }
    return neededPermissions
}
