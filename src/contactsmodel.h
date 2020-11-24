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

#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

#include "tdlibwrapper.h"

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ContactsModel(TDLibWrapper *tdLibWrapper, QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void hydrateContacts();
    Q_INVOKABLE void applyFilter(const QString &filter);

public slots:
    void handleUsersReceived(const QString &extra, const QVariantList &userIds, int totalUsers);

private:
    TDLibWrapper *tdLibWrapper;
    QVariantList contacts;
    QVariantList filteredContacts;
    QList<QString> contactIds;
    QString filter;
};

#endif // CONTACTSMODEL_H
