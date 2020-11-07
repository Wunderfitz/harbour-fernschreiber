import QtQuick 2.6
import Sailfish.Silica 1.0
import "../components"
import "../components/chatInformationPage"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: chatInformationPage

    allowedOrientations: Orientation.All
    property string searchString

    property int chatOnlineMemberCount: 0;
    property int myUserId: tdLibWrapper.getUserInformation().id;

    property bool isPrivateChat: false
    property bool isBasicGroup: false
    property bool isSuperGroup: false
    property bool isChannel: false

    property string chatPartnerGroupId

    property bool userIsMember: (isPrivateChat && chatInformation["@type"]) || // should be optimized
                                (isBasicGroup || isSuperGroup) && (
                                    (groupInformation.status["@type"] === "chatMemberStatusMember")
                                    || (groupInformation.status["@type"] === "chatMemberStatusAdministrator")
                                    || (groupInformation.status["@type"] === "chatMemberStatusRestricted" && groupInformation.status.is_member)
                                    || (groupInformation.status["@type"] === "chatMemberStatusCreator" && groupInformation.status.is_member)
                                    )

    property var chatInformation:({});
    property var privateChatUserInformation:({});
    property var chatPartnerFullInformation:({});
    property var chatPartnerProfilePhotos:([]);
    property var groupInformation: ({});
    property var groupFullInformation: ({});

    property alias membersList: membersList

    function initializePage() {
        console.log("[ChatInformationPage] Initializing chat info page...");
        membersList.clear();
        var chatType = chatInformation.type["@type"];
        switch(chatType) {
        case "chatTypePrivate":
            isPrivateChat = true;
            chatInformationPage.chatPartnerGroupId = chatInformation.type.user_id.toString();
            if(!privateChatUserInformation.id) {
                privateChatUserInformation = tdLibWrapper.getUserInformation(chatInformationPage.chatPartnerGroupId);
            }
            tdLibWrapper.getUserFullInfo(chatInformationPage.chatPartnerGroupId);
            tdLibWrapper.getUserProfilePhotos(chatInformationPage.chatPartnerGroupId, 100, 0);
            break;
        case "chatTypeBasicGroup":
            isBasicGroup = true;
            chatInformationPage.chatPartnerGroupId = chatInformation.type.basic_group_id.toString();
            if(!groupInformation.id) {
                groupInformation = tdLibWrapper.getBasicGroup(chatInformationPage.chatPartnerGroupId);
            }
            tdLibWrapper.getGroupFullInfo(chatInformationPage.chatPartnerGroupId, false);
            break;
        case "chatTypeSupergroup":
            isSuperGroup = true;
            chatInformationPage.chatPartnerGroupId = chatInformation.type.supergroup_id.toString();
            if(!groupInformation.id) {
                groupInformation = tdLibWrapper.getSuperGroup(chatInformationPage.chatPartnerGroupId);
            }

            tdLibWrapper.getGroupFullInfo(chatInformationPage.chatPartnerGroupId, true);
            isChannel = groupInformation.is_channel;
            break;
        }
        if(!isPrivateChat) {
            updateGroupStatusText();
        }
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
        groupFullInformation = groupFullInfo;
        membersList.clear();
        if(groupFullInfo.members && groupFullInfo.members.length > 0) {
            for(var memberIndex in groupFullInfo.members) {
                var memberData = groupFullInfo.members[memberIndex];
                var userInfo = tdLibWrapper.getUserInformation(memberData.user_id) || {user:{}, bot_info:{}};
                memberData.user = userInfo;
                memberData.bot_info = memberData.bot_info || {};
                membersList.append(memberData);
            }
            groupInformation.member_count = groupFullInfo.members.length
            updateGroupStatusText();
        }
    }

    Connections {
        target: tdLibWrapper

        onChatOnlineMemberCountUpdated: {
            if ((isSuperGroup || isBasicGroup) && chatInformation.id.toString() === chatId) {
                chatOnlineMemberCount = onlineMemberCount;
                updateGroupStatusText();
            }
        }
        onSupergroupFullInfoReceived: {
            if(isSuperGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                groupFullInformation = groupFullInfo;
            }
        }
        onSupergroupFullInfoUpdated: {
            if(isSuperGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                groupFullInformation = groupFullInfo;
            }
        }
        onBasicGroupFullInfoReceived: {
            if(isBasicGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                handleBasicGroupFullInfo(groupFullInfo)
            }
        }

        onBasicGroupFullInfoUpdated: {
            if(isBasicGroup && chatInformationPage.chatPartnerGroupId === groupId) {
                handleBasicGroupFullInfo(groupFullInfo)
            }
        }
        onUserFullInfoReceived: {
            if(isPrivateChat && userFullInfo["@extra"] === chatInformationPage.chatPartnerGroupId) {
                chatPartnerFullInformation = userFullInfo;
            }
        }
        onUserFullInfoUpdated: {
            if(isPrivateChat && userId === chatInformationPage.chatPartnerGroupId) {
                chatPartnerFullInformation = userFullInfo;
            }
        }

        onUserProfilePhotosReceived: {
            if(isPrivateChat && extra === chatInformationPage.chatPartnerGroupId) {
                chatPartnerProfilePhotos = photos;
            }
        }
        onChatPermissionsUpdated: {
            if (chatInformation.id.toString() === chatId) {
                // set whole object to trigger change
                var newInformation = chatInformation;
                newInformation.permissions = permissions
                chatInformation = newInformation
            }
        }
        onChatTitleUpdated: {
            if (chatInformation.id.toString() === chatId) {
                // set whole object to trigger change
                var newInformation = chatInformation;
                newInformation.title = title
                chatInformation = newInformation
            }
        }
    }
    Component.onCompleted: initializePage();

    ListModel {
        id: membersList
    }

    AppNotification {
        id: infoNotification
    }
    SilicaFlickable {
        id: pageFlickable
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                visible: (chatPage.isSuperGroup || chatPage.isBasicGroup) && groupInformation && groupInformation.status["@type"] !== "chatMemberStatusBanned"
                text: userIsMember ? qsTr("Leave Chat") : qsTr("Join Chat")
                onClicked: {
                    // ensure it's done even if the page is closed:
                    if (userIsMember) {
                        var remorse = Remorse.popupAction(appWindow, qsTr("Leaving chat"), (function(chatid) {
                            return function() {
                                tdLibWrapper.leaveChat(chatid);
                                // this does not care about the response (ideally type "ok" without further reference) for now
                                pageStack.pop(pageStack.find( function(page){ return(page._depth === 0)} ));
                            };
                        }(chatInformation.id)))
                    } else {
                        tdLibWrapper.joinChat(chatInformation.id);
                    }


                }
            }
            MenuItem {
                visible: userIsMember
                onClicked: {
                    var newNotificationSettings = chatInformation.notification_settings;
                    if (newNotificationSettings.mute_for > 0) {
                        newNotificationSettings.mute_for = 0;
                    } else {
                        newNotificationSettings.mute_for = 6666666;
                    }
                    tdLibWrapper.setChatNotificationSettings(chat_id, newNotificationSettings);
                }
                text: chatInformation.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat")
            }
            // MenuItem { //TODO Implement
            //     visible: !userIsMember
            //     onClicked: {
            //         tdLibWrapper.joinChat(chat_id);
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
                property bool hasImage: typeof chatInformation.photo !== "undefined"
                property int minDimension: chatInformationPage.isLandscape ? Theme.itemSizeSmall : Theme.itemSizeMedium
                property int maxDimension: Screen.width / 2
                property int minX: Theme.horizontalPageMargin
                property int maxX: (chatInformationPage.width - maxDimension)/2
                property int minY: (parent.height - minDimension)/2
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
                    photoData: imageContainer.hasImage ? chatInformation.photo.small : ""
                    replacementStringHint: headerItem.title
                    width: parent.width
                    height: width
                    radius: imageContainer.minDimension / 2
                    opacity: profilePictureLoader.status !== Loader.Ready || profilePictureLoader.item.opacity < 1 ? 1.0 : 0.0
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
            // PageHeader changes the html base path:
            property url emojiBase: "../js/emoji/"
            leftMargin: imageContainer.minDimension + Theme.horizontalPageMargin + Theme.paddingMedium
            title: chatInformation.title !== "" ? Emoji.emojify(chatInformation.title, Theme.fontSizeLarge, emojiBase) : qsTr("Unknown")
            description: chatInformationPage.isPrivateChat ? ("@"+(chatInformationPage.privateChatUserInformation.username || chatInformationPage.chatPartnerGroupId)) : ""
        }

        SilicaFlickable {
            id: contentFlickable
            contentHeight: groupInfoItem.height + tabViewLoader.height
            clip: true
            interactive: true//groupInfoItem.height > pageFlickable.height * 0.5

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
                    text: chatInformation.title

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
                        text: !isPrivateChat ? groupFullInformation.invite_link : ""
                        width: parent.width - inviteLinkButton.width
                    }
                    IconButton {
                        id: inviteLinkButton
                        icon.source: "image://theme/icon-m-clipboard"
                        anchors.verticalCenter: inviteLinkItem.verticalCenter
                        onClicked: {
                            Clipboard.text = groupFullInformation.invite_link
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

                    Behavior on opacity { PropertyAnimation {duration: 500}}
                }
            }

            Loader {
                id: tabViewLoader
                asynchronous: true
                active: true
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
    function updateGroupStatusText() {
        if (chatOnlineMemberCount > 0) {
            headerItem.description = qsTr("%1 members, %2 online").arg(Functions.getShortenedCount(groupInformation.member_count)).arg(Functions.getShortenedCount(chatOnlineMemberCount));
        } else {
            if (isChannel) {
                headerItem.description = qsTr("%1 subscribers").arg(Functions.getShortenedCount(groupInformation.member_count));
            } else {
                headerItem.description = qsTr("%1 members").arg(Functions.getShortenedCount(groupInformation.member_count));
            }
        }
    }

}
