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
#include <QDebug>

#define LOG(x) qDebug() << "[ChatModel]" << x

namespace {
    const QString ID("id");
    const QString CHAT_ID("chat_id");
    const QString PHOTO("photo");
    const QString SMALL("small");
    const QString UNREAD_COUNT("unread_count");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString SENDER_USER_ID("sender_user_id");
    const QString PINNED_MESSAGE_ID("pinned_message_id");
}

ChatModel::ChatModel(TDLibWrapper *tdLibWrapper) :
    chatId(0),
    inReload(false),
    inIncrementalUpdate(false)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(this->tdLibWrapper, SIGNAL(messagesReceived(QVariantList, int)), this, SLOT(handleMessagesReceived(QVariantList, int)));
    connect(this->tdLibWrapper, SIGNAL(newMessageReceived(QString, QVariantMap)), this, SLOT(handleNewMessageReceived(QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatReadInboxUpdated(QString, QString, int)), this, SLOT(handleChatReadInboxUpdated(QString, QString, int)));
    connect(this->tdLibWrapper, SIGNAL(chatReadOutboxUpdated(QString, QString)), this, SLOT(handleChatReadOutboxUpdated(QString, QString)));
    connect(this->tdLibWrapper, SIGNAL(messageSendSucceeded(QString, QString, QVariantMap)), this, SLOT(handleMessageSendSucceeded(QString, QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatNotificationSettingsUpdated(QString, QVariantMap)), this, SLOT(handleChatNotificationSettingsUpdated(QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatPhotoUpdated(qlonglong, QVariantMap)), this, SLOT(handleChatPhotoUpdated(qlonglong, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatPinnedMessageUpdated(qlonglong, qlonglong)), this, SLOT(handleChatPinnedMessageUpdated(qlonglong, qlonglong)));
    connect(this->tdLibWrapper, SIGNAL(messageContentUpdated(QString, QString, QVariantMap)), this, SLOT(handleMessageContentUpdated(QString, QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(messagesDeleted(QString, QVariantList)), this, SLOT(handleMessagesDeleted(QString, QVariantList)));
}

ChatModel::~ChatModel()
{
    LOG("Destroying myself...");
}

int ChatModel::rowCount(const QModelIndex &) const
{
    return messages.size();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(messages.value(index.row()));
    }
    return QVariant();
}

bool ChatModel::insertRows(int row, int count, const QModelIndex &parent)
{
    LOG("Inserting at" << row << ", row count:" << count);
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        this->messages.insert(row + i, this->messagesToBeAdded.at(i));
    }
    this->calculateMessageIndexMap();
    endInsertRows();
    return true;
}

void ChatModel::clear()
{
    LOG("Clearing chat model");
    chatId = 0;
    inReload = false;
    inIncrementalUpdate = false;
    if (!messages.isEmpty()) {
        beginResetModel();
        messages.clear();
        endResetModel();
    }
    if (!chatInformation.isEmpty()) {
        chatInformation.clear();
        emit smallPhotoChanged();
    }
}

void ChatModel::initialize(const QVariantMap &chatInformation)
{
    LOG("Initializing chat model...");
    beginResetModel();
    this->chatInformation = chatInformation;
    this->chatId = chatInformation.value(ID).toLongLong();
    this->messages.clear();
    this->messageIndexMap.clear();
    this->messagesToBeAdded.clear();
    endResetModel();
    emit smallPhotoChanged();
    tdLibWrapper->getChatHistory(chatId, this->chatInformation.value(LAST_READ_INBOX_MESSAGE_ID).toLongLong());
}

void ChatModel::triggerLoadMoreHistory()
{
    if (!this->inIncrementalUpdate && !messages.isEmpty()) {
        LOG("Trigger loading older history...");
        this->inIncrementalUpdate = true;
        this->tdLibWrapper->getChatHistory(chatId, messages.first().toMap().value(ID).toLongLong());
    }
}

void ChatModel::triggerLoadMoreFuture()
{
    if (!this->inIncrementalUpdate && !messages.isEmpty()) {
        LOG("Trigger loading newer future...");
        this->inIncrementalUpdate = true;
        this->tdLibWrapper->getChatHistory(chatId, messages.last().toMap().value(ID).toLongLong(), -49);
    }
}

QVariantMap ChatModel::getChatInformation()
{
    return this->chatInformation;
}

QVariantMap ChatModel::getMessage(int index)
{
    if (index < this->messages.size()) {
        return this->messages.at(index).toMap();
    } else {
        return QVariantMap();
    }
}

int ChatModel::getLastReadMessageIndex()
{
    LOG("Obtaining last read message index");
    if (this->messages.isEmpty()) {
        LOG("Messages are empty, nothing to do...");
        return 0;
    } else if (this->messages.last().toMap().value(SENDER_USER_ID).toString() == tdLibWrapper->getUserInformation().value(ID).toString()) {
        LOG("Last message is an own one, then simply set the last read to the last one...");
        return this->messages.size() - 1;
    } else {
        int lastReadMessageIndex = this->messageIndexMap.value(this->chatInformation.value(LAST_READ_INBOX_MESSAGE_ID).toString()).toInt();
        if (lastReadMessageIndex == 0) {
            LOG("Last read message not found in the list of messages. That shouldn't happen, therefore setting the unread indicator to the end of the list.");
            return this->messages.size() - 1;
        } else {
            LOG("Found last read message in the already loaded messages. Index: " << lastReadMessageIndex);
            return lastReadMessageIndex;
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

static bool compareMessages(const QVariant &message1, const QVariant &message2)
{
    const QVariantMap messageMap1 = message1.toMap();
    const QVariantMap messageMap2 = message2.toMap();
    return messageMap1.value(ID).toLongLong() < messageMap2.value(ID).toLongLong();
}

void ChatModel::handleMessagesReceived(const QVariantList &messages, int totalCount)
{
    LOG("Receiving new messages :)" << messages.size());

    if (messages.size() == 0) {
        LOG("No additional messages loaded, notifying chat UI...");
        this->inReload = false;
        int listInboxPosition = this->calculateLastKnownMessageId();
        int listOutboxPosition = this->calculateLastReadSentMessageId();
        if (this->inIncrementalUpdate) {
            this->inIncrementalUpdate = false;
            emit messagesIncrementalUpdate(listInboxPosition, listOutboxPosition);
        } else {
            emit messagesReceived(listInboxPosition, listOutboxPosition, totalCount);
        }
    } else {
        if (this->isMostRecentMessageLoaded() || this->inIncrementalUpdate) {
            this->messagesToBeAdded.clear();
            QListIterator<QVariant> messagesIterator(messages);
            while (messagesIterator.hasNext()) {
                QVariantMap currentMessage = messagesIterator.next().toMap();
                if (currentMessage.value(CHAT_ID).toLongLong() == chatId && !this->messageIndexMap.contains(currentMessage.value(ID).toString())) {
                    LOG("New message will be added: " + currentMessage.value(ID).toString());
                    this->messagesToBeAdded.append(currentMessage);
                }
            }

            std::sort(this->messagesToBeAdded.begin(), this->messagesToBeAdded.end(), compareMessages);

            if (!this->messagesToBeAdded.isEmpty()) {
                this->insertMessages();
            }

            // First call only returns a few messages, we need to get a little more than that...
            if (!this->messagesToBeAdded.isEmpty() && (this->messagesToBeAdded.size() + this->messages.size()) < 10 && !this->inReload) {
                LOG("Only a few messages received in first call, loading more...");
                this->inReload = true;
                this->tdLibWrapper->getChatHistory(this->chatId, this->messagesToBeAdded.first().toMap().value(ID).toLongLong());
            } else {
                LOG("Messages loaded, notifying chat UI...");
                this->inReload = false;
                int listInboxPosition = this->calculateLastKnownMessageId();
                int listOutboxPosition = this->calculateLastReadSentMessageId();
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

void ChatModel::handleNewMessageReceived(const QString &id, const QVariantMap &message)
{
    if (id.toLongLong() == chatId && !this->messageIndexMap.contains(id)) {
        if (this->isMostRecentMessageLoaded()) {
            LOG("New message received for this chat");

            this->messagesToBeAdded.clear();
            this->messagesToBeAdded.append(message);

            this->insertMessages();
            emit newMessageReceived(message);
        } else {
            LOG("New message in this chat, but not relevant as less recent messages need to be loaded first!");
        }
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
        this->chatInformation.insert("last_read_outbox_message_id", lastReadOutboxMessageId);
        int sentIndex = calculateLastReadSentMessageId();
        LOG("Updating sent message ID, new index" << sentIndex);
        emit lastReadSentMessageUpdated(sentIndex);
    }
}

void ChatModel::handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message)
{
    LOG("Message send succeeded, new message ID" << messageId << "old message ID" << oldMessageId << ", chat ID" << message.value(CHAT_ID).toString());
    LOG("index map:" << messageIndexMap.contains(oldMessageId) << ", index count:" << messageIndexMap.size() << ", message count:" << messages.size());
    if (this->messageIndexMap.contains(oldMessageId)) {
        LOG("Message was successfully sent" << oldMessageId);
        int messageIndex = this->messageIndexMap.value(oldMessageId).toInt();
        this->messages.replace(messageIndex, message);
        this->calculateMessageIndexMap();
        LOG("Message was replaced at index" << messageIndex);
        emit dataChanged(index(messageIndex), index(messageIndex));
        emit lastReadSentMessageUpdated(calculateLastReadSentMessageId());
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

void ChatModel::handleMessageContentUpdated(const QString &id, const QString &messageId, const QVariantMap &newContent)
{
    LOG("Message content updated" << id << messageId);
    if (id.toLongLong() == chatId && messageIndexMap.contains(messageId)) {
        LOG("We know the message that was updated " << messageId);
        int messageIndex = this->messageIndexMap.value(messageId).toInt();
        QVariantMap messageToBeUpdated = this->messages.at(messageIndex).toMap();
        messageToBeUpdated.insert("content", newContent);
        this->messages.replace(messageIndex, messageToBeUpdated);
        this->calculateMessageIndexMap();
        LOG("Message was replaced at index" << messageIndex);
        emit messageUpdated(messageIndex);
        emit dataChanged(index(messageIndex), index(messageIndex));
    }
}

void ChatModel::handleMessagesDeleted(const QString &id, const QVariantList &messageIds)
{
    LOG("Messages were deleted in a chat" << id);
    if (id.toLongLong() == chatId) {
        LOG("Messages in this chat were deleted...");
        QListIterator<QVariant> messageIdIterator(messageIds);
        while (messageIdIterator.hasNext()) {
            QString messageId = messageIdIterator.next().toString();
            if (this->messageIndexMap.contains(messageId)) {
                int messageIndex = this->messageIndexMap.value(messageId).toInt();
                beginRemoveRows(QModelIndex(), messageIndex, messageIndex);
                LOG("...and we even know this message!" << messageId << messageIndex);
                this->messages.removeAt(messageIndex);
                this->calculateMessageIndexMap();
                endRemoveRows();
            }
        }
        emit messagesDeleted();
    }
}

void ChatModel::insertMessages()
{
    if (this->messages.isEmpty()) {
        beginResetModel();
        this->messages.append(this->messagesToBeAdded);
        this->calculateMessageIndexMap();
        endResetModel();
    } else {
        // There is only an append or a prepend, tertium non datur! (probably ;))
        qlonglong lastKnownId = this->messages.last().toMap().value(ID).toLongLong();
        qlonglong firstNewId = this->messagesToBeAdded.first().toMap().value(ID).toLongLong();
        LOG("Inserting messages, last known ID: " + QString::number(lastKnownId) + ", first new ID: " + QString::number(firstNewId));
        if (lastKnownId < firstNewId) {
            // Append
            LOG("Appending new messages...");
            this->insertRows(rowCount(QModelIndex()), this->messagesToBeAdded.size());
        } else {
            // Prepend
            LOG("Prepending new messages...");
            this->insertRows(0, this->messagesToBeAdded.size());
        }
    }
}

QVariantMap ChatModel::enhanceMessage(const QVariantMap &message)
{
    QVariantMap enhancedMessage = message;
    if (enhancedMessage.value("content").toMap().value("@type").toString() == "messageVoiceNote" ) {
        QVariantMap contentMap = enhancedMessage.value("content").toMap();
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
        enhancedMessage.insert("content", contentMap);
    }
    return enhancedMessage;
}

int ChatModel::calculateLastKnownMessageId()
{
    LOG("calculateLastKnownMessageId");
    QString lastKnownMessageId = this->chatInformation.value(LAST_READ_INBOX_MESSAGE_ID).toString();
    LOG("lastKnownMessageId" << lastKnownMessageId);
    LOG("size messageIndexMap" << messageIndexMap.size());
    LOG("contains ID?" << messageIndexMap.contains(lastKnownMessageId));
    int listInboxPosition = this->messageIndexMap.value(lastKnownMessageId, this->messages.size() - 1).toInt();
    if (listInboxPosition > this->messages.size() - 1 ) {
        listInboxPosition = this->messages.size() - 1;
    }
    LOG("Last known message is at position" << listInboxPosition);
    return listInboxPosition;
}

int ChatModel::calculateLastReadSentMessageId()
{
    LOG("calculateLastReadSentMessageId");
    QString lastReadSentMessageId = this->chatInformation.value("last_read_outbox_message_id").toString();
    LOG("lastReadSentMessageId" << lastReadSentMessageId);
    LOG("size messageIndexMap" << messageIndexMap.size());
    LOG("contains ID?" << messageIndexMap.contains(lastReadSentMessageId));
    int listOutboxPosition = this->messageIndexMap.value(lastReadSentMessageId, this->messages.size() - 1).toInt();
    LOG("Last read sent message is at position" << listOutboxPosition);
    return listOutboxPosition;
}

void ChatModel::calculateMessageIndexMap()
{
    LOG("calculateMessageIndexMap");
    this->messageIndexMap.clear();
    for (int i = 0; i < this->messages.size(); i++) {
        this->messageIndexMap.insert(this->messages.at(i).toMap().value(ID).toString(), i);
    }
}

bool ChatModel::isMostRecentMessageLoaded()
{
    // Need to check if we can actually add messages (only possible if the previously latest messages are loaded)
    // Trying with half of the size of an initial list to ensure that everything is there...
    return this->getLastReadMessageIndex() >= this->messages.size() - 25;
}
