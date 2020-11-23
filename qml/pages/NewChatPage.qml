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
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: newChatPage
    allowedOrientations: Orientation.All

    property var contacts;
    property bool isLoading: true;

    onStatusChanged: {
        if (status === PageStatus.Active) {
            newChatPage.contacts = tdLibWrapper.getContactsFullInfo();
            newChatPage.isLoading = false;
        }
    }

    SilicaFlickable {
        id: newChatContainer
        contentHeight: newChatPage.height
        anchors.fill: parent

        Column {
            id: newChatPageColumn
            width: newChatPage.width
            height: newChatPage.height

            PageHeader {
                id: newChatPageHeader
                title: qsTr("Your Contacts")
            }

            Item {
                id: contactsItem

                width: newChatPageColumn.width
                height: newChatPageColumn.height - newChatPageHeader.height

                SilicaListView {
                    id: contactsListView
                    model: newChatPage.contacts
                    clip: true
                    width: parent.width
                    height: parent.height
                    visible: !newChatPage.isLoading
                    opacity: visible ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }                    

                    signal newChatInitiated ( int currentIndex )

                    ViewPlaceholder {
                        y: Theme.paddingLarge
                        enabled: contactsListView.count === 0
                        text: qsTr("You don't have any contacts.")
                    }

                    delegate: Item {
                        id: newChatListItem
                        width: parent.width
                        height: contactListItem.height

                        PhotoTextsListItem {
                            id: contactListItem

                            opacity: visible ? 1 : 0
                            Behavior on opacity { FadeAnimation {} }

                            pictureThumbnail {
                                photoData: (typeof modelData.profile_photo !== "undefined") ? modelData.profile_photo.small : {}
                            }
                            width: parent.width

                            primaryText.text: Emoji.emojify(Functions.getUserName(modelData), primaryText.font.pixelSize, "../js/emoji/")
                            prologSecondaryText.text: "@" + ( modelData.username !== "" ? modelData.username : modelData.id )
                            tertiaryText {
                                maximumLineCount: 1
                                text: Functions.getChatPartnerStatusText(modelData.status["@type"], modelData.status.was_online);
                            }

                            onClicked: {
                                contactsListView.newChatInitiated(index);
                            }

                            Connections {
                                target: contactsListView
                                onNewChatInitiated: {
                                    if (index === currentIndex) {
                                        contactListItem.visible = false;
                                    } else {
                                        contactListItem.visible = true;
                                    }
                                }
                            }
                        }

                        Column {
                            id: selectChatTypeColumn
                            visible: !contactListItem.visible
                            opacity: visible ? 1 : 0
                            Behavior on opacity { FadeAnimation {} }
                            width: parent.width
                            height: contactListItem.height

                            Item {
                                width: parent.width
                                height: parent.height - chatTypeSeparator.height

                                Rectangle {
                                    anchors.fill: parent
                                    opacity: 0.3
                                    color: Theme.overlayBackgroundColor
                                }

                                Item {
                                    id: privateChatItem
                                    height: parent.height
                                    width: parent.width / 2 + ( Theme.horizontalPageMargin / 2 )
                                    anchors.left: parent.left
                                    anchors.top: parent.top

                                    Rectangle {
                                        id: privateChatHighlightBackground
                                        anchors.fill: parent
                                        color: Theme.highlightBackgroundColor
                                        opacity: 0.5
                                        visible: false
                                    }

                                    Row {
                                        width: parent.width
                                        height: parent.height - ( 2 * Theme.paddingSmall )
                                        anchors.verticalCenter: parent.verticalCenter

                                        IconButton {
                                            id: privateChatButton
                                            width: Theme.itemSizeLarge
                                            height: Theme.itemSizeLarge
                                            icon.source: "image://theme/icon-m-chat"
                                            anchors.verticalCenter: parent.verticalCenter
                                            onClicked: {
                                                tdLibWrapper.createPrivateChat(modelData.id);
                                            }
                                        }

                                        Column {
                                            height: parent.height
                                            width: parent.width - privateChatButton.width - Theme.horizontalPageMargin
                                            anchors.verticalCenter: parent.verticalCenter
                                            spacing: Theme.paddingSmall
                                            Text {
                                                id: privateChatHeader
                                                width: parent.width
                                                font.pixelSize: Theme.fontSizeMedium
                                                font.weight: Font.ExtraBold
                                                color: Theme.primaryColor
                                                maximumLineCount: 1
                                                elide: Text.ElideRight
                                                textFormat: Text.StyledText
                                                text: qsTr("Private Chat")
                                            }
                                            Text {
                                                width: parent.width
                                                height: parent.height - privateChatHeader.height - Theme.paddingSmall
                                                font.pixelSize: Theme.fontSizeTiny
                                                color: Theme.secondaryColor
                                                wrapMode: Text.Wrap
                                                elide: Text.ElideRight
                                                textFormat: Text.StyledText
                                                text: qsTr("Transport-encrypted, stored in Telegram Cloud, sharable across devices")
                                            }
                                        }

                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            tdLibWrapper.createPrivateChat(modelData.id);
                                        }
                                        onPressed: {
                                            privateChatHighlightBackground.visible = true;
                                        }
                                        onReleased: {
                                            privateChatHighlightBackground.visible = false;
                                        }
                                    }
                                }

                                Item {
                                    id: secretChatItem
                                    height: parent.height
                                    width: parent.width / 2 + ( Theme.horizontalPageMargin / 2 )
                                    anchors.left: privateChatItem.right
                                    anchors.top: parent.top

                                    Rectangle {
                                        id: secretChatHighlightBackground
                                        anchors.fill: parent
                                        color: Theme.highlightBackgroundColor
                                        opacity: 0.5
                                        visible: false
                                    }

                                    Row {
                                        width: parent.width
                                        height: parent.height - ( 2 * Theme.paddingSmall )
                                        anchors.verticalCenter: parent.verticalCenter

                                        IconButton {
                                            id: secretChatButton
                                            width: Theme.itemSizeLarge
                                            height: Theme.itemSizeLarge
                                            icon.source: "image://theme/icon-m-device-lock"
                                            anchors.verticalCenter: parent.verticalCenter
                                            onClicked: {
                                                console.log("SECRET CHAT!");
                                            }
                                        }

                                        Column {
                                            height: parent.height
                                            width: parent.width - secretChatButton.width - Theme.horizontalPageMargin
                                            anchors.verticalCenter: parent.verticalCenter
                                            spacing: Theme.paddingSmall
                                            Text {
                                                width: parent.width
                                                font.pixelSize: Theme.fontSizeMedium
                                                font.weight: Font.ExtraBold
                                                color: Theme.primaryColor
                                                maximumLineCount: 1
                                                elide: Text.ElideRight
                                                textFormat: Text.StyledText
                                                text: qsTr("Secret Chat")
                                            }
                                            Text {
                                                width: parent.width
                                                height: parent.height - privateChatHeader.height - Theme.paddingSmall
                                                font.pixelSize: Theme.fontSizeTiny
                                                color: Theme.secondaryColor
                                                wrapMode: Text.Wrap
                                                elide: Text.ElideRight
                                                textFormat: Text.StyledText
                                                text: qsTr("End-to-end-encrypted, accessible on this device only")
                                            }
                                        }

                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            console.log("SECRET CHAT!");
                                        }
                                        onPressed: {
                                            secretChatHighlightBackground.visible = true;
                                        }
                                        onReleased: {
                                            secretChatHighlightBackground.visible = false;
                                        }
                                    }
                                }

                            }

                            Separator {
                                id: chatTypeSeparator
                                width: parent.width
                                color: Theme.primaryColor
                                horizontalAlignment: Qt.AlignHCenter
                            }

                        }

                    }

                    VerticalScrollDecorator {}
                }

                Column {

                    opacity: visible ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    visible: newChatPage.isLoading
                    width: parent.width
                    height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium

                    spacing: Theme.paddingMedium

                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: loadingLabel
                        text: qsTr("Loading contacs...")
                    }

                    BusyIndicator {
                        id: loadingBusyIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: newChatPage.isLoading
                        size: BusyIndicatorSize.Large
                    }
                }

            }

        }
    }
}
