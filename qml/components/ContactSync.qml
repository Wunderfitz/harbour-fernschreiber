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
import QtQuick 2.0
import org.nemomobile.contacts 1.0

Item {

    signal syncError();

    function synchronize() {
        if (peopleModel.count === 0) {
            appNotification.show(qsTr("Could not synchronize your contacts with Telegram."));
            syncError();
        } else {
            contactsModel.startImportingContacts();
            for (var i = 0; i < peopleModel.count; i++ ) {
                contactsModel.importContact(peopleModel.get(i));
            }
            contactsModel.stopImportingContacts();
        }
    }

    PeopleModel {
        id: peopleModel
        requiredProperty: PeopleModel.PhoneNumberRequired
    }

}
