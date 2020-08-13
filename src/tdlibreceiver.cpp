/*
    Copyright (C) 2020 Sebastian J. Wolf

    This file is part of Fernschreiber.

    fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    fernschreiber is distributed in the hope that it will be useful,
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

    if (objectTypeName == "updateOption") {
        this->processUpdateOption(receivedInformation);
    }

    if (objectTypeName == "updateAuthorizationState") {
        this->processUpdateAuthorizationState(receivedInformation);
    }

    if (objectTypeName == "updateConnectionState") {
        this->processUpdateConnectionState(receivedInformation);
    }
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
