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
import "../"
import "../../js/functions.js" as Functions
import "../../js/twemoji.js" as Emoji
import "../../js/debug.js" as Debug

MessageContentBase {
    id: contactMessageComponent
    height: contactRow.height

    readonly property var contactData: rawMessage.content.contact
    readonly property string contactUserId: contactData.user_id ? contactData.user_id.toString() : "0"
    readonly property bool isTelegramUser: contactUserId !== "0"
    readonly property bool isOwnMessage: messageListItem ? messageListItem.isOwnMessage : overlayFlickable.isOwnMessage
    readonly property string contactName: Functions.getUserName(contactData)
    readonly property string contactPhoneNumber: Functions.getPhoneNumber(contactData.phone_number)
    readonly property string contactUserName: Functions.getUserNameTag(contactUserInformation)
    readonly property bool isKnownContact: !!contactUserInformation.is_contact
    readonly property bool isMyself: isTelegramUser && contactUserId === (tdLibWrapper.getUserInformation().id || "").toString()
    readonly property bool canAddContact: !isKnownContact && !isMyself && ( isTelegramUser || contactPhoneNumber !== "" )

    property var contactUserInformation: isTelegramUser ? tdLibWrapper.getUserInformation(contactUserId) : ({})
    property bool addContactRequested
    // the name the contact is being stored with, as entered in the dialog
    property string addedContactName

    property list<NamedAction> extraContextMenuItems: [
        NamedAction {
            visible: contactMessageComponent.canAddContact
            name: qsTr("Add to Contacts")
            action: function () { contactMessageComponent.addToContacts() }
        }
    ]

    function addToContacts() {
        var addContactDialog = pageStack.push(Qt.resolvedUrl("../../pages/AddContactPage.qml"), {
                                                  "userId" : contactData.user_id || 0,
                                                  "firstName" : contactData.first_name || "",
                                                  "lastName" : contactData.last_name || "",
                                                  "phoneNumber" : contactPhoneNumber,
                                                  "photoData" : (typeof contactUserInformation.profile_photo !== "undefined") ? contactUserInformation.profile_photo.small : ({})
                                              });
        addContactDialog.accepted.connect(function() {
            // The dialog sends the request itself, we only report how it went
            Debug.log("[MessageContact] Contact is being added to Telegram");
            contactMessageComponent.addedContactName = Functions.getUserName({ "first_name" : addContactDialog.firstName, "last_name" : addContactDialog.lastName });
            contactMessageComponent.addContactRequested = true;
        });
    }

    onClicked: {
        if (isTelegramUser) {
            tdLibWrapper.createPrivateChat(contactUserId, "openDirectly");
        } else if (contactPhoneNumber !== "") {
            Qt.openUrlExternally("tel:" + contactPhoneNumber);
        }
    }

    Connections {
        target: tdLibWrapper
        onUserUpdated: {
            if (userId === contactMessageComponent.contactUserId) {
                contactMessageComponent.contactUserInformation = userInformation;
            }
        }
        onOkReceived: {
            if (request === "addContact" && contactMessageComponent.addContactRequested) {
                contactMessageComponent.addContactRequested = false;
                contactMessageComponent.contactUserInformation = tdLibWrapper.getUserInformation(contactMessageComponent.contactUserId);
                appNotification.show(qsTr("%1 was added to your contacts.").arg(contactMessageComponent.addedContactName));
            }
        }
        onContactsImported: {
            if (contactMessageComponent.addContactRequested) {
                contactMessageComponent.addContactRequested = false;
                appNotification.show(qsTr("%1 was added to your contacts.").arg(contactMessageComponent.addedContactName));
            }
        }
        onErrorReceived: {
            if (extra === "addContact") {
                contactMessageComponent.addContactRequested = false;
            }
        }
    }

    Row {
        id: contactRow
        width: parent.width
        height: Math.max(contactThumbnail.height, contactColumn.height)
        spacing: Theme.paddingMedium

        ProfileThumbnail {
            id: contactThumbnail
            photoData: (typeof contactMessageComponent.contactUserInformation.profile_photo !== "undefined") ? contactMessageComponent.contactUserInformation.profile_photo.small : ({})
            replacementStringHint: contactMessageComponent.contactName
            width: Theme.itemSizeMedium
            height: width
            highlighted: contactMessageComponent.highlighted
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            id: contactColumn
            width: parent.width - contactThumbnail.width - parent.spacing
            anchors.verticalCenter: parent.verticalCenter

            Label {
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
                font.bold: true
                text: Emoji.emojify(contactMessageComponent.contactName, font.pixelSize)
                textFormat: Text.StyledText
                truncationMode: TruncationMode.Fade
                color: contactMessageComponent.isOwnMessage || contactMessageComponent.highlighted ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                visible: text !== ""
                text: contactMessageComponent.contactPhoneNumber
                truncationMode: TruncationMode.Fade
                color: contactMessageComponent.isOwnMessage || contactMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }

            Label {
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                visible: text !== ""
                text: contactMessageComponent.contactUserName
                truncationMode: TruncationMode.Fade
                color: contactMessageComponent.isOwnMessage || contactMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }

            Label {
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                visible: !contactMessageComponent.isTelegramUser
                text: qsTr("Not a Telegram user yet")
                font.italic: true
                truncationMode: TruncationMode.Fade
                color: contactMessageComponent.isOwnMessage || contactMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }
        }
    }
}
