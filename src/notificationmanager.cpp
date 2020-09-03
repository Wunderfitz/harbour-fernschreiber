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

    notificationGroup.insert("type", notificationGroupUpdate.value("type"));
    notificationGroup.insert("chat_id", notificationGroupUpdate.value("chat_id"));
    notificationGroup.insert("notification_settings_chat_id", notificationGroupUpdate.value("notification_settings_chat_id"));
    notificationGroup.insert("is_silent", notificationGroupUpdate.value("is_silent"));
    notificationGroup.insert("total_count", notificationGroupUpdate.value("total_count"));

    QVariantMap notifications = notificationGroup.value("notifications").toMap();
    QVariantList addedNotifications = notificationGroupUpdate.value("added_notifications").toList();
    QListIterator<QVariant> addedNotificationIterator(addedNotifications);
    while (addedNotificationIterator.hasNext()) {
        QVariantMap addedNotification = addedNotificationIterator.next().toMap();
        notifications.insert(addedNotification.value("id").toString(), addedNotification);
    }
    QVariantList removedNotifications = notificationGroupUpdate.value("removed_notification_ids").toList();
    QListIterator<QVariant> removedNotificationIterator(removedNotifications);
    while (removedNotificationIterator.hasNext()) {
        QString removedNotificationId = removedNotificationIterator.next().toString();
        notifications.remove(removedNotificationId);
    }
    notificationGroup.insert("notifications", notifications);
    this->notificationGroups.insert(notificationGroupId, notificationGroup);
    // this->sendNotifications();
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

void NotificationManager::sendNotifications()
{
    QVariantList notificationGroupList = this->notificationGroups.values();
    QListIterator<QVariant> notificationGroupIterator(notificationGroupList);
    QUrl appIconUrl = SailfishApp::pathTo("images/fernschreiber-notification.png");
    while (notificationGroupIterator.hasNext()) {
        QVariantMap notificationGroup = notificationGroupIterator.next().toMap();
        Notification nemoNotification;
        nemoNotification.setAppName("Fernschreiber");
        nemoNotification.setAppIcon(appIconUrl.toLocalFile());
        nemoNotification.setBody("This is the body");
        nemoNotification.setSummary("This is the summary");
        nemoNotification.setPreviewSummary("This is the preview summary");
        nemoNotification.setPreviewBody("This is the preview body");
        nemoNotification.publish();
    }
}
