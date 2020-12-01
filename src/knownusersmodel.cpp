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

#include "knownusersmodel.h"

#define DEBUG_MODULE KnwonUsersModel
#include "debuglog.h"

KnownUsersModel::KnownUsersModel(TDLibWrapper *tdLibWrapper, QObject *parent)
    : QAbstractListModel(parent)
{
    this->tdLibWrapper = tdLibWrapper;

    connect(this->tdLibWrapper, SIGNAL(userUpdated(QString, QVariantMap)), this, SLOT(handleUserUpdated(QString, QVariantMap)));
}

QHash<int, QByteArray> KnownUsersModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(KnownUserRole::RoleDisplay, "display");
    roles.insert(KnownUserRole::RoleUserId, "user_id");
    roles.insert(KnownUserRole::RoleTitle, "title");
    roles.insert(KnownUserRole::RoleUsername, "user_name");
    roles.insert(KnownUserRole::RoleUserHandle, "user_handle");
    roles.insert(KnownUserRole::RolePhotoSmall, "photo_small");
    roles.insert(KnownUserRole::RoleFilter, "filter");
    return roles;
}

int KnownUsersModel::rowCount(const QModelIndex &) const
{
    return this->knownUsers.size();
}

QVariant KnownUsersModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        QVariantMap requestedUser = knownUsers.values().value(index.row()).toMap();
        switch (static_cast<KnownUserRole>(role)) {
            case KnownUserRole::RoleDisplay: return requestedUser;
            case KnownUserRole::RoleUserId: return requestedUser.value("id");
            case KnownUserRole::RoleTitle: return QString(requestedUser.value("first_name").toString() + " " + requestedUser.value("last_name").toString()).trimmed();
            case KnownUserRole::RoleUsername: return requestedUser.value("username");
            case KnownUserRole::RoleUserHandle: return QString("@" + (requestedUser.value("username").toString().isEmpty() ? requestedUser.value("id").toString() : requestedUser.value("username").toString()));
            case KnownUserRole::RolePhotoSmall: return requestedUser.value("profile_photo").toMap().value("small");
            case KnownUserRole::RoleFilter: return  QString(requestedUser.value("first_name").toString() + " " + requestedUser.value("last_name").toString() + " " + requestedUser.value("username").toString()).trimmed();
        }
    }
    return QVariant();
}

void KnownUsersModel::handleUserUpdated(const QString &userId, const QVariantMap &userInformation)
{
    this->knownUsers.insert(userId, userInformation);
}
