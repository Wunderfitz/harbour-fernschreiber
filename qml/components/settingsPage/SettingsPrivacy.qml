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
import WerkWolf.Fernschreiber 1.0

AccordionItem {
    text: qsTr("Privacy")
    Component {
        Column {
            bottomPadding: Theme.paddingMedium
            ResponsiveGrid {
                id: privacySettingsGrid
                Repeater {
                    model: [
                        { setting: TelegramAPI.SettingShowPhoneNumber, label: qsTr("Show phone number"), description: qsTr("Privacy setting for managing whether your phone number is visible.") },
                        // TDLib only accepts "allow all" and "allow contacts" for this one
                        { setting: TelegramAPI.SettingAllowFindingByPhoneNumber, label: qsTr("Allow finding by phone number"), description: qsTr("Privacy setting for managing whether you can be found by your phone number."), canRestrictAll: false },
                        { setting: TelegramAPI.SettingShowStatus, label: qsTr("Show status"), description: qsTr("Privacy setting for managing whether your online status is visible.") },
                        { setting: TelegramAPI.SettingShowProfilePhoto, label: qsTr("Show profile photo"), description: qsTr("Privacy setting for managing whether your profile photo is visible.") },
                        { setting: TelegramAPI.SettingShowBio, label: qsTr("Show bio"), description: qsTr("Privacy setting for managing whether your bio is visible.") },
                        { setting: TelegramAPI.SettingShowBirthdate, label: qsTr("Show birthdate"), description: qsTr("Privacy setting for managing whether your birthdate is visible.") },
                        { setting: TelegramAPI.SettingShowLinkInForwardedMessages, label: qsTr("Show link in forwarded messages"), description: qsTr("Privacy setting for managing whether a link to your account is included in forwarded messages.") },
                        { setting: TelegramAPI.SettingAllowCalls, label: qsTr("Allow calls"), description: qsTr("Privacy setting for managing whether you can be called.") },
                        { setting: TelegramAPI.SettingAllowPeerToPeerCalls, label: qsTr("Allow peer-to-peer calls"), description: qsTr("Privacy setting for managing whether peer-to-peer connections can be used for calls.") },
                        { setting: TelegramAPI.SettingAllowChatInvites, label: qsTr("Allow chat invites"), description: qsTr("Privacy setting for managing whether you can be invited to chats.") }
                    ]
                    // The menu items are in the order of TelegramAPI.RuleAllowAll, RuleAllowContacts and RuleRestrictAll,
                    // so a rule is also the index of its menu item
                    ComboBox {
                        id: privacySettingComboBox
                        width: privacySettingsGrid.columnWidth
                        label: modelData.label
                        description: modelData.description
                        menu: ContextMenu {
                            x: 0
                            width: privacySettingComboBox.width

                            MenuItem {
                                text: qsTr("Yes")
                                onClicked: {
                                    tdLibWrapper.setUserPrivacySettingRule(modelData.setting, TelegramAPI.RuleAllowAll);
                                }
                            }
                            MenuItem {
                                text: qsTr("Your contacts only")
                                onClicked: {
                                    tdLibWrapper.setUserPrivacySettingRule(modelData.setting, TelegramAPI.RuleAllowContacts);
                                }
                            }
                            MenuItem {
                                visible: modelData.canRestrictAll !== false
                                text: qsTr("No")
                                onClicked: {
                                    tdLibWrapper.setUserPrivacySettingRule(modelData.setting, TelegramAPI.RuleRestrictAll);
                                }
                            }
                        }

                        Component.onCompleted: {
                            currentIndex = tdLibWrapper.getUserPrivacySettingRule(modelData.setting);
                        }

                        Connections {
                            target: tdLibWrapper
                            onUserPrivacySettingUpdated: {
                                if (setting === modelData.setting) {
                                    Debug.log("Received updated privacy setting: " + setting + ":" + rule);
                                    privacySettingComboBox.currentIndex = rule;
                                }
                            }
                        }
                    }
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x
                topPadding: Theme.paddingMedium
                bottomPadding: Theme.paddingMedium
                text: qsTr("Exceptions for single users or chats, e.g. made in other Telegram apps, are kept.")
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                wrapMode: Text.Wrap
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
        }
    }
}
