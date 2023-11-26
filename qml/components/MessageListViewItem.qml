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
import "./messageContent"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

ListItem {
    id: messageListItem
    contentHeight: messageBackground.height + Theme.paddingMedium + ( reactionsColumn.visible ? reactionsColumn.height : 0 )
    Behavior on contentHeight { NumberAnimation { duration: 200 } }
    property var chatId
    property var messageId
    property int messageIndex
    property int messageViewCount
    property var myMessage
    property var reactions
    property bool canReplyToMessage
    readonly property bool isAnonymous: myMessage.sender_id["@type"] === "messageSenderChat"
    readonly property var userInformation: tdLibWrapper.getUserInformation(myMessage.sender_id.user_id)
    property QtObject precalculatedValues: ListView.view.precalculatedValues
    readonly property color textColor: isOwnMessage ? Theme.highlightColor : Theme.primaryColor
    readonly property int textAlign: isOwnMessage ? Text.AlignRight : Text.AlignLeft
    readonly property Page page: precalculatedValues.page
    readonly property bool isSelected: messageListItem.precalculatedValues.pageIsSelecting && page.selectedMessages.some(function(existingMessage) {
        return existingMessage.id === messageId
    });
    readonly property bool isOwnMessage: page.myUserId === myMessage.sender_id.user_id
    readonly property bool canDeleteMessage: myMessage.can_be_deleted_for_all_users || (myMessage.can_be_deleted_only_for_self && myMessage.chat_id === page.myUserId)
    property bool hasContentComponent
    property bool additionalOptionsOpened

    readonly property var additionalItemsModel: (extraContentLoader.item && ("extraContextMenuItems" in extraContentLoader.item)) ?
        extraContentLoader.item.extraContextMenuItems : 0
    readonly property int numberOfExtraOptionsOtherThanDeleteMessage:
        (showCopyMessageToClipboardMenuItem ? 0 : 1) +
        (showForwardMessageMenuItem ? 0 : 1) +
        (page.canPinMessages() ? 1 : 0) +
        (additionalItemsModel ? additionalItemsModel.length : 0)
    readonly property bool deleteMessageIsOnlyExtraOption: canDeleteMessage && !numberOfExtraOptionsOtherThanDeleteMessage

    readonly property int maxContextMenuItemCount: page.isPortrait ? 5 : 4
    readonly property int baseContextMenuItemCount: (canReplyToMessage ? 1 : 0) +
        (myMessage.can_be_edited ? 1 : 0) + 2 /* "Select Message" and "More Options..." */
    readonly property bool showCopyMessageToClipboardMenuItem: (baseContextMenuItemCount + 1) <= maxContextMenuItemCount
    readonly property bool showForwardMessageMenuItem: (baseContextMenuItemCount + 2) <= maxContextMenuItemCount
    // And don't count "More Options..." for "Delete Message" if "Delete Message" is the only extra option
    readonly property bool haveSpaceForDeleteMessageMenuItem: (baseContextMenuItemCount + 3 - (deleteMessageIsOnlyExtraOption ? 1 : 0)) <= maxContextMenuItemCount
    property var chatReactions
    property var messageReactions

    highlighted: (down || isSelected || additionalOptionsOpened) && !menuOpen
    openMenuOnPressAndHold: !messageListItem.precalculatedValues.pageIsSelecting

    signal replyToMessage()
    signal editMessage()
    signal forwardMessage()

    function deleteMessage() {
        var chatId = page.chatInformation.id
        var messageId = myMessage.id
        Remorse.itemAction(messageListItem, qsTr("Message deleted"), function() {
            tdLibWrapper.deleteMessages(chatId, [ messageId ]);
        })
    }

    function copyMessageToClipboard() {
        Clipboard.text = Functions.getMessageText(myMessage, true, userInformation.id, true)
    }

    function openContextMenu() {
        messageOptionsDrawer.open = false
        if (menu) {
            openMenu()
        } else {
            contextMenuLoader.active = true
        }
    }

    function getInteractionText(viewCount, reactions, size, highlightColor) {
        var interactionText = "";
        if (viewCount > 0) {
            interactionText = Emoji.emojify("👁️ ", size) + Functions.getShortenedCount(viewCount);
        }
        for (var i = 0; i < reactions.length; i++) {
            var reaction = reactions[i]
            var reactionText = reaction.reaction ? reaction.reaction : (reaction.type && reaction.type.emoji) ? reaction.type.emoji : ""
            if (reactionText) {
                interactionText += ( "&nbsp;" + Emoji.emojify(reactionText, size) );
                if (!chatPage.isPrivateChat) {
                    var count = Functions.getShortenedCount(reaction.total_count)
                    interactionText += " "
                    interactionText += (reaction.is_chosen ? ( "<font color='" + highlightColor + "'><b>" + count + "</b></font>" ) : count)
                }
            }
        }
        return interactionText;
    }

    onClicked: {
        if (messageListItem.precalculatedValues.pageIsSelecting) {
            page.toggleMessageSelection(myMessage);
        } else {
            if (messageOptionsDrawer.sourceItem !== messageListItem) {
                messageOptionsDrawer.open = false
            }
            // Allow extra context to react to click
            var extraContent = extraContentLoader.item
            if (extraContent && extraContentLoader.contains(mapToItem(extraContentLoader, mouse.x, mouse.y))) {
                extraContent.clicked()
            } else if (webPagePreviewLoader.item) {
                webPagePreviewLoader.item.clicked()
            }

            if (messageListItem.messageReactions) {
                messageListItem.messageReactions = null;
            }
        }
    }

    onDoubleClicked: {
        if (messageListItem.chatReactions) {
            Debug.log("Using chat reactions")
            messageListItem.messageReactions = chatReactions
            showItemCompletelyTimer.requestedIndex = index;
            showItemCompletelyTimer.start();
        } else {
            Debug.log("Obtaining message reactions")
            tdLibWrapper.getMessageAvailableReactions(messageListItem.chatId, messageListItem.messageId);
        }
    }

    onPressAndHold: {
        if (openMenuOnPressAndHold) {
            openContextMenu()
        } else {
            page.selectedMessages = []
            page.state = ""
        }
    }

    onMenuOpenChanged: {
        // When opening/closing the context menu, we no longer scroll automatically
        chatView.manuallyScrolledToBottom = false;
    }

    Connections {
        target: additionalOptionsOpened ? messageOptionsDrawer : null
        onOpenChanged: {
            if (!messageOptionsDrawer.open) {
                additionalOptionsOpened = false
            }
        }
    }

    Loader {
        id: contextMenuLoader
        active: false
        asynchronous: true
        onStatusChanged: {
            if(status === Loader.Ready) {
                messageListItem.menu = item;
                messageListItem.openMenu();
            }
        }
        sourceComponent: Component {
            ContextMenu {
                MenuItem {
                    visible: canReplyToMessage
                    onClicked: replyToMessage()
                    text: qsTr("Reply to Message")
                }
                MenuItem {
                    visible: typeof myMessage.can_be_edited !== "undefined" && myMessage.can_be_edited
                    onClicked: editMessage()
                    text: qsTr("Edit Message")
                }
                MenuItem {
                    onClicked: page.toggleMessageSelection(myMessage)
                    text: qsTr("Select Message")
                }
                MenuItem {
                    visible: showCopyMessageToClipboardMenuItem
                    onClicked: copyMessageToClipboard()
                    text: qsTr("Copy Message to Clipboard")
                }
                MenuItem {
                    visible: showForwardMessageMenuItem
                    onClicked: forwardMessage()
                    text: qsTr("Forward Message")
                }
                MenuItem {
                    visible: canDeleteMessage && haveSpaceForDeleteMessageMenuItem
                    onClicked: deleteMessage()
                    text: qsTr("Delete Message")
                }
                MenuItem {
                    visible: (numberOfExtraOptionsOtherThanDeleteMessage > 0) ||
                        (deleteMessageIsOnlyExtraOption && !haveSpaceForDeleteMessageMenuItem)
                    onClicked: {
                        messageOptionsDrawer.myMessage = myMessage;
                        messageOptionsDrawer.userInformation = userInformation;
                        messageOptionsDrawer.sourceItem = messageListItem
                        messageOptionsDrawer.additionalItemsModel = additionalItemsModel
                        messageOptionsDrawer.showCopyMessageToClipboardMenuItem = !showCopyMessageToClipboardMenuItem
                        messageOptionsDrawer.showForwardMessageMenuItem = !showForwardMessageMenuItem
                        messageOptionsDrawer.showDeleteMessageMenuItem = canDeleteMessage && !haveSpaceForDeleteMessageMenuItem
                        messageListItem.additionalOptionsOpened = true;
                        messageOptionsDrawer.open = true;
                    }
                    text: qsTr("More Options...")
                }
            }
        }
    }

    Connections {
        target: chatModel
        onMessagesReceived: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex() && myMessage['@type'] !== "sponsoredMessage";
        }
        onMessagesIncrementalUpdate: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex() && myMessage['@type'] !== "sponsoredMessage";
        }
        onNewMessageReceived: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex() && myMessage['@type'] !== "sponsoredMessage";
        }
        onUnreadCountUpdated: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex() && myMessage['@type'] !== "sponsoredMessage";
        }
        onLastReadSentMessageUpdated: {
            Debug.log("[ChatModel] Messages in this chat were read, new last read: ", lastReadSentIndex, ", updating description for index ", index, ", status: ", (index <= lastReadSentIndex));
            messageDateText.text = getMessageStatusText(myMessage, index, lastReadSentIndex, messageDateText.useElapsed);
        }
    }

    Connections {
        target: tdLibWrapper
        onReceivedMessage: {
            if (messageId === myMessage.reply_to_message_id) {
                messageInReplyToLoader.inReplyToMessage = message;
            }
        }
        onMessageNotFound: {
            if (messageId === myMessage.reply_to_message_id) {
                messageInReplyToLoader.inReplyToMessageDeleted = true;
            }
        }
        onAvailableReactionsReceived: {
            if (messageListItem.messageId === messageId &&
                    pageStack.currentPage === chatPage) {
                Debug.log("Available reactions for this message: " + reactions);
                messageListItem.messageReactions = reactions;
                showItemCompletelyTimer.requestedIndex = index;
                showItemCompletelyTimer.start();
            } else {
                messageListItem.messageReactions = null;
            }
        }
    }

    Timer {
        id: showItemCompletelyTimer

        property int requestedIndex: (chatView.count - 1)

        repeat: false
        running: false
        interval: 200
        triggeredOnStart: false
        onTriggered: {
            Debug.log("Show item completely timer triggered, requested index: " + requestedIndex + ", current index: " + index)
            if (requestedIndex === index) {
                chatView.highlightMoveDuration = -1;
                chatView.highlightResizeDuration = -1;
                chatView.scrollToIndex(requestedIndex);
                chatView.highlightMoveDuration = 0;
                chatView.highlightResizeDuration = 0;
            }
        }
    }

    Component.onCompleted: {
        delegateComponentLoadingTimer.start();
        if (myMessage.reply_to_message_id) {
            tdLibWrapper.getMessage(myMessage.reply_in_chat_id ? myMessage.reply_in_chat_id : page.chatInformation.id,
                myMessage.reply_to_message_id)
        }
    }

    onMyMessageChanged: {
        Debug.log("[ChatModel] This message was updated, index", messageIndex, ", updating content...");
        messageDateText.text = getMessageStatusText(myMessage, messageIndex, chatView.lastReadSentIndex, messageDateText.useElapsed);
        messageText.text = Emoji.emojify(Functions.getMessageText(myMessage, false, page.myUserId, false), Theme.fontSizeSmall);
        if (webPagePreviewLoader.item) {
            webPagePreviewLoader.item.webPageData = myMessage.content.web_page;
        }
    }

    Timer {
        id: delegateComponentLoadingTimer
        interval: 500
        repeat: false
        running: false
        onTriggered: {
            if (messageListItem.hasContentComponent) {
                var type = myMessage.content["@type"];
                extraContentLoader.setSource(
                            "../components/messageContent/" + type.charAt(0).toUpperCase() + type.substring(1) + ".qml",
                            {
                                messageListItem: messageListItem
                            })
            } else {
                if (typeof myMessage.content.web_page !== "undefined") { // only in messageText
                    webPagePreviewLoader.active = true;
                }
            }
        }
    }

    Row {
        id: messageTextRow
        spacing: Theme.paddingSmall
        width: precalculatedValues.entryWidth
        anchors.horizontalCenter: parent.horizontalCenter
        y: Theme.paddingSmall

        Loader {
            id: profileThumbnailLoader
            active: precalculatedValues.showUserInfo
            asynchronous: true
            width: precalculatedValues.profileThumbnailDimensions
            height: width
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.paddingSmall
            sourceComponent: Component {
                ProfileThumbnail {
                    id: messagePictureThumbnail
                    photoData: messageListItem.isAnonymous ? ((typeof page.chatInformation.photo !== "undefined") ? page.chatInformation.photo.small : {}) : ((typeof messageListItem.userInformation.profile_photo !== "undefined") ? messageListItem.userInformation.profile_photo.small : ({}))
                    replacementStringHint: userText.text
                    width: Theme.itemSizeSmall
                    height: Theme.itemSizeSmall
                    visible: precalculatedValues.showUserInfo
                    MouseArea {
                        anchors.fill: parent
                        enabled: !(messageListItem.precalculatedValues.pageIsSelecting || messageListItem.isAnonymous)
                        onClicked: {
                            tdLibWrapper.createPrivateChat(messageListItem.userInformation.id, "openDirectly");
                        }
                    }
                }
            }
        }

        Item {
            id: messageTextItem

            width: precalculatedValues.textItemWidth
            height: messageBackground.height

            Rectangle {
                id: messageBackground

                anchors {
                    left: parent.left
                    leftMargin: messageListItem.isOwnMessage ? precalculatedValues.pageMarginDouble : 0
                    verticalCenter: parent.verticalCenter
                }
                height: messageTextColumn.height + precalculatedValues.paddingMediumDouble
                width: precalculatedValues.backgroundWidth
                property bool isUnread: index > chatModel.getLastReadMessageIndex() && myMessage['@type'] !== "sponsoredMessage"
                color: Theme.colorScheme === Theme.LightOnDark ? (isUnread ? Theme.secondaryHighlightColor : Theme.secondaryColor) : (isUnread ? Theme.backgroundGlowColor : Theme.overlayBackgroundColor)
                radius: parent.width / 50
                opacity: isUnread ? 0.5 : 0.2
                visible: appSettings.showStickersAsImages || (myMessage.content['@type'] !== "messageSticker" && myMessage.content['@type'] !== "messageAnimatedEmoji")
                Behavior on color { ColorAnimation { duration: 200 } }
                Behavior on opacity { FadeAnimation {} }
            }

            Column {
                id: messageTextColumn

                spacing: Theme.paddingSmall

                width: precalculatedValues.textColumnWidth
                anchors.centerIn: messageBackground


                Label {
                    id: userText

                    width: parent.width
                    text: messageListItem.isOwnMessage ? qsTr("You") : Emoji.emojify( myMessage['@type'] === "sponsoredMessage" ? tdLibWrapper.getChat(myMessage.sponsor_chat_id).title : ( messageListItem.isAnonymous ? page.chatInformation.title : Functions.getUserName(messageListItem.userInformation) ), font.pixelSize)
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.weight: Font.ExtraBold
                    color: messageListItem.textColor
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Fade
                    textFormat: Text.StyledText
                    horizontalAlignment: messageListItem.textAlign
                    visible: precalculatedValues.showUserInfo || myMessage['@type'] === "sponsoredMessage"
                    MouseArea {
                        anchors.fill: parent
                        enabled: !(messageListItem.precalculatedValues.pageIsSelecting || messageListItem.isAnonymous)
                        onClicked: {
                            tdLibWrapper.createPrivateChat(messageListItem.userInformation.id, "openDirectly");
                        }
                    }
                }

                MessageViaLabel {
                    message: myMessage
                }

                Loader {
                    id: messageInReplyToLoader
                    active: typeof myMessage.reply_to_message_id !== "undefined" && myMessage.reply_to_message_id !== 0
                    width: parent.width
                    // text height ~= 1,28*font.pixelSize
                    height: active ? precalculatedValues.messageInReplyToHeight : 0
                    property var inReplyToMessage;
                    property bool inReplyToMessageDeleted: false;
                    sourceComponent: Component {
                        Item {
                            width: messageInReplyToRow.width
                            height: messageInReplyToRow.height
                            InReplyToRow {
                                id: messageInReplyToRow
                                myUserId: page.myUserId
                                layer.enabled: messageInReplyToMouseArea.pressed && !messageListItem.highlighted && !messageListItem.menuOpen
                                layer.effect: PressEffect { source: messageInReplyToRow }
                                inReplyToMessage: messageInReplyToLoader.inReplyToMessage
                                inReplyToMessageDeleted: messageInReplyToLoader.inReplyToMessageDeleted
                            }
                            MouseArea {
                                id: messageInReplyToMouseArea
                                anchors.fill: parent
                                onClicked: {
                                    if (precalculatedValues.pageIsSelecting) {
                                        page.toggleMessageSelection(myMessage)
                                    } else {
                                        messageOptionsDrawer.open = false
                                        messageOverlayLoader.overlayMessage = messageInReplyToRow.inReplyToMessage
                                        messageOverlayLoader.active = true
                                    }
                                }
                                onPressAndHold: {
                                    if (openMenuOnPressAndHold) {
                                        openContextMenu()
                                    }
                                }
                            }
                        }
                    }
                }

                Loader {
                    id: forwardedInformationLoader
                    active: typeof myMessage.forward_info !== "undefined"
                    asynchronous: true
                    width: parent.width
                    height: active ? ( item ? item.height : Theme.itemSizeExtraSmall ) : 0
                    sourceComponent: Component {
                        Row {
                            id: forwardedMessageInformationRow
                            spacing: Theme.paddingSmall
                            width: parent.width

                            Component.onCompleted: {
                                var originType = myMessage.forward_info.origin["@type"]
                                if (originType === "messageOriginChannel" || originType === "messageForwardOriginChannel") {
                                    var otherChatInformation = tdLibWrapper.getChat(myMessage.forward_info.origin.chat_id);
                                    forwardedThumbnail.photoData = (typeof otherChatInformation.photo !== "undefined") ? otherChatInformation.photo.small : {};
                                    forwardedChannelText.text = Emoji.emojify(otherChatInformation.title, Theme.fontSizeExtraSmall);
                                } else if (originType === "messageOriginUser" || originType === "messageForwardOriginUser") {
                                    var otherUserInformation = tdLibWrapper.getUserInformation(myMessage.forward_info.origin.sender_user_id);
                                    forwardedThumbnail.photoData = (typeof otherUserInformation.profile_photo !== "undefined") ? otherUserInformation.profile_photo.small : {};
                                    forwardedChannelText.text = Emoji.emojify(Functions.getUserName(otherUserInformation), Theme.fontSizeExtraSmall);
                                } else {
                                    forwardedChannelText.text = Emoji.emojify(myMessage.forward_info.origin.sender_name, Theme.fontSizeExtraSmall);
                                    forwardedThumbnail.photoData = {};
                                }
                            }

                            ProfileThumbnail {
                                id: forwardedThumbnail
                                replacementStringHint: forwardedChannelText.text
                                width: Theme.itemSizeExtraSmall
                                height: Theme.itemSizeExtraSmall
                            }

                            Column {
                                spacing: Theme.paddingSmall
                                width: parent.width - forwardedThumbnail.width - Theme.paddingSmall
                                Label {
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                    width: parent.width
                                    font.italic: true
                                    truncationMode: TruncationMode.Fade
                                    textFormat: Text.StyledText
                                    text: qsTr("Forwarded Message")
                                }
                                Label {
                                    id: forwardedChannelText
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                    color: Theme.primaryColor
                                    width: parent.width
                                    font.bold: true
                                    truncationMode: TruncationMode.Fade
                                    textFormat: Text.StyledText
                                    text: Emoji.emojify(forwardedMessageInformationRow.otherChatInformation.title, font.pixelSize)
                                }
                            }
                        }
                    }
                }

                Text {
                    id: messageText
                    width: parent.width
                    text: Emoji.emojify(Functions.getMessageText(myMessage, false, page.myUserId, false), Theme.fontSizeMedium)
                    font.pixelSize: Theme.fontSizeSmall
                    color: messageListItem.textColor
                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText
                    onLinkActivated: {
                        var chatCommand = Functions.handleLink(link);
                        if(chatCommand) {
                            tdLibWrapper.sendTextMessage(chatInformation.id, chatCommand);
                        }
                    }
                    horizontalAlignment: messageListItem.textAlign
                    linkColor: Theme.highlightColor
                    visible: (text !== "")
                }

                Loader {
                    id: sponsoredMessageButtonLoader
                    active: myMessage['@type'] === "sponsoredMessage"
                    asynchronous: true
                    width: parent.width
                    height: (status === Loader.Ready) ? item.implicitHeight : myMessage['@type'] === "sponsoredMessage" ? Theme.itemSizeMedium : 0

                    sourceComponent: Component {
                        SponsoredMessage {
                            sponsoredMessageData: myMessage
                            width: parent.width
                        }
                    }
                }

                Loader {
                    id: webPagePreviewLoader
                    active: false
                    asynchronous: true
                    width: parent.width
                    height: (status === Loader.Ready) ? item.implicitHeight : myMessage.content.web_page ? precalculatedValues.webPagePreviewHeight : 0

                    sourceComponent: Component {
                        WebPagePreview {
                            webPageData: myMessage.content.web_page
                            width: parent.width
                            highlighted: messageListItem.highlighted
                        }
                    }
                }

                Loader {
                    id: extraContentLoader
                    width: parent.width
                    asynchronous: true
                    height: item ? item.height : (messageListItem.hasContentComponent ? chatView.getContentComponentHeight(model.content_type, myMessage.content, width) : 0)
                }

                Binding {
                    target: extraContentLoader.item
                    when: extraContentLoader.item && ("highlighted" in extraContentLoader.item) && (typeof extraContentLoader.item.highlighted === "boolean")
                    property: "highlighted"
                    value: messageListItem.highlighted
                }

                Loader {
                    id: replyMarkupLoader
                    width: parent.width
                    height: active ? (myMessage.reply_markup.rows.length * (Theme.itemSizeSmall + Theme.paddingSmall) - Theme.paddingSmall) : 0
                    asynchronous: true
                    active: !!myMessage.reply_markup && myMessage.reply_markup.rows
                    source: Qt.resolvedUrl("ReplyMarkupButtons.qml")
                }

                Timer {
                    id: messageDateUpdater
                    interval: 60000
                    running: true
                    repeat: true
                    onTriggered: {
                        messageDateText.text = getMessageStatusText(myMessage, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                    }
                }

                Text {
                    width: parent.width

                    property bool useElapsed: true

                    id: messageDateText
                    font.pixelSize: Theme.fontSizeTiny
                    color: messageListItem.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    horizontalAlignment: messageListItem.textAlign
                    text: getMessageStatusText(myMessage, index, chatView.lastReadSentIndex, messageDateText.useElapsed)
                    MouseArea {
                        anchors.fill: parent
                        enabled: !messageListItem.precalculatedValues.pageIsSelecting
                        onClicked: {
                            messageDateText.useElapsed = !messageDateText.useElapsed;
                            messageDateText.text = getMessageStatusText(myMessage, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                        }
                    }
                }

                Loader {
                    id: interactionLoader
                    width: parent.width
                    asynchronous: true
                    active: ( chatPage.isChannel && messageViewCount > 0 ) || reactions.length > 0
                    height: ( ( chatPage.isChannel && messageViewCount > 0 ) || reactions.length > 0 ) ? ( Theme.fontSizeExtraSmall + Theme.paddingSmall ) : 0
                    sourceComponent: Component {
                        Label {
                            text: getInteractionText(messageViewCount, reactions, font.pixelSize, Theme.highlightColor)
                            width: parent.width
                            font.pixelSize: Theme.fontSizeTiny
                            color: messageListItem.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor
                            horizontalAlignment: messageListItem.textAlign
                            textFormat: Text.StyledText
                            maximumLineCount: 1
                            elide: Text.ElideRight
                        }
                    }
                }

            }

        }

    }

    Column {
        id: reactionsColumn
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        anchors.top: messageTextRow.bottom
        anchors.topMargin: Theme.paddingSmall
        anchors.horizontalCenter: parent.horizontalCenter
        visible: messageListItem.messageReactions ? ( messageListItem.messageReactions.length > 0 ? true : false ) : false
        opacity: messageListItem.messageReactions ? ( messageListItem.messageReactions.length > 0 ? 1 : 0 ) : 0
        Behavior on opacity { NumberAnimation {} }
        spacing: Theme.paddingMedium

        Flickable {
            width: parent.width
            height: reactionsResultRow.height + Theme.paddingSmall
            anchors.horizontalCenter: parent.horizontalCenter
            contentWidth: reactionsResultRow.width
            clip: true
            Row {
                id: reactionsResultRow
                spacing: Theme.paddingMedium
                Repeater {
                    model: messageListItem.messageReactions

                    Item {
                        height: singleReactionRow.height
                        width: singleReactionRow.width

                        Row {
                            id: singleReactionRow
                            spacing: Theme.paddingSmall

                            Image {
                                id: emojiPicture
                                source: Emoji.getEmojiPath(modelData)
                                width: status === Image.Ready ? Theme.fontSizeLarge : 0
                                height: Theme.fontSizeLarge
                            }

                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                for (var i = 0; i < reactions.length; i++) {
                                    var reaction = reactions[i]
                                    var reactionText = reaction.reaction ? reaction.reaction : (reaction.type && reaction.type.emoji) ? reaction.type.emoji : ""
                                    if (reactionText === modelData) {
                                        if (reaction.is_chosen) {
                                            // Reaction is already selected
                                            tdLibWrapper.removeMessageReaction(chatId, messageId, reactionText)
                                            messageReactions = null
                                            return
                                        }
                                        break
                                    }
                                }
                                // Reaction is not yet selected
                                tdLibWrapper.addMessageReaction(chatId, messageId, modelData)
                                messageReactions = null
                            }
                        }
                    }
                }
            }
        }
    }


}
