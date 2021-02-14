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
    contentHeight: messageBackground.height + Theme.paddingMedium
    property var chatId
    property var messageId
    property int messageIndex
    property int messageViewCount
    property var myMessage
    property bool canReplyToMessage
    readonly property bool isAnonymous: myMessage.sender["@type"] === "messageSenderChat"
    readonly property var userInformation: tdLibWrapper.getUserInformation(myMessage.sender.user_id)
    property QtObject precalculatedValues: ListView.view.precalculatedValues
    readonly property color textColor: isOwnMessage ? Theme.highlightColor : Theme.primaryColor
    readonly property int textAlign: isOwnMessage ? Text.AlignRight : Text.AlignLeft
    readonly property Page page: precalculatedValues.page
    readonly property bool isSelected: messageListItem.precalculatedValues.pageIsSelecting && page.selectedMessages.some(function(existingMessage) {
        return existingMessage.id === messageId
    });
    readonly property bool isOwnMessage: page.myUserId === myMessage.sender.user_id
    property bool hasContentComponent

    highlighted: (down || isSelected) && !menuOpen
    openMenuOnPressAndHold: !messageListItem.precalculatedValues.pageIsSelecting

    signal replyToMessage()
    signal editMessage()

    onClicked: {
        if(messageListItem.precalculatedValues.pageIsSelecting) {
            page.toggleMessageSelection(myMessage);
        } else {
            // Allow extra context to react to click
            var extraContent = extraContentLoader.item
            if (extraContent && extraContentLoader.contains(mapToItem(extraContentLoader, mouse.x, mouse.y))) {
                extraContent.clicked()
            } else if (webPagePreviewLoader.item) {
                webPagePreviewLoader.item.clicked()
            }
        }
    }

    onPressAndHold: {
        if(messageListItem.precalculatedValues.pageIsSelecting) {
            page.selectedMessages = [];
            page.state = ""
        } else {
            contextMenuLoader.active = true;
        }
    }

    onMenuOpenChanged: {
        // When opening/closing the context menu, we no longer scroll automatically
        chatView.manuallyScrolledToBottom = false;
    }

    Connections {
        target: messageOptionsDrawer
        onCloseRequested: {
            messageListItem.highlighted = false;
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
                Repeater {
                    model: (extraContentLoader.item && ("extraContextMenuItems" in extraContentLoader.item)) ?
                        extraContentLoader.item.extraContextMenuItems : 0
                    delegate: MenuItem {
                        visible: modelData.visible
                        text: modelData.name
                        onClicked: modelData.action()
                    }
                }

                MenuItem {
                    visible: messageListItem.canReplyToMessage
                    onClicked: messageListItem.replyToMessage()
                    text: qsTr("Reply to Message")
                }
                MenuItem {
                    visible: myMessage.can_be_edited
                    onClicked: messageListItem.editMessage()
                    text: qsTr("Edit Message")
                }
                MenuItem {
                    onClicked: {
                        page.toggleMessageSelection(myMessage);
                    }
                    text: qsTr("Select Message")
                }
                MenuItem {
                    onClicked: {
                        messageOptionsDrawer.myMessage = myMessage;
                        messageOptionsDrawer.userInformation = userInformation;
                        messageListItem.highlighted = true;
                        messageOptionsDrawer.open = true;
                    }
                    text: qsTr("More Options...")
                }
                MenuItem {
                    onClicked: {
                        if (myMessage.is_pinned) {
                            Remorse.popupAction(page, qsTr("Message unpinned"), function() { tdLibWrapper.unpinMessage(page.chatInformation.id, messageId);
                                                                                             pinnedMessageItem.requestCloseMessage(); } );
                        } else {
                            tdLibWrapper.pinMessage(page.chatInformation.id, messageId);
                        }
                    }
                    text: myMessage.is_pinned ? qsTr("Unpin Message") : qsTr("Pin Message")
                    visible: canPinMessages()
                }
                MenuItem {
                    onClicked: {
                        var chatId = page.chatInformation.id;
                        var messageId = messageListItem.messageId;
                        Remorse.itemAction(messageListItem, qsTr("Message deleted"), function() { tdLibWrapper.deleteMessages(chatId, [ messageId]);  })
                    }
                    text: qsTr("Delete Message")
                    visible: myMessage.can_be_deleted_for_all_users || (myMessage.can_be_deleted_only_for_self && myMessage.chat_id === page.myUserId)
                }
            }
        }
    }

    Connections {
        target: chatModel
        onMessagesReceived: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex();
        }
        onMessagesIncrementalUpdate: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex();
        }
        onNewMessageReceived: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex();
        }
        onUnreadCountUpdated: {
            messageBackground.isUnread = index > chatModel.getLastReadMessageIndex();
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
    }

    Component.onCompleted: {
        delegateComponentLoadingTimer.start();

        if (myMessage.reply_to_message_id !== 0) {
            tdLibWrapper.getMessage(page.chatInformation.id, myMessage.reply_to_message_id);
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
        anchors.centerIn: parent

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
                property bool isUnread: index > chatModel.getLastReadMessageIndex()
                color: Theme.colorScheme === Theme.LightOnDark ? (isUnread ? Theme.secondaryHighlightColor : Theme.secondaryColor) : (isUnread ? Theme.backgroundGlowColor : Theme.overlayBackgroundColor)
                radius: parent.width / 50
                opacity: isUnread ? 0.5 : 0.2
                visible: appSettings.showStickersAsImages || myMessage.content['@type'] !== "messageSticker"
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
                    text: messageListItem.isOwnMessage ? qsTr("You") : Emoji.emojify(messageListItem.isAnonymous ? page.chatInformation.title : Functions.getUserName(messageListItem.userInformation), font.pixelSize)
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.weight: Font.ExtraBold
                    color: messageListItem.textColor
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Fade
                    textFormat: Text.StyledText
                    horizontalAlignment: messageListItem.textAlign
                    visible: precalculatedValues.showUserInfo
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
                    active: myMessage.reply_to_message_id !== 0
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
                                visible: true
                                inReplyToMessage: messageInReplyToLoader.inReplyToMessage
                                inReplyToMessageDeleted: messageInReplyToLoader.inReplyToMessageDeleted
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    messageOverlayLoader.overlayMessage = messageInReplyToRow.inReplyToMessage;
                                    messageOverlayLoader.active = true;
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
                                if (myMessage.forward_info.origin["@type"] === "messageForwardOriginChannel") {
                                    var otherChatInformation = tdLibWrapper.getChat(myMessage.forward_info.origin.chat_id);
                                    forwardedThumbnail.photoData = (typeof otherChatInformation.photo !== "undefined") ? otherChatInformation.photo.small : {};
                                    forwardedChannelText.text = Emoji.emojify(otherChatInformation.title, Theme.fontSizeExtraSmall);
                                } else if (myMessage.forward_info.origin["@type"] === "messageForwardOriginUser") {
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
                    rightPadding: interactionLoader.active ? interactionLoader.width : 0
                    MouseArea {
                        anchors.fill: parent
                        enabled: !messageListItem.precalculatedValues.pageIsSelecting
                        onClicked: {
                            messageDateText.useElapsed = !messageDateText.useElapsed;
                            messageDateText.text = getMessageStatusText(myMessage, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                        }
                    }

                    Loader {
                        id: interactionLoader
                        height: parent.height
                        anchors.right: parent.right
                        asynchronous: true
                        active: chatPage.isChannel && messageViewCount
                        sourceComponent: Component {
                            Label {
                                text: Functions.getShortenedCount(messageViewCount)
                                leftPadding: Theme.iconSizeSmall
                                font.pixelSize: Theme.fontSizeTiny
                                color: Theme.secondaryColor
                                Icon {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: Theme.iconSizeExtraSmall
                                    height: Theme.iconSizeExtraSmall
                                    opacity: 0.6
                                    source: "../../images/icon-s-eye.svg"
                                    sourceSize {
                                        width: Theme.iconSizeExtraSmall
                                        height: Theme.iconSizeExtraSmall
                                    }
                                }
                            }
                        }

                    }
                }

            }

        }

    }

}
