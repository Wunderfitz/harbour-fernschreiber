/*
    Copyright (C) 2020 Sebastian J. Wolf

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


Column {
    id: chatPermissionsColumn

    function setChatPermissions(textSwitchItem) {
        if(textSwitchItem.busy) {return;}
        var messageSubItems = ["can_send_media_messages", "can_send_polls", "can_send_other_messages", "can_add_web_page_previews"];
        var permissionName = textSwitchItem.permissionName;
        var permissionValue = !textSwitchItem.checked
        textSwitchItem.busy = true;
        var newPermissions = chatInformationPage.chatInformation.permissions;
        if(permissionName in newPermissions) {
            newPermissions[permissionName] = permissionValue;
            // some permissions infer can_send_messages:
            if(permissionName === "can_send_messages" && !permissionValue) {
                for(var i in messageSubItems) {
                    newPermissions[messageSubItems[i]] = false;
                }
            } else if(messageSubItems.indexOf(permissionName) > -1 && permissionValue) {
                newPermissions.can_send_message = true;
            }
        }
        tdLibWrapper.setChatPermissions(chatInformationPage.chatInformation.id, newPermissions);
    }

    Column {
        visible: chatInformationPage.groupInformation.status.can_restrict_members || chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusCreator"
        width: parent.width

        SectionHeader {
            text: qsTr("Group Member Permissions", "what can normal group members do")
        }
        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_send_messages"
            text: qsTr("Send Messages", "member permission")
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onCheckedChanged: {busy = false;}
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }
        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_send_media_messages"
            text: qsTr("Send Media Messages", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_send_other_messages"
            text: qsTr("Send Other Messages", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_add_web_page_previews"
            text: qsTr("Add Web Page Previews", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_change_info"
            text: qsTr("Change Chat Info", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_invite_users"
            text: qsTr("Invite Users", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property string permissionName: "can_pin_messages"
            text: qsTr("Pin Messages", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatInformationPage.chatInformation.permissions[permissionName]
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }
    }

    SectionHeader {
        visible: historyAvailableSwitch.visible
        text: qsTr("New Members", "what can new group members do")
    }


    TextSwitch {
        id: historyAvailableSwitch
        visible: chatInformationPage.isSuperGroup && chatInformationPage.groupInformation.status && chatInformationPage.groupInformation.status.can_change_info
        automaticCheck: false
        text: qsTr("New members can see older messages", "member permission")
        onCheckedChanged: {busy = false;}
        checked: chatInformationPage.groupFullInformation.is_all_history_available
        onClicked: {
            tdLibWrapper.toggleSupergroupIsAllHistoryAvailable(chatInformationPage.chatPartnerGroupId, !checked);
        }
    }
}
