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
#ifndef EMOJISEARCHWORKER_H
#define EMOJISEARCHWORKER_H

#include <QThread>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>

class EmojiSearchWorker : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        performSearch();
    }
public:
    ~EmojiSearchWorker() override;
    explicit EmojiSearchWorker(QObject *parent = nullptr);
    void setParameters(const QString &queryString);

signals:
    void searchCompleted(const QString &queryString, const QVariantList &resultList);

public slots:

private:
    QSqlDatabase database;
    QString queryString;

    void performSearch();
};

#endif // EMOJISEARCHWORKER_H
