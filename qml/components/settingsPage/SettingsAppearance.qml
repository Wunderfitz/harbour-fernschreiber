/*
    Copyright (C) 2021 Sebastian J. Wolf and other contributors

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

AccordionItem {
    text: qsTr("Appearance")
    clip: heightBehavior.enabled || heightAnimation.running

    // One-shot behavior
    Behavior on height {
        id: heightBehavior
        enabled: false
        SequentialAnimation {
            id: heightAnimation
            SmoothedAnimation { duration: 200 }
            ScriptAction { script: heightBehavior.enabled = false }
        }
    }

    Component {
        ResponsiveGrid {
            bottomPadding: Theme.paddingMedium

            TextSwitch {
                width: parent.columnWidth
                checked: appSettings.showStickersAsEmojis
                text: qsTr("Show stickers as emojis")
                description: qsTr("Only display emojis instead of the actual stickers")
                automaticCheck: false
                onClicked: {
                    heightBehavior.enabled = true
                    appSettings.showStickersAsEmojis = !checked
                }
            }

            TextSwitch {
                width: parent.columnWidth
                checked: appSettings.showStickersAsImages
                text: qsTr("Show stickers as images")
                description: qsTr("Show background for stickers and align them centrally like images")
                automaticCheck: false
                onClicked: {
                    appSettings.showStickersAsImages = !checked
                }
                visible: !appSettings.showStickersAsEmojis
                opacity: visible ? 1 : 0
                Behavior on opacity { FadeAnimation  { } }
            }

            Item {
                // Placeholder to move the next switch to the second column
                visible: parent.columns === 2
                width: 1
                height: 1
            }

            TextSwitch {
                width: parent.columnWidth
                checked: appSettings.animateStickers
                text: qsTr("Animate stickers")
                automaticCheck: false
                onClicked: {
                    appSettings.animateStickers = !checked
                }
                visible: !appSettings.showStickersAsEmojis
                opacity: visible ? 1 : 0
                Behavior on opacity { FadeAnimation  { } }
            }

            Item {
                // Placeholder to keep the next switch underneath "Animate stickers"
                visible: parent.columns === 2 && !appSettings.showStickersAsEmojis
                width: 1
                height: 1
            }

            TextSwitch {
                width: parent.columnWidth
                checked: appSettings.animateStickersInPicker
                text: qsTr("Animate stickers in the sticker picker")
                description: qsTr("Plays animated stickers while choosing one to send. Needs more battery and data.")
                automaticCheck: false
                enabled: appSettings.animateStickers
                onClicked: {
                    appSettings.animateStickersInPicker = !checked
                }
                visible: !appSettings.showStickersAsEmojis
                opacity: visible ? 1 : 0
                Behavior on opacity { FadeAnimation  { } }
            }

            TextSwitch {
                width: parent.columnWidth
                checked: appSettings.showSavedMessagesProfile
                text: qsTr("Show profile info for saved messages")
                description: qsTr("Shows your own name and picture for the chat with yourself instead of \"Saved Messages\"")
                automaticCheck: false
                onClicked: {
                    appSettings.showSavedMessagesProfile = !checked
                }
            }
        }
    }
}
