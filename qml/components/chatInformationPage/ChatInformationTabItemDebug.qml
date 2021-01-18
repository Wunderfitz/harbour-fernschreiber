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
    title: "debug"
    image: "image://theme/icon-m-diagnostic"
    SilicaFlickable {
        height: tabBase.height
        width: tabBase.width
        contentHeight: contentColumn.height
        Column {
            id: contentColumn
            width: tabBase.width - Theme.horizontalPageMargin * 2
            x: Theme.horizontalPageMargin

            InformationTextItem {
                headerText: "chatInformation"
                text:chatInformationPage.chatInformation ?  JSON.stringify(chatInformationPage.chatInformation, null, 2) : ""
                isLinkedLabel: true
            }
            InformationTextItem {
                headerText: "groupInformation"
                text: chatInformationPage.groupInformation ? JSON.stringify(chatInformationPage.groupInformation, null, 2) : ""
                isLinkedLabel: true
            }

            InformationTextItem {
                headerText: "groupFullInformation"
                text: chatInformationPage.groupFullInformation ? JSON.stringify(chatInformationPage.groupFullInformation, null, 2) : ""
                isLinkedLabel: true
            }
        }
    }
}
