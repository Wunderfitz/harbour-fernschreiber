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

#ifndef DEBUGLOGJS_H
#define DEBUGLOGJS_H

#include <QObject>
#include <QQmlEngine>
#include <QLoggingCategory>

class DebugLogJS : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:

    DebugLogJS(QObject* parent = Q_NULLPTR) : QObject(parent), category("fernschreiber.JS") {
        enabled = category.isDebugEnabled();
    }
    static QObject* createSingleton(QQmlEngine*, QJSEngine*) { return new DebugLogJS(); }
    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) {
        if (enabled != value) {
            enabled = value;
            Q_EMIT enabledChanged();
        }
    }
Q_SIGNALS:
    void enabledChanged();
private:
    bool enabled;
    const QLoggingCategory category;
};

#endif // DEBUGLOGJS_H
