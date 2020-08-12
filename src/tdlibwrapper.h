#ifndef TDLIBWRAPPER_H
#define TDLIBWRAPPER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <td/telegram/td_json_client.h>
#include "tdlibreceiver.h"

class TDLibWrapper : public QObject
{
    Q_OBJECT
public:
    explicit TDLibWrapper(QObject *parent = nullptr);
    ~TDLibWrapper();

    Q_INVOKABLE QString getVersion();
    Q_INVOKABLE QString getAuthorizationState();

signals:
    void versionDetected(const QString &version);
    void authorizationStateChanged(const QString &authorizationState);

public slots:
    void handleVersionDetected(const QString &version);
    void handleAuthorizationStateChanged(const QString &authorizationState);

private:
    void *tdLibClient;
    TDLibReceiver *tdLibReceiver;

    QString version;
    QString authorizationState;

};

#endif // TDLIBWRAPPER_H
