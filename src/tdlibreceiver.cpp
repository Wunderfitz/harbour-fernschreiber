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
#include "tdlibreceiver.h"

#define LOG(x) qDebug() << "[TDLibReceiver]" << x

#if defined (QT_DEBUG) || defined (DEBUG)
#  define VERBOSE(x) LOG(x)
#else
#  define VERBOSE(x)
#endif

namespace {
    const QString ID("id");
    const QString LIST("list");
    const QString CHAT_ID("chat_id");
    const QString POSITION("position");
    const QString POSITIONS("positions");
    const QString ORDER("order");
    const QString BASIC_GROUP("basic_group");
    const QString SUPERGROUP("supergroup");
    const QString LAST_MESSAGE("last_message");
    const QString UNREAD_COUNT("unread_count");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");

    const QString TYPE("@type");
    const QString EXTRA("@extra");
    const QString TYPE_CHAT_POSITION("chatPosition");
    const QString TYPE_CHAT_LIST_MAIN("chatListMain");
}

static QString getChatPositionOrder(const QVariantMap &position)
{
    if (position.value(TYPE).toString() == TYPE_CHAT_POSITION &&
        position.value(LIST).toMap().value(TYPE) == TYPE_CHAT_LIST_MAIN) {
        return position.value(ORDER).toString();
    }
    return QString();
}

static QString findChatPositionOrder(const QVariantList &positions)
{
    const int n = positions.count();
    for (int i = 0; i < n; i++) {
        const QString order(getChatPositionOrder(positions.at(i).toMap()));
        if (!order.isEmpty()) {
            return order;
        }
    }
    return QString();
}

TDLibReceiver::TDLibReceiver(void *tdLibClient, QObject *parent) : QThread(parent)
{
    this->tdLibClient = tdLibClient;
    this->isActive = true;

    handlers.insert("updateOption", &TDLibReceiver::processUpdateOption);
    handlers.insert("updateAuthorizationState", &TDLibReceiver::processUpdateAuthorizationState);
    handlers.insert("updateConnectionState", &TDLibReceiver::processUpdateConnectionState);
    handlers.insert("updateUser", &TDLibReceiver::processUpdateUser);
    handlers.insert("updateUserStatus", &TDLibReceiver::processUpdateUserStatus);
    handlers.insert("updateFile", &TDLibReceiver::processUpdateFile);
    handlers.insert("file", &TDLibReceiver::processFile);
    handlers.insert("updateNewChat", &TDLibReceiver::processUpdateNewChat);
    handlers.insert("updateUnreadMessageCount", &TDLibReceiver::processUpdateUnreadMessageCount);
    handlers.insert("updateUnreadChatCount", &TDLibReceiver::processUpdateUnreadChatCount);
    handlers.insert("updateChatLastMessage", &TDLibReceiver::processUpdateChatLastMessage);
    handlers.insert("updateChatOrder", &TDLibReceiver::processUpdateChatOrder);
    handlers.insert("updateChatPosition", &TDLibReceiver::processUpdateChatPosition);
    handlers.insert("updateChatReadInbox", &TDLibReceiver::processUpdateChatReadInbox);
    handlers.insert("updateChatReadOutbox", &TDLibReceiver::processUpdateChatReadOutbox);
    handlers.insert("updateBasicGroup", &TDLibReceiver::processUpdateBasicGroup);
    handlers.insert("updateSupergroup", &TDLibReceiver::processUpdateSuperGroup);
    handlers.insert("updateChatOnlineMemberCount", &TDLibReceiver::processChatOnlineMemberCountUpdated);
    handlers.insert("messages", &TDLibReceiver::processMessages);
    handlers.insert("updateNewMessage", &TDLibReceiver::processUpdateNewMessage);
    handlers.insert("message", &TDLibReceiver::processMessage);
    handlers.insert("updateMessageSendSucceeded", &TDLibReceiver::processMessageSendSucceeded);
    handlers.insert("updateActiveNotifications", &TDLibReceiver::processUpdateActiveNotifications);
    handlers.insert("updateNotificationGroup", &TDLibReceiver::processUpdateNotificationGroup);
    handlers.insert("updateChatNotificationSettings", &TDLibReceiver::processUpdateChatNotificationSettings);
    handlers.insert("updateMessageContent", &TDLibReceiver::processUpdateMessageContent);
    handlers.insert("updateDeleteMessages", &TDLibReceiver::processUpdateDeleteMessages);
    handlers.insert("chats", &TDLibReceiver::processChats);
    handlers.insert("chat", &TDLibReceiver::processChat);
    handlers.insert("updateRecentStickers", &TDLibReceiver::processUpdateRecentStickers);
    handlers.insert("stickers", &TDLibReceiver::processStickers);
    handlers.insert("updateInstalledStickerSets", &TDLibReceiver::processUpdateInstalledStickerSets);
    handlers.insert("stickerSets", &TDLibReceiver::processStickerSets);
    handlers.insert("stickerSet", &TDLibReceiver::processStickerSet);
    handlers.insert("chatMembers", &TDLibReceiver::processChatMembers);
    handlers.insert("userFullInfo", &TDLibReceiver::processUserFullInfo);
    handlers.insert("updateUserFullInfo", &TDLibReceiver::processUpdateUserFullInfo);
    handlers.insert("basicGroupFullInfo", &TDLibReceiver::processBasicGroupFullInfo);
    handlers.insert("updateBasicGroupFullInfo", &TDLibReceiver::processUpdateBasicGroupFullInfo);
    handlers.insert("supergroupFullInfo", &TDLibReceiver::processSupergroupFullInfo);
    handlers.insert("updateSupergroupFullInfo", &TDLibReceiver::processUpdateSupergroupFullInfo);
    handlers.insert("userProfilePhotos", &TDLibReceiver::processUserProfilePhotos);
    handlers.insert("updateChatPermissions", &TDLibReceiver::processUpdateChatPermissions);
    handlers.insert("updateChatTitle", &TDLibReceiver::processUpdateChatTitle);
    handlers.insert("users", &TDLibReceiver::processUsers);
    handlers.insert("error", &TDLibReceiver::processError);
}

void TDLibReceiver::setActive(bool active)
{
    if (active) {
        LOG("Activating receiver loop...");
    } else {
        LOG("Deactivating receiver loop, this may take a while...");
    }
    this->isActive = active;
}

void TDLibReceiver::receiverLoop()
{
    LOG("Starting receiver loop");
    const double WAIT_TIMEOUT = 5.0;
    while (this->isActive) {
      const char *result = td_json_client_receive(this->tdLibClient, WAIT_TIMEOUT);
      if (result) {
          QJsonDocument receivedJsonDocument = QJsonDocument::fromJson(QByteArray(result));
          VERBOSE("Raw result:" << receivedJsonDocument.toJson(QJsonDocument::Indented).constData());
          processReceivedDocument(receivedJsonDocument);
      }
    }
    LOG("Stopping receiver loop");
}

void TDLibReceiver::processReceivedDocument(const QJsonDocument &receivedJsonDocument)
{
    QVariantMap receivedInformation = receivedJsonDocument.object().toVariantMap();
    QString objectTypeName = receivedInformation.value("@type").toString();

    Handler handler = handlers.value(objectTypeName);
    if (handler) {
        (this->*handler)(receivedInformation);
    } else {
        LOG("Unhandled object type" << objectTypeName);
    }
}

void TDLibReceiver::processUpdateOption(const QVariantMap &receivedInformation)
{
    QString currentOption = receivedInformation.value("name").toString();
    if (currentOption == "version") {
        QString detectedVersion = receivedInformation.value("value").toMap().value("value").toString();
        LOG("TD Lib version detected: " << detectedVersion);
        emit versionDetected(detectedVersion);
    } else {
        QVariant currentValue = receivedInformation.value("value").toMap().value("value");
        LOG("Option updated: " << currentOption << currentValue);
        emit optionUpdated(currentOption, currentValue);
    }
}

void TDLibReceiver::processUpdateAuthorizationState(const QVariantMap &receivedInformation)
{
    QString authorizationState = receivedInformation.value("authorization_state").toMap().value("@type").toString();
    LOG("Authorization state changed: " << authorizationState);
    emit authorizationStateChanged(authorizationState, receivedInformation);
}

void TDLibReceiver::processUpdateConnectionState(const QVariantMap &receivedInformation)
{
    QString connectionState = receivedInformation.value("state").toMap().value("@type").toString();
    LOG("Connection state changed: " << connectionState);
    emit connectionStateChanged(connectionState);
}

void TDLibReceiver::processUpdateUser(const QVariantMap &receivedInformation)
{
    QVariantMap userInformation = receivedInformation.value("user").toMap();
    // LOG("User was updated: " << userInformation.value("username").toString() << userInformation.value("first_name").toString() << userInformation.value("last_name").toString());
    emit userUpdated(userInformation);
}

void TDLibReceiver::processUpdateUserStatus(const QVariantMap &receivedInformation)
{
    QString userId = receivedInformation.value("user_id").toString();
    QVariantMap userStatusInformation = receivedInformation.value("status").toMap();
    // LOG("User status was updated: " << receivedInformation.value("user_id").toString() << userStatusInformation.value("@type").toString());
    emit userStatusUpdated(userId, userStatusInformation);
}

void TDLibReceiver::processUpdateFile(const QVariantMap &receivedInformation)
{
    QVariantMap fileInformation = receivedInformation.value("file").toMap();
    LOG("File was updated: " << fileInformation.value(ID).toString());
    emit fileUpdated(fileInformation);
}

void TDLibReceiver::processFile(const QVariantMap &receivedInformation)
{
    LOG("File was updated: " << receivedInformation.value(ID).toString());
    emit fileUpdated(receivedInformation);
}

void TDLibReceiver::processUpdateNewChat(const QVariantMap &receivedInformation)
{
    QVariantMap chatInformation = receivedInformation.value("chat").toMap();
    LOG("New chat discovered: " << chatInformation.value(ID).toString() << chatInformation.value("title").toString());
    emit newChatDiscovered(chatInformation);
}

void TDLibReceiver::processUpdateUnreadMessageCount(const QVariantMap &receivedInformation)
{
    QVariantMap messageCountInformation;
    messageCountInformation.insert("chat_list_type", receivedInformation.value("chat_list").toMap().value("@type"));
    messageCountInformation.insert("unread_count", receivedInformation.value("unread_count"));
    messageCountInformation.insert("unread_unmuted_count", receivedInformation.value("unread_unmuted_count"));
    LOG("Unread message count updated: " << messageCountInformation.value("chat_list_type").toString() << messageCountInformation.value("unread_count").toString());
    emit unreadMessageCountUpdated(messageCountInformation);
}

void TDLibReceiver::processUpdateUnreadChatCount(const QVariantMap &receivedInformation)
{
    QVariantMap chatCountInformation;
    chatCountInformation.insert("chat_list_type", receivedInformation.value("chat_list").toMap().value("@type"));
    chatCountInformation.insert("marked_as_unread_count", receivedInformation.value("marked_as_unread_count"));
    chatCountInformation.insert("marked_as_unread_unmuted_count", receivedInformation.value("marked_as_unread_unmuted_count"));
    chatCountInformation.insert("total_count", receivedInformation.value("total_count"));
    chatCountInformation.insert("unread_count", receivedInformation.value("unread_count"));
    chatCountInformation.insert("unread_unmuted_count", receivedInformation.value("unread_unmuted_count"));
    LOG("Unread chat count updated: " << chatCountInformation.value("chat_list_type").toString() << chatCountInformation.value("unread_count").toString());
    emit unreadChatCountUpdated(chatCountInformation);
}

void TDLibReceiver::processUpdateChatLastMessage(const QVariantMap &receivedInformation)
{
    const QString chat_id(receivedInformation.value(CHAT_ID).toString());
    QString order;
    if (receivedInformation.contains(POSITIONS)) {
        order = findChatPositionOrder(receivedInformation.value(POSITIONS).toList());
    } else {
        order = receivedInformation.value(ORDER).toString();
    }
    const QVariantMap lastMessage = receivedInformation.value(LAST_MESSAGE).toMap();
    LOG("Last message of chat" << chat_id << "updated, order" << order << "type" << lastMessage.value("@type").toString());
    emit chatLastMessageUpdated(chat_id, order, lastMessage);
}

void TDLibReceiver::processUpdateChatOrder(const QVariantMap &receivedInformation)
{
    const QString chat_id(receivedInformation.value(CHAT_ID).toString());
    const QString order(receivedInformation.value(ORDER).toString());
    LOG("Chat order updated for ID" << chat_id << "to" << order);
    emit chatOrderUpdated(chat_id, order);
}

void TDLibReceiver::processUpdateChatPosition(const QVariantMap &receivedInformation)
{
    const QString chat_id(receivedInformation.value(CHAT_ID).toString());
    const QString order(receivedInformation.value(POSITION).toMap().value(ORDER).toString());
    LOG("Chat position updated for ID" << chat_id << "new order" << order);
    emit chatOrderUpdated(chat_id, order);
}

void TDLibReceiver::processUpdateChatReadInbox(const QVariantMap &receivedInformation)
{
    const QString chat_id(receivedInformation.value(CHAT_ID).toString());
    const QString unread_count(receivedInformation.value(UNREAD_COUNT).toString());
    LOG("Chat read information updated for" << chat_id << "unread count:" << unread_count);
    emit chatReadInboxUpdated(chat_id, receivedInformation.value(LAST_READ_INBOX_MESSAGE_ID).toString(), unread_count.toInt());
}

void TDLibReceiver::processUpdateChatReadOutbox(const QVariantMap &receivedInformation)
{
    const QString chat_id(receivedInformation.value(CHAT_ID).toString());
    const QString last_read_outbox_message_id(receivedInformation.value(LAST_READ_OUTBOX_MESSAGE_ID).toString());
    LOG("Sent messages read information updated for" << chat_id << "last read message ID:" << last_read_outbox_message_id);
    emit chatReadOutboxUpdated(chat_id, last_read_outbox_message_id);
}

void TDLibReceiver::processUpdateBasicGroup(const QVariantMap &receivedInformation)
{
    const QVariantMap basicGroup(receivedInformation.value(BASIC_GROUP).toMap());
    const qlonglong basicGroupId = basicGroup.value(ID).toLongLong();
    LOG("Basic group information updated for " << basicGroupId);
    emit basicGroupUpdated(basicGroupId, basicGroup);
}

void TDLibReceiver::processUpdateSuperGroup(const QVariantMap &receivedInformation)
{
    const QVariantMap supergroup(receivedInformation.value(SUPERGROUP).toMap());
    const qlonglong superGroupId = supergroup.value(ID).toLongLong();
    LOG("Super group information updated for " << superGroupId);
    emit superGroupUpdated(superGroupId, supergroup);
}

void TDLibReceiver::processChatOnlineMemberCountUpdated(const QVariantMap &receivedInformation)
{
    const QString chatId = receivedInformation.value(CHAT_ID).toString();
    LOG("Online member count updated for chat " << chatId);
    emit chatOnlineMemberCountUpdated(chatId, receivedInformation.value("online_member_count").toInt());
}

void TDLibReceiver::processMessages(const QVariantMap &receivedInformation)
{
    LOG("Received new messages, amount: " << receivedInformation.value("total_count").toString());
    emit messagesReceived(receivedInformation.value("messages").toList(), receivedInformation.value("total_count").toInt());
}

void TDLibReceiver::processUpdateNewMessage(const QVariantMap &receivedInformation)
{
    const QString chatId = receivedInformation.value("message").toMap().value(CHAT_ID).toString();
    LOG("Received new message for chat " << chatId);
    emit newMessageReceived(chatId, receivedInformation.value("message").toMap());
}

void TDLibReceiver::processMessage(const QVariantMap &receivedInformation)
{
    const QString chatId = receivedInformation.value(CHAT_ID).toString();
    const QString messageId = receivedInformation.value(ID).toString();
    LOG("Received message " << chatId << messageId);
    emit messageInformation(messageId, receivedInformation);
}

void TDLibReceiver::processMessageSendSucceeded(const QVariantMap &receivedInformation)
{
    QString oldMessageId = receivedInformation.value("old_message_id").toString();
    QVariantMap message = receivedInformation.value("message").toMap();
    const QString messageId = message.value(ID).toString();
    LOG("Message send succeeded " << messageId << oldMessageId);
    emit messageSendSucceeded(messageId, oldMessageId, message);
}

void TDLibReceiver::processUpdateActiveNotifications(const QVariantMap &receivedInformation)
{
    LOG("Received active notification groups");
    emit activeNotificationsUpdated(receivedInformation.value("groups").toList());
}

void TDLibReceiver::processUpdateNotificationGroup(const QVariantMap &receivedInformation)
{
    LOG("Received updated notification group");
    emit notificationGroupUpdated(receivedInformation);
}

void TDLibReceiver::processUpdateNotification(const QVariantMap &receivedInformation)
{
    LOG("Received notification update");
    emit notificationUpdated(receivedInformation);
}

void TDLibReceiver::processUpdateChatNotificationSettings(const QVariantMap &receivedInformation)
{
    const QString chatId = receivedInformation.value(CHAT_ID).toString();
    LOG("Received new notification settings for chat " << chatId);
    emit chatNotificationSettingsUpdated(chatId, receivedInformation.value("notification_settings").toMap());
}

void TDLibReceiver::processUpdateMessageContent(const QVariantMap &receivedInformation)
{
    const QString chatId = receivedInformation.value(CHAT_ID).toString();
    QString messageId = receivedInformation.value("message_id").toString();
    LOG("Message content updated " << chatId << messageId);
    emit messageContentUpdated(chatId, messageId, receivedInformation.value("new_content").toMap());
}

void TDLibReceiver::processUpdateDeleteMessages(const QVariantMap &receivedInformation)
{
    const QString chatId = receivedInformation.value(CHAT_ID).toString();
    QVariantList messageIds = receivedInformation.value("message_ids").toList();
    LOG("Some messages were deleted " << chatId);
    emit messagesDeleted(chatId, messageIds);
}

void TDLibReceiver::processChats(const QVariantMap &receivedInformation)
{
    emit chats(receivedInformation);
}

void TDLibReceiver::processChat(const QVariantMap &receivedInformation)
{
    emit chat(receivedInformation);
}

void TDLibReceiver::processUpdateRecentStickers(const QVariantMap &receivedInformation)
{
    LOG("Recent stickers updated");
    emit recentStickersUpdated(receivedInformation.value("sticker_ids").toList());
}

void TDLibReceiver::processStickers(const QVariantMap &receivedInformation)
{
    LOG("Received some stickers...");
    emit stickers(receivedInformation.value("stickers").toList());
}

void TDLibReceiver::processUpdateInstalledStickerSets(const QVariantMap &receivedInformation)
{
    LOG("Recent sticker sets updated");
    emit installedStickerSetsUpdated(receivedInformation.value("sticker_set_ids").toList());
}

void TDLibReceiver::processStickerSets(const QVariantMap &receivedInformation)
{
    LOG("Received some sticker sets...");
    emit stickerSets(receivedInformation.value("sets").toList());
}

void TDLibReceiver::processStickerSet(const QVariantMap &receivedInformation)
{
    LOG("Received a sticker set...");
    emit stickerSet(receivedInformation);
}
void TDLibReceiver::processChatMembers(const QVariantMap &receivedInformation)
{
    LOG("Received super group members");
    const QString extra = receivedInformation.value(EXTRA).toString();
    emit chatMembers(extra, receivedInformation.value("members").toList(), receivedInformation.value("total_count").toInt());
}

void TDLibReceiver::processUserFullInfo(const QVariantMap &receivedInformation)
{

    LOG("Received UserFullInfo");

    emit userFullInfo(receivedInformation);
}

void TDLibReceiver::processUpdateUserFullInfo(const QVariantMap &receivedInformation)
{

    LOG("Received UserFullInfoUpdate");

    emit userFullInfoUpdated(receivedInformation.value("user_id").toString(), receivedInformation.value("user_full_info").toMap());
}

void TDLibReceiver::processBasicGroupFullInfo(const QVariantMap &receivedInformation)
{

    LOG("Received BasicGroupFullInfo");
    const QString groupId = receivedInformation.value(EXTRA).toString();

    emit basicGroupFullInfo(groupId, receivedInformation);
}
void TDLibReceiver::processUpdateBasicGroupFullInfo(const QVariantMap &receivedInformation)
{
    LOG("Received BasicGroupFullInfoUpdate");
    const QString groupId = receivedInformation.value("basic_group_id").toString();

    emit basicGroupFullInfoUpdated(groupId, receivedInformation.value("basic_group_full_info").toMap());
}

void TDLibReceiver::processSupergroupFullInfo(const QVariantMap &receivedInformation)
{
    LOG("Received SuperGroupFullInfoUpdate");
    const QString groupId = receivedInformation.value(EXTRA).toString();

    emit supergroupFullInfo(groupId, receivedInformation);
}

void TDLibReceiver::processUpdateSupergroupFullInfo(const QVariantMap &receivedInformation)
{

    LOG("Received SuperGroupFullInfoUpdate");
    const QString groupId = receivedInformation.value("supergroup_id").toString();

    emit supergroupFullInfoUpdated(groupId, receivedInformation.value("supergroup_full_info").toMap());
}

void TDLibReceiver::processUserProfilePhotos(const QVariantMap &receivedInformation)
{
    const QString extra = receivedInformation.value(EXTRA).toString();
    emit userProfilePhotos(extra, receivedInformation.value("photos").toList(), receivedInformation.value("total_count").toInt());
}

void TDLibReceiver::processUpdateChatPermissions(const QVariantMap &receivedInformation)
{
    emit chatPermissionsUpdated(receivedInformation.value("chat_id").toString(), receivedInformation.value("permissions").toMap());
}

void TDLibReceiver::processUpdateChatTitle(const QVariantMap &receivedInformation)
{

    LOG("Received UpdateChatTitle");
    emit chatTitleUpdated(receivedInformation.value("chat_id").toString(), receivedInformation.value("title").toString());
}

void TDLibReceiver::processUsers(const QVariantMap &receivedInformation)
{
    LOG("Received Users");
    emit usersReceived(receivedInformation.value(EXTRA).toString(), receivedInformation.value("user_ids").toList(), receivedInformation.value("total_count").toInt());
}

void TDLibReceiver::processError(const QVariantMap &receivedInformation)
{
    LOG("Received an error");
    emit errorReceived(receivedInformation.value("code").toInt(), receivedInformation.value("message").toString());
}
