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

Dialog {
    id: chatSelectionPage
    allowedOrientations: Orientation.All
    canAccept: false
    acceptDestinationAction: PageStackAction.Replace
    acceptDestinationReplaceTarget: pageStack.find( function(page){ return(page._depth === 0)} )
    property int myUserId: tdLibWrapper.getUserInformation().id;
    property alias headerTitle: pageHeader.title
    property alias headerDescription: pageHeader.description
    /*
        payload dependent on chatSelectionPage.state
         - forwardMessages: {fromChatId, messageIds, neededPermissions}
    */
    property var payload: ({})

    onAccepted: {
        switch(chatSelectionPage.state) {
        case "forwardMessages":
            acceptDestinationInstance.forwardMessages(payload.fromChatId, payload.messageIds)
            break;
        // future uses of chat selection can be processed here
        }
    }

    PageHeader {
                id: pageHeader
                title: qsTr("Select Chat")
                width: parent.width
            }

    SilicaListView {
        id: chatListView

        anchors {
            top: pageHeader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        clip: true

        model: chatListModel
        delegate: ChatListViewItem {
            ownUserId: chatSelectionPage.myUserId
            Loader { // checking permissions takes a while, so we defer those calculations
                id: visibleLoader
                asynchronous: true
                sourceComponent: Component {
                    QtObject {
                        property bool visible: false
                        Component.onCompleted: {
                            if(chatSelectionPage.state === "forwardMessages") {
                                var chatType = display.type['@type'];
                                var chatGroupInformation;
                                if(chatType === "chatTypePrivate" || chatType === "chatTypeSecret") {
                                    visible = true
                                    return;
                                }
                                else if (chatType === "chatTypeBasicGroup" ) {
                                    chatGroupInformation = tdLibWrapper.getBasicGroup(display.type.basic_group_id);
                                }
                                else if (chatType === "chatTypeSupergroup" ) {
                                    chatGroupInformation = tdLibWrapper.getSuperGroup(display.type.supergroup_id);
                                }
                                var groupStatus = chatGroupInformation.status;
                                var groupStatusType = groupStatus["@type"];
                                var groupStatusPermissions = groupStatus.permissions;
                                var groupPermissions = display.permissions;
                                visible = (groupStatusType === "chatMemberStatusCreator"
                                        || groupStatusType === "chatMemberStatusAdministrator"
                                        || (groupStatusType === "chatMemberStatusMember"
                                                && chatSelectionPage.payload.neededPermissions.every(function(neededPermission){
                                                    return groupPermissions[neededPermission];
                                                })
                                            )
                                        || (groupStatusType === "chatMemberStatusRestricted"
                                           && chatSelectionPage.payload.neededPermissions.every(function(neededPermission){
                                               return groupStatusPermissions[neededPermission];
                                           })
                                       )
                                        );
                            } else { // future uses of chat selection can be processed here
                                visible = true;
                            }
                        }
                    }
                }
            }

            property bool valid: visibleLoader && visibleLoader.item && visibleLoader.item.visible
            opacity: valid ? 1.0 : 0

            Behavior on opacity { FadeAnimation {}}
            Behavior on height { NumberAnimation {}}

            // normal height while calculating, otherwise all elements get displayed at once
            height: !visibleLoader.item || valid ? contentHeight : 0
            enabled: valid
            onClicked: {
                var chat = tdLibWrapper.getChat(display.id);
                switch(chatSelectionPage.state) {
                case "forwardMessages":
                    chatSelectionPage.acceptDestinationProperties = { "chatInformation" :  chat};
                    chatSelectionPage.acceptDestination = Qt.resolvedUrl("../pages/ChatPage.qml");
                    break;
                }
                chatSelectionPage.canAccept = true;
                chatSelectionPage.accept();
            }
        }

        ViewPlaceholder {
            enabled: chatListView.count === 0
            text: qsTr("You don't have any chats yet.")
        }

        VerticalScrollDecorator {}
    }


}
