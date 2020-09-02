/*
    Copyright (C) 2020 Sebastian J. Wolf

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

#include "notificationmanager.h"

NotificationManager::NotificationManager(TDLibWrapper *tdLibWrapper, QObject *parent) : QObject(parent)
{
    qDebug() << "[NotificationManager] Initializing...";
    this->tdLibWrapper = tdLibWrapper;

    connect(this->tdLibWrapper, SIGNAL(activeNotificationsUpdated(QVariantList)), this, SLOT(handleUpdateActiveNotifications(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(notificationGroupUpdated(QVariantMap)), this, SLOT(handleUpdateNotificationGroup(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(notificationUpdated(QVariantMap)), this, SLOT(handleUpdateNotification(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
}

NotificationManager::~NotificationManager()
{
    qDebug() << "[NotificationManager] Destroying myself...";
}

void NotificationManager::handleUpdateActiveNotifications(const QVariantList notificationGroups)
{
    qDebug() << "[NotificationManager] Received active notifications, number of groups:" << notificationGroups.size();
}

void NotificationManager::handleUpdateNotificationGroup(const QVariantMap notificationGroupUpdate)
{
    qDebug() << "[NotificationManager] Received notification group update, group ID:" << notificationGroupUpdate.value("notification_group_id").toInt();
}

void NotificationManager::handleUpdateNotification(const QVariantMap updatedNotification)
{
    qDebug() << "[NotificationManager] Received notification update, group ID:" << updatedNotification.value("notification_group_id").toInt();
}

void NotificationManager::handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation)
{
    this->chatListMutex.lock();
    qDebug() << "[NotificationManager] Adding chat to internal map " << chatId;
    this->chatMap.insert(chatId, chatInformation);
    this->chatListMutex.unlock();
}
