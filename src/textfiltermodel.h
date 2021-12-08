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

#ifndef TEXTFILTERMODEL_H
#define TEXTFILTERMODEL_H

#include <QSortFilterProxyModel>

class TextFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterRoleName READ getFilterRoleName WRITE setFilterRoleName NOTIFY filterRoleNameChanged)
    Q_PROPERTY(QString filterText READ getFilterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(QObject* sourceModel READ sourceModel WRITE setSource NOTIFY sourceChanged)

public:
    TextFilterModel(QObject *parent = Q_NULLPTR);

    void setSource(QObject* model);
    void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

    QString getFilterRoleName() const;
    void setFilterRoleName(QString role);

    QString getFilterText() const;
    void setFilterText(QString text);

signals:
    void sourceChanged();
    void filterRoleNameChanged();
    void filterTextChanged();

private slots:
    void updateFilterRole();

private:
    static int findRole(QAbstractItemModel *model, QString role);

private:
    QString filterRoleName;
    QString filterText;
};

#endif // TEXTFILTERMODEL_H
