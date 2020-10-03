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
#include <QDebug>

#define LOG(x) qDebug() << "[ChatListModel]" << x

namespace {
    const QString ID("id");
    const QString DATE("date");
    const QString TEXT("text");
    const QString TITLE("title");
    const QString PHOTO("photo");
    const QString SMALL("small");
    const QString ORDER("order");
    const QString CHAT_ID("chat_id");
    const QString CONTENT("content");
    const QString LAST_MESSAGE("last_message");
    const QString SENDER_USER_ID("sender_user_id");
    const QString UNREAD_COUNT("unread_count");
    const QString NOTIFICATION_SETTINGS("notification_settings");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");
    const QString TYPE_MAP("type");
    const QString IS_CHANNEL("is_channel");

    const QString TYPE("@type");
    const QString TYPE_MESSAGE_TEXT("messageText");
}

class ChatListModel::ChatData
{
public:
    enum Role {
        RoleDisplay = Qt::DisplayRole,
        RoleChatId,
        RoleTitle,
        RolePhotoSmall,
        RoleUnreadCount,
        RoleLastReadInboxMessageId,
        RoleLastMessageSenderId,
        RoleLastMessageDate,
        RoleLastMessageText,
        RoleIsChannel
    };

    ChatData(const QVariantMap &data);

    int compareTo(const ChatData *chat) const;
    bool setOrder(const QString &order);
    const QVariant lastMessage(const QString &key) const;
    QString title() const;
    int unreadCount() const;
    QVariant photoSmall() const;
    qlonglong lastReadInboxMessageId() const;
    qlonglong senderUserId() const;
    qlonglong senderMessageDate() const;
    QString senderMessageText() const;
    bool isChannel() const;
    bool updateUnreadCount(int unreadCount);
    bool updateLastReadInboxMessageId(qlonglong messageId);
    QVector<int> updateLastMessage(const QVariantMap &message);

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

inline const QVariant ChatListModel::ChatData::lastMessage(const QString &key) const
{
    return chatData.value(LAST_MESSAGE).toMap().value(key);
}

QString ChatListModel::ChatData::title() const
{
    return chatData.value(TITLE).toString();
}

int ChatListModel::ChatData::unreadCount() const
{
    return chatData.value(UNREAD_COUNT).toInt();
}

QVariant ChatListModel::ChatData::photoSmall() const
{
    return chatData.value(PHOTO).toMap().value(SMALL);
}

qlonglong ChatListModel::ChatData::lastReadInboxMessageId() const
{
    return chatData.value(LAST_READ_INBOX_MESSAGE_ID).toLongLong();
}

qlonglong ChatListModel::ChatData::senderUserId() const
{
    return lastMessage(SENDER_USER_ID).toLongLong();
}

qlonglong ChatListModel::ChatData::senderMessageDate() const
{
    return lastMessage(DATE).toLongLong();
}

QString ChatListModel::ChatData::senderMessageText() const
{
    const QVariantMap content(lastMessage(CONTENT).toMap());
    return (content.value(TYPE).toString() == TYPE_MESSAGE_TEXT) ? content.value(TEXT).toMap().value(TEXT).toString() : QString();
}

bool ChatListModel::ChatData::isChannel() const
{
    return chatData.value(TYPE_MAP).toMap().value(IS_CHANNEL).toBool();
}

bool ChatListModel::ChatData::updateUnreadCount(int count)
{
    const int prevUnreadCount(unreadCount());
    chatData.insert(UNREAD_COUNT, count);
    return prevUnreadCount != unreadCount();
}

bool ChatListModel::ChatData::updateLastReadInboxMessageId(qlonglong messageId)
{
    const qlonglong prevLastReadInboxMessageId(lastReadInboxMessageId());
    chatData.insert(LAST_READ_INBOX_MESSAGE_ID, messageId);
    return prevLastReadInboxMessageId != lastReadInboxMessageId();
}

QVector<int> ChatListModel::ChatData::updateLastMessage(const QVariantMap &message)
{
    const qlonglong prevSenderUserId(senderUserId());
    const qlonglong prevSenderMessageDate(senderMessageDate());
    const QString prevSenderMessageText(senderMessageText());

    chatData.insert(LAST_MESSAGE, message);

    QVector<int> changedRoles;
    changedRoles.append(RoleDisplay);
    if (prevSenderUserId != senderUserId()) {
        changedRoles.append(RoleLastMessageSenderId);
    }
    if (prevSenderMessageDate != senderMessageDate()) {
        changedRoles.append(RoleLastMessageDate);
    }
    if (prevSenderMessageText != senderMessageText()) {
        changedRoles.append(RoleLastMessageText);
    }
    return changedRoles;
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

    // Don't start the timer until we have at least one chat
    relativeTimeRefreshTimer = new QTimer(this);
    relativeTimeRefreshTimer->setSingleShot(false);
    relativeTimeRefreshTimer->setInterval(30000);
    connect(relativeTimeRefreshTimer, SIGNAL(timeout()), SLOT(handleRelativeTimeRefreshTimer()));
}

ChatListModel::~ChatListModel()
{
    LOG("Destroying myself...");
    qDeleteAll(chatList);
}

QHash<int,QByteArray> ChatListModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(ChatData::RoleDisplay, "display");
    roles.insert(ChatData::RoleChatId, "chat_id");
    roles.insert(ChatData::RoleTitle, "title");
    roles.insert(ChatData::RolePhotoSmall, "photo_small");
    roles.insert(ChatData::RoleUnreadCount, "unread_count");
    roles.insert(ChatData::RoleLastReadInboxMessageId, "last_read_inbox_message_id");
    roles.insert(ChatData::RoleLastMessageSenderId, "last_message_sender_id");
    roles.insert(ChatData::RoleLastMessageDate, "last_message_date");
    roles.insert(ChatData::RoleLastMessageText, "last_message_text");
    roles.insert(ChatData::RoleIsChannel, "is_channel");
    return roles;
}

int ChatListModel::rowCount(const QModelIndex &) const
{
    return chatList.size();
}

QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row >= 0 && row < chatList.size()) {
        const ChatData *data = chatList.at(row);
        switch ((ChatData::Role)role) {
        case ChatData::RoleDisplay: return data->chatData;
        case ChatData::RoleChatId: return data->chatId;
        case ChatData::RoleTitle: return data->title();
        case ChatData::RolePhotoSmall: return data->photoSmall();
        case ChatData::RoleUnreadCount: return data->unreadCount();
        case ChatData::RoleLastReadInboxMessageId: return data->lastReadInboxMessageId();
        case ChatData::RoleLastMessageSenderId: return data->senderUserId();
        case ChatData::RoleLastMessageText: return data->senderMessageText();
        case ChatData::RoleLastMessageDate: return data->senderMessageDate();
        case ChatData::RoleIsChannel: return data->isChannel();
        }
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
    ChatData *chat = chatList.at(chatIndex);

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
    ChatData *chat = new ChatData(chatToBeAdded);
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

    // Start timestamp refresh timer when the first chat is discovered
    if (!relativeTimeRefreshTimer->isActive()) {
        relativeTimeRefreshTimer->start();
    }
}

void ChatListModel::handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage)
{
    if (chatIndexMap.contains(chatId)) {
        int chatIndex = chatIndexMap.value(chatId);
        LOG("Updating last message for chat" << chatId <<" at index" << chatIndex << "new order" << order);
        ChatData *chat = chatList.at(chatIndex);
        if (chat->setOrder(order)) {
            chatIndex = updateChatOrder(chatIndex);
        }
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex, chat->updateLastMessage(lastMessage));
    }
}

void ChatListModel::handleChatOrderUpdated(const QString &chatId, const QString &order)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating chat order of" << chatId << "to" << order);
        int chatIndex = chatIndexMap.value(chatId);
        if (chatList.at(chatIndex)->setOrder(order)) {
            updateChatOrder(chatIndex);
        }
    }
}

void ChatListModel::handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, int unreadCount)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating chat unread count for" << chatId << "unread messages" << unreadCount << ", last read message ID: " << lastReadInboxMessageId);
        const int chatIndex = chatIndexMap.value(chatId);
        ChatData *chat = chatList.at(chatIndex);
        QVector<int> changedRoles;
        changedRoles.append(ChatData::RoleDisplay);
        if (chat->updateUnreadCount(unreadCount)) {
            changedRoles.append(ChatData::RoleUnreadCount);
        }
        if (chat->updateLastReadInboxMessageId(lastReadInboxMessageId.toLongLong())) {
            changedRoles.append(ChatData::RoleLastReadInboxMessageId);
        }
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex, changedRoles);
    }
}

void ChatListModel::handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating last read message for" << chatId << "last ID" << lastReadOutboxMessageId);
        const int chatIndex = chatIndexMap.value(chatId);
        ChatData *chat = chatList.at(chatIndex);
        chat->chatData.insert(LAST_READ_OUTBOX_MESSAGE_ID, lastReadOutboxMessageId);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
    }
}

void ChatListModel::handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message)
{
    const QString chatId(message.value(CHAT_ID).toString());
    if (chatIndexMap.contains(chatId)) {
        const int chatIndex = chatIndexMap.value(chatId);
        LOG("Updating last message for chat" << chatId << "at index" << chatIndex << ", as message was sent, old ID:" << oldMessageId << ", new ID:" << messageId);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex, chatList.at(chatIndex)->updateLastMessage(message));
    }
}

void ChatListModel::handleChatNotificationSettingsUpdated(const QString &chatId, const QVariantMap &chatNotificationSettings)
{
    if (chatIndexMap.contains(chatId)) {
        const int chatIndex = chatIndexMap.value(chatId);
        LOG("Updating notification settings for chat" << chatId << "at index" << chatIndex);
        ChatData *chat = chatList.at(chatIndex);
        chat->chatData.insert(NOTIFICATION_SETTINGS, chatNotificationSettings);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex);
    }
}

void ChatListModel::handleRelativeTimeRefreshTimer()
{
    LOG("Refreshing timestamps");
    QVector<int> roles;
    roles.append(ChatData::RoleLastMessageDate);
    emit dataChanged(index(0), index(chatList.size() - 1), roles);
}
