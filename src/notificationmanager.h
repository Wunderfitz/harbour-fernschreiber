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

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <nemonotifications-qt5/notification.h>
#include "tdlibwrapper.h"
#include "appsettings.h"
#include "mceinterface.h"

class ChatModel;

class NotificationManager : public QObject
{
    Q_OBJECT
    class ChatInfo;
    class NotificationGroup;

public:

    NotificationManager(TDLibWrapper *tdLibWrapper, AppSettings *appSettings, MceInterface *mceInterface, ChatModel *chatModel);
    ~NotificationManager() override;

public slots:

    void handleUpdateActiveNotifications(const QVariantList &notificationGroups);
    void handleUpdateNotificationGroup(const QVariantMap &notificationGroupUpdate);
    void handleUpdateNotification(const QVariantMap &updatedNotification);
    void handleChatDiscovered(qlonglong chatId, const QVariantMap &chatInformation);
    void handleChatTitleUpdated(const QString &chatId, const QString &title);

private:

    void publishNotification(const NotificationGroup *notificationGroup, bool needFeedback);
    void controlLedNotification(bool enabled);
    void updateNotificationGroup(int groupId, qlonglong chatId, int totalCount,
        const QVariantList &addedNotifications,
        const QVariantList &removedNotificationIds = QVariantList(),
        AppSettings::NotificationFeedback feedback = AppSettings::NotificationFeedbackNone);

private:

    TDLibWrapper *tdLibWrapper;
    AppSettings *appSettings;
    MceInterface *mceInterface;
    ChatModel *chatModel;
    QMap<qlonglong,ChatInfo*> chatMap;
    QMap<int,NotificationGroup*> notificationGroups;
    QString appIconFile;

};

#endif // NOTIFICATIONMANAGER_H
