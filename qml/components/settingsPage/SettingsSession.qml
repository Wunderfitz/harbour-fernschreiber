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
import WerkWolf.Fernschreiber 1.0
import "../../components"
import "../../js/functions.js" as Functions

AccordionItem {
    text: qsTr("Sessions")
    property SilicaFlickable flickable: parent.flickable
    Component {
        Column {
            id: activeSessionsItem
            bottomPadding: Theme.paddingMedium
            property variant activeSessions;
            property bool loaded : false;

            Component.onCompleted: {
                if (!activeSessions) {
                    tdLibWrapper.getActiveSessions();
                } else {
                    activeSessionsItem.loaded = true;
                }
            }

            Connections {
                target: tdLibWrapper
                onSessionsReceived: {
                    activeSessionsItem.activeSessions = sessions;
                    activeSessionsItem.loaded = true;
                }
                onOkReceived: {
                    if (request === "terminateSession") {
                        appNotification.show(qsTr("Session was terminated"));
                        activeSessionsItem.loaded = false;
                        tdLibWrapper.getActiveSessions();
                    }
                }
            }

            Loader {
                id: sessionInformationLoader
                active: tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady
                width: parent.width
                sourceComponent: Component {
                    SilicaListView {
                        id: activeSessionsListView
                        width: parent.width
                        height: contentHeight
                        model: activeSessionsItem.activeSessions
                        headerPositioning: ListView.OverlayHeader
                        header: Separator {
                            width: parent.width
                            color: Theme.primaryColor
                            horizontalAlignment: Qt.AlignHCenter
                        }
                        delegate: ListItem {
                            id: activeSessionListItem
                            width: parent.width
                            contentHeight: activeSessionColumn.height + ( 2 * Theme.paddingMedium )

                            menu: ContextMenu {
                                hasContent: !modelData.is_current
                                onHeightChanged: {
                                    if (parent && flickable) {
                                        // Make sure we are inside the screen area
                                        var bottom = parent.mapToItem(flickable, x, y).y + height
                                        if (bottom > flickable.height) {
                                            flickable.contentY += bottom - flickable.height
                                        }
                                    }
                                }
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
                    }
                }
            }
        }
    }
}
