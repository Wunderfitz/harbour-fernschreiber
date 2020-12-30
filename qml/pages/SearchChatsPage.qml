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
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/debug.js" as Debug
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: searchChatsPage
    allowedOrientations: Orientation.All

    function resetFocus() {
        publicChatsSearchField.focus = false;
        searchChatsPage.focus = true;
    }

    Timer {
        id: searchPublicChatsTimer
        interval: 800
        running: false
        repeat: false
        onTriggered: {
            Debug.log("Searching for '" + publicChatsSearchField.text + "'");
            tdLibWrapper.searchPublicChats(publicChatsSearchField.text);
            searchChatsPage.isLoading = true;
        }
    }

    Connections {
        target: tdLibWrapper
        onChatsReceived: {
            searchChatsPage.isLoading = false;
            Debug.log(JSON.stringify(chats));
            chatsFound = chats;
        }
        onErrorReceived: {
            searchChatsPage.isLoading = false;
            Functions.handleErrorMessage(code, message);
        }
    }

    property bool isLoading: false;
    property var chatsFound;
    readonly property var ownUserId: tdLibWrapper.getUserInformation().id;

    SilicaFlickable {
        id: searchChatsContainer
        contentHeight: searchChatsPage.height
        anchors.fill: parent

        Column {
            id: searchChatsPageColumn
            width: searchChatsPage.width
            height: searchChatsPage.height

            PageHeader {
                id: searchChatsPageHeader
                title: qsTr("Search Chats")
            }

            Item {
                id: publicChatsItem

                width: searchChatsPageColumn.width
                height: searchChatsPageColumn.height - searchChatsPageHeader.height

                Column {

                    width: parent.width
                    height: parent.height

                    SearchField {
                        id: publicChatsSearchField
                        width: parent.width
                        placeholderText: qsTr("Search a chat...")
                        focus: true

                        onTextChanged: {
                            searchPublicChatsTimer.restart();
                        }

                        EnterKey.iconSource: "image://theme/icon-m-enter-close"
                        EnterKey.onClicked: {
                            resetFocus();
                        }

                    }

                    SilicaListView {
                        id: searchChatsListView
                        clip: true
                        width: parent.width
                        height: parent.height - publicChatsSearchField.height
                        visible: !searchChatsPage.isLoading
                        opacity: visible ? 1 : 0
                        Behavior on opacity { FadeAnimation {} }
                        model: searchChatsPage.chatsFound.chat_ids

                        ViewPlaceholder {
                            y: Theme.paddingLarge
                            enabled: searchChatsListView.count === 0
                            text: publicChatsSearchField.text.length < 5 ? qsTr("Enter your query to start searching (at least 5 characters needed)") : qsTr("No chats found.")
                        }

                        delegate: Item {
                            id: foundChatListDelegate
                            width: parent.width
                            height: foundChatListItem.height

                            property var foundChatInformation: tdLibWrapper.getChat(modelData);
                            property var relatedInformation;
                            property bool isPrivateChat: false;
                            property bool isBasicGroup: false;
                            property bool isSupergroup: false;

                            Component.onCompleted: {
                                switch (foundChatInformation.type["@type"]) {
                                case "chatTypePrivate":
                                    relatedInformation = tdLibWrapper.getUserInformation(foundChatInformation.type.user_id);
                                    foundChatListItem.prologSecondaryText.text = qsTr("Private Chat");
                                    foundChatListItem.secondaryText.text = "@" + ( relatedInformation.username !== "" ? relatedInformation.username : relatedInformation.user_id );
                                    tdLibWrapper.getUserFullInfo(foundChatInformation.type.user_id);
                                    isPrivateChat = true;
                                    break;
                                case "chatTypeBasicGroup":
                                    relatedInformation = tdLibWrapper.getBasicGroup(foundChatInformation.type.basic_group_id);
                                    foundChatListItem.prologSecondaryText.text = qsTr("Group");
                                    tdLibWrapper.getGroupFullInfo(foundChatInformation.type.basic_group_id, false);
                                    isBasicGroup = true;
                                    break;
                                case "chatTypeSupergroup":
                                    relatedInformation = tdLibWrapper.getSuperGroup(foundChatInformation.type.supergroup_id);
                                    if (relatedInformation.is_channel) {
                                        foundChatListItem.prologSecondaryText.text = qsTr("Channel");
                                    } else {
                                        foundChatListItem.prologSecondaryText.text = qsTr("Group");
                                    }
                                    tdLibWrapper.getGroupFullInfo(foundChatInformation.type.supergroup_id, true);
                                    isSupergroup = true;
                                    break;
                                }
                            }

                            Connections {
                                target: tdLibWrapper
                                onUserFullInfoUpdated: {
                                    if (foundChatListDelegate.isPrivateChat && userId.toString() === foundChatListDelegate.foundChatInformation.type.user_id.toString()) {
                                        foundChatListItem.tertiaryText.text = Emoji.emojify(userFullInfo.bio, foundChatListItem.tertiaryText.font.pixelSize, "../js/emoji/");
                                    }
                                }
                                onUserFullInfoReceived: {
                                    if (foundChatListDelegate.isPrivateChat && userFullInfo["@extra"].toString() === foundChatListDelegate.foundChatInformation.type.user_id.toString()) {
                                        foundChatListItem.tertiaryText.text = Emoji.emojify(userFullInfo.bio, foundChatListItem.tertiaryText.font.pixelSize, "../js/emoji/");
                                    }
                                }

                                onBasicGroupFullInfoUpdated: {
                                    if (foundChatListDelegate.isBasicGroup && groupId.toString() === foundChatListDelegate.foundChatInformation.type.basic_group_id.toString()) {
                                        foundChatListItem.secondaryText.text = qsTr("%1 members").arg(Number(groupFullInfo.members.length).toLocaleString(Qt.locale(), "f", 0));
                                        foundChatListItem.tertiaryText.text = Emoji.emojify(groupFullInfo.description, foundChatListItem.tertiaryText.font.pixelSize, "../js/emoji/");
                                    }
                                }
                                onBasicGroupFullInfoReceived: {
                                    if (foundChatListDelegate.isBasicGroup && groupId.toString() === foundChatListDelegate.foundChatInformation.type.basic_group_id.toString()) {
                                        foundChatListItem.secondaryText.text = qsTr("%1 members").arg(Number(groupFullInfo.members.length).toLocaleString(Qt.locale(), "f", 0));
                                        foundChatListItem.tertiaryText.text = Emoji.emojify(groupFullInfo.description, foundChatListItem.tertiaryText.font.pixelSize, "../js/emoji/");
                                    }
                                }

                                onSupergroupFullInfoUpdated: {
                                    if (foundChatListDelegate.isSupergroup && groupId.toString() === foundChatListDelegate.foundChatInformation.type.supergroup_id.toString()) {
                                        if (foundChatListDelegate.relatedInformation.is_channel) {
                                            foundChatListItem.secondaryText.text = qsTr("%1 subscribers").arg(Number(groupFullInfo.member_count).toLocaleString(Qt.locale(), "f", 0));
                                        } else {
                                            foundChatListItem.secondaryText.text = qsTr("%1 members").arg(Number(groupFullInfo.member_count).toLocaleString(Qt.locale(), "f", 0));
                                        }
                                        foundChatListItem.tertiaryText.text = Emoji.emojify(groupFullInfo.description, foundChatListItem.tertiaryText.font.pixelSize, "../js/emoji/");
                                    }
                                }
                                onSupergroupFullInfoReceived: {
                                    if (foundChatListDelegate.isSupergroup && groupId.toString() === foundChatListDelegate.foundChatInformation.type.supergroup_id.toString()) {
                                        if (foundChatListDelegate.relatedInformation.is_channel) {
                                            foundChatListItem.secondaryText.text = qsTr("%1 subscribers").arg(Number(groupFullInfo.member_count).toLocaleString(Qt.locale(), "f", 0));
                                        } else {
                                            foundChatListItem.secondaryText.text = qsTr("%1 members").arg(Number(groupFullInfo.member_count).toLocaleString(Qt.locale(), "f", 0));
                                        }
                                        foundChatListItem.tertiaryText.text = Emoji.emojify(groupFullInfo.description, foundChatListItem.tertiaryText.font.pixelSize, "../js/emoji/");
                                    }
                                }
                            }

                            PhotoTextsListItem {
                                id: foundChatListItem

                                pictureThumbnail {
                                    photoData: typeof foundChatInformation.photo.small !== "undefined" ? foundChatInformation.photo.small : {}
                                }
                                width: parent.width

                                primaryText.text: Emoji.emojify(foundChatInformation.title, primaryText.font.pixelSize, "../js/emoji/")
                                tertiaryText.maximumLineCount: 1

                                onClicked: {
                                    pageStack.push(Qt.resolvedUrl("../pages/ChatPage.qml"), { "chatInformation" : foundChatInformation });
                                }
                            }
                        }

                        VerticalScrollDecorator {}
                    }

                }

                Column {

                    opacity: visible ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    visible: searchChatsPage.isLoading
                    width: parent.width
                    height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium

                    spacing: Theme.paddingMedium

                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: loadingLabel
                        text: qsTr("Searching chats...")
                    }

                    BusyIndicator {
                        id: loadingBusyIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: searchChatsPage.isLoading
                        size: BusyIndicatorSize.Large
                    }
                }

            }

        }
    }
}
