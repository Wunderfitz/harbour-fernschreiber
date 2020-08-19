#include "chatlistmodel.h"
#include <QListIterator>

ChatListModel::ChatListModel(TDLibWrapper *tdLibWrapper)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(this->tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatLastMessageUpdated(QString, QVariantMap)), this, SLOT(handleChatLastMessageUpdated(QString, QVariantMap)));
    // connect(this->tdLibWrapper, SIGNAL(chatOrderUpdated(QString, QString)), this, SLOT(handleChatOrderUpdated(QString, QString)));
}

ChatListModel::~ChatListModel()
{

}

int ChatListModel::rowCount(const QModelIndex &) const
{
    return chatList.size();
}

QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(chatList.value(index.row()));
    }
    return QVariant();
}

bool ChatListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "[ChatListModel] Inserting at " << row << ", row count: " << count;
    beginInsertRows(parent, rowCount(QModelIndex()), rowCount(QModelIndex()) + count - 1);
    this->chatList.append(this->chatToBeAdded);
    this->chatIndexMap.insert(this->chatToBeAdded.value("id").toString(), row);
    endInsertRows();
    return true;
}

void ChatListModel::handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation)
{
    qDebug() << "[ChatListModel] Adding new chat " << chatId;
    this->chatToBeAdded = chatInformation;
    insertRows(rowCount(QModelIndex()), 1);
}

void ChatListModel::handleChatLastMessageUpdated(const QString &chatId, const QVariantMap &lastMessage)
{
    int chatIndex = this->chatIndexMap.value(chatId).toInt();
    qDebug() << "[ChatListModel] Updating last message for chat " << chatId << " at index " << chatIndex;
    QVariantMap currentChat = this->chatList.value(chatIndex).toMap();
    currentChat.insert("last_message", lastMessage);
    this->chatList.replace(chatIndex, currentChat);
    emit dataChanged(this->index(chatIndex), this->index(chatIndex));
}

void ChatListModel::handleChatOrderUpdated(const QString &chatId, const QString &order)
{
    qDebug() << "[ChatListModel] Updating chat order because of " << chatId << " , new order " << order;
    int currentChatIndex = this->chatIndexMap.value(chatId).toInt();
    int otherChatIndex = 0;
    QListIterator<QVariant> currentChatIterator(this->chatList);
    while (currentChatIterator.hasNext()) {
        QVariantMap otherChat = currentChatIterator.next().toMap();
        QString otherOrder = otherChat.value("order").toString();
        if (otherOrder < order) {
            // Our chat gets reordered to the position of the other chat
            otherChatIndex = this->chatIndexMap.value(otherChat.value("id").toString()).toInt();
            qDebug() << "[ChatListModel] Old position: " << currentChatIndex << "- new position " << otherChatIndex;
            break;
        }
    }
    this->chatList.move(currentChatIndex, otherChatIndex);
    this->chatIndexMap.clear();
    QListIterator<QVariant> newChatIterator(this->chatList);
    int currentIndex = 0;
    while (newChatIterator.hasNext()) {
        QVariantMap currentChat = newChatIterator.next().toMap();
        this->chatIndexMap.insert(currentChat.value("id").toString(), currentIndex);
        currentIndex++;
    }
}
