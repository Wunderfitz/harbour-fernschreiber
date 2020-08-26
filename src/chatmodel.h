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

    Q_INVOKABLE void initialize(const QString &chatId);
    Q_INVOKABLE void triggerLoadMoreHistory();

signals:
    void messagesReceived();
    void messagesIncrementalUpdate();
    void noMessagesAvailable();
    void newMessageReceived();

public slots:
    void handleMessagesReceived(const QVariantList &messages);
    void handleNewMessageReceived(const QString &chatId, const QVariantMap &message);

private:

    TDLibWrapper *tdLibWrapper;
    QVariantList messages;
    QVariantList messagesToBeAdded;
    QVariantMap messageIndexMap;
    QMutex messagesMutex;
    QString chatId;
    bool inReload;
    bool inIncrementalUpdate;

    void insertMessages();
};

#endif // CHATMODEL_H
