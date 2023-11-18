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

#include "contactsmodel.h"
#include <QListIterator>

#define DEBUG_MODULE ContactsModel
#include "debuglog.h"

namespace {
    const QString STATUS("status");
    const QString ID("id");
    const QString TYPE("type");
    const QString LAST_NAME("last_name");
    const QString FIRST_NAME("first_name");
    const QString USERNAME("username");
    const QString _TYPE("@type");
    const QString _EXTRA("@extra");
}

ContactsModel::ContactsModel(TDLibWrapper *tdLibWrapper, QObject *parent)
    : QAbstractListModel(parent)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(this->tdLibWrapper, SIGNAL(usersReceived(QString, QVariantList, int)), this, SLOT(handleUsersReceived(QString, QVariantList, int)));
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(ContactRole::RoleDisplay, "display");
    roles.insert(ContactRole::RoleTitle, "title");
    roles.insert(ContactRole::RoleUserId, "user_id");
    roles.insert(ContactRole::RoleUsername, "username");
    roles.insert(ContactRole::RolePhotoSmall, "photo_small");
    roles.insert(ContactRole::RoleUserStatus, "user_status");
    roles.insert(ContactRole::RoleUserLastOnline, "user_last_online");
    roles.insert(ContactRole::RoleFilter, "filter");
    return roles;
}

int ContactsModel::rowCount(const QModelIndex &) const
{
    return this->contacts.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        QVariantMap requestedContact = contacts.value(index.row()).toMap();
        switch (static_cast<ContactRole>(role)) {
            case ContactRole::RoleDisplay: return requestedContact;
            case ContactRole::RoleTitle: return QString(requestedContact.value("first_name").toString() + " " + requestedContact.value("last_name").toString()).trimmed();
            case ContactRole::RoleUserId: return requestedContact.value("id");
            case ContactRole::RoleUsername: return requestedContact.value("usernames").toMap().value("editable_username").toString();
            case ContactRole::RolePhotoSmall: return requestedContact.value("profile_photo").toMap().value("small");
            case ContactRole::RoleUserStatus: return requestedContact.value("status").toMap().value("@type");
            case ContactRole::RoleUserLastOnline: return requestedContact.value("status").toMap().value("was_online");
            case ContactRole::RoleFilter: return QString(requestedContact.value("first_name").toString() + " " + requestedContact.value("last_name").toString() + " " + requestedContact.value("username").toString()).trimmed();
        }
    }
    return QVariant();
}

void ContactsModel::handleUsersReceived(const QString &extra, const QVariantList &userIds, int totalUsers)
{
    if (extra == "contactsRequested") {
        LOG("Received contacts list..." << totalUsers);
        this->contactIds.clear();
        QListIterator<QVariant> userIdIterator(userIds);
        while (userIdIterator.hasNext()) {
            QString nextUserId = userIdIterator.next().toString();
            if (!this->tdLibWrapper->hasUserInformation(nextUserId)) {
                this->tdLibWrapper->getUserFullInfo(nextUserId);
            }
            this->contactIds.append(nextUserId);
        }
    }
}

static bool compareUsers(const QVariant &user1, const QVariant &user2)
{
    const QVariantMap userMap1 = user1.toMap();
    const QVariantMap userMap2 = user2.toMap();

    const QString lastName1 = userMap1.value(LAST_NAME).toString();
    const QString lastName2 = userMap2.value(LAST_NAME).toString();
    if (!lastName1.isEmpty()) {
        if (lastName1 < lastName2) {
            return true;
        } else if (lastName1 > lastName2) {
            return false;
        }
    }

    const QString firstName1 = userMap1.value(FIRST_NAME).toString();
    const QString firstName2 = userMap2.value(FIRST_NAME).toString();
    if (firstName1 < firstName2) {
        return true;
    } else if (firstName1 > firstName2) {
        return false;
    }
    const QString userName1 = userMap1.value(USERNAME).toString();
    const QString userName2 = userMap2.value(USERNAME).toString();
    if (userName1 < userName2) {
        return true;
    } else if (userName1 > userName2) {
        return false;
    }
    return userMap1.value(ID).toLongLong() < userMap2.value(ID).toLongLong();
}

void ContactsModel::hydrateContacts()
{
    LOG("Hydrating contacts...");
    this->contacts.clear();
    QListIterator<QString> userIdIterator(contactIds);
    while (userIdIterator.hasNext()) {
        QString nextUserId = userIdIterator.next();
        LOG("Hydrating contact:" << nextUserId);
        this->contacts.append(this->tdLibWrapper->getUserInformation(nextUserId));
    }
    LOG("Hydrated contacts:" << this->contacts.size());
    std::sort(this->contacts.begin(), this->contacts.end(), compareUsers);
}

void ContactsModel::startImportingContacts()
{
    this->deviceContacts.clear();
}

void ContactsModel::stopImportingContacts()
{
    if (!deviceContacts.isEmpty()) {
        LOG("Importing found contacts" << deviceContacts.size());
        this->tdLibWrapper->importContacts(deviceContacts);
    }
}

void ContactsModel::importContact(const QVariantMap &singlePerson)
{
    QString firstName = singlePerson.value("firstName").toString();
    QVariantList phoneNumbers = singlePerson.value("phoneNumbers").toList();
    if (!firstName.isEmpty() && !phoneNumbers.isEmpty()) {
        for (QVariant phoneNumber : phoneNumbers) {
            QVariantMap singleContact;
            singleContact.insert("first_name", firstName);
            singleContact.insert("last_name", singlePerson.value("lastName").toString());
            singleContact.insert("phone_number", phoneNumber.toString());
            deviceContacts.append(singleContact);
            LOG("Found contact" << singleContact.value("first_name").toString() << singleContact.value("last_name").toString() << singleContact.value("phone_number").toString());
        }
    }
}
