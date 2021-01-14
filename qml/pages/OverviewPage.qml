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
import Nemo.Notifications 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: overviewPage
    allowedOrientations: Orientation.All

    property bool initializationCompleted: false;
    property bool loading: true;
    property bool logoutLoading: false;
    property int authorizationState: TelegramAPI.Closed
    property int connectionState: TelegramAPI.WaitingForNetwork
    property int ownUserId;
    property bool chatListCreated: false;

    onStatusChanged: {
        if (status === PageStatus.Active && initializationCompleted && !chatListCreated && !logoutLoading) {
            updateContent();
        }
    }

    Connections {
        target: dBusAdaptor
        onPleaseOpenMessage: {
            Debug.log("[OverviewPage] Opening chat from external call...")
            if (chatListCreated) {
                pageStack.pop(overviewPage, PageStackAction.Immediate)
                pageStack.push(Qt.resolvedUrl("../pages/ChatPage.qml"), { "chatInformation" : chatListModel.getById(chatId) }, PageStackAction.Immediate)
            }
        }
        onPleaseOpenUrl: {
            Debug.log("[OverviewPage] Opening URL requested: ", url);
            Functions.handleLink(url);
        }
    }

    Timer {
        id: chatListCreatedTimer
        interval: 100
        running: false
        repeat: false
        onTriggered: {
            overviewPage.chatListCreated = true;
            chatListView.scrollToTop();
            updateSecondaryContentTimer.start();
            var remainingInteractionHints = appSettings.remainingInteractionHints;
            Debug.log("Remaining interaction hints: " + remainingInteractionHints);
            if (remainingInteractionHints > 0) {
                interactionHintTimer.start();
                titleInteractionHint.opacity = 1.0;
                appSettings.remainingInteractionHints = remainingInteractionHints - 1;
            }
        }
    }

    Timer {
        id: openInitializationPageTimer
        interval: 0
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
        }
    }
    Timer {
        id: updateSecondaryContentTimer
        interval: 600
        onTriggered: {
            chatListModel.calculateUnreadState();
            tdLibWrapper.getRecentStickers();
            tdLibWrapper.getInstalledStickerSets();
            tdLibWrapper.getContacts();
        }
    }

    Timer {
        id: searchChatTimer
        interval: 300
        running: false
        repeat: false
        onTriggered: {
            if (chatSearchField.text === "") {
                chatListView.model = chatListModel;
            } else {
                chatListView.model = chatListProxyModel;
            }
            chatListProxyModel.setFilterWildcard("*" + chatSearchField.text + "*");
        }
    }

    function setPageStatus() {
        switch (overviewPage.connectionState) {
        case TelegramAPI.WaitingForNetwork:
            pageStatus.color = "red";
            pageHeader.title = qsTr("Waiting for network...");
            break;
        case TelegramAPI.Connecting:
            pageStatus.color = "gold";
            pageHeader.title = qsTr("Connecting to network...");
            break;
        case TelegramAPI.ConnectingToProxy:
            pageStatus.color = "gold";
            pageHeader.title = qsTr("Connecting to proxy...");
            break;
        case TelegramAPI.ConnectionReady:
            pageStatus.color = "green";
            pageHeader.title = qsTr("Fernschreiber");
            break;
        case TelegramAPI.Updating:
            pageStatus.color = "lightblue";
            pageHeader.title = qsTr("Updating content...");
            break;
        }
    }

    function updateContent() {
        tdLibWrapper.getChats();
    }

    function initializePage() {
        overviewPage.authorizationState = tdLibWrapper.getAuthorizationState();
        overviewPage.handleAuthorizationState(true);
        overviewPage.connectionState = tdLibWrapper.getConnectionState();
        overviewPage.setPageStatus();
    }

    function handleAuthorizationState(isOnInitialization) {
        switch (overviewPage.authorizationState) {
        case TelegramAPI.WaitPhoneNumber:
        case TelegramAPI.WaitCode:
        case TelegramAPI.WaitPassword:
        case TelegramAPI.WaitRegistration:
        case TelegramAPI.AuthorizationStateClosed:
            overviewPage.loading = false;
            overviewPage.logoutLoading = false;
            if(isOnInitialization) { // pageStack isn't ready on Component.onCompleted
                openInitializationPageTimer.start()
            } else {
                pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
            }
            break;
        case TelegramAPI.AuthorizationReady:
            loadingBusyIndicator.text = qsTr("Loading chat list...");
            overviewPage.loading = false;
            overviewPage.initializationCompleted = true;
            overviewPage.updateContent();
            break;
        case TelegramAPI.AuthorizationStateLoggingOut:
            if (logoutLoading) {
                Debug.log("Resources cleared already");
                return;
            }
            Debug.log("Logging out")
            overviewPage.initializationCompleted = false;
            overviewPage.loading = false;
            chatListCreatedTimer.stop();
            updateSecondaryContentTimer.stop();
            loadingBusyIndicator.text = qsTr("Logging out")
            overviewPage.logoutLoading = true;
            chatListModel.reset();
            break;
        default:
            // Nothing ;)
        }
    }

    function resetFocus() {
        if (chatSearchField.text === "") {
            chatSearchField.opacity = 0.0;
            pageHeader.opacity = 1.0;
        }
        chatSearchField.focus = false;
        overviewPage.focus = true;
    }

    Connections {
        target: tdLibWrapper
        onAuthorizationStateChanged: {
            overviewPage.authorizationState = authorizationState;
            handleAuthorizationState();
        }
        onConnectionStateChanged: {
            overviewPage.connectionState = connectionState;
            setPageStatus();
        }
        onOwnUserIdFound: {
            overviewPage.ownUserId = ownUserId;
        }
        onChatLastMessageUpdated: {
            if (!overviewPage.chatListCreated) {
                chatListCreatedTimer.restart();
            } else {
                chatListModel.calculateUnreadState();
            }
        }
        onChatOrderUpdated: {
            if (!overviewPage.chatListCreated) {
                chatListCreatedTimer.restart();
            } else {
                chatListModel.calculateUnreadState();
            }
        }
        onChatsReceived: {
            if(chats && chats.chat_ids && chats.chat_ids.length === 0) {
                chatListCreatedTimer.restart();
            }
        }
        onChatReceived: {
            if(chat["@extra"] === "openDirectly") {
                pageStack.pop(overviewPage, PageStackAction.Immediate)
                // if we get a new chat (no messages?), we can not use the provided data
                pageStack.push(Qt.resolvedUrl("../pages/ChatPage.qml"), { "chatInformation" : tdLibWrapper.getChat(chat.id) });
            }
        }
        onErrorReceived: {
            Functions.handleErrorMessage(code, message);
        }
        onCopyToDownloadsSuccessful: {
            appNotification.show(qsTr("Download of %1 successful.").arg(fileName), filePath);
        }

        onCopyToDownloadsError: {
            appNotification.show(qsTr("Download failed."));
        }
    }

    Component.onCompleted: {
        initializePage();
    }

    SilicaFlickable {
        id: overviewContainer
        contentHeight: parent.height
        contentWidth: parent.width
        anchors.fill: parent
        visible: !overviewPage.loading

        PullDownMenu {
            MenuItem {
                text: "Debug"
                visible: Debug.enabled
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/DebugPage.qml"))
            }
            MenuItem {
                text: qsTr("About Fernschreiber")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/AboutPage.qml"), {isLoggedIn : (overviewPage.authorizationState == TelegramAPI.AuthorizationReady)})
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/SettingsPage.qml"))
            }
            MenuItem {
                text: qsTr("Search Chats")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/SearchChatsPage.qml"))
            }
            MenuItem {
                text: qsTr("New Chat")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/NewChatPage.qml"))
            }
        }

        PageHeader {
            id: pageHeader
            title: qsTr("Fernschreiber")
            leftMargin: Theme.itemSizeMedium
            visible: opacity > 0
            Behavior on opacity { FadeAnimation {} }

            GlassItem {
                id: pageStatus
                width: Theme.itemSizeMedium
                height: Theme.itemSizeMedium
                color: "red"
                falloffRadius: 0.1
                radius: 0.2
                cache: false
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    chatSearchField.focus = true;
                    chatSearchField.opacity = 1.0;
                    pageHeader.opacity = 0.0;
                }
            }

        }

        SearchField {
            id: chatSearchField
            visible: opacity > 0
            opacity: 0
            Behavior on opacity { FadeAnimation {} }
            width: parent.width
            height: pageHeader.height
            placeholderText: qsTr("Filter your chats...")
            canHide: text === ""

            onTextChanged: {
                searchChatTimer.restart();
            }

            onHideClicked: {
                resetFocus();
            }

            EnterKey.iconSource: "image://theme/icon-m-enter-close"
            EnterKey.onClicked: {
                resetFocus();
            }
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
            opacity: (overviewPage.chatListCreated && !overviewPage.logoutLoading) ? 1 : 0
            Behavior on opacity { FadeAnimation {} }
            model: chatListModel
            delegate: ChatListViewItem {
                ownUserId: overviewPage.ownUserId
                isVerified: is_verified
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/ChatPage.qml"), {
                        chatInformation : display,
                        chatPicture: photo_small
                    })
                }
            }

            ViewPlaceholder {
                enabled: chatListView.count === 0
                text: chatSearchField.text === "" ? qsTr("You don't have any chats yet.") : qsTr("No matching chats found.")
                hintText: qsTr("You can search public chats or create a new chat via the pull-down menu.")
            }

            VerticalScrollDecorator {}
        }

        Column {
            width: parent.width
            spacing: Theme.paddingMedium
            anchors.verticalCenter: chatListView.verticalCenter

            opacity: overviewPage.chatListCreated && !overviewPage.logoutLoading ? 0 : 1
            Behavior on opacity { FadeAnimation {} }
            visible: !overviewPage.chatListCreated || overviewPage.logoutLoading

            BusyLabel {
                    id: loadingBusyIndicator
                    running: true
            }
        }
    }

    Timer {
        id: interactionHintTimer
        running: false
        interval: 4000
        onTriggered: {
            titleInteractionHint.opacity = 0.0;
        }
    }

    InteractionHintLabel {
        id: titleInteractionHint
        text: qsTr("Tap on the title bar to filter your chats")
        visible: opacity > 0
        invert: true
        anchors.fill: parent
        Behavior on opacity { FadeAnimation {} }
        opacity: 0
    }

}
