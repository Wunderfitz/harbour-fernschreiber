/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

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

Page {
    id: activeSessionsPage
    allowedOrientations: Orientation.All

    property variant activeSessions;
    property bool loaded : false;

    Component.onCompleted: {
        if (!activeSessions) {
            tdLibWrapper.getActiveSessions();
        } else {
            activeSessionsPage.loaded = true;
        }
    }

    Connections {
        target: tdLibWrapper
        onSessionsReceived: {
            activeSessionsPage.activeSessions = sessions;
            activeSessionsPage.loaded = true;
        }
        onOkReceived: {
            if (request === "terminateSession") {
                appNotification.show(qsTr("Session was terminated"));
                activeSessionsPage.loaded = false;
                tdLibWrapper.getActiveSessions();
            }
        }
    }

    SilicaFlickable {
        id: activeSessionsFlickable
        anchors.fill: parent

        Column {
            anchors.fill: parent

            PageHeader {
                id: activeSessionsHeader
                title: qsTr("Active Sessions")
            }

            SilicaListView {
                id: activeSessionsListView
                width: parent.width
                height: parent.height - activeSessionsHeader.height

                clip: true

                model: activeSessionsPage.activeSessions
                delegate: ListItem {
                    id: activeSessionListItem
                    width: parent.width
                    contentHeight: activeSessionColumn.height + ( 2 * Theme.paddingMedium )

                    menu: ContextMenu {
                        hasContent: !modelData.is_current
                        MenuItem {
                            onClicked: {
                                var sessionId = modelData.id;
                                Remorse.itemAction(activeSessionListItem, qsTr("Terminating session"), function() { tdLibWrapper.terminateSession(sessionId); });
                            }
                            text: qsTr("Terminate Session")
                        }
                    }

                    Column {
                        id: activeSessionColumn
                        width: parent.width - ( 2 * Theme.horizontalPageMargin )
                        spacing: Theme.paddingSmall
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter

                        Label {
                            width: parent.width
                            text: qsTr("This app")
                            font.pixelSize: Theme.fontSizeMedium
                            font.bold: true
                            visible: modelData.is_current
                            color: Theme.highlightColor
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            width: parent.width
                            text: modelData.application_name + " " + modelData.application_version
                            font.pixelSize: Theme.fontSizeMedium
                            font.bold: true
                            color: Theme.primaryColor
                            maximumLineCount: 1
                            elide: Text.ElideRight
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            width: parent.width
                            text: modelData.device_model + ", " + (modelData.platform + " " + modelData.system_version).trim()
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.primaryColor
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            width: parent.width
                            text: qsTr("IP address: %1, origin: %2").arg(modelData.ip).arg(modelData.country)
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            width: parent.width
                            text: qsTr("Active since: %1, last online: %2").arg(Functions.getDateTimeTimepoint(modelData.log_in_date)).arg(Functions.getDateTimeElapsed(modelData.last_active_date))
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.primaryColor
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }

                    Separator {
                        id: separator
                        anchors {
                            bottom: parent.bottom
                        }

                        width: parent.width
                        color: Theme.primaryColor
                        horizontalAlignment: Qt.AlignHCenter
                    }

                }

                VerticalScrollDecorator {}
            }
        }

        Column {

            opacity: visible ? 1 : 0
            Behavior on opacity { FadeAnimation {} }
            visible: !activeSessionsPage.loaded
            width: parent.width
            height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium

            spacing: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter

            InfoLabel {
                id: loadingLabel
                text: qsTr("Getting active sessions...")
            }

            BusyIndicator {
                id: loadingBusyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                running: !activeSessionsPage.loaded
                size: BusyIndicatorSize.Large
            }
        }

    }
}

