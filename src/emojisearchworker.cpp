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
#include "emojisearchworker.h"

#define LOG(x) qDebug() << "[EmojiSearchWorker]" << x

EmojiSearchWorker::~EmojiSearchWorker()
{
    LOG("Destroying myself...");
    database.close();
}

EmojiSearchWorker::EmojiSearchWorker(QObject *parent) : QThread(parent)
{
    LOG("Initializing Emoji database");
    QSqlDatabase::removeDatabase("emojis");
    database = QSqlDatabase::addDatabase("QSQLITE", "emojis");
    database.setDatabaseName("/usr/share/harbour-fernschreiber/db/emojis.db");
}

void EmojiSearchWorker::setParameters(const QString &queryString)
{
    this->queryString = queryString;
}

void EmojiSearchWorker::performSearch()
{
    LOG("Performing emoji search" << this->queryString);
    QVariantList resultList;

    if (database.open()) {
        QSqlQuery query(database);
        query.prepare("select * from emojis where description match (:queryString) limit 25");
        query.bindValue(":queryString", queryString + "*");
        query.exec();
        while (query.next()) {
            if (isInterruptionRequested()) {
                break;
            }
            QVariantMap foundEmoji;
            foundEmoji.insert("file_name", query.value(0).toString());
            foundEmoji.insert("emoji", query.value(1).toString());
            foundEmoji.insert("emoji_version", query.value(2).toString());
            foundEmoji.insert("description", query.value(3).toString());
            resultList.append(foundEmoji);
        }
        database.close();
    } else {
        LOG("Unable to perform a query on database" << database.lastError().databaseText());
    }

    emit searchCompleted(queryString, resultList);
}
