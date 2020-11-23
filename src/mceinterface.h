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

#ifndef MCE_INTERFACE_H
#define MCE_INTERFACE_H

#include <QDBusInterface>

class MceInterface : public QDBusInterface
{
public:
    MceInterface(QObject *parent = Q_NULLPTR);

    void ledPatternActivate(const QString &pattern);
    void ledPatternDeactivate(const QString &pattern);
    void displayCancelBlankingPause();
    void displayBlankingPause();
};

#endif // MCE_INTERFACE_H

