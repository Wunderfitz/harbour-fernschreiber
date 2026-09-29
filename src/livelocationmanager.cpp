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

#include "livelocationmanager.h"
#include <QDateTime>
#include <QStringList>

#define DEBUG_MODULE LiveLocationManager
#include "debuglog.h"

namespace {
    const QString CHAT_ID("chat_id");
    const QString _EXTRA("@extra");
    const QString _TYPE("@type");
    const QString SENDING_STATE("sending_state");
    const QString LATITUDE("latitude");
    const QString LONGITUDE("longitude");
    const QString HORIZONTAL_ACCURACY("horizontalAccuracy");

    // See functions.js handleErrorMessage, which keeps quiet about the updates
    const QString EXTRA_START("liveLocation:start:");
    const QString EXTRA_UPDATE("liveLocation:update:");
    const QString EXTRA_STOP("liveLocation:stop:");

    const qint64 UPDATE_INTERVAL = 30000;
    // The first fix this accurate is sent right away, not only with the next update
    const double ACCURATE_LOCATION = 20;
}

LiveLocationManager::LiveLocationManager(TDLibWrapper *tdLibWrapper, FernschreiberUtils *fernschreiberUtils, QObject *parent)
    : QObject(parent)
    , tdLibWrapper(tdLibWrapper)
    , fernschreiberUtils(fernschreiberUtils)
    , nextStartId(0)
    , latestPositionTime(0)
{
    LOG("Initializing...");
    // Also ends the live locations that expire while no position comes in
    this->updateTimer.setInterval(5000);
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateLiveLocations()));

    connect(this->fernschreiberUtils, SIGNAL(newPositionInformation(QVariantMap)), this, SLOT(handleNewPositionInformation(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(receivedMessage(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageReceived(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(messageSendSucceeded(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageSendSucceeded(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(messagesDeleted(qlonglong, QList<qlonglong>)), this, SLOT(handleMessagesDeleted(qlonglong, QList<qlonglong>)));
    connect(this->tdLibWrapper, SIGNAL(errorReceived(int, QString, QString)), this, SLOT(handleErrorReceived(int, QString, QString)));
}

void LiveLocationManager::startLiveLocation(qlonglong chatId, int livePeriod, const QVariantMap &positionInformation, qlonglong replyToMessageId)
{
    LOG("Starting live location" << chatId << livePeriod);
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const double accuracy = positionInformation.value(HORIZONTAL_ACCURACY).toDouble();

    LiveLocation liveLocation;
    liveLocation.chatId = chatId;
    liveLocation.messageId = 0;
    liveLocation.livePeriod = livePeriod;
    liveLocation.endTime = now + livePeriod * 1000LL;
    liveLocation.sent = false;
    liveLocation.lastUpdateTime = now;
    liveLocation.accurateLocationSent = accuracy > 0 && accuracy < ACCURATE_LOCATION;

    const int startId = this->nextStartId++;
    this->startingLiveLocations.insert(startId, liveLocation);
    this->latestPosition = positionInformation;
    this->latestPositionTime = now;
    this->tdLibWrapper->sendLiveLocationMessage(chatId, positionInformation.value(LATITUDE).toDouble(), positionInformation.value(LONGITUDE).toDouble(),
                                                accuracy, livePeriod, replyToMessageId, EXTRA_START + QString::number(startId));
    this->handleLiveLocationsChanged();
}

void LiveLocationManager::stopLiveLocation(qlonglong chatId, qlonglong messageId)
{
    const int index = this->indexOf(chatId, messageId);
    if (index < 0) {
        return;
    }
    LOG("Stopping live location" << chatId << messageId);
    const LiveLocation liveLocation(this->liveLocations.takeAt(index));
    if (liveLocation.sent) {
        this->tdLibWrapper->stopMessageLiveLocation(chatId, messageId, EXTRA_STOP + QString::number(chatId) + ":" + QString::number(messageId));
    } else {
        this->stoppedPendingMessageIds.insert(messageId);
    }
    this->handleLiveLocationsChanged();
}

QVariantMap LiveLocationManager::getActiveLiveLocations() const
{
    QVariantMap activeLiveLocations;
    for (const LiveLocation &liveLocation : this->liveLocations) {
        activeLiveLocations.insert(QString::number(liveLocation.chatId) + ":" + QString::number(liveLocation.messageId), liveLocation.endTime);
    }
    return activeLiveLocations;
}

void LiveLocationManager::handleNewPositionInformation(const QVariantMap &positionInformation)
{
    this->latestPosition = positionInformation;
    this->latestPositionTime = QDateTime::currentMSecsSinceEpoch();
    if (!this->liveLocations.isEmpty()) {
        this->updateLiveLocations();
    }
}

void LiveLocationManager::handleMessageReceived(qlonglong chatId, qlonglong messageId, const QVariantMap &message)
{
    const QString extra(message.value(_EXTRA).toString());
    if (!extra.startsWith(EXTRA_START)) {
        return;
    }
    const int startId = extra.mid(EXTRA_START.length()).toInt();
    if (!this->startingLiveLocations.contains(startId)) {
        return;
    }
    LiveLocation liveLocation(this->startingLiveLocations.take(startId));
    liveLocation.messageId = messageId;
    liveLocation.sent = message.value(SENDING_STATE).toMap().value(_TYPE).toString() != "messageSendingStatePending";
    LOG("Live location message created" << chatId << messageId << liveLocation.sent);
    this->liveLocations.append(liveLocation);
    this->handleLiveLocationsChanged();
}

void LiveLocationManager::handleMessageSendSucceeded(qlonglong messageId, qlonglong oldMessageId, const QVariantMap &message)
{
    const qlonglong chatId = message.value(CHAT_ID).toLongLong();
    if (this->stoppedPendingMessageIds.remove(oldMessageId)) {
        LOG("Live location was stopped while being sent" << chatId << messageId);
        this->tdLibWrapper->stopMessageLiveLocation(chatId, messageId, EXTRA_STOP + QString::number(chatId) + ":" + QString::number(messageId));
        return;
    }
    const int index = this->indexOf(chatId, oldMessageId);
    if (index >= 0) {
        LOG("Live location message sent" << chatId << messageId);
        LiveLocation &liveLocation = this->liveLocations[index];
        liveLocation.messageId = messageId;
        liveLocation.sent = true;
        this->handleLiveLocationsChanged();
    }
}

void LiveLocationManager::handleMessagesDeleted(qlonglong chatId, const QList<qlonglong> &messageIds)
{
    bool changed = false;
    for (int i = this->liveLocations.size() - 1; i >= 0; i--) {
        const LiveLocation &liveLocation = this->liveLocations.at(i);
        if (liveLocation.chatId == chatId && messageIds.contains(liveLocation.messageId)) {
            LOG("Live location message deleted" << chatId << liveLocation.messageId);
            this->liveLocations.removeAt(i);
            changed = true;
        }
    }
    if (changed) {
        this->handleLiveLocationsChanged();
    }
}

void LiveLocationManager::handleErrorReceived(int code, const QString &message, const QString &extra)
{
    if (extra.startsWith(EXTRA_START)) {
        LOG("Live location could not be sent" << code << message);
        this->startingLiveLocations.remove(extra.mid(EXTRA_START.length()).toInt());
        this->handleLiveLocationsChanged();
    } else if (extra.startsWith(EXTRA_UPDATE)) {
        // An unchanged position or too many requests don't end the live location,
        // anything else means that the message can't be edited any more
        if (message == "MESSAGE_NOT_MODIFIED" || code == 429) {
            LOG("Live location update skipped" << code << message);
            return;
        }
        const QStringList ids(extra.mid(EXTRA_UPDATE.length()).split(':'));
        if (ids.size() == 2) {
            const int index = this->indexOf(ids.at(0).toLongLong(), ids.at(1).toLongLong());
            if (index >= 0) {
                LOG("Live location can't be updated any more" << code << message);
                this->liveLocations.removeAt(index);
                this->handleLiveLocationsChanged();
            }
        }
    }
}

void LiveLocationManager::updateLiveLocations()
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    bool changed = false;
    for (int i = this->liveLocations.size() - 1; i >= 0; i--) {
        LiveLocation &liveLocation = this->liveLocations[i];
        if (now >= liveLocation.endTime) {
            // Telegram ends it on its own
            LOG("Live location expired" << liveLocation.chatId << liveLocation.messageId);
            this->liveLocations.removeAt(i);
            changed = true;
            continue;
        }
        if (!liveLocation.sent || this->latestPositionTime <= liveLocation.lastUpdateTime) {
            continue;
        }
        const double accuracy = this->latestPosition.value(HORIZONTAL_ACCURACY).toDouble();
        const bool firstAccurateLocation = !liveLocation.accurateLocationSent && accuracy > 0 && accuracy < ACCURATE_LOCATION;
        if (firstAccurateLocation || now - liveLocation.lastUpdateTime >= UPDATE_INTERVAL) {
            this->sendUpdate(liveLocation);
        }
    }
    if (changed) {
        this->handleLiveLocationsChanged();
    }
}

int LiveLocationManager::indexOf(qlonglong chatId, qlonglong messageId) const
{
    for (int i = 0; i < this->liveLocations.size(); i++) {
        const LiveLocation &liveLocation = this->liveLocations.at(i);
        if (liveLocation.chatId == chatId && liveLocation.messageId == messageId) {
            return i;
        }
    }
    return -1;
}

void LiveLocationManager::sendUpdate(LiveLocation &liveLocation)
{
    const double accuracy = this->latestPosition.value(HORIZONTAL_ACCURACY).toDouble();
    liveLocation.lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    if (accuracy > 0 && accuracy < ACCURATE_LOCATION) {
        liveLocation.accurateLocationSent = true;
    }
    this->tdLibWrapper->editMessageLiveLocation(liveLocation.chatId, liveLocation.messageId,
                                                this->latestPosition.value(LATITUDE).toDouble(), this->latestPosition.value(LONGITUDE).toDouble(),
                                                accuracy, liveLocation.livePeriod,
                                                EXTRA_UPDATE + QString::number(liveLocation.chatId) + ":" + QString::number(liveLocation.messageId));
}

void LiveLocationManager::handleLiveLocationsChanged()
{
    const bool active = !this->liveLocations.isEmpty() || !this->startingLiveLocations.isEmpty();
    this->fernschreiberUtils->setLiveGeoLocationUpdates(active);
    if (active) {
        if (!this->updateTimer.isActive()) {
            this->updateTimer.start();
        }
    } else {
        this->updateTimer.stop();
    }
    emit activeLiveLocationsChanged();
}
