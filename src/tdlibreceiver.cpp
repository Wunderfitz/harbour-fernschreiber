#include "tdlibreceiver.h"

TDLibReceiver::TDLibReceiver(void *tdLibClient, QObject *parent) : QThread(parent)
{
    this->tdLibClient = tdLibClient;
    this->isActive = true;
}

void TDLibReceiver::setActive(const bool &active)
{
    qDebug() << "[TDLibReceiver] setActive " << active;
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
          handleReceivedDocument(receivedJsonDocument);
      }
    }
    qDebug() << "[TDLibReceiver] Stopping receiver loop";
}

void TDLibReceiver::handleReceivedDocument(const QJsonDocument &receivedJsonDocument)
{
    QVariantMap receivedInformation = receivedJsonDocument.object().toVariantMap();
    QString objectTypeName = receivedInformation.value("@type").toString();

    if (objectTypeName == "updateOption") {
        this->handleUpdateOption(receivedInformation);
    }

    if (objectTypeName == "updateAuthorizationState") {
        this->handleUpdateAuthorizationState(receivedInformation);
    }
}

void TDLibReceiver::handleUpdateOption(const QVariantMap &receivedInformation)
{
    QString currentOption = receivedInformation.value("name").toString();
    if (currentOption == "version") {
        QString detectedVersion = receivedInformation.value("value").toMap().value("value").toString();
        qDebug() << "[TDLibReceiver] TD Lib version detected: " << detectedVersion;
        emit versionDetected(detectedVersion);
    }
}

void TDLibReceiver::handleUpdateAuthorizationState(const QVariantMap &receivedInformation)
{
    QString authorizationState = receivedInformation.value("authorization_state").toMap().value("@type").toString();
    qDebug() << "[TDLibReceiver] Authorization state changed: " << authorizationState;
    emit authorizationStateChanged(authorizationState);
}
