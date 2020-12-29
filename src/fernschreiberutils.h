#ifndef FERNSCHREIBERUTILS_H
#define FERNSCHREIBERUTILS_H

#include <QObject>
#include "tdlibwrapper.h"

class FernschreiberUtils : public QObject
{
    Q_OBJECT
public:
    explicit FernschreiberUtils(QObject *parent = nullptr);

    static QString getMessageShortText(TDLibWrapper *tdLibWrapper, const QVariantMap &messageContent, const bool isChannel, const qlonglong currentUserId, const QVariantMap &messageSender);
    static QString getUserName(const QVariantMap &userInformation);

signals:

public slots:
};

#endif // FERNSCHREIBERUTILS_H
