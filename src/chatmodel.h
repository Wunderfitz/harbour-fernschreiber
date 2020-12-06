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
#include "tdlibwrapper.h"

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(qlonglong chatId READ getChatId NOTIFY chatIdChanged)
    Q_PROPERTY(QVariantMap smallPhoto READ smallPhoto NOTIFY smallPhotoChanged)

public:
    ChatModel(TDLibWrapper *tdLibWrapper);
    ~ChatModel() override;

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual int rowCount(const QModelIndex&) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void initialize(const QVariantMap &chatInformation);
    Q_INVOKABLE void triggerLoadMoreHistory();
    Q_INVOKABLE void triggerLoadMoreFuture();
    Q_INVOKABLE QVariantMap getChatInformation();
    Q_INVOKABLE QVariantMap getMessage(int index);
    Q_INVOKABLE int getLastReadMessageIndex();

    QVariantMap smallPhoto() const;
    qlonglong getChatId() const;

signals:
    void messagesReceived(int modelIndex, int lastReadSentIndex, int totalCount);
    void messagesIncrementalUpdate(int modelIndex, int lastReadSentIndex);
    void newMessageReceived(const QVariantMap &message);
    void unreadCountUpdated(int unreadCount, const QString &lastReadInboxMessageId);
    void lastReadSentMessageUpdated(int lastReadSentIndex);
    void notificationSettingsUpdated();
    void messageUpdated(int modelIndex);
    void smallPhotoChanged();
    void chatIdChanged();
    void pinnedMessageChanged();

private slots:
    void handleMessagesReceived(const QVariantList &messages, int totalCount);
    void handleNewMessageReceived(qlonglong chatId, const QVariantMap &message);
    void handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void handleMessageSendSucceeded(qlonglong messageId, qlonglong oldMessageId, const QVariantMap &message);
    void handleChatNotificationSettingsUpdated(const QString &chatId, const QVariantMap &chatNotificationSettings);
    void handleChatPhotoUpdated(qlonglong chatId, const QVariantMap &photo);
    void handleChatPinnedMessageUpdated(qlonglong chatId, qlonglong pinnedMessageId);
    void handleMessageContentUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &newContent);
    void handleMessagesDeleted(qlonglong chatId, const QList<qlonglong> &messageIds);

private:
    class MessageData;
    void removeRange(int firstDeleted, int lastDeleted);
    void insertMessages(const QList<MessageData*> newMessages);
    void appendMessages(const QList<MessageData*> newMessages);
    void prependMessages(const QList<MessageData*> newMessages);
    QVariantMap enhanceMessage(const QVariantMap &message);
    int calculateLastKnownMessageId();
    int calculateLastReadSentMessageId();
    bool isMostRecentMessageLoaded();

private:
    TDLibWrapper *tdLibWrapper;
    QList<MessageData*> messages;
    QHash<qlonglong,int> messageIndexMap;
    QVariantMap chatInformation;
    qlonglong chatId;
    bool inReload;
    bool inIncrementalUpdate;
};

#endif // CHATMODEL_H
