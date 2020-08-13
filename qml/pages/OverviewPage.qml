/*
    Copyright (C) 2020 Sebastian J. Wolf

    This file is part of Fernschreiber.

    fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    fernschreiber is distributed in the hope that it will be useful,
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

    Connections {
        target: tdLibWrapper
        onAuthorizationStateChanged: {
            switch (authorizationState) {
            case TelegramAPI.WaitPhoneNumber:
                overviewPage.loading = false;
                pageStack.clear();
                pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
                break;
            case TelegramAPI.WaitCode:
                overviewPage.loading = false;
                pageStack.clear();
                pageStack.push(Qt.resolvedUrl("../pages/InitializationPage.qml"));
                break;
            case TelegramAPI.AuthorizationReady:
                overviewPage.loading = false;
                break;
            default:
                // Nothing ;)
            }
            overviewPage.authorizationState = authorizationState;
        }
        onConnectionStateChanged: {
            overviewPage.connectionState = connectionState;
        }
    }

    Component.onCompleted: {
        overviewPage.authorizationState = tdLibWrapper.getAuthorizationState();
        if (overviewPage.authorizationState === TelegramAPI.AuthorizationReady) {
            overviewPage.loading = false;
        }

        overviewPage.connectionState = tdLibWrapper.getConnectionState();
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

            PageHeader {
                title: qsTr("Welcome to Fernschreiber")
            }

            VerticalScrollDecorator {}
        }

    }

}
