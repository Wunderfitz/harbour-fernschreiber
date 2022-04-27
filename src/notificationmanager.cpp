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
#include "chatmodel.h"
#include <sailfishapp.h>
#include <QListIterator>
#include <QUrl>
#include <QDateTime>
#include <QDBusConnection>
#include <QGuiApplication>

#define DEBUG_MODULE NotificationManager
#include "debuglog.h"

namespace {
    const QString _TYPE("@type");
    const QString TYPE("type");
    const QString ID("id");
    const QString CHAT_ID("chat_id");
    const QString IS_CHANNEL("is_channel");
    const QString TOTAL_COUNT("total_count");
    const QString DATE("date");
    const QString TITLE("title");
    const QString CONTENT("content");
    const QString MESSAGE("message");
    const QString FIRST_NAME("first_name");
    const QString LAST_NAME("last_name");
    const QString SENDER_ID("sender_id");
    const QString USER_ID("user_id");
    const QString NOTIFICATIONS("notifications");
    const QString NOTIFICATION_GROUP_ID("notification_group_id");
    const QString ADDED_NOTIFICATIONS("added_notifications");
    const QString REMOVED_NOTIFICATION_IDS("removed_notification_ids");

    const QString CHAT_TYPE_BASIC_GROUP("chatTypeBasicGroup");
    const QString CHAT_TYPE_SUPERGROUP("chatTypeSupergroup");

    const QString APP_NAME("Fernschreiber");

    // Notification hints
    const QString HINT_GROUP_ID("x-fernschreiber.group_id");        // int
    const QString HINT_CHAT_ID("x-fernschreiber.chat_id");          // qlonglong
    const QString HINT_TOTAL_COUNT("x-fernschreiber.total_count");  // int

    const QString HINT_IMAGE_PATH("image-path");                    // QString
    const QString HINT_VIBRA("x-nemo-vibrate");                     // bool
    const QString HINT_SUPPRESS_SOUND("suppress-sound");            // bool
    const QString HINT_DISPLAY_ON("x-nemo-display-on");             // bool
    const QString HINT_VISIBILITY("x-nemo-visibility");             // QString
    const QString HINT_FEEDBACK("x-nemo-feedback");                 // QString
    const QString HINT_PRIORITY("x-nemo-priority");                 // int
    const QString VISIBILITY_PUBLIC("public");
}

class NotificationManager::ChatInfo
{
public:
    ChatInfo(const QVariantMap &info);

    void setChatInfo(const QVariantMap &info);

public:
    TDLibWrapper::ChatType type;
    bool isChannel;
    QString title;
};

NotificationManager::ChatInfo::ChatInfo(const QVariantMap &chatInfo)
{
    setChatInfo(chatInfo);
}

void NotificationManager::ChatInfo::setChatInfo(const QVariantMap &chatInfo)
{
    const QVariantMap chatTypeInformation = chatInfo.value(TYPE).toMap();
    type = TDLibWrapper::chatTypeFromString(chatTypeInformation.value(_TYPE).toString());
    isChannel = chatTypeInformation.value(IS_CHANNEL).toBool();
    title = chatInfo.value(TITLE).toString();
}

class NotificationManager::NotificationGroup
{
public:
    NotificationGroup(int groupId, qlonglong chatId, int count, Notification *notification);
    NotificationGroup(Notification *notification);
    ~NotificationGroup();

public:
    int notificationGroupId;
    qlonglong chatId;
    int totalCount;
    Notification *nemoNotification;
    QMap<int,QVariantMap> activeNotifications;
    QList<int> notificationOrder;
};

NotificationManager::NotificationGroup::NotificationGroup(int group, qlonglong chat, int count, Notification *notification) :
    notificationGroupId(group),
    chatId(chat),
    totalCount(count),
    nemoNotification(notification)
{
}

NotificationManager::NotificationGroup::~NotificationGroup()
{
    delete nemoNotification;
}

NotificationManager::NotificationManager(TDLibWrapper *tdLibWrapper, AppSettings *appSettings, MceInterface *mceInterface, ChatModel *chatModel) :
    appIconFile(SailfishApp::pathTo("images/fernschreiber-notification.png").toLocalFile())
{
    LOG("Initializing...");
    this->tdLibWrapper = tdLibWrapper;
    this->appSettings = appSettings;
    this->mceInterface = mceInterface;
    this->chatModel = chatModel;

    connect(this->tdLibWrapper, SIGNAL(activeNotificationsUpdated(QVariantList)), this, SLOT(handleUpdateActiveNotifications(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(notificationGroupUpdated(QVariantMap)), this, SLOT(handleUpdateNotificationGroup(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(notificationUpdated(QVariantMap)), this, SLOT(handleUpdateNotification(QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatTitleUpdated(QString, QString)), this, SLOT(handleChatTitleUpdated(QString, QString)));

    this->controlLedNotification(false);

    // Restore notifications
    QList<QObject*> notifications = Notification::notifications();
    const int n = notifications.count();
    LOG("Found" << n << "existing notifications");
    for (int i = 0; i < n; i++) {
        QObject *notificationObject = notifications.at(i);
        Notification *notification = qobject_cast<Notification *>(notificationObject);
        if (notification) {
            bool groupOk, chatOk, countOk;
            const int groupId = notification->hintValue(HINT_GROUP_ID).toInt(&groupOk);
            const qlonglong chatId = notification->hintValue(HINT_CHAT_ID).toLongLong(&chatOk);
            const int totalCount = notification->hintValue(HINT_TOTAL_COUNT).toInt(&countOk);
            if (groupOk && chatOk && countOk && !notificationGroups.contains(groupId)) {
                LOG("Restoring notification group" << groupId << "chatId" << chatId << "count" << totalCount);
                notificationGroups.insert(groupId, new NotificationGroup(groupId, chatId, totalCount, notification));
                continue;
            }
        }
        delete notificationObject;
    }
}

NotificationManager::~NotificationManager()
{
    LOG("Destroying myself...");
    qDeleteAll(chatMap.values());
    qDeleteAll(notificationGroups.values());
}

void NotificationManager::handleUpdateActiveNotifications(const QVariantList &notificationGroups)
{
    const int n = notificationGroups.size();
    LOG("Received active notifications, number of groups:" << n);
    for (int i = 0; i < n; i++) {
        const QVariantMap notificationGroupInfo(notificationGroups.at(i).toMap());
        updateNotificationGroup(notificationGroupInfo.value(ID).toInt(),
            notificationGroupInfo.value(CHAT_ID).toLongLong(),
            notificationGroupInfo.value(TOTAL_COUNT).toInt(),
            notificationGroupInfo.value(NOTIFICATIONS).toList());
    }
}

void NotificationManager::handleUpdateNotificationGroup(const QVariantMap &notificationGroupUpdate)
{
    const int notificationGroupId = notificationGroupUpdate.value(NOTIFICATION_GROUP_ID).toInt();
    const int totalCount = notificationGroupUpdate.value(TOTAL_COUNT).toInt();
    LOG("Received notification group update, group ID:" << notificationGroupId << "total count" << totalCount);
    updateNotificationGroup(notificationGroupId,
        notificationGroupUpdate.value(CHAT_ID).toLongLong(), totalCount,
        notificationGroupUpdate.value(ADDED_NOTIFICATIONS).toList(),
        notificationGroupUpdate.value(REMOVED_NOTIFICATION_IDS).toList(),
        appSettings->notificationFeedback());
}

void NotificationManager::updateNotificationGroup(int groupId, qlonglong chatId, int totalCount,
    const QVariantList &addedNotifications, const QVariantList & removedNotificationIds,
    AppSettings::NotificationFeedback feedback)
{
    bool needFeedback = false;
    NotificationGroup* notificationGroup = notificationGroups.value(groupId);

    LOG("Received notification group update, group ID:" << groupId << "total count" << totalCount);
    if (totalCount) {
        if (notificationGroup) {
            // Notification group already exists
            notificationGroup->totalCount = totalCount;
        } else {
            // New notification
            Notification *notification = new Notification(this);
            notification->setCategory("x-nemo.messaging.im");
            notification->setAppName(APP_NAME);
            notification->setAppIcon(appIconFile);
            notification->setIcon(appIconFile);
            notification->setHintValue(HINT_GROUP_ID, groupId);
            notification->setHintValue(HINT_CHAT_ID, chatId);
            notification->setHintValue(HINT_TOTAL_COUNT, totalCount);
            notification->setHintValue(HINT_FEEDBACK, "chat_exists");
            notification->setHintValue(HINT_PRIORITY, 120);
            notificationGroups.insert(groupId, notificationGroup =
                new NotificationGroup(groupId, chatId, totalCount, notification));
        }

        QListIterator<QVariant> addedNotificationIterator(addedNotifications);
        while (addedNotificationIterator.hasNext()) {
            const QVariantMap addedNotification = addedNotificationIterator.next().toMap();
            const int addedId = addedNotification.value(ID).toInt();
            notificationGroup->activeNotifications.insert(addedId, addedNotification);
            notificationGroup->notificationOrder.append(addedId);
        }

        QListIterator<QVariant> removedNotificationIdsIterator(removedNotificationIds);
        while (removedNotificationIdsIterator.hasNext()) {
            const int removedId = removedNotificationIdsIterator.next().toInt();
            notificationGroup->activeNotifications.remove(removedId);
            notificationGroup->notificationOrder.removeOne(removedId);
        }

        // Make sure that if there's no notifications, order is empty too.
        // That's usually already the case but double-check won't wort. It's cheap.
        if (notificationGroup->activeNotifications.isEmpty()) {
            notificationGroup->notificationOrder.clear();
        }

        // Decide if we need a bzzz
        switch (feedback) {
        case AppSettings::NotificationFeedbackNone:
            break;
        case AppSettings::NotificationFeedbackNew:
            // Non-zero replacesId means that notification has already been published
            needFeedback = !notificationGroup->nemoNotification->replacesId();
            break;
        case AppSettings::NotificationFeedbackAll:
            // Even in this case don't alert the user just about removals
            needFeedback = !addedNotifications.isEmpty();
            break;
        }

        // Publish new or update the existing notification
        LOG("Feedback" << needFeedback);
        publishNotification(notificationGroup, needFeedback);
    } else if (notificationGroup) {
        // No active notifications left in this group
        notificationGroup->nemoNotification->close();
        notificationGroups.remove(groupId);
        delete notificationGroup;
    }

    if (notificationGroups.isEmpty()) {
        // No active notifications left at all
        controlLedNotification(false);
    } else if (needFeedback) {
        controlLedNotification(true);
    }
}

void NotificationManager::handleUpdateNotification(const QVariantMap &updatedNotification)
{
    LOG("Received notification update, group ID:" << updatedNotification.value(NOTIFICATION_GROUP_ID).toInt());
}

void NotificationManager::handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation)
{
    const qlonglong id = chatId.toLongLong();
    ChatInfo *chat = chatMap.value(id);
    if (chat) {
        chat->setChatInfo(chatInformation);
        LOG("Updated chat information" << id << chat->title);
    } else {
        chat = new ChatInfo(chatInformation);
        chatMap.insert(id, chat);
        LOG("New chat" << id << chat->title);
    }
}

void NotificationManager::handleChatTitleUpdated(const QString &chatId, const QString &title)
{
    const qlonglong id = chatId.toLongLong();
    ChatInfo *chat = chatMap.value(id);
    if (chat) {
        LOG("Chat" << id << "title changed to" << title);
        chat->title = title;

        // Silently update notification summary
        QListIterator<NotificationGroup*> groupsIterator(notificationGroups.values());
        while (groupsIterator.hasNext()) {
            const NotificationGroup *group = groupsIterator.next();
            if (group->chatId == id) {
                LOG("Updating summary for group ID" << group->notificationGroupId);
                publishNotification(group, false);
                break;
            }
        }
    }
}

void NotificationManager::publishNotification(const NotificationGroup *notificationGroup, bool needFeedback)
{
    QVariantMap messageMap;
    const ChatInfo *chatInformation = chatMap.value(notificationGroup->chatId);
    if (!notificationGroup->notificationOrder.isEmpty()) {
        const int lastNotificationId = notificationGroup->notificationOrder.last();
        const QVariantMap lastNotification(notificationGroup->activeNotifications.value(lastNotificationId));
        messageMap = lastNotification.value(TYPE).toMap().value(MESSAGE).toMap();
    }

    Notification *nemoNotification = notificationGroup->nemoNotification;
    if (!messageMap.isEmpty()) {
        nemoNotification->setTimestamp(QDateTime::fromMSecsSinceEpoch(messageMap.value(DATE).toLongLong() * 1000));

        QVariantList remoteActionArguments;
        remoteActionArguments.append(QString::number(notificationGroup->chatId));
        remoteActionArguments.append(messageMap.value(ID).toString());
        nemoNotification->setRemoteAction(Notification::remoteAction("default", "openMessage",
            "de.ygriega.fernschreiber", "/de/ygriega/fernschreiber", "de.ygriega.fernschreiber",
            "openMessage", remoteActionArguments));
    }

    QString notificationBody;
    const QVariantMap senderInformation = messageMap.value(SENDER_ID).toMap();
    if (notificationGroup->totalCount == 1 && !messageMap.isEmpty()) {
        LOG("Group" << notificationGroup->notificationGroupId << "has 1 notification");
        if (chatInformation && (chatInformation->type == TDLibWrapper::ChatTypeBasicGroup ||
           (chatInformation->type == TDLibWrapper::ChatTypeSupergroup && !chatInformation->isChannel))) {
            // Add author
            QString fullName;
            if (senderInformation.value(_TYPE).toString() == "messageSenderChat") {
                fullName = tdLibWrapper->getChat(senderInformation.value(CHAT_ID).toString()).value(TITLE).toString();
            } else {
                fullName = FernschreiberUtils::getUserName(tdLibWrapper->getUserInformation(senderInformation.value(USER_ID).toString()));
            }

            notificationBody = notificationBody + fullName.trimmed() + ": ";
        }
        notificationBody += FernschreiberUtils::getMessageShortText(tdLibWrapper, messageMap.value(CONTENT).toMap(), (chatInformation ? chatInformation->isChannel : false), tdLibWrapper->getUserInformation().value(ID).toLongLong(), senderInformation );
        nemoNotification->setBody(notificationBody);
    } else {
        // Either we have more than one notification or we have no content to display
        LOG("Group" << notificationGroup->notificationGroupId << "has" << notificationGroup->totalCount << "notifications");
        notificationBody = tr("%Ln unread messages", "", notificationGroup->totalCount);
    }

    const QString summary(chatInformation ? chatInformation->title : QString());
    nemoNotification->setBody(notificationBody);
    nemoNotification->setSummary(summary);
    nemoNotification->setHintValue(HINT_VIBRA, needFeedback);
    nemoNotification->setHintValue(HINT_IMAGE_PATH, QString());

    // Don't show popup for the currently open chat
    if (!needFeedback || (chatModel->getChatId() == notificationGroup->chatId &&
            qGuiApp->applicationState() == Qt::ApplicationActive)) {
        nemoNotification->setHintValue(HINT_SUPPRESS_SOUND, true);
        nemoNotification->setHintValue(HINT_DISPLAY_ON, false);
        nemoNotification->setHintValue(HINT_VISIBILITY, QString());
        nemoNotification->setUrgency(Notification::Low);
    } else {
        nemoNotification->setPreviewBody(notificationBody);
        nemoNotification->setPreviewSummary(summary);
        nemoNotification->setHintValue(HINT_SUPPRESS_SOUND, !appSettings->notificationSoundsEnabled());
        nemoNotification->setHintValue(HINT_DISPLAY_ON, appSettings->notificationTurnsDisplayOn());
        nemoNotification->setHintValue(HINT_VISIBILITY, VISIBILITY_PUBLIC);
        nemoNotification->setUrgency(Notification::Normal);
    }

    nemoNotification->publish();
}

void NotificationManager::controlLedNotification(bool enabled)
{
    static const QString PATTERN("PatternCommunicationIM");
    if (enabled) {
        mceInterface->ledPatternActivate(PATTERN);
    } else {
        mceInterface->ledPatternDeactivate(PATTERN);
    }
}
