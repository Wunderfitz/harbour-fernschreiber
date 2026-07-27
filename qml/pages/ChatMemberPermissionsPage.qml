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

Dialog {
    id: memberPermissionsDialog
    allowedOrientations: Orientation.All

    property string chatId
    property string memberUserId
    property string userName
    property var memberStatus: ({})
    property var defaultPermissions: ({})
    // set on accept; the new chatMemberStatus that was sent to TDLib
    property var resultStatus: null

    readonly property bool isRestricted: memberStatus["@type"] === "chatMemberStatusRestricted"

    // key: granular chatPermissions field (TDLib >= 1.8.11),
    // legacy: pre-granular fallback used when reading older TDLib data
    readonly property var permissionItems: [
        { key: "can_send_basic_messages", legacy: "can_send_messages", label: qsTr("Send Text Messages", "member permission") },
        { key: "can_send_photos", legacy: "can_send_media_messages", label: qsTr("Send Photos", "member permission") },
        { key: "can_send_videos", legacy: "can_send_media_messages", label: qsTr("Send Videos", "member permission") },
        { key: "can_send_audios", legacy: "can_send_media_messages", label: qsTr("Send Music & Audio Files", "member permission") },
        { key: "can_send_documents", legacy: "can_send_media_messages", label: qsTr("Send Files", "member permission") },
        { key: "can_send_voice_notes", legacy: "can_send_media_messages", label: qsTr("Send Voice Messages", "member permission") },
        { key: "can_send_video_notes", legacy: "can_send_media_messages", label: qsTr("Send Video Messages", "member permission") },
        { key: "can_send_polls", legacy: "can_send_polls", label: qsTr("Send Polls", "member permission") },
        { key: "can_send_other_messages", legacy: "can_send_other_messages", label: qsTr("Send Stickers, GIFs & Games", "member permission") },
        { key: "can_add_web_page_previews", legacy: "can_add_web_page_previews", label: qsTr("Add Web Page Previews", "member permission") },
        { key: "can_change_info", legacy: "can_change_info", label: qsTr("Change Chat Info", "member permission") },
        { key: "can_invite_users", legacy: "can_invite_users", label: qsTr("Invite Users", "member permission") },
        { key: "can_pin_messages", legacy: "can_pin_messages", label: qsTr("Pin Messages", "member permission") }
    ]

    function lookupPermission(permissions, item) {
        if (!permissions) {
            return true;
        }
        if (permissions[item.key] !== undefined) {
            return permissions[item.key];
        }
        if (permissions[item.legacy] !== undefined) {
            return permissions[item.legacy];
        }
        return true;
    }
    function chatAllows(item) {
        return lookupPermission(defaultPermissions, item);
    }
    function memberHas(item) {
        return isRestricted ? lookupPermission(memberStatus.permissions, item) : chatAllows(item);
    }

    onAccepted: {
        var permissions = { "@type": "chatPermissions" };
        var fullyUnrestricted = true;
        for (var i = 0; i < permissionItems.length; i++) {
            var permissionSwitch = permissionRepeater.itemAt(i);
            permissions[permissionItems[i].key] = permissionSwitch.checked;
            // a switch disabled by the chat's default permissions can't grant
            // anything anyway, so it doesn't keep the member "restricted"
            if (permissionSwitch.enabled && !permissionSwitch.checked) {
                fullyUnrestricted = false;
            }
        }
        if (isRestricted && memberStatus.permissions && memberStatus.permissions.can_manage_topics !== undefined) {
            permissions.can_manage_topics = memberStatus.permissions.can_manage_topics;
        }
        resultStatus = fullyUnrestricted ? { "@type": "chatMemberStatusMember" }
                                         : { "@type": "chatMemberStatusRestricted",
                                             is_member: true,
                                             restricted_until_date: 0,
                                             permissions: permissions };
        tdLibWrapper.setChatMemberStatus(chatId, memberUserId, resultStatus);
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            width: parent.width

            DialogHeader {
                title: memberPermissionsDialog.userName
                acceptText: qsTr("Save", "member permissions dialog")
            }

            SectionHeader {
                text: qsTr("What can this member do?", "member permissions dialog")
            }

            Repeater {
                id: permissionRepeater
                model: memberPermissionsDialog.permissionItems

                TextSwitch {
                    text: modelData.label
                    description: enabled ? "" : qsTr("Not allowed by the group's default permissions", "member permissions dialog")
                    checked: memberPermissionsDialog.memberHas(modelData)
                    enabled: memberPermissionsDialog.chatAllows(modelData)
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
