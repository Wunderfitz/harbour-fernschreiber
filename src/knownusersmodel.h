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

#ifndef KNOWNUSERSMODEL_H
#define KNOWNUSERSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "tdlibwrapper.h"

class KnownUsersModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum KnownUserRole {
        RoleDisplay = Qt::DisplayRole,
        RoleUserId,
        RoleTitle,
        RoleUsername,
        RoleUserHandle,
        RolePhotoSmall,
        RoleFilter
    };

    KnownUsersModel(TDLibWrapper *tdLibWrapper, QObject *parent = nullptr);

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual int rowCount(const QModelIndex &) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void handleUserUpdated(const QString &userId, const QVariantMap &userInformation);

private:
    TDLibWrapper *tdLibWrapper;
    QVariantMap knownUsers;

};

#endif // KNOWNUSERSMODEL_H
