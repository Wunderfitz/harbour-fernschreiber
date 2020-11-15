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

#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include "tdlibwrapper.h"

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap smallPhoto READ smallPhoto NOTIFY smallPhotoChanged)

public:
    ChatModel(TDLibWrapper *tdLibWrapper);
    ~ChatModel() override;

    virtual int rowCount(const QModelIndex&) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;


    Q_INVOKABLE void initialize(const QVariantMap &chatInformation);
    Q_INVOKABLE void triggerLoadMoreHistory();
    Q_INVOKABLE void triggerLoadMoreFuture();
    Q_INVOKABLE QVariantMap getChatInformation();
    Q_INVOKABLE QVariantMap getMessage(int index);
    Q_INVOKABLE int getLastReadMessageIndex();
    QVariantMap smallPhoto() const;

signals:
    void messagesReceived(int modelIndex, int lastReadSentIndex, int totalCount);
    void messagesIncrementalUpdate(int modelIndex, int lastReadSentIndex);
    void newMessageReceived(const QVariantMap &message);
    void unreadCountUpdated(int unreadCount, const QString &lastReadInboxMessageId);
    void lastReadSentMessageUpdated(int lastReadSentIndex);
    void notificationSettingsUpdated();
    void messageUpdated(int modelIndex);
    void messagesDeleted();
    void smallPhotoChanged();

public slots:
    void handleMessagesReceived(const QVariantList &messages, int totalCount);
    void handleNewMessageReceived(const QString &chatId, const QVariantMap &message);
    void handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);
    void handleChatNotificationSettingsUpdated(const QString &chatId, const QVariantMap &chatNotificationSettings);
    void handleChatPhotoUpdated(qlonglong chatId, const QVariantMap &photo);
    void handleMessageContentUpdated(const QString &chatId, const QString &messageId, const QVariantMap &newContent);
    void handleMessagesDeleted(const QString &chatId, const QVariantList &messageIds);

private:
    TDLibWrapper *tdLibWrapper;
    QVariantList messages;
    QVariantList messagesToBeAdded;
    QVariantMap messageIndexMap;
    QMutex messagesMutex;
    QVariantMap chatInformation;
    qlonglong chatId;
    bool inReload;
    bool inIncrementalUpdate;

    void insertMessages();
    QVariantMap enhanceMessage(const QVariantMap &message);
    int calculateLastKnownMessageId();
    int calculateLastReadSentMessageId();
    void calculateMessageIndexMap();
};

#endif // CHATMODEL_H
