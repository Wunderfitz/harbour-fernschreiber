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
#include "tdlibreceiver.h"

TDLibReceiver::TDLibReceiver(void *tdLibClient, QObject *parent) : QThread(parent)
{
    this->tdLibClient = tdLibClient;
    this->isActive = true;
}

void TDLibReceiver::setActive(const bool &active)
{
    if (active) {
        qDebug() << "[TDLibReceiver] Activating receiver loop...";
    } else {
        qDebug() << "[TDLibReceiver] Deactivating receiver loop, this may take a while...";
    }
    this->isActive = active;
}

void TDLibReceiver::receiverLoop()
{
    qDebug() << "[TDLibReceiver] Starting receiver loop";
    const double WAIT_TIMEOUT = 5.0;
    while (this->isActive) {
      const char *result = td_json_client_receive(this->tdLibClient, WAIT_TIMEOUT);
      if (result) {
          QJsonDocument receivedJsonDocument = QJsonDocument::fromJson(QByteArray(result));
          qDebug().noquote() << "[TDLibReceiver] Raw result: " << receivedJsonDocument.toJson(QJsonDocument::Indented);
          processReceivedDocument(receivedJsonDocument);
      }
    }
    qDebug() << "[TDLibReceiver] Stopping receiver loop";
}

void TDLibReceiver::processReceivedDocument(const QJsonDocument &receivedJsonDocument)
{
    QVariantMap receivedInformation = receivedJsonDocument.object().toVariantMap();
    QString objectTypeName = receivedInformation.value("@type").toString();

    if (objectTypeName == "updateOption") { this->processUpdateOption(receivedInformation); }
    if (objectTypeName == "updateAuthorizationState") { this->processUpdateAuthorizationState(receivedInformation); }
    if (objectTypeName == "updateConnectionState") { this->processUpdateConnectionState(receivedInformation); }
    if (objectTypeName == "updateUser") { this->processUpdateUser(receivedInformation); }
    if (objectTypeName == "updateFile") { this->processUpdateFile(receivedInformation); }
    if (objectTypeName == "updateNewChat") { this->processUpdateNewChat(receivedInformation); }
    if (objectTypeName == "updateUnreadMessageCount") { this->processUpdateUnreadMessageCount(receivedInformation); }
    if (objectTypeName == "updateUnreadChatCount") { this->processUpdateUnreadChatCount(receivedInformation); }
}

void TDLibReceiver::processUpdateOption(const QVariantMap &receivedInformation)
{
    QString currentOption = receivedInformation.value("name").toString();
    if (currentOption == "version") {
        QString detectedVersion = receivedInformation.value("value").toMap().value("value").toString();
        qDebug() << "[TDLibReceiver] TD Lib version detected: " << detectedVersion;
        emit versionDetected(detectedVersion);
    } else {
        QVariant currentValue = receivedInformation.value("value").toMap().value("value");
        qDebug() << "[TDLibReceiver] Option updated: " << currentOption << currentValue;
        emit optionUpdated(currentOption, currentValue);
    }
}

void TDLibReceiver::processUpdateAuthorizationState(const QVariantMap &receivedInformation)
{
    QString authorizationState = receivedInformation.value("authorization_state").toMap().value("@type").toString();
    qDebug() << "[TDLibReceiver] Authorization state changed: " << authorizationState;
    emit authorizationStateChanged(authorizationState);
}

void TDLibReceiver::processUpdateConnectionState(const QVariantMap &receivedInformation)
{
    QString connectionState = receivedInformation.value("state").toMap().value("@type").toString();
    qDebug() << "[TDLibReceiver] Connection state changed: " << connectionState;
    emit connectionStateChanged(connectionState);
}

void TDLibReceiver::processUpdateUser(const QVariantMap &receivedInformation)
{
    QVariantMap userInformation = receivedInformation.value("user").toMap();
    // qDebug() << "[TDLibReceiver] User was updated: " << userInformation.value("username").toString() << userInformation.value("first_name").toString() << userInformation.value("last_name").toString();
    emit userUpdated(userInformation);
}

void TDLibReceiver::processUpdateFile(const QVariantMap &receivedInformation)
{
    QVariantMap fileInformation = receivedInformation.value("file").toMap();
    qDebug() << "[TDLibReceiver] File was updated: " << fileInformation.value("id").toString();
    emit fileUpdated(fileInformation);
}

void TDLibReceiver::processUpdateNewChat(const QVariantMap &receivedInformation)
{
    QVariantMap chatInformation = receivedInformation.value("chat").toMap();
    qDebug() << "[TDLibReceiver] New chat discovered: " << chatInformation.value("id").toString() << chatInformation.value("title").toString();
    emit newChatDiscovered(chatInformation);
}

void TDLibReceiver::processUpdateUnreadMessageCount(const QVariantMap &receivedInformation)
{
    QVariantMap messageCountInformation;
    messageCountInformation.insert("chat_list_type", receivedInformation.value("chat_list").toMap().value("@type"));
    messageCountInformation.insert("unread_count", receivedInformation.value("unread_count"));
    messageCountInformation.insert("unread_unmuted_count", receivedInformation.value("unread_unmuted_count"));
    qDebug() << "[TDLibReceiver] Unread message count updated: " << messageCountInformation.value("chat_list_type").toString() << messageCountInformation.value("unread_count").toString();
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
    qDebug() << "[TDLibReceiver] Unread chat count updated: " << chatCountInformation.value("chat_list_type").toString() << chatCountInformation.value("unread_count").toString();
    emit unreadChatCountUpdated(chatCountInformation);
}
