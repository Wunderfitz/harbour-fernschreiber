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
    property int authorizationState: TelegramAPI.Closed
    property int connectionState: TelegramAPI.WaitingForNetwork
    property int ownUserId;
    property bool chatListCreated: false;

    onStatusChanged: {
        if (status === PageStatus.Active && initializationCompleted && !chatListCreated) {
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
            overviewPage.loading = false;
            if(isOnInitialization) { // pageStack isn't ready on Component.onCompleted
                openInitializationPageTimer.start()
            } else {
                pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
            }
            break;
        case TelegramAPI.AuthorizationReady:
            overviewPage.loading = false;
            overviewPage.initializationCompleted = true;
            overviewPage.updateContent();
            break;
        default:
            // Nothing ;)
        }
    }

    function resetFocus() {
        chatSearchField.focus = false;
        overviewPage.focus = true;
        chatSearchField.visible = false;
        pageHeader.visible = true;
        searchChatButton.visible = overviewPage.connectionState === TelegramAPI.ConnectionReady;
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
            }
        }
        onChatOrderUpdated: {
            if (!overviewPage.chatListCreated) {
                chatListCreatedTimer.restart();
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
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/SettingsPage.qml"))
            }
            MenuItem {
                text: qsTr("New Chat")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/NewChatPage.qml"))
            }
        }

        Row {
            id: headerRow
            width: parent.width - Theme.horizontalPageMargin

            GlassItem {
                id: pageStatus
                width: Theme.itemSizeMedium
                height: Theme.itemSizeMedium
                color: "red"
                falloffRadius: 0.1
                radius: 0.2
                cache: false
            }

            PageHeader {
                id: pageHeader
                title: qsTr("Fernschreiber")
                width: visible ? ( parent.width - pageStatus.width - searchChatButton.width ) : 0
                opacity: visible ? 1 : 0
                Behavior on opacity { FadeAnimation {} }
            }

            IconButton {
                id: searchChatButton
                width: visible ? height : 0
                opacity: visible ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
                anchors.verticalCenter: parent.verticalCenter
                icon {
                    source: "image://theme/icon-m-search?" + Theme.highlightColor
                    asynchronous: true
                }
                visible: overviewPage.connectionState === TelegramAPI.ConnectionReady
                onClicked: {
                    chatSearchField.focus = true;
                    chatSearchField.visible = true;
                    pageHeader.visible = false;
                    searchChatButton.visible = false;
                }
            }

            SearchField {
                id: chatSearchField
                visible: false
                opacity: visible ? 1 : 0
                Behavior on opacity { FadeAnimation {} }
                width: visible ? ( parent.width - pageStatus.width ) : 0
                height: pageHeader.height
                placeholderText: qsTr("Search a chat...")
                active: searchHeaderItem.visible

                onTextChanged: {
                    searchChatTimer.restart();
                }

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    resetFocus();
                }
            }

        }

        SilicaListView {
            id: chatListView
            anchors {
                top: headerRow.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            clip: true
            opacity: overviewPage.chatListCreated ? 1 : 0
            Behavior on opacity { FadeAnimation {} }
            model: chatSearchField.text !== "" ? chatListProxyModel : chatListModel
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
                text: qsTr("You don't have any chats yet.")
            }

            VerticalScrollDecorator {}
        }

        Column {
            width: parent.width
            spacing: Theme.paddingMedium
            anchors.verticalCenter: chatListView.verticalCenter

            opacity: overviewPage.chatListCreated ? 0 : 1
            Behavior on opacity { FadeAnimation {} }
            visible: !overviewPage.chatListCreated

            InfoLabel {
                id: loadingLabel
                text: qsTr("Loading chat list...")
            }

            BusyIndicator {
                id: loadingBusyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                running: !overviewPage.chatListCreated
                size: BusyIndicatorSize.Large
            }
        }
    }
}
