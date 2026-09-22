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

Dialog {
    id: addContactPage
    allowedOrientations: Orientation.All

    // 0 for someone who doesn't use Telegram (yet)
    property var userId: 0
    property string phoneNumber
    property var photoData: ({})
    // set on accept; the names the contact was stored with
    property string firstName
    property string lastName
    property bool saveToDeviceContacts: true

    readonly property bool isTelegramUser: !!userId && userId.toString() !== "0"
    readonly property bool deviceContactsSupported: deviceContactsLoader.status === Loader.Ready
    readonly property string enteredName: ( firstNameField.text.trim() + " " + lastNameField.text.trim() ).trim()

    canAccept: firstNameField.text.trim() !== ""

    Component.onCompleted: {
        // The address book is only needed from here on, and reading it takes
        // a moment - which is why it starts now and not when we accept
        deviceContactsLoader.active = true;
    }

    onAccepted: {
        firstName = firstNameField.text.trim();
        lastName = lastNameField.text.trim();
        saveToDeviceContacts = saveToDeviceContactsSwitch.checked;
        if (isTelegramUser) {
            tdLibWrapper.addContact(userId, firstName, lastName, phoneNumber, false);
        } else {
            // Without a Telegram account there is nobody to add, the phone
            // number is all Telegram can go by until that person signs up
            tdLibWrapper.importContacts([{ "first_name" : firstName, "last_name" : lastName, "phone_number" : phoneNumber }]);
        }
        if (saveToDeviceContacts && deviceContactsSupported && phoneNumber !== "") {
            deviceContactsLoader.item.saveContact(firstName, lastName, phoneNumber);
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentColumn.height

        VerticalScrollDecorator {}

        Column {
            id: contentColumn
            width: parent.width
            spacing: Theme.paddingMedium

            DialogHeader {
                title: qsTr("Add to Contacts")
                acceptText: qsTr("Add", "add contact dialog")
            }

            Row {
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                height: Math.max(contactThumbnail.height, contactColumn.height)
                spacing: Theme.paddingMedium

                ProfileThumbnail {
                    id: contactThumbnail
                    photoData: addContactPage.photoData
                    replacementStringHint: addContactPage.enteredName
                    width: Theme.itemSizeMedium
                    height: width
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    id: contactColumn
                    width: parent.width - contactThumbnail.width - parent.spacing
                    anchors.verticalCenter: parent.verticalCenter

                    Label {
                        width: parent.width
                        font.pixelSize: Theme.fontSizeMedium
                        visible: text !== ""
                        text: Emoji.emojify(addContactPage.enteredName, font.pixelSize)
                        textFormat: Text.StyledText
                        truncationMode: TruncationMode.Fade
                    }

                    Label {
                        width: parent.width
                        font.pixelSize: Theme.fontSizeSmall
                        visible: text !== ""
                        text: addContactPage.phoneNumber
                        truncationMode: TruncationMode.Fade
                        color: Theme.secondaryColor
                    }

                    Label {
                        width: parent.width
                        font.pixelSize: Theme.fontSizeSmall
                        visible: !addContactPage.isTelegramUser
                        text: qsTr("Not a Telegram user yet")
                        font.italic: true
                        truncationMode: TruncationMode.Fade
                        color: Theme.secondaryColor
                    }
                }
            }

            TextField {
                id: firstNameField
                width: parent.width
                text: addContactPage.firstName
                label: qsTr("First name", "add contact dialog")
                placeholderText: label
                errorHighlight: text.trim() === ""
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: lastNameField.focus = true
            }

            TextField {
                id: lastNameField
                width: parent.width
                text: addContactPage.lastName
                label: qsTr("Last name", "add contact dialog")
                placeholderText: label
                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: focus = false
            }

            TextSwitch {
                id: saveToDeviceContactsSwitch
                checked: addContactPage.saveToDeviceContacts
                // Without a phone number there is nothing worth storing on the device
                visible: addContactPage.deviceContactsSupported && addContactPage.phoneNumber !== ""
                text: qsTr("Save on this device as well")
                description: qsTr("Adds the contact to the address book of your device")
            }

            Item {
                width: parent.width
                height: Theme.paddingLarge
            }
        }
    }
}
