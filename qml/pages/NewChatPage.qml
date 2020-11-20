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
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: newChatPage
    allowedOrientations: Orientation.All

    property var contacts;
    property bool isLoading: true;

    onStatusChanged: {
        if (status === PageStatus.Active) {
            newChatPage.contacts = tdLibWrapper.getContactsFullInfo();
        }
    }

    SilicaFlickable {
        id: newChatContainer
        contentHeight: newChatPage.height
        anchors.fill: parent

        Column {
            id: newChatPageColumn
            width: newChatPage.width
            height: newChatPage.height

            PageHeader {
                id: newChatPageHeader
                title: qsTr("Your Contacts")
            }

            SilicaListView {
                id: contactsListView
                model: newChatPage.contacts
                clip: true
                height: newChatPageColumn.height - newChatPageHeader.height
                width: newChatPageColumn.width
                //opacity: newChatPage.isLoading ? 0 : 1
                Behavior on opacity { FadeAnimation {} }

                ViewPlaceholder {
                    y: Theme.paddingLarge
                    enabled: contactsListView.count === 0
                    text: qsTr("You don't have any contacts.")
                }

                delegate: PhotoTextsListItem {
                    pictureThumbnail {
                        photoData: (typeof modelData.profile_photo !== "undefined") ? modelData.profile_photo.small : ""
                    }
                    width: parent.width

                    primaryText.text: Emoji.emojify(Functions.getUserName(modelData), primaryText.font.pixelSize, "../js/emoji/")
                    prologSecondaryText.text: "@" + ( modelData.username !== "" ? modelData.username : modelData.id )
//                    secondaryText {
//                        horizontalAlignment: Text.AlignRight
//                        property string statusText: Functions.getChatMemberStatusText(model.status["@type"])
//                        property string customText: model.status.custom_title ? Emoji.emojify(model.status.custom_title, secondaryText.font.pixelSize, "../js/emoji/") : ""
//                        text: (statusText !== "" && customText !== "") ? statusText + ", " + customText : statusText + customText
//                    }
                    tertiaryText {
                        maximumLineCount: 1
                        text: Functions.getChatPartnerStatusText(modelData.status["@type"], modelData.status.was_online);
                        elide: Text.ElideRight
                    }

                    onClicked: {
                        tdLibWrapper.createPrivateChat(modelData.id);
                    }
                }

                VerticalScrollDecorator {}
            }
        }

    }
}
