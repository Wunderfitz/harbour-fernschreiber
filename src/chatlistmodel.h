#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include "tdlibwrapper.h"

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ChatListModel(TDLibWrapper *tdLibWrapper);
    ~ChatListModel() override;

    virtual int rowCount(const QModelIndex&) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public slots:
    void handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation);

private:
    TDLibWrapper *tdLibWrapper;
    QVariantList chatList;
    QVariantMap chatToBeAdded;
};

#endif // CHATLISTMODEL_H
