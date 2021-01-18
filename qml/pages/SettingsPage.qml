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
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/functions.js" as Functions

Page {
    id: settingsPage
    allowedOrientations: Orientation.All

    readonly property bool landscapeLayout: (width > height && Screen.sizeCategory > Screen.Small) || Screen.sizeCategory > Screen.Medium

    Connections {
        target: tdLibWrapper
        onOwnUserUpdated: {
            firstNameEditArea.text = userInformation.first_name;
            lastNameEditArea.text = userInformation.last_name;
        }
    }

    SilicaFlickable {
        id: settingsContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: settingsPage.width

            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("User Profile")
            }

            InformationEditArea {
                id: firstNameEditArea
                visible: true
                canEdit: true
                headerText: qsTr("First Name", "first name of the logged-in profile - header")
                text: tdLibWrapper.getUserInformation().first_name
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                headerLeftAligned: true
                anchors.horizontalCenter: parent.horizontalCenter

                onSaveButtonClicked: {
                    if(!editItem.errorHighlight) {
                        tdLibWrapper.setName(textValue, lastNameEditArea.text);
                    } else {
                        isEditing = true;
                    }
                }

                onTextEdited: {
                    if(textValue.length > 0 && textValue.length < 65) {
                        editItem.errorHighlight = false;
                        editItem.label = "";
                        editItem.placeholderText = "";
                    } else {
                        editItem.label = qsTr("Enter 1-64 characters");
                        editItem.placeholderText = editItem.label;
                        editItem.errorHighlight = true;
                    }
                }
            }

            InformationEditArea {
                id: lastNameEditArea
                visible: true
                canEdit: true
                headerText: qsTr("Last Name", "last name of the logged-in profile - header")
                text: tdLibWrapper.getUserInformation().last_name
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                headerLeftAligned: true
                anchors.horizontalCenter: parent.horizontalCenter

                onSaveButtonClicked: {
                    if(!editItem.errorHighlight) {
                        tdLibWrapper.setName(firstNameEditArea.text, textValue);
                    } else {
                        isEditing = true;
                    }
                }

                onTextEdited: {
                    if(textValue.length >= 0 && textValue.length < 65) {
                        editItem.errorHighlight = false;
                        editItem.label = "";
                        editItem.placeholderText = "";
                    } else {
                        editItem.label = qsTr("Enter 0-64 characters");
                        editItem.placeholderText = editItem.label;
                        editItem.errorHighlight = true;
                    }
                }
            }

            SectionHeader {
                text: qsTr("Behavior")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1

                readonly property real columnWidth: width/columns

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.sendByEnter
                    text: qsTr("Send message by enter")
                    description: qsTr("Send your message by pressing the enter key")
                    automaticCheck: false
                    onClicked: {
                        appSettings.sendByEnter = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.focusTextAreaAfterSend
                    text: qsTr("Focus text input area after send")
                    description: qsTr("Focus the text input area after sending a message")
                    automaticCheck: false
                    onClicked: {
                        appSettings.focusTextAreaAfterSend = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.useOpenWith
                    text: qsTr("Open-with menu integration")
                    description: qsTr("Integrate Fernschreiber into open-with menu of Sailfish OS")
                    automaticCheck: false
                    onClicked: {
                        appSettings.useOpenWith = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.notificationTurnsDisplayOn && enabled
                    text: qsTr("Notification turns on the display")
                    height: appSettings.notificationFeedback === AppSettings.NotificationFeedbackNone ? 0 : implicitHeight
                    clip: height < implicitHeight
                    visible: height > 0
                    automaticCheck: false
                    onClicked: {
                        appSettings.notificationTurnsDisplayOn = !checked
                    }
                    Behavior on height { SmoothedAnimation { duration: 200 } }
                }
            }

            ComboBox {
                id: feedbackComboBox
                label: qsTr("Notification feedback")
                description: qsTr("Use non-graphical feedback (sound, vibration) for notifications")
                menu: ContextMenu {
                    id: feedbackMenu

                    MenuItem {
                        readonly property int value: AppSettings.NotificationFeedbackAll
                        text: qsTr("All events")
                        onClicked: {
                            appSettings.notificationFeedback = value
                        }
                    }
                    MenuItem {
                        readonly property int value: AppSettings.NotificationFeedbackNew
                        text: qsTr("Only new events")
                        onClicked: {
                            appSettings.notificationFeedback = value
                        }
                    }
                    MenuItem {
                        readonly property int value: AppSettings.NotificationFeedbackNone
                        text: qsTr("None")
                        onClicked: {
                            appSettings.notificationFeedback = value
                        }
                    }
                }

                Component.onCompleted: updateFeedbackSelection()

                function updateFeedbackSelection() {
                    var menuItems = feedbackMenu.children
                    var n = menuItems.length
                    for (var i=0; i<n; i++) {
                        if (menuItems[i].value === appSettings.notificationFeedback) {
                            currentIndex = i
                            return
                        }
                    }
                }

                Connections {
                    target: appSettings
                    onNotificationFeedbackChanged: {
                        feedbackComboBox.updateFeedbackSelection()
                    }
                }
            }

            SectionHeader {
                text: qsTr("Appearance")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1

                readonly property real columnWidth: width/columns

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.showStickersAsImages
                    text: qsTr("Show stickers as images")
                    description: qsTr("Show background for stickers and align them centrally like images")
                    automaticCheck: false
                    onClicked: {
                        appSettings.showStickersAsImages = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.animateStickers
                    text: qsTr("Animate stickers")
                    automaticCheck: false
                    onClicked: {
                        appSettings.animateStickers = !checked
                    }
                }
            }

            SectionHeader {
                text: qsTr("Privacy")
            }

            TextSwitch {
                checked: appSettings.allowInlineBotLocationAccess
                text: qsTr("Allow sending Location to inline bots")
                description: qsTr("Some inline bots request location data when using them")
                automaticCheck: false
                onClicked: {
                    appSettings.allowInlineBotLocationAccess = !checked
                }
            }

            SectionHeader {
                text: qsTr("Storage")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1

                readonly property real columnWidth: width/columns

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.onlineOnlyMode
                    text: qsTr("Enable online-only mode")
                    description: qsTr("Disables offline caching. Certain features may be limited or missing in this mode. Changes require a restart of Fernschreiber to take effect.")
                    automaticCheck: false
                    onClicked: {
                        appSettings.onlineOnlyMode = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.storageOptimizer
                    text: qsTr("Enable storage optimizer")
                    automaticCheck: false
                    onClicked: {
                        appSettings.storageOptimizer = !checked
                    }
                }
            }

            Item {
                width: 1
                height: Theme.paddingLarge // Some space at the bottom
            }
        }

        VerticalScrollDecorator {}
    }
}
