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
    loadingText:  (isPrivateChat || isSecretChat) ? qsTr("Loading common chats…", "chats you have in common with a user") : qsTr("Loading group members…")
    loading: ( chatInformationPage.isSuperGroup || chatInformationPage.isPrivateChat || chatInformationPage.isSecretChat) && !chatInformationPage.isChannel
    loadingVisible: loading && membersView.count === 0

    property var chatPartnerCommonGroupsIds: ([]);

    SilicaListView {
        id: membersView
        model: (chatInformationPage.isPrivateChat || chatInformationPage.isSecretChat) ? (chatPartnerCommonGroupsIds.length > 0 ? delegateModel : null) : pageContent.membersList
        clip: true
        height: tabBase.height
        width: tabBase.width
        opacity: tabBase.loading ? (count > 0 ? 0.5 : 0.0) : 1.0
        Behavior on opacity { FadeAnimation {} }
        move: Transition {
            NumberAnimation { properties: "y"; duration: 200; easing.type: Easing.InOutCubic }
        }
        moveDisplaced: Transition {
            NumberAnimation { properties: "y"; duration: 200; easing.type: Easing.InOutCubic }
        }
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
            if(tabBase.active && !tabBase.loading && chatInformationPage.isSuperGroup && !chatInformationPage.isChannel && (chatInformationPage.groupInformation.member_count > membersView.count) && membersView.atYEnd) {
                tabBase.loading = true;
                fetchMoreMembersTimer.start()
            }
        }
        onQuickScrollAnimatingChanged: {
            handleScrollIntoView(true)
        }
        ViewPlaceholder {
            y: Theme.paddingLarge
            enabled: membersView.count === 0
            text: (chatInformationPage.isPrivateChat || chatInformationPage.isSecretChat) ? qsTr("You don't have any groups in common with this user.") : ( chatInformationPage.isChannel ? qsTr("Channel members are anonymous.") : qsTr("This group is empty.") )
        }
        delegate: PhotoTextsListItem {
            id: memberListItem
            pictureThumbnail {
                photoData: user.profile_photo ? user.profile_photo.small : null
            }
            width: parent.width

            // Admin actions on other members: only offered if we can restrict
            // members and the target is a regular/restricted member (admins and
            // the creator can't be managed this way). Newer TDLib nests the
            // admin rights in status.rights, older TDLib has them flat.
            readonly property bool ownUserCanRestrictMembers: (chatInformationPage.isSuperGroup || chatInformationPage.isBasicGroup)
                                                              && chatInformationPage.groupInformation.status
                                                              && ( chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusCreator"
                                                                  || ( chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusAdministrator"
                                                                      && ( chatInformationPage.groupInformation.status.rights
                                                                          ? chatInformationPage.groupInformation.status.rights.can_restrict_members
                                                                          : chatInformationPage.groupInformation.status.can_restrict_members ) ) )
            readonly property bool memberIsManageable: member_id.user_id !== chatInformationPage.myUserId
                                                       && ( model.status["@type"] === "chatMemberStatusMember"
                                                           || model.status["@type"] === "chatMemberStatusRestricted" )

            menu: (ownUserCanRestrictMembers && memberIsManageable) ? memberContextMenu : null

            Component {
                id: memberContextMenu
                ContextMenu {
                    MenuItem {
                        visible: chatInformationPage.isSuperGroup
                        text: qsTr("Member Permissions", "edit a group member's individual permissions")
                        onClicked: {
                            var setIndex = index;
                            var dialog = pageStack.push(Qt.resolvedUrl("../../pages/ChatMemberPermissionsPage.qml"), {
                                chatId: chatInformationPage.chatInformation.id,
                                memberUserId: member_id.user_id,
                                userName: Functions.getUserName(user),
                                memberStatus: model.status,
                                defaultPermissions: chatInformationPage.chatInformation.permissions
                            });
                            dialog.accepted.connect(function() {
                                if (dialog.resultStatus) {
                                    pageContent.membersList.set(setIndex, { status: dialog.resultStatus });
                                }
                            });
                        }
                    }
                    MenuItem {
                        visible: chatInformationPage.isSuperGroup && model.status["@type"] === "chatMemberStatusMember"
                        text: qsTr("Revoke Write Permission", "restrict a group member")
                        onClicked: {
                            // All chatPermissions fields default to false, so an
                            // empty object mutes the member regardless of the
                            // TDLib generation (old flat vs. new granular fields).
                            var newStatus = {
                                "@type": "chatMemberStatusRestricted",
                                is_member: true,
                                restricted_until_date: 0,
                                permissions: { "@type": "chatPermissions" }
                            };
                            tdLibWrapper.setChatMemberStatus(chatInformationPage.chatInformation.id, member_id.user_id, newStatus);
                            pageContent.membersList.set(index, { status: newStatus });
                        }
                    }
                    MenuItem {
                        visible: chatInformationPage.isSuperGroup && model.status["@type"] === "chatMemberStatusRestricted"
                        text: qsTr("Remove Restrictions", "lift restrictions from a group member")
                        onClicked: {
                            var newStatus = { "@type": "chatMemberStatusMember" };
                            tdLibWrapper.setChatMemberStatus(chatInformationPage.chatInformation.id, member_id.user_id, newStatus);
                            pageContent.membersList.set(index, { status: newStatus });
                        }
                    }
                    MenuItem {
                        text: qsTr("Remove from Group", "ban a group member")
                        onClicked: {
                            var chatId = chatInformationPage.chatInformation.id;
                            var userId = member_id.user_id;
                            memberListItem.remorseAction(qsTr("Removing member", "remorse timer text"), function() {
                                tdLibWrapper.banChatMember(chatId, userId, 0, false);
                                pageContent.membersList.remove(index);
                            });
                        }
                    }
                    MenuItem {
                        text: qsTr("Remove and Delete All Messages", "ban a group member, revoking their messages")
                        onClicked: {
                            var chatId = chatInformationPage.chatInformation.id;
                            var userId = member_id.user_id;
                            memberListItem.remorseAction(qsTr("Removing member and deleting messages", "remorse timer text"), function() {
                                tdLibWrapper.banChatMember(chatId, userId, 0, true);
                                pageContent.membersList.remove(index);
                            });
                        }
                    }
                }
            }

            // chat title
            primaryText.text: Emoji.emojify(Functions.getUserName(user), primaryText.font.pixelSize)
            // last user
            prologSecondaryText.text: "@"+(user.username ? user.username : member_id.user_id) + (member_id.user_id === chatInformationPage.myUserId ? " " + qsTr("You") : "")
            secondaryText {
                horizontalAlignment: Text.AlignRight
                property string statusText: Functions.getChatMemberStatusText(model.status["@type"])
                property string customText: model.status.custom_title ? Emoji.emojify(model.status.custom_title, secondaryText.font.pixelSize) : ""
                text: (statusText !== "" && customText !== "") ? statusText + ", " + customText : statusText + customText
            }
            tertiaryText {
                maximumLineCount: 1
                text: user.type["@type"] === "userTypeBot" ? (Emoji.emojify("🤖 "+bot_info.description, tertiaryText.font.pixelSize)) : Functions.getChatPartnerStatusText(user.status["@type"], user.status.was_online);
            }

            onClicked: {
                tdLibWrapper.createPrivateChat(member_id.user_id, "openDirectly");
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
        }
    }

    Timer {
        id: fetchMoreMembersTimer
        interval: 600
        property int fetchLimit: 50
        onTriggered: {
            if(chatInformationPage.isSuperGroup && (!chatInformationPage.isChannel || chatInformationPage.canGetMembers) && (chatInformationPage.groupInformation.member_count > membersView.count)) {
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
                        var userInfo = tdLibWrapper.getUserInformation(memberData.member_id.user_id) || {user:{}, bot_info:{}};
                        if (!userInfo.username && userInfo.usernames && userInfo.usernames.active_usernames) {
                            userInfo.username = userInfo.usernames.active_usernames[0]
                        }
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
            if((isPrivateChat || isSecretChat) && chats["@extra"] === chatInformationPage.chatPartnerGroupId) {
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
        if(chatInformationPage.isPrivateChat || chatInformationPage.isSecretChat) {
            tdLibWrapper.getGroupsInCommon(chatInformationPage.chatPartnerGroupId, 200, 0); // we only use the first 200
        } else if(chatInformationPage.isSuperGroup) {
            fetchMoreMembersTimer.start();
        }
    }

}
