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

#define DEBUG_MODULE TDLibReceiver
#include "debuglog.h"

namespace {
    const QString ID("id");
    const QString LIST("list");
    const QString CHAT_ID("chat_id");
    const QString USER_ID("user_id");
    const QString OLD_MESSAGE_ID("old_message_id");
    const QString MESSAGE_ID("message_id");
    const QString MESSAGE_IDS("message_ids");
    const QString MESSAGE("message");
    const QString MESSAGES("messages");
    const QString TITLE("title");
    const QString NAME("name");
    const QString VALUE("value");
    const QString POSITION("position");
    const QString POSITIONS("positions");
    const QString PHOTO("photo");
    const QString ORDER("order");
    const QString IS_PINNED("is_pinned");
    const QString BASIC_GROUP("basic_group");
    const QString SUPERGROUP("supergroup");
    const QString LAST_MESSAGE("last_message");
    const QString TOTAL_COUNT("total_count");
    const QString UNREAD_COUNT("unread_count");
    const QString UNREAD_MENTION_COUNT("unread_mention_count");
    const QString UNREAD_REACTION_COUNT("unread_reaction_count");
    const QString TEXT("text");
    const QString LAST_READ_INBOX_MESSAGE_ID("last_read_inbox_message_id");
    const QString LAST_READ_OUTBOX_MESSAGE_ID("last_read_outbox_message_id");
    const QString SECRET_CHAT("secret_chat");
    const QString INTERACTION_INFO("interaction_info");
    const QString ANIMATED_EMOJI("animated_emoji");
    const QString COLOR_REPLACEMENTS("color_replacements");
    const QString STICKER("sticker");
    const QString STICKERS("stickers");
    const QString COVERS("covers");
    const QString OUTLINE("outline");
    const QString CONTENT("content");
    const QString NEW_CONTENT("new_content");
    const QString SETS("sets");
    const QString EMOJIS("emojis");
    const QString REPLY_TO("reply_to");
    const QString REPLY_IN_CHAT_ID("reply_in_chat_id");
    const QString REPLY_TO_MESSAGE_ID("reply_to_message_id");
    const QString DRAFT_MESSAGE("draft_message");

    const QString _TYPE("@type");
    const QString _EXTRA("@extra");
    const QString TYPE_CHAT_POSITION("chatPosition");
    const QString TYPE_CHAT_LIST_MAIN("chatListMain");
    const QString TYPE_STICKER_SET_INFO("stickerSetInfo");
    const QString TYPE_STICKER_SET("stickerSet");
    const QString TYPE_MESSAGE("message");
    const QString TYPE_STICKER("sticker");
    const QString TYPE_MESSAGE_STICKER("messageSticker");
    const QString TYPE_MESSAGE_REPLY_TO_MESSAGE("messageReplyToMessage");
    const QString TYPE_MESSAGE_ANIMATED_EMOJI("messageAnimatedEmoji");
    const QString TYPE_ANIMATED_EMOJI("animatedEmoji");
    const QString TYPE_INPUT_MESSAGE_REPLY_TO_MESSAGE("inputMessageReplyToMessage");
    const QString TYPE_DRAFT_MESSAGE("draftMessage");
}

static QString getChatPositionOrder(const QVariantMap &position)
{
    if (position.value(_TYPE).toString() == TYPE_CHAT_POSITION &&
        position.value(LIST).toMap().value(_TYPE) == TYPE_CHAT_LIST_MAIN) {
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
    handlers.insert("sponsoredMessage", &TDLibReceiver::processSponsoredMessage);   // TdLib <= 1.8.7
    handlers.insert("sponsoredMessages", &TDLibReceiver::processSponsoredMessages); // TdLib >= 1.8.8
    handlers.insert("updateNewMessage", &TDLibReceiver::processUpdateNewMessage);
    handlers.insert("message", &TDLibReceiver::processMessage);
    handlers.insert("messageLinkInfo", &TDLibReceiver::processMessageLinkInfo);
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
    handlers.insert("chatPhotos", &TDLibReceiver::processUserProfilePhotos);
    handlers.insert("updateChatPermissions", &TDLibReceiver::processUpdateChatPermissions);
    handlers.insert("updateChatPhoto", &TDLibReceiver::processUpdateChatPhoto);
    handlers.insert("updateChatTitle", &TDLibReceiver::processUpdateChatTitle);
    handlers.insert("updateChatPinnedMessage", &TDLibReceiver::processUpdateChatPinnedMessage);
    handlers.insert("updateMessageIsPinned", &TDLibReceiver::processUpdateMessageIsPinned);
    handlers.insert("users", &TDLibReceiver::processUsers);
    handlers.insert("error", &TDLibReceiver::processError);
    handlers.insert("ok", &TDLibReceiver::ok);
    handlers.insert("secretChat", &TDLibReceiver::processSecretChat);
    handlers.insert("updateSecretChat", &TDLibReceiver::processUpdateSecretChat);
    handlers.insert("importedContacts", &TDLibReceiver::processImportedContacts);
    handlers.insert("updateMessageEdited", &TDLibReceiver::processUpdateMessageEdited);
    handlers.insert("updateChatIsMarkedAsUnread", &TDLibReceiver::processUpdateChatIsMarkedAsUnread);
    handlers.insert("updateChatDraftMessage", &TDLibReceiver::processUpdateChatDraftMessage);
    handlers.insert("inlineQueryResults", &TDLibReceiver::processInlineQueryResults);
    handlers.insert("callbackQueryAnswer", &TDLibReceiver::processCallbackQueryAnswer);
    handlers.insert("userPrivacySettingRules", &TDLibReceiver::processUserPrivacySettingRules);
    handlers.insert("updateUserPrivacySettingRules", &TDLibReceiver::processUpdateUserPrivacySettingRules);
    handlers.insert("updateMessageInteractionInfo", &TDLibReceiver::processUpdateMessageInteractionInfo);
    handlers.insert("sessions", &TDLibReceiver::processSessions);
    handlers.insert("availableReactions", &TDLibReceiver::processAvailableReactions);
    handlers.insert("updateChatUnreadMentionCount", &TDLibReceiver::processUpdateChatUnreadMentionCount);
    handlers.insert("updateChatUnreadReactionCount", &TDLibReceiver::processUpdateChatUnreadReactionCount);
    handlers.insert("updateActiveEmojiReactions", &TDLibReceiver::processUpdateActiveEmojiReactions);
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
    QString objectTypeName = receivedInformation.value(_TYPE).toString();

    Handler handler = handlers.value(objectTypeName);
    if (handler) {
        (this->*handler)(receivedInformation);
    } else {
        LOG("Unhandled object type" << objectTypeName);
    }
}

void TDLibReceiver::processUpdateOption(const QVariantMap &receivedInformation)
{
    const QString currentOption = receivedInformation.value(NAME).toString();
    const QVariant value = receivedInformation.value(VALUE).toMap().value(VALUE);
    if (currentOption == "version") {
        QString detectedVersion = value.toString();
        LOG("TD Lib version detected: " << detectedVersion);
        emit versionDetected(detectedVersion);
    } else {
        LOG("Option updated: " << currentOption << value);
        emit optionUpdated(currentOption, value);
    }
}

void TDLibReceiver::processUpdateAuthorizationState(const QVariantMap &receivedInformation)
{
    QString authorizationState = receivedInformation.value("authorization_state").toMap().value(_TYPE).toString();
    LOG("Authorization state changed: " << authorizationState);
    emit authorizationStateChanged(authorizationState, receivedInformation);
}

void TDLibReceiver::processUpdateConnectionState(const QVariantMap &receivedInformation)
{
    QString connectionState = receivedInformation.value("state").toMap().value(_TYPE).toString();
    LOG("Connection state changed: " << connectionState);
    emit connectionStateChanged(connectionState);
}

void TDLibReceiver::processUpdateUser(const QVariantMap &receivedInformation)
{
    QVariantMap userInformation = receivedInformation.value("user").toMap();
    VERBOSE("User was updated: " << userInformation.value("username").toString() << userInformation.value("first_name").toString() << userInformation.value("last_name").toString());
    emit userUpdated(userInformation);
}

void TDLibReceiver::processUpdateUserStatus(const QVariantMap &receivedInformation)
{
    const QString userId = receivedInformation.value(USER_ID).toString();
    QVariantMap userStatusInformation = receivedInformation.value("status").toMap();
    VERBOSE("User status was updated: " << receivedInformation.value(USER_ID).toString() << userStatusInformation.value(_TYPE).toString());
    emit userStatusUpdated(userId, userStatusInformation);
}

void TDLibReceiver::processUpdateFile(const QVariantMap &receivedInformation)
{
    const QVariantMap fileInformation = receivedInformation.value("file").toMap();
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
    const QVariantMap chatInformation = receivedInformation.value("chat").toMap();
    LOG("New chat discovered: " << chatInformation.value(ID).toString() << chatInformation.value(TITLE).toString());
    emit newChatDiscovered(chatInformation);
}

void TDLibReceiver::processUpdateUnreadMessageCount(const QVariantMap &receivedInformation)
{
    QVariantMap messageCountInformation;
    messageCountInformation.insert("chat_list_type", receivedInformation.value("chat_list").toMap().value(_TYPE));
    messageCountInformation.insert(UNREAD_COUNT, receivedInformation.value(UNREAD_COUNT));
    messageCountInformation.insert("unread_unmuted_count", receivedInformation.value("unread_unmuted_count"));
    LOG("Unread message count updated: " << messageCountInformation.value("chat_list_type").toString() << messageCountInformation.value(UNREAD_COUNT).toString());
    emit unreadMessageCountUpdated(messageCountInformation);
}

void TDLibReceiver::processUpdateUnreadChatCount(const QVariantMap &receivedInformation)
{
    QVariantMap chatCountInformation;
    chatCountInformation.insert("chat_list_type", receivedInformation.value("chat_list").toMap().value(_TYPE));
    chatCountInformation.insert("marked_as_unread_count", receivedInformation.value("marked_as_unread_count"));
    chatCountInformation.insert("marked_as_unread_unmuted_count", receivedInformation.value("marked_as_unread_unmuted_count"));
    chatCountInformation.insert(TOTAL_COUNT, receivedInformation.value(TOTAL_COUNT));
    chatCountInformation.insert(UNREAD_COUNT, receivedInformation.value(UNREAD_COUNT));
    chatCountInformation.insert("unread_unmuted_count", receivedInformation.value("unread_unmuted_count"));
    LOG("Unread chat count updated: " << chatCountInformation.value("chat_list_type").toString() << chatCountInformation.value(UNREAD_COUNT).toString());
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
    LOG("Last message of chat" << chat_id << "updated, order" << order << "type" << lastMessage.value(_TYPE).toString());
    emit chatLastMessageUpdated(chat_id, order, cleanupMap(lastMessage));
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
    QVariantMap positionMap = receivedInformation.value(POSITION).toMap();

    QString updateForChatList = positionMap.value(LIST).toMap().value(_TYPE).toString();
    const QString order(positionMap.value(ORDER).toString());
    bool is_pinned = positionMap.value(IS_PINNED).toBool();

    // We are only processing main chat list updates at the moment...
    if (updateForChatList == "chatListMain") {
        LOG("Chat position updated for ID" << chat_id << "new order" << order << "is pinned" << is_pinned);
        emit chatOrderUpdated(chat_id, order);
        emit chatPinnedUpdated(chat_id.toLongLong(), is_pinned);
    } else {
        LOG("Received chat position update for uninteresting list" << updateForChatList << "ID" << chat_id << "new order" << order << "is pinned" << is_pinned);
    }
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
    const int total_count = receivedInformation.value(TOTAL_COUNT).toInt();
    LOG("Received new messages, amount: " << total_count);
    emit messagesReceived(cleanupList(receivedInformation.value(MESSAGES).toList()), total_count);
}

void TDLibReceiver::processSponsoredMessage(const QVariantMap &receivedInformation)
{
    // TdLib <= 1.8.7
    const qlonglong chatId = receivedInformation.value(_EXTRA).toLongLong(); // See TDLibWrapper::getChatSponsoredMessage
    LOG("Received sponsored message for chat" << chatId);
    emit sponsoredMessageReceived(chatId, receivedInformation);
}

void TDLibReceiver::processSponsoredMessages(const QVariantMap &receivedInformation)
{
    // TdLib >= 1.8.8
    const qlonglong chatId = receivedInformation.value(_EXTRA).toLongLong(); // See TDLibWrapper::getChatSponsoredMessage
    const QVariantList messages(receivedInformation.value(MESSAGES).toList());
    LOG("Received" << messages.count() << "sponsored messages for chat" << chatId);
    QListIterator<QVariant> it(messages);
    while (it.hasNext()) {
        emit sponsoredMessageReceived(chatId, it.next().toMap());
    }
}

void TDLibReceiver::processUpdateNewMessage(const QVariantMap &receivedInformation)
{
    const QVariantMap message = receivedInformation.value(MESSAGE).toMap();
    const qlonglong chatId = message.value(CHAT_ID).toLongLong();
    LOG("Received new message for chat" << chatId);
    emit newMessageReceived(chatId, cleanupMap(message));
}

void TDLibReceiver::processMessage(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const qlonglong messageId = receivedInformation.value(ID).toLongLong();
    LOG("Received message " << chatId << messageId);
    emit messageInformation(chatId, messageId, cleanupMap(receivedInformation));
}

void TDLibReceiver::processMessageLinkInfo(const QVariantMap &receivedInformation)
{
    const QString oldExtra = receivedInformation.value(_EXTRA).toString();
    QString url = "";
    QString extra = "";
    LOG("Received message link info " << oldExtra);
    if (oldExtra.contains("|")) {
        const int midIndex = oldExtra.indexOf("|");
        url = oldExtra.left(midIndex);
        extra = oldExtra.mid(midIndex + 1);
    } else {
        url = oldExtra;
    }
    emit messageLinkInfoReceived(url, receivedInformation, extra);
}

void TDLibReceiver::processMessageSendSucceeded(const QVariantMap &receivedInformation)
{
    const qlonglong oldMessageId = receivedInformation.value(OLD_MESSAGE_ID).toLongLong();
    const QVariantMap message = receivedInformation.value(MESSAGE).toMap();
    const qlonglong messageId = message.value(ID).toLongLong();
    LOG("Message send succeeded" << messageId << oldMessageId);
    emit messageSendSucceeded(messageId, oldMessageId, cleanupMap(message));
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
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const qlonglong messageId = receivedInformation.value(MESSAGE_ID).toLongLong();
    LOG("Message content updated" << chatId << messageId);
    emit messageContentUpdated(chatId, messageId, cleanupMap(receivedInformation.value(NEW_CONTENT).toMap()));
}

void TDLibReceiver::processUpdateDeleteMessages(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const QVariantList messageIds = receivedInformation.value(MESSAGE_IDS).toList();
    QList<qlonglong> ids;
    const int n = messageIds.size();
    ids.reserve(n);
    for (int i = 0; i < n; i++) {
        ids.append(messageIds.at(i).toLongLong());
    }
    LOG(n << "messages were deleted from chat" << chatId);
    emit messagesDeleted(chatId, ids);
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
    emit stickers(cleanupList(receivedInformation.value(STICKERS).toList()));
}

void TDLibReceiver::processUpdateInstalledStickerSets(const QVariantMap &receivedInformation)
{
    LOG("Recent sticker sets updated");
    emit installedStickerSetsUpdated(receivedInformation.value("sticker_set_ids").toList());
}

void TDLibReceiver::processStickerSets(const QVariantMap &receivedInformation)
{
    LOG("Received some sticker sets...");
    emit stickerSets(cleanupList(receivedInformation.value(SETS).toList()));
}

void TDLibReceiver::processStickerSet(const QVariantMap &receivedInformation)
{
    LOG("Received a sticker set...");
    emit stickerSet(cleanupMap(receivedInformation));
}
void TDLibReceiver::processChatMembers(const QVariantMap &receivedInformation)
{
    LOG("Received super group members");
    const QString extra = receivedInformation.value(_EXTRA).toString();
    emit chatMembers(extra, receivedInformation.value("members").toList(), receivedInformation.value(TOTAL_COUNT).toInt());
}

void TDLibReceiver::processUserFullInfo(const QVariantMap &receivedInformation)
{
    LOG("Received UserFullInfo");
    emit userFullInfo(receivedInformation);
}

void TDLibReceiver::processUpdateUserFullInfo(const QVariantMap &receivedInformation)
{
    LOG("Received UserFullInfoUpdate");
    emit userFullInfoUpdated(receivedInformation.value(USER_ID).toString(), receivedInformation.value("user_full_info").toMap());
}

void TDLibReceiver::processBasicGroupFullInfo(const QVariantMap &receivedInformation)
{
    LOG("Received BasicGroupFullInfo");
    const QString groupId = receivedInformation.value(_EXTRA).toString();
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
    const QString groupId = receivedInformation.value(_EXTRA).toString();
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
    const QString extra = receivedInformation.value(_EXTRA).toString();
    emit userProfilePhotos(extra, receivedInformation.value("photos").toList(), receivedInformation.value(TOTAL_COUNT).toInt());
}

void TDLibReceiver::processUpdateChatPermissions(const QVariantMap &receivedInformation)
{
    emit chatPermissionsUpdated(receivedInformation.value(CHAT_ID).toString(), receivedInformation.value("permissions").toMap());
}

void TDLibReceiver::processUpdateChatPhoto(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    LOG("Photo updated for chat" << chatId);
    emit chatPhotoUpdated(chatId, receivedInformation.value(PHOTO).toMap());
}

void TDLibReceiver::processUpdateChatTitle(const QVariantMap &receivedInformation)
{
    LOG("Received UpdateChatTitle");
    emit chatTitleUpdated(receivedInformation.value(CHAT_ID).toString(), receivedInformation.value(TITLE).toString());
}

void TDLibReceiver::processUpdateChatPinnedMessage(const QVariantMap &receivedInformation)
{
    LOG("Received UpdateChatPinnedMessage");
    emit chatPinnedMessageUpdated(receivedInformation.value(CHAT_ID).toLongLong(), receivedInformation.value("pinned_message_id").toLongLong());
}

void TDLibReceiver::processUpdateMessageIsPinned(const QVariantMap &receivedInformation)
{
    LOG("Received UpdateMessageIsPinned");
    emit messageIsPinnedUpdated(receivedInformation.value(CHAT_ID).toLongLong(), receivedInformation.value(MESSAGE_ID).toLongLong(), receivedInformation.value("is_pinned").toBool());
}

void TDLibReceiver::processUsers(const QVariantMap &receivedInformation)
{
    LOG("Received Users");
    emit usersReceived(receivedInformation.value(_EXTRA).toString(), receivedInformation.value("user_ids").toList(), receivedInformation.value(TOTAL_COUNT).toInt());
}

void TDLibReceiver::processError(const QVariantMap &receivedInformation)
{
    LOG("Received an error");
    emit errorReceived(receivedInformation.value("code").toInt(), receivedInformation.value(MESSAGE).toString(), receivedInformation.value(_EXTRA).toString());
}

void TDLibReceiver::ok(const QVariantMap &receivedInformation)
{
    LOG("Received an OK");
    if (receivedInformation.contains(_EXTRA)) {
        emit okReceived(receivedInformation.value(_EXTRA).toString());
    }
}

void TDLibReceiver::processSecretChat(const QVariantMap &receivedInformation)
{
    LOG("Received a secret chat");
    emit secretChat(receivedInformation.value(ID).toLongLong(), receivedInformation);
}

void TDLibReceiver::processUpdateSecretChat(const QVariantMap &receivedInformation)
{
    LOG("A secret chat was updated");
    QVariantMap updatedSecretChat = receivedInformation.value(SECRET_CHAT).toMap();
    emit secretChatUpdated(updatedSecretChat.value(ID).toLongLong(), updatedSecretChat);
}

void TDLibReceiver::processUpdateMessageEdited(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const qlonglong messageId = receivedInformation.value(MESSAGE_ID).toLongLong();
    LOG("Message was edited" << chatId << messageId);
    emit messageEditedUpdated(chatId, messageId, receivedInformation.value("reply_markup").toMap());
}

void TDLibReceiver::processImportedContacts(const QVariantMap &receivedInformation)
{
    LOG("Contacts were imported");
    emit contactsImported(receivedInformation.value("importer_count").toList(), receivedInformation.value("user_ids").toList());
}

void TDLibReceiver::processUpdateChatIsMarkedAsUnread(const QVariantMap &receivedInformation)
{
    LOG("The unread state of a chat was updated");
    emit chatIsMarkedAsUnreadUpdated(receivedInformation.value(CHAT_ID).toLongLong(), receivedInformation.value("is_marked_as_unread").toBool());
}

void TDLibReceiver::processUpdateChatDraftMessage(const QVariantMap &receivedInformation)
{
    LOG("Draft message was updated");
    emit chatDraftMessageUpdated(receivedInformation.value(CHAT_ID).toLongLong(), cleanupMap(receivedInformation.value(DRAFT_MESSAGE).toMap()), findChatPositionOrder(receivedInformation.value(POSITIONS).toList()));
}

void TDLibReceiver::processInlineQueryResults(const QVariantMap &receivedInformation)
{
    LOG("Inline Query results");
    emit inlineQueryResults(receivedInformation.value("inline_query_id").toString(), receivedInformation.value("next_offset").toString(), receivedInformation.value("results").toList(), receivedInformation.value("switch_pm_text").toString(), receivedInformation.value("switch_pm_parameter").toString(), receivedInformation.value(_EXTRA).toString());
}

void TDLibReceiver::processCallbackQueryAnswer(const QVariantMap &receivedInformation)
{
    LOG("Callback Query answer");
    emit callbackQueryAnswer(receivedInformation.value(TEXT).toString(), receivedInformation.value("alert").toBool(), receivedInformation.value("url").toString());
}

void TDLibReceiver::processUserPrivacySettingRules(const QVariantMap &receivedInformation)
{
    LOG("User privacy setting rules");
    emit userPrivacySettingRules(receivedInformation);
}

void TDLibReceiver::processUpdateUserPrivacySettingRules(const QVariantMap &receivedInformation)
{
    LOG("User privacy setting rules updated");
    emit userPrivacySettingRulesUpdated(receivedInformation);
}

void TDLibReceiver::processUpdateMessageInteractionInfo(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const qlonglong messageId = receivedInformation.value(MESSAGE_ID).toLongLong();
    LOG("Message interaction info updated" << chatId << messageId);
    emit messageInteractionInfoUpdated(chatId, messageId, receivedInformation.value(INTERACTION_INFO).toMap());
}

void TDLibReceiver::processSessions(const QVariantMap &receivedInformation)
{
    int inactive_session_ttl_days = receivedInformation.value("inactive_session_ttl_days").toInt();
    QVariantList sessions = receivedInformation.value("sessions").toList();
    emit sessionsReceived(inactive_session_ttl_days, sessions);
}

void TDLibReceiver::processAvailableReactions(const QVariantMap &receivedInformation)
{
    const qlonglong messageId = receivedInformation.value(_EXTRA).toLongLong();
    const QStringList reactions = receivedInformation.value("reactions").toStringList();
    if (!reactions.isEmpty()) {
        emit availableReactionsReceived(messageId, reactions);
    }
}

void TDLibReceiver::processUpdateChatUnreadMentionCount(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const int unreadMentionCount = receivedInformation.value(UNREAD_MENTION_COUNT).toInt();
    LOG("Chat unread mention count updated" << chatId << unreadMentionCount);
    emit chatUnreadMentionCountUpdated(chatId, unreadMentionCount);
}

void TDLibReceiver::processUpdateChatUnreadReactionCount(const QVariantMap &receivedInformation)
{
    const qlonglong chatId = receivedInformation.value(CHAT_ID).toLongLong();
    const int unreadReactionCount = receivedInformation.value(UNREAD_REACTION_COUNT).toInt();
    LOG("Chat unread reaction count updated" << chatId << unreadReactionCount);
    emit chatUnreadReactionCountUpdated(chatId, unreadReactionCount);
}

void TDLibReceiver::processUpdateActiveEmojiReactions(const QVariantMap &receivedInformation)
{
    // updateActiveEmojiReactions was introduced between 1.8.5 and 1.8.6
    // See https://github.com/tdlib/td/commit/d29d367
    emit activeEmojiReactionsUpdated(receivedInformation.value(EMOJIS).toStringList());
}

// Recursively removes (some) unused entries from QVariantMaps to reduce
// memory usage. QStrings allocated by QVariantMaps are the top consumers
// of memory. The biggest saving is achieved by removing "outline" from
// stickers.
const QVariantMap TDLibReceiver::cleanupMap(const QVariantMap& map, bool *updated)
{
    const QString type(map.value(_TYPE).toString());
    if (type == TYPE_STICKER) {
        QVariantMap sticker(map);
        if (sticker.remove(OUTLINE)) {
            sticker.remove(_TYPE);
            sticker.insert(_TYPE, TYPE_STICKER); // Replace with a shared value
            if (updated) *updated = true;
            return sticker;
        }
    } else if (type == TYPE_ANIMATED_EMOJI) {
        bool cleaned = false;
        const QVariantMap sticker(cleanupMap(map.value(STICKER).toMap(), &cleaned));
        if (cleaned) {
            QVariantMap animated_emoji(map);
            animated_emoji.remove(STICKER);
            animated_emoji.insert(STICKER, sticker);
            animated_emoji.remove(COLOR_REPLACEMENTS);
            animated_emoji.remove(_TYPE);
            animated_emoji.insert(_TYPE, TYPE_ANIMATED_EMOJI); // Replace with a shared value
            if (updated) *updated = true;
            return animated_emoji;
        }
    } else if (type == TYPE_MESSAGE) {
        QVariantMap message(map);
        bool messageChanged = false;
        const QVariantMap content(cleanupMap(map.value(CONTENT).toMap(), &messageChanged));
        if (messageChanged) {
            message.remove(CONTENT);
            message.insert(CONTENT, content);
        }
        if (map.contains(REPLY_TO)) {
            // In TdLib 1.8.15 reply_to_message_id and reply_in_chat_id attributes
            // had been replaced with reply_to structure, e.g:
            //
            //     "reply_to": {
            //         "@type": "messageReplyToMessage",
            //         "chat_id": -1001234567890,
            //         "is_quote_manual": false,
            //         "message_id": 234567890,
            //         "origin_send_date": 0
            //     }
            //
            QVariantMap reply_to(message.value(REPLY_TO).toMap());
            if (reply_to.value(_TYPE).toString() == TYPE_MESSAGE_REPLY_TO_MESSAGE) {
                if (reply_to.contains(MESSAGE_ID) &&
                    !message.contains(REPLY_TO_MESSAGE_ID)) {
                    message.insert(REPLY_TO_MESSAGE_ID, reply_to.value(MESSAGE_ID));
                }
                if (reply_to.contains(CHAT_ID) &&
                    !message.contains(REPLY_IN_CHAT_ID)) {
                    message.insert(REPLY_IN_CHAT_ID, reply_to.value(CHAT_ID));
                }
                reply_to.remove(_TYPE);
                reply_to.insert(_TYPE, TYPE_MESSAGE_REPLY_TO_MESSAGE);
                message.insert(REPLY_TO, reply_to);
                messageChanged = true;
            }
        }
        if (messageChanged) {
            message.remove(_TYPE);
            message.insert(_TYPE, TYPE_MESSAGE); // Replace with a shared value
            if (updated) *updated = true;
            return message;
        }
    } else if (type == TYPE_DRAFT_MESSAGE) {
        QVariantMap draftMessage(map);
        QVariantMap reply_to(draftMessage.value(REPLY_TO).toMap());
        // In TdLib 1.8.21 reply_to_message_id has been replaced with reply_to
        if (reply_to.value(_TYPE).toString() == TYPE_INPUT_MESSAGE_REPLY_TO_MESSAGE) {
            if (reply_to.contains(MESSAGE_ID) &&
                !draftMessage.contains(REPLY_TO_MESSAGE_ID)) {
                // reply_to_message_id is what QML (still) expects
                draftMessage.insert(REPLY_TO_MESSAGE_ID, reply_to.value(MESSAGE_ID));
            }
            reply_to.remove(_TYPE);
            reply_to.insert(_TYPE, TYPE_INPUT_MESSAGE_REPLY_TO_MESSAGE); // Shared value
            draftMessage.insert(REPLY_TO, reply_to);
            draftMessage.remove(_TYPE);
            draftMessage.insert(_TYPE, DRAFT_MESSAGE); // Shared value
            if (updated) *updated = true;
            return draftMessage;
        }
    } else if (type == TYPE_MESSAGE_STICKER) {
        bool cleaned = false;
        const QVariantMap content(cleanupMap(map.value(CONTENT).toMap(), &cleaned));
        if (cleaned) {
            QVariantMap messageSticker(map);
            messageSticker.remove(CONTENT);
            messageSticker.insert(CONTENT, content);
            messageSticker.remove(_TYPE);
            messageSticker.insert(_TYPE, TYPE_MESSAGE_STICKER); // Replace with a shared value
            if (updated) *updated = true;
            return messageSticker;
        }
    } else if (type == TYPE_MESSAGE_ANIMATED_EMOJI) {
        bool cleaned = false;
        const QVariantMap animated_emoji(cleanupMap(map.value(ANIMATED_EMOJI).toMap(), &cleaned));
        if (cleaned) {
            QVariantMap messageAnimatedEmoji(map);
            messageAnimatedEmoji.remove(ANIMATED_EMOJI);
            messageAnimatedEmoji.insert(ANIMATED_EMOJI, animated_emoji);
            messageAnimatedEmoji.remove(_TYPE);
            messageAnimatedEmoji.insert(_TYPE, TYPE_MESSAGE_ANIMATED_EMOJI); // Replace with a shared value
            if (updated) *updated = true;
            return messageAnimatedEmoji;
        }
    } else if (type == TYPE_STICKER_SET_INFO) {
        bool cleaned = false;
        const QVariantList covers(cleanupList(map.value(COVERS).toList(), &cleaned));
        if (cleaned) {
            QVariantMap stickerSetInfo(map);
            stickerSetInfo.remove(COVERS);
            stickerSetInfo.insert(COVERS, covers);
            stickerSetInfo.remove(_TYPE);
            stickerSetInfo.insert(_TYPE, TYPE_STICKER_SET_INFO); // Replace with a shared value
            if (updated) *updated = true;
            return stickerSetInfo;
        }
    } else if (type == TYPE_STICKER_SET) {
        bool cleaned = false;
        const QVariantList stickers(cleanupList(map.value(STICKERS).toList(), &cleaned));
        if (cleaned) {
            QVariantMap stickerSet(map);
            stickerSet.remove(STICKERS);
            stickerSet.insert(STICKERS, stickers);
            stickerSet.remove(_TYPE);
            stickerSet.insert(_TYPE, TYPE_STICKER_SET); // Replace with a shared value
            if (updated) *updated = true;
            return stickerSet;
        }
    }
    if (updated) *updated = false;
    return map;
}

const QVariantList TDLibReceiver::cleanupList(const QVariantList& list, bool *updated)
{
    QVariantList newList(list);
    bool somethingChanged = false;
    const int n = list.count();
    for (int i = 0; i < n; i++) {
        bool cleaned = false;
        const QVariantMap entry(cleanupMap(list.at(i).toMap(), &cleaned));
        if (cleaned) {
            newList.replace(i, entry);
            somethingChanged = true;
        }
    }
    if (somethingChanged) {
        if (updated) *updated = true;
        return newList;
    } else {
        return list;
    }
}
