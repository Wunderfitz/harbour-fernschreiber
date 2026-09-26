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
import QtQuick 2.0
import org.nemomobile.contacts 1.0
import "../js/debug.js" as Debug

// Writes a contact to the address book of the device. Whoever asks for that
// may be gone long before the contact manager is done, which is why this
// lives in the application window and not in the page that requests a save.

Item {

    id: deviceContacts

    // A contact waiting for the contact cache to be populated
    property var pendingContact: null
    // Kept alive until the next save, the contact manager may still use it
    property var lastSavedPerson: null

    function saveContact(firstName, lastName, phoneNumber) {
        pendingContact = { "firstName" : firstName, "lastName" : lastName, "phoneNumber" : phoneNumber };
        if (peopleModel.populated) {
            savePendingContact();
        } else {
            Debug.log("[DeviceContacts] Contacts not available yet, waiting for them...");
        }
    }

    function savePendingContact() {
        var contact = pendingContact;
        pendingContact = null;
        if (!contact) {
            return;
        }
        if (peopleModel.personByPhoneNumber(contact.phoneNumber, false)) {
            Debug.log("[DeviceContacts] A contact with this phone number exists already");
            appNotification.show(qsTr("%1 is already in the contacts of your device.").arg(contact.phoneNumber));
            return;
        }
        if (lastSavedPerson) {
            lastSavedPerson.destroy();
        }
        lastSavedPerson = personComponent.createObject(deviceContacts);
        lastSavedPerson.firstName = contact.firstName;
        lastSavedPerson.lastName = contact.lastName;
        lastSavedPerson.phoneDetails = [{
            'type' : Person.PhoneNumberType,
            'subTypes' : [ Person.NoSubType ],
            'label' : Person.NoLabel,
            'number' : contact.phoneNumber,
            'index' : -1
        }];
        Debug.log("[DeviceContacts] Saving contact on this device...");
        if (!peopleModel.savePerson(lastSavedPerson)) {
            handleSaveFailed();
        }
    }

    function handleSaveFailed() {
        Debug.log("[DeviceContacts] Saving the contact on this device failed");
        appNotification.show(qsTr("Could not save the contact on your device."));
    }

    PeopleModel {
        id: peopleModel

        onPopulatedChanged: {
            if (populated) {
                deviceContacts.savePendingContact();
            }
        }
        onSavePersonSucceeded: {
            Debug.log("[DeviceContacts] Contact saved on this device");
        }
        onSavePersonFailed: {
            deviceContacts.handleSaveFailed();
        }
    }

    Component {
        id: personComponent
        Person {}
    }

}
