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
    property alias headerTitle: pageHeader.title
    property alias headerDescription: pageHeader.description
    property var payload: ({})

    onAccepted: {
        switch(chatSelectionPage.state) {
        case "forwardMessages":
            acceptDestinationInstance.forwardMessages(payload.fromChatId, payload.messageIds)
            break;
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
            ownUserId: overviewPage.ownUserId

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
