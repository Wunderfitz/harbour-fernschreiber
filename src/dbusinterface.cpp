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

#include "dbusinterface.h"

#define DEBUG_MODULE DBusInterface
#include "debuglog.h"

DBusInterface::DBusInterface(QObject *parent) : QObject(parent)
{
    LOG("Initializing D-BUS connectivity");
    this->dbusAdaptor = new DBusAdaptor(this);
    QDBusConnection sessionBusConnection = QDBusConnection::sessionBus();

    if (!sessionBusConnection.isConnected()) {
        WARN("Error connecting to D-BUS");
        return;
    }

    if (!sessionBusConnection.registerObject(PATH_NAME, this)) {
        WARN("Error registering root object to D-BUS" << sessionBusConnection.lastError().message());
        return;
    }

    if (!sessionBusConnection.registerService(INTERFACE_NAME)) {
        WARN("Error registering interface to D-BUS" << sessionBusConnection.lastError().message());
        return;
    }
}

DBusAdaptor *DBusInterface::getDBusAdaptor()
{
    return this->dbusAdaptor;
}
