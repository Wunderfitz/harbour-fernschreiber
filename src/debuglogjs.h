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

#ifndef DEBUGLOG_H
#define DEBUGLOG_H


#include <QObject>
#include <QQmlEngine>

class DebugLogJS : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:

#ifdef QT_QML_DEBUG
    DebugLogJS(QObject* parent = Q_NULLPTR) : QObject(parent), enabled(true) {}
#else
    DebugLog(QObject* parent = Q_NULLPTR) : QObject(parent), enabled(false) {}
#endif
    static QObject* createSingleton(QQmlEngine*, QJSEngine*) { return new DebugLogJS(); }
    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) {
        if (enabled != value) {
            enabled = value;
            Q_EMIT enabledChanged(value);
        }
    }
Q_SIGNALS:
    void enabledChanged(bool value);
private:
    bool enabled;
};

#endif // DEBUGLOG_H
