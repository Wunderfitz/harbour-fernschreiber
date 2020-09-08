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
#include <nemonotifications-qt5/notification.h>
#include <sailfishapp.h>
#include <QDebug>
#include <QListIterator>
#include <QUrl>

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
    QString notificationGroupId = notificationGroupUpdate.value("notification_group_id").toString();
    qDebug() << "[NotificationManager] Received notification group update, group ID:" << notificationGroupId;
    QVariantMap notificationGroup = this->notificationGroups.value(notificationGroupId).toMap();

    QString chatId = notificationGroupUpdate.value("chat_id").toString();
    notificationGroup.insert("type", notificationGroupUpdate.value("type"));
    notificationGroup.insert("chat_id", chatId);
    notificationGroup.insert("notification_group_id", notificationGroupId);
    notificationGroup.insert("notification_settings_chat_id", notificationGroupUpdate.value("notification_settings_chat_id"));
    notificationGroup.insert("is_silent", notificationGroupUpdate.value("is_silent"));
    notificationGroup.insert("total_count", notificationGroupUpdate.value("total_count"));

    QVariantMap activeNotifications = notificationGroup.value("notifications").toMap();

    QVariantList removedNotificationIds = notificationGroupUpdate.value("removed_notification_ids").toList();
    QListIterator<QVariant> removedNotificationsIterator(removedNotificationIds);
    while (removedNotificationsIterator.hasNext()) {
        QString removedNotificationId = removedNotificationsIterator.next().toString();
        QVariantMap notificationInformation = activeNotifications.value(removedNotificationId).toMap();
        if (!notificationInformation.isEmpty()) {
            this->removeNotification(notificationInformation);
            activeNotifications.remove(removedNotificationId);
        }
    }

    QVariantList addedNotifications = notificationGroupUpdate.value("added_notifications").toList();
    QListIterator<QVariant> addedNotificationIterator(addedNotifications);
    while (addedNotificationIterator.hasNext()) {
        QVariantMap addedNotification = addedNotificationIterator.next().toMap();
        QVariantMap activeNotification = this->sendNotification(chatId, addedNotification);
        activeNotifications.insert(activeNotification.value("id").toString(), activeNotification);
    }

    notificationGroup.insert("notifications", activeNotifications);
    this->notificationGroups.insert(notificationGroupId, notificationGroup);
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

QVariantMap NotificationManager::sendNotification(const QString &chatId, const QVariantMap &notificationInformation)
{
    qDebug() << "[NotificationManager] Sending notification" << notificationInformation.value("id").toString();

    QVariantMap chatInformation = this->chatMap.value(chatId).toMap();

    QVariantMap updatedNotificationInformation = notificationInformation;
    QUrl appIconUrl = SailfishApp::pathTo("images/fernschreiber-notification.png");
    Notification nemoNotification;
    nemoNotification.setAppName("Fernschreiber");
    nemoNotification.setAppIcon(appIconUrl.toLocalFile());
    nemoNotification.setSummary(chatInformation.value("title").toString());
    nemoNotification.setCategory("x-nemo.messaging.im");
    nemoNotification.setBody(this->getNotificationText(notificationInformation.value("type").toMap().value("message").toMap().value("content").toMap()));

    nemoNotification.publish();
    updatedNotificationInformation.insert("replaces_id", nemoNotification.replacesId());
    return updatedNotificationInformation;
}

void NotificationManager::removeNotification(const QVariantMap &notificationInformation)
{
    qDebug() << "[NotificationManager] Removing notification" << notificationInformation.value("id").toString();
    Notification nemoNotification;
    nemoNotification.setReplacesId(notificationInformation.value("replaces_id").toUInt());
    nemoNotification.close();
}

QString NotificationManager::getNotificationText(const QVariantMap &notificationContent)
{
    qDebug() << "[NotificationManager] Getting notification text from content" << notificationContent;
    return notificationContent.value("text").toMap().value("text").toString();
}
