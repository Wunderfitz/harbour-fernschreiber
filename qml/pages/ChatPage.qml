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
import Sailfish.Pickers 1.0
import Nemo.Thumbnailer 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
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
    property bool isPrivateChat: false;
    property bool isBasicGroup: false;
    property bool isSuperGroup: false;
    property bool isChannel: false;
    property var chatPartnerInformation;
    property var chatGroupInformation;
    property int chatOnlineMemberCount: 0;
    property var emojiProposals;
    readonly property bool userIsMember: (isPrivateChat && chatInformation["@type"]) || // should be optimized
                                (isBasicGroup || isSuperGroup) && (
                                    (chatGroupInformation.status["@type"] === "chatMemberStatusMember")
                                    || (chatGroupInformation.status["@type"] === "chatMemberStatusAdministrator")
                                    || (chatGroupInformation.status["@type"] === "chatMemberStatusRestricted" && chatGroupInformation.status.is_member)
                                    || (chatGroupInformation.status["@type"] === "chatMemberStatusCreator" && chatGroupInformation.status.is_member)
                                    )

    function updateChatPartnerStatusText() {
        var statusText = Functions.getChatPartnerStatusText(chatPartnerInformation.status['@type'], chatPartnerInformation.status.was_online);
        if(statusText) {
            chatStatusText.text = statusText;
        }
    }

    function updateGroupStatusText() {
        if (chatOnlineMemberCount > 0) {
            chatStatusText.text = qsTr("%1 members, %2 online").arg(Functions.getShortenedCount(chatGroupInformation.member_count)).arg(Functions.getShortenedCount(chatOnlineMemberCount));
        } else {
            if (isChannel) {
                chatStatusText.text = qsTr("%1 subscribers").arg(Functions.getShortenedCount(chatGroupInformation.member_count));
            } else {
                chatStatusText.text = qsTr("%1 members").arg(Functions.getShortenedCount(chatGroupInformation.member_count));
            }
        }
        joinLeaveChatMenuItem.text = chatPage.userIsMember ? qsTr("Leave Chat") : qsTr("Join Chat");
    }

    function initializePage() {
        console.log("[ChatPage] Initializing chat page...");
        chatView.currentIndex = -1;
        chatView.lastReadSentIndex = 0;
        var chatType = chatInformation.type['@type'];
        isPrivateChat = ( chatType === "chatTypePrivate" );
        isBasicGroup = ( chatType === "chatTypeBasicGroup" );
        isSuperGroup = ( chatType === "chatTypeSupergroup" );
        if (isPrivateChat) {
            chatPartnerInformation = tdLibWrapper.getUserInformation(chatInformation.type.user_id);
            updateChatPartnerStatusText();
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
            console.log("Stickers will be reloaded!");
            tdLibWrapper.getRecentStickers();
            tdLibWrapper.getInstalledStickerSets();
            stickerManager.setNeedsReload(false);
        }
    }

    function getMessageStatusText(message, listItemIndex, lastReadSentIndex, useElapsed) {
        var messageStatusSuffix = "";
        if(!message) {
            return "";
        }

        if (message.edit_date > 0) {
            messageStatusSuffix += " - " + qsTr("edited");
        }

        if (chatPage.myUserId === message.sender_user_id) {
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
        attachmentPreviewRow.visible = false;
        attachmentPreviewRow.isPicture = false;
        attachmentPreviewRow.isVideo = false;
        attachmentPreviewRow.isDocument = false;
        attachmentPreviewRow.fileProperties = {};
    }

    function controlSendButton() {
        if (newMessageTextField.text.length !== 0
                || attachmentPreviewRow.isPicture
                || attachmentPreviewRow.isDocument
                || attachmentPreviewRow.isVideo) {
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
                clearAttachmentPreviewRow();
            } else {
                tdLibWrapper.sendTextMessage(chatInformation.id, newMessageTextField.text, newMessageColumn.replyToMessageId);
            }
        }
        controlSendButton();
        newMessageInReplyToRow.inReplyToMessage = null;
        newMessageColumn.editMessageId = "0";
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
        var wordBoundaries = getWordBoundaries(text, cursorPosition);

        var currentWord = text.substring(wordBoundaries.beginIndex, wordBoundaries.endIndex);
        if (currentWord.length > 1 && currentWord.charAt(0) === ':') {
            tdLibWrapper.searchEmoji(currentWord.substring(1));
        } else {
            chatPage.emojiProposals = null;
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

    Component.onCompleted: {
        initializePage();
    }

    Component.onDestruction: {
        tdLibWrapper.closeChat(chatInformation.id);
    }

    onStatusChanged: {
        switch(status) {
        case PageStatus.Activating:
            tdLibWrapper.openChat(chatInformation.id);
            break;
        case PageStatus.Active:
            if (!chatPage.isInitialized) {
                chatModel.initialize(chatInformation);
                chatPage.isInitialized = true;
            }
            break;
        }
    }

    Connections {
        target: tdLibWrapper
        onUserUpdated: {
            if (isPrivateChat && chatPartnerInformation.id.toString() === userId ) {
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
            console.log(isSuperGroup + "/" + isBasicGroup + "/" + chatInformation.id.toString() + "/" + chatId);
            if ((isSuperGroup || isBasicGroup) && chatInformation.id.toString() === chatId) {
                chatOnlineMemberCount = onlineMemberCount;
                updateGroupStatusText();
            }
        }
        onFileUpdated: {
            uploadStatusRow.visible = fileInformation.remote.is_uploading_active;
            uploadingProgressBar.maximumValue = fileInformation.size;
            uploadingProgressBar.value = fileInformation.remote.uploaded_size;
        }
        onEmojiSearchSuccessful: {
            chatPage.emojiProposals = result;
        }
    }

    Connections {
        target: chatModel
        onMessagesReceived: {
            console.log("[ChatPage] Messages received, view has " + chatView.count + " messages, setting view to index " + modelIndex + ", own messages were read before index " + lastReadSentIndex);
            if(totalCount === 0) {
                console.log("[ChatPage] actually, skipping that: No Messages in Chat.");
                chatView.positionViewAtEnd();
                chatPage.loading = false;
                return;
            }

            chatView.lastReadSentIndex = lastReadSentIndex;
            chatView.scrollToIndex(modelIndex);
            chatPage.loading = false;
            if (chatView.height > chatView.contentHeight) {
                console.log("[ChatPage] Chat content quite small...");
                viewMessageTimer.queueViewMessage(chatView.count - 1);
            }
        }
        onNewMessageReceived: {
            if (chatView.manuallyScrolledToBottom || message.sender_user_id === chatPage.myUserId) {
                console.log("[ChatPage] Own message received or was scrolled to bottom, scrolling down to see it...");
                chatView.scrollToIndex(chatView.count - 1);
            }
        }
        onUnreadCountUpdated: {
            console.log("[ChatPage] Unread count updated, new count: " + unreadCount);
            chatInformation.unread_count = unreadCount;
            chatUnreadMessagesCountBackground.visible = ( !chatPage.loading && unreadCount > 0 );
            chatUnreadMessagesCount.text = unreadCount > 99 ? "99+" : unreadCount;
        }
        onLastReadSentMessageUpdated: {
            console.log("[ChatPage] Updating last read sent index, new index: " + lastReadSentIndex);
            chatView.lastReadSentIndex = lastReadSentIndex;
        }
        onMessagesIncrementalUpdate: {
            console.log("Incremental update received. View now has " + chatView.count + " messages, view is on index " + modelIndex + ", own messages were read before index " + lastReadSentIndex);
            chatView.lastReadSentIndex = lastReadSentIndex;
            chatViewCooldownTimer.start();
        }
        onNotificationSettingsUpdated: {
            chatInformation = chatModel.getChatInformation();
            muteChatMenuItem.text = chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat");
        }
    }

    Connections {
        target: chatListModel
        onChatJoined: {
            chatPageNotification.show(qsTr("You joined the chat %1").arg(chatTitle));
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
        running: isPrivateChat
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
                if(messageToRead && messageToRead.id) {
                    tdLibWrapper.viewMessage(chatInformation.id, messageToRead.id, false);
                }
                lastQueuedIndex = -1
            }
        }
    }

    SilicaFlickable {
        id: chatContainer
        anchors.fill: parent
        contentHeight: height
        contentWidth: width

        PullDownMenu {
            visible: chatInformation.id !== chatPage.myUserId && !stickerPickerLoader.active
            MenuItem {
                id: joinLeaveChatMenuItem
                visible: (chatPage.isSuperGroup || chatPage.isBasicGroup) && chatGroupInformation && chatGroupInformation.status["@type"] !== "chatMemberStatusBanned"
                onClicked: {
                    if (chatPage.userIsMember) {
                        var remorse = Remorse.popupAction(appWindow, qsTr("Leaving chat"), (function(chatid) {
                            return function() {
                                tdLibWrapper.leaveChat(chatid);
                                // this does not care about the response (ideally type "ok" without further reference) for now
                                pageStack.pop(pageStack.find( function(page){ return(page._depth === 0)} ));
                            };
                        }(chatInformation.id)))
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
                    tdLibWrapper.setChatNotificationSettings(chatInformation.id, newNotificationSettings);
                }
                text: chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat")
            }
        }

        AppNotification {
            id: chatPageNotification
        }

        BackgroundItem {
            id: headerMouseArea
            height: headerRow.height
            width: parent.width
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ChatInformationPage.qml"), { "chatInformation" : chatInformation, "privateChatUserInformation": chatPartnerInformation, "groupInformation": chatGroupInformation, "chatOnlineMemberCount": chatOnlineMemberCount});
            }
        }
        Column {
            id: chatColumn
            width: parent.width
            height: parent.height
            spacing: Theme.paddingSmall

            Row {
                id: headerRow
                width: parent.width - (3 * Theme.horizontalPageMargin)
                height: chatOverviewColumn.height + Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium

                ProfileThumbnail {
                    id: chatPictureThumbnail
                    photoData: (typeof chatInformation.photo !== "undefined") ? chatInformation.photo.small : ""
                    replacementStringHint: chatNameText.text
                    width: chatOverviewColumn.height
                    height: chatOverviewColumn.height
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingSmall
                }

                Column {
                    id: chatOverviewColumn
                    width: parent.width - chatPictureThumbnail.width - Theme.paddingMedium
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingSmall
                    Text {
                        id: chatNameText
                        text: chatInformation.title !== "" ? Emoji.emojify(chatInformation.title, font.pixelSize) : qsTr("Unknown")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeLarge
                        font.family: Theme.fontFamilyHeading
                        color: Theme.highlightColor
                        elide: Text.ElideRight
                        width: parent.width
                        maximumLineCount: 1
                        horizontalAlignment: Text.AlignRight
                        onTruncatedChanged: {
                            // There is obviously a bug in QML in truncating text with images.
                            // We simply remove Emojis then...
                            if (truncated) {
                                text = text.replace(/\<img [^>]+\/\>/g, "");
                            }
                        }
                    }
                    Text {
                        id: chatStatusText
                        text: ""
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        font.family: Theme.fontFamilyHeading
                        color: headerMouseArea.pressed ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        elide: Text.ElideRight
                        width: parent.width
                        maximumLineCount: 1
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }

            Item {
                id: chatViewItem
                width: parent.width
                height: parent.height - headerRow.height - Theme.paddingSmall - ( chatPage.isChannel ? 0 : ( newMessageColumn.height + Theme.paddingSmall ) )

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
                        console.log("[ChatPage] Cooldown completed...");
                        chatView.inCooldown = false;
                    }
                }


                SilicaListView {
                    id: chatView

                    anchors.fill: parent
                    opacity: chatPage.loading ? 0 : 1
                    Behavior on opacity { NumberAnimation {} }
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
                        readonly property int backgroundWidth: textItemWidth - pageMarginDouble
                        readonly property int backgroundRadius: textItemWidth/50
                        readonly property int textColumnWidth: backgroundWidth - Theme.horizontalPageMargin
                        readonly property int messageInReplyToHeight: Theme.fontSizeExtraSmall * 2.571428571 + Theme.paddingSmall;
                        readonly property int webPagePreviewHeight: ( (textColumnWidth * 2 / 3) + (6 * Theme.fontSizeExtraSmall) + ( 7 * Theme.paddingSmall) )
                    }

                    function handleScrollPositionChanged() {
                        console.log("Current position: " + chatView.contentY);
                        if(chatInformation.unread_count > 0) {
                            var bottomIndex = chatView.indexAt(chatView.contentX, ( chatView.contentY + chatView.height - Theme.horizontalPageMargin ));
                            if(bottomIndex > -1) {
                                viewMessageTimer.queueViewMessage(bottomIndex)
                            }
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
                        if (!chatPage.loading && !chatView.inCooldown && chatView.indexAt(chatView.contentX, chatView.contentY) < 10) {
                            console.log("[ChatPage] Trying to get older history items...");
                            chatView.inCooldown = true;
                            chatModel.triggerLoadMoreHistory();
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
                    readonly property var contentComponentNames: ({
                                                              messageSticker: "StickerPreview",
                                                              messagePhoto: "ImagePreview",
                                                              messageVideo: "VideoPreview",
                                                              messageAnimation: "VideoPreview",
                                                              messageAudio: "AudioPreview",
                                                              messageVoiceNote: "AudioPreview",
                                                              messageDocument: "DocumentPreview",
                                                              messageLocation: "LocationPreview",
                                                              messageVenue: "LocationPreview",
                                                              messagePoll: "PollPreview"
                                                          })
                    function getContentComponentHeight(componentName, content, parentWidth) {
                        switch(componentName) {
                        case "StickerPreview": return content.sticker.height;
                        case "ImagePreview":
                        case "LocationPreview":
                            return parentWidth * 0.66666666; // 2 / 3;
                        case "VideoPreview":
                            return Functions.getVideoHeight(parentWidth, ( content['@type'] === "messageVideo" ) ? content.video : content.animation);
                        case "AudioPreview":
                            return parentWidth / 2;
                        case "DocumentPreview":
                            return Theme.itemSizeSmall;
                        case "PollPreview":
                            return Theme.itemSizeSmall * (4 + content.poll.options);
                        }
                    }

                    readonly property var simpleDelegateMessages: ["messageBasicGroupChatCreate", "messageChatAddMembers", "messageChatChangePhoto", "messageChatChangeTitle", "messageChatDeleteMember", "messageChatDeletePhoto", "messageChatJoinByLink", "messageChatSetTtl", "messageChatUpgradeFrom", "messageChatUpgradeTo", "messageCustomServiceAction", "messagePinMessage", "messageScreenshotTaken", "messageSupergroupChatCreate", "messageUnsupported"]
                    delegate: Loader {
                        width: chatView.width
                        Component {
                            id: messageListViewItemComponent
                            MessageListViewItem {
                                precalculatedValues: chatView.precalculatedValues
                            }
                        }
                        Component {
                            id: messageListViewItemSimpleComponent
                            MessageListViewItemSimple {}
                        }
                        sourceComponent: chatView.simpleDelegateMessages.indexOf(display.content['@type']) > -1 ? messageListViewItemSimpleComponent : messageListViewItemComponent
                    }
                    VerticalScrollDecorator {}

                    ViewPlaceholder {
                        enabled: chatView.count === 0
                        text: qsTr("This chat is empty.")
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
                    Rectangle {
                        id: chatUnreadMessagesCountBackground
                        color: Theme.highlightBackgroundColor
                        anchors.fill: parent
                        radius: width / 2
                        visible: !chatPage.loading && chatInformation.unread_count > 0
                    }

                    Text {
                        id: chatUnreadMessagesCount
                        font.pixelSize: Theme.fontSizeMedium
                        font.bold: true
                        color: Theme.primaryColor
                        anchors.centerIn: chatUnreadMessagesCountBackground
                        visible: chatUnreadMessagesCountBackground.visible
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

            }

            Column {
                id: newMessageColumn
                spacing: Theme.paddingSmall

                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !chatPage.isChannel

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

                Row {
                    id: attachmentOptionsRow
                    visible: false
                    anchors.right: parent.right
                    width: parent.width
                    layoutDirection: Qt.RightToLeft
                    spacing: Theme.paddingMedium
                    IconButton {
                        id: imageAttachmentButton
                        icon.source: "image://theme/icon-m-image"
                        onClicked: {
                            var picker = pageStack.push("Sailfish.Pickers.ImagePickerPage");
                            picker.selectedContentPropertiesChanged.connect(function(){
                                attachmentOptionsRow.visible = false;
                                console.log("Selected document: " + picker.selectedContentProperties.filePath );
                                attachmentPreviewRow.fileProperties = picker.selectedContentProperties;
                                attachmentPreviewRow.isPicture = true;
                                attachmentPreviewRow.visible = true;
                                controlSendButton();
                            })
                        }
                    }
                    IconButton {
                        id: videoAttachmentButton
                        icon.source: "image://theme/icon-m-video"
                        onClicked: {
                            var picker = pageStack.push("Sailfish.Pickers.VideoPickerPage");
                            picker.selectedContentPropertiesChanged.connect(function(){
                                attachmentOptionsRow.visible = false;
                                console.log("Selected video: " + picker.selectedContentProperties.filePath );
                                attachmentPreviewRow.fileProperties = picker.selectedContentProperties;
                                attachmentPreviewRow.isVideo = true;
                                attachmentPreviewRow.visible = true;
                                controlSendButton();
                            })
                        }
                    }
                    IconButton {
                        id: documentAttachmentButton
                        icon.source: "image://theme/icon-m-document"
                        onClicked: {
                            var picker = pageStack.push("Sailfish.Pickers.DocumentPickerPage");
                            picker.selectedContentPropertiesChanged.connect(function(){
                                attachmentOptionsRow.visible = false;
                                console.log("Selected document: " + picker.selectedContentProperties.filePath );
                                attachmentPreviewRow.fileProperties = picker.selectedContentProperties;
                                attachmentPreviewRow.isDocument = true;
                                attachmentPreviewRow.visible = true;
                                controlSendButton();
                            })
                        }
                    }

                    IconButton {
                        icon.source: "../../images/icon-m-sticker.svg"
                        icon.sourceSize {
                            width: Theme.iconSizeMedium
                            height: Theme.iconSizeMedium
                        }

                        highlighted: down || stickerPickerLoader.active
                        onClicked: {
                            stickerPickerLoader.active = !stickerPickerLoader.active;
                        }
                    }
                    IconButton {
                        visible: !chatPage.isPrivateChat && chatGroupInformation &&
                                 (chatGroupInformation.status["@type"] === "chatMemberStatusCreator"
                                  || chatGroupInformation.status["@type"] === "chatMemberStatusAdministrator"
                                  || (chatGroupInformation.status["@type"] === "chatMemberStatusMember" && chatInformation.permissions.can_send_polls))
                        icon.source: "image://theme/icon-m-question"
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("../pages/PollCreationPage.qml"), { "chatId" : chatInformation.id, groupName: chatInformation.title});
                            attachmentOptionsRow.visible = false;
                        }
                    }
                }

                Row {
                    id: attachmentPreviewRow
                    visible: false
                    spacing: Theme.paddingMedium
                    width: parent.width
                    layoutDirection: Qt.RightToLeft
                    anchors.right: parent.right

                    property bool isPicture: false;
                    property bool isVideo: false;
                    property bool isDocument: false;
                    property var fileProperties:({});

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
                        mimeType: typeof attachmentPreviewRow.fileProperties !== "undefined" ? attachmentPreviewRow.fileProperties.mimeType || "" : ""
                        source: typeof attachmentPreviewRow.fileProperties !== "undefined" ? attachmentPreviewRow.fileProperties.url || "" : ""
                        visible: attachmentPreviewRow.isPicture || attachmentPreviewRow.isVideo
                    }

                    Text {
                        id: attachmentPreviewText
                        font.pixelSize: Theme.fontSizeSmall
                        text: typeof attachmentPreviewRow.fileProperties !== "undefined" ? attachmentPreviewRow.fileProperties.fileName || "" : "";
                        anchors.verticalCenter: parent.verticalCenter

                        maximumLineCount: 1
                        elide: Text.ElideRight
                        color: Theme.secondaryColor
                        visible: attachmentPreviewRow.isDocument
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
                        width: parent.width - attachmentIconButton.width - newMessageSendButton.width
                        height: Math.min(chatContainer.height / 3, implicitHeight)
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: Theme.fontSizeSmall
                        placeholderText: qsTr("Your message")
                        labelVisible: false
                        textLeftMargin: 0
                        textTopMargin: 0
                        EnterKey.onClicked: {
                            if (appSettings.sendByEnter) {
                                sendMessage();
                                newMessageTextField.text = "";
                                newMessageTextField.focus = false;
                            }
                        }

                        EnterKey.enabled: !appSettings.sendByEnter || text.length
                        EnterKey.iconSource: appSettings.sendByEnter ? "image://theme/icon-m-chat" : "image://theme/icon-m-enter"

                        onTextChanged: {
                            controlSendButton();
                            textReplacementTimer.restart();
                        }
                    }

                    IconButton {
                        id: attachmentIconButton
                        icon.source: attachmentOptionsRow.visible ? "image://theme/icon-m-attach?" + Theme.highlightColor : "image://theme/icon-m-attach?" + Theme.primaryColor

                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingSmall
                        enabled: !attachmentPreviewRow.visible
                        onClicked: {
                            if (attachmentOptionsRow.visible) {
                                attachmentOptionsRow.visible = false;
                                stickerPickerLoader.active = false;
                            } else {
                                attachmentOptionsRow.visible = true;
                            }
                        }
                    }

                    IconButton {
                        id: newMessageSendButton
                        icon.source: "image://theme/icon-m-chat"
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingSmall
                        enabled: false
                        onClicked: {
                            sendMessage();
                            newMessageTextField.text = "";
                            newMessageTextField.focus = false;
                        }
                    }
                }
            }
        }
    }

}
