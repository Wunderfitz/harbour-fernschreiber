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
    return ((userInformation.first_name || "") + " " + (userInformation.last_name || "")).trim();
}

function getMessageText(message, simple, currentUserId, ignoreEntities) {

    var myself = ( message.sender['@type'] === "messageSenderUser" && message.sender.user_id.toString() === currentUserId.toString() );

    switch(message.content['@type']) {
    case 'messageText':
        if (simple) {
            return message.content.text.text;
        } else {
            return enhanceMessageText(message.content.text, ignoreEntities);
        }
    case 'messageSticker':
        return simple ? qsTr("Sticker: %1").arg(message.content.sticker.emoji) : "";
    case 'messagePhoto':
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Picture: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent a picture", "myself") : qsTr("sent a picture")) : "";
        }
    case 'messageVideo':
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Video: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent a video", "myself") : qsTr("sent a video")) : "";
        }
    case 'messageVideoNote':
        return simple ? (myself ? qsTr("sent a video note", "myself") : qsTr("sent a video note")) : "";
    case 'messageAnimation':
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Animation: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent an animation", "myself") : qsTr("sent an animation")) : "";
        }
    case 'messageAudio':
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Audio: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent an audio", "myself") : qsTr("sent an audio")) : "";
        }
    case 'messageVoiceNote':
        if (message.content.caption.text !== "") {
            return simple ? qsTr("Voice Note: %1").arg(message.content.caption.text) : enhanceMessageText(message.content.caption, ignoreEntities)
        } else {
            return simple ? (myself ? qsTr("sent a voice note", "myself") : qsTr("sent a voice note")) : "";
        }
    case 'messageDocument':
        if (message.content.document.file_name !== "") {
            return simple ? qsTr("Document: %1").arg(message.content.document.file_name) : (message.content.document.file_name + ( message.content.caption.text !== "" ? ("<br />" + enhanceMessageText(message.content.caption, ignoreEntities) ) : "")).trim();
        } else {
            return simple ? (myself ? qsTr("sent a document", "myself") : qsTr("sent a document")) : "";
        }
    case 'messageLocation':
        return simple ? (myself ? qsTr("sent a location", "myself") : qsTr("sent a location")) : "";
    case 'messageVenue':
        return simple ? (myself ? qsTr("sent a venue", "myself") : qsTr("sent a venue")) : ( "<b>" + message.content.venue.title + "</b>, " + message.content.venue.address );
    case 'messageContactRegistered':
        return myself ? qsTr("have registered with Telegram") : qsTr("has registered with Telegram");
    case 'messageChatJoinByLink':
        return myself ? qsTr("joined this chat", "myself") : qsTr("joined this chat");
    case 'messageChatAddMembers':
        if (message.sender['@type'] === "messageSenderUser" && message.sender.user_id === message.content.member_user_ids[0]) {
            return myself ? qsTr("were added to this chat", "myself") : qsTr("was added to this chat");
        } else {
            var addedUserNames = "";
            for (var i = 0; i < message.content.member_user_ids.length; i++) {
                if (i > 0) {
                    addedUserNames += ", ";
                }
                addedUserNames += getUserName(tdLibWrapper.getUserInformation(message.content.member_user_ids[i]));
            }
            return myself ? qsTr("have added %1 to the chat", "myself").arg(addedUserNames) : qsTr("has added %1 to the chat").arg(addedUserNames);
        }
    case 'messageChatDeleteMember':
        if (message.sender['@type'] === "messageSenderUser" && message.sender.user_id === message.content.user_id) {
            return myself ? qsTr("left this chat", "myself") : qsTr("left this chat");
        } else {
            return myself ? qsTr("have removed %1 from the chat", "myself").arg(getUserName(tdLibWrapper.getUserInformation(message.content.user_id))) : qsTr("has removed %1 from the chat").arg(getUserName(tdLibWrapper.getUserInformation(message.content.user_id)));
        }
    case 'messageChatChangeTitle':
        return myself ? qsTr("changed the chat title to %1", "myself").arg(message.content.title) : qsTr("changed the chat title to %1").arg(message.content.title);
    case 'messagePoll':
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
    case 'messageBasicGroupChatCreate':
    case 'messageSupergroupChatCreate':
        return myself ? qsTr("created this group", "myself") : qsTr("created this group");
    case 'messageChatChangePhoto':
        return myself ? qsTr("changed the chat photo", "myself") : qsTr("changed the chat photo");
    case 'messageChatDeletePhoto':
        return myself ? qsTr("deleted the chat photo", "myself") : qsTr("deleted the chat photo");
    case 'messageChatSetTtl':
        return myself ? qsTr("changed the secret chat TTL setting", "myself; TTL = Time To Live") : qsTr("changed the secret chat TTL setting", "TTL = Time To Live");
    case 'messageChatUpgradeFrom':
    case 'messageChatUpgradeTo':
        return myself ? qsTr("upgraded this group to a supergroup", "myself") : qsTr("upgraded this group to a supergroup");
    case 'messageCustomServiceAction':
        return message.content.text;
    case 'messagePinMessage':
        return myself ? qsTr("changed the pinned message", "myself") : qsTr("changed the pinned message");
    case 'messageExpiredPhoto':
        return myself ? qsTr("sent a self-destructing photo that is expired", "myself") : qsTr("sent a self-destructing photo that is expired");
    case 'messageExpiredVideo':
        return myself ? qsTr("sent a self-destructing video that is expired", "myself") : qsTr("sent a self-destructing video that is expired");
    case 'messageScreenshotTaken':
        return myself ? qsTr("created a screenshot in this chat", "myself") : qsTr("created a screenshot in this chat");
    case 'messageGame':
        return simple ? (myself ? qsTr("sent a game", "myself") : qsTr("sent a game")) : "";
    case 'messageGameScore':
        return myself ? qsTr("scored %Ln points", "myself", message.content.score) : qsTr("scored %Ln points", "myself", message.content.score);
    case 'messageUnsupported':
        return myself ? qsTr("sent an unsupported message", "myself") : qsTr("sent an unsupported message");
    default:
        return myself ? qsTr("sent an unsupported message: %1", "myself; %1 is message type").arg(message.content['@type'].substring(7)) : qsTr("sent an unsupported message: %1", "%1 is message type").arg(message.content['@type'].substring(7));
    }
}

function getChatPartnerStatusText(statusType, was_online) {
    switch(statusType) {
    case "userStatusEmpty":
        return qsTr("was never online");
    case "userStatusLastMonth":
        return qsTr("last online: last month");
    case "userStatusLastWeek":
        return qsTr("last online: last week");
    case "userStatusOffline":
        return qsTr("last online: %1").arg(getDateTimeElapsed(was_online));
    case "userStatusOnline":
        return qsTr("online");
    case "userStatusRecently":
        return qsTr("was recently online");
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

function handleHtmlEntity(messageText, messageInsertions, originalString, replacementString) {
    var nextIndex = -1;
    while ((nextIndex = messageText.indexOf(originalString, nextIndex + 1)) > -1) {
        messageInsertions.push({ offset: nextIndex, insertionString: replacementString, removeLength: originalString.length });
    }
}

var rawNewLineRegExp = /\r?\n/g;
var ampRegExp = /&/g;
var ltRegExp = /</g;
var gtRegExp = />/g;

function enhanceHtmlEntities(simpleText) {
    return simpleText.replace(ampRegExp, "&amp;").replace(ltRegExp, "&lt;").replace(gtRegExp, "&gt;");//.replace(rawNewLineRegExp, "<br>");
}

function messageInsertionSorter(a, b) { return (b.offset+b.removeLength) - (a.offset+a.removeLength) }

function enhanceMessageText(formattedText, ignoreEntities) {

    var messageInsertions = [];
    var messageText = formattedText.text;
    var entity;
    if (ignoreEntities) {
        return messageText;
    }
    if(formattedText.entities.length === 0) {
        return messageText.replace(ampRegExp, "&amp;").replace(ltRegExp, "&lt;").replace(gtRegExp, "&gt;").replace(rawNewLineRegExp, "<br>");
    }

    for (var i = 0; i < formattedText.entities.length; i++) {
        entity = formattedText.entities[i];
        if (entity['@type'] !== "textEntity") {
            continue;
        }
        switch(entity.type['@type']) {
            case "textEntityTypeBold":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<b>", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</b>", removeLength: 0 }
                );
            break;
            case "textEntityTypeUrl":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"" + messageText.substring(entity.offset, ( entity.offset + entity.length )) + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
            case "textEntityTypeCode":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<pre>", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</pre>", removeLength: 0 }
                );
            break;
            case "textEntityTypeEmailAddress":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"mailto:" + messageText.substring(entity.offset, ( entity.offset + entity.length )) + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
            case "textEntityTypeItalic":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<i>", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</i>", removeLength: 0 }
                );
            break;
            case "textEntityTypeMention":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"user://" + messageText.substring(entity.offset, ( entity.offset + entity.length )) + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
            case "textEntityTypeMentionName":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"userId://" + entity.type.user_id + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
            case "textEntityTypePhoneNumber":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"tel:" + messageText.substring(entity.offset, ( entity.offset + entity.length )) + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
            case "textEntityTypePre":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<pre>", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</pre>", removeLength: 0 }
                );
            break;
            case "textEntityTypePreCode":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<pre>", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</pre>", removeLength: 0 }
                );
            break;
            case "textEntityTypeTextUrl":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"" + entity.type.url + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
            case "textEntityTypeUnderline":
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<u>", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</u>", removeLength: 0 }
                );
            break;
            case "textEntityTypeBotCommand":
                var command = messageText.substring(entity.offset, entity.offset + entity.length);
                messageInsertions.push(
                    { offset: entity.offset, insertionString: "<a href=\"botCommand://" + command + "\">", removeLength: 0 },
                    { offset: (entity.offset + entity.length), insertionString: "</a>", removeLength: 0 }
                );
            break;
        }
    }

    if(messageInsertions.length === 0) {
        return messageText.replace(ampRegExp, "&amp;").replace(ltRegExp, "&lt;").replace(gtRegExp, "&gt;").replace(rawNewLineRegExp, "<br>");
    }

    handleHtmlEntity(messageText, messageInsertions, "&", "&amp;");
    handleHtmlEntity(messageText, messageInsertions, "<", "&lt;");
    handleHtmlEntity(messageText, messageInsertions, ">", "&gt;");
    messageInsertions.sort(messageInsertionSorter);

    for (var z = 0; z < messageInsertions.length; z++) {
        messageText = messageText.substring(0, messageInsertions[z].offset)
         + messageInsertions[z].insertionString
         + messageText.substring(messageInsertions[z].offset + messageInsertions[z].removeLength);
    }

    messageText = messageText.replace(rawNewLineRegExp, "<br>");

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
            tdLibWrapper.createPrivateChat(userInformation.id, "openDirectly");
        }
    } else if (link.indexOf("userId://") === 0) {
        tdLibWrapper.createPrivateChat(link.substring(9), "openDirectly");
    } else if (link.indexOf("tg://") === 0) {
        Debug.log("Special TG link: ", link);
        if (link.indexOf("tg://join?invite=") === 0) {
            tdLibWrapper.joinChatByInviteLink(tMePrefix + "joinchat/" + link.substring(17));
        } else if (link.indexOf("tg://resolve?domain=") === 0) {
            tdLibWrapper.searchPublicChat(link.substring(20), true);
        }
    } else if (link.indexOf("botCommand://") === 0) { // this gets returned to send on ChatPage
        return link.substring(13);
    } else {
        if (link.indexOf(tMePrefix) === 0) {
            if (link.indexOf("joinchat") !== -1) {
                Debug.log("Joining Chat: ", link);
                tdLibWrapper.joinChatByInviteLink(link);
                // Do the necessary stuff to open the chat if successful
                // Fail with nice error message if it doesn't work
            } else {
                Debug.log("Search public chat: ", link.substring(tMePrefix.length));
                tdLibWrapper.searchPublicChat(link.substring(tMePrefix.length), true);
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
        var senderName = getUserName(tdLibWrapper.getUserInformation(messages[i].sender.user_id));
        if(senderName !== lastSenderName) {
            lines.push(senderName);
        }
        lastSenderName = senderName;
        lines.push(getMessageText(messages[i], true, tdLibWrapper.getUserInformation().id, false));
        lines.push("");
    }
    return lines.join("\n");
}

function handleErrorMessage(code, message) {
    if (code === 404 || (code === 400 && message === "USERNAME_INVALID")) {
        // Silently ignore
        // - 404 Not Found messages (occur sometimes, without clear context...)
        // - searchPublicChat messages for "invalid" inline queries
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
