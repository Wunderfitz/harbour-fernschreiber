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

#include "chatpermissionfiltermodel.h"
#include "chatlistmodel.h"

#define DEBUG_MODULE ChatPermissionFilterModel
#include "debuglog.h"

namespace {
    const QString PERMISSIONS("permissions");
    const QString STATUS("status");
}

ChatPermissionFilterModel::ChatPermissionFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void ChatPermissionFilterModel::setSource(QObject *model)
{
    setSourceModel(qobject_cast<ChatListModel*>(model));
}

void ChatPermissionFilterModel::setSourceModel(QAbstractItemModel *model)
{
    if (sourceModel() != model) {
        LOG(model);
        QSortFilterProxyModel::setSourceModel(model);
        emit sourceChanged();
    }
}

TDLibWrapper *ChatPermissionFilterModel::getTDLibWrapper() const
{
    return tdLibWrapper;
}

void ChatPermissionFilterModel::setTDLibWrapper(QObject *obj)
{
    TDLibWrapper *wrapper = qobject_cast<TDLibWrapper*>(obj);
    if (tdLibWrapper != wrapper) {
        tdLibWrapper = wrapper;
        LOG(wrapper);
        invalidateFilter();
    }
}

QStringList ChatPermissionFilterModel::getRequirePermissions() const
{
    return requirePermissions;
}

void ChatPermissionFilterModel::setRequirePermissions(QStringList permissions)
{
    if (requirePermissions != permissions) {
        requirePermissions = permissions;
        LOG(requirePermissions);
        invalidateFilter();
        emit requirePermissionsChanged();
    }
}

bool ChatPermissionFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QAbstractItemModel* model = sourceModel();
    if (model && tdLibWrapper && !requirePermissions.isEmpty()) {
        const TDLibWrapper::Group* group = Q_NULLPTR;
        const QModelIndex index(model->index(sourceRow, 0, sourceParent));
        TDLibWrapper::ChatType chatType = (TDLibWrapper::ChatType)
            model->data(index, ChatListModel::RoleChatType).toInt();

        switch (chatType) {
        case TDLibWrapper::ChatTypeUnknown:
            return false;
        case TDLibWrapper::TDLibWrapper::ChatTypePrivate:
        case TDLibWrapper::TDLibWrapper::ChatTypeSecret:
            return true;
        case TDLibWrapper::TDLibWrapper::ChatTypeBasicGroup:
        case TDLibWrapper::TDLibWrapper::ChatTypeSupergroup:
            group = tdLibWrapper->getGroup(model->data(index,
                ChatListModel::RoleGroupId).toLongLong());
            break;
        }

        if (group) {
            TDLibWrapper::ChatMemberStatus memberStatus = (TDLibWrapper::ChatMemberStatus)
                model->data(index, ChatListModel::RoleChatMemberStatus).toInt();
            QVariantMap permissions;

            switch (memberStatus) {
            case TDLibWrapper::ChatMemberStatusCreator:
            case TDLibWrapper::ChatMemberStatusAdministrator:
            case TDLibWrapper::ChatMemberStatusMember:
                permissions = model->data(index, ChatListModel::RoleDisplay).toMap().value(PERMISSIONS).toMap();
                break;
            case TDLibWrapper::ChatMemberStatusRestricted:
                permissions = group->groupInfo.value(STATUS).toMap().value(PERMISSIONS).toMap();
                break;
            case TDLibWrapper::ChatMemberStatusLeft:
            case TDLibWrapper::ChatMemberStatusUnknown:
            case TDLibWrapper::ChatMemberStatusBanned:
                return false;
            }

            if (!permissions.isEmpty()) {
                const int n = requirePermissions.count();
                for (int i = 0; i < n; i++) {
                    if (permissions.value(requirePermissions.at(i)).toBool()) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
