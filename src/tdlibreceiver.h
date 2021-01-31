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
#ifndef TDLIBRECEIVER_H
#define TDLIBRECEIVER_H

#include <QHash>
#include <QVariantMap>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <td/telegram/td_json_client.h>

class TDLibReceiver : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        receiverLoop();
    }
public:
    explicit TDLibReceiver(void *tdLibClient, QObject *parent = nullptr);
    void setActive(bool active);

signals:
    void versionDetected(const QString &version);
    void authorizationStateChanged(const QString &authorizationState, const QVariantMap &authorizationStateData);
    void optionUpdated(const QString &optionName, const QVariant &optionValue);
    void connectionStateChanged(const QString &connectionState);
    void userUpdated(const QVariantMap &userInformation);
    void userStatusUpdated(const QString &userId, const QVariantMap &userStatusInformation);
    void fileUpdated(const QVariantMap &fileInformation);
    void newChatDiscovered(const QVariantMap &chatInformation);
    void unreadMessageCountUpdated(const QVariantMap &messageCountInformation);
    void unreadChatCountUpdated(const QVariantMap &chatCountInformation);
    void chatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void chatOrderUpdated(const QString &chatId, const QString &order);
    void chatPinnedUpdated(qlonglong chatId, bool isPinned);
    void chatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, int unreadCount);
    void chatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void basicGroupUpdated(qlonglong groupId, const QVariantMap &groupInformation);
    void superGroupUpdated(qlonglong groupId, const QVariantMap &groupInformation);
    void chatOnlineMemberCountUpdated(const QString &chatId, int onlineMemberCount);
    void messagesReceived(const QVariantList &messages, int totalCount);
    void newMessageReceived(qlonglong chatId, const QVariantMap &message);
    void messageInformation(qlonglong chatId, qlonglong messageId, const QVariantMap &message);
    void messageSendSucceeded(qlonglong messageId, qlonglong oldMessageId, const QVariantMap &message);
    void activeNotificationsUpdated(const QVariantList notificationGroups);
    void notificationGroupUpdated(const QVariantMap notificationGroupUpdate);
    void notificationUpdated(const QVariantMap updatedNotification);
    void chatNotificationSettingsUpdated(const QString &chatId, const QVariantMap updatedChatNotificationSettings);
    void messageContentUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &newContent);
    void messageEditedUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &replyMarkup);
    void messagesDeleted(qlonglong chatId, const QList<qlonglong> &messageIds);
    void chats(const QVariantMap &chats);
    void chat(const QVariantMap &chats);
    void recentStickersUpdated(const QVariantList &stickerIds);
    void stickers(const QVariantList &stickers);
    void installedStickerSetsUpdated(const QVariantList &stickerSetIds);
    void stickerSets(const QVariantList &stickerSets);
    void stickerSet(const QVariantMap &stickerSet);
    void chatMembers(const QString &extra, const QVariantList &members, int totalMembers);
    void userFullInfo(const QVariantMap &userFullInfo);
    void userFullInfoUpdated(const QString &userId, const QVariantMap &userFullInfo);
    void basicGroupFullInfo(const QString &groupId, const QVariantMap &groupFullInfo);
    void basicGroupFullInfoUpdated(const QString &groupId, const QVariantMap &groupFullInfo);
    void supergroupFullInfo(const QString &groupId, const QVariantMap &groupFullInfo);
    void supergroupFullInfoUpdated(const QString &groupId, const QVariantMap &groupFullInfo);
    void userProfilePhotos(const QString &extra, const QVariantList &photos, int totalPhotos);
    void chatPermissionsUpdated(const QString &chatId, const QVariantMap &chatPermissions);
    void chatPhotoUpdated(qlonglong chatId, const QVariantMap &photo);
    void chatTitleUpdated(const QString &chatId, const QString &title);
    void chatPinnedMessageUpdated(qlonglong chatId, qlonglong pinnedMessageId);
    void messageIsPinnedUpdated(qlonglong chatId, qlonglong messageId, bool isPinned);
    void usersReceived(const QString &extra, const QVariantList &userIds, int totalUsers);
    void errorReceived(const int code, const QString &message, const QString &extra);
    void secretChat(qlonglong secretChatId, const QVariantMap &secretChat);
    void secretChatUpdated(qlonglong secretChatId, const QVariantMap &secretChat);
    void contactsImported(const QVariantList &importerCount, const QVariantList &userIds);
    void chatIsMarkedAsUnreadUpdated(qlonglong chatId, bool chatIsMarkedAsUnread);
    void chatDraftMessageUpdated(qlonglong chatId, const QVariantMap &draftMessage, const QString &order);
    void inlineQueryResults(const QString &inlineQueryId, const QString &nextOffset, const QVariantList &results, const QString &switchPmText, const QString &switchPmParameter, const QString &extra);
    void callbackQueryAnswer(const QString &text, bool alert, const QString &url);
    void userPrivacySettingRules(const QVariantMap &rules);
    void userPrivacySettingRulesUpdated(const QVariantMap &updatedRules);
    void messageInteractionInfoUpdated(qlonglong chatId, qlonglong messageId, const QVariantMap &updatedInfo);
    void okReceived(const QString &request);

private:
    typedef void (TDLibReceiver::*Handler)(const QVariantMap &);

    QHash<QString, Handler> handlers;
    void *tdLibClient;
    bool isActive;

    void receiverLoop();
    void nop(const QVariantMap &receivedInformation);
    void processReceivedDocument(const QJsonDocument &receivedJsonDocument);
    void processUpdateOption(const QVariantMap &receivedInformation);
    void processUpdateAuthorizationState(const QVariantMap &receivedInformation);
    void processUpdateConnectionState(const QVariantMap &receivedInformation);
    void processUpdateUser(const QVariantMap &receivedInformation);
    void processUpdateUserStatus(const QVariantMap &receivedInformation);
    void processUpdateFile(const QVariantMap &receivedInformation);
    void processFile(const QVariantMap &receivedInformation);
    void processUpdateNewChat(const QVariantMap &receivedInformation);
    void processUpdateUnreadMessageCount(const QVariantMap &receivedInformation);
    void processUpdateUnreadChatCount(const QVariantMap &receivedInformation);
    void processUpdateChatLastMessage(const QVariantMap &receivedInformation);
    void processUpdateChatOrder(const QVariantMap &receivedInformation);
    void processUpdateChatPosition(const QVariantMap &receivedInformation);
    void processUpdateChatReadInbox(const QVariantMap &receivedInformation);
    void processUpdateChatReadOutbox(const QVariantMap &receivedInformation);
    void processUpdateBasicGroup(const QVariantMap &receivedInformation);
    void processUpdateSuperGroup(const QVariantMap &receivedInformation);
    void processChatOnlineMemberCountUpdated(const QVariantMap &receivedInformation);
    void processMessages(const QVariantMap &receivedInformation);
    void processUpdateNewMessage(const QVariantMap &receivedInformation);
    void processMessage(const QVariantMap &receivedInformation);
    void processMessageSendSucceeded(const QVariantMap &receivedInformation);
    void processUpdateActiveNotifications(const QVariantMap &receivedInformation);
    void processUpdateNotificationGroup(const QVariantMap &receivedInformation);
    void processUpdateNotification(const QVariantMap &receivedInformation);
    void processUpdateChatNotificationSettings(const QVariantMap &receivedInformation);
    void processUpdateMessageContent(const QVariantMap &receivedInformation);
    void processUpdateDeleteMessages(const QVariantMap &receivedInformation);
    void processChats(const QVariantMap &receivedInformation);
    void processChat(const QVariantMap &receivedInformation);
    void processUpdateRecentStickers(const QVariantMap &receivedInformation);
    void processStickers(const QVariantMap &receivedInformation);
    void processUpdateInstalledStickerSets(const QVariantMap &receivedInformation);
    void processStickerSets(const QVariantMap &receivedInformation);
    void processStickerSet(const QVariantMap &receivedInformation);
    void processChatMembers(const QVariantMap &receivedInformation);
    void processUserFullInfo(const QVariantMap &receivedInformation);
    void processUpdateUserFullInfo(const QVariantMap &receivedInformation);
    void processBasicGroupFullInfo(const QVariantMap &receivedInformation);
    void processUpdateBasicGroupFullInfo(const QVariantMap &receivedInformation);
    void processSupergroupFullInfo(const QVariantMap &receivedInformation);
    void processUpdateSupergroupFullInfo(const QVariantMap &receivedInformation);
    void processUserProfilePhotos(const QVariantMap &receivedInformation);
    void processUpdateChatPermissions(const QVariantMap &receivedInformation);
    void processUpdateChatPhoto(const QVariantMap &receivedInformation);
    void processUpdateChatTitle(const QVariantMap &receivedInformation);
    void processUpdateChatPinnedMessage(const QVariantMap &receivedInformation);
    void processUpdateMessageIsPinned(const QVariantMap &receivedInformation);
    void processUsers(const QVariantMap &receivedInformation);
    void processError(const QVariantMap &receivedInformation);
    void processSecretChat(const QVariantMap &receivedInformation);
    void processUpdateSecretChat(const QVariantMap &receivedInformation);
    void processUpdateMessageEdited(const QVariantMap &receivedInformation);
    void processImportedContacts(const QVariantMap &receivedInformation);
    void processUpdateChatIsMarkedAsUnread(const QVariantMap &receivedInformation);
    void processUpdateChatDraftMessage(const QVariantMap &receivedInformation);
    void processInlineQueryResults(const QVariantMap &receivedInformation);
    void processCallbackQueryAnswer(const QVariantMap &receivedInformation);
    void processUserPrivacySettingRules(const QVariantMap &receivedInformation);
    void processUpdateUserPrivacySettingRules(const QVariantMap &receivedInformation);
    void processUpdateMessageInteractionInfo(const QVariantMap &receivedInformation);
};

#endif // TDLIBRECEIVER_H
