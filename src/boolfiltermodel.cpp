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

#include "boolfiltermodel.h"

#define DEBUG_MODULE BoolFilterModel
#include "debuglog.h"

BoolFilterModel::BoolFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
//    setFilterCaseSensitivity(Qt::CaseInsensitive);
//    setFilterFixedString(QString());
    filterValue = true;
}

void BoolFilterModel::setSource(QObject *model)
{
    setSourceModel(qobject_cast<QAbstractItemModel*>(model));
}

void BoolFilterModel::setSourceModel(QAbstractItemModel *model)
{
    if (sourceModel() != model) {
        LOG(model);
        QSortFilterProxyModel::setSourceModel(model);
        updateFilterRole();
        emit sourceChanged();
    }
}

QString BoolFilterModel::getFilterRoleName() const
{
    return filterRoleName;
}

void BoolFilterModel::setFilterRoleName(QString role)
{
    if (filterRoleName != role) {
        filterRoleName = role;
        LOG(role);
        updateFilterRole();
        emit filterRoleNameChanged();
    }
}

bool BoolFilterModel::getFilterValue() const
{
    return filterValue;
}

void BoolFilterModel::setFilterValue(bool value)
{
    if(value != filterValue) {
        filterValue = value;
        invalidateFilter();
    }
}

int BoolFilterModel::mapRowFromSource(int i, int fallbackDirection)
{
    QModelIndex myIndex = mapFromSource(sourceModel()->index(i, 0));
    LOG("mapping index" << i << "to source model:" << myIndex.row() << "valid?" << myIndex.isValid());
    if(myIndex.isValid()) {
        return myIndex.row();
    }

    if(fallbackDirection > 0) {
        int max = sourceModel()->rowCount();
        i += 1;
        while (i < max) {
            myIndex = mapFromSource(sourceModel()->index(i, 0));

            LOG("fallback ++ " << i << "to source model:" << myIndex.row() << "valid?" << myIndex.isValid());
            if(myIndex.isValid()) {
                return myIndex.row();
            }
            i += 1;
        }
    } else if(fallbackDirection < 0) {
        i -= 1;
        while (i > -1) {
            myIndex = mapFromSource(sourceModel()->index(i, 0));
            LOG("fallback -- " << i << "to source model:" << myIndex.row() << "valid?" << myIndex.isValid());
            if(myIndex.isValid()) {
                return myIndex.row();
            }
            i -= 1;
        }
    }

    return myIndex.row(); // may still be -1
}

int BoolFilterModel::mapRowToSource(int i)
{
    QModelIndex sourceIndex = mapToSource(index(i, 0));
    return sourceIndex.row();
}
bool BoolFilterModel::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
 {
//    sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data(); //.toString().contains( /*string for column 0*/ ))
//    LOG("Filter Role " <<  filterRole());
//    QModelIndex index = this->sourceModel()->index(sourceRow,1,sourceParent);
//    sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data(filterRole()).toBool();
//    LOG("Filter index DATA"<< sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data(filterRole())); //<<  index << index.isValid());
//    LOG("Filter parent " <<  sourceParent << sourceParent.isValid());
//    LOG("Filter Model Value" << sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data(filterRole()).toBool());
//    LOG("Filter Model filterValue" << filterValue);
//    LOG("Filter Model result" << (sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data(filterRole()).toBool() == filterValue));
//    LOG("Filter Model MESSAGE" << sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data());
    return sourceModel()->index(sourceRow, 0, sourceParent.child(sourceRow, 0)).data(filterRole()).toBool() == filterValue;
 }

int BoolFilterModel::findRole(QAbstractItemModel *model, QString role)
{
    if (model && !role.isEmpty()) {
        const QByteArray roleName(role.toUtf8());
        const QHash<int,QByteArray> roleMap(model->roleNames());
        const QList<int> roles(roleMap.keys());
        const int n = roles.count();
        for (int i = 0; i < n; i++) {
            const QByteArray name(roleMap.value(roles.at(i)));
            if (name == roleName) {
                LOG(role << roles.at(i));
                return roles.at(i);
            }
        }
        LOG("Unknown role" << role);
    }
    return -1;
}

void BoolFilterModel::updateFilterRole()
{
    const int role = findRole(sourceModel(), filterRoleName);
    setFilterRole((role >= 0) ? role : Qt::DisplayRole);
}
