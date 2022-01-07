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

#include "chatmodel.h"

#include <QListIterator>
#include <QByteArray>
#include <QBitArray>

#define DEBUG_MODULE ChatModel
#include "debuglog.h"

namespace {
    const QString ID("id");
    const QString CONTENT("content");
    const QString CHAT_ID("chat_id");
    const QString PHOTO("photo");
    const QString SMALL("small");
    const QString UNREAD_COUNT("unread_count");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");
    const QString SENDER_ID("sender_id");
    const QString USER_ID("user_id");
    const QString MESSAGE_ID("message_id");
    const QString PINNED_MESSAGE_ID("pinned_message_id");
    const QString REPLY_MARKUP("reply_markup");
    const QString _TYPE("@type");

    // "interaction_info": {
    //     "@type": "messageInteractionInfo",
    //     "forward_count": 0,
    //     "view_count": 47
    // }
    const QString TYPE_MESSAGE_INTERACTION_INFO("messageInteractionInfo");
    const QString INTERACTION_INFO("interaction_info");
    const QString VIEW_COUNT("view_count");

    const QString TYPE_SPONSORED_MESSAGE("sponsoredMessage");
}

class ChatModel::MessageData
{
public:
    enum Role {
        RoleDisplay = Qt::DisplayRole,
        RoleMessageId,
        RoleMessageContentType,
        RoleMessageViewCount
    };

    enum RoleFlag {
        RoleFlagDisplay = 0x01,
        RoleFlagMessageId = 0x02,
        RoleFlagMessageContentType = 0x04,
        RoleFlagMessageViewCount = 0x08
    };

    MessageData(const QVariantMap &data, qlonglong msgid);

    static bool lessThan(const MessageData *message1, const MessageData *message2);
    static QVector<int> flagsToRoles(uint flags);

    uint updateMessageData(const QVariantMap &data);
    uint updateContent(const QVariantMap &content);
    uint updateContentType(const QVariantMap &content);
    uint updateReplyMarkup(const QVariantMap &replyMarkup);
    uint updateViewCount(const QVariantMap &interactionInfo);
    uint updateInteractionInfo(const QVariantMap &interactionInfo);

    QVector<int> diff(const MessageData *message) const;
    QVector<int> setMessageData(const QVariantMap &data);
    QVector<int> setContent(const QVariantMap &content);
    QVector<int> setReplyMarkup(const QVariantMap &replyMarkup);
    QVector<int> setInteractionInfo(const QVariantMap &interactionInfo);

    int senderUserId() const;
    qlonglong senderChatId() const;
    bool senderIsChat() const;

public:
    QVariantMap messageData;
    const qlonglong messageId;
    QString messageType;
    QString messageContentType;
    int viewCount;
};

ChatModel::MessageData::MessageData(const QVariantMap &data, qlonglong msgid) :
    messageData(data),
    messageId(msgid),
    messageType(data.value(_TYPE).toString()),
    messageContentType(data.value(CONTENT).toMap().value(_TYPE).toString()),
    viewCount(data.value(INTERACTION_INFO).toMap().value(VIEW_COUNT).toInt())
{
}

QVector<int> ChatModel::MessageData::flagsToRoles(uint flags)
{
    QVector<int> roles;
    if (flags & RoleFlagDisplay) {
        roles.append(RoleDisplay);
    }
    if (flags & RoleFlagMessageId) {
        roles.append(RoleMessageId);
    }
    if (flags & RoleFlagMessageContentType) {
        roles.append(RoleMessageContentType);
    }
    if (flags & RoleFlagMessageViewCount) {
        roles.append(RoleMessageViewCount);
    }
    return roles;
}

int ChatModel::MessageData::senderUserId() const
{
    return messageData.value(SENDER_ID).toMap().value(USER_ID).toInt();
}

qlonglong ChatModel::MessageData::senderChatId() const
{
    return messageData.value(SENDER_ID).toMap().value(CHAT_ID).toLongLong();
}

bool ChatModel::MessageData::senderIsChat() const
{
    return messageData.value(SENDER_ID).toMap().value(_TYPE).toString() == "messageSenderChat";
}

QVector<int> ChatModel::MessageData::diff(const MessageData *message) const
{
    QVector<int> roles;
    if (message != this) {
        roles.append(RoleDisplay);
        if (message->messageId != messageId) {
            roles.append(RoleMessageId);
        }
        if (message->messageContentType != messageContentType) {
            roles.append(RoleMessageContentType);
        }
        if (message->viewCount != viewCount) {
            roles.append(RoleMessageViewCount);
        }
    }
    return roles;
}

uint ChatModel::MessageData::updateMessageData(const QVariantMap &data)
{
    messageData = data;
    messageType = data.value(_TYPE).toString();
    return RoleFlagDisplay |
        updateContentType(data.value(CONTENT).toMap()) |
        updateViewCount(data.value(INTERACTION_INFO).toMap());
}

QVector<int> ChatModel::MessageData::setMessageData(const QVariantMap &data)
{
    return flagsToRoles(updateMessageData(data));
}

uint ChatModel::MessageData::updateContentType(const QVariantMap &content)
{
    const QString oldContentType(messageContentType);
    messageContentType = content.value(_TYPE).toString();
    return (oldContentType == messageContentType) ? 0 : RoleFlagMessageContentType;
}

uint ChatModel::MessageData::updateContent(const QVariantMap &content)
{
    messageData.insert(CONTENT, content);
    return RoleFlagDisplay | updateContentType(content);
}

QVector<int> ChatModel::MessageData::setContent(const QVariantMap &content)
{
    return flagsToRoles(updateContent(content));
}

uint ChatModel::MessageData::updateReplyMarkup(const QVariantMap &replyMarkup)
{
    messageData.insert(REPLY_MARKUP, replyMarkup);
    return RoleFlagDisplay;
}

QVector<int> ChatModel::MessageData::setReplyMarkup(const QVariantMap &replyMarkup)
{
    return flagsToRoles(updateReplyMarkup(replyMarkup));
}

uint ChatModel::MessageData::updateViewCount(const QVariantMap &interactionInfo)
{
    const int oldViewCount = viewCount;
    viewCount = interactionInfo.value(VIEW_COUNT).toInt();
    return (viewCount == oldViewCount) ? 0 : RoleFlagMessageViewCount;
}

uint ChatModel::MessageData::updateInteractionInfo(const QVariantMap &interactionInfo)
{
    if (interactionInfo.value(_TYPE) == TYPE_MESSAGE_INTERACTION_INFO) {
        messageData.insert(INTERACTION_INFO, interactionInfo);
        return RoleFlagDisplay | updateViewCount(interactionInfo);
    }
    return 0;
}

QVector<int> ChatModel::MessageData::setInteractionInfo(const QVariantMap &info)
{
    return flagsToRoles(updateInteractionInfo(info));
}

bool ChatModel::MessageData::lessThan(const MessageData *message1, const MessageData *message2)
{
    bool message1Sponsored = message1->messageType == TYPE_SPONSORED_MESSAGE;
    bool message2Sponsored = message2->messageType == TYPE_SPONSORED_MESSAGE;
    if (message1Sponsored && message2Sponsored) {
        return message1->messageId < message2->messageId;
    }
    if (message1Sponsored && !message2Sponsored) {
        return false;
    }
    if (!message1Sponsored && message2Sponsored) {
        return true;
    }
    return message1->messageId < message2->messageId;
}

ChatModel::ChatModel(TDLibWrapper *tdLibWrapper) :
    chatId(0),
    inReload(false),
    inIncrementalUpdate(false),
    searchModeActive(false)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(this->tdLibWrapper, SIGNAL(messagesReceived(QVariantList, int)), this, SLOT(handleMessagesReceived(QVariantList, int)));
    connect(this->tdLibWrapper, SIGNAL(sponsoredMessageReceived(qlonglong, QVariantMap)), this, SLOT(handleSponsoredMessageReceived(qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(newMessageReceived(qlonglong, QVariantMap)), this, SLOT(handleNewMessageReceived(qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(receivedMessage(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageReceived(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatReadInboxUpdated(QString, QString, int)), this, SLOT(handleChatReadInboxUpdated(QString, QString, int)));
    connect(this->tdLibWrapper, SIGNAL(chatReadOutboxUpdated(QString, QString)), this, SLOT(handleChatReadOutboxUpdated(QString, QString)));
    connect(this->tdLibWrapper, SIGNAL(messageSendSucceeded(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageSendSucceeded(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatNotificationSettingsUpdated(QString, QVariantMap)), this, SLOT(handleChatNotificationSettingsUpdated(QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatPhotoUpdated(qlonglong, QVariantMap)), this, SLOT(handleChatPhotoUpdated(qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatPinnedMessageUpdated(qlonglong, qlonglong)), this, SLOT(handleChatPinnedMessageUpdated(qlonglong, qlonglong)));
    connect(this->tdLibWrapper, SIGNAL(messageContentUpdated(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageContentUpdated(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(messageEditedUpdated(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageEditedUpdated(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(messageInteractionInfoUpdated(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageInteractionInfoUpdated(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(messagesDeleted(qlonglong, QList<qlonglong>)), this, SLOT(handleMessagesDeleted(qlonglong, QList<qlonglong>)));
}

ChatModel::~ChatModel()
{
    LOG("Destroying myself...");
    qDeleteAll(messages);
}

QHash<int,QByteArray> ChatModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(MessageData::RoleDisplay, "display");
    roles.insert(MessageData::RoleMessageId, "message_id");
    roles.insert(MessageData::RoleMessageContentType, "content_type");
    roles.insert(MessageData::RoleMessageViewCount, "view_count");
    return roles;
}

int ChatModel::rowCount(const QModelIndex &) const
{
    return messages.size();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row >= 0 && row < messages.size()) {
        const MessageData *message = messages.at(row);
        switch ((MessageData::Role)role) {
        case MessageData::RoleDisplay: return message->messageData;
        case MessageData::RoleMessageId: return message->messageId;
        case MessageData::RoleMessageContentType: return message->messageContentType;
        case MessageData::RoleMessageViewCount: return message->viewCount;
        }
    }
    return QVariant();
}

void ChatModel::clear(bool contentOnly)
{
    LOG("Clearing chat model");
    inReload = false;
    inIncrementalUpdate = false;
    searchModeActive = false;
    searchQuery.clear();
    if (!messages.isEmpty()) {
        beginResetModel();
        qDeleteAll(messages);
        messages.clear();
        messageIndexMap.clear();
        endResetModel();
    }

    if (!contentOnly) {
        if (!chatInformation.isEmpty()) {
            chatInformation.clear();
            emit smallPhotoChanged();
        }
        if (chatId) {
            chatId = 0;
            emit chatIdChanged();
        }
    }
}

void ChatModel::initialize(const QVariantMap &chatInformation)
{
    const qlonglong chatId = chatInformation.value(ID).toLongLong();
    LOG("Initializing chat model..." << chatId);
    beginResetModel();
    qDeleteAll(messages);
    this->chatInformation = chatInformation;
    this->chatId = chatId;
    this->messages.clear();
    this->messageIndexMap.clear();
    this->searchQuery.clear();
    endResetModel();
    emit chatIdChanged();
    emit smallPhotoChanged();
    tdLibWrapper->getChatHistory(chatId, this->chatInformation.value(LAST_READ_INBOX_MESSAGE_ID).toLongLong());
}

void ChatModel::triggerLoadMoreHistory()
{
    if (!this->inIncrementalUpdate && !messages.isEmpty()) {
        if (searchModeActive) {
            LOG("Trigger loading older found messages...");
            this->inIncrementalUpdate = true;
            this->tdLibWrapper->searchChatMessages(chatId, searchQuery, messages.first()->messageId);
        } else {
            LOG("Trigger loading older history...");
            this->inIncrementalUpdate = true;
            this->tdLibWrapper->getChatHistory(chatId, messages.first()->messageId);
        }
    }
}

void ChatModel::triggerLoadMoreFuture()
{
    if (!this->inIncrementalUpdate && !messages.isEmpty() && !searchModeActive) {
        LOG("Trigger loading newer future...");
        this->inIncrementalUpdate = true;
        this->tdLibWrapper->getChatHistory(chatId, messages.last()->messageId, -49);
    }
}

QVariantMap ChatModel::getChatInformation()
{
    return this->chatInformation;
}

QVariantMap ChatModel::getMessage(int index)
{
    if (index >= 0 && index < messages.size()) {
        return messages.at(index)->messageData;
    }
    return QVariantMap();
}

int ChatModel::getLastReadMessageIndex()
{
    LOG("Obtaining last read message index");
    return this->calculateLastKnownMessageId();
}

void ChatModel::setSearchQuery(const QString newSearchQuery)
{
    if (this->searchQuery != newSearchQuery) {
        this->clear(true);
        this->searchQuery = newSearchQuery;
        this->searchModeActive = !this->searchQuery.isEmpty();
        if (this->searchModeActive) {
            this->tdLibWrapper->searchChatMessages(this->chatId, this->searchQuery);
        } else {
            this->tdLibWrapper->getChatHistory(chatId, this->chatInformation.value(LAST_READ_INBOX_MESSAGE_ID).toLongLong());
        }
    }
}

QVariantMap ChatModel::smallPhoto() const
{
    return chatInformation.value(PHOTO).toMap().value(SMALL).toMap();
}

qlonglong ChatModel::getChatId() const
{
    return chatId;
}

void ChatModel::handleMessagesReceived(const QVariantList &messages, int totalCount)
{
    LOG("Receiving new messages :)" << messages.size());
    LOG("Received while search mode is" << searchModeActive);

    if (messages.size() == 0) {
        LOG("No additional messages loaded, notifying chat UI...");
        this->inReload = false;
        int listInboxPosition = this->calculateLastKnownMessageId();
        int listOutboxPosition = this->calculateLastReadSentMessageId();
        listInboxPosition = this->calculateScrollPosition(listInboxPosition);
        if (this->inIncrementalUpdate) {
            this->inIncrementalUpdate = false;
            emit messagesIncrementalUpdate(listInboxPosition, listOutboxPosition);
        } else {
            emit messagesReceived(listInboxPosition, listOutboxPosition, totalCount);
        }
    } else {
        if (this->isMostRecentMessageLoaded() || this->inIncrementalUpdate) {
            QList<MessageData*> messagesToBeAdded;
            QListIterator<QVariant> messagesIterator(messages);

            while (messagesIterator.hasNext()) {
                const QVariantMap messageData = messagesIterator.next().toMap();
                const qlonglong messageId = messageData.value(ID).toLongLong();
                if (messageId && messageData.value(CHAT_ID).toLongLong() == chatId && !messageIndexMap.contains(messageId)) {
                    LOG("New message will be added:" << messageId);
                    messagesToBeAdded.append(new MessageData(messageData, messageId));
                }
            }

            std::sort(messagesToBeAdded.begin(), messagesToBeAdded.end(), MessageData::lessThan);

            if (!messagesToBeAdded.isEmpty()) {
                insertMessages(messagesToBeAdded);
            }

            // First call only returns a few messages, we need to get a little more than that...
            if (!messagesToBeAdded.isEmpty() && (messagesToBeAdded.size() + messages.size()) < 10 && !inReload) {
                LOG("Only a few messages received in first call, loading more...");
                this->inReload = true;
                if (this->searchModeActive) {
                    this->tdLibWrapper->searchChatMessages(chatId, searchQuery, messagesToBeAdded.first()->messageId);
                } else {
                    this->tdLibWrapper->getChatHistory(chatId, messagesToBeAdded.first()->messageId, 0);
                }
            } else {
                LOG("Messages loaded, notifying chat UI...");
                this->inReload = false;
                int listInboxPosition = this->calculateLastKnownMessageId();
                int listOutboxPosition = this->calculateLastReadSentMessageId();
                listInboxPosition = this->calculateScrollPosition(listInboxPosition);
                if (this->inIncrementalUpdate) {
                    this->inIncrementalUpdate = false;
                    emit messagesIncrementalUpdate(listInboxPosition, listOutboxPosition);
                } else {
                    emit messagesReceived(listInboxPosition, listOutboxPosition, totalCount);
                }
            }
        } else {
            // Cleanup... Is that really needed? Well, let's see...
            this->inReload = false;
            this->inIncrementalUpdate = false;
            LOG("New messages in this chat, but not relevant as less recent messages need to be loaded first!");
        }
    }

}

void ChatModel::handleSponsoredMessageReceived(qlonglong chatId, const QVariantMap &sponsoredMessage)
{
    LOG("Handling sponsored message" << chatId);
    QList<MessageData*> messagesToBeAdded;
    const qlonglong messageId = sponsoredMessage.value(MESSAGE_ID).toLongLong();
    if (messageId && !messageIndexMap.contains(messageId)) {
        LOG("New sponsored message will be added:" << messageId);
        messagesToBeAdded.append(new MessageData(sponsoredMessage, messageId));
    }
    appendMessages(messagesToBeAdded);
}

void ChatModel::handleNewMessageReceived(qlonglong chatId, const QVariantMap &message)
{
    const qlonglong messageId = message.value(ID).toLongLong();
    if (chatId == this->chatId && !messageIndexMap.contains(messageId)) {
        if (this->isMostRecentMessageLoaded() && !this->searchModeActive) {
            LOG("New message received for this chat");
            QList<MessageData*> messagesToBeAdded;
            messagesToBeAdded.append(new MessageData(message, messageId));
            insertMessages(messagesToBeAdded);
            emit newMessageReceived(message);
        } else {
            LOG("New message in this chat, but not relevant as less recent messages need to be loaded first!");
        }
    }
}

void ChatModel::handleMessageReceived(qlonglong chatId, qlonglong messageId, const QVariantMap &message)
{
    if (chatId == this->chatId && messageIndexMap.contains(messageId)) {
        LOG("Received a message that we already know, let's update it!");
        const int position = messageIndexMap.value(messageId);
        const QVector<int> changedRoles(messages.at(position)->setMessageData(message));
        LOG("Message was updated at index" << position);
        const QModelIndex messageIndex(index(position));
        emit dataChanged(messageIndex, messageIndex, changedRoles);
    }
}

void ChatModel::handleChatReadInboxUpdated(const QString &id, const QString &lastReadInboxMessageId, int unreadCount)
{
    if (id.toLongLong() == chatId) {
        LOG("Updating chat unread count, unread messages" << unreadCount << ", last read message ID:" << lastReadInboxMessageId);
        this->chatInformation.insert("unread_count", unreadCount);
        this->chatInformation.insert(LAST_READ_INBOX_MESSAGE_ID, lastReadInboxMessageId);
        emit unreadCountUpdated(unreadCount, lastReadInboxMessageId);
    }
}

void ChatModel::handleChatReadOutboxUpdated(const QString &id, const QString &lastReadOutboxMessageId)
{
    if (id.toLongLong() == chatId) {
        this->chatInformation.insert(LAST_READ_OUTBOX_MESSAGE_ID, lastReadOutboxMessageId);
        int sentIndex = calculateLastReadSentMessageId();
        LOG("Updating sent message ID, new index" << sentIndex);
        emit lastReadSentMessageUpdated(sentIndex);
    }
}

void ChatModel::handleMessageSendSucceeded(qlonglong messageId, qlonglong oldMessageId, const QVariantMap &message)
{
    LOG("Message send succeeded, new message ID" << messageId << "old message ID" << oldMessageId << ", chat ID" << message.value(CHAT_ID).toString());
    LOG("index map:" << messageIndexMap.contains(oldMessageId) << ", index count:" << messageIndexMap.size() << ", message count:" << messages.size());
    if (this->messageIndexMap.contains(oldMessageId)) {
        LOG("Message was successfully sent" << oldMessageId);
        const int pos = messageIndexMap.take(oldMessageId);
        MessageData* oldMessage = messages.at(pos);
        MessageData* newMessage = new MessageData(message, messageId);
        messages.replace(pos, newMessage);
        messageIndexMap.remove(oldMessageId);
        messageIndexMap.insert(messageId, pos);
        const QVector<int> changedRoles(newMessage->diff(oldMessage));
        delete oldMessage;
        LOG("Message was replaced at index" << pos);
        const QModelIndex messageIndex(index(pos));
        emit dataChanged(messageIndex, messageIndex, changedRoles);
        emit lastReadSentMessageUpdated(calculateLastReadSentMessageId());
        tdLibWrapper->viewMessage(QString::number(this->chatId), QString::number(messageId), false);
    }
}

void ChatModel::handleChatNotificationSettingsUpdated(const QString &id, const QVariantMap &chatNotificationSettings)
{
    if (id.toLongLong() == chatId) {
        this->chatInformation.insert("notification_settings", chatNotificationSettings);
        LOG("Notification settings updated");
        emit notificationSettingsUpdated();
    }
}

void ChatModel::handleChatPhotoUpdated(qlonglong id, const QVariantMap &photo)
{
    if (id == chatId) {
        LOG("Chat photo updated" << chatId);
        chatInformation.insert(PHOTO, photo);
        emit smallPhotoChanged();
    }
}

void ChatModel::handleChatPinnedMessageUpdated(qlonglong id, qlonglong pinnedMessageId)
{
    if (id == chatId) {
        LOG("Pinned message updated" << chatId);
        chatInformation.insert(PINNED_MESSAGE_ID, pinnedMessageId);
        emit pinnedMessageChanged();
    }
}

void ChatModel::handleMessageContentUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &newContent)
{
    LOG("Message content updated" << chatId << messageId);
    if (chatId == this->chatId && messageIndexMap.contains(messageId)) {
        LOG("We know the message that was updated" << messageId);
        const int pos = messageIndexMap.value(messageId, -1);
        if (pos >= 0) {
            const QVector<int> changedRoles(messages.at(pos)->setContent(newContent));
            LOG("Message was updated at index" << pos);
            const QModelIndex messageIndex(index(pos));
            emit dataChanged(messageIndex, messageIndex, changedRoles);
            emit messageUpdated(pos);
        }
    }
}

void ChatModel::handleMessageInteractionInfoUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &updatedInfo)
{
    if (chatId == this->chatId && messageIndexMap.contains(messageId)) {
        const int pos = messageIndexMap.value(messageId, -1);
        if (pos >= 0) {
            LOG("Message interaction info was updated at index" << pos);
            const QVector<int> changedRoles(messages.at(pos)->setInteractionInfo(updatedInfo));
            const QModelIndex messageIndex(index(pos));
            emit dataChanged(messageIndex, messageIndex, changedRoles);
        }
    }
}

void ChatModel::handleMessageEditedUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &replyMarkup)
{
    LOG("Message edited updated" << chatId << messageId);
    if (chatId == this->chatId && messageIndexMap.contains(messageId)) {
        LOG("We know the message that was updated" << messageId);
        const int pos = messageIndexMap.value(messageId, -1);
        if (pos >= 0) {
            const QVector<int> changedRoles(messages.at(pos)->setReplyMarkup(replyMarkup));
            LOG("Message was edited at index" << pos);
            const QModelIndex messageIndex(index(pos));
            emit dataChanged(messageIndex, messageIndex, changedRoles);
            emit messageUpdated(pos);
        }
    }
}

void ChatModel::handleMessagesDeleted(qlonglong chatId, const QList<qlonglong> &messageIds)
{
    LOG("Messages were deleted in a chat" << chatId);
    if (chatId == this->chatId) {
        const int count = messageIds.size();
        LOG(count << "messages in this chat were deleted...");

        int firstPosition = count, lastPosition = count;
        for (int i = (count - 1); i > -1; i--) {
            const int position = messageIndexMap.value(messageIds.at(i), -1);
            if (position >= 0) {
                // We found at least one message in our list that needs to be deleted
                if (lastPosition == count) {
                    lastPosition = position;
                }
                if (firstPosition == count) {
                    firstPosition = position;
                }
                if (position < (firstPosition - 1)) {
                    // Some gap in between, can remove previous range and reset positions
                    removeRange(firstPosition, lastPosition);
                    firstPosition = lastPosition = position;
                } else {
                    // No gap in between, extend the range and continue loop
                    firstPosition = position;
                }
            }
        }
        // After all elements have been processed, there may be one last range to remove
        // But only if we found at least one item to remove
        if (firstPosition != count && lastPosition != count) {
            removeRange(firstPosition, lastPosition);
        }
    }
}

void ChatModel::removeRange(int firstDeleted, int lastDeleted)
{
    if (firstDeleted >= 0 && firstDeleted <= lastDeleted) {
        LOG("Removing range" << firstDeleted << "..." << lastDeleted << "| current messages size" << messages.size());
        beginRemoveRows(QModelIndex(), firstDeleted, lastDeleted);
        for (int i = firstDeleted; i <= lastDeleted; i++) {
            MessageData *message = messages.at(i);
            messageIndexMap.remove(message->messageId);
            delete message;
        }
        messages.erase(messages.begin() + firstDeleted, messages.begin() + (lastDeleted + 1));
        endRemoveRows();
    }
}

void ChatModel::insertMessages(const QList<MessageData*> newMessages)
{
    // Caller ensures that newMessages is not empty
    if (messages.isEmpty()) {
        appendMessages(newMessages);
    } else if (!newMessages.isEmpty()) {
        // There is only an append or a prepend, tertium non datur! (probably ;))
        qlonglong lastKnownId = -1;
        for (int i = (messages.size() - 1); i >= 0; i-- ) {
            const MessageData* message = messages.at(i);
            if (message->messageType != TYPE_SPONSORED_MESSAGE) {
                lastKnownId = message->messageId;
            }
        }
        const qlonglong firstNewId = newMessages.first()->messageId;
        LOG("Inserting messages, last known ID:" << lastKnownId << ", first new ID:" << firstNewId);
        if (lastKnownId < firstNewId) {
            appendMessages(newMessages);
        } else {
            prependMessages(newMessages);
        }
    }
}

void ChatModel::appendMessages(const QList<MessageData*> newMessages)
{
    const int oldSize = messages.size();
    const int count = newMessages.size();
    LOG("Appending" << count << "new messages...");

    beginInsertRows(QModelIndex(), oldSize, oldSize + count - 1);
    messages.append(newMessages);
    for (int i = 0; i < count; i++) {
        // Appens new indeces to the map
        messageIndexMap.insert(newMessages.at(i)->messageId, oldSize + i);
    }
    endInsertRows();
}

void ChatModel::prependMessages(const QList<MessageData*> newMessages)
{
    const int insertCount = newMessages.size();
    const int totalCount = messages.size() + insertCount;
    LOG("Prepending" << insertCount << "messages...");

    beginInsertRows(QModelIndex(), 0, insertCount - 1);
    // Too bad there's no bulk insert
    messages.reserve(totalCount);
    int i;
    for (i = 0; i < insertCount; i++) {
        MessageData* message = newMessages.at(i);
        messages.insert(i, message);
        messageIndexMap.insert(message->messageId, i);
    }
    // The rest of the map has been damaged too
    for (; i < totalCount; i++) {
        messageIndexMap.insert(messages.at(i)->messageId, i);
    }
    endInsertRows();
}

QVariantMap ChatModel::enhanceMessage(const QVariantMap &message)
{
    QVariantMap enhancedMessage = message;
    if (enhancedMessage.value(CONTENT).toMap().value(_TYPE).toString() == "messageVoiceNote" ) {
        QVariantMap contentMap = enhancedMessage.value(CONTENT).toMap();
        QVariantMap voiceNoteMap = contentMap.value("voice_note").toMap();
        QByteArray waveBytes = QByteArray::fromBase64(voiceNoteMap.value("waveform").toByteArray());
        QBitArray waveBits(waveBytes.count() * 8);

        for (int i = 0; i < waveBytes.count(); i++) {
            for (int b = 0; b < 8; b++) {
                waveBits.setBit( i * 8 + b, waveBytes.at(i) & (1 << (7 - b)) );
            }
        }
        int waveSize = 10;
        int waveformSets = waveBits.size() / waveSize;
        QVariantList decodedWaveform;
        for (int i = 0; i < waveformSets; i++) {
            int waveformHeight = 0;
            for (int j = 0; j < waveSize; j++) {
                waveformHeight = waveformHeight + ( waveBits.at(i * waveSize + j) * (2 ^ (j)) );
            }
            decodedWaveform.append(waveformHeight);
        }
        voiceNoteMap.insert("decoded_voice_note", decodedWaveform);
        contentMap.insert("voice_note", voiceNoteMap);
        enhancedMessage.insert(CONTENT, contentMap);
    }
    return enhancedMessage;
}

int ChatModel::calculateLastKnownMessageId()
{
    LOG("calculateLastKnownMessageId");
    const qlonglong lastKnownMessageId = this->chatInformation.value(LAST_READ_INBOX_MESSAGE_ID).toLongLong();
    LOG("lastKnownMessageId" << lastKnownMessageId);
    const int myUserId = tdLibWrapper->getUserInformation().value(ID).toInt();
    qlonglong lastOwnMessageId = 0;
    for (int i = (messages.size() - 1); i >= 0; i--) {
        MessageData *currentMessage = messages.at(i);
        if (currentMessage->senderUserId() == myUserId) {
            lastOwnMessageId = currentMessage->messageId;
            break;
        }
    }
    LOG("size messageIndexMap" << messageIndexMap.size());
    LOG("contains last read ID?" << messageIndexMap.contains(lastKnownMessageId));
    LOG("contains last own ID?" << messageIndexMap.contains(lastOwnMessageId));
    int listInboxPosition = messageIndexMap.value(lastKnownMessageId, messages.size() - 1);
    int listOwnPosition = messageIndexMap.value(lastOwnMessageId, -1);
    if (listInboxPosition > this->messages.size() - 1 ) {
        listInboxPosition = this->messages.size() - 1;
    }
    if (listOwnPosition > this->messages.size() - 1 ) {
        listOwnPosition = -1;
    }
    LOG("Last known message is at position" << listInboxPosition);
    LOG("Last own message is at position" << listOwnPosition);
    return (listInboxPosition > listOwnPosition) ? listInboxPosition : listOwnPosition;
}

int ChatModel::calculateLastReadSentMessageId()
{
    LOG("calculateLastReadSentMessageId");
    const qlonglong lastReadSentMessageId = this->chatInformation.value(LAST_READ_OUTBOX_MESSAGE_ID).toLongLong();
    LOG("lastReadSentMessageId" << lastReadSentMessageId);
    LOG("size messageIndexMap" << messageIndexMap.size());
    LOG("contains ID?" << messageIndexMap.contains(lastReadSentMessageId));
    const int listOutboxPosition = messageIndexMap.value(lastReadSentMessageId, -1);
    LOG("Last read sent message is at position" << listOutboxPosition);
    emit lastReadSentMessageUpdated(listOutboxPosition);
    return listOutboxPosition;
}

int ChatModel::calculateScrollPosition(int listInboxPosition)
{
    LOG("Calculating new scroll position, current:" << listInboxPosition << ", list size:" << this->messages.size());
    if ((this->messages.size() - 1) > listInboxPosition) {
        return listInboxPosition + 1;
    } else {
        return listInboxPosition;
    }
}

bool ChatModel::isMostRecentMessageLoaded()
{
    // Need to check if we can actually add messages (only possible if the previously latest messages are loaded)
    // Trying with half of the size of an initial list to ensure that everything is there...
    return this->getLastReadMessageIndex() >= this->messages.size() - 25;
}
