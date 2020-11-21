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
import "../"
import "../../js/twemoji.js" as Emoji
import "../../js/functions.js" as Functions


SilicaFlickable {
    id: pageContent
    property alias membersList: membersList

    function initializePage() {
        membersList.clear();
        var chatType = chatInformation.type["@type"];
        switch(chatType) {
        case "chatTypePrivate":
            chatInformationPage.isPrivateChat = true;
            chatInformationPage.chatPartnerGroupId = chatInformationPage.chatInformation.type.user_id.toString();
            if(!chatInformationPage.privateChatUserInformation.id) {
                chatInformationPage.privateChatUserInformation = tdLibWrapper.getUserInformation(chatInformationPage.chatPartnerGroupId);
            }
            tdLibWrapper.getUserFullInfo(chatInformationPage.chatPartnerGroupId);
            tdLibWrapper.getUserProfilePhotos(chatInformationPage.chatPartnerGroupId, 100, 0);
            break;
        case "chatTypeBasicGroup":
            chatInformationPage.isBasicGroup = true;
            chatInformationPage.chatPartnerGroupId = chatInformation.type.basic_group_id.toString();
            if(!chatInformationPage.groupInformation.id) {
                chatInformationPage.groupInformation = tdLibWrapper.getBasicGroup(chatInformationPage.chatPartnerGroupId);
            }
            tdLibWrapper.getGroupFullInfo(chatInformationPage.chatPartnerGroupId, false);
            break;
        case "chatTypeSupergroup":
            chatInformationPage.isSuperGroup = true;
            chatInformationPage.chatPartnerGroupId = chatInformation.type.supergroup_id.toString();
            if(!chatInformationPage.groupInformation.id) {
                chatInformationPage.groupInformation = tdLibWrapper.getSuperGroup(chatInformationPage.chatPartnerGroupId);
            }

            tdLibWrapper.getGroupFullInfo(chatInformationPage.chatPartnerGroupId, true);
            chatInformationPage.isChannel = chatInformationPage.groupInformation.is_channel;
            break;
        }
        console.log("is set up", chatInformationPage.isPrivateChat, chatInformationPage.isBasicGroup, chatInformationPage.isSuperGroup, chatInformationPage.chatPartnerGroupId)
        if(!chatInformationPage.isPrivateChat) {
            updateGroupStatusText();
        }


        tabViewLoader.active = true;
    }
    function scrollUp(force) {
        if(force) {
            // animation does not always work while quick scrolling
            scrollUpTimer.start()
        } else {
            scrollUpAnimation.start()
        }
    }
    function scrollDown(force) {
        if(force) {
            scrollDownTimer.start()
        } else {
            scrollDownAnimation.start()
        }
    }
    function handleBasicGroupFullInfo(groupFullInfo) {
        chatInformationPage.groupFullInformation = groupFullInfo;
        membersList.clear();
        if(groupFullInfo.members && groupFullInfo.members.length > 0) {
            for(var memberIndex in groupFullInfo.members) {
                var memberData = groupFullInfo.members[memberIndex];
                var userInfo = tdLibWrapper.getUserInformation(memberData.user_id) || {user:{}, bot_info:{}};
                memberData.user = userInfo;
                memberData.bot_info = memberData.bot_info || {};
                membersList.append(memberData);
            }
            chatInformationPage.groupInformation.member_count = groupFullInfo.members.length
            updateGroupStatusText();
        }
    }
    function updateGroupStatusText() {
        if (chatInformationPage.chatOnlineMemberCount > 0) {
            headerItem.description = qsTr("%1, %2", "combination of '[x members], [y online]', which are separate translations")
                .arg(qsTr("%1 members", "", chatInformationPage.groupInformation.member_count)
                    .arg(Functions.getShortenedCount(chatInformationPage.groupInformation.member_count)))
                .arg(qsTr("%1 online", "", chatInformationPage.chatOnlineMemberCount)
                    .arg(Functions.getShortenedCount(chatInformationPage.chatOnlineMemberCount)));
        } else {
            if (isChannel) {
                headerItem.description = qsTr("%1 subscribers", "", chatInformationPage.groupInformation.member_count ).arg(Functions.getShortenedCount(chatInformationPage.groupInformation.member_count));
            } else {
                headerItem.description = qsTr("%1 members", "", chatInformationPage.groupInformation.member_count).arg(Functions.getShortenedCount(chatInformationPage.groupInformation.member_count));
            }
        }
    }

    Connections {
        target: tdLibWrapper

        onChatOnlineMemberCountUpdated: {
            if ((chatInformationPage.isSuperGroup || chatInformationPage.isBasicGroup) && chatInformationPage.chatInformation.id.toString() === chatId) {
                chatInformationPage.chatOnlineMemberCount = onlineMemberCount;
                updateGroupStatusText();
            }
        }
        onSupergroupFullInfoReceived: {
            console.log("onSupergroupFullInfoReceived", chatInformationPage.isSuperGroup, chatInformationPage.chatPartnerGroupId, groupId)
            if(chatInformationPage.isSuperGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                chatInformationPage.groupFullInformation = groupFullInfo;
            }
        }
        onSupergroupFullInfoUpdated: {
            console.log("onSupergroupFullInfoUpdated", chatInformationPage.isSuperGroup, chatInformationPage.chatPartnerGroupId, groupId)
            if(chatInformationPage.isSuperGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                chatInformationPage.groupFullInformation = groupFullInfo;
            }
        }
        onBasicGroupFullInfoReceived: {
            if(chatInformationPage.isBasicGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                handleBasicGroupFullInfo(groupFullInfo)
            }
        }

        onBasicGroupFullInfoUpdated: {
            if(chatInformationPage.isBasicGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                handleBasicGroupFullInfo(groupFullInfo)
            }
        }
        onUserFullInfoReceived: {
            if(chatInformationPage.isPrivateChat && userFullInfo["@extra"] === chatInformationPage.chatPartnerGroupId) {
                chatInformationPage.chatPartnerFullInformation = userFullInfo;
            }
        }
        onUserFullInfoUpdated: {
            if(chatInformationPage.isPrivateChat && userId === chatInformationPage.chatPartnerGroupId) {
                chatInformationPage.chatPartnerFullInformation = userFullInfo;
            }
        }

        onUserProfilePhotosReceived: {
            if(chatInformationPage.isPrivateChat && extra === chatInformationPage.chatPartnerGroupId) {
                chatInformationPage.chatPartnerProfilePhotos = photos;
            }
        }
        onChatPermissionsUpdated: {
            if (chatInformationPage.chatInformation.id.toString() === chatId) {
                // set whole object to trigger change
                var newInformation = chatInformation;
                newInformation.permissions = permissions
                chatInformationPage.chatInformation = newInformation
            }
        }
        onChatTitleUpdated: {
            if (chatInformationPage.chatInformation.id.toString() === chatId) {
                // set whole object to trigger change
                var newInformation = chatInformation;
                newInformation.title = title
                chatInformationPage.chatInformation = newInformation
            }
        }
    }



    Component.onCompleted: {7
        initializePage();
    }



    ListModel {
        id: membersList
    }

    AppNotification {
        id: infoNotification
    }
    PullDownMenu {
        MenuItem {
            visible: (chatInformationPage.isSuperGroup || chatInformationPage.isBasicGroup) && chatInformationPage.groupInformation && chatInformationPage.groupInformation.status["@type"] !== "chatMemberStatusBanned"
            text: chatInformationPage.userIsMember ? qsTr("Leave Chat") : qsTr("Join Chat")
            onClicked: {
                // ensure it's done even if the page is closed:
                if (chatInformationPage.userIsMember) {
                    var remorse = Remorse.popupAction(appWindow, qsTr("Leaving chat"), (function(chatid) {
                        return function() {
                            tdLibWrapper.leaveChat(chatid);
                            // this does not care about the response (ideally type "ok" without further reference) for now
                            pageStack.pop(pageStack.find( function(page){ return(page._depth === 0)} ));
                        };
                    }(chatInformationPage.chatInformation.id)))
                } else {
                    tdLibWrapper.joinChat(chatInformationPage.chatInformation.id);
                }


            }
        }
        MenuItem {
            visible: chatInformationPage.userIsMember
            onClicked: {
                var newNotificationSettings = chatInformationPage.chatInformation.notification_settings;
                if (newNotificationSettings.mute_for > 0) {
                    newNotificationSettings.mute_for = 0;
                } else {
                    newNotificationSettings.mute_for = 6666666;
                }
                newNotificationSettings.use_default_mute_for = false;
                tdLibWrapper.setChatNotificationSettings(chatInformationPage.chatInformation.id, newNotificationSettings);
            }
            text: chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat")
        }
        // MenuItem { //TODO Implement
        //     visible: !userIsMember
        //     onClicked: {
        //         tdLibWrapper.joinChat(chatInformationPage.chatInformation.id);
        //     }
        //     text: qsTr("Join Chat")
        // }
    }
    // header
    PageHeader {
        id: headerItem
        z: 5
        Item {
            id: imageContainer
            property bool hasImage: typeof chatInformationPage.chatInformation.photo !== "undefined"
            property int minDimension: chatInformationPage.isLandscape ? Theme.itemSizeSmall : Theme.itemSizeMedium
            property int maxDimension: Screen.width / 2
            property int minX: Theme.horizontalPageMargin
            property int maxX: (chatInformationPage.width - maxDimension)/2
            property int minY: Theme.paddingSmall//(parent.height - minDimension)/2
            property int maxY: parent.height
            property double tweenFactor: {
                if(!hasImage) {
                    return 0
                }
                return 1 - Math.max(0, Math.min(1, contentFlickable.contentY / maxDimension))
            }

            function getEased(min,max,factor) {
                return min + (max-min)*factor
            }
            width: getEased(minDimension,maxDimension, tweenFactor)
            height: width
            x: getEased(minX,maxX, tweenFactor)
            y: getEased(minY,maxY, tweenFactor)

            ProfileThumbnail {
                id: chatPictureThumbnail
                photoData: imageContainer.hasImage ? chatInformationPage.chatInformation.photo.small : ""
                replacementStringHint: headerItem.title
                width: parent.width
                height: width
                radius: imageContainer.minDimension / 2
                opacity: profilePictureLoader.status !== Loader.Ready || profilePictureLoader.item.opacity < 1 ? 1.0 : 0.0
                optimizeImageSize: false
            }
            Loader {
                id: profilePictureLoader
                active: imageContainer.hasImage
                asynchronous: true
                anchors.fill: chatPictureThumbnail
                source: chatInformationPage.isPrivateChat
                        ? "../components/chatInformationPage/ChatInformationProfilePictureList.qml"
                        : "../components/chatInformationPage/ChatInformationProfilePicture.qml"
            }
        }
        leftMargin: imageContainer.getEased((imageContainer.minDimension + Theme.paddingMedium), 0, imageContainer.tweenFactor) + Theme.horizontalPageMargin
        title: chatInformationPage.chatInformation.title !== "" ? Emoji.emojify(chatInformationPage.chatInformation.title, Theme.fontSizeLarge) : qsTr("Unknown")
        description: chatInformationPage.isPrivateChat ? ("@"+(chatInformationPage.privateChatUserInformation.username || chatInformationPage.chatPartnerGroupId)) : ""
    }

    SilicaFlickable {
        id: contentFlickable
        contentHeight: groupInfoItem.height + tabViewLoader.height
        clip: true
        interactive: !scrollUpAnimation.running && !scrollDownAnimation.running

        anchors {
            top: headerItem.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        NumberAnimation {
            id: scrollDownAnimation
            target: contentFlickable
            to: groupInfoItem.height
            property: "contentY"
            duration: 500
            easing.type: Easing.InOutCubic
        }
        NumberAnimation {
            id: scrollUpAnimation
            target: contentFlickable
            to: 0
            property: "contentY"
            duration: 500
            easing.type: Easing.InOutCubic
            property Timer scrollUpTimer: Timer {
                id: scrollUpTimer
                interval: 50
                onTriggered: {
                    contentFlickable.scrollToTop()
                }
            }
            property Timer scrollDownTimer: Timer {
                id: scrollDownTimer
                interval: 50
                onTriggered: {
                    contentFlickable.scrollToBottom()
                }
            }
        }

        Column {
            id: groupInfoItem
            bottomPadding: Theme.paddingLarge
            topPadding: Theme.paddingLarge
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
                rightMargin: Theme.horizontalPageMargin
            }

            Item { //large image placeholder
                width: parent.width
                height: imageContainer.hasImage ? imageContainer.maxDimension : 0
            }

            ChatInformationEditArea {
                visible: canEdit
                canEdit: !chatInformationPage.isPrivateChat && chatInformationPage.groupInformation.status && (chatInformationPage.groupInformation.status.can_change_info  || chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusCreator")
                headerText: qsTr("Chat Title", "group title header")
                text: chatInformationPage.chatInformation.title

                onSaveButtonClicked: {
                    if(!editItem.errorHighlight) {
                        tdLibWrapper.setChatTitle(chatInformationPage.chatInformation.id, textValue);
                    } else {
                        isEditing = true
                    }
                }

                onTextEdited: {
                    if(textValue.length > 0 && textValue.length < 129) {
                        editItem.errorHighlight = false
                        editItem.label = ""
                        editItem.placeholderText = ""
                    } else {
                        editItem.label = qsTr("Enter 1-128 characters")
                        editItem.placeholderText = editItem.label
                        editItem.errorHighlight = true
                    }
                }
            }
            ChatInformationEditArea {
                canEdit: (chatInformationPage.isPrivateChat && chatInformationPage.privateChatUserInformation.id === chatInformationPage.myUserId) || ((chatInformationPage.isBasicGroup || chatInformationPage.isSuperGroup) && chatInformationPage.groupInformation && (chatInformationPage.groupInformation.status.can_change_info || chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusCreator"))
                emptyPlaceholderText: qsTr("There is no information text available, yet.")
                headerText: qsTr("Info", "group or user infotext header")
                multiLine: true
                text: (chatInformationPage.isPrivateChat ? chatInformationPage.chatPartnerFullInformation.bio : chatInformationPage.groupFullInformation.description) || ""
                onSaveButtonClicked: {
                    if(chatInformationPage.isPrivateChat) { // own bio
                        tdLibWrapper.setBio(textValue);
                    } else { // group info
                        tdLibWrapper.setChatDescription(chatInformationPage.chatInformation.id, textValue);
                    }
                }
            }

            ChatInformationTextItem {
                headerText: qsTr("Phone Number", "user phone number header")
                text: (chatInformationPage.isPrivateChat && chatInformationPage.privateChatUserInformation.phone_number ? "+"+chatInformationPage.privateChatUserInformation.phone_number : "") || ""
                isLinkedLabel: true
            }

            SectionHeader {
                font.pixelSize: Theme.fontSizeExtraSmall
                visible: !!inviteLinkItem.text
                height: visible ? Theme.itemSizeExtraSmall : 0
                text: qsTr("Invite Link", "header")
                x: 0
            }

            Row {
                width: parent.width
                visible: !!inviteLinkItem.text
                ChatInformationTextItem {
                    id: inviteLinkItem
                    text: !isPrivateChat ? chatInformationPage.groupFullInformation.invite_link : ""
                    width: parent.width - inviteLinkButton.width
                }
                IconButton {
                    id: inviteLinkButton
                    icon.source: "image://theme/icon-m-clipboard"
                    anchors.verticalCenter: inviteLinkItem.verticalCenter
                    onClicked: {
                        Clipboard.text = chatInformationPage.groupFullInformation.invite_link
                        infoNotification.show(qsTr("The Invite Link has been copied to the clipboard."));
                    }
                }
            }

            Item {
                width: parent.width
                height: Theme.paddingLarge
            }

            Separator {
                width: parent.width
                color: Theme.primaryColor
                horizontalAlignment: Qt.AlignHCenter
                opacity: (tabViewLoader.status === Loader.Ready && tabViewLoader.item.count > 0) ? 1.0 : 0.0

                Behavior on opacity { FadeAnimation {}}
            }
        }

        Loader {
            id: tabViewLoader
            asynchronous: true
            active: false
            anchors {
                left: parent.left
                right: parent.right
                top: groupInfoItem.bottom
            }
            sourceComponent: Component {
                ChatInformationTabView {
                    id: tabView
                    height: tabView.count > 0 ? chatInformationPage.height - headerItem.height : 0
                }
            }
        }
    }
}
