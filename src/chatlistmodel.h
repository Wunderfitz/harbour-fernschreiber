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
#include <QDebug>
#include <QMutex>
#include "tdlibwrapper.h"

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ChatListModel(TDLibWrapper *tdLibWrapper);
    ~ChatListModel() override;

    virtual int rowCount(const QModelIndex&) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    Q_INVOKABLE void enableDeltaUpdates();
    Q_INVOKABLE void redrawModel();

signals:
    void chatChanged(const QString &chatId);

public slots:
    void handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation);
    void handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void handleChatOrderUpdated(const QString &chatId, const QString &order);
    void handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, const int &unreadCount);
    void handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);
    void handleChatNotificationSettingsUpdated(const QString &chatId, const QVariantMap &chatNotificationSettings);

private:
    TDLibWrapper *tdLibWrapper;
    QVariantList chatList;
    QVariantMap chatToBeAdded;
    QVariantMap chatIndexMap;
    QMutex chatListMutex;
    bool deltaUpdates;

    void updateChatOrder(const int &currentChatIndex, const QVariantMap &updatedChat);

};

#endif // CHATLISTMODEL_H
