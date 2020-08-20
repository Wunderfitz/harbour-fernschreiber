#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
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

    Q_INVOKABLE void enableDeltaUpdates();

public slots:
    void handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation);
    void handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage);
    void handleChatOrderUpdated(const QString &chatId, const QString &order);
    void handleChatReadInboxUpdated(const QString &chatId, const int &unreadCount);

private:
    TDLibWrapper *tdLibWrapper;
    QVariantList chatList;
    QVariantMap chatToBeAdded;
    QVariantMap chatIndexMap;
    QMutex chatListMutex;
    bool deltaUpdates;

    void updateChatOrder(const int &currentChatIndex, const QVariantMap &updatedChat);

};

#endif // CHATLISTMODEL_H
