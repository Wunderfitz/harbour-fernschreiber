/*
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

#ifndef CHATPERMISSIONFILTERMODEL_H
#define CHATPERMISSIONFILTERMODEL_H

#include "tdlibwrapper.h"

#include <QSortFilterProxyModel>

class ChatPermissionFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* tdlib READ getTDLibWrapper WRITE setTDLibWrapper NOTIFY tdlibChanged)
    Q_PROPERTY(QObject* sourceModel READ sourceModel WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QStringList requirePermissions READ getRequirePermissions WRITE setRequirePermissions NOTIFY requirePermissionsChanged)

public:
    ChatPermissionFilterModel(QObject *parent = Q_NULLPTR);

    TDLibWrapper *getTDLibWrapper() const;
    void setTDLibWrapper(QObject* obj);

    void setSource(QObject* model);
    void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

    void setRequirePermissions(QStringList permissions);
    QStringList getRequirePermissions() const;

signals:
    void tdlibChanged();
    void sourceChanged();
    void requirePermissionsChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;

private:
    TDLibWrapper *tdLibWrapper;
    QStringList requirePermissions;
};

#endif // CHATPERMISSIONFILTERMODEL_H
