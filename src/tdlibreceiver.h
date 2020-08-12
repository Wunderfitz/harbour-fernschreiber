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

private:
    void *tdLibClient;
    bool isActive;

    void receiverLoop();
    void processReceivedDocument(const QJsonDocument &receivedJsonDocument);
    void processUpdateOption(const QVariantMap &receivedInformation);
    void processUpdateAuthorizationState(const QVariantMap &receivedInformation);
};

#endif // TDLIBRECEIVER_H
