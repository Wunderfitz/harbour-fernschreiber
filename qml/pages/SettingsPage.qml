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
import "../js/functions.js" as Functions

Page {
    id: settingsPage
    allowedOrientations: Orientation.All

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
                text: qsTr("Behavior")
            }

            TextSwitch {
                checked: appSettings.sendByEnter
                text: qsTr("Send message by enter")
                description: qsTr("Send your message by pressing the enter key")
                automaticCheck: false
                onClicked: {
                    appSettings.sendByEnter = !checked
                }
            }

            TextSwitch {
                checked: appSettings.focusTextAreaAfterSend
                text: qsTr("Focus text input area after send")
                description: qsTr("Focus the text input area after sending a message")
                automaticCheck: false
                onClicked: {
                    appSettings.focusTextAreaAfterSend = !checked
                }
            }

            TextSwitch {
                checked: appSettings.useOpenWith
                text: qsTr("Open-with menu integration")
                description: qsTr("Integrate Fernschreiber into open-with menu of Sailfish OS")
                automaticCheck: false
                onClicked: {
                    appSettings.useOpenWith = !checked
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

            TextSwitch {
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

            SectionHeader {
                text: qsTr("Appearance")
            }

            TextSwitch {
                checked: appSettings.animateStickers
                text: qsTr("Animate stickers")
                automaticCheck: false
                onClicked: {
                    appSettings.animateStickers = !checked
                }
            }

            TextSwitch {
                checked: appSettings.showStickersAsImages
                text: qsTr("Show stickers as images")
                description: qsTr("Show background for stickers and align them centrally like images")
                automaticCheck: false
                onClicked: {
                    appSettings.showStickersAsImages = !checked
                }
            }

            SectionHeader {
                text: qsTr("Storage")
            }

            TextSwitch {
                checked: appSettings.storageOptimizer
                text: qsTr("Enable storage optimizer")
                automaticCheck: false
                onClicked: {
                    appSettings.storageOptimizer = !checked
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
