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

signals:

public slots:


private:
    void *tdLibClient;
    TDLibReceiver *tdLibReceiver;

};

#endif // TDLIBWRAPPER_H
