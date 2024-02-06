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

#ifndef BOOLFILTERMODEL_H
#define BOOLFILTERMODEL_H

#include <QSortFilterProxyModel>

class BoolFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterRoleName READ getFilterRoleName WRITE setFilterRoleName NOTIFY filterRoleNameChanged)
    Q_PROPERTY(bool filterValue READ getFilterValue WRITE setFilterValue NOTIFY filterValueChanged)
    Q_PROPERTY(QObject* sourceModel READ sourceModel WRITE setSource NOTIFY sourceChanged)

public:
    BoolFilterModel(QObject *parent = Q_NULLPTR);

    void setSource(QObject* model);
    void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;


    QString getFilterRoleName() const;
    void setFilterRoleName(QString role);

    bool getFilterValue() const;
    void setFilterValue(bool value);
    Q_INVOKABLE int mapRowFromSource(int i, int fallbackDirection);
    Q_INVOKABLE int mapRowToSource(int i);

signals:
    void sourceChanged();
    void filterRoleNameChanged();
    void filterValueChanged();

private slots:
    void updateFilterRole();

private:
    static int findRole(QAbstractItemModel *model, QString role);

private:
    QString filterRoleName;
    bool filterValue;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // BOOLFILTERMODEL_H
