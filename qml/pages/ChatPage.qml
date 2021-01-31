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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0
import Nemo.Thumbnailer 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/debug.js" as Debug
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: chatPage
    allowedOrientations: Orientation.All
    backNavigation: !stickerPickerLoader.active

    property bool loading: true;
    property bool isInitialized: false;
    readonly property int myUserId: tdLibWrapper.getUserInformation().id;
    property var chatInformation;
    property var secretChatDetails;
    property alias chatPicture: chatPictureThumbnail.photoData
    property bool isPrivateChat: false;
    property bool isSecretChat: false;
    property bool isSecretChatReady: false;
    property bool isBasicGroup: false;
    property bool isSuperGroup: false;
    property bool isChannel: false;
    property var chatPartnerInformation;
    property var botInformation;
    property var chatGroupInformation;
    property int chatOnlineMemberCount: 0;
    property var emojiProposals;
    property bool iterativeInitialization: false;
    readonly property bool userIsMember: ((isPrivateChat || isSecretChat) && chatInformation["@type"]) || // should be optimized
                                (isBasicGroup || isSuperGroup) && (
                                    (chatGroupInformation.status["@type"] === "chatMemberStatusMember")
                                    || (chatGroupInformation.status["@type"] === "chatMemberStatusAdministrator")
                                    || (chatGroupInformation.status["@type"] === "chatMemberStatusRestricted" && chatGroupInformation.status.is_member)
                                    || (chatGroupInformation.status["@type"] === "chatMemberStatusCreator" && chatGroupInformation.status.is_member)
                                    )
    property var selectedMessages: []
    readonly property bool isSelecting: selectedMessages.length > 0
    readonly property bool canSendMessages: hasSendPrivilege("can_send_messages")
    property bool doSendBotStartMessage
    property string sendBotStartMessageParameter

    states: [
        State {
            name: "selectMessages"
            when: isSelecting
            PropertyChanges {
                target: chatNameText
                text: qsTr("Select Messages")
            }
            PropertyChanges {
                target: chatStatusText
                text: qsTr("%Ln messages selected", "number of messages selected", chatPage.selectedMessages.length)
            }
            PropertyChanges {
                target: newMessageTextField
                focus: false
            }
        }

    ]

    function toggleMessageSelection(message) {
        var selectionArray = selectedMessages;
        var foundIndex = -1
        if(selectionArray.length > 0) {
            for(var i = 0; i < selectionArray.length; i += 1) {
                if(selectionArray[i].id === message.id) {
                    foundIndex = i;
                    continue;
                }
            }
        }
        if(foundIndex > -1) {
            selectionArray.splice(foundIndex, 1);
        } else {
            selectionArray.push(message);
        }
        selectedMessages = selectionArray;
    }

    function updateChatPartnerStatusText() {
        if (chatPage.isSelecting) {
            return
        }
        var statusText = Functions.getChatPartnerStatusText(chatPartnerInformation.status['@type'], chatPartnerInformation.status.was_online);
        if (chatPage.secretChatDetails) {
            var secretChatStatus = Functions.getSecretChatStatus(chatPage.secretChatDetails);
            if (statusText && secretChatStatus) {
                statusText += " - ";
            }
            if (secretChatStatus) {
                statusText += secretChatStatus;
            }
        }

        if (statusText) {
            chatStatusText.text = statusText;
        }
    }

    function updateGroupStatusText() {
        if (chatPage.isSelecting) {
            return
        }
        if (chatOnlineMemberCount > 0) {
            chatStatusText.text = qsTr("%1, %2", "combination of '[x members], [y online]', which are separate translations")
                .arg(qsTr("%1 members", "", chatGroupInformation.member_count)
                    .arg(Functions.getShortenedCount(chatGroupInformation.member_count)))
                .arg(qsTr("%1 online", "", chatOnlineMemberCount)
                    .arg(Functions.getShortenedCount(chatOnlineMemberCount)));
        } else {
            if (isChannel) {
                chatStatusText.text = qsTr("%1 subscribers", "", chatGroupInformation.member_count).arg(Functions.getShortenedCount(chatGroupInformation.member_count));
            } else {
                chatStatusText.text = qsTr("%1 members", "", chatGroupInformation.member_count).arg(Functions.getShortenedCount(chatGroupInformation.member_count));
            }
        }
        joinLeaveChatMenuItem.text = chatPage.userIsMember ? qsTr("Leave Chat") : qsTr("Join Chat");
    }

    function initializePage() {
        Debug.log("[ChatPage] Initializing chat page...");
        chatView.currentIndex = -1;
        chatView.lastReadSentIndex = 0;
        var chatType = chatInformation.type['@type'];
        isPrivateChat = chatType === "chatTypePrivate";
        isSecretChat = chatType === "chatTypeSecret";
        isBasicGroup = ( chatType === "chatTypeBasicGroup" );
        isSuperGroup = ( chatType === "chatTypeSupergroup" );
        if (isPrivateChat || isSecretChat) {
            chatPartnerInformation = tdLibWrapper.getUserInformation(chatInformation.type.user_id);
            updateChatPartnerStatusText();
            if (isSecretChat) {
                tdLibWrapper.getSecretChat(chatInformation.type.secret_chat_id);
            }
            if(chatPartnerInformation.type["@type"] === "userTypeBot") {
                tdLibWrapper.getUserFullInfo(chatPartnerInformation.id)
            }
        }
        else if (isBasicGroup) {
            chatGroupInformation = tdLibWrapper.getBasicGroup(chatInformation.type.basic_group_id);
            updateGroupStatusText();
        }
        else if (isSuperGroup) {
            chatGroupInformation = tdLibWrapper.getSuperGroup(chatInformation.type.supergroup_id);
            isChannel = chatGroupInformation.is_channel;
            updateGroupStatusText();
        }
        if (stickerManager.needsReload()) {
            Debug.log("[ChatPage] Stickers will be reloaded!");
            tdLibWrapper.getRecentStickers();
            tdLibWrapper.getInstalledStickerSets();
            stickerManager.setNeedsReload(false);
        }
        tdLibWrapper.getChatPinnedMessage(chatInformation.id);
        tdLibWrapper.toggleChatIsMarkedAsUnread(chatInformation.id, false);

    }

    function getMessageStatusText(message, listItemIndex, lastReadSentIndex, useElapsed) {
        var messageStatusSuffix = "";
        if(!message) {
            return "";
        }

        if (message.edit_date > 0) {
            messageStatusSuffix += " - " + qsTr("edited");
        }

        if (chatPage.myUserId === message.sender.user_id) {
            messageStatusSuffix += "&nbsp;&nbsp;"
            if (listItemIndex <= lastReadSentIndex) {
                // Read by other party
                messageStatusSuffix += Emoji.emojify("✅", Theme.fontSizeTiny);
            } else {
                // Not yet read by other party
                if (message.sending_state) {
                    if (message.sending_state['@type'] === "messageSendingStatePending") {
                        messageStatusSuffix += Emoji.emojify("🕙", Theme.fontSizeTiny);
                    } else {
                        // Sending failed...
                        messageStatusSuffix += Emoji.emojify("❌", Theme.fontSizeTiny);
                    }
                } else {
                    messageStatusSuffix += Emoji.emojify("☑️", Theme.fontSizeTiny);
                }
            }
        }
        return ( useElapsed ? Functions.getDateTimeElapsed(message.date) : Functions.getDateTimeTranslated(message.date) ) + messageStatusSuffix;
    }

    function clearAttachmentPreviewRow() {
        attachmentPreviewRow.isPicture = false;
        attachmentPreviewRow.isVideo = false;
        attachmentPreviewRow.isDocument = false;
        attachmentPreviewRow.isVoiceNote = false;
        attachmentPreviewRow.isLocation = false;
        attachmentPreviewRow.fileProperties = null;
        attachmentPreviewRow.locationData = null;
        attachmentPreviewRow.attachmentDescription = "";
        fernschreiberUtils.stopGeoLocationUpdates();
    }

    function controlSendButton() {
        if (newMessageTextField.text.length !== 0
                || attachmentPreviewRow.isPicture
                || attachmentPreviewRow.isDocument
                || attachmentPreviewRow.isVideo
                || attachmentPreviewRow.isVoiceNote
                || attachmentPreviewRow.isLocation) {
            newMessageSendButton.enabled = true;
        } else {
            newMessageSendButton.enabled = false;
        }
    }

    function sendMessage() {
        if (newMessageColumn.editMessageId !== "0") {
            tdLibWrapper.editMessageText(chatInformation.id, newMessageColumn.editMessageId, newMessageTextField.text);
        } else {
            if (attachmentPreviewRow.visible) {
                if (attachmentPreviewRow.isPicture) {
                    tdLibWrapper.sendPhotoMessage(chatInformation.id, attachmentPreviewRow.fileProperties.filePath, newMessageTextField.text, newMessageColumn.replyToMessageId);
                }
                if (attachmentPreviewRow.isVideo) {
                    tdLibWrapper.sendVideoMessage(chatInformation.id, attachmentPreviewRow.fileProperties.filePath, newMessageTextField.text, newMessageColumn.replyToMessageId);
                }
                if (attachmentPreviewRow.isDocument) {
                    tdLibWrapper.sendDocumentMessage(chatInformation.id, attachmentPreviewRow.fileProperties.filePath, newMessageTextField.text, newMessageColumn.replyToMessageId);
                }
                if (attachmentPreviewRow.isVoiceNote) {
                    tdLibWrapper.sendVoiceNoteMessage(chatInformation.id, fernschreiberUtils.voiceNotePath(), newMessageTextField.text, newMessageColumn.replyToMessageId);
                }
                if (attachmentPreviewRow.isLocation) {
                    tdLibWrapper.sendLocationMessage(chatInformation.id, attachmentPreviewRow.locationData.latitude, attachmentPreviewRow.locationData.longitude, attachmentPreviewRow.locationData.horizontalAccuracy, newMessageColumn.replyToMessageId);
                }
                clearAttachmentPreviewRow();
            } else {
                tdLibWrapper.sendTextMessage(chatInformation.id, newMessageTextField.text, newMessageColumn.replyToMessageId);
            }

            if(appSettings.focusTextAreaAfterSend) {
                lostFocusTimer.start();
            }
        }
        controlSendButton();
        newMessageInReplyToRow.inReplyToMessage = null;
        newMessageColumn.editMessageId = "0";
        fernschreiberUtils.stopGeoLocationUpdates();
    }

    function getWordBoundaries(text, cursorPosition) {
        var wordBoundaries = { beginIndex : 0, endIndex : text.length};
        var currentIndex = 0;
        for (currentIndex = (cursorPosition - 1); currentIndex > 0; currentIndex--) {
            if (text.charAt(currentIndex) === ' ') {
                wordBoundaries.beginIndex = currentIndex + 1;
                break;
            }
        }
        for (currentIndex = cursorPosition; currentIndex < text.length; currentIndex++) {
            if (text.charAt(currentIndex) === ' ') {
                wordBoundaries.endIndex = currentIndex;
                break;
            }
        }
        return wordBoundaries;
    }

    function handleMessageTextReplacement(text, cursorPosition) {
        if(!newMessageTextField.focus) {
            return;
        }

        var wordBoundaries = getWordBoundaries(text, cursorPosition);

        var currentWord = text.substring(wordBoundaries.beginIndex, wordBoundaries.endIndex);
        if (currentWord.length > 1 && currentWord.charAt(0) === ':') {
            tdLibWrapper.searchEmoji(currentWord.substring(1));
        } else {
            chatPage.emojiProposals = null;
        }
        if (currentWord.length > 1 && currentWord.charAt(0) === '@') {
            knownUsersRepeater.model = knownUsersProxyModel;
            knownUsersProxyModel.setFilterWildcard("*" + currentWord.substring(1) + "*");
        } else {
            knownUsersRepeater.model = undefined;
        }

    }

    function replaceMessageText(text, cursorPosition, newText) {
        var wordBoundaries = getWordBoundaries(text, cursorPosition);
        var newCompleteText = text.substring(0, wordBoundaries.beginIndex) + newText + " " + text.substring(wordBoundaries.endIndex);
        var newIndex = wordBoundaries.beginIndex + newText.length + 1;
        newMessageTextField.text = newCompleteText;
        newMessageTextField.cursorPosition = newIndex;
        lostFocusTimer.start();
    }

    function setMessageText(text, doSend) {
        if(doSend) {
            tdLibWrapper.sendTextMessage(chatInformation.id, text, "0");
        }
        else {
            newMessageTextField.text = text
            newMessageTextField.cursorPosition = text.length
            lostFocusTimer.start();
        }

    }

    function forwardMessages(fromChatId, messageIds) {
        forwardMessagesTimer.fromChatId = fromChatId;
        forwardMessagesTimer.messageIds = messageIds;
        forwardMessagesTimer.start();
    }
    function hasSendPrivilege(privilege) {
        var groupStatus = chatGroupInformation ? chatGroupInformation.status : null
        var groupStatusType = groupStatus ? groupStatus["@type"] : null
        return chatPage.isPrivateChat
                    || (groupStatusType === "chatMemberStatusMember" && chatInformation.permissions[privilege])
                    || groupStatusType === "chatMemberStatusAdministrator"
                    || groupStatusType === "chatMemberStatusCreator"
                    || (groupStatusType === "chatMemberStatusRestricted" && groupStatus.permissions[privilege])
                    || (chatPage.isSecretChat && chatPage.isSecretChatReady)
    }
    function canPinMessages() {
        Debug.log("Can we pin messages?");
        if (chatPage.isPrivateChat || chatPage.isSecretChat) {
            Debug.log("Private/Secret Chat: No!");
            return false;
        }
        if (chatPage.chatGroupInformation.status["@type"] === "chatMemberStatusCreator") {
            Debug.log("Creator of this chat: Yes!");
            return true;
        }
        if (chatPage.chatInformation.permissions.can_pin_messages) {
            Debug.log("All people can pin: Yes!");
            return true;
        }
        if (chatPage.chatGroupInformation.status["@type"] === "chatMemberStatusAdministrator") {
            Debug.log("Admin with privileges? ", chatPage.chatGroupInformation.status.can_pin_messages);
            return chatPage.chatGroupInformation.status.can_pin_messages;
        }
        if (chatPage.chatGroupInformation.status["@type"] === "chatMemberStatusRestricted") {
            Debug.log("Restricted, but can pin messages? ", chatPage.chatGroupInformation.status.permissions.can_pin_messages);
            return chatPage.chatGroupInformation.status.permissions.can_pin_messages;
        }
        Debug.log("Something else: No!");
        return false;
    }

    function resetFocus() {
        if (searchInChatField.text === "") {
            chatOverviewItem.visible = true;
        }
        searchInChatField.focus = false;
        chatPage.focus = true;
    }

    Timer {
        id: forwardMessagesTimer
        interval: 200

        property string fromChatId
        property var messageIds
        onTriggered: {
            if(chatPage.loading) {
                forwardMessagesTimer.start()
            } else {
                var forwardedToSecretChat = chatInformation.type["@type"] === "chatTypeSecret";
                tdLibWrapper.forwardMessages(chatInformation.id, fromChatId, messageIds, forwardedToSecretChat, false);
            }
        }
    }

    Timer {
        id: searchInChatTimer
        interval: 300
        running: false
        repeat: false
        onTriggered: {
            Debug.log("Searching for '" + searchInChatField.text + "'");
            chatModel.setSearchQuery(searchInChatField.text);
        }
    }

    Component.onCompleted: {
        initializePage();
    }

    Component.onDestruction: {
        if (chatPage.canSendMessages) {
            tdLibWrapper.setChatDraftMessage(chatInformation.id, 0, newMessageColumn.replyToMessageId, newMessageTextField.text);
        }
        fernschreiberUtils.stopGeoLocationUpdates();
        tdLibWrapper.closeChat(chatInformation.id);
    }

    onStatusChanged: {
        switch(status) {
        case PageStatus.Activating:
            tdLibWrapper.openChat(chatInformation.id);
            if(!chatPage.isInitialized) {
                if(chatInformation.draft_message) {
                    if(chatInformation.draft_message && chatInformation.draft_message.input_message_text) {
                        newMessageTextField.text = chatInformation.draft_message.input_message_text.text.text;
                        if(chatInformation.draft_message.reply_to_message_id) {
                            tdLibWrapper.getMessage(chatInformation.id, chatInformation.draft_message.reply_to_message_id);
                        }
                    }
                }
            }

            break;
        case PageStatus.Active:
            if (!chatPage.isInitialized) {
                chatModel.initialize(chatInformation);

                pageStack.pushAttached(Qt.resolvedUrl("ChatInformationPage.qml"), { "chatInformation" : chatInformation, "privateChatUserInformation": chatPartnerInformation, "groupInformation": chatGroupInformation, "chatOnlineMemberCount": chatOnlineMemberCount});
                chatPage.isInitialized = true;

                if(doSendBotStartMessage) {
                    tdLibWrapper.sendBotStartMessage(chatInformation.id, chatInformation.id, sendBotStartMessageParameter, "")
                }
            }
            break;
        case PageStatus.Inactive:
            if (pageStack.depth === 1) {
                // Only clear chat model if navigated back to overview page. In other cases we keep the information...
                chatModel.clear();
            }
            break;
        }
    }

    Connections {
        target: tdLibWrapper
        onUserUpdated: {
            if ((isPrivateChat || isSecretChat) && chatPartnerInformation.id.toString() === userId ) {
                chatPartnerInformation = userInformation;
                updateChatPartnerStatusText();
            }
        }
        onBasicGroupUpdated: {
            if (isBasicGroup && chatGroupInformation.id.toString() === groupId ) {
                chatGroupInformation = groupInformation;
                updateGroupStatusText();
            }
        }
        onSuperGroupUpdated: {
            if (isSuperGroup && chatGroupInformation.id.toString() === groupId ) {
                chatGroupInformation = groupInformation;
                updateGroupStatusText();
            }
        }
        onChatOnlineMemberCountUpdated: {
            Debug.log(isSuperGroup, "/", isBasicGroup, "/", chatInformation.id.toString(), "/", chatId);
            if ((isSuperGroup || isBasicGroup) && chatInformation.id.toString() === chatId) {
                chatOnlineMemberCount = onlineMemberCount;
                updateGroupStatusText();
            }
        }
        onFileUpdated: {
            uploadStatusRow.visible = fileInformation.remote.is_uploading_active;
            if (uploadStatusRow.visible) {
                uploadingProgressBar.maximumValue = fileInformation.size;
                uploadingProgressBar.value = fileInformation.remote.uploaded_size;
            }
        }
        onEmojiSearchSuccessful: {
            chatPage.emojiProposals = result;
        }
        onErrorReceived: {
            Functions.handleErrorMessage(code, message);
        }
        onReceivedMessage: {
            if (message.is_pinned) {
                Debug.log("[ChatPage] Received pinned message");
                pinnedMessageItem.pinnedMessage = message;
            }
            if (chatInformation.draft_message && messageId === chatInformation.draft_message.reply_to_message_id) {
                newMessageInReplyToRow.inReplyToMessage = message;
            }
        }
        onSecretChatReceived: {
            if (secretChatId === chatInformation.type.secret_chat_id) {
                Debug.log("[ChatPage] Received detailed information about this secret chat");
                chatPage.secretChatDetails = secretChat;
                updateChatPartnerStatusText();
                chatPage.isSecretChatReady = chatPage.secretChatDetails.state["@type"] === "secretChatStateReady";
            }
        }
        onSecretChatUpdated: {
            if (secretChatId.toString() === chatInformation.type.secret_chat_id.toString()) {
                Debug.log("[ChatPage] Detailed information about this secret chat was updated");
                chatPage.secretChatDetails = secretChat;
                updateChatPartnerStatusText();
                chatPage.isSecretChatReady = chatPage.secretChatDetails.state["@type"] === "secretChatStateReady";
            }
        }
        onCallbackQueryAnswer: {
            if(text.length > 0) { // ignore bool "alert", just show as notification:
                appNotification.show(Emoji.emojify(text, Theme.fontSizeSmall));
            }
            if(url.length > 0) {
                Functions.handleLink(url);
            }
        }
        onUserFullInfoReceived: {
            if(userFullInfo["@extra"] === chatPartnerInformation.id.toString()) {
                chatPage.botInformation = userFullInfo;
            }
        }
        onUserFullInfoUpdated: {

            if(userId === chatPartnerInformation.id) {
                chatPage.botInformation = userFullInfo;
            }
        }
    }

    Connections {
        target: chatModel
        onMessagesReceived: {
            Debug.log("[ChatPage] Messages received, view has ", chatView.count, " messages, setting view to index ", modelIndex, ", own messages were read before index ", lastReadSentIndex);
            if (totalCount === 0) {
                if (chatPage.iterativeInitialization) {
                    chatPage.iterativeInitialization = false;
                    Debug.log("[ChatPage] actually, skipping that: No Messages in Chat.");
                    chatView.positionViewAtEnd();
                    chatPage.loading = false;
                    return;
                } else {
                    chatPage.iterativeInitialization = true;
                }
            }

            chatView.lastReadSentIndex = lastReadSentIndex;
            chatView.scrollToIndex(modelIndex);
            chatPage.loading = false;
            if (chatOverviewItem.visible && modelIndex >= (chatView.count - 10)) {
                chatView.inCooldown = true;
                chatModel.triggerLoadMoreFuture();
            }

            if (chatView.height > chatView.contentHeight) {
                Debug.log("[ChatPage] Chat content quite small...");
                viewMessageTimer.queueViewMessage(chatView.count - 1);
            }

            chatViewCooldownTimer.restart();
        }
        onNewMessageReceived: {
            if (chatView.manuallyScrolledToBottom || message.sender.user_id === chatPage.myUserId) {
                Debug.log("[ChatPage] Own message received or was scrolled to bottom, scrolling down to see it...");
                chatView.scrollToIndex(chatView.count - 1);
            }
        }
        onUnreadCountUpdated: {
            Debug.log("[ChatPage] Unread count updated, new count: ", unreadCount);
            chatInformation.unread_count = unreadCount;
            chatUnreadMessagesItem.visible = ( !chatPage.loading && chatInformation.unread_count > 0 && chatOverviewItem.visible );
            chatUnreadMessagesCount.text = unreadCount > 99 ? "99+" : unreadCount;
        }
        onLastReadSentMessageUpdated: {
            Debug.log("[ChatPage] Updating last read sent index, new index: ", lastReadSentIndex);
            chatView.lastReadSentIndex = lastReadSentIndex;
        }
        onMessagesIncrementalUpdate: {
            Debug.log("Incremental update received. View now has ", chatView.count, " messages, view is on index ", modelIndex, ", own messages were read before index ", lastReadSentIndex);
            chatView.lastReadSentIndex = lastReadSentIndex;
            chatViewCooldownTimer.restart();
        }
        onNotificationSettingsUpdated: {
            chatInformation = chatModel.getChatInformation();
            muteChatMenuItem.text = chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat");
        }
        onPinnedMessageChanged: {
            chatInformation = chatModel.getChatInformation();
            if (chatInformation.pinned_message_id.toString() !== "0") {
                Debug.log("[ChatPage] Loading pinned message ", chatInformation.pinned_message_id);
                tdLibWrapper.getMessage(chatInformation.id, chatInformation.pinned_message_id);
            } else {
                pinnedMessageItem.pinnedMessage = undefined;
            }
        }
    }

    Connections {
        target: chatListModel
        onChatJoined: {
            appNotification.show(qsTr("You joined the chat %1").arg(chatTitle));
        }
    }

    Timer {
        id: lostFocusTimer
        interval: 200
        running: false
        repeat: false
        onTriggered: {
            newMessageTextField.forceActiveFocus();
        }
    }

    Timer {
        id: textReplacementTimer
        interval: 600
        running: false
        repeat: false
        onTriggered: {
            handleMessageTextReplacement(newMessageTextField.text, newMessageTextField.cursorPosition);
        }
    }

    Timer {
        id: chatContactTimeUpdater
        interval: 60000
        running: isPrivateChat || isSecretChat
        repeat: true
        onTriggered: {
            updateChatPartnerStatusText();
        }
    }
    Timer {
        id: viewMessageTimer
        interval: 1000
        property int lastQueuedIndex: -1
        function queueViewMessage(index) {
            if (index > lastQueuedIndex) {
                lastQueuedIndex = index;
                start();
            }
        }

        onTriggered: {
            if(chatInformation.unread_count > 0 && lastQueuedIndex > -1) {
                var messageToRead = chatModel.getMessage(lastQueuedIndex);
                if (messageToRead && messageToRead.id) {
                    tdLibWrapper.viewMessage(chatInformation.id, messageToRead.id, false);
                }
                lastQueuedIndex = -1
            }
            if (chatInformation.unread_count === 0) {
                tdLibWrapper.readAllChatMentions(chatInformation.id);
            }
        }
    }

    SilicaFlickable {
        id: chatContainer

        onContentYChanged: {
            // For some strange reason contentY sometimes is > 0 which doesn't make sense without a PushUpMenu (?)
            // That leads to the problem that the whole flickable is moved slightly (or sometimes considerably) up
            // which creates UX issues... As a workaround we are setting it to 0 in such cases.
            // Better solutions are highly appreciated, contributions always welcome! ;)
            if (contentY > 0) {
                contentY = 0;
            }
        }

        anchors.fill: parent
        contentHeight: height
        contentWidth: width

        PullDownMenu {
            visible: chatInformation.id !== chatPage.myUserId && !stickerPickerLoader.active && !voiceNoteOverlayLoader.active && !messageOverlayLoader.active
            MenuItem {
                id: closeSecretChatMenuItem
                visible: chatPage.isSecretChat && chatPage.secretChatDetails.state["@type"] !== "secretChatStateClosed"
                onClicked: {
                    var secretChatId = chatPage.secretChatDetails.id;
                    Remorse.popupAction(chatPage, qsTr("Closing chat"), function() { tdLibWrapper.closeSecretChat(secretChatId) });
                }
                text: qsTr("Close Chat")
            }

            MenuItem {
                id: joinLeaveChatMenuItem
                visible: (chatPage.isSuperGroup || chatPage.isBasicGroup) && chatGroupInformation && chatGroupInformation.status["@type"] !== "chatMemberStatusBanned"
                onClicked: {
                    if (chatPage.userIsMember) {
                        var chatId = chatInformation.id;
                        Remorse.popupAction(chatPage, qsTr("Leaving chat"), function() {
                            tdLibWrapper.leaveChat(chatId);
                            // this does not care about the response (ideally type "ok" without further reference) for now
                            pageStack.pop(pageStack.find( function(page){ return(page._depth === 0)} ));
                        });
                    } else {
                        tdLibWrapper.joinChat(chatInformation.id);
                    }
                }
                text: chatPage.userIsMember ? qsTr("Leave Chat") : qsTr("Join Chat")
            }

            MenuItem {
                id: muteChatMenuItem
                visible: chatPage.userIsMember
                onClicked: {
                    var newNotificationSettings = chatInformation.notification_settings;
                    if (newNotificationSettings.mute_for > 0) {
                        newNotificationSettings.mute_for = 0;
                    } else {
                        newNotificationSettings.mute_for = 6666666;
                    }
                    newNotificationSettings.use_default_mute_for = false;
                    tdLibWrapper.setChatNotificationSettings(chatInformation.id, newNotificationSettings);
                }
                text: chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat")
            }

            MenuItem {
                id: searchInChatMenuItem
                visible: !chatPage.isSecretChat && chatOverviewItem.visible
                onClicked: {
                    // This automatically shows the search field as well
                    chatOverviewItem.visible = false;
                    searchInChatField.focus = true;
                }
                text: qsTr("Search in Chat")
            }
        }

        BackgroundItem {
            id: headerMouseArea
            height: headerRow.height
            width: parent.width
            onClicked: {
                if (chatPage.isSelecting) {
                    chatPage.selectedMessages = [];
                } else {
                    pageStack.navigateForward();
                }
            }
        }

        Column {
            id: chatColumn
            width: parent.width
            height: parent.height

            Row {
                id: headerRow
                width: parent.width - (3 * Theme.horizontalPageMargin)
                height: chatOverviewItem.height + ( chatPage.isPortrait ? (2 * Theme.paddingMedium) : (2 * Theme.paddingSmall) )
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium

                Item {
                    width: chatOverviewItem.height
                    height: chatOverviewItem.height
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: chatPage.isPortrait ? Theme.paddingMedium : Theme.paddingSmall

                    ProfileThumbnail {
                        id: chatPictureThumbnail
                        replacementStringHint: chatNameText.text
                        width: parent.height
                        height: parent.height

                        // Setting it directly may cause an stale state for the thumbnail in case the chat page
                        // was previously loaded with a picture and now it doesn't have one. Instead setting it
                        // when the ChatModel indicates a change. This also avoids flickering when the page is loaded...
                        Connections {
                            target: chatModel
                            onSmallPhotoChanged: {
                                chatPictureThumbnail.photoData = chatModel.smallPhoto;
                            }
                        }
                    }

                    Rectangle {
                        id: chatSecretBackground
                        color: Theme.highlightBackgroundColor
                        width: chatPage.isPortrait ? Theme.fontSizeLarge : Theme.fontSizeMedium
                        height: width
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        radius: parent.width / 2
                        visible: chatPage.isSecretChat
                    }

                    Image {
                        source: "image://theme/icon-s-secure"
                        width: chatPage.isPortrait ? Theme.fontSizeSmall : Theme.fontSizeExtraSmall
                        height: width
                        anchors.centerIn: chatSecretBackground
                        visible: chatPage.isSecretChat
                    }

                }

                Item {
                    id: chatOverviewItem
                    opacity: visible ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    width: parent.width - chatPictureThumbnail.width - Theme.paddingMedium
                    height: chatNameText.height + chatStatusText.height
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: chatPage.isPortrait ? Theme.paddingMedium : Theme.paddingSmall
                    Label {
                        id: chatNameText
                        width: Math.min(implicitWidth, parent.width)
                        anchors.right: parent.right
                        text: chatInformation.title !== "" ? Emoji.emojify(chatInformation.title, font.pixelSize) : qsTr("Unknown")
                        textFormat: Text.StyledText
                        font.pixelSize: chatPage.isPortrait ? Theme.fontSizeLarge : Theme.fontSizeMedium
                        font.family: Theme.fontFamilyHeading
                        color: Theme.highlightColor
                        truncationMode: TruncationMode.Fade
                        maximumLineCount: 1
                    }
                    Label {
                        id: chatStatusText
                        width: Math.min(implicitWidth, parent.width)
                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                        }
                        text: ""
                        textFormat: Text.StyledText
                        font.pixelSize: chatPage.isPortrait ? Theme.fontSizeExtraSmall : Theme.fontSizeTiny
                        font.family: Theme.fontFamilyHeading
                        color: headerMouseArea.pressed ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        truncationMode: TruncationMode.Fade
                        maximumLineCount: 1
                    }
                }

                Item {
                    id: searchInChatItem
                    visible: !chatOverviewItem.visible
                    opacity: visible ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    width: parent.width - chatPictureThumbnail.width - Theme.paddingMedium
                    height: searchInChatField.height
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: chatPage.isPortrait ? Theme.paddingSmall : 0

                    SearchField {
                        id: searchInChatField
                        visible: false
                        width: visible ? parent.width : 0
                        placeholderText: qsTr("Search in chat...")
                        active: searchInChatItem.visible
                        canHide: text === ""

                        onTextChanged: {
                            searchInChatTimer.restart();
                        }

                        onHideClicked: {
                            resetFocus();
                        }

                        EnterKey.iconSource: "image://theme/icon-m-enter-close"
                        EnterKey.onClicked: {
                            resetFocus();
                        }
                    }
                }
            }

            PinnedMessageItem {
                id: pinnedMessageItem
                onRequestShowMessage: {
                    messageOverlayLoader.overlayMessage = pinnedMessageItem.pinnedMessage;
                    messageOverlayLoader.active = true;
                }
                onRequestCloseMessage: {
                    messageOverlayLoader.overlayMessage = undefined;
                    messageOverlayLoader.active = false;
                }
            }

            Item {
                id: chatViewItem
                width: parent.width
                height: parent.height - headerRow.height - pinnedMessageItem.height - newMessageColumn.height - selectedMessagesActions.height

                property int previousHeight;

                Component.onCompleted: {
                    previousHeight = height;
                }

                onHeightChanged: {
                    var deltaHeight = previousHeight - height;
                    chatView.contentY = chatView.contentY + deltaHeight;
                    previousHeight = height;
                }

                Timer {
                    id: chatViewCooldownTimer
                    interval: 2000
                    repeat: false
                    running: false
                    onTriggered: {
                        Debug.log("[ChatPage] Cooldown completed...");
                        chatView.inCooldown = false;
                    }
                }

                Loader {
                    asynchronous: true
                    active: chatView.blurred
                    anchors.fill: chatView
                    sourceComponent: Component {
                        FastBlur {
                            source: chatView
                            radius: Theme.paddingLarge
                        }
                    }
                }

                SilicaListView {
                    id: chatView

                    visible: !blurred
                    property bool blurred: messageOverlayLoader.item || stickerPickerLoader.item || voiceNoteOverlayLoader.item || inlineQuery.hasOverlay

                    anchors.fill: parent
                    opacity: chatPage.loading ? 0 : 1
                    Behavior on opacity { FadeAnimation {} }
                    clip: true
                    highlightMoveDuration: 0
                    highlightResizeDuration: 0
                    property int lastReadSentIndex: 0
                    property bool inCooldown: false
                    property bool manuallyScrolledToBottom
                    property QtObject precalculatedValues: QtObject {
                        readonly property alias page: chatPage
                        readonly property bool showUserInfo: page.isBasicGroup || ( page.isSuperGroup && !page.isChannel)
                        readonly property int profileThumbnailDimensions: showUserInfo ? Theme.itemSizeSmall : 0
                        readonly property int pageMarginDouble: 2 * Theme.horizontalPageMargin
                        readonly property int paddingMediumDouble: 2 * Theme.paddingMedium
                        readonly property int entryWidth: chatView.width - pageMarginDouble
                        readonly property int textItemWidth: entryWidth - profileThumbnailDimensions - Theme.paddingSmall
                        readonly property int backgroundWidth: page.isChannel ? textItemWidth : textItemWidth - pageMarginDouble
                        readonly property int backgroundRadius: textItemWidth/50
                        readonly property int textColumnWidth: backgroundWidth - Theme.horizontalPageMargin
                        readonly property int messageInReplyToHeight: Theme.fontSizeExtraSmall * 2.571428571 + Theme.paddingSmall;
                        readonly property int webPagePreviewHeight: ( (textColumnWidth * 2 / 3) + (6 * Theme.fontSizeExtraSmall) + ( 7 * Theme.paddingSmall) )
                        readonly property bool pageIsSelecting: chatPage.isSelecting

                    }

                    function handleScrollPositionChanged() {
                        Debug.log("Current position: ", chatView.contentY);
                        if (chatOverviewItem.visible && chatInformation.unread_count > 0) {
                            var bottomIndex = chatView.indexAt(chatView.contentX, ( chatView.contentY + chatView.height - Theme.horizontalPageMargin ));
                            if (bottomIndex > -1) {
                                viewMessageTimer.queueViewMessage(bottomIndex)
                            }
                        } else {
                            tdLibWrapper.readAllChatMentions(chatInformation.id);
                        }
                        manuallyScrolledToBottom = chatView.atYEnd
                    }

                    function scrollToIndex(index) {
                        if(index > 0 && index < chatView.count) {
                            positionViewAtIndex(index, ListView.Contain)
//                            currentIndex = index;
                            if(index === chatView.count - 1) {
                                manuallyScrolledToBottom = true;
                            }
                        }
                    }

                    onContentYChanged: {
                        if (!chatPage.loading && !chatView.inCooldown) {
                            if (chatView.indexAt(chatView.contentX, chatView.contentY) < 10) {
                                Debug.log("[ChatPage] Trying to get older history items...");
                                chatView.inCooldown = true;
                                chatModel.triggerLoadMoreHistory();
                            } else if (chatOverviewItem.visible && chatView.indexAt(chatView.contentX, chatView.contentY) > ( count - 10)) {
                                Debug.log("[ChatPage] Trying to get newer history items...");
                                chatView.inCooldown = true;
                                chatModel.triggerLoadMoreFuture();
                            }
                        }
                    }

                    onMovementEnded: {
                        handleScrollPositionChanged();
                    }

                    onQuickScrollAnimatingChanged: {
                        if (!quickScrollAnimating) {
                            handleScrollPositionChanged();
                            if(atYEnd) { // handle some false guesses from quick scroll
                                chatView.scrollToIndex(chatView.count - 2)
                                chatView.scrollToIndex(chatView.count - 1)
                            }
                        }
                    }

                    model: chatModel
                    header: Component {
                        Loader {
                            active: !!chatPage.botInformation
                                    && !!chatPage.botInformation.bot_info && chatPage.botInformation.bot_info.description.length > 0
                            asynchronous: true
                            width: chatView.width
                            sourceComponent: Component {
                                Label {
                                    id: botInfoLabel
                                    topPadding: Theme.paddingLarge
                                    bottomPadding: Theme.paddingLarge
                                    leftPadding: Theme.horizontalPageMargin
                                    rightPadding: Theme.horizontalPageMargin
                                    text: Emoji.emojify(chatPage.botInformation.bot_info.description, font.pixelSize)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.highlightColor
                                    wrapMode: Text.Wrap
                                    textFormat: Text.StyledText
                                    horizontalAlignment: Text.AlignHCenter
                                    onLinkActivated: {
                                        var chatCommand = Functions.handleLink(link);
                                        if(chatCommand) {
                                            tdLibWrapper.sendTextMessage(chatInformation.id, chatCommand);
                                        }
                                    }
                                    linkColor: Theme.primaryColor
                                    visible: (text !== "")
                                }
                            }
                        }
                    }

                    function getContentComponentHeight(contentType, content, parentWidth) {
                        switch(contentType) {
                        case "messageAnimation":
                            return Functions.getVideoHeight(parentWidth, content.animation);
                        case "messageAudio":
                        case "messageVoiceNote":
                        case "messageDocument":
                            return Theme.itemSizeLarge;
                        case "messageGame":
                            return parentWidth * 0.66666666 + Theme.itemSizeLarge; // 2 / 3;
                        case "messageLocation":
                        case "messageVenue":
                            return parentWidth * 0.66666666; // 2 / 3;
                        case "messagePhoto":
                            var biggest = content.photo.sizes[content.photo.sizes.length - 1];
                            var aspectRatio = biggest.width/biggest.height;
                            return Math.max(Theme.itemSizeExtraSmall, Math.min(parentWidth * 0.66666666, parentWidth / aspectRatio));
                        case "messagePoll":
                            return Theme.itemSizeSmall * (4 + content.poll.options);
                        case "messageSticker":
                            return content.sticker.height;
                        case "messageVideo":
                            return Functions.getVideoHeight(parentWidth, content.video);
                        case "messageVideoNote":
                            return parentWidth
                        }
                    }

                    readonly property var delegateMessagesContent: [
                        "messageAnimation",
                        "messageAudio",
                        // "messageContact",
                        // "messageDice"
                        "messageDocument",
                        "messageGame",
                        // "messageInvoice",
                        "messageLocation",
                        // "messagePassportDataSent",
                        // "messagePaymentSuccessful",
                        "messagePhoto",
                        "messagePoll",
                        // "messageProximityAlertTriggered",
                        "messageSticker",
                        "messageVenue",
                        "messageVideo",
                        "messageVideoNote",
                        "messageVoiceNote"
                    ]

                    readonly property var simpleDelegateMessages: ["messageBasicGroupChatCreate",
                                                                   "messageChatAddMembers",
                                                                   "messageChatChangePhoto",
                                                                   "messageChatChangeTitle",
                                                                   "messageChatDeleteMember",
                                                                   "messageChatDeletePhoto",
                                                                   "messageChatJoinByLink",
                                                                   "messageChatSetTtl",
                                                                   "messageChatUpgradeFrom",
                                                                   // "messageContactRegistered","messageExpiredPhoto", "messageExpiredVideo","messageWebsiteConnected"
                                                                   "messageGameScore",
                                                                   "messageChatUpgradeTo",
                                                                   "messageCustomServiceAction",
                                                                   "messagePinMessage",
                                                                   "messageScreenshotTaken",
                                                                   "messageSupergroupChatCreate",
                                                                   "messageUnsupported"]
                    delegate: Loader {
                        width: chatView.width
                        Component {
                            id: messageListViewItemComponent
                            MessageListViewItem {
                                precalculatedValues: chatView.precalculatedValues
                                chatId: chatModel.chatId
                                myMessage: model.display
                                messageId: model.message_id
                                messageViewCount: model.view_count
                                messageIndex: model.index
                                hasContentComponent: !!myMessage.content && chatView.delegateMessagesContent.indexOf(model.content_type) > -1
                                canReplyToMessage: chatPage.canSendMessages
                                onReplyToMessage: {
                                    newMessageInReplyToRow.inReplyToMessage = myMessage
                                    newMessageTextField.focus = true
                                }
                                onEditMessage: {
                                    newMessageColumn.editMessageId = messageId
                                    newMessageTextField.text = Functions.getMessageText(myMessage, false, chatPage.myUserId, true)
                                    newMessageTextField.focus = true
                                }
                            }
                        }
                        Component {
                            id: messageListViewItemSimpleComponent
                            MessageListViewItemSimple {}
                        }
                        sourceComponent: chatView.simpleDelegateMessages.indexOf(model.content_type) > -1 ? messageListViewItemSimpleComponent : messageListViewItemComponent
                    }
                    VerticalScrollDecorator {}

                    ViewPlaceholder {
                        id: chatViewPlaceholder
                        enabled: chatView.count === 0
                        text: (chatPage.isSecretChat && !chatPage.isSecretChatReady) ? qsTr("This secret chat is not yet ready. Your chat partner needs to go online first.") : qsTr("This chat is empty.")
                    }
                }

                Column {
                    width: parent.width
                    height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium
                    spacing: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter

                    opacity: chatPage.loading ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    visible: chatPage.loading

                    InfoLabel {
                        id: loadingLabel
                        text: qsTr("Loading messages...")
                    }

                    BusyIndicator {
                        id: loadingBusyIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: chatPage.loading
                        size: BusyIndicatorSize.Large
                    }
                }

                Item {
                    id: chatUnreadMessagesItem
                    width: Theme.fontSizeHuge
                    height: Theme.fontSizeHuge
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingMedium
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingMedium
                    visible: !chatPage.loading && chatInformation.unread_count > 0 && chatOverviewItem.visible
                    Rectangle {
                        id: chatUnreadMessagesCountBackground
                        color: Theme.highlightBackgroundColor
                        anchors.fill: parent
                        radius: width / 2
                        visible: chatUnreadMessagesItem.visible
                    }

                    Text {
                        id: chatUnreadMessagesCount
                        font.pixelSize: Theme.fontSizeMedium
                        font.bold: true
                        color: Theme.primaryColor
                        anchors.centerIn: chatUnreadMessagesCountBackground
                        visible: chatUnreadMessagesItem.visible
                        text: chatInformation.unread_count > 99 ? "99+" : chatInformation.unread_count
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chatView.scrollToIndex(chatView.count - 1 - chatInformation.unread_count)
                        }
                    }
                }

                Loader {
                    id: stickerPickerLoader
                    active: false
                    asynchronous: true
                    width: parent.width
                    height: active ? parent.height : 0
                    source: "../components/StickerPicker.qml"
                }

                Connections {
                    target: stickerPickerLoader.item
                    onStickerPicked: {
                        Debug.log("Sticker picked: " + stickerId);
                        tdLibWrapper.sendStickerMessage(chatInformation.id, stickerId);
                        stickerPickerLoader.active = false;
                        attachmentOptionsFlickable.isNeeded = false;
                    }
                }

                Loader {
                    id: messageOverlayLoader

                    property var overlayMessage;

                    active: false
                    asynchronous: true
                    width: parent.width
                    height: active ? parent.height : 0
                    sourceComponent: Component {
                        MessageOverlayFlickable {
                            overlayMessage: messageOverlayLoader.overlayMessage
                            showHeader: !chatPage.isChannel
                            onRequestClose: {
                                messageOverlayLoader.active = false;
                            }
                        }
                    }
                }

                Loader {
                    id: voiceNoteOverlayLoader
                    active: false
                    asynchronous: true
                    width: parent.width
                    height: active ? parent.height : 0
                    source: "../components/VoiceNoteOverlay.qml"
                    onActiveChanged: {
                        if (!active) {
                            fernschreiberUtils.stopRecordingVoiceNote();
                        }
                    }
                }

                InlineQuery {
                    id: inlineQuery
                    textField: newMessageTextField
                    chatId: chatInformation.id
                }
            }

            Column {
                id: newMessageColumn
                spacing: Theme.paddingSmall
                topPadding: Theme.paddingSmall + inlineQuery.buttonPadding
                anchors.horizontalCenter: parent.horizontalCenter
                visible: height > 0
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                height: isNeeded ? implicitHeight : 0
                Behavior on height { SmoothedAnimation { duration: 200 } }

                readonly property bool isNeeded: !chatPage.isSelecting && chatPage.canSendMessages
                property string replyToMessageId: "0";
                property string editMessageId: "0";

                InReplyToRow {
                    onInReplyToMessageChanged: {
                        if (inReplyToMessage) {
                            newMessageColumn.replyToMessageId = newMessageInReplyToRow.inReplyToMessage.id.toString()
                            newMessageInReplyToRow.visible = true;
                        } else {
                            newMessageInReplyToRow.visible = false;
                            newMessageColumn.replyToMessageId = "0";
                        }
                    }

                    editable: true

                    onClearRequested: {
                        newMessageInReplyToRow.inReplyToMessage = null;
                    }

                    id: newMessageInReplyToRow
                    myUserId: chatPage.myUserId
                    visible: false
                }

                Flickable {
                    id: attachmentOptionsFlickable

                    property bool isNeeded: false
                    width: chatPage.width
                    x: -Theme.horizontalPageMargin
                    height: isNeeded && !inlineQuery.userNameIsValid ? attachmentOptionsRow.height : 0
                    Behavior on height { SmoothedAnimation { duration: 200 } }
                    visible: height > 0
                    contentHeight: attachmentOptionsRow.height
                    contentWidth: Math.max(width, attachmentOptionsRow.width)
                    property bool fadeRight: (attachmentOptionsRow.width-contentX) > width
                    property bool fadeLeft: !fadeRight && contentX > 0
                    layer.enabled: fadeRight || fadeLeft
                    layer.effect: OpacityRampEffectBase {
                        direction: attachmentOptionsFlickable.fadeRight ? OpacityRamp.LeftToRight : OpacityRamp.RightToLeft
                        source: attachmentOptionsFlickable
                        slope: 1 + 6 * (chatPage.width) / Screen.width
                        offset: 1 - 1 / slope
                    }


                    Row {
                        id: attachmentOptionsRow

                        height: attachImageIconButton.height

                        anchors.right: parent.right
                        layoutDirection: Qt.RightToLeft
                        spacing: Theme.paddingMedium
                        leftPadding: Theme.horizontalPageMargin
                        rightPadding: Theme.horizontalPageMargin

                        IconButton {
                            id: attachImageIconButton
                            visible: chatPage.hasSendPrivilege("can_send_media_messages")
                            icon.source: "image://theme/icon-m-image"
                            onClicked: {
                                var picker = pageStack.push("Sailfish.Pickers.ImagePickerPage", {
                                    allowedOrientations: chatPage.allowedOrientations
                                })
                                picker.selectedContentPropertiesChanged.connect(function(){
                                    attachmentOptionsFlickable.isNeeded = false;
                                    Debug.log("Selected document: ", picker.selectedContentProperties.filePath );
                                    attachmentPreviewRow.fileProperties = picker.selectedContentProperties;
                                    attachmentPreviewRow.isPicture = true;
                                    controlSendButton();
                                })
                            }
                        }
                        IconButton {
                            visible: chatPage.hasSendPrivilege("can_send_media_messages")
                            icon.source: "image://theme/icon-m-video"
                            onClicked: {
                                var picker = pageStack.push("Sailfish.Pickers.VideoPickerPage", {
                                    allowedOrientations: chatPage.allowedOrientations
                                })
                                picker.selectedContentPropertiesChanged.connect(function(){
                                    attachmentOptionsFlickable.isNeeded = false;
                                    Debug.log("Selected video: ", picker.selectedContentProperties.filePath );
                                    attachmentPreviewRow.fileProperties = picker.selectedContentProperties;
                                    attachmentPreviewRow.isVideo = true;
                                    controlSendButton();
                                })
                            }
                        }
                        IconButton {
                            visible: chatPage.hasSendPrivilege("can_send_media_messages")
                            icon.source: "image://theme/icon-m-mic"
                            icon.sourceSize {
                                width: Theme.iconSizeMedium
                                height: Theme.iconSizeMedium
                            }
                            highlighted: down || voiceNoteOverlayLoader.active
                            onClicked: {
                                voiceNoteOverlayLoader.active = !voiceNoteOverlayLoader.active;
                                stickerPickerLoader.active = false;
                            }
                        }
                        IconButton {
                            visible: chatPage.hasSendPrivilege("can_send_media_messages")
                            icon.source: "image://theme/icon-m-document"
                            onClicked: {
                                var picker = pageStack.push("Sailfish.Pickers.FilePickerPage", {
                                    allowedOrientations: chatPage.allowedOrientations
                                })
                                picker.selectedContentPropertiesChanged.connect(function(){
                                    attachmentOptionsFlickable.isNeeded = false;
                                    Debug.log("Selected document: ", picker.selectedContentProperties.filePath );
                                    attachmentPreviewRow.fileProperties = picker.selectedContentProperties;
                                    attachmentPreviewRow.isDocument = true;
                                    controlSendButton();
                                })
                            }
                        }
                        IconButton {
                            visible: chatPage.hasSendPrivilege("can_send_other_messages")
                            icon.source: "../../images/icon-m-sticker.svg"
                            icon.sourceSize {
                                width: Theme.iconSizeMedium
                                height: Theme.iconSizeMedium
                            }
                            highlighted: down || stickerPickerLoader.active
                            onClicked: {
                                stickerPickerLoader.active = !stickerPickerLoader.active;
                                voiceNoteOverlayLoader.active = false;
                            }
                        }
                        IconButton {
                            visible: !(chatPage.isPrivateChat || chatPage.isSecretChat) && chatPage.hasSendPrivilege("can_send_polls")
                            icon.source: "image://theme/icon-m-question"
                            onClicked: {
                                pageStack.push(Qt.resolvedUrl("../pages/PollCreationPage.qml"), { "chatId" : chatInformation.id, groupName: chatInformation.title});
                                attachmentOptionsFlickable.isNeeded = false;
                            }
                        }
                        IconButton {
                            visible: fernschreiberUtils.supportsGeoLocation() && newMessageTextField.text === ""
                            icon.source: "image://theme/icon-m-location"
                            icon.sourceSize {
                                width: Theme.iconSizeMedium
                                height: Theme.iconSizeMedium
                            }
                            onClicked: {
                                fernschreiberUtils.startGeoLocationUpdates();
                                attachmentOptionsFlickable.isNeeded = false;
                                attachmentPreviewRow.isLocation = true;
                                attachmentPreviewRow.attachmentDescription = qsTr("Location: Obtaining position...");
                                controlSendButton();
                            }
                        }
                    }

                }


                Row {
                    id: attachmentPreviewRow
                    visible: (!!locationData || !!fileProperties) && !inlineQuery.userNameIsValid
                    spacing: Theme.paddingMedium
                    width: parent.width
                    layoutDirection: Qt.RightToLeft
                    anchors.right: parent.right

                    property bool isPicture: false;
                    property bool isVideo: false;
                    property bool isDocument: false;
                    property bool isVoiceNote: false;
                    property bool isLocation: false;
                    property var locationData: null;
                    property var fileProperties: null;
                    property string attachmentDescription: "";

                    Connections {
                        target: fernschreiberUtils
                        onNewPositionInformation: {
                            attachmentPreviewRow.locationData = positionInformation;
                            if (attachmentPreviewRow.isLocation) {
                                attachmentPreviewRow.attachmentDescription = qsTr("Location (%1/%2)").arg(attachmentPreviewRow.locationData.latitude).arg(attachmentPreviewRow.locationData.longitude);
                            }
                        }
                    }

                    IconButton {
                        id: removeAttachmentsIconButton
                        icon.source: "image://theme/icon-m-clear"
                        onClicked: {
                            clearAttachmentPreviewRow();
                            controlSendButton();
                        }
                    }

                    Thumbnail {
                        id: attachmentPreviewImage
                        width: Theme.itemSizeMedium
                        height: Theme.itemSizeMedium
                        sourceSize.width: width
                        sourceSize.height: height

                        fillMode: Thumbnail.PreserveAspectCrop
                        mimeType: !!attachmentPreviewRow.fileProperties ? attachmentPreviewRow.fileProperties.mimeType || "" : ""
                        source: !!attachmentPreviewRow.fileProperties ? attachmentPreviewRow.fileProperties.url || "" : ""
                        visible: attachmentPreviewRow.isPicture || attachmentPreviewRow.isVideo
                    }

                    Label {
                        id: attachmentPreviewText
                        font.pixelSize: Theme.fontSizeSmall
                        text: ( attachmentPreviewRow.isVoiceNote || attachmentPreviewRow.isLocation ) ? attachmentPreviewRow.attachmentDescription : ( !!attachmentPreviewRow.fileProperties ? attachmentPreviewRow.fileProperties.fileName || "" : "" );
                        anchors.verticalCenter: parent.verticalCenter

                        maximumLineCount: 1
                        truncationMode: TruncationMode.Fade
                        color: Theme.secondaryColor
                        visible: attachmentPreviewRow.isDocument || attachmentPreviewRow.isVoiceNote || attachmentPreviewRow.isLocation
                    }
                }

                Row {
                    id: uploadStatusRow
                    visible: false
                    spacing: Theme.paddingMedium
                    width: parent.width
                    anchors.right: parent.right

                    Text {
                        id: uploadingText
                        font.pixelSize: Theme.fontSizeSmall
                        text: qsTr("Uploading...")
                        anchors.verticalCenter: parent.verticalCenter
                        color: Theme.secondaryColor
                        visible: uploadStatusRow.visible
                    }

                    ProgressBar {
                        id: uploadingProgressBar
                        minimumValue: 0
                        maximumValue: 100
                        value: 0
                        visible: uploadStatusRow.visible
                        width: parent.width - uploadingText.width - Theme.paddingMedium
                    }

                }

                Column {
                    id: emojiColumn
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: emojiProposals ? ( emojiProposals.length > 0 ? true : false ) : false
                    opacity: emojiProposals ? ( emojiProposals.length > 0 ? 1 : 0 ) : 0
                    Behavior on opacity { NumberAnimation {} }
                    spacing: Theme.paddingMedium

                    Flickable {
                        width: parent.width
                        height: emojiResultRow.height + Theme.paddingSmall
                        anchors.horizontalCenter: parent.horizontalCenter
                        contentWidth: emojiResultRow.width
                        clip: true
                        Row {
                            id: emojiResultRow
                            spacing: Theme.paddingMedium
                            Repeater {
                                model: emojiProposals

                                Item {
                                    height: singleEmojiRow.height
                                    width: singleEmojiRow.width

                                    Row {
                                        id: singleEmojiRow
                                        spacing: Theme.paddingSmall

                                        Image {
                                            id: emojiPicture
                                            source: "../js/emoji/" + modelData.file_name
                                            width: Theme.fontSizeLarge
                                            height: Theme.fontSizeLarge
                                        }

                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            replaceMessageText(newMessageTextField.text, newMessageTextField.cursorPosition, modelData.emoji);
                                            emojiProposals = null;
                                        }
                                    }
                                }

                            }
                        }
                    }
                }

                Column {
                    id: atMentionColumn
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: opacity > 0
                    opacity: knownUsersRepeater.count > 0 ? 1 : 0
                    Behavior on opacity { NumberAnimation {} }
                    height: knownUsersRepeater.count > 0 ? childrenRect.height : 0
                    Behavior on height { SmoothedAnimation { duration: 200 } }
                    spacing: Theme.paddingMedium

                    Flickable {
                        width: parent.width
                        height: atMentionResultRow.height + Theme.paddingSmall
                        anchors.horizontalCenter: parent.horizontalCenter
                        contentWidth: atMentionResultRow.width
                        clip: true
                        Row {
                            id: atMentionResultRow
                            spacing: Theme.paddingMedium
                            Repeater {
                                id: knownUsersRepeater

                                Item {
                                    id: knownUserItem
                                    height: singleAtMentionRow.height
                                    width: singleAtMentionRow.width

                                    property string atMentionText: "@" + (user_name ? user_name : user_id + "(" + title + ")");

                                    Row {
                                        id: singleAtMentionRow
                                        spacing: Theme.paddingSmall

                                        Item {
                                            width: Theme.fontSizeHuge
                                            height: Theme.fontSizeHuge
                                            anchors.verticalCenter: parent.verticalCenter
                                            ProfileThumbnail {
                                                id: atMentionThumbnail
                                                replacementStringHint: title
                                                width: parent.width
                                                height: parent.width
                                                photoData: photo_small
                                            }
                                        }

                                        Column {
                                            Text {
                                                text: Emoji.emojify(title, Theme.fontSizeExtraSmall)
                                                textFormat: Text.StyledText
                                                color: Theme.primaryColor
                                                font.pixelSize: Theme.fontSizeExtraSmall
                                                font.bold: true
                                            }
                                            Text {
                                                id: userHandleText
                                                text: user_handle
                                                textFormat: Text.StyledText
                                                color: Theme.primaryColor
                                                font.pixelSize: Theme.fontSizeExtraSmall
                                            }
                                        }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            replaceMessageText(newMessageTextField.text, newMessageTextField.cursorPosition, knownUserItem.atMentionText);
                                            knownUsersRepeater.model = undefined;
                                        }
                                    }
                                }

                            }
                        }
                    }
                }

                Row {
                    width: parent.width
                    spacing: Theme.paddingSmall
                    visible: newMessageColumn.editMessageId !== "0"

                    Text {
                        width: parent.width - Theme.paddingSmall - removeEditMessageIconButton.width

                        anchors.verticalCenter: parent.verticalCenter

                        id: editMessageText
                        font.pixelSize: Theme.fontSizeSmall
                        font.bold: true
                        text: qsTr("Edit Message")
                        color: Theme.secondaryColor
                    }

                    IconButton {
                        id: removeEditMessageIconButton
                        icon.source: "image://theme/icon-m-clear"
                        onClicked: {
                            newMessageColumn.editMessageId = "0";
                            newMessageTextField.text = "";
                        }
                    }
                }

                Row {
                    id: newMessageRow
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter

                    TextArea {
                        id: newMessageTextField
                        width: parent.width - (attachmentIconButton.visible ? attachmentIconButton.width : 0) - (newMessageSendButton.visible ? newMessageSendButton.width : 0) - (cancelInlineQueryButton.visible ? cancelInlineQueryButton.width : 0)
                        height: Math.min(chatContainer.height / 3, implicitHeight)
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: Theme.fontSizeSmall
                        placeholderText: qsTr("Your message")
                        labelVisible: false
                        textLeftMargin: 0
                        textTopMargin: 0
                        enabled: !attachmentPreviewRow.isLocation
                        EnterKey.onClicked: {
                            if (appSettings.sendByEnter) {
                                sendMessage();
                                newMessageTextField.text = "";
                                if(!appSettings.focusTextAreaAfterSend) {
                                    newMessageTextField.focus = false;
                                }
                            }
                        }

                        EnterKey.enabled: !inlineQuery.userNameIsValid && (!appSettings.sendByEnter || text.length)
                        EnterKey.iconSource: appSettings.sendByEnter ? "image://theme/icon-m-chat" : "image://theme/icon-m-enter"

                        onTextChanged: {
                            controlSendButton();
                            textReplacementTimer.restart();
                        }
                    }

                    IconButton {
                        id: attachmentIconButton
                        icon.source: "image://theme/icon-m-attach?" +  (attachmentOptionsFlickable.isNeeded ? Theme.highlightColor : Theme.primaryColor)
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingSmall
                        enabled: !attachmentPreviewRow.visible
                        visible: !inlineQuery.userNameIsValid
                        onClicked: {
                            if (attachmentOptionsFlickable.isNeeded) {
                                attachmentOptionsFlickable.isNeeded = false;
                                stickerPickerLoader.active = false;
                                voiceNoteOverlayLoader.active = false;
                            } else {
                                attachmentOptionsFlickable.isNeeded = true;
                            }
                        }
                    }

                    IconButton {
                        id: newMessageSendButton
                        icon.source: "image://theme/icon-m-chat"
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingSmall
                        visible: !inlineQuery.userNameIsValid && (!appSettings.sendByEnter || attachmentPreviewRow.visible)
                        enabled: false
                        onClicked: {
                            sendMessage();
                            newMessageTextField.text = "";
                            if(!appSettings.focusTextAreaAfterSend) {
                                newMessageTextField.focus = false;
                            }
                        }
                    }

                    Item {
                        width: cancelInlineQueryButton.width
                        height: cancelInlineQueryButton.height
                        visible: inlineQuery.userNameIsValid
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingSmall

                        IconButton {
                            id: cancelInlineQueryButton
                            icon.source: "image://theme/icon-m-cancel"
                            visible: parent.visible
                            opacity: inlineQuery.isLoading ? 0.2 : 1
                            Behavior on opacity { FadeAnimation {} }
                            onClicked: {
                                if(inlineQuery.query !== "") {
                                    newMessageTextField.text = "@" + inlineQuery.userName + " "
                                    newMessageTextField.cursorPosition = newMessageTextField.text.length
                                    lostFocusTimer.start();
                                } else {
                                    newMessageTextField.text = ""
                                }
                            }
                            onPressAndHold: {
                                newMessageTextField.text = ""
                            }
                        }

                        BusyIndicator {
                            size: BusyIndicatorSize.Small
                            anchors.centerIn: parent
                            running: inlineQuery.isLoading
                        }
                    }


                }
            }
        }
    }

    Loader {
        id: selectedMessagesActions
        asynchronous: true
        anchors.bottom: parent.bottom
        readonly property bool isNeeded: chatPage.isSelecting
        active: height > 0
        width: parent.width
        height: isNeeded ? Theme.itemSizeMedium : 0
        Behavior on height { SmoothedAnimation { duration: 200 } }
        sourceComponent: Component {
            Item {
                clip: true

                IconButton {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        verticalCenter: parent.verticalCenter
                    }
                    icon.source: "image://theme/icon-m-cancel"
                    onClicked: {
                        chatPage.selectedMessages = [];
                    }
                }

                Row {
                    spacing: Theme.paddingSmall
                    anchors {
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin
                        verticalCenter: parent.verticalCenter
                    }

                    IconButton {
                        icon.source: "../../images/icon-m-copy.svg"
                        icon.sourceSize: Qt.size(Theme.iconSizeMedium, Theme.iconSizeMedium)
                        onClicked: {
                            Clipboard.text = Functions.getMessagesArrayText(chatPage.selectedMessages);
                            appNotification.show(qsTr("%Ln messages have been copied", "", selectedMessages.length));
                            chatPage.selectedMessages = [];
                        }
                    }

                    IconButton {
                        visible: !chatPage.isSecretChat && selectedMessages.every(function(message){
                            return message.can_be_forwarded
                        })
                        icon.sourceSize: Qt.size(Theme.iconSizeMedium, Theme.iconSizeMedium)
                        icon.source: "image://theme/icon-m-forward"
                        onClicked: {
                            var ids = Functions.getMessagesArrayIds(chatPage.selectedMessages)
                            var neededPermissions = Functions.getMessagesNeededForwardPermissions(chatPage.selectedMessages)
                            var chatId = chatInformation.id
                            pageStack.push(Qt.resolvedUrl("../pages/ChatSelectionPage.qml"), {
                                myUserId: chatPage.myUserId,
                                headerDescription: qsTr("Forward %Ln messages", "dialog header", ids.length),
                                payload: {fromChatId: chatId, messageIds:ids, neededPermissions: neededPermissions},
                                state: "forwardMessages"
                            })
                        }

                    }
                    IconButton {
                        icon.source: "image://theme/icon-m-delete"
                        visible: chatInformation.id === chatPage.myUserId || selectedMessages.every(function(message){
                            return message.can_be_deleted_for_all_users
                        })
                        icon.sourceSize: Qt.size(Theme.iconSizeMedium, Theme.iconSizeMedium)
                        onClicked: {
                            var ids = Functions.getMessagesArrayIds(selectedMessages);
                            var chatId = chatInformation.id
                            var wrapper = tdLibWrapper;
                            Remorse.popupAction(chatPage, qsTr("%Ln Messages deleted", "", ids.length), function() {
                                wrapper.deleteMessages(chatId, ids);
                            });
                            chatPage.selectedMessages = [];
                        }
                    }
                }
            }
        }
    }
}
