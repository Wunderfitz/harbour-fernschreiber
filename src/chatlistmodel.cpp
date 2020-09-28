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

#include "chatlistmodel.h"
#include <QListIterator>
#include <QDebug>

#define LOG(x) qDebug() << "[ChatListModel]" << x

namespace {
    const QString ID("id");
    const QString ORDER("order");
    const QString CHAT_ID("chat_id");
    const QString LAST_MESSAGE("last_message");
    const QString UNREAD_COUNT("unread_count");
    const QString NOTIFICATION_SETTINGS("notification_settings");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");
}

class ChatListModel::ChatData
{
public:
    ChatData(const QVariantMap &data);

    int compareTo(const ChatData *chat) const;
    bool setOrder(const QString &order);

public:
    QVariantMap chatData;
    QString chatId;
    qlonglong order;
};

ChatListModel::ChatData::ChatData(const QVariantMap &data) :
    chatData(data),
    chatId(data.value(ID).toString()),
    order(data.value(ORDER).toLongLong())
{
}

int ChatListModel::ChatData::compareTo(const ChatData *other) const
{
    if (order == other->order) {
        return chatId.compare(other->chatId);
    } else {
        // This puts most recent ones to the top of the list
        return (order < other->order) ? 1 : -1;
    }
}

bool ChatListModel::ChatData::setOrder(const QString &newOrder)
{
    if (!newOrder.isEmpty()) {
        chatData.insert(ORDER, newOrder);
        order = newOrder.toLongLong();
        return true;
    }
    return false;
}

ChatListModel::ChatListModel(TDLibWrapper *tdLibWrapper)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatLastMessageUpdated(QString, QString, QVariantMap)), this, SLOT(handleChatLastMessageUpdated(QString, QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatOrderUpdated(QString, QString)), this, SLOT(handleChatOrderUpdated(QString, QString)));
    connect(tdLibWrapper, SIGNAL(chatReadInboxUpdated(QString, QString, int)), this, SLOT(handleChatReadInboxUpdated(QString, QString, int)));
    connect(tdLibWrapper, SIGNAL(chatReadOutboxUpdated(QString, QString)), this, SLOT(handleChatReadOutboxUpdated(QString, QString)));
    connect(tdLibWrapper, SIGNAL(messageSendSucceeded(QString, QString, QVariantMap)), this, SLOT(handleMessageSendSucceeded(QString, QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatNotificationSettingsUpdated(QString, QVariantMap)), this, SLOT(handleChatNotificationSettingsUpdated(QString, QVariantMap)));
}

ChatListModel::~ChatListModel()
{
    LOG("Destroying myself...");
    qDeleteAll(chatList);
}

int ChatListModel::rowCount(const QModelIndex &) const
{
    return chatList.size();
}

QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row >= 0 && row < chatList.size() && role == Qt::DisplayRole) {
        return chatList.at(row)->chatData;
    }
    return QVariant();
}

void ChatListModel::redrawModel()
{
    LOG("Enforcing UI redraw...");
    layoutChanged();
}

int ChatListModel::updateChatOrder(int chatIndex)
{
    ChatData* chat = chatList.at(chatIndex);

    const int n = chatList.size();
    int newIndex = chatIndex;
    while (newIndex > 0 && chat->compareTo(chatList.at(newIndex-1)) < 0) {
        newIndex--;
    }
    if (newIndex == chatIndex) {
        while (newIndex < n-1 && chat->compareTo(chatList.at(newIndex+1)) > 0) {
            newIndex++;
        }
    }
    if (newIndex != chatIndex) {
        LOG("Moving chat" << chat->chatId << "from position" << chatIndex << "to" << newIndex);
        beginMoveRows(QModelIndex(), chatIndex, chatIndex, QModelIndex(), (newIndex < chatIndex) ? newIndex : (newIndex+1));
        chatList.move(chatIndex, newIndex);
        chatIndexMap.insert(chat->chatId, newIndex);
        // Update damaged part of the map
        const int last = qMax(chatIndex, newIndex);
        if (newIndex < chatIndex) {
            // First index is already correct
            for (int i = newIndex + 1; i <= last; i++) {
                chatIndexMap.insert(chatList.at(i)->chatId, i);
            }
        } else {
            // Last index is already correct
            for (int i = chatIndex; i < last; i++) {
                chatIndexMap.insert(chatList.at(i)->chatId, i);
            }
        }
        endMoveRows();
    } else {
        LOG("Chat" << chat->chatId << "stays at position" << chatIndex);
    }

    return newIndex;
}

void ChatListModel::handleChatDiscovered(const QString &chatId, const QVariantMap &chatToBeAdded)
{
    ChatData* chat = new ChatData(chatToBeAdded);
    const int n = chatList.size();
    int chatIndex;
    for (chatIndex = 0; chatIndex < n && chat->compareTo(chatList.at(chatIndex)) >= 0; chatIndex++);
    LOG("Adding new chat" << chatId << "at" << chatIndex);
    beginInsertRows(QModelIndex(), chatIndex, chatIndex);
    chatList.insert(chatIndex, chat);
    chatIndexMap.insert(chat->chatId, chatIndex);
    // Update damaged part of the map
    for (int i = chatIndex + 1; i <= n; i++) {
        chatIndexMap.insert(chatList.at(i)->chatId, i);
    }
    endInsertRows();
}

void ChatListModel::handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage)
{
    if (chatIndexMap.contains(chatId)) {
        int chatIndex = chatIndexMap.value(chatId);
        LOG("Updating last message for chat" << chatId <<" at index" << chatIndex << "new order" << order);
        ChatData* chat = chatList.at(chatIndex);
        chat->chatData.insert(LAST_MESSAGE, lastMessage);
        if (chat->setOrder(order)) {
            chatIndex = updateChatOrder(chatIndex);
        }
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
        emit chatChanged(chatId);
    }
}

void ChatListModel::handleChatOrderUpdated(const QString &chatId, const QString &order)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating chat order of" << chatId << "to" << order);
        int chatIndex = chatIndexMap.value(chatId);
        if (chatList.at(chatIndex)->setOrder(order)) {
            chatIndex = updateChatOrder(chatIndex);
        }
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
        emit chatChanged(chatId);
    }
}

void ChatListModel::handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, const int &unreadCount)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating chat unread count for" << chatId << "unread messages" << unreadCount << ", last read message ID: " << lastReadInboxMessageId);
        const int chatIndex = chatIndexMap.value(chatId);
        ChatData* chat = chatList.at(chatIndex);
        chat->chatData.insert(UNREAD_COUNT, unreadCount);
        chat->chatData.insert(LAST_READ_INBOX_MESSAGE_ID, lastReadInboxMessageId);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
        emit chatChanged(chatId);
    }
}

void ChatListModel::handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating last read message for" << chatId << "last ID" << lastReadOutboxMessageId);
        const int chatIndex = chatIndexMap.value(chatId);
        ChatData* chat = chatList.at(chatIndex);
        chat->chatData.insert(LAST_READ_OUTBOX_MESSAGE_ID, lastReadOutboxMessageId);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
        emit chatChanged(chatId);
    }
}

void ChatListModel::handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message)
{
    const QString chatId(message.value(CHAT_ID).toString());
    if (chatIndexMap.contains(chatId)) {
        const int chatIndex = chatIndexMap.value(chatId);
        LOG("Updating last message for chat" << chatId << "at index" << chatIndex << ", as message was sent, old ID:" << oldMessageId << ", new ID:" << messageId);
        ChatData* chat = chatList.at(chatIndex);
        chat->chatData.insert(LAST_MESSAGE, message);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
        emit chatChanged(chatId);
    }
}

void ChatListModel::handleChatNotificationSettingsUpdated(const QString &chatId, const QVariantMap &chatNotificationSettings)
{
    if (chatIndexMap.contains(chatId)) {
        const int chatIndex = chatIndexMap.value(chatId);
        LOG("Updating notification settings for chat" << chatId << "at index" << chatIndex);
        ChatData* chat = chatList.at(chatIndex);
        chat->chatData.insert(NOTIFICATION_SETTINGS, chatNotificationSettings);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
        emit chatChanged(chatId);
    }
}
