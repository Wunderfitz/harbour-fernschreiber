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

ListItem {
    id: messageListItem
    contentHeight: messageBackground.height + Theme.paddingMedium
    property variant myMessage: display
    property variant userInformation: tdLibWrapper.getUserInformation(display.sender_user_id)
    property Page page: chatPage

    property bool isOwnMessage: chatPage.myUserId === display.sender_user_id
    property string extraContentComponentName: typeof display.content !== "undefined"
                                               && chatView.contentComponentNames.hasOwnProperty(display.content['@type']) ?
                                                   chatView.contentComponentNames[display.content['@type']] : ""
    menu: ContextMenu {
        MenuItem {
            onClicked: {
                newMessageInReplyToRow.inReplyToMessage = display;
                newMessageTextField.focus = true;
            }
            text: qsTr("Reply to Message")
        }
        MenuItem {
            onClicked: {
                newMessageColumn.editMessageId = display.id;
                newMessageTextField.text = Functions.getMessageText(display, false, false);
                newMessageTextField.focus = true;
            }
            text: qsTr("Edit Message")
            visible: display.can_be_edited
        }
        MenuItem {
            onClicked: {
                Clipboard.text = Functions.getMessageText(display, true, false);
            }
            text: qsTr("Copy Message to Clipboard")
        }
        MenuItem {
            onClicked: {
                var chatId = chatInformation.id;
                var messageId = display.id;
                Remorse.itemAction(messageListItem, qsTr("Message deleted"), function() { tdLibWrapper.deleteMessages(chatId, [ messageId]);  })
            }
            text: qsTr("Delete Message")
            visible: display.can_be_deleted_for_all_users || (display.can_be_deleted_only_for_self && display.chat_id === chatPage.myUserId)
        }
    }

    Connections {
        target: chatModel
        onUnreadCountUpdated: {
            messageBackground.color = index > ( chatView.count - unreadCount - 1 ) ? Theme.secondaryHighlightColor : Theme.secondaryColor;
            messageBackground.opacity = index > ( chatView.count - unreadCount - 1 ) ? 0.5 : 0.2;
        }
        onNewMessageReceived: {
            messageBackground.color = index > ( chatView.count - chatInformation.unreadCount - 1 ) ? Theme.secondaryHighlightColor : Theme.secondaryColor;
            messageBackground.opacity = index > ( chatView.count - chatInformation.unreadCount - 1 ) ? 0.5 : 0.2;
        }

        onLastReadSentMessageUpdated: {
            console.log("[ChatModel] Messages in this chat were read, new last read: " + lastReadSentIndex + ", updating description for index " + index + ", status: " + (index <= lastReadSentIndex));
            messageDateText.text = getMessageStatusText(display, index, lastReadSentIndex, messageDateText.useElapsed);
        }
        onMessageUpdated: {
            if (index === modelIndex) {
                console.log("[ChatModel] This message was updated, index " + index + ", updating content...");
                messageDateText.text = getMessageStatusText(display, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                messageText.text = Emoji.emojify(Functions.getMessageText(display, false, messageListItem.isOwnMessage), messageText.font.pixelSize);
                if(locationPreviewLoader.active && locationPreviewLoader.status === Loader.Ready) {
                    locationPreviewLoader.item.locationData = display.content.location;
                    locationPreviewLoader.item.updatePicture()
                }
            }
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

    Component.onCompleted: {
        delegateComponentLoadingTimer.start();
    }

    Timer {
        id: delegateComponentLoadingTimer
        interval: 500
        repeat: false
        running: false
        onTriggered: {
            if (typeof display.content !== "undefined") {
                if (messageListItem.extraContentComponentName !== "") {
                    extraContentLoader.setSource(
                                "../components/" +messageListItem.extraContentComponentName +".qml",
                                {
                                    messageListItem: messageListItem
                                })
                } else {
                    if (typeof display.content.web_page !== "undefined") { // only in messageText
                        webPagePreviewLoader.active = true;
                    }
                }
            }
        }
    }

    Row {
        id: messageTextRow
        spacing: Theme.paddingSmall
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Loader {
            id: profileThumbnailLoader
            active: (( chatPage.isBasicGroup || chatPage.isSuperGroup ) && !chatPage.isChannel)
            asynchronous: true
            width: active ? Theme.itemSizeSmall : 0
            height: active ? Theme.itemSizeSmall : 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.paddingSmall
            sourceComponent: Component {
                ProfileThumbnail {
                    id: messagePictureThumbnail
                    photoData: (typeof messageListItem.userInformation.profile_photo !== "undefined") ? messageListItem.userInformation.profile_photo.small : ""
                    replacementStringHint: userText.text
                    width: visible ? Theme.itemSizeSmall : 0
                    height: visible ? Theme.itemSizeSmall : 0
                    visible: ( chatPage.isBasicGroup || chatPage.isSuperGroup ) && !chatPage.isChannel
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tdLibWrapper.createPrivateChat(messageListItem.userInformation.id);
                        }
                    }
                }
            }
        }

        Item {
            id: messageTextItem

            width: parent.width - profileThumbnailLoader.width - Theme.paddingSmall
            height: messageBackground.height

            Rectangle {
                id: messageBackground
                anchors {
                    left: parent.left
                    leftMargin: messageListItem.isOwnMessage ? 2 * Theme.horizontalPageMargin : 0
                    right: parent.right
                    rightMargin: messageListItem.isOwnMessage ? 0 : 2 * Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                height: messageTextColumn.height + ( 2 * Theme.paddingMedium )

                color: index > ( chatView.count - chatInformation.unread_count - 1 ) ? Theme.secondaryHighlightColor : Theme.secondaryColor
                radius: parent.width / 50
                opacity: index > ( chatView.count - chatInformation.unread_count - 1 ) ? 0.5 : 0.2
                visible: appSettings.showStickersAsImages || display.content['@type'] !== "messageSticker"
                Behavior on color { ColorAnimation { duration: 200 } }
                Behavior on opacity { FadeAnimation {} }
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


                Text {
                    id: userText

                    width: parent.width
                    text: !messageListItem.isOwnMessage ? Emoji.emojify(Functions.getUserName(messageListItem.userInformation), font.pixelSize) : qsTr("You")
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.weight: Font.ExtraBold
                    color: messageListItem.isOwnMessage ? Theme.highlightColor : Theme.primaryColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    textFormat: Text.StyledText
                    horizontalAlignment: messageListItem.isOwnMessage ? Text.AlignRight : Text.AlignLeft
                    visible: ( chatPage.isBasicGroup || chatPage.isSuperGroup ) && !chatPage.isChannel
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tdLibWrapper.createPrivateChat(messageListItem.userInformation.id);
                        }
                    }
                }

                InReplyToRow {
                    id: messageInReplyToRow
                    myUserId: chatPage.myUserId
                    visible: false
                }

                Loader {
                    id: forwardedInformationLoader
                    active: typeof display.forward_info !== "undefined"
                    asynchronous: true
                    width: parent.width
                    //                                        height: active ? ( item ? item.height : Theme.itemSizeExtraSmall ) : 0
                    sourceComponent: Component {
                        Row {
                            id: forwardedMessageInformationRow
                            spacing: Theme.paddingSmall
                            width: parent.width

                            Component.onCompleted: {
                                if (display.forward_info.origin["@type"] === "messageForwardOriginChannel") {
                                    var otherChatInformation = tdLibWrapper.getChat(display.forward_info.origin.chat_id);
                                    forwardedThumbnail.photoData = (typeof otherChatInformation.photo !== "undefined") ? otherChatInformation.photo.small : "";
                                    forwardedChannelText.text = Emoji.emojify(otherChatInformation.title, Theme.fontSizeExtraSmall);
                                } else if (display.forward_info.origin["@type"] === "messageForwardOriginUser") {
                                    var otherUserInformation = tdLibWrapper.getUserInformation(display.forward_info.origin.sender_user_id);
                                    forwardedThumbnail.photoData = (typeof otherUserInformation.profile_photo !== "undefined") ? otherUserInformation.profile_photo.small : "";
                                    forwardedChannelText.text = Emoji.emojify(Functions.getUserName(otherUserInformation), Theme.fontSizeExtraSmall);
                                } else {
                                    forwardedThumbnail.photoData = "";
                                    forwardedChannelText.text = Emoji.emojify(display.forward_info.origin.sender_user_name, Theme.fontSizeExtraSmall);
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
                                width: parent.width
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
                    text: Emoji.emojify(Functions.getMessageText(display, false, messageListItem.isOwnMessage), font.pixelSize)
                    font.pixelSize: Theme.fontSizeSmall
                    color: messageListItem.isOwnMessage ? Theme.highlightColor : Theme.primaryColor
                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText
                    onLinkActivated: {
                        Functions.handleLink(link);
                    }
                    horizontalAlignment: messageListItem.isOwnMessage ? Text.AlignRight : Text.AlignLeft
                    linkColor: Theme.highlightColor
                    visible: (text !== "")
                }

                Loader {
                    id: webPagePreviewLoader
                    active: false
                    asynchronous: true
                    width: parent.width
                    height: typeof display.content.web_page !== "undefined" ? ( (parent.width * 2 / 3) + (6 * Theme.fontSizeExtraSmall) + ( 7 * Theme.paddingSmall) ) : 0

                    sourceComponent: Component {
                        id: webPagePreviewComponent
                        WebPagePreview {
                            id: webPagePreview

                            onImplicitHeightChanged: {
                                webPagePreviewLoader.height = webPagePreview.implicitHeight;
                            }

                            webPageData: display.content.web_page
                            width: parent.width
                        }
                    }
                }
                Loader {
                    id: extraContentLoader
                    width: parent.width
                    asynchronous: true
                    property int heightPreset: messageListItem.extraContentComponentName !== "" ? chatView.getContentComponentHeight(messageListItem.extraContentComponentName, display.content, width) : 0
                    height: item ? item.height : heightPreset
                }

                Timer {
                    id: messageDateUpdater
                    interval: 60000
                    running: true
                    repeat: true
                    onTriggered: {
                        messageDateText.text = getMessageStatusText(display, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                    }
                }


                Text {
                    width: parent.width

                    property bool useElapsed: true

                    id: messageDateText
                    font.pixelSize: Theme.fontSizeTiny
                    color: messageListItem.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    horizontalAlignment: messageListItem.isOwnMessage ? Text.AlignRight : Text.AlignLeft
                    text: getMessageStatusText(display, index, chatView.lastReadSentIndex, messageDateText.useElapsed)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            messageDateText.useElapsed = !messageDateText.useElapsed;
                            messageDateText.text = getMessageStatusText(display, index, chatView.lastReadSentIndex, messageDateText.useElapsed);
                        }
                    }
                }

            }

        }

    }

}
