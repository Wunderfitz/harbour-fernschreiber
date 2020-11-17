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
import QtQml.Models 2.3

import "../"
import "../../pages"
import "../../js/twemoji.js" as Emoji
import "../../js/functions.js" as Functions

ChatInformationTabItemBase {
    id: tabBase
    loadingText:  isPrivateChat ? qsTr("Loading common chats…", "chats you have in common with a user") : qsTr("Loading group members…")
    loading: ( chatInformationPage.isSuperGroup || chatInformationPage.isPrivateChat) && !chatInformationPage.isChannel
    loadingVisible: loading && membersView.count === 0

    property var chatPartnerCommonGroupsIds: ([]);

    SilicaListView {
        id: membersView
        model: chatInformationPage.isPrivateChat ? (chatPartnerCommonGroupsIds.length > 0 ? delegateModel : null) : pageContent.membersList
        clip: true
        height: tabBase.height
        width: tabBase.width
        opacity: tabBase.loading ? (count > 0 ? 0.5 : 0.0) : 1.0
        Behavior on opacity { FadeAnimation {} }
        function handleScrollIntoView(force){
            if(!tabBase.loading && !dragging && !quickScrollAnimating ) {
                if(!atYBeginning) {
                    pageContent.scrollDown()
                } else {
                    pageContent.scrollUp(force);
                }
            }
        }
        onDraggingChanged: {
            handleScrollIntoView()
        }
        onAtYBeginningChanged: {
            handleScrollIntoView()
        }
        onAtYEndChanged: {
            if(tabBase.active && !tabBase.loading && chatInformationPage.isSuperGroup && (chatInformationPage.groupInformation.member_count > membersView.count) && membersView.atYEnd) {
                tabBase.loading = true;
                console.log("LOAD MEMBERS BECAUSE ATYEND")
                fetchMoreMembersTimer.start()
            }
        }
        onQuickScrollAnimatingChanged: {
            handleScrollIntoView(true)
        }
        ViewPlaceholder {
            y: Theme.paddingLarge
            enabled: membersView.count === 0
            text: chatInformationPage.isPrivateChat ? qsTr("You don't have any groups in common with this user.") : ( chatInformationPage.isChannel ? qsTr("Channel members are anonymous.") : qsTr("This group is empty.") )
        }
        delegate: PhotoTextsListItem {
            pictureThumbnail {
                photoData: (typeof user.profile_photo !== "undefined") ? user.profile_photo.small : ""
            }
            width: parent.width

            // chat title
            primaryText.text: Emoji.emojify(Functions.getUserName(user), primaryText.font.pixelSize, "../js/emoji/")
            // last user
            prologSecondaryText.text: "@"+(user.username !== "" ? user.username : user_id) + (user_id === chatInformationPage.myUserId ? " " + qsTr("You") : "")
            secondaryText {
                horizontalAlignment: Text.AlignRight
                property string statusText: Functions.getChatMemberStatusText(model.status["@type"])
                property string customText: model.status.custom_title ? Emoji.emojify(model.status.custom_title, secondaryText.font.pixelSize, "../js/emoji/") : ""
                text: (statusText !== "" && customText !== "") ? statusText + ", " + customText : statusText + customText
            }
            tertiaryText {
                maximumLineCount: 1
                text: user.type["@type"] === "userTypeBot" ? (Emoji.emojify("🤖 "+bot_info.description, tertiaryText.font.pixelSize, "../js/emoji/")) : Functions.getChatPartnerStatusText(user.status["@type"], user.status.was_online);
                elide: Text.ElideRight
            }

            onClicked: {
                tdLibWrapper.createPrivateChat(user_id);
            }
        }
        footer: Component {
            Item {
                property bool active: tabBase.active && chatInformationPage.isSuperGroup && (chatInformationPage.groupInformation.member_count > membersView.count)
                width: tabBase.width
                height: active ? Theme.itemSizeLarge : Theme.paddingMedium

                BusyIndicator {
                    id: loadMoreIndicator
                    anchors.centerIn: parent
                    size: BusyIndicatorSize.Small
                    running: tabBase.loading
                }
            }
        }

        VerticalScrollDecorator {}
    }


    DelegateModel {
        id: delegateModel
        model: chatListModel
        groups: [
            DelegateModelGroup {
                name: "filterGroup"; includeByDefault: true
            }
        ]
        filterOnGroup: "filterGroup"
        function hasMatch(searchInArray) {
            for (var i = 0; i < searchInArray.length; i++) {
                if(searchInArray[i].toLowerCase().indexOf(chatInformationPage.searchString) > -1) {
                    return true;
                }
            }
            return false;
        }

        function applyFilter(){
            var numberOfEntries = chatListModel.rowCount();
            var hasFilterString = !!chatInformationPage.searchString && chatInformationPage.searchString !== ""
            for (var i = 0; i < numberOfEntries; i++){
                var metadata = chatListModel.get(i);
                if(tabBase.chatPartnerCommonGroupsIds.indexOf(metadata.chat_id) > -1) {
                    items.addGroups(i, 1, "filterGroup");
                } else {
                    items.removeGroups(i, 1, "filterGroup");
                }

            }
        }

        delegate: ChatListViewItem {
            ownUserId: chatInformationPage.myUserId

            unreadCount: unread_count
            onClicked: {
                pageStack.pop(pageStack.find( function(page){ return(page._depth === 0)} ), PageStackAction.Immediate);
                pageStack.push(Qt.resolvedUrl("../../pages/ChatPage.qml"), { "chatInformation" : display });
            }
            Connections {
                target: chatListModel
                onChatChanged: {
                    if (changedChatId === chat_id) {
                        // Force update of some list item elements (currently only last message text seems to create problems). dataChanged() doesn't seem to trigger them all :(
                        secondaryText.text = last_message_text ? Emoji.emojify(last_message_text, Theme.fontSizeExtraSmall, "../../js/emoji/") : qsTr("Unknown")
                    }
                }
            }
        }
    }

    Timer {
        id: fetchMoreMembersTimer
        interval: 600
        property int fetchLimit: 50
        onTriggered: {
            if(chatInformationPage.isSuperGroup && !chatInformationPage.isChannel && (chatInformationPage.groupInformation.member_count > membersView.count)) { //
                tabBase.loading = true
                tdLibWrapper.getSupergroupMembers(chatInformationPage.chatPartnerGroupId, fetchLimit, pageContent.membersList.count);
                fetchLimit = 200
                interval = 400
            }
        }
    }

    Connections {
        target: tdLibWrapper

        onChatMembersReceived: {
            if (chatInformationPage.isSuperGroup && chatInformationPage.chatPartnerGroupId === extra) {
                if(members && members.length > 0 && chatInformationPage.groupInformation.member_count > membersView.count) {
                    for(var memberIndex in members) {
                        var memberData = members[memberIndex];
                        var userInfo = tdLibWrapper.getUserInformation(memberData.user_id) || {user:{}, bot_info:{}};
                        memberData.user = userInfo;
                        memberData.bot_info = memberData.bot_info || {};
                        pageContent.membersList.append(memberData);
                    }
                    chatInformationPage.groupInformation.member_count = totalMembers
                    updateGroupStatusText();
//                    if(pageContent.membersList.count < totalMembers) {
//                        fetchMoreMembersTimer.start()
//                    }
                }
                // if we set it directly, the views start scrolling
                loadedTimer.start();
            }
        }
        onChatsReceived: {// common chats with user
            if(isPrivateChat && chats["@extra"] === chatInformationPage.chatPartnerGroupId) {
                tabBase.chatPartnerCommonGroupsIds = chats.chat_ids;
                delegateModel.applyFilter();
                // if we set it directly, the views start scrolling
                loadedTimer.start();
            }
        }
    }
    Timer {
        id: loadedTimer
        interval: 50
        onTriggered: {
            tabBase.loading = false
        }
    }

    Component.onCompleted: {
        if(chatInformationPage.isPrivateChat) {
            tdLibWrapper.getGroupsInCommon(chatInformationPage.chatPartnerGroupId, 200, 0); // we only use the first 200
        } else if(chatInformationPage.isSuperGroup) {
            fetchMoreMembersTimer.start();
        }
    }

}
