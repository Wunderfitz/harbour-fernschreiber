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

    enum ConnectionState {
        Connecting,
        ConnectingToProxy,
        IsReady,
        Updating,
        WaitingForNetwork
    };

    Q_INVOKABLE void sendRequest(const QVariantMap &requestObject);
    Q_INVOKABLE QString getVersion();
    Q_INVOKABLE TDLibWrapper::AuthorizationState getAuthorizationState();
    Q_INVOKABLE TDLibWrapper::ConnectionState getConnectionState();

signals:
    void versionDetected(const QString &version);
    void authorizationStateChanged(const TDLibWrapper::AuthorizationState &authorizationState);
    void optionUpdated(const QString &optionName, const QVariant &optionValue);
    void connectionStateChanged(const TDLibWrapper::ConnectionState &connectionState);

public slots:
    void handleVersionDetected(const QString &version);
    void handleAuthorizationStateChanged(const QString &authorizationState);
    void handleOptionUpdated(const QString &optionName, const QVariant &optionValue);
    void handleConnectionStateChanged(const QString &connectionState);

private:
    void *tdLibClient;
    TDLibReceiver *tdLibReceiver;
    QString version;
    TDLibWrapper::AuthorizationState authorizationState;
    TDLibWrapper::ConnectionState connectionState;
    QVariantMap options;

    void setInitialParameters();
    void setEncryptionKey();

};

#endif // TDLIBWRAPPER_H
