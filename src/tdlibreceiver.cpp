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
          qDebug() << "[TDLibReceiver] Raw result: " << result;
          // parse the result as JSON object and process it as an incoming update or an answer to a previously sent request
      }
    }
    qDebug() << "[TDLibReceiver] Stopping receiver loop";
}
