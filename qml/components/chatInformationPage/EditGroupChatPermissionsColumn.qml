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


Column {
    id: chatPermissionsColumn

    // TDLib has no single "send media" permission, there is one per kind of media
    readonly property var mediaPermissions: ["can_send_audios", "can_send_documents", "can_send_photos",
                                             "can_send_videos", "can_send_video_notes", "can_send_voice_notes"]
    // Everything that can only be sent by someone who may send messages at all
    readonly property var messageSubPermissions: mediaPermissions.concat(["can_send_polls", "can_send_other_messages",
                                                                          "can_add_link_previews"])

    function hasPermissions(permissionNames) {
        var permissions = chatInformationPage.chatInformation.permissions;
        for(var i = 0; i < permissionNames.length; i++) {
            if(!permissions[permissionNames[i]]) {
                return false;
            }
        }
        return true;
    }

    function setChatPermissions(textSwitchItem) {
        if(textSwitchItem.busy) {return;}
        var permissionNames = textSwitchItem.permissionNames;
        var permissionValue = !textSwitchItem.checked;
        var i;
        textSwitchItem.busy = true;
        var newPermissions = chatInformationPage.chatInformation.permissions;
        for(i = 0; i < permissionNames.length; i++) {
            newPermissions[permissionNames[i]] = permissionValue;
        }
        if(permissionNames.indexOf("can_send_basic_messages") > -1) {
            // Revoking the right to send messages revokes everything built on it
            if(!permissionValue) {
                for(i = 0; i < messageSubPermissions.length; i++) {
                    newPermissions[messageSubPermissions[i]] = false;
                }
            }
        } else if(permissionValue && messageSubPermissions.indexOf(permissionNames[0]) > -1) {
            // ... and granting any of them implies that right
            newPermissions.can_send_basic_messages = true;
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
            property var permissionNames: ["can_send_basic_messages"]
            text: qsTr("Send Messages", "member permission")
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
            onCheckedChanged: {busy = false;}
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }
        TextSwitch {
            automaticCheck: false
            property var permissionNames: chatPermissionsColumn.mediaPermissions
            text: qsTr("Send Media Messages", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property var permissionNames: ["can_send_other_messages"]
            text: qsTr("Send Other Messages", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property var permissionNames: ["can_add_link_previews"]
            text: qsTr("Add Web Page Previews", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property var permissionNames: ["can_change_info"]
            text: qsTr("Change Chat Info", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property var permissionNames: ["can_invite_users"]
            text: qsTr("Invite Users", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
            onClicked: {
                chatPermissionsColumn.setChatPermissions(this)
            }
        }

        TextSwitch {
            automaticCheck: false
            property var permissionNames: ["can_pin_messages"]
            text: qsTr("Pin Messages", "member permission")
            onCheckedChanged: {busy = false;}
            checked: chatPermissionsColumn.hasPermissions(permissionNames)
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
