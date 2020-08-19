/*
    Copyright (C) 2020 Sebastian J. Wolf

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
import QtQuick 2.5
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: overviewPage
    allowedOrientations: Orientation.All

    property bool initializationCompleted: false;
    property bool loading: true;
    property int authorizationState: TelegramAPI.Closed
    property int connectionState: TelegramAPI.WaitingForNetwork

    onStatusChanged: {
        console.log("[OverviewPage] Status changed: " + status + ", initialization completed: " + initializationCompleted);
        if (status === PageStatus.Active && initializationCompleted) {
            updateContent();
        }
    }

    BusyLabel {
        text: qsTr("Loading...")
        running: overviewPage.loading
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
        overviewPage.handleAuthorizationState();
        overviewPage.connectionState = tdLibWrapper.getConnectionState();
        overviewPage.setPageStatus();
    }

    function handleAuthorizationState() {
        switch (overviewPage.authorizationState) {
        case TelegramAPI.WaitPhoneNumber:
            overviewPage.loading = false;
            pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
            break;
        case TelegramAPI.WaitCode:
            overviewPage.loading = false;
            pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
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
    }

    Component.onCompleted: {
        initializePage();
    }

    SilicaFlickable {
        id: aboutContainer
        contentHeight: parent.height
        contentWidth: parent.width
        anchors.fill: parent
        visible: !overviewPage.loading

        PullDownMenu {
            MenuItem {
                text: qsTr("About Fernschreiber")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/AboutPage.qml"))
            }
        }

        Column {
            id: column
            width: parent.width
            height: parent.height
            spacing: Theme.paddingMedium

            Row {
                id: headerRow
                width: parent.width

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
                    width: parent.width - pageStatus.width
                }
            }

            SilicaListView {

                id: chatListView

                width: parent.width
                height: parent.height - Theme.paddingMedium - headerRow.height

                clip: true
                visible: count > 0

                model: chatListModel
                delegate: ListItem {

                    id: chatListItem

                    contentHeight: chatListRow.height + chatListSeparator.height + 2 * Theme.paddingMedium
                    contentWidth: parent.width

                    onClicked: {
                        // jump to chat details here... ;)
                        // pageStack.push(Qt.resolvedUrl("../pages/ConversationPage.qml"), { "conversationModel" : display, "myUserId": overviewPage.myUser.id_str, "configuration": overviewPage.configuration });
                    }

                    Column {
                        id: chatListColumn
                        width: parent.width - ( 2 * Theme.horizontalPageMargin )
                        spacing: Theme.paddingSmall
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                        }

                        Row {
                            id: chatListRow
                            width: parent.width
                            spacing: Theme.paddingMedium

                            Column {
                                id: chatListPictureColumn
                                width: chatListContentColumn.height
                                height: chatListContentColumn.height
                                spacing: Theme.paddingSmall

                                ProfileThumbnail {
                                    id: chatListPictureThumbnail
                                    photoData: (typeof display.photo !== "undefined") ? display.photo.small : ""
                                    replacementStringHint: chatListNameText.text
                                    width: parent.width
                                    height: parent.width
                                }
                            }

                            Column {
                                id: chatListContentColumn
                                width: parent.width * 5 / 6 - Theme.horizontalPageMargin

                                Text {
                                    id: chatListNameText
                                    text: display.title !== "" ? Emoji.emojify(display.title, Theme.fontSizeMedium) : qsTr("Unknown")
                                    textFormat: Text.StyledText
                                    font.pixelSize: Theme.fontSizeMedium
                                    color: Theme.primaryColor
                                    elide: Text.ElideRight
                                    width: parent.width
                                    onTruncatedChanged: {
                                        // There is obviously a bug in QML in truncating text with images.
                                        // We simply remove Emojis then...
                                        if (truncated) {
                                            text = text.replace(/\<img [^>]+\/\>/g, "");
                                        }
                                    }
                                }

                                Row {
                                    id: chatListLastMessageRow
                                    width: parent.width
                                    spacing: Theme.paddingMedium
                                    Text {
                                        id: chatListLastUserText
                                        text: Emoji.emojify("Unknown", Theme.fontSizeExtraSmall)
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: Theme.highlightColor
                                        textFormat: Text.StyledText
                                        onTruncatedChanged: {
                                            // There is obviously a bug in QML in truncating text with images.
                                            // We simply remove Emojis then...
                                            if (truncated) {
                                                text = text.replace(/\<img [^>]+\/\>/g, "");
                                            }
                                        }
                                    }
                                    Text {
                                        id: chatListLastMessageText
                                        text: Emoji.emojify("Unknown", Theme.fontSizeExtraSmall)
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: Theme.primaryColor
                                        width: parent.width - Theme.paddingMedium - chatListLastUserText.width
                                        elide: Text.ElideRight
                                        textFormat: Text.StyledText
                                        onTruncatedChanged: {
                                            // There is obviously a bug in QML in truncating text with images.
                                            // We simply remove Emojis then...
                                            if (truncated) {
                                                text = text.replace(/\<img [^>]+\/\>/g, "");
                                            }
                                        }
                                    }
                                }

                                Timer {
                                    id: messageContactTimeUpdater
                                    interval: 60000
                                    running: true
                                    repeat: true
                                    onTriggered: {
                                        //messageContactTimeElapsedText.text = getConversationTimeElapsed(display.messages);
                                    }
                                }

                                Text {
                                    id: messageContactTimeElapsedText
                                    //text: getConversationTimeElapsed(display.messages)
                                    text: "somewhen"
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.primaryColor
                                }
                            }
                        }

                    }

                    Separator {
                        id: chatListSeparator

                        anchors {
                            top: chatListColumn.bottom
                            topMargin: Theme.paddingMedium
                        }

                        width: parent.width
                        color: Theme.primaryColor
                        horizontalAlignment: Qt.AlignHCenter
                    }

                }

                VerticalScrollDecorator {}
            }

        }

    }

}
