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
import "../../js/twemoji.js" as Emoji
import "../../js/functions.js" as Functions

AccordionItem {
    text: qsTr("Sessions")
    Component {
        Column {
            id: activeSessionsItem
            bottomPadding: Theme.paddingMedium
            property variant activeSessions;
            property bool loaded : false;
            readonly property var userInformation : tdLibWrapper.userInformation

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

            ResponsiveGrid {
                bottomPadding: Theme.paddingMedium
                width: parent.width

                Loader {
                    id: userInformationLoader
                    active: tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady
                    width: parent.columnWidth
                    sourceComponent: Component {
                        Column {
                            anchors.topMargin: Theme.paddingMedium
                            spacing: Theme.paddingMedium

                            Text {
                                x: Theme.horizontalPageMargin
                                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                                horizontalAlignment: Text.AlignHCenter
                                text: qsTr("Logged in as %1").arg(Emoji.emojify(activeSessionsItem.userInformation.first_name + " " + activeSessionsItem.userInformation.last_name, Theme.fontSizeSmall))
                                font.pixelSize: Theme.fontSizeSmall
                                wrapMode: Text.Wrap
                                color: Theme.primaryColor
                                textFormat: Text.StyledText
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                            }

                            ProfileThumbnail {
                                photoData: ((typeof activeSessionsItem.userInformation.profile_photo !== "undefined") ? activeSessionsItem.userInformation.profile_photo.small : {})
                                width: Theme.itemSizeExtraLarge
                                height: Theme.itemSizeExtraLarge
                                replacementStringHint: activeSessionsItem.userInformation.first_name + " " + activeSessionsItem.userInformation.last_name
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                            }

                            Label {
                                x: Theme.horizontalPageMargin
                                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                                horizontalAlignment: Text.AlignHCenter
                                text: qsTr("Phone number: +%1").arg(activeSessionsItem.userInformation.phone_number)
                                font.pixelSize: Theme.fontSizeSmall
                                wrapMode: Text.Wrap
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                            }

                            BackgroundItem {
                                id: logOutItem
                                width: parent.width
                                function showRemorseItem() {
                                    remorse.execute(logOutItem, qsTr("Logged out"), function() {
                                        tdLibWrapper.logout();
                                        pageStack.pop();
                                    });
                                }
                                RemorseItem { id: remorse }
                                Button {
                                    id: logOutButton
                                    text: qsTr("Log Out")
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    onClicked: logOutItem.showRemorseItem()
                                }
                            }
                        }
                    }
                }

                Loader {
                    id: sessionInformationLoader
                    active: tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady
                    width: parent.columnWidth
                    sourceComponent: Component {
                        SilicaListView {
                            id: activeSessionsListView
                            width: parent.width
                            // one activeSessionListItem is about 1.52 times itemSizeLarge
                            // show max 5 items at a time
                            height: Theme.itemSizeLarge * 1.5 * Math.min(5 , activeSessionsItem.activeSessions.length )
                            clip: true

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
                                    MenuItem {
                                        onClicked: {
                                            var sessionId = modelData.id;
                                            Remorse.itemAction(activeSessionListItem, qsTr("Terminating session"), function() { tdLibWrapper.terminateSession(sessionId); });
                                        }
                                        text: qsTr("Terminate Session")
                                    }
                                }

                                Rectangle {
                                    height: parent.height - Theme.paddingSmall
                                    width: parent.width
                                    anchors.centerIn: parent
                                    visible: modelData.is_current
                                    color: Theme.rgba(Theme.highlightBackgroundColor, Theme.opacityFaint)
                                }

                                Column {
                                    id: activeSessionColumn
                                    width: parent.width - ( 2 * Theme.horizontalPageMargin )
                                    spacing: Theme.paddingSmall
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    Label {
                                        width: parent.width
                                        property string hltext: modelData.is_current ? " (" + qsTr("this app") + ")" : ""
                                        text: Theme.highlightText( modelData.application_name + " " + modelData.application_version + hltext, hltext, Theme.highlightColor)
                                        font.pixelSize: modelData.is_current ? Theme.fontSizeMedium : Theme.fontSizeSmall
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
                }
            }
        }
    }
}
