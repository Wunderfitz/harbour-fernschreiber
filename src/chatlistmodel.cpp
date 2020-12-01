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

#include "chatlistmodel.h"
#include "fernschreiberutils.h"

#define DEBUG_MODULE ChatListModel
#include "debuglog.h"

namespace {
    const QString ID("id");
    const QString DATE("date");
    const QString TEXT("text");
    const QString TYPE("type");
    const QString TITLE("title");
    const QString PHOTO("photo");
    const QString SMALL("small");
    const QString ORDER("order");
    const QString CHAT_ID("chat_id");
    const QString CONTENT("content");
    const QString LAST_MESSAGE("last_message");
    const QString SENDER_USER_ID("sender_user_id");
    const QString BASIC_GROUP_ID("basic_group_id");
    const QString SUPERGROUP_ID("supergroup_id");
    const QString UNREAD_COUNT("unread_count");
    const QString NOTIFICATION_SETTINGS("notification_settings");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");
    const QString SENDING_STATE("sending_state");
    const QString IS_CHANNEL("is_channel");
    const QString PINNED_MESSAGE_ID("pinned_message_id");
    const QString _TYPE("@type");
}

class ChatListModel::ChatData
{
public:
    enum Role {
        RoleDisplay = Qt::DisplayRole,
        RoleChatId,
        RoleChatType,
        RoleTitle,
        RolePhotoSmall,
        RoleUnreadCount,
        RoleLastReadInboxMessageId,
        RoleLastMessageSenderId,
        RoleLastMessageDate,
        RoleLastMessageText,
        RoleLastMessageStatus,
        RoleChatMemberStatus,
        RoleIsChannel
    };

    ChatData(const QVariantMap &data, const QVariantMap &userInformation);

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
    QString senderMessageStatus() const;
    bool isChannel() const;
    bool isHidden() const;
    bool updateUnreadCount(int unreadCount);
    bool updateLastReadInboxMessageId(qlonglong messageId);
    QVector<int> updateLastMessage(const QVariantMap &message);
    QVector<int> updateGroup(const TDLibWrapper::Group *group);

public:
    QVariantMap chatData;
    qlonglong chatId;
    qlonglong order;
    qlonglong groupId;
    TDLibWrapper::ChatType chatType;
    TDLibWrapper::ChatMemberStatus memberStatus;
    QVariantMap userInformation;
};

ChatListModel::ChatData::ChatData(const QVariantMap &data, const QVariantMap &userInfo) :
    chatData(data),
    chatId(data.value(ID).toLongLong()),
    order(data.value(ORDER).toLongLong()),
    groupId(0),
    memberStatus(TDLibWrapper::ChatMemberStatusUnknown),
    userInformation(userInfo)
{
    const QVariantMap type(data.value(TYPE).toMap());
    switch (chatType = TDLibWrapper::chatTypeFromString(type.value(_TYPE).toString())) {
    case TDLibWrapper::ChatTypeBasicGroup:
        groupId = type.value(BASIC_GROUP_ID).toLongLong();
        break;
    case TDLibWrapper::ChatTypeSupergroup:
        groupId = type.value(SUPERGROUP_ID).toLongLong();
        break;
    case TDLibWrapper::ChatTypeUnknown:
    case TDLibWrapper::ChatTypePrivate:
    case TDLibWrapper::ChatTypeSecret:
        break;
    }
}

int ChatListModel::ChatData::compareTo(const ChatData *other) const
{
    if (order == other->order) {
        return (chatId < other->chatId) ? 1 : -1;
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
    return FernschreiberUtils::getMessageShortText(lastMessage(CONTENT).toMap(), this->userInformation.value(ID).toLongLong() == senderUserId());
}

QString ChatListModel::ChatData::senderMessageStatus() const
{
    if (isChannel() || this->userInformation.value(ID).toLongLong() != senderUserId() || this->userInformation.value(ID).toLongLong() == chatId) {
        return "";
    }
    if (lastMessage(ID) == chatData.value(LAST_READ_OUTBOX_MESSAGE_ID)) {
        return "&nbsp;&nbsp;✅";
    } else {
        QVariantMap lastMessage = chatData.value(LAST_MESSAGE).toMap();
        if (lastMessage.contains(SENDING_STATE)) {
            QVariantMap sendingState = lastMessage.value(SENDING_STATE).toMap();
            if (sendingState.value(_TYPE).toString() == "messageSendingStatePending") {
                return "&nbsp;&nbsp;🕙";
            } else {
                return "&nbsp;&nbsp;❌";
            }
        } else {
            return "&nbsp;&nbsp;☑️";
        }
    }
}

bool ChatListModel::ChatData::isChannel() const
{
    return chatData.value(TYPE).toMap().value(IS_CHANNEL).toBool();
}

bool ChatListModel::ChatData::isHidden() const
{
    // Cover all enum values so that compiler warns us when/if enum gets extended
    switch (chatType) {
    case TDLibWrapper::ChatTypeBasicGroup:
    case TDLibWrapper::ChatTypeSupergroup:
        switch (memberStatus) {
        case TDLibWrapper::ChatMemberStatusLeft:
        case TDLibWrapper::ChatMemberStatusUnknown:
            return true;
        case TDLibWrapper::ChatMemberStatusCreator:
        case TDLibWrapper::ChatMemberStatusAdministrator:
        case TDLibWrapper::ChatMemberStatusMember:
        case TDLibWrapper::ChatMemberStatusRestricted:
        case TDLibWrapper::ChatMemberStatusBanned:
            break;
        }
        break;
    case TDLibWrapper::ChatTypeUnknown:
    case TDLibWrapper::ChatTypePrivate:
    case TDLibWrapper::ChatTypeSecret:
        break;
    }
    return false;
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
    const QString prevSenderMessageStatus(senderMessageStatus());


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
    if (prevSenderMessageStatus != senderMessageStatus()) {
        changedRoles.append(RoleLastMessageStatus);
    }
    return changedRoles;
}

QVector<int> ChatListModel::ChatData::updateGroup(const TDLibWrapper::Group *group)
{
    QVector<int> changedRoles;

    if (group && groupId == group->groupId) {
        const TDLibWrapper::ChatMemberStatus groupMemberStatus = group->chatMemberStatus();
        if (memberStatus != groupMemberStatus) {
            memberStatus = groupMemberStatus;
            changedRoles.append(RoleChatMemberStatus);
        }
    }
    return changedRoles;
}

ChatListModel::ChatListModel(TDLibWrapper *tdLibWrapper) : showHiddenChats(false)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatLastMessageUpdated(QString, QString, QVariantMap)), this, SLOT(handleChatLastMessageUpdated(QString, QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatOrderUpdated(QString, QString)), this, SLOT(handleChatOrderUpdated(QString, QString)));
    connect(tdLibWrapper, SIGNAL(chatReadInboxUpdated(QString, QString, int)), this, SLOT(handleChatReadInboxUpdated(QString, QString, int)));
    connect(tdLibWrapper, SIGNAL(chatReadOutboxUpdated(QString, QString)), this, SLOT(handleChatReadOutboxUpdated(QString, QString)));
    connect(tdLibWrapper, SIGNAL(chatPhotoUpdated(qlonglong, QVariantMap)), this, SLOT(handleChatPhotoUpdated(qlonglong, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatPinnedMessageUpdated(qlonglong, qlonglong)), this, SLOT(handleChatPinnedMessageUpdated(qlonglong, qlonglong)));
    connect(tdLibWrapper, SIGNAL(messageSendSucceeded(QString, QString, QVariantMap)), this, SLOT(handleMessageSendSucceeded(QString, QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(chatNotificationSettingsUpdated(QString, QVariantMap)), this, SLOT(handleChatNotificationSettingsUpdated(QString, QVariantMap)));
    connect(tdLibWrapper, SIGNAL(superGroupUpdated(qlonglong)), this, SLOT(handleGroupUpdated(qlonglong)));
    connect(tdLibWrapper, SIGNAL(basicGroupUpdated(qlonglong)), this, SLOT(handleGroupUpdated(qlonglong)));

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
    qDeleteAll(hiddenChats.values());
}

QHash<int,QByteArray> ChatListModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(ChatData::RoleDisplay, "display");
    roles.insert(ChatData::RoleChatId, "chat_id");
    roles.insert(ChatData::RoleChatType, "chat_type");
    roles.insert(ChatData::RoleTitle, "title");
    roles.insert(ChatData::RolePhotoSmall, "photo_small");
    roles.insert(ChatData::RoleUnreadCount, "unread_count");
    roles.insert(ChatData::RoleLastReadInboxMessageId, "last_read_inbox_message_id");
    roles.insert(ChatData::RoleLastMessageSenderId, "last_message_sender_id");
    roles.insert(ChatData::RoleLastMessageDate, "last_message_date");
    roles.insert(ChatData::RoleLastMessageText, "last_message_text");
    roles.insert(ChatData::RoleLastMessageStatus, "last_message_status");
    roles.insert(ChatData::RoleChatMemberStatus, "chat_member_status");
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
        case ChatData::RoleChatType: return data->chatType;
        case ChatData::RoleTitle: return data->title();
        case ChatData::RolePhotoSmall: return data->photoSmall();
        case ChatData::RoleUnreadCount: return data->unreadCount();
        case ChatData::RoleLastReadInboxMessageId: return data->lastReadInboxMessageId();
        case ChatData::RoleLastMessageSenderId: return data->senderUserId();
        case ChatData::RoleLastMessageText: return data->senderMessageText();
        case ChatData::RoleLastMessageDate: return data->senderMessageDate();
        case ChatData::RoleLastMessageStatus: return data->senderMessageStatus();
        case ChatData::RoleChatMemberStatus: return data->memberStatus;
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

QVariantMap ChatListModel::get(int row)
{

    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    QModelIndex idx = index(row, 0);
    while (i.hasNext()) {
        i.next();
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}

QVariantMap ChatListModel::getById(qlonglong chatId)
{
    if (chatIndexMap.contains(chatId)) {
        return chatList.value(chatIndexMap.value(chatId))->chatData;
    }
    return QVariantMap();
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

void ChatListModel::addVisibleChat(ChatData *chat)
{
    const int n = chatList.size();
    int pos;
    for (pos = 0; pos < n && chat->compareTo(chatList.at(pos)) >= 0; pos++);
    LOG("Adding chat" << chat->chatId << "at" << pos);
    beginInsertRows(QModelIndex(), pos, pos);
    chatList.insert(pos, chat);
    chatIndexMap.insert(chat->chatId, pos);
    // Update damaged part of the map
    for (int i = pos + 1; i <= n; i++) {
        chatIndexMap.insert(chatList.at(i)->chatId, i);
    }
    endInsertRows();
    if (this->tdLibWrapper->getJoinChatRequested()) {
        this->tdLibWrapper->registerJoinChat();
        emit chatJoined(chat->chatId, chat->chatData.value("title").toString());
    }
}

void ChatListModel::updateChatVisibility(const TDLibWrapper::Group *group)
{
    LOG("Updating chat visibility" << group->groupId);
    // See if any group has been removed from from view
    for (int i = 0; i < chatList.size(); i++) {
        ChatData *chat = chatList.at(i);
        const QVector<int> changedRoles(chat->updateGroup(group));
        if (chat->isHidden() && !showHiddenChats) {
            LOG("Hiding chat" << chat->chatId << "at" << i);
            beginRemoveRows(QModelIndex(), i, i);
            chatList.removeAt(i);
            // Update damaged part of the map
            const int n = chatList.size();
            for (int pos = i; pos < n; pos++) {
                chatIndexMap.insert(chatList.at(pos)->chatId, pos);
            }
            i--;
            hiddenChats.insert(chat->chatId, chat);
            endRemoveRows();
        } else if (!changedRoles.isEmpty()) {
            const QModelIndex modelIndex(index(i));
            emit dataChanged(modelIndex, modelIndex, changedRoles);
        }
    }

    // And see if any group been added to the view
    const QList<ChatData*> hiddenChatList = hiddenChats.values();
    const int n = hiddenChatList.size();
    for (int j = 0; j < n; j++) {
        ChatData *chat = hiddenChatList.at(j);
        chat->updateGroup(group);
        if (!chat->isHidden() || showHiddenChats) {
            hiddenChats.remove(chat->chatId);
            addVisibleChat(chat);
        }
    }
}

bool ChatListModel::showAllChats() const
{
    return showHiddenChats;
}

void ChatListModel::setShowAllChats(bool showAll)
{
    if (showHiddenChats != showAll) {
        showHiddenChats = showAll;
        updateChatVisibility(Q_NULLPTR);
        emit showAllChatsChanged();
    }
}

void ChatListModel::handleChatDiscovered(const QString &, const QVariantMap &chatToBeAdded)
{
    ChatData *chat = new ChatData(chatToBeAdded, tdLibWrapper->getUserInformation());
    const TDLibWrapper::Group *group = tdLibWrapper->getGroup(chat->groupId);
    if (group) {
        chat->updateGroup(group);
    }
    if (chat->isHidden()) {
        LOG("Hidden chat" << chat->chatId);
        hiddenChats.insert(chat->chatId, chat);
    } else {
        addVisibleChat(chat);

        // Start timestamp refresh timer when the first visible chat is discovered
        if (!relativeTimeRefreshTimer->isActive()) {
            relativeTimeRefreshTimer->start();
        }
    }
}

void ChatListModel::handleChatLastMessageUpdated(const QString &id, const QString &order, const QVariantMap &lastMessage)
{
    bool ok;
    const qlonglong chatId = id.toLongLong(&ok);
    if (ok) {
        if (chatIndexMap.contains(chatId)) {
            int chatIndex = chatIndexMap.value(chatId);
            LOG("Updating last message for chat" << chatId <<" at index" << chatIndex << "new order" << order);
            ChatData *chat = chatList.at(chatIndex);
            if (chat->setOrder(order)) {
                chatIndex = updateChatOrder(chatIndex);
            }
            const QModelIndex modelIndex(index(chatIndex));
            emit dataChanged(modelIndex, modelIndex, chat->updateLastMessage(lastMessage));
            emit chatChanged(chatId);
        } else {
            ChatData *chat = hiddenChats.value(chatId);
            if (chat) {
                LOG("Updating last message for hidden chat" << chatId << "new order" << order);
                chat->setOrder(order);
                chat->chatData.insert(LAST_MESSAGE, lastMessage);
            }
        }
    }
}

void ChatListModel::handleChatOrderUpdated(const QString &id, const QString &order)
{
    bool ok;
    const qlonglong chatId = id.toLongLong(&ok);
    if (ok) {
        if (chatIndexMap.contains(chatId)) {
            LOG("Updating chat order of" << chatId << "to" << order);
            int chatIndex = chatIndexMap.value(chatId);
            if (chatList.at(chatIndex)->setOrder(order)) {
                updateChatOrder(chatIndex);
            }
        } else {
            ChatData *chat = hiddenChats.value(chatId);
            if (chat) {
                LOG("Updating order of hidden chat" << chatId << "to" << order);
                chat->setOrder(order);
            }
        }
    }
}

void ChatListModel::handleChatReadInboxUpdated(const QString &id, const QString &lastReadInboxMessageId, int unreadCount)
{
    bool ok;
    const qlonglong chatId = id.toLongLong(&ok);
    if (ok) {
        const qlonglong messageId = lastReadInboxMessageId.toLongLong();
        if (chatIndexMap.contains(chatId)) {
            LOG("Updating chat unread count for" << chatId << "unread messages" << unreadCount << ", last read message ID: " << lastReadInboxMessageId);
            const int chatIndex = chatIndexMap.value(chatId);
            ChatData *chat = chatList.at(chatIndex);
            QVector<int> changedRoles;
            changedRoles.append(ChatData::RoleDisplay);
            if (chat->updateUnreadCount(unreadCount)) {
                changedRoles.append(ChatData::RoleUnreadCount);
            }
            if (chat->updateLastReadInboxMessageId(messageId)) {
                changedRoles.append(ChatData::RoleLastReadInboxMessageId);
            }
            const QModelIndex modelIndex(index(chatIndex));
            emit dataChanged(modelIndex, modelIndex, changedRoles);
        } else {
            ChatData *chat = hiddenChats.value(chatId);
            if (chat) {
                LOG("Updating unread count for hidden chat" << chatId << "unread messages" << unreadCount << ", last read message ID: " << lastReadInboxMessageId);
                chat->updateUnreadCount(unreadCount);
                chat->updateLastReadInboxMessageId(messageId);
            }
        }
    }
}

void ChatListModel::handleChatReadOutboxUpdated(const QString &id, const QString &lastReadOutboxMessageId)
{
    bool ok;
    const qlonglong chatId = id.toLongLong(&ok);
    if (ok) {
        if (chatIndexMap.contains(chatId)) {
            LOG("Updating last read message for" << chatId << "last ID" << lastReadOutboxMessageId);
            const int chatIndex = chatIndexMap.value(chatId);
            ChatData *chat = chatList.at(chatIndex);
            chat->chatData.insert(LAST_READ_OUTBOX_MESSAGE_ID, lastReadOutboxMessageId);
            const QModelIndex modelIndex(index(chatIndex));
            emit dataChanged(modelIndex, modelIndex);
        } else {
            ChatData *chat = hiddenChats.value(chatId);
            if (chat) {
                chat->chatData.insert(LAST_READ_OUTBOX_MESSAGE_ID, lastReadOutboxMessageId);
            }
        }
    }
}

void ChatListModel::handleChatPhotoUpdated(qlonglong chatId, const QVariantMap &photo)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating chat photo for" << chatId);
        const int chatIndex = chatIndexMap.value(chatId);
        ChatData *chat = chatList.at(chatIndex);
        chat->chatData.insert(PHOTO, photo);
        QVector<int> changedRoles;
        changedRoles.append(ChatData::RolePhotoSmall);
        const QModelIndex modelIndex(index(chatIndex));
        emit dataChanged(modelIndex, modelIndex, changedRoles);
    } else {
        ChatData *chat = hiddenChats.value(chatId);
        if (chat) {
            LOG("Updating photo for hidden chat" << chatId);
            chat->chatData.insert(PHOTO, photo);
        }
    }
}

void ChatListModel::handleChatPinnedMessageUpdated(qlonglong chatId, qlonglong pinnedMessageId)
{
    if (chatIndexMap.contains(chatId)) {
        LOG("Updating pinned message for" << chatId);
        const int chatIndex = chatIndexMap.value(chatId);
        ChatData *chat = chatList.at(chatIndex);
        chat->chatData.insert(PINNED_MESSAGE_ID, pinnedMessageId);
    } else {
        ChatData *chat = hiddenChats.value(chatId);
        if (chat) {
            LOG("Updating pinned message for hidden chat" << chatId);
            chat->chatData.insert(PINNED_MESSAGE_ID, pinnedMessageId);
        }
    }
}

void ChatListModel::handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message)
{
    bool ok;
    const qlonglong chatId(message.value(CHAT_ID).toLongLong(&ok));
    if (ok) {
        if (chatIndexMap.contains(chatId)) {
            const int chatIndex = chatIndexMap.value(chatId);
            LOG("Updating last message for chat" << chatId << "at index" << chatIndex << ", as message was sent, old ID:" << oldMessageId << ", new ID:" << messageId);
            const QModelIndex modelIndex(index(chatIndex));
            emit dataChanged(modelIndex, modelIndex, chatList.at(chatIndex)->updateLastMessage(message));
        } else {
            ChatData *chat = hiddenChats.value(chatId);
            if (chat) {
                LOG("Updating last message for hidden chat" << chatId << ", as message was sent, old ID:" << oldMessageId << ", new ID:" << messageId);
                chat->chatData.insert(LAST_MESSAGE, message);
            }
        }
    }
}

void ChatListModel::handleChatNotificationSettingsUpdated(const QString &id, const QVariantMap &chatNotificationSettings)
{
    bool ok;
    const qlonglong chatId = id.toLongLong(&ok);
    if (ok) {
        if (chatIndexMap.contains(chatId)) {
            const int chatIndex = chatIndexMap.value(chatId);
            LOG("Updating notification settings for chat" << chatId << "at index" << chatIndex);
            ChatData *chat = chatList.at(chatIndex);
            chat->chatData.insert(NOTIFICATION_SETTINGS, chatNotificationSettings);
            const QModelIndex modelIndex(index(chatIndex));
            emit dataChanged(modelIndex, modelIndex);
        } else {
            ChatData *chat = hiddenChats.value(chatId);
            if (chat) {
                chat->chatData.insert(NOTIFICATION_SETTINGS, chatNotificationSettings);
            }
        }
    }
}

void ChatListModel::handleGroupUpdated(qlonglong groupId)
{
    updateChatVisibility(tdLibWrapper->getGroup(groupId));
}

void ChatListModel::handleRelativeTimeRefreshTimer()
{
    LOG("Refreshing timestamps");
    QVector<int> roles;
    roles.append(ChatData::RoleLastMessageDate);
    emit dataChanged(index(0), index(chatList.size() - 1), roles);
}
