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


Page {
    id: overviewPage
    allowedOrientations: Orientation.All

    property bool loading: true;
    property int authorizationState: TelegramAPI.Closed
    property int connectionState: TelegramAPI.WaitingForNetwork

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
        if (overviewPage.authorizationState === TelegramAPI.AuthorizationReady) {
            overviewPage.loading = false;
            overviewPage.updateContent();
        }
        overviewPage.connectionState = tdLibWrapper.getConnectionState();
        overviewPage.setPageStatus();
    }

    Connections {
        target: tdLibWrapper
        onAuthorizationStateChanged: {
            switch (authorizationState) {
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
                overviewPage.updateContent();
                break;
            default:
                // Nothing ;)
            }
            overviewPage.authorizationState = authorizationState;
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
        contentHeight: column.height
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
            spacing: Theme.paddingLarge

            Row {
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

            VerticalScrollDecorator {}
        }

    }

}
