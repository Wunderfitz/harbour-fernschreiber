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

#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include <QAbstractListModel>
#include "tdlibwrapper.h"

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ChatListModel(TDLibWrapper *tdLibWrapper);
    ~ChatListModel() override;

    virtual QHash<int,QByteArray> roleNames() const override;
    virtual int rowCount(const QModelIndex&) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void redrawModel();

private slots:
    void handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation);
    void handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void handleChatOrderUpdated(const QString &chatId, const QString &order);
    void handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);
    void handleChatNotificationSettingsUpdated(const QString &chatId, const QVariantMap &chatNotificationSettings);
    void handleRelativeTimeRefreshTimer();

private:
    int updateChatOrder(int chatIndex);

private:
    class ChatData;

    TDLibWrapper *tdLibWrapper;
    QTimer *relativeTimeRefreshTimer;
    QList<ChatData*> chatList;
    QHash<QString,int> chatIndexMap;
};

#endif // CHATLISTMODEL_H
