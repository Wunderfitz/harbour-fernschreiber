#include "chatmodel.h"

#include <QListIterator>

ChatModel::ChatModel(TDLibWrapper *tdLibWrapper)
{
    this->tdLibWrapper = tdLibWrapper;
    this->inReload = false;
    connect(this->tdLibWrapper, SIGNAL(messagesReceived(QVariantList)), this, SLOT(handleMessagesReceived(QVariantList)));
    connect(this->tdLibWrapper, SIGNAL(newMessageReceived(QString, QVariantMap)), this, SLOT(handleNewMessageReceived(QString, QVariantMap)));
}

ChatModel::~ChatModel()
{

}

int ChatModel::rowCount(const QModelIndex &) const
{
    return messages.size();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(messages.value(index.row()));
    }
    return QVariant();
}

bool ChatModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "[ChatModel] Inserting at " << row << ", row count: " << count;
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        this->messages.insert(row + i, this->messagesToBeAdded.at(i));
    }
    this->messageIndexMap.clear();
    for (int i = 0; i < this->messages.size(); i++) {
        this->messageIndexMap.insert(this->messages.at(i).toMap().value("id").toString(), i);
    }
    endInsertRows();
    return true;
}

void ChatModel::initialize(const QString &chatId)
{
    this->chatId = chatId;
    this->messages.clear();
    this->messageIndexMap.clear();
    this->messagesToBeAdded.clear();
}

bool compareMessages(const QVariant &message1, const QVariant &message2)
{
    QVariantMap messageMap1 = message1.toMap();
    QVariantMap messageMap2 = message2.toMap();
    if (messageMap1.value("id").toLongLong() < messageMap2.value("id").toLongLong()) {
        return true;
    } else {
        return false;
    }
}

void ChatModel::handleMessagesReceived(const QVariantList &messages)
{
    qDebug() << "[ChatModel] Receiving new messages :)";
    this->messagesMutex.lock();

    this->messagesToBeAdded.clear();
    QListIterator<QVariant> messagesIterator(messages);
    while (messagesIterator.hasNext()) {
        QVariantMap currentMessage = messagesIterator.next().toMap();
        if (currentMessage.value("chat_id").toString() == this->chatId) {
            this->messagesToBeAdded.append(currentMessage);
        }
    }
    std::sort(this->messagesToBeAdded.begin(), this->messagesToBeAdded.end(), compareMessages);

    this->insertMessages();
    this->messagesMutex.unlock();

    // First call only returns one message, we need to get a little more than that...
    if (this->messagesToBeAdded.size() == 1 && !this->inReload) {
        qDebug() << "[ChatModel] Only one message received in first call, loading more...";
        this->inReload = true;
        this->tdLibWrapper->getChatHistory(this->chatId, this->messagesToBeAdded.first().toMap().value("id").toLongLong());
    } else {
        qDebug() << "[ChatModel] Messages loaded, notifying chat UI...";
        this->inReload = false;
        emit messagesReceived();
    }
}

void ChatModel::handleNewMessageReceived(const QString &chatId, const QVariantMap &message)
{
    if (chatId == this->chatId) {
        qDebug() << "[ChatModel] New message received for this chat";
        this->messagesMutex.lock();

        this->messagesToBeAdded.clear();
        this->messagesToBeAdded.append(message);

        this->insertMessages();
        this->messagesMutex.unlock();
        emit newMessageReceived();
    }
}

void ChatModel::insertMessages()
{
    if (this->messages.isEmpty()) {
        beginResetModel();
        this->messages.append(this->messagesToBeAdded);
        endResetModel();
    } else {
        // There is only an append or a prepend, tertium non datur! (probably ;))
        if (this->messages.last().toMap().value("id").toLongLong() < this->messagesToBeAdded.first().toMap().value("id").toLongLong()) {
            // Append
            this->insertRows(rowCount(QModelIndex()), this->messagesToBeAdded.size());
        } else {
            // Prepend
            this->insertRows(0, this->messagesToBeAdded.size());
        }
    }
}
