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
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import QtQml.Models 2.3

ListItem {
    id: messageListItem
    contentHeight: messageBackground.height + Theme.paddingMedium
    readonly property var myMessage: display
    readonly property var userInformation: tdLibWrapper.getUserInformation(myMessage.sender_user_id)
    property QtObject precalculatedValues: ListView.view.precalculatedValues
    readonly property color textColor: isOwnMessage ? Theme.highlightColor : Theme.primaryColor
    readonly property int textAlign: isOwnMessage ? Text.AlignRight : Text.AlignLeft
    readonly property Page page: precalculatedValues.page
    readonly property bool isSelected: messageListItem.precalculatedValues.pageIsSelecting && page.selectedMessages.some(function(existingMessage) {
        return existingMessage.id === myMessage.id;
    });
    readonly property bool isOwnMessage: page.myUserId === myMessage.sender_user_id
    readonly property string extraContentComponentName: typeof myMessage.content !== "undefined"
                                               && typeof chatView.contentComponentNames[myMessage.content['@type']]  !== "undefined" ?
                                                   chatView.contentComponentNames[myMessage.content['@type']] : ""

    readonly property ObjectModel additionalContextItems: ObjectModel {}
    highlighted: down || isSelected
    openMenuOnPressAndHold: !messageListItem.precalculatedValues.pageIsSelecting

    onClicked: {
        if(messageListItem.precalculatedValues.pageIsSelecting) {
            page.toggleMessageSelection(myMessage);
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
                    model: messageListItem.additionalContextItems
                }

                MenuItem {
                    onClicked: {
                        newMessageInReplyToRow.inReplyToMessage = myMessage;
                        newMessageTextField.focus = true;
                    }
                    text: qsTr("Reply to Message")
                }
                MenuItem {
                    onClicked: {
                        newMessageColumn.editMessageId = myMessage.id;
                        newMessageTextField.text = Functions.getMessageText(myMessage, false, false);
                        newMessageTextField.focus = true;
                    }
                    text: qsTr("Edit Message")
                    visible: myMessage.can_be_edited
                }
                MenuItem {
                    onClicked: {
                        Clipboard.text = Functions.getMessageText(myMessage, true, false);
                    }
                    text: qsTr("Copy Message to Clipboard")
                }
                MenuItem {
                    onClicked: {
                        page.toggleMessageSelection(myMessage);
                    }
                    text: qsTr("Select Message")
                }
                MenuItem {

                    function amIVisible() {
                        console.log("Is pin message menu visible?");
                        if (page.isPrivateChat) {
                            console.log("Private Chat: No!");
                            return false;
                        }
                        if (page.chatGroupInformation.status["@type"] === "chatMemberStatusCreator") {
                            console.log("Creator of this chat: Yes!");
                            return true;
                        }
                        if (page.chatInformation.permissions.can_pin_messages) {
                            console.log("All people can pin: Yes!");
                            return true;
                        }
                        if (page.chatGroupInformation.status["@type"] === "chatMemberStatusAdministrator") {
                            console.log("Admin with privileges? " + page.chatGroupInformation.status.can_pin_messages);
                            return page.chatGroupInformation.status.can_pin_messages;
                        }
                        console.log("Something else: No!");
                        return false;
                    }

                    onClicked: {
                        tdLibWrapper.pinMessage(page.chatInformation.id, myMessage.id);
                    }
                    text: qsTr("Pin Message")
                    visible: amIVisible()
                }
                MenuItem {
                    onClicked: {
                        var chatId = page.chatInformation.id;
                        var messageId = myMessage.id;
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
            console.log("[ChatModel] Messages in this chat were read, new last read: " + lastReadSentIndex + ", updating description for index " + index + ", status: " + (index <= lastReadSentIndex));
            messageDateText.text = getMessageStatusText(myMessage, index, lastReadSentIndex, messageDateText.useElapsed);
        }
        onMessageUpdated: {
            if (index === modelIndex) {
                console.log("[ChatModel] This message was updated, index " + index + ", updating content...");
                messageDateText.text = getMessageStatusText(myMessage, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                messageText.text = Emoji.emojify(Functions.getMessageText(myMessage, false, messageListItem.isOwnMessage), messageText.font.pixelSize);
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onReceivedMessage: {
            if (messageId === myMessage.reply_to_message_id.toString()) {
                messageInReplyToLoader.inReplyToMessage = message;
            }
        }
    }

    Component.onCompleted: {
        delegateComponentLoadingTimer.start();

        if (myMessage.reply_to_message_id !== 0) {
            tdLibWrapper.getMessage(page.chatInformation.id, myMessage.reply_to_message_id);
        }
    }

    Timer {
        id: delegateComponentLoadingTimer
        interval: 500
        repeat: false
        running: false
        onTriggered: {
            if (typeof myMessage.content !== "undefined") {
                if (messageListItem.extraContentComponentName !== "") {
                    extraContentLoader.setSource(
                                "../components/" +messageListItem.extraContentComponentName +".qml",
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
                    photoData: (typeof messageListItem.userInformation.profile_photo !== "undefined") ? messageListItem.userInformation.profile_photo.small : ({})
                    replacementStringHint: userText.text
                    width: Theme.itemSizeSmall
                    height: Theme.itemSizeSmall
                    visible: precalculatedValues.showUserInfo
                    MouseArea {
                        anchors.fill: parent
                        enabled: !messageListItem.precalculatedValues.pageIsSelecting
                        onClicked: {
                            tdLibWrapper.createPrivateChat(messageListItem.userInformation.id);
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
                color: isUnread ? Theme.secondaryHighlightColor : Theme.secondaryColor
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


                Text {
                    id: userText

                    width: parent.width
                    text: messageListItem.isOwnMessage ? qsTr("You") : Emoji.emojify(Functions.getUserName(messageListItem.userInformation), font.pixelSize)
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.weight: Font.ExtraBold
                    color: messageListItem.textColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    textFormat: Text.StyledText
                    horizontalAlignment: messageListItem.textAlign
                    visible: precalculatedValues.showUserInfo
                    MouseArea {
                        anchors.fill: parent
                        enabled: !messageListItem.precalculatedValues.pageIsSelecting
                        onClicked: {
                            tdLibWrapper.createPrivateChat(messageListItem.userInformation.id);
                        }
                    }
                }

                Loader {
                    id: messageInReplyToLoader
                    active: myMessage.reply_to_message_id !== 0
                    width: parent.width
                    // text height ~= 1,28*font.pixelSize
                    height: active ? precalculatedValues.messageInReplyToHeight : 0
                    property var inReplyToMessage;
                    sourceComponent: Component {
                        Item {
                            width: messageInReplyToRow.width
                            height: messageInReplyToRow.height
                            InReplyToRow {
                                id: messageInReplyToRow
                                myUserId: page.myUserId
                                visible: true
                                inReplyToMessage: messageInReplyToLoader.inReplyToMessage
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
                                Text {
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                    color: Theme.primaryColor
                                    width: parent.width
                                    font.italic: true
                                    elide: Text.ElideRight
                                    textFormat: Text.StyledText
                                    text: qsTr("Forwarded Message")
                                    onTruncatedChanged: {
                                        // There is obviously a bug in QML in truncating text with images.
                                        // We simply remove Emojis then...
                                        if (truncated) {
                                            text = text.replace(/\<img [^>]+\/\>/g, "");
                                        }
                                    }
                                }
                                Text {
                                    id: forwardedChannelText
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                    color: Theme.primaryColor
                                    width: parent.width
                                    font.bold: true
                                    elide: Text.ElideRight
                                    textFormat: Text.StyledText
                                    text: Emoji.emojify(forwardedMessageInformationRow.otherChatInformation.title, font.pixelSize)
                                    onTruncatedChanged: {
                                        // There is obviously a bug in QML in truncating text with images.
                                        // We simply remove Emojis then...
                                        if (truncated) {
                                            text = text.replace(/\<img [^>]+\/\>/g, "");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Text {
                    id: messageText
                    width: parent.width
                    text: Emoji.emojify(Functions.getMessageText(myMessage, false, messageListItem.isOwnMessage), font.pixelSize)
                    font.pixelSize: Theme.fontSizeSmall
                    color: messageListItem.textColor
                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText
                    onLinkActivated: {
                        Functions.handleLink(link);
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
                    height: typeof myMessage.content.web_page !== "undefined" ? precalculatedValues.webPagePreviewHeight : 0

                    sourceComponent: Component {
                        id: webPagePreviewComponent
                        WebPagePreview {
                            id: webPagePreview

                            onImplicitHeightChanged: {
                                webPagePreviewLoader.height = webPagePreview.implicitHeight;
                            }

                            webPageData: myMessage.content.web_page
                            width: parent.width
                        }
                    }
                }
                Loader {
                    id: extraContentLoader
                    width: parent.width
                    asynchronous: true
                    height: item ? item.height : (messageListItem.extraContentComponentName !== "" ? chatView.getContentComponentHeight(messageListItem.extraContentComponentName, myMessage.content, width) : 0)
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

            }

        }

    }

}
