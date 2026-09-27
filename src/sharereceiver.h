/*
    Copyright (C) 2026 Sebastian J. Wolf and other contributors

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

#ifndef SHARERECEIVER_H
#define SHARERECEIVER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

// Receives what other apps share with Fernschreiber through the Sailfish share
// sheet (the "file" X-Share Method of the desktop file), which calls share() at
// /share/<method>.
//
// Sailfish.Share's ShareProvider does the same in QML, but only accepts
// resources with a filePath or a name and data pair. A link shared by the
// browser has neither - {type: "text/x-url", status: <url>, linkTitle: <title>} -
// so ShareProvider drops it and the browser reports that sharing failed.
class ShareReceiver : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.sailfishos.share")

public:
    explicit ShareReceiver(QObject *parent = nullptr);

signals:
    // Each resource is a map with either a "filePath", a "text" to be put into
    // the message field, or a "name" and "data" pair to be written to a file.
    void pleaseShare(const QVariantList &resources);

public slots:
    Q_SCRIPTABLE void share(const QVariantMap &configuration);

};

#endif // SHARERECEIVER_H
