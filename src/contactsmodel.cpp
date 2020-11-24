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

int ContactsModel::rowCount(const QModelIndex &) const
{
    return this->filter.isEmpty() ? this->contacts.size() : this->filteredContacts.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        return this->filter.isEmpty() ? QVariant(contacts.value(index.row())) : QVariant(filteredContacts.value(index.row())) ;
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

void ContactsModel::applyFilter(const QString &filter)
{
    LOG("Applying filter:" << filter);
    beginResetModel();
    this->filter = filter;
    this->filteredContacts.clear();
    if (!this->filter.isEmpty()) {
        QListIterator<QVariant> contactIterator(this->contacts);
        while (contactIterator.hasNext()) {
            QVariantMap contact = contactIterator.next().toMap();
            if (contact.value(LAST_NAME).toString().contains(this->filter, Qt::CaseInsensitive)) {
                this->filteredContacts.append(contact);
                continue;
            }
            if (contact.value(FIRST_NAME).toString().contains(this->filter, Qt::CaseInsensitive)) {
                this->filteredContacts.append(contact);
                continue;
            }
            if (contact.value(USERNAME).toString().contains(this->filter, Qt::CaseInsensitive)) {
                this->filteredContacts.append(contact);
                continue;
            }
        }
    }
    endResetModel();
}
