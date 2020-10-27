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

#include "notificationmanager.h"
#include "fernschreiberutils.h"
#include <nemonotifications-qt5/notification.h>
#include <sailfishapp.h>
#include <QDebug>
#include <QListIterator>
#include <QUrl>
#include <QDateTime>
#include <QDBusConnection>

#define LOG(x) qDebug() << "[NotificationManager]" << x

namespace {
    const QString _TYPE("@type");
    const QString TYPE("type");
    const QString ID("id");
    const QString CHAT_ID("chat_id");
    const QString IS_SILENT("is_silent");
    const QString IS_CHANNEL("is_channel");
    const QString REPLACES_ID("replaces_id");
    const QString TOTAL_COUNT("total_count");
    const QString DATE("date");
    const QString TITLE("title");
    const QString CONTENT("content");
    const QString MESSAGE("message");
    const QString FIRST_NAME("first_name");
    const QString LAST_NAME("last_name");
    const QString SENDER_USER_ID("sender_user_id");
    const QString NOTIFICATIONS("notifications");
    const QString NOTIFICATION_GROUP_ID("notification_group_id");
    const QString NOTIFICATION_SETTINGS_CHAT_ID("notification_settings_chat_id");
    const QString ADDED_NOTIFICATIONS("added_notifications");
    const QString REMOVED_NOTIFICATION_IDS("removed_notification_ids");

    const QString CHAT_TYPE_BASIC_GROUP("chatTypeBasicGroup");
    const QString CHAT_TYPE_SUPERGROUP("chatTypeSupergroup");

    const QString NOTIFICATION_CATEGORY("x-nemo.messaging.im");
    const QString NGF_EVENT("chat");

    const QString APP_NAME("Fernschreiber");
}

NotificationManager::NotificationManager(TDLibWrapper *tdLibWrapper, AppSettings *appSettings) :
    mceInterface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", QDBusConnection::systemBus()),
    appIconFile(SailfishApp::pathTo("images/fernschreiber-notification.png").toLocalFile())
{
    LOG("Initializing...");
    this->tdLibWrapper = tdLibWrapper;
    this->appSettings = appSettings;
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
        LOG("NGF Client successfully initialized...");
    } else {
        LOG("Failed to initialize NGF Client...");
    }

    this->controlLedNotification(false);
}

NotificationManager::~NotificationManager()
{
    LOG("Destroying myself...");
}

void NotificationManager::handleUpdateActiveNotifications(const QVariantList notificationGroups)
{
    LOG("Received active notifications, number of groups:" << notificationGroups.size());
}

void NotificationManager::handleUpdateNotificationGroup(const QVariantMap notificationGroupUpdate)
{
    const QString chatId = notificationGroupUpdate.value(CHAT_ID).toString();
    const QString notificationGroupId = notificationGroupUpdate.value(NOTIFICATION_GROUP_ID).toString();
    LOG("Received notification group update, group ID:" << notificationGroupId);

    QVariantMap notificationGroup = this->notificationGroups.value(notificationGroupId).toMap();
    notificationGroup.insert(TYPE, notificationGroupUpdate.value(TYPE));
    notificationGroup.insert(CHAT_ID, chatId);
    notificationGroup.insert(NOTIFICATION_GROUP_ID, notificationGroupId);
    notificationGroup.insert(NOTIFICATION_SETTINGS_CHAT_ID, notificationGroupUpdate.value(NOTIFICATION_SETTINGS_CHAT_ID));
    notificationGroup.insert(IS_SILENT, notificationGroupUpdate.value(IS_SILENT));
    notificationGroup.insert(TOTAL_COUNT, notificationGroupUpdate.value(TOTAL_COUNT));

    QVariantMap activeNotifications = notificationGroup.value(NOTIFICATIONS).toMap();

    const QVariantList removedNotificationIds = notificationGroupUpdate.value(REMOVED_NOTIFICATION_IDS).toList();
    QListIterator<QVariant> removedNotificationsIterator(removedNotificationIds);
    while (removedNotificationsIterator.hasNext()) {
        const QString removedNotificationId = removedNotificationsIterator.next().toString();
        const QVariantMap notificationInformation = activeNotifications.value(removedNotificationId).toMap();
        if (!notificationInformation.isEmpty()) {
            this->removeNotification(notificationInformation);
            activeNotifications.remove(removedNotificationId);
        }
    }

    // If we have deleted notifications, we need to update possibly existing ones
    if (!removedNotificationIds.isEmpty() && !activeNotifications.isEmpty()) {
        LOG("Some removals happend, but we have" << activeNotifications.size() << "existing notifications.");
        const QVariantMap firstActiveNotification = activeNotifications.first().toMap();
        activeNotifications.remove(firstActiveNotification.value(ID).toString());
        QVariantMap newFirstActiveNotification = this->sendNotification(chatId, firstActiveNotification, activeNotifications);
        QVariantMap newActiveNotifications;
        QListIterator<QVariant> activeNotificationsIterator(activeNotifications.values());
        while (activeNotificationsIterator.hasNext()) {
            QVariantMap newActiveNotification = activeNotificationsIterator.next().toMap();
            newActiveNotification.insert(REPLACES_ID, newFirstActiveNotification.value(REPLACES_ID));
            newActiveNotifications.insert(newActiveNotification.value(ID).toString(), newActiveNotification);
        }
        newActiveNotifications.insert(newFirstActiveNotification.value(ID).toString(), newFirstActiveNotification);
        activeNotifications = newActiveNotifications;
    }

    if (activeNotifications.isEmpty()) {
        this->controlLedNotification(false);
    }

    const QVariantList addedNotifications = notificationGroupUpdate.value(ADDED_NOTIFICATIONS).toList();
    QListIterator<QVariant> addedNotificationIterator(addedNotifications);
    while (addedNotificationIterator.hasNext()) {
        const QVariantMap addedNotification = addedNotificationIterator.next().toMap();
        const QVariantMap activeNotification = this->sendNotification(chatId, addedNotification, activeNotifications);
        activeNotifications.insert(activeNotification.value(ID).toString(), activeNotification);
    }

    notificationGroup.insert(NOTIFICATIONS, activeNotifications);
    this->notificationGroups.insert(notificationGroupId, notificationGroup);
}

void NotificationManager::handleUpdateNotification(const QVariantMap updatedNotification)
{
    LOG("Received notification update, group ID:" << updatedNotification.value(NOTIFICATION_GROUP_ID).toInt());
}

void NotificationManager::handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation)
{
    LOG("Adding chat to internal map" << chatId);
    this->chatMap.insert(chatId, chatInformation);
}

void NotificationManager::handleNgfConnectionStatus(bool connected)
{
    LOG("NGF Daemon connection status changed" << connected);
}

void NotificationManager::handleNgfEventFailed(quint32 eventId)
{
    LOG("NGF event failed, id:" << eventId);
}

void NotificationManager::handleNgfEventCompleted(quint32 eventId)
{
    LOG("NGF event completed, id:" << eventId);
}

void NotificationManager::handleNgfEventPlaying(quint32 eventId)
{
    LOG("NGF event playing, id:" << eventId);
}

void NotificationManager::handleNgfEventPaused(quint32 eventId)
{
    LOG("NGF event paused, id:" << eventId);
}

QVariantMap NotificationManager::sendNotification(const QString &chatId, const QVariantMap &notificationInformation, const QVariantMap &activeNotifications)
{
    LOG("Sending notification" << notificationInformation.value(ID).toString());

    const QVariantMap chatInformation = this->chatMap.value(chatId).toMap();
    const QVariantMap chatTypeInformation = chatInformation.value(TYPE).toMap();
    const QString chatType = chatTypeInformation.value(_TYPE).toString();
    const bool addAuthor = (chatType == CHAT_TYPE_BASIC_GROUP || ( chatType == CHAT_TYPE_SUPERGROUP && !chatTypeInformation.value(IS_CHANNEL).toBool() ));

    const QVariantMap messageMap = notificationInformation.value(TYPE).toMap().value(MESSAGE).toMap();
    Notification nemoNotification;
    nemoNotification.setAppName(APP_NAME);
    nemoNotification.setAppIcon(appIconFile);
    nemoNotification.setSummary(chatInformation.value(TITLE).toString());
    nemoNotification.setTimestamp(QDateTime::fromMSecsSinceEpoch(messageMap.value(DATE).toLongLong() * 1000));
    QVariantList remoteActionArguments;
    remoteActionArguments.append(chatId);
    remoteActionArguments.append(messageMap.value(ID).toString());
    nemoNotification.setRemoteAction(Notification::remoteAction("default", "openMessage", "de.ygriega.fernschreiber", "/de/ygriega/fernschreiber", "de.ygriega.fernschreiber", "openMessage", remoteActionArguments));

    bool needFeedback;
    const AppSettings::NotificationFeedback feedbackStyle = appSettings->notificationFeedback();

    if (activeNotifications.isEmpty()) {
        QString notificationBody;
        if (addAuthor) {
            const QVariantMap authorInformation = tdLibWrapper->getUserInformation(messageMap.value(SENDER_USER_ID).toString());
            const QString firstName = authorInformation.value(FIRST_NAME).toString();
            const QString lastName = authorInformation.value(LAST_NAME).toString();
            const QString fullName = firstName + " " + lastName;
            notificationBody = notificationBody + fullName.trimmed() + ": ";
        }
        notificationBody = notificationBody + this->getNotificationText(messageMap.value(CONTENT).toMap());
        nemoNotification.setBody(notificationBody);
        needFeedback = (feedbackStyle != AppSettings::NotificationFeedbackNone);
    } else {
        nemoNotification.setReplacesId(activeNotifications.first().toMap().value(REPLACES_ID).toUInt());
        nemoNotification.setBody(tr("%1 unread messages").arg(activeNotifications.size() + 1));
        needFeedback = (feedbackStyle == AppSettings::NotificationFeedbackAll);
    }

    if (needFeedback) {
        nemoNotification.setCategory(NOTIFICATION_CATEGORY);
        ngfClient->play(NGF_EVENT);
    }

    nemoNotification.publish();
    this->controlLedNotification(true);

    QVariantMap updatedNotificationInformation = notificationInformation;
    updatedNotificationInformation.insert(REPLACES_ID, nemoNotification.replacesId());
    return updatedNotificationInformation;
}

void NotificationManager::removeNotification(const QVariantMap &notificationInformation)
{
    LOG("Removing notification" << notificationInformation.value(ID).toString());
    Notification nemoNotification;
    nemoNotification.setReplacesId(notificationInformation.value(REPLACES_ID).toUInt());
    nemoNotification.close();
}

QString NotificationManager::getNotificationText(const QVariantMap &notificationContent)
{
    LOG("Getting notification text from content" << notificationContent);

    return FernschreiberUtils::getMessageShortText(notificationContent, false);
}

void NotificationManager::controlLedNotification(bool enabled)
{
    static const QString PATTERN("PatternCommunicationIM");
    static const QString ACTIVATE("req_led_pattern_activate");
    static const QString DEACTIVATE("req_led_pattern_deactivate");

    LOG("Controlling notification LED" << enabled);
    mceInterface.call(enabled ? ACTIVATE : DEACTIVATE, PATTERN);
}
