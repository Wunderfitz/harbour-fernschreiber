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

#include "dbusadaptor.h"

#include <QDebug>

DBusAdaptor::DBusAdaptor(QObject *parent): QDBusAbstractAdaptor(parent)
{

}

void DBusAdaptor::openMessage(const QString &chatId, const QString &messageId)
{
    qDebug() << "[DBusAdaptor] Open Message " << chatId << messageId;
    emit pleaseOpenMessage(chatId, messageId);
}

void DBusAdaptor::openUrl(const QStringList &arguments)
{
    qDebug() << "[DBusAdaptor] Open Url" << arguments;
    if (arguments.length() >= 1) {
        emit pleaseOpenUrl(arguments.first());
    }
}
