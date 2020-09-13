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
#include <QDateTime>
#include <QDBusConnection>
#include <QDBusInterface>

NotificationManager::NotificationManager(TDLibWrapper *tdLibWrapper, QObject *parent) : QObject(parent)
{
    qDebug() << "[NotificationManager] Initializing...";
    this->tdLibWrapper = tdLibWrapper;
    this->ngfClient = new Ngf::Client(this);

    connect(this->tdLibWrapper, SIGNAL(activeNotificationsUpdated(QVariantList)), this, SLOT(handleUpdateActiveNotifications(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(notificationGroupUpdated(QVariantMap)), this, SLOT(handleUpdateNotificationGroup(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(notificationUpdated(QVariantMap)), this, SLOT(handleUpdateNotification(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
    connect(this->ngfClient, SIGNAL(connectionStatus(bool)), this, SLOT(handleNgfConnectionStatus(bool)));
    connect(this->ngfClient, SIGNAL(eventCompleted(quint32)), this, SLOT(handleNgfEventCompleted(quint32)));
    connect(this->ngfClient, SIGNAL(eventFailed(quint32)), this, SLOT(handleNgfEventFailed(quint32)));
    connect(this->ngfClient, SIGNAL(eventPlaying(quint32)), this, SLOT(handleNgfEventPlaying(quint32)));

    if (this->ngfClient->connect()) {
        qDebug() << "[NotificationManager] NGF Client successfully initialized...";
    } else {
        qDebug() << "[NotificationManager] Failed to initialize NGF Client...";
    }

    this->controlLedNotification(false);

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

    // If we have deleted notifications, we need to update possibly existing ones
    if (!removedNotificationIds.isEmpty() && !activeNotifications.isEmpty()) {
        qDebug() << "[NotificationManager] Some removals happend, but we have " << activeNotifications.size() << "existing notifications.";
        QVariantMap firstActiveNotification = activeNotifications.first().toMap();
        activeNotifications.remove(firstActiveNotification.value("id").toString());
        QVariantMap newFirstActiveNotification = this->sendNotification(chatId, firstActiveNotification, activeNotifications);
        QVariantMap newActiveNotifications;
        QListIterator<QVariant> activeNotificationsIterator(activeNotifications.values());
        while (activeNotificationsIterator.hasNext()) {
            QVariantMap newActiveNotification = activeNotificationsIterator.next().toMap();
            newActiveNotification.insert("replaces_id", newFirstActiveNotification.value("replaces_id"));
            newActiveNotifications.insert(newActiveNotification.value("id").toString(), newActiveNotification);
        }
        newActiveNotifications.insert(newFirstActiveNotification.value("id").toString(), newFirstActiveNotification);
        activeNotifications = newActiveNotifications;
    }

    if (activeNotifications.isEmpty()) {
        this->controlLedNotification(false);
    }

    QVariantList addedNotifications = notificationGroupUpdate.value("added_notifications").toList();
    QListIterator<QVariant> addedNotificationIterator(addedNotifications);
    while (addedNotificationIterator.hasNext()) {
        QVariantMap addedNotification = addedNotificationIterator.next().toMap();
        QVariantMap activeNotification = this->sendNotification(chatId, addedNotification, activeNotifications);
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

void NotificationManager::handleNgfConnectionStatus(const bool &connected)
{
    qDebug() << "[NotificationManager] NGF Daemon connection status changed " << connected;
}

void NotificationManager::handleNgfEventFailed(const quint32 &eventId)
{
    qDebug() << "[NotificationManager] NGF event failed, id: " << eventId;
}

void NotificationManager::handleNgfEventCompleted(const quint32 &eventId)
{
    qDebug() << "[NotificationManager] NGF event completed, id: " << eventId;
}

void NotificationManager::handleNgfEventPlaying(const quint32 &eventId)
{
    qDebug() << "[NotificationManager] NGF event playing, id: " << eventId;
}

void NotificationManager::handleNgfEventPaused(const quint32 &eventId)
{
    qDebug() << "[NotificationManager] NGF event paused, id: " << eventId;
}

QVariantMap NotificationManager::sendNotification(const QString &chatId, const QVariantMap &notificationInformation, const QVariantMap &activeNotifications)
{
    qDebug() << "[NotificationManager] Sending notification" << notificationInformation.value("id").toString();

    QVariantMap chatInformation = this->chatMap.value(chatId).toMap();
    QString chatType = chatInformation.value("type").toMap().value("@type").toString();
    bool addAuthor = false;
    if (chatType == "chatTypeBasicGroup" || ( chatType == "chatTypeSupergroup" && !chatInformation.value("type").toMap().value("@is_channel").toBool() )) {
        addAuthor = true;
    }

    QVariantMap updatedNotificationInformation = notificationInformation;
    QUrl appIconUrl = SailfishApp::pathTo("images/fernschreiber-notification.png");
    QVariantMap messageMap = notificationInformation.value("type").toMap().value("message").toMap();
    Notification nemoNotification;
    nemoNotification.setAppName("Fernschreiber");
    nemoNotification.setAppIcon(appIconUrl.toLocalFile());
    nemoNotification.setSummary(chatInformation.value("title").toString());
    nemoNotification.setCategory("x-nemo.messaging.im");
    nemoNotification.setTimestamp(QDateTime::fromMSecsSinceEpoch(messageMap.value("date").toLongLong() * 1000));
    if (activeNotifications.isEmpty()) {
        QString notificationBody;
        if (addAuthor) {
            QVariantMap authorInformation = tdLibWrapper->getUserInformation(messageMap.value("sender_user_id").toString());
            QString firstName = authorInformation.value("first_name").toString();
            QString lastName = authorInformation.value("last_name").toString();
            QString fullName = firstName + " " + lastName;
            notificationBody = notificationBody + fullName.trimmed() + ": ";
        }
        notificationBody = notificationBody + this->getNotificationText(messageMap.value("content").toMap());
        nemoNotification.setBody(notificationBody);
    } else {
        nemoNotification.setReplacesId(activeNotifications.first().toMap().value("replaces_id").toUInt());
        nemoNotification.setBody(tr("%1 unread messages").arg(activeNotifications.size() + 1));
    }

    nemoNotification.publish();
    this->ngfClient->play("chat");
    this->controlLedNotification(true);
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

    QString contentType = notificationContent.value("@type").toString();

    if (contentType == "messageText") {
        return notificationContent.value("text").toMap().value("text").toString();
    }
    if (contentType == "messagePhoto") {
        return tr("sent a picture");
    }
    if (contentType == "messageVideo") {
        return tr("sent a video");
    }
    if (contentType == "messageAnimation") {
        return tr("sent an animation");
    }
    if (contentType == "messageVoiceNote") {
        return tr("sent a voice note");
    }
    if (contentType == "messageDocument") {
        return tr("sent a document");
    }
    if (contentType == "messageLocation") {
        return tr("sent a location");
    }
    if (contentType == "messageContactRegistered") {
        return tr("has registered with Telegram");
    }
    if (contentType == "messageChatJoinByLink") {
        return tr("joined this chat");
    }
    if (contentType == "messageChatAddMembers") {
        return tr("was added to this chat");
    }
    if (contentType == "messageChatDeleteMember") {
        return tr("left this chat");
    }
    return tr("Unsupported message: %1").arg(contentType.mid(7));
}

void NotificationManager::controlLedNotification(const bool &enabled)
{
    qDebug() << "[NotificationManager] Controlling notification LED" << enabled;
    QDBusConnection dbusConnection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
    QDBusInterface dbusInterface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", dbusConnection);

    if (enabled) {
        dbusInterface.call("req_led_pattern_activate", "PatternCommunicationIM");
    } else {
        dbusInterface.call("req_led_pattern_deactivate", "PatternCommunicationIM");
    }

}
