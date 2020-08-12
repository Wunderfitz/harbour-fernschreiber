#ifndef TDLIBWRAPPER_H
#define TDLIBWRAPPER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QStandardPaths>
#include <td/telegram/td_json_client.h>
#include "tdlibreceiver.h"

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
        Ready,
        WaitCode,
        WaitEncryptionKey,
        WaitOtherDeviceConfirmation,
        WaitPassword,
        WaitPhoneNumber,
        WaitRegistration,
        WaitTdlibParameters
    };

    Q_INVOKABLE void sendRequest(const QVariantMap &requestObject);
    Q_INVOKABLE QString getVersion();
    Q_INVOKABLE TDLibWrapper::AuthorizationState getAuthorizationState();

signals:
    void versionDetected(const QString &version);
    void authorizationStateChanged(const TDLibWrapper::AuthorizationState &authorizationState);
    void optionUpdated(const QString &optionName, const QVariant &optionValue);

public slots:
    void handleVersionDetected(const QString &version);
    void handleAuthorizationStateChanged(const QString &authorizationState);
    void handleOptionUpdated(const QString &optionName, const QVariant &optionValue);

private:
    void *tdLibClient;
    TDLibReceiver *tdLibReceiver;
    QString version;
    TDLibWrapper::AuthorizationState authorizationState;
    QVariantMap options;

    void setInitialParameters();
    void setEncryptionKey();

};

#endif // TDLIBWRAPPER_H
