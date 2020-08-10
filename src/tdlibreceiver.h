#ifndef TDLIBRECEIVER_H
#define TDLIBRECEIVER_H

#include <QDebug>
#include <QThread>
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

private:
    void *tdLibClient;
    bool isActive;

    void receiverLoop();
};

#endif // TDLIBRECEIVER_H
