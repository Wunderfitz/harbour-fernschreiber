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
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji
import "../js/debug.js" as Debug

Flickable {
    id: messageOverlayFlickable
    anchors.fill: parent
    boundsBehavior: Flickable.StopAtBounds
    contentHeight: messageContentColumn.height
    clip: true

    property var overlayMessage;
    property bool showHeader: true
    readonly property var userInformation: tdLibWrapper.getUserInformation(overlayMessage.sender.user_id);
    readonly property bool isOwnMessage: tdLibWrapper.getUserInformation().id === overlayMessage.sender.user_id;
    readonly property bool isAnonymous: overlayMessage.sender["@type"] === "messageSenderChat"
    property bool hasContentComponent: overlayMessage.content && chatView.delegateMessagesContent.indexOf(overlayMessage.content['@type']) > -1
    signal requestClose;

    function getOriginalAuthor(forwardInformation, fontSize) {
        switch (forwardInformation.origin["@type"]) {
            case "messageForwardOriginChannel":
                var otherChatInformation = tdLibWrapper.getChat(forwardInformation.origin.chat_id);
                return Emoji.emojify(otherChatInformation.title, fontSize);
            case "messageForwardOriginUser":
                var otherUserInformation = tdLibWrapper.getUserInformation(forwardInformation.origin.sender.user_id);
                return Emoji.emojify(Functions.getUserName(otherUserInformation), fontSize);
            default:
                return Emoji.emojify(forwardInformation.origin.sender_name, fontSize);
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
            if (messageOverlayFlickable.hasContentComponent) {
                var type = overlayMessage.content["@type"];
                overlayExtraContentLoader.setSource(
                            "../components/messageContent/" + type.charAt(0).toUpperCase() + type.substring(1) + ".qml",
                            {
                                overlayFlickable: messageOverlayFlickable
                            })
            } else if(overlayMessage.content && overlayMessage.content.web_page) {
                overlayWebPagePreviewLoader.active = true;
            }
        }
    }

    Rectangle {
        id: messageContentBackground
        color: Theme.overlayBackgroundColor
        opacity: 0.7
        width: parent.width
        height: messageContentColumn.height >= messageOverlayFlickable.height ? messageContentColumn.height : messageOverlayFlickable.height
        MouseArea {
            anchors.fill: parent
            onClicked: {
                messageOverlayFlickable.requestClose();
            }
        }
    }

    Column {
        id: messageContentColumn
        spacing: Theme.paddingMedium
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingMedium
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - ( 2 * Theme.horizontalPageMargin )

        Row {
            visible: messageOverlayFlickable.showHeader
            width: parent.width
            spacing: Theme.paddingMedium
            ProfileThumbnail {
                id: overlayMessagePictureThumbnail
                photoData: messageOverlayFlickable.isAnonymous ? ((typeof chatPage.chatInformation.photo !== "undefined") ? chatPage.chatInformation.photo.small : {}) : ((typeof messageOverlayFlickable.userInformation.profile_photo !== "undefined") ? messageOverlayFlickable.userInformation.profile_photo.small : ({}))
                replacementStringHint: overlayMessageUserText.text
                width: Theme.itemSizeLarge
                height: Theme.itemSizeLarge
            }
            Label {
                id: overlayMessageUserText

                width: parent.width - overlayMessagePictureThumbnail.width
                anchors.verticalCenter: parent.verticalCenter
                text: messageOverlayFlickable.isOwnMessage ? qsTr("You") : Emoji.emojify(messageOverlayFlickable.isAnonymous ? chatPage.chatInformation.title : Functions.getUserName(messageOverlayFlickable.userInformation), font.pixelSize)
                font.pixelSize: Theme.fontSizeExtraLarge
                font.weight: Font.ExtraBold
                maximumLineCount: 1
                truncationMode: TruncationMode.Fade
                textFormat: Text.StyledText
            }
        }

        MessageViaLabel {
            message: overlayMessage
        }

        Text {
            id: overlayForwardedInfoText
            width: parent.width
            visible: typeof overlayMessage.forward_info !== "undefined"
            font.pixelSize: Theme.fontSizeSmall
            font.italic: true
            textFormat: Text.StyledText
            color: Theme.secondaryColor
            wrapMode: Text.Wrap
            text: visible ? qsTr("This message was forwarded. Original author: %1").arg(getOriginalAuthor(overlayMessage.forward_info, font.pixelSize)) : ""
        }

        Text {
            id: overlayMessageText
            width: parent.width
            text: Emoji.emojify(Functions.getMessageText(overlayMessage, false, tdLibWrapper.getUserInformation().id, false), font.pixelSize)
            font.pixelSize: Theme.fontSizeMedium
            color: Theme.primaryColor
            wrapMode: Text.Wrap
            textFormat: Text.StyledText
            onLinkActivated: {
                Functions.handleLink(link);
            }
            linkColor: Theme.highlightColor
            visible: (text !== "")
        }

        Loader {
            id: overlayWebPagePreviewLoader
            active: false
            asynchronous: true
            width: parent.width

            sourceComponent: Component {
                id: webPagePreviewComponent
                WebPagePreview {
                    id: webPagePreview

                    onImplicitHeightChanged: {
                        overlayWebPagePreviewLoader.height = webPagePreview.implicitHeight;
                    }

                    webPageData: overlayMessage.content.web_page
                    largerFontSize: true
                    width: parent.width
                }
            }
        }

        Loader {
            id: overlayExtraContentLoader
            width: parent.width
            asynchronous: true
        }

        Loader {
            id: replyMarkupLoader
            property var myMessage: overlayMessage
            width: parent.width
            height: active ? (overlayMessage.reply_markup.rows.length * (Theme.itemSizeSmall + Theme.paddingSmall) - Theme.paddingSmall) : 0
            asynchronous: true
            active: !!overlayMessage.reply_markup && myMessage.reply_markup.rows
            source: Qt.resolvedUrl("ReplyMarkupButtons.qml")
        }

        Timer {
            id: messageDateUpdater
            interval: 60000
            running: true
            repeat: true
            onTriggered: {
                overlayMessageDateText.text = ( overlayMessageDateText.useElapsed ? Functions.getDateTimeElapsed(overlayMessage.date) : Functions.getDateTimeTranslated(overlayMessage.date) );
            }
        }

        Text {
            width: parent.width

            property bool useElapsed: true

            id: overlayMessageDateText
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
            text: ( useElapsed ? Functions.getDateTimeElapsed(overlayMessage.date) : Functions.getDateTimeTranslated(overlayMessage.date) )
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    overlayMessageDateText.useElapsed = !overlayMessageDateText.useElapsed;
                    overlayMessageDateText.text = ( useElapsed ? Functions.getDateTimeElapsed(overlayMessage.date) : Functions.getDateTimeTranslated(overlayMessage.date) );
                }
            }
        }

        Label {
            id: separatorLabel
            width: parent.width
            font.pixelSize: Theme.fontSizeSmall
        }

    }

    VerticalScrollDecorator {}
}
