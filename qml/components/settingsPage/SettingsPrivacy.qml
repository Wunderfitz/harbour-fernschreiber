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
            Connections {
                target: tdLibWrapper
                onUserPrivacySettingUpdated: {
                    Debug.log("Received updated privacy setting: " + setting + ":" + rule);
                    switch (setting) {
                    case TelegramAPI.SettingAllowChatInvites:
                        allowChatInvitesComboBox.currentIndex = rule;
                        break;
                    case TelegramAPI.SettingAllowFindingByPhoneNumber:
                        allowFindingByPhoneNumberComboBox.currentIndex = rule;
                        break;
                    case TelegramAPI.SettingShowLinkInForwardedMessages:
                        showLinkInForwardedMessagesComboBox.currentIndex = rule;
                        break;
                    case TelegramAPI.SettingShowPhoneNumber:
                        showPhoneNumberComboBox.currentIndex = rule;
                        break;
                    case TelegramAPI.SettingShowProfilePhoto:
                        showProfilePhotoComboBox.currentIndex = rule;
                        break;
                    case TelegramAPI.SettingShowStatus:
                        showStatusComboBox.currentIndex = rule;
                        break;
                    }
                }
            }
            ResponsiveGrid {
                ComboBox {
                    id: allowChatInvitesComboBox
                    width: parent.columnWidth
                    label: qsTr("Allow chat invites")
                    description: qsTr("Privacy setting for managing whether you can be invited to chats.")
                    menu: ContextMenu {
                        x: 0
                        width: allowChatInvitesComboBox.width

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites);
                    }
                }

                ComboBox {
                    id: allowFindingByPhoneNumberComboBox
                    width: parent.columnWidth
                    label: qsTr("Allow finding by phone number")
                    description: qsTr("Privacy setting for managing whether you can be found by your phone number.")
                    menu: ContextMenu {
                        x: 0
                        width: allowFindingByPhoneNumberComboBox.width

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowFindingByPhoneNumber, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowFindingByPhoneNumber, TelegramAPI.RuleAllowContacts);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingAllowFindingByPhoneNumber);
                    }
                }

                ComboBox {
                    id: showLinkInForwardedMessagesComboBox
                    width: parent.columnWidth
                    label: qsTr("Show link in forwarded messages")
                    description: qsTr("Privacy setting for managing whether a link to your account is included in forwarded messages.")
                    menu: ContextMenu {
                        x: 0
                        width: showLinkInForwardedMessagesComboBox.width

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages);
                    }
                }

                ComboBox {
                    id: showPhoneNumberComboBox
                    width: parent.columnWidth
                    label: qsTr("Show phone number")
                    description: qsTr("Privacy setting for managing whether your phone number is visible.")
                    menu: ContextMenu {
                        x: 0
                        width: showPhoneNumberComboBox.width

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber);
                    }
                }

                ComboBox {
                    id: showProfilePhotoComboBox
                    width: parent.columnWidth
                    label: qsTr("Show profile photo")
                    description: qsTr("Privacy setting for managing whether your profile photo is visible.")
                    menu: ContextMenu {
                        x: 0
                        width: showProfilePhotoComboBox.width

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto);
                    }
                }

                ComboBox {
                    id: showStatusComboBox
                    width: parent.columnWidth
                    label: qsTr("Show status")
                    description: qsTr("Privacy setting for managing whether your online status is visible.")
                    menu: ContextMenu {
                        x: 0
                        width: showStatusComboBox.width

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowStatus, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowStatus, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowStatus, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowStatus);
                    }
                }
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
