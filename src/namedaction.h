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

#ifndef NAMEDACTION_H
#define NAMEDACTION_H

#include <QObject>
#include <QString>
#include <QJSValue>

class NamedAction : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ getVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QJSValue action READ getAction WRITE setAction NOTIFY actionChanged)

public:
    NamedAction(QObject *parent = Q_NULLPTR);

    bool getVisible() const;
    void setVisible(bool newVisible);

    QString getName() const;
    void setName(QString newName);

    QJSValue getAction() const;
    void setAction(QJSValue newAction);

signals:
    void visibleChanged();
    void nameChanged();
    void actionChanged();

private:
    bool visible;
    QString name;
    QJSValue action;
};

#endif // NAMEDACTION_H
