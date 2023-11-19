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
            property variant activeSessions
            property int inactiveSessionsTtlDays

            Component.onCompleted: {
                if (!activeSessions) {
                    tdLibWrapper.getActiveSessions();
                }
            }

            Connections {
                target: tdLibWrapper
                onSessionsReceived: {
                    activeSessionsItem.activeSessions = sessions
                    activeSessionsItem.inactiveSessionsTtlDays = inactive_session_ttl_days
                }
                onOkReceived: {
                    if (request === "terminateSession") {
                        appNotification.show(qsTr("Session was terminated"));
                        tdLibWrapper.getActiveSessions();
                    }
                }
            }

            Loader {
                active: tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady
                width: parent.width
                sourceComponent: Component {
                    Column {
                        BusyIndicator {
                            anchors.horizontalCenter: parent.horizontalCenter
                            running: !activeSessionsListView.count && !activeSessionsItem.inactiveSessionsTtlDays
                            size: BusyIndicatorSize.Medium
                            visible: opacity > 0
                            height: running ? implicitHeight : 0
                        }

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
                                visible: activeSessionsListView.count > 0
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
                                    }

                                    Label {
                                        width: parent.width
                                        text: modelData.application_name + " " + modelData.application_version
                                        font.pixelSize: Theme.fontSizeMedium
                                        font.bold: true
                                        maximumLineCount: 1
                                        elide: Text.ElideRight
                                    }

                                    Label {
                                        width: parent.width
                                        text: modelData.device_model + ", " + (modelData.platform + " " + modelData.system_version).trim()
                                        font.pixelSize: Theme.fontSizeSmall
                                        maximumLineCount: 1
                                        truncationMode: TruncationMode.Fade
                                    }

                                    Label {
                                        width: parent.width
                                        text: qsTr("Active since: %1, last online: %2").arg(Functions.getDateTimeTimepoint(modelData.log_in_date)).arg(Functions.getDateTimeElapsed(modelData.last_active_date))
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        maximumLineCount: 1
                                        truncationMode: TruncationMode.Fade
                                    }
                                }

                                Separator {
                                    anchors {
                                        bottom: parent.bottom
                                    }

                                    width: parent.width
                                    color: Theme.primaryColor
                                    horizontalAlignment: Qt.AlignHCenter
                                }
                            }
                        }

                        ComboBox {
                            readonly property int ttl: activeSessionsItem.inactiveSessionsTtlDays
                            label: qsTr("Terminate old sessions if inactive for")
                            value: (currentItem && currentItem.text) ? currentItem.text : qsTr("%1 day(s)", "", ttl).arg(ttl)
                            visible: ttl > 0
                            menu: ContextMenu {
                                id: ttlMenu
                                MenuItem {
                                    readonly property int days: 7
                                    text: qsTr("1 week")
                                    onClicked: tdLibWrapper.setInactiveSessionTtl(days)
                                }
                                MenuItem {
                                    readonly property int days: 30
                                    text: qsTr("1 month")
                                    onClicked: tdLibWrapper.setInactiveSessionTtl(days)
                                }
                                MenuItem {
                                    readonly property int days: 90
                                    text: qsTr("3 months")
                                    onClicked: tdLibWrapper.setInactiveSessionTtl(days)
                                }
                                MenuItem {
                                    readonly property int days: 180
                                    text: qsTr("6 months")
                                    onClicked: tdLibWrapper.setInactiveSessionTtl(days)
                                }
                                MenuItem {
                                    readonly property int days: 365
                                    text: qsTr("1 year")
                                    onClicked: tdLibWrapper.setInactiveSessionTtl(days)
                                }
                            }

                            Component.onCompleted: updateSelection()

                            onTtlChanged: updateSelection()

                            function updateSelection() {
                                var menuItems = ttlMenu.children
                                var n = menuItems.length
                                for (var i = 0; i < n; i++) {
                                    if (menuItems[i].days === ttl) {
                                        currentIndex = i
                                        return
                                    }
                                }
                                currentIndex = -1
                            }
                        }
                    }
                }
            }
        }
    }
}
