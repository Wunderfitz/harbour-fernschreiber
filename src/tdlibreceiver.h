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
#ifndef TDLIBRECEIVER_H
#define TDLIBRECEIVER_H

#include <QDebug>
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
    void setActive(const bool &active);

signals:
    void versionDetected(const QString &version);
    void authorizationStateChanged(const QString &authorizationState);
    void optionUpdated(const QString &optionName, const QVariant &optionValue);
    void connectionStateChanged(const QString &connectionState);
    void userUpdated(const QVariantMap &userInformation);
    void fileUpdated(const QVariantMap &fileInformation);
    void newChatDiscovered(const QVariantMap &chatInformation);
    void unreadMessageCountUpdated(const QVariantMap &messageCountInformation);
    void unreadChatCountUpdated(const QVariantMap &chatCountInformation);
    void chatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void chatOrderUpdated(const QString &chatId, const QString &order);

private:
    void *tdLibClient;
    bool isActive;

    void receiverLoop();
    void processReceivedDocument(const QJsonDocument &receivedJsonDocument);
    void processUpdateOption(const QVariantMap &receivedInformation);
    void processUpdateAuthorizationState(const QVariantMap &receivedInformation);
    void processUpdateConnectionState(const QVariantMap &receivedInformation);
    void processUpdateUser(const QVariantMap &receivedInformation);
    void processUpdateFile(const QVariantMap &receivedInformation);
    void processUpdateNewChat(const QVariantMap &receivedInformation);
    void processUpdateUnreadMessageCount(const QVariantMap &receivedInformation);
    void processUpdateUnreadChatCount(const QVariantMap &receivedInformation);
    void processUpdateChatLastMessage(const QVariantMap &receivedInformation);
    void processUpdateChatOrder(const QVariantMap &receivedInformation);
};

#endif // TDLIBRECEIVER_H
