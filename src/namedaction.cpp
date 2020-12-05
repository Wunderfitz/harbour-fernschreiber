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

#include "namedaction.h"

// A workaroud for
// ListElement: cannot use script for property value

NamedAction::NamedAction(QObject *parent) :
    QObject(parent),
    visible(true),
    action(QJSValue::UndefinedValue)
{
}

bool NamedAction::getVisible() const
{
    return visible;
}

void NamedAction::setVisible(bool newVisible)
{
    if (visible != newVisible) {
        visible = newVisible;
        emit visibleChanged();
    }
}

QString NamedAction::getName() const
{
    return name;
}

void NamedAction::setName(QString newName)
{
    if (name != newName) {
        name = newName;
        emit nameChanged();
    }
}

QJSValue NamedAction::getAction() const
{
    return action;
}

void NamedAction::setAction(QJSValue newAction)
{
    action = newAction;
    emit actionChanged();
}
