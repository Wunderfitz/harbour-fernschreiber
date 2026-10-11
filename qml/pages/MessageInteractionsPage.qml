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
import "../js/debug.js" as Debug

// Who read a message and who reacted to it with what. TDLib gives the read
// date of the other user in private chats, the viewers in groups and the
// added reactions wherever the message allows it, otherwise the few recent
// senders of each reaction - see Functions.canShowMessageInteractions() for
// when this page is offered.
Page {
    id: messageInteractionsPage
    allowedOrientations: Orientation.All

    property var message: ({})

    readonly property string extraPrefix: "messageInteractions:" + message.chat_id + ":" + message.id + ":"
    readonly property string myUserId: tdLibWrapper.getUserInformation().id.toString()

    property int pendingRequests: 0
    // Everybody who read or reacted, by sender, merged before being shown
    property var entries: ({})
    property int viewerCount: 0
    property int reactionCount: 0
    property bool loadingFailed: false
    // Only the first page of the added reactions is part of the initial load,
    // the others follow while scrolling towards the end of the reactions
    property bool modelFilled: false
    property bool loadingReactions: false
    property string nextReactionsOffset: ""
    // The rows with a reaction come first, those who only read it after them
    property int reactionRows: 0
    readonly property int rowHeight: Theme.itemSizeMedium

    function getKey(sender) {
        return sender["@type"] === "messageSenderUser" ? "user:" + sender.user_id : "chat:" + sender.chat_id;
    }

    function getEntry(sender) {
        var isUser = sender["@type"] === "messageSenderUser";
        var senderId = isUser ? sender.user_id : sender.chat_id;
        var key = getKey(sender);
        if (!entries[key]) {
            var name = "";
            // Never null, the model takes the type of a role from its first entry
            var photo = {};
            if (isUser) {
                var user = tdLibWrapper.getUserInformation(senderId.toString());
                name = Functions.getUserName(user);
                photo = user.profile_photo ? user.profile_photo.small : {};
            } else {
                var chat = tdLibWrapper.getChat(senderId.toString());
                name = chat.title || "";
                photo = chat.photo ? chat.photo.small : {};
            }
            entries[key] = {
                key: key,
                isUser: isUser,
                senderId: senderId.toString(),
                name: name,
                photo: photo,
                viewDate: 0,
                statusText: "",
                hasReaction: false,
                reactionDate: 0,
                emojis: "",
                hasCustomReaction: false
            };
        }
        return entries[key];
    }

    function addReaction(sender, reactionType, date) {
        var entry = getEntry(sender);
        entry.hasReaction = true;
        if (reactionType["@type"] === "reactionTypeEmoji") {
            entry.emojis += reactionType.emoji;
        } else {
            entry.hasCustomReaction = true;
        }
        entry.reactionDate = Math.max(entry.reactionDate, date);
        return entry;
    }

    function findRow(key, from, to) {
        for (var i = from; i < to; i++) {
            if (interactionsModel.get(i).key === key) {
                return i;
            }
        }
        return -1;
    }

    // A reaction from a later page is older than the ones shown, so its sender
    // goes to the end of the reactions unless they're already among them
    function addLoadedReaction(sender, reactionType, date) {
        var key = getKey(sender);
        var hadReaction = !!entries[key] && entries[key].hasReaction;
        var readerRow = !!entries[key] && !hadReaction ? findRow(key, reactionRows, interactionsModel.count) : -1;
        var entry = addReaction(sender, reactionType, date);
        if (hadReaction) {
            var row = findRow(key, 0, reactionRows);
            if (row >= 0) {
                interactionsModel.set(row, entry);
            }
        } else {
            if (readerRow >= 0) {
                interactionsModel.remove(readerRow);
            }
            interactionsModel.insert(reactionRows, entry);
            reactionRows += 1;
        }
    }

    // Asks for the next page of reactions once their end is less than a screen
    // below the visible part of the list
    function loadMoreReactionsIfNeeded() {
        if (modelFilled && !loadingReactions && nextReactionsOffset !== ""
                && interactionsListView.contentY + 2 * interactionsListView.height >= reactionRows * rowHeight) {
            loadingReactions = true;
            requestReactions(nextReactionsOffset);
        }
    }

    function finishRequest() {
        pendingRequests -= 1;
        if (pendingRequests === 0) {
            fillModel();
        }
    }

    function fillModel() {
        var list = [];
        for (var key in entries) {
            list.push(entries[key]);
        }
        list.sort(function(a, b) {
            if (a.hasReaction !== b.hasReaction) {
                return a.hasReaction ? -1 : 1;
            }
            return (b.reactionDate || b.viewDate) - (a.reactionDate || a.viewDate);
        });
        interactionsModel.clear();
        reactionRows = 0;
        for (var i = 0; i < list.length; i++) {
            interactionsModel.append(list[i]);
            if (list[i].hasReaction) {
                reactionRows += 1;
            }
        }
        modelFilled = true;
        loadMoreReactionsIfNeeded();
    }

    function requestReactions(offset) {
        tdLibWrapper.getMessageAddedReactions(message.chat_id, message.id, offset, 100, extraPrefix + "reactions");
    }

    function getReadStatusText(readDate) {
        switch (readDate["@type"]) {
        case "messageReadDateUnread":
            return qsTr("Not read yet");
        case "messageReadDateTooOld":
            return qsTr("Too old to know when it was read");
        case "messageReadDateUserPrivacyRestricted":
            return qsTr("Hides when they read messages");
        case "messageReadDateMyPrivacyRestricted":
            return qsTr("Hidden, as you hide when you read messages");
        default:
            return "";
        }
    }

    Component.onCompleted: {
        if (message.can_get_read_date) {
            pendingRequests += 1;
            tdLibWrapper.getMessageReadDate(message.chat_id, message.id, extraPrefix + "readDate");
        }
        if (message.can_get_viewers) {
            pendingRequests += 1;
            tdLibWrapper.getMessageViewers(message.chat_id, message.id, extraPrefix + "viewers");
        }
        var interactionInfo = message.interaction_info;
        if (interactionInfo && interactionInfo.can_get_added_reactions) {
            pendingRequests += 1;
            requestReactions("");
        } else if (interactionInfo && interactionInfo.reactions) {
            // No list of the reactions, as in private chats, but each one
            // knows a few of its senders, without a date
            for (var i = 0; i < interactionInfo.reactions.length; i++) {
                reactionCount += interactionInfo.reactions[i].total_count;
                var senders = interactionInfo.reactions[i].recent_sender_ids || [];
                for (var j = 0; j < senders.length; j++) {
                    addReaction(senders[j], interactionInfo.reactions[i].type, 0);
                }
            }
        }
        if (pendingRequests === 0) {
            fillModel();
        }
    }

    Connections {
        target: tdLibWrapper
        onMessageReadDateReceived: {
            if (extra === extraPrefix + "readDate") {
                var chat = tdLibWrapper.getChat(messageInteractionsPage.message.chat_id.toString());
                if (chat.type && chat.type.user_id) {
                    var entry = getEntry({ "@type": "messageSenderUser", user_id: chat.type.user_id });
                    if (readDate["@type"] === "messageReadDateRead") {
                        entry.viewDate = readDate.read_date;
                    } else {
                        entry.statusText = getReadStatusText(readDate);
                    }
                }
                finishRequest();
            }
        }
        onMessageViewersReceived: {
            if (extra === extraPrefix + "viewers") {
                for (var i = 0; i < viewers.length; i++) {
                    getEntry({ "@type": "messageSenderUser", user_id: viewers[i].user_id }).viewDate = viewers[i].view_date;
                }
                viewerCount = viewers.length;
                finishRequest();
            }
        }
        onAddedReactionsReceived: {
            if (extra === extraPrefix + "reactions") {
                reactionCount = totalCount;
                nextReactionsOffset = reactions.length > 0 ? nextOffset : "";
                if (modelFilled) {
                    for (var i = 0; i < reactions.length; i++) {
                        addLoadedReaction(reactions[i].sender_id, reactions[i].type, reactions[i].date);
                    }
                    loadingReactions = false;
                    loadMoreReactionsIfNeeded();
                } else {
                    for (var j = 0; j < reactions.length; j++) {
                        addReaction(reactions[j].sender_id, reactions[j].type, reactions[j].date);
                    }
                    finishRequest();
                }
            }
        }
        onErrorReceived: {
            if (extra.indexOf(extraPrefix) === 0) {
                Debug.log("[MessageInteractionsPage] Request failed: " + extra + " " + message);
                if (modelFilled) {
                    // A later page of reactions, keep what is shown and stop there
                    loadingReactions = false;
                    nextReactionsOffset = "";
                } else {
                    loadingFailed = true;
                    finishRequest();
                }
            }
        }
    }

    ListModel {
        id: interactionsModel
    }

    SilicaListView {
        id: interactionsListView
        anchors.fill: parent
        model: interactionsModel

        onContentYChanged: messageInteractionsPage.loadMoreReactionsIfNeeded()
        onHeightChanged: messageInteractionsPage.loadMoreReactionsIfNeeded()

        header: PageHeader {
            page: messageInteractionsPage
            title: qsTr("Readers and Reactions")
            description: {
                var parts = [];
                if (messageInteractionsPage.message.can_get_viewers && messageInteractionsPage.pendingRequests === 0) {
                    parts.push(qsTr("%Ln reader(s)", "number of users who read a message", messageInteractionsPage.viewerCount));
                }
                if (messageInteractionsPage.reactionCount > 0) {
                    parts.push(qsTr("%Ln reaction(s)", "number of reactions to a message", messageInteractionsPage.reactionCount));
                }
                return parts.join(", ");
            }
        }

        footer: Item {
            width: interactionsListView.width
            height: moreReactionsIndicator.running ? Theme.itemSizeLarge : 0

            BusyIndicator {
                id: moreReactionsIndicator
                anchors.centerIn: parent
                size: BusyIndicatorSize.Medium
                running: messageInteractionsPage.loadingReactions
            }
        }

        delegate: ListItem {
            id: interactionItem
            readonly property bool isOwnRow: model.isUser && model.senderId === messageInteractionsPage.myUserId
            contentHeight: messageInteractionsPage.rowHeight
            // Nothing to open for yourself
            enabled: !isOwnRow

            ProfileThumbnail {
                id: interactionThumbnail
                photoData: model.photo
                replacementStringHint: nameLabel.text
                width: Theme.itemSizeExtraSmall
                height: width
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
            }

            Column {
                anchors {
                    left: interactionThumbnail.right
                    leftMargin: Theme.paddingMedium
                    right: reactionRow.left
                    rightMargin: Theme.paddingMedium
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    id: nameLabel
                    width: parent.width
                    text: Emoji.emojify(interactionItem.isOwnRow ? qsTr("You") : model.name, font.pixelSize)
                    truncationMode: TruncationMode.Fade
                    color: interactionItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
                Label {
                    width: parent.width
                    visible: text !== ""
                    text: model.viewDate > 0 ? qsTr("Read %1", "%1 is a point in time").arg(Functions.getDateTimeTimepoint(model.viewDate))
                          : model.statusText !== "" ? model.statusText
                          : model.reactionDate > 0 ? qsTr("Reacted %1", "%1 is a point in time").arg(Functions.getDateTimeTimepoint(model.reactionDate)) : ""
                    truncationMode: TruncationMode.Fade
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: interactionItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }
            }

            Row {
                id: reactionRow
                spacing: Theme.paddingSmall
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    visible: model.emojis !== ""
                    text: Emoji.emojify(model.emojis, font.pixelSize)
                    font.pixelSize: Theme.fontSizeLarge
                    anchors.verticalCenter: parent.verticalCenter
                }
                Icon {
                    visible: model.hasCustomReaction
                    source: "image://theme/icon-s-favorite"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            onClicked: {
                if (model.isUser) {
                    tdLibWrapper.createPrivateChat(model.senderId, "openDirectly");
                } else {
                    pageStack.push(Qt.resolvedUrl("ChatPage.qml"), { "chatInformation" : tdLibWrapper.getChat(model.senderId) });
                }
            }
        }

        ViewPlaceholder {
            enabled: interactionsModel.count === 0 && messageInteractionsPage.pendingRequests === 0
            text: messageInteractionsPage.loadingFailed ? qsTr("Couldn't load who read or reacted to this message")
                                                        : qsTr("Nobody has read or reacted to this message yet")
        }

        BusyIndicator {
            anchors.centerIn: parent
            size: BusyIndicatorSize.Large
            running: messageInteractionsPage.pendingRequests > 0
        }

        VerticalScrollDecorator {}
    }
}
