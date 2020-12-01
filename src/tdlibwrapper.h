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
#ifndef TDLIBWRAPPER_H
#define TDLIBWRAPPER_H

#include <QCoreApplication>
#include <td/telegram/td_json_client.h>
#include "tdlibreceiver.h"
#include "dbusadaptor.h"
#include "dbusinterface.h"
#include "emojisearchworker.h"
#include "appsettings.h"
#include "mceinterface.h"

class TDLibWrapper : public QObject
{
    Q_OBJECT
public:
    explicit TDLibWrapper(AppSettings *appSettings, MceInterface *mceInterface, QObject *parent = nullptr);
    ~TDLibWrapper();

    enum AuthorizationState {
        Closed,
        Closing,
        LoggingOut,
        AuthorizationReady,
        WaitCode,
        WaitEncryptionKey,
        WaitOtherDeviceConfirmation,
        WaitPassword,
        WaitPhoneNumber,
        WaitRegistration,
        WaitTdlibParameters
    };
    Q_ENUM(AuthorizationState)

    enum ConnectionState {
        Connecting,
        ConnectingToProxy,
        ConnectionReady,
        Updating,
        WaitingForNetwork
    };
    Q_ENUM(ConnectionState)

    enum ChatType {
        ChatTypeUnknown,
        ChatTypePrivate,
        ChatTypeBasicGroup,
        ChatTypeSupergroup,
        ChatTypeSecret
    };
    Q_ENUM(ChatType)

    enum ChatMemberStatus {
        ChatMemberStatusUnknown,
        ChatMemberStatusCreator,
        ChatMemberStatusAdministrator,
        ChatMemberStatusMember,
        ChatMemberStatusRestricted,
        ChatMemberStatusLeft,
        ChatMemberStatusBanned
    };
    Q_ENUM(ChatMemberStatus)

    enum SecretChatState {
        SecretChatStateUnknown,
        SecretChatStateClosed,
        SecretChatStatePending,
        SecretChatStateReady,
    };
    Q_ENUM(SecretChatState)

    class Group {
    public:
        Group(qlonglong id) : groupId(id) { }
        ChatMemberStatus chatMemberStatus() const;
    public:
        const qlonglong groupId;
        QVariantMap groupInfo;
    };

    Q_INVOKABLE QString getVersion();
    Q_INVOKABLE TDLibWrapper::AuthorizationState getAuthorizationState();
    Q_INVOKABLE QVariantMap getAuthorizationStateData();
    Q_INVOKABLE TDLibWrapper::ConnectionState getConnectionState();
    Q_INVOKABLE QVariantMap getUserInformation();
    Q_INVOKABLE QVariantMap getUserInformation(const QString &userId);
    Q_INVOKABLE bool hasUserInformation(const QString &userId);
    Q_INVOKABLE QVariantMap getUserInformationByName(const QString &userName);
    Q_INVOKABLE QVariantMap getUnreadMessageInformation();
    Q_INVOKABLE QVariantMap getUnreadChatInformation();
    Q_INVOKABLE QVariantMap getBasicGroup(qlonglong groupId) const;
    Q_INVOKABLE QVariantMap getSuperGroup(qlonglong groupId) const;
    Q_INVOKABLE QVariantMap getChat(const QString &chatId);
    Q_INVOKABLE QVariantMap getSecretChatFromCache(qlonglong secretChatId);
    Q_INVOKABLE QString getOptionString(const QString &optionName);
    Q_INVOKABLE void copyFileToDownloads(const QString &filePath);
    Q_INVOKABLE void openFileOnDevice(const QString &filePath);
    Q_INVOKABLE void controlScreenSaver(bool enabled);
    Q_INVOKABLE bool getJoinChatRequested();
    Q_INVOKABLE void registerJoinChat();

    DBusAdaptor *getDBusAdaptor();

    // Direct TDLib functions
    Q_INVOKABLE void sendRequest(const QVariantMap &requestObject);
    Q_INVOKABLE void setAuthenticationPhoneNumber(const QString &phoneNumber);
    Q_INVOKABLE void setAuthenticationCode(const QString &authenticationCode);
    Q_INVOKABLE void setAuthenticationPassword(const QString &authenticationPassword);
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName);
    Q_INVOKABLE void getChats();
    Q_INVOKABLE void downloadFile(int fileId);
    Q_INVOKABLE void openChat(const QString &chatId);
    Q_INVOKABLE void closeChat(const QString &chatId);
    Q_INVOKABLE void joinChat(const QString &chatId);
    Q_INVOKABLE void leaveChat(const QString &chatId);
    Q_INVOKABLE void getChatHistory(qlonglong chatId, const qlonglong &fromMessageId = 0, int offset = 0, int limit = 50, bool onlyLocal = false);
    Q_INVOKABLE void viewMessage(const QString &chatId, const QString &messageId, bool force);
    Q_INVOKABLE void pinMessage(const QString &chatId, const QString &messageId, bool disableNotification = false);
    Q_INVOKABLE void unpinMessage(const QString &chatId);
    Q_INVOKABLE void sendTextMessage(const QString &chatId, const QString &message, const QString &replyToMessageId = "0");
    Q_INVOKABLE void sendPhotoMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId = "0");
    Q_INVOKABLE void sendVideoMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId = "0");
    Q_INVOKABLE void sendDocumentMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId = "0");
    Q_INVOKABLE void sendStickerMessage(const QString &chatId, const QString &fileId, const QString &replyToMessageId = "0");
    Q_INVOKABLE void sendPollMessage(const QString &chatId, const QString &question, const QVariantList &options, const bool &anonymous, const int &correctOption, const bool &multiple, const QString &replyToMessageId = "0");
    Q_INVOKABLE void forwardMessages(const QString &chatId, const QString &fromChatId, const QVariantList &messageIds, const bool sendCopy, const bool removeCaption);
    Q_INVOKABLE void getMessage(const QString &chatId, const QString &messageId);
    Q_INVOKABLE void setOptionInteger(const QString &optionName, int optionValue);
    Q_INVOKABLE void setOptionBoolean(const QString &optionName, bool optionValue);
    Q_INVOKABLE void setChatNotificationSettings(const QString &chatId, const QVariantMap &notificationSettings);
    Q_INVOKABLE void editMessageText(const QString &chatId, const QString &messageId, const QString &message);
    Q_INVOKABLE void deleteMessages(const QString &chatId, const QVariantList messageIds);
    Q_INVOKABLE void getMapThumbnailFile(const QString &chatId, double latitude, double longitude, int width, int height);
    Q_INVOKABLE void getRecentStickers();
    Q_INVOKABLE void getInstalledStickerSets();
    Q_INVOKABLE void getStickerSet(const QString &setId);
    Q_INVOKABLE void getSupergroupMembers(const QString &groupId, int limit, int offset);
    Q_INVOKABLE void getGroupFullInfo(const QString &groupId, bool isSuperGroup);
    Q_INVOKABLE void getUserFullInfo(const QString &userId);
    Q_INVOKABLE void createPrivateChat(const QString &userId);
    Q_INVOKABLE void createNewSecretChat(const QString &userId);
    Q_INVOKABLE void createSupergroupChat(const QString &supergroupId);
    Q_INVOKABLE void createBasicGroupChat(const QString &basicGroupId);
    Q_INVOKABLE void getGroupsInCommon(const QString &userId, int limit, int offset);
    Q_INVOKABLE void getUserProfilePhotos(const QString &userId, int limit, int offset);
    Q_INVOKABLE void setChatPermissions(const QString &chatId, const QVariantMap &chatPermissions);
    Q_INVOKABLE void setChatSlowModeDelay(const QString &chatId, int delay);
    Q_INVOKABLE void setChatDescription(const QString &chatId, const QString &description);
    Q_INVOKABLE void setChatTitle(const QString &chatId, const QString &title);
    Q_INVOKABLE void setBio(const QString &bio);
    Q_INVOKABLE void toggleSupergroupIsAllHistoryAvailable(const QString &groupId, bool isAllHistoryAvailable);
    Q_INVOKABLE void setPollAnswer(const QString &chatId, const qlonglong &messageId, QVariantList optionIds);
    Q_INVOKABLE void stopPoll(const QString &chatId, const qlonglong &messageId);
    Q_INVOKABLE void getPollVoters(const QString &chatId, const qlonglong &messageId, const int &optionId, const int &limit, const int &offset, const QString &extra);
    Q_INVOKABLE void searchPublicChat(const QString &userName);
    Q_INVOKABLE void joinChatByInviteLink(const QString &inviteLink);
    Q_INVOKABLE void getDeepLinkInfo(const QString &link);
    Q_INVOKABLE void getContacts();
    Q_INVOKABLE void getSecretChat(qlonglong secretChatId);
    Q_INVOKABLE void closeSecretChat(qlonglong secretChatId);
    Q_INVOKABLE void importContacts(const QVariantList &contacts);

    // Others (candidates for extraction ;))
    Q_INVOKABLE void searchEmoji(const QString &queryString);
    Q_INVOKABLE void initializeOpenWith();
    Q_INVOKABLE void removeOpenWith();

public:
    const Group* getGroup(qlonglong groupId) const;
    static ChatType chatTypeFromString(const QString &type);
    static ChatMemberStatus chatMemberStatusFromString(const QString &status);
    static SecretChatState secretChatStateFromString(const QString &state);

signals:
    void versionDetected(const QString &version);
    void ownUserIdFound(const QString &ownUserId);
    void authorizationStateChanged(const TDLibWrapper::AuthorizationState &authorizationState, const QVariantMap &authorizationStateData);
    void optionUpdated(const QString &optionName, const QVariant &optionValue);
    void connectionStateChanged(const TDLibWrapper::ConnectionState &connectionState);
    void fileUpdated(int fileId, const QVariantMap &fileInformation);
    void newChatDiscovered(const QString &chatId, const QVariantMap &chatInformation);
    void unreadMessageCountUpdated(const QVariantMap &messageCountInformation);
    void unreadChatCountUpdated(const QVariantMap &chatCountInformation);
    void chatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void chatOrderUpdated(const QString &chatId, const QString &order);
    void chatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, int unreadCount);
    void chatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void userUpdated(const QString &userId, const QVariantMap &userInformation);
    void basicGroupUpdated(qlonglong groupId);
    void superGroupUpdated(qlonglong groupId);
    void chatOnlineMemberCountUpdated(const QString &chatId, int onlineMemberCount);
    void messagesReceived(const QVariantList &messages, int totalCount);
    void newMessageReceived(const QString &chatId, const QVariantMap &message);
    void copyToDownloadsSuccessful(const QString &fileName, const QString &filePath);
    void copyToDownloadsError(const QString &fileName, const QString &filePath);
    void receivedMessage(const QString &messageId, const QVariantMap &message);
    void messageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);
    void activeNotificationsUpdated(const QVariantList notificationGroups);
    void notificationGroupUpdated(const QVariantMap notificationGroupUpdate);
    void notificationUpdated(const QVariantMap updatedNotification);
    void chatNotificationSettingsUpdated(const QString &chatId, const QVariantMap chatNotificationSettings);
    void messageContentUpdated(const QString &chatId, const QString &messageId, const QVariantMap &newContent);
    void messagesDeleted(const QString &chatId, const QVariantList &messageIds);
    void chatsReceived(const QVariantMap &chats);
    void chatReceived(const QVariantMap &chat);
    void secretChatReceived(qlonglong secretChatId, const QVariantMap &secretChat);
    void secretChatUpdated(qlonglong secretChatId, const QVariantMap &secretChat);
    void recentStickersUpdated(const QVariantList &stickerIds);
    void stickersReceived(const QVariantList &stickers);
    void installedStickerSetsUpdated(const QVariantList &stickerSetIds);
    void stickerSetsReceived(const QVariantList &stickerSets);
    void stickerSetReceived(const QVariantMap &stickerSet);
    void emojiSearchSuccessful(const QVariantList &result);
    void chatMembersReceived(const QString &extra, const QVariantList &members, int totalMembers);
    void userFullInfoReceived(const QVariantMap &userFullInfo);
    void userFullInfoUpdated(const QString &userId, const QVariantMap &userFullInfo);
    void basicGroupFullInfoReceived(const QString &groupId, const QVariantMap &groupFullInfo);
    void supergroupFullInfoReceived(const QString &groupId, const QVariantMap &groupFullInfo);
    void basicGroupFullInfoUpdated(const QString &groupId, const QVariantMap &groupFullInfo);
    void supergroupFullInfoUpdated(const QString &groupId, const QVariantMap &groupFullInfo);
    void userProfilePhotosReceived(const QString &extra, const QVariantList &photos, int totalPhotos);
    void chatPermissionsUpdated(const QString &chatId, const QVariantMap &permissions);
    void chatPhotoUpdated(qlonglong chatId, const QVariantMap &photo);
    void chatTitleUpdated(const QString &chatId, const QString &title);
    void chatPinnedMessageUpdated(qlonglong chatId, qlonglong pinnedMessageId);
    void usersReceived(const QString &extra, const QVariantList &userIds, int totalUsers);
    void errorReceived(const int code, const QString &message);
    void contactsImported(const QVariantList &importerCount, const QVariantList &userIds);

public slots:
    void handleVersionDetected(const QString &version);
    void handleAuthorizationStateChanged(const QString &authorizationState, const QVariantMap authorizationStateData);
    void handleOptionUpdated(const QString &optionName, const QVariant &optionValue);
    void handleConnectionStateChanged(const QString &connectionState);
    void handleUserUpdated(const QVariantMap &userInformation);
    void handleUserStatusUpdated(const QString &userId, const QVariantMap &userStatusInformation);
    void handleFileUpdated(const QVariantMap &fileInformation);
    void handleNewChatDiscovered(const QVariantMap &chatInformation);
    void handleChatReceived(const QVariantMap &chatInformation);
    void handleUnreadMessageCountUpdated(const QVariantMap &messageCountInformation);
    void handleUnreadChatCountUpdated(const QVariantMap &chatCountInformation);
    void handleBasicGroupUpdated(qlonglong groupId, const QVariantMap &groupInformation);
    void handleSuperGroupUpdated(qlonglong groupId, const QVariantMap &groupInformation);
    void handleStickerSets(const QVariantList &stickerSets);
    void handleEmojiSearchCompleted(const QString &queryString, const QVariantList &resultList);
    void handleOpenWithChanged();
    void handleSecretChatReceived(qlonglong secretChatId, const QVariantMap &secretChat);
    void handleSecretChatUpdated(qlonglong secretChatId, const QVariantMap &secretChat);
    void handleStorageOptimizerChanged();

private:
    void setOption(const QString &name, const QString &type, const QVariant &value);
    void setInitialParameters();
    void setEncryptionKey();
    void setLogVerbosityLevel();
    const Group *updateGroup(qlonglong groupId, const QVariantMap &groupInfo, QHash<qlonglong,Group*> *groups);

private:
    void *tdLibClient;
    AppSettings *appSettings;
    MceInterface *mceInterface;
    TDLibReceiver *tdLibReceiver;
    DBusInterface *dbusInterface;
    QString version;
    TDLibWrapper::AuthorizationState authorizationState;
    QVariantMap authorizationStateData;
    TDLibWrapper::ConnectionState connectionState;
    QVariantMap options;
    QVariantMap userInformation;
    QVariantMap allUsers;
    QVariantMap allUserNames;
    QVariantMap chats;
    QMap<qlonglong, QVariantMap> secretChats;
    QVariantMap unreadMessageInformation;
    QVariantMap unreadChatInformation;
    QHash<qlonglong,Group*> basicGroups;
    QHash<qlonglong,Group*> superGroups;
    EmojiSearchWorker emojiSearchWorker;

    QString activeChatSearchName;
    bool joinChatRequested;

};

#endif // TDLIBWRAPPER_H
