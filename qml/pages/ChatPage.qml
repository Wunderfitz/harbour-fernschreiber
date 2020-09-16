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
import QtQuick 2.5
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: chatPage
    allowedOrientations: Orientation.All

    property bool loading: true;
    property bool isInitialized: false;
    property int myUserId: tdLibWrapper.getUserInformation().id;
    property variant chatInformation;
    property bool isPrivateChat: false;
    property bool isBasicGroup: false;
    property bool isSuperGroup: false;
    property bool isChannel: false;
    property variant chatPartnerInformation;
    property variant chatGroupInformation;
    property int chatOnlineMemberCount: 0;

    function getShortenedCount(count) {
        if (count >= 1000000) {
            return qsTr("%1M").arg((count / 1000000).toLocaleString(Qt.locale(), 'f', 0));
        } else if (count >= 1000 ) {
            return qsTr("%1K").arg((count / 1000).toLocaleString(Qt.locale(), 'f', 0));
        } else {
            return count;
        }
    }

    function updateChatPartnerStatusText() {
        if (chatPartnerInformation.status['@type'] === "userStatusEmpty" ) {
            chatStatusText.text = qsTr("was never online");
        }
        if (chatPartnerInformation.status['@type'] === "userStatusLastMonth" ) {
            chatStatusText.text = qsTr("offline, last online: last month");
        }
        if (chatPartnerInformation.status['@type'] === "userStatusLastWeek" ) {
            chatStatusText.text = qsTr("offline, last online: last week");
        }
        if (chatPartnerInformation.status['@type'] === "userStatusOffline" ) {
            chatStatusText.text = qsTr("offline, last online: %1").arg(Functions.getDateTimeElapsed(chatPartnerInformation.status.was_online));
        }
        if (chatPartnerInformation.status['@type'] === "userStatusOnline" ) {
            chatStatusText.text = qsTr("online");
        }
        if (chatPartnerInformation.status['@type'] === "userStatusRecently" ) {
            chatStatusText.text = qsTr("offline, was recently online");
        }
    }

    function updateGroupStatusText() {
        if (chatOnlineMemberCount > 0) {
            chatStatusText.text = qsTr("%1 members, %2 online").arg(getShortenedCount(chatGroupInformation.member_count)).arg(getShortenedCount(chatOnlineMemberCount));
        } else {
            if (isChannel) {
                chatStatusText.text = qsTr("%1 subscribers").arg(getShortenedCount(chatGroupInformation.member_count));
            } else {
                chatStatusText.text = qsTr("%1 members").arg(getShortenedCount(chatGroupInformation.member_count));
            }
        }
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
        if (isBasicGroup) {
            chatGroupInformation = tdLibWrapper.getBasicGroup(chatInformation.type.basic_group_id);
            updateGroupStatusText();
        }
        if (isSuperGroup) {
            chatGroupInformation = tdLibWrapper.getSuperGroup(chatInformation.type.supergroup_id);
            isChannel = chatGroupInformation.is_channel;
            updateGroupStatusText();
        }
    }

    function getMessageStatusText(message, listItemIndex, lastReadSentIndex) {
        var messageStatusSuffix = "";
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
        return Functions.getDateTimeElapsed(message.date) + messageStatusSuffix;
    }

    Component.onCompleted: {
        initializePage();
    }

    onStatusChanged: {
        if (status === PageStatus.Activating) {
            tdLibWrapper.openChat(chatInformation.id);
        }
        if (status === PageStatus.Active) {
            if (!chatPage.isInitialized) {
                chatModel.initialize(chatInformation);
                chatPage.isInitialized = true;
            }
        }
        if (status === PageStatus.Deactivating) {
            tdLibWrapper.closeChat(chatInformation.id);
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
    }

    Connections {
        target: chatModel
        onMessagesReceived: {
            console.log("[ChatPage] Messages received, view has " + chatView.count + " messages, setting view to index " + modelIndex + ", own messages were read before index " + lastReadSentIndex);
            chatView.lastReadSentIndex = lastReadSentIndex;
            chatView.positionViewAtIndex(modelIndex, ListView.Contain);
            chatPage.loading = false;
            if (chatView.height > chatView.contentHeight) {
                console.log("[ChatPage] Chat content quite small...");
                tdLibWrapper.viewMessage(chatInformation.id, chatModel.getMessage(chatView.count - 1).id);
            }
        }
        onNewMessageReceived: {
            // Notify user about new messages...
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
            chatView.currentIndex = modelIndex;
            chatView.lastReadSentIndex = lastReadSentIndex;
        }
    }

    Timer {
        id: chatContactTimeUpdater
        interval: 60000
        running: true
        repeat: true
        onTriggered: {
            updateChatPartnerStatusText();
        }
    }

    SilicaFlickable {
        id: chatContainer
        contentHeight: parent.height
        contentWidth: parent.width
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                id: muteChatMenuItem
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

        Connections {
            target: chatModel
            onNotificationSettingsUpdated: {
                chatInformation = chatModel.getChatInformation();
                muteChatMenuItem.text = chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat");
            }
        }

        Column {
            id: chatColumn
            width: parent.width
            height: parent.height
            spacing: Theme.paddingMedium

            Row {
                id: headerRow
                width: parent.width - (3 * Theme.horizontalPageMargin)
                height: chatOverviewColumn.height + ( 2 * Theme.paddingLarge )
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium

                ProfileThumbnail {
                    id: chatPictureThumbnail
                    photoData: (typeof chatInformation.photo !== "undefined") ? chatInformation.photo.small : ""
                    replacementStringHint: chatNameText.text
                    width: chatOverviewColumn.height
                    height: chatOverviewColumn.height
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    id: chatOverviewColumn
                    width: parent.width - chatPictureThumbnail.width - Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
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
                        color: Theme.secondaryColor
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
                height: parent.height - Theme.paddingMedium - headerRow.height - ( chatPage.isChannel ? 0 : ( newMessageColumn.height + Theme.paddingMedium ) )

                Timer {
                    id: chatViewLoadingTimer
                    interval: 100
                    repeat: false
                    running: false
                    onTriggered: {
                        chatPage.loading = false;
                    }
                }

                SilicaListView {
                    id: chatView

                    anchors.fill: parent

                    opacity: chatPage.loading ? 0 : 1
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    property int lastReadSentIndex: 0

                    function handleScrollPositionChanged() {
                        console.log("Current position: " + chatView.contentY);
                        tdLibWrapper.viewMessage(chatInformation.id, chatView.itemAt(chatView.contentX, ( chatView.contentY + chatView.height - Theme.horizontalPageMargin )).myMessage.id);
                    }

                    onContentYChanged: {
                        if (!chatPage.loading) {
                            if (chatView.indexAt(chatView.contentX, chatView.contentY) < 10) {
                                console.log("Trying to get older history items...");
                                chatModel.triggerLoadMoreHistory();
                            }
                        }
                    }

                    onMovementEnded: {
                        handleScrollPositionChanged();
                    }

                    onQuickScrollAnimatingChanged: {
                        if (!quickScrollAnimating) {
                            handleScrollPositionChanged();
                        }
                    }

                    model: chatModel
                    delegate: ListItem {

                        id: messageListItem
                        contentHeight: messageBackground.height + Theme.paddingMedium
                        contentWidth: parent.width

                        property variant myMessage: display
                        property variant userInformation: tdLibWrapper.getUserInformation(display.sender_user_id)

                        menu: ContextMenu {
                            MenuItem {
                                onClicked: {
                                    newMessageInReplyToRow.inReplyToMessage = display;
                                    newMessageTextField.focus = true;
                                }
                                text: qsTr("Reply to Message")
                            }
                        }

                        Connections {
                            target: chatModel
                            onUnreadCountUpdated: {
                                messageBackground.color = index > ( chatView.count - unreadCount - 1 ) ? Theme.secondaryHighlightColor : Theme.secondaryColor;
                                messageBackground.opacity = index > ( chatView.count - unreadCount - 1 ) ? 0.5 : 0.2;
                            }
                        }

                        Row {
                            id: messageTextRow
                            spacing: Theme.paddingSmall
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter

                            ProfileThumbnail {
                                id: messagePictureThumbnail
                                photoData: (typeof messageListItem.userInformation.profile_photo !== "undefined") ? messageListItem.userInformation.profile_photo.small : ""
                                replacementStringHint: userText.text
                                width: visible ? Theme.itemSizeSmall : 0
                                height: visible ? Theme.itemSizeSmall : 0
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: Theme.paddingSmall
                                visible: ( chatPage.isBasicGroup || chatPage.isSuperGroup ) && !chatPage.isChannel
                            }

                            Item {
                                id: messageTextItem

                                width: parent.width - messagePictureThumbnail.width - Theme.paddingSmall
                                height: messageBackground.height

                                Rectangle {
                                    id: messageBackground
                                    anchors {
                                        left: parent.left
                                        leftMargin: (chatPage.myUserId === display.sender_user_id) ? 2 * Theme.horizontalPageMargin : 0
                                        right: parent.right
                                        rightMargin: (chatPage.myUserId === display.sender_user_id) ? 0 : 2 * Theme.horizontalPageMargin
                                        verticalCenter: parent.verticalCenter
                                    }
                                    height: messageTextColumn.height + ( 2 * Theme.paddingMedium )

                                    color: index > ( chatView.count - chatInformation.unread_count - 1 ) ? Theme.secondaryHighlightColor : Theme.secondaryColor
                                    radius: parent.width / 50
                                    opacity: index > ( chatView.count - chatInformation.unread_count - 1 ) ? 0.5 : 0.2
                                }

                                Column {
                                    id: messageTextColumn

                                    spacing: Theme.paddingSmall

                                    width: messageBackground.width - Theme.horizontalPageMargin
                                    anchors.centerIn: messageBackground

                                    Component.onCompleted: {
                                        if (display.reply_to_message_id !== 0) {
                                            tdLibWrapper.getMessage(chatInformation.id, display.reply_to_message_id);
                                        }
                                    }

                                    Connections {
                                        target: tdLibWrapper
                                        onReceivedMessage: {
                                            if (messageId === display.reply_to_message_id.toString()) {
                                                messageInReplyToRow.inReplyToMessage = message;
                                                messageInReplyToRow.visible = true;
                                            }
                                        }
                                    }

                                    Text {
                                        id: userText

                                        width: parent.width
                                        text: display.sender_user_id !== chatPage.myUserId ? Emoji.emojify(Functions.getUserName(messageListItem.userInformation), font.pixelSize) : qsTr("You")
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        font.weight: Font.ExtraBold
                                        color: (chatPage.myUserId === display.sender_user_id) ? Theme.highlightColor : Theme.primaryColor
                                        maximumLineCount: 1
                                        elide: Text.ElideRight
                                        textFormat: Text.StyledText
                                        horizontalAlignment: (chatPage.myUserId === display.sender_user_id) ? Text.AlignRight : Text.AlignLeft
                                        visible: ( chatPage.isBasicGroup || chatPage.isSuperGroup ) && !chatPage.isChannel
                                    }

                                    InReplyToRow {
                                        id: messageInReplyToRow
                                        myUserId: chatPage.myUserId
                                        visible: false
                                    }

                                    Text {
                                        id: messageText

                                        width: parent.width
                                        text: Emoji.emojify(Functions.getMessageText(display, false), font.pixelSize)
                                        font.pixelSize: Theme.fontSizeSmall
                                        color: (chatPage.myUserId === display.sender_user_id) ? Theme.highlightColor : Theme.primaryColor
                                        wrapMode: Text.Wrap
                                        textFormat: Text.StyledText
                                        onLinkActivated: {
                                            Functions.handleLink(link);
                                        }
                                        horizontalAlignment: (chatPage.myUserId === display.sender_user_id) ? Text.AlignRight : Text.AlignLeft
                                        linkColor: Theme.highlightColor
                                        visible: (text !== "")
                                    }

                                    WebPagePreview {
                                        id: webPagePreview
                                        webPageData: ( typeof display.content.web_page !== "undefined" ) ? display.content.web_page : ""
                                        width: parent.width
                                        visible: typeof display.content.web_page !== "undefined"
                                    }

                                    ImagePreview {
                                        id: messageImagePreview
                                        photoData: ( display.content['@type'] === "messagePhoto" ) ?  display.content.photo : ""
                                        width: parent.width
                                        height: parent.width * 2 / 3
                                        visible: display.content['@type'] === "messagePhoto"
                                    }

                                    StickerPreview {
                                        id: messageStickerPreview
                                        stickerData: ( display.content['@type'] === "messageSticker" ) ?  display.content.sticker : ""
                                        visible: display.content['@type'] === "messageSticker"
                                        anchors.horizontalCenter: parent.horizontalCenter
                                    }

                                    VideoPreview {
                                        id: messageVideoPreview
                                        videoData: ( display.content['@type'] === "messageVideo" ) ?  display.content.video : ( ( display.content['@type'] === "messageAnimation" ) ? display.content.animation : "")
                                        width: parent.width
                                        height: ( display.content['@type'] === "messageVideo" ) ? Functions.getVideoHeight(width, display.content.video) : Functions.getVideoHeight(width, display.content.animation)
                                        visible: ( display.content['@type'] === "messageVideo" || display.content['@type'] === "messageAnimation" )
                                        onScreen: chatPage.status === PageStatus.Active
                                    }

                                    AudioPreview {
                                        id: messageAudioPreview
                                        audioData: ( display.content['@type'] === "messageVoiceNote" ) ?  display.content.voice_note : ( ( display.content['@type'] === "messageAudio" ) ? display.content.audio : "")
                                        width: parent.width
                                        height: parent.width / 2
                                        visible: ( display.content['@type'] === "messageVoiceNote" || display.content['@type'] === "messageAudio" )
                                        onScreen: chatPage.status === PageStatus.Active
                                    }

                                    DocumentPreview {
                                        id: messageDocumentPreview
                                        documentData: ( display.content['@type'] === "messageDocument" ) ?  display.content.document : ""
                                        visible: display.content['@type'] === "messageDocument"
                                    }

                                    Timer {
                                        id: messageDateUpdater
                                        interval: 60000
                                        running: true
                                        repeat: true
                                        onTriggered: {
                                            messageDateText.text = getMessageStatusText(display, index, chatView.lastReadSentIndex);
                                        }
                                    }

                                    Connections {
                                        target: chatModel
                                        onLastReadSentMessageUpdated: {
                                            console.log("[ChatModel] Messages in this chat were read, new last read: " + lastReadSentIndex + ", updating description for index " + index + ", status: " + (index <= lastReadSentIndex));
                                            messageDateText.text = getMessageStatusText(display, index, lastReadSentIndex);
                                        }
                                    }

                                    Text {
                                        width: parent.width

                                        id: messageDateText
                                        font.pixelSize: Theme.fontSizeTiny
                                        color: (chatPage.myUserId === display.sender_user_id) ? Theme.secondaryHighlightColor : Theme.secondaryColor
                                        horizontalAlignment: (chatPage.myUserId === display.sender_user_id) ? Text.AlignRight : Text.AlignLeft
                                        text: getMessageStatusText(display, index, chatView.lastReadSentIndex)
                                    }

                                }

                            }

                        }

                    }

                    VerticalScrollDecorator {}
                }

                Column {
                    width: parent.width
                    height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium
                    spacing: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter

                    opacity: chatPage.loading ? 1 : 0
                    Behavior on opacity { NumberAnimation {} }
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

                Rectangle {
                    id: chatUnreadMessagesCountBackground
                    color: Theme.highlightBackgroundColor
                    width: Theme.fontSizeHuge
                    height: Theme.fontSizeHuge
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingMedium
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingMedium
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

            }

            Column {
                id: newMessageColumn
                spacing: Theme.paddingMedium
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !chatPage.isChannel

                property string replyToMessageId: "0";

                InReplyToRow {
                    onInReplyToMessageChanged: {
                        console.log("This is a reply!");
                        if (inReplyToMessage) {
                            newMessageColumn.replyToMessageId = newMessageInReplyToRow.inReplyToMessage.id.toString()
                            newMessageInReplyToRow.visible = true;
                        } else {
                            newMessageInReplyToRow.visible = false;
                            newMessageColumn.replyToMessageId = "0";
                        }
                    }

                    id: newMessageInReplyToRow
                    myUserId: chatPage.myUserId
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                }

                Row {
                    id: newMessageRow
                    width: parent.width
                    height: sendMessageColumn.height + ( 2 * Theme.paddingLarge )
                    anchors.horizontalCenter: parent.horizontalCenter

                    Column {
                        id: sendMessageColumn
                        width: parent.width - newMessageSendButton.width
                        anchors.verticalCenter: parent.verticalCenter

                        TextArea {
                            id: newMessageTextField
                            width: parent.width
                            font.pixelSize: Theme.fontSizeSmall
                            placeholderText: qsTr("Your message")
                            labelVisible: false
                            textLeftMargin: 0
                            onFocusChanged: {
                                if (!focus) {
                                    newMessageInReplyToRow.inReplyToMessage = null;
                                }
                            }
                        }
                    }

                    Column {
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingLarge

                        IconButton {
                            id: newMessageSendButton
                            icon.source: "image://theme/icon-m-chat"
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: {
                                tdLibWrapper.sendTextMessage(chatInformation.id, newMessageTextField.text, newMessageColumn.replyToMessageId);
                                newMessageTextField.text = "";
                                newMessageTextField.focus = false;
                            }
                        }
                    }
                }

            }

        }
    }

}
