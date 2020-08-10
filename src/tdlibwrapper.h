#ifndef TDLIBWRAPPER_H
#define TDLIBWRAPPER_H

#include <QObject>
#include <QDebug>
#include <td/telegram/td_json_client.h>

class TDLibWrapper : public QObject
{
    Q_OBJECT
public:
    explicit TDLibWrapper(QObject *parent = nullptr);
    ~TDLibWrapper();

    Q_INVOKABLE void testIt();

signals:

public slots:


private:
    void *tdLibClient;

};

#endif // TDLIBWRAPPER_H
