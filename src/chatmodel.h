#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include "tdlibwrapper.h"

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ChatModel(TDLibWrapper *tdLibWrapper);
    ~ChatModel() override;

    virtual int rowCount(const QModelIndex&) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    Q_INVOKABLE void initialize(const QVariantMap &chatInformation);
    Q_INVOKABLE void triggerLoadMoreHistory();

signals:
    void messagesReceived(const int &modelIndex, const int &lastReadSentIndex);
    void messagesIncrementalUpdate(const int &modelIndex, const int &lastReadSentIndex);
    void newMessageReceived();
    void unreadCountUpdated(const int &unreadCount, const QString &lastReadInboxMessageId);
    void lastReadSentMessageUpdated(const int &lastReadSentIndex);

public slots:
    void handleMessagesReceived(const QVariantList &messages);
    void handleNewMessageReceived(const QString &chatId, const QVariantMap &message);
    void handleChatReadInboxUpdated(const QString &chatId, const QString &lastReadInboxMessageId, const int &unreadCount);
    void handleChatReadOutboxUpdated(const QString &chatId, const QString &lastReadOutboxMessageId);
    void handleMessageSendSucceeded(const QString &messageId, const QString &oldMessageId, const QVariantMap &message);

private:

    TDLibWrapper *tdLibWrapper;
    QVariantList messages;
    QVariantList messagesToBeAdded;
    QVariantMap messageIndexMap;
    QMutex messagesMutex;
    QVariantMap chatInformation;
    QString chatId;
    bool inReload;
    bool inIncrementalUpdate;

    void insertMessages();
    QVariantMap enhanceMessage(const QVariantMap &message);
    int calculateLastKnownMessageId();
    int calculateLastReadSentMessageId();
    void calculateMessageIndexMap();
};

#endif // CHATMODEL_H
