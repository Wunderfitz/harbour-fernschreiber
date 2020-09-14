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
#ifndef TDLIBWRAPPER_H
#define TDLIBWRAPPER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QStandardPaths>
#include <td/telegram/td_json_client.h>
#include "tdlibreceiver.h"
#include "dbusadaptor.h"
#include "dbusinterface.h"

class TDLibWrapper : public QObject
{
    Q_OBJECT
public:
    explicit TDLibWrapper(QObject *parent = nullptr);
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

    Q_INVOKABLE QString getVersion();
    Q_INVOKABLE TDLibWrapper::AuthorizationState getAuthorizationState();
    Q_INVOKABLE TDLibWrapper::ConnectionState getConnectionState();
    Q_INVOKABLE QVariantMap getUserInformation();
    Q_INVOKABLE QVariantMap getUserInformation(const QString &userId);
    Q_INVOKABLE QVariantMap getUnreadMessageInformation();
    Q_INVOKABLE QVariantMap getUnreadChatInformation();
    Q_INVOKABLE QVariantMap getBasicGroup(const QString &groupId);
    Q_INVOKABLE QVariantMap getSuperGroup(const QString &groupId);
    Q_INVOKABLE void copyFileToDownloads(const QString &filePath);
    Q_INVOKABLE void openFileOnDevice(const QString &filePath);
    Q_INVOKABLE void controlScreenSaver(const bool &enabled);

    DBusAdaptor *getDBusAdaptor();

    // Direct TDLib functions
    Q_INVOKABLE void sendRequest(const QVariantMap &requestObject);
    Q_INVOKABLE void setAuthenticationPhoneNumber(const QString &phoneNumber);
    Q_INVOKABLE void setAuthenticationCode(const QString &authenticationCode);
    Q_INVOKABLE void getChats();
    Q_INVOKABLE void downloadFile(const QString &fileId);
    Q_INVOKABLE void openChat(const QString &chatId);
    Q_INVOKABLE void closeChat(const QString &chatId);
    Q_INVOKABLE void getChatHistory(const QString &chatId, const qlonglong &fromMessageId = 0, const int &offset = 0, const int &limit = 50, const bool &onlyLocal = false);
    Q_INVOKABLE void viewMessage(const QString &chatId, const QString &messageId);
    Q_INVOKABLE void sendTextMessage(const QString &chatId, const QString &message, const QString &replyToMessageId = "0");
    Q_INVOKABLE void getMessage(const QString &chatId, const QString &messageId);
    Q_INVOKABLE void setOptionInteger(const QString &optionName, const int &optionValue);

signals:
    void versionDetected(const QString &version);
    void ownUserIdFound(const QString &ownUserId);
    void authorizationStateChanged(const TDLibWrapper::AuthorizationState &authorizationState);
    void optionUpdated(const QString &optionName, const QVariant &optionValue);
    void connectionStateChanged(const TDLibWrapper::ConnectionState &connectionState);
    void fileUpdated(const int fileId, const QVariantMap &fileInformation);
    void newChatDiscovered(const QString &chatId, const QVariantMap &chatInformation);
    void unreadMessageCountUpdated(const QVariantMap &messageCountInformation);
    void unreadChatCountUpdated(const QVariantMap &chatCountInformation);
    void chatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void chatOrderUpdated(const QString &chatId, const QString &order);
    void chatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, const int &unreadCount);
    void chatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void userUpdated(const QString &userId, const QVariantMap &userInformation);
    void basicGroupUpdated(const QString &groupId, const QVariantMap &groupInformation);
    void superGroupUpdated(const QString &groupId, const QVariantMap &groupInformation);
    void chatOnlineMemberCountUpdated(const QString &chatId, const int &onlineMemberCount);
    void messagesReceived(const QVariantList &messages);
    void newMessageReceived(const QString &chatId, const QVariantMap &message);
    void copyToDownloadsSuccessful(const QString &fileName, const QString &filePath);
    void copyToDownloadsError(const QString &fileName, const QString &filePath);
    void receivedMessage(const QString &messageId, const QVariantMap &message);
    void messageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);
    void activeNotificationsUpdated(const QVariantList notificationGroups);
    void notificationGroupUpdated(const QVariantMap notificationGroupUpdate);
    void notificationUpdated(const QVariantMap updatedNotification);

public slots:
    void handleVersionDetected(const QString &version);
    void handleAuthorizationStateChanged(const QString &authorizationState);
    void handleOptionUpdated(const QString &optionName, const QVariant &optionValue);
    void handleConnectionStateChanged(const QString &connectionState);
    void handleUserUpdated(const QVariantMap &userInformation);
    void handleUserStatusUpdated(const QString &userId, const QVariantMap &userStatusInformation);
    void handleFileUpdated(const QVariantMap &fileInformation);
    void handleNewChatDiscovered(const QVariantMap &chatInformation);
    void handleUnreadMessageCountUpdated(const QVariantMap &messageCountInformation);
    void handleUnreadChatCountUpdated(const QVariantMap &chatCountInformation);
    void handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void handleChatOrderUpdated(const QString &chatId, const QString &order);
    void handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, const int &unreadCount);
    void handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void handleBasicGroupUpdated(const QString &groupId, const QVariantMap &groupInformation);
    void handleSuperGroupUpdated(const QString &groupId, const QVariantMap &groupInformation);
    void handleChatOnlineMemberCountUpdated(const QString &chatId, const int &onlineMemberCount);
    void handleMessagesReceived(const QVariantList &messages);
    void handleNewMessageReceived(const QString &chatId, const QVariantMap &message);
    void handleMessageInformation(const QString &messageId, const QVariantMap &message);
    void handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);
    void handleUpdateActiveNotifications(const QVariantList notificationGroups);
    void handleUpdateNotificationGroup(const QVariantMap notificationGroupUpdate);
    void handleUpdateNotification(const QVariantMap updatedNotification);

private:
    void *tdLibClient;
    TDLibReceiver *tdLibReceiver;
    DBusInterface *dbusInterface;
    QString version;
    TDLibWrapper::AuthorizationState authorizationState;
    TDLibWrapper::ConnectionState connectionState;
    QVariantMap options;
    QVariantMap userInformation;
    QVariantMap allUsers;
    QVariantMap chats;
    QVariantMap unreadMessageInformation;
    QVariantMap unreadChatInformation;
    QVariantMap basicGroups;
    QVariantMap superGroups;

    void setInitialParameters();
    void setEncryptionKey();
    void setLogVerbosityLevel();
    void initializeOpenWith();

};

#endif // TDLIBWRAPPER_H
