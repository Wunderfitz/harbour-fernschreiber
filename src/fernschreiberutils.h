#ifndef FERNSCHREIBERUTILS_H
#define FERNSCHREIBERUTILS_H

#include <QObject>

class FernschreiberUtils : public QObject
{
    Q_OBJECT
public:
    explicit FernschreiberUtils(QObject *parent = nullptr);

    static QString getMessageShortText(const QVariantMap &messageContent, const bool &myself);

signals:

public slots:
};

#endif // FERNSCHREIBERUTILS_H
