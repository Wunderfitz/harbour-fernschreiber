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
import QtQml.Models 2.3

import "../"
import "../../pages"
import "../../js/twemoji.js" as Emoji
import "../../js/functions.js" as Functions

ChatInformationTabItemBase {
    id: tabBase
    title: qsTr("Settings", "Button: Chat Settings")
    image: "image://theme/icon-m-developer-mode"

    SilicaFlickable {
        height: tabBase.height
        width: tabBase.width
        contentHeight: contentColumn.height
        Column {
            id: contentColumn
            width: tabBase.width

            //permissions

            // if chatInformationPage.chatInformation.permissions.can_change_info
            //  - upload/change chat photo/VIDEO (hahaha)
            //  - description change
            //  - toggleSupergroupIsAllHistoryAvailable
            // if ?????? can_promote_members ???? can_restrict_members
            // - setChatMemberStatus
            // if creator (BasicGroup)
            // - upgradeBasicGroupChatToSupergroupChat
            // if creator (supergroup/channel)
            // - canTransferOwnership?
            //   - transferChatOwnership
            Loader {
                active: (chatInformationPage.isBasicGroup || chatInformationPage.isSuperGroup) && chatInformationPage.groupInformation &&  (chatInformationPage.groupInformation.status.can_restrict_members || chatInformationPage.groupInformation.status.can_change_info || chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusCreator")
                asynchronous: true
                source: "./EditGroupChatPermissionsColumn.qml"
                width: parent.width
            }

            Loader {
                active: chatInformationPage.isSuperGroup && chatInformationPage.groupInformation &&  chatInformationPage.groupInformation.status.can_restrict_members || chatInformationPage.groupInformation.status["@type"] === "chatMemberStatusCreator"
                asynchronous: true
                source: "./EditSuperGroupSlowModeColumn.qml"
                width: parent.width
            }

        }
    }
}
