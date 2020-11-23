/*
    Copyright (C) 2020 Slava Monich et al.

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

#include "mceinterface.h"
#include <QDBusConnection>

#define DEBUG_MODULE MceInterface
#include "debuglog.h"

MceInterface::MceInterface(QObject *parent) :
    QDBusInterface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request",
    QDBusConnection::systemBus(), parent)
{
}

void MceInterface::ledPatternActivate(const QString &pattern)
{
    LOG("Activating pattern" << pattern);
    call(QStringLiteral("req_led_pattern_activate"), pattern);
}

void MceInterface::ledPatternDeactivate(const QString &pattern)
{
    LOG("Deactivating pattern" << pattern);
    call(QStringLiteral("req_led_pattern_deactivate"), pattern);
}

void MceInterface::displayCancelBlankingPause()
{
    LOG("Enabling display blanking");
    call(QStringLiteral("req_display_cancel_blanking_pause"));
}

void MceInterface::displayBlankingPause()
{
    LOG("Disabling display blanking");
    call(QStringLiteral("req_display_blanking_pause"));
}
