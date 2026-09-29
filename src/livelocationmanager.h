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

#ifndef LIVELOCATIONMANAGER_H
#define LIVELOCATIONMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QSet>
#include <QTimer>
#include <QVariantMap>

#include "tdlibwrapper.h"
#include "fernschreiberutils.h"

// Keeps the live locations shared from this device up to date until they
// expire or are stopped, independent of the page that is open
class LiveLocationManager : public QObject
{
    Q_OBJECT
    // "<chat ID>:<message ID>" -> end of the live location, in ms since the epoch
    Q_PROPERTY(QVariantMap activeLiveLocations READ getActiveLiveLocations NOTIFY activeLiveLocationsChanged)
public:
    explicit LiveLocationManager(TDLibWrapper *tdLibWrapper, FernschreiberUtils *fernschreiberUtils, QObject *parent = nullptr);

    Q_INVOKABLE void startLiveLocation(qlonglong chatId, int livePeriod, const QVariantMap &positionInformation, qlonglong replyToMessageId = 0);
    Q_INVOKABLE void stopLiveLocation(qlonglong chatId, qlonglong messageId);

    QVariantMap getActiveLiveLocations() const;

signals:
    void activeLiveLocationsChanged();

private slots:
    void handleNewPositionInformation(const QVariantMap &positionInformation);
    void handleMessageReceived(qlonglong chatId, qlonglong messageId, const QVariantMap &message);
    void handleMessageSendSucceeded(qlonglong messageId, qlonglong oldMessageId, const QVariantMap &message);
    void handleMessagesDeleted(qlonglong chatId, const QList<qlonglong> &messageIds);
    void handleErrorReceived(int code, const QString &message, const QString &extra);
    void updateLiveLocations();

private:
    struct LiveLocation {
        qlonglong chatId;
        qlonglong messageId;
        int livePeriod;
        qint64 endTime;
        // Until the server confirmed the message, it can't be edited
        bool sent;
        qint64 lastUpdateTime;
        bool accurateLocationSent;
    };

    int indexOf(qlonglong chatId, qlonglong messageId) const;
    void sendUpdate(LiveLocation &liveLocation);
    void handleLiveLocationsChanged();

    TDLibWrapper *tdLibWrapper;
    FernschreiberUtils *fernschreiberUtils;
    QList<LiveLocation> liveLocations;
    // Waiting for the answer to sendMessage, which tells the message ID
    QMap<int, LiveLocation> startingLiveLocations;
    int nextStartId;
    // Stopped while still being sent, the stop follows once they are sent
    QSet<qlonglong> stoppedPendingMessageIds;
    QVariantMap latestPosition;
    qint64 latestPositionTime;
    QTimer updateTimer;
};

#endif // LIVELOCATIONMANAGER_H
