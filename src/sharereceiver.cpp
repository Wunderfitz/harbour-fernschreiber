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

#include "sharereceiver.h"

#include <QDBusArgument>
#include <QDBusVariant>
#include <QMimeDatabase>
#include <QUrl>

#define DEBUG_MODULE ShareReceiver
#include "debuglog.h"

namespace {
    const QString RESOURCES("resources");
    const QString TYPE("type");
    const QString FILE_PATH("filePath");
    const QString NAME("name");
    const QString DATA("data");
    const QString STATUS("status");
    const QString TEXT("text");
    const QString MIME_TYPE_URL("text/x-url");
    const QString MIME_TYPE_TEXT("text/plain");

    // Nested containers of the D-Bus call arrive as QDBusArgument or
    // QDBusVariant instead of QVariantList/QVariantMap
    QVariant demarshalled(const QVariant &value)
    {
        if (value.userType() == qMetaTypeId<QDBusVariant>()) {
            return demarshalled(value.value<QDBusVariant>().variant());
        }
        if (value.userType() != qMetaTypeId<QDBusArgument>()) {
            return value;
        }
        const QDBusArgument argument = value.value<QDBusArgument>();
        switch (argument.currentType()) {
        case QDBusArgument::ArrayType: {
            QVariantList list;
            argument.beginArray();
            while (!argument.atEnd()) {
                list.append(demarshalled(argument.asVariant()));
            }
            argument.endArray();
            return list;
        }
        case QDBusArgument::MapType: {
            QVariantMap map;
            argument.beginMap();
            while (!argument.atEnd()) {
                argument.beginMapEntry();
                const QString key = argument.asVariant().toString();
                map.insert(key, demarshalled(argument.asVariant()));
                argument.endMapEntry();
            }
            argument.endMap();
            return map;
        }
        default: {
            // Structures aren't part of a share, don't recurse into them
            const QVariant inner = argument.asVariant();
            return inner.userType() == qMetaTypeId<QDBusArgument>() ? QVariant() : demarshalled(inner);
        }
        }
    }

    QVariantMap filePathResource(const QString &filePath)
    {
        QVariantMap resource;
        resource.insert(FILE_PATH, filePath);
        return resource;
    }

    QVariantMap textResource(const QString &text)
    {
        QVariantMap resource;
        resource.insert(TEXT, text);
        return resource;
    }

    // A resource as a ShareAction takes it: a file URL or path, or a map with
    // a filePath, with a name and data pair of a mime type, or - links shared
    // by the browser - with the URL as status. Plain text and links go into
    // the message field instead of a file.
    QVariantMap normalizedResource(const QVariant &resource)
    {
        if (resource.userType() != QVariant::Map) {
            const QString resourceString = resource.toString();
            const QUrl url(resourceString);
            if (url.isLocalFile()) {
                return filePathResource(url.toLocalFile());
            } else if (resourceString.startsWith("/")) {
                return filePathResource(resourceString);
            } else if (!resourceString.isEmpty()) {
                return textResource(resourceString);
            }
            return QVariantMap();
        }

        const QVariantMap resourceMap = resource.toMap();
        const QString filePath = resourceMap.value(FILE_PATH).toString();
        if (!filePath.isEmpty()) {
            return normalizedResource(filePath);
        }
        QString type = resourceMap.value(TYPE).toString();
        if (type == MIME_TYPE_URL) {
            const QString url = resourceMap.value(STATUS, resourceMap.value(DATA)).toString();
            return url.isEmpty() ? QVariantMap() : textResource(url);
        }
        if (!resourceMap.contains(DATA)) {
            return QVariantMap();
        }
        const QString name = resourceMap.value(NAME).toString();
        if (type.isEmpty()) {
            type = name.isEmpty() ? MIME_TYPE_TEXT : QMimeDatabase().mimeTypeForFile(name, QMimeDatabase::MatchExtension).name();
        }
        const QString data = resourceMap.value(DATA).toString();
        if (type == MIME_TYPE_TEXT) {
            return data.isEmpty() ? QVariantMap() : textResource(data);
        }
        QVariantMap dataResource;
        dataResource.insert(NAME, name);
        dataResource.insert(DATA, data);
        return dataResource;
    }
}

ShareReceiver::ShareReceiver(QObject *parent) : QObject(parent)
{
}

void ShareReceiver::share(const QVariantMap &configuration)
{
    const QVariantList resources = demarshalled(configuration.value(RESOURCES)).toList();
    LOG("Share received" << resources);
    QVariantList normalizedResources;
    for (const QVariant &resource : resources) {
        const QVariantMap normalized = normalizedResource(demarshalled(resource));
        if (normalized.isEmpty()) {
            WARN("Unusable shared resource" << resource);
        } else {
            normalizedResources.append(normalized);
        }
    }
    if (!normalizedResources.isEmpty()) {
        emit pleaseShare(normalizedResources);
    }
}
