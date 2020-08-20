#include "chatlistmodel.h"
#include <QListIterator>

ChatListModel::ChatListModel(TDLibWrapper *tdLibWrapper)
{
    this->tdLibWrapper = tdLibWrapper;
    this->deltaUpdates = false;
    connect(this->tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatLastMessageUpdated(QString, QString, QVariantMap)), this, SLOT(handleChatLastMessageUpdated(QString, QString, QVariantMap)));
    connect(this->tdLibWrapper, SIGNAL(chatOrderUpdated(QString, QString)), this, SLOT(handleChatOrderUpdated(QString, QString)));
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

void ChatListModel::uiCreated()
{
    qDebug() << "[ChatListModel] Chat list on UI created, enabling delta updates...";
    layoutChanged();
    this->deltaUpdates = true;
}

bool compareChats(const QVariant &chat1, const QVariant &chat2)
{
    QVariantMap chatMap1 = chat1.toMap();
    QVariantMap chatMap2 = chat2.toMap();
    // Order comes first...
    if (chatMap1.value("order").toLongLong() > chatMap2.value("order").toLongLong()) {
        return true;
    }
    // ID comes next...
    if (chatMap1.value("order").toLongLong() == chatMap2.value("order").toLongLong() && chatMap1.value("id").toLongLong() > chatMap2.value("id").toLongLong()) {
        return true;
    } else {
        return false;
    }
}

void ChatListModel::handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation)
{
    this->chatListMutex.lock();
    qDebug() << "[ChatListModel] Adding new chat " << chatId;
    this->chatToBeAdded = chatInformation;
    insertRows(rowCount(QModelIndex()), 1);
    this->chatListMutex.unlock();
}



void ChatListModel::handleChatLastMessageUpdated(const QString &chatId, const QString &order, const QVariantMap &lastMessage)
{
    this->chatListMutex.lock();
    int chatIndex = this->chatIndexMap.value(chatId).toInt();
    qDebug() << "[ChatListModel] Updating last message for chat " << chatId << " at index " << chatIndex;
    QVariantMap currentChat = this->chatList.value(chatIndex).toMap();
    currentChat.insert("last_message", lastMessage);
    currentChat.insert("order", order);
    this->chatList.replace(chatIndex, currentChat);
    emit dataChanged(this->index(chatIndex), this->index(chatIndex));

    this->updateChatOrder(chatIndex, currentChat);

    this->chatListMutex.unlock();
}

void ChatListModel::handleChatOrderUpdated(const QString &chatId, const QString &order)
{
    this->chatListMutex.lock();
    qDebug() << "[ChatListModel] Updating chat order because of " << chatId << " new order " << order;
    int chatIndex = this->chatIndexMap.value(chatId).toInt();
    QVariantMap currentChat = this->chatList.at(chatIndex).toMap();
    currentChat.insert("order", order);
    this->chatList.replace(chatIndex, currentChat);
    emit dataChanged(this->index(chatIndex), this->index(chatIndex));

    this->updateChatOrder(chatIndex, currentChat);

    this->chatListMutex.unlock();
}

void ChatListModel::updateChatOrder(const int &currentChatIndex, const QVariantMap &updatedChat)
{
    // Finding the new position manually as information is needed by beginMoveRows()
    // This seems to be the most convenient way of persisting the list position while changing the items
    // Other alternative layoutChanged() after sorting resets the index position - there we would need to calculate the new position as well
    // If somebody has a better solution - go for it ;)
    int newChatIndex = 0;
    QVariantMap previousChat;
    for (int i = 0; i < this->chatList.length(); i++) {
        QVariantMap otherChat = this->chatList.at(i).toMap();
        if (compareChats(updatedChat, otherChat)) {
            if (previousChat.value("id") == updatedChat.value("id")) {
                newChatIndex = currentChatIndex;
            } else {
                newChatIndex = i;
            }
            break;
        }
        previousChat = otherChat;
    }
    if (newChatIndex != currentChatIndex) {
        // The updated chat now needs to go to the position of the other chat
        qDebug() << "[ChatListModel] Chat " << updatedChat.value("id").toString() << " will be moved from position " << currentChatIndex << " to " << newChatIndex;
        if (deltaUpdates) {
            beginMoveRows(QModelIndex(), currentChatIndex, currentChatIndex, QModelIndex(), (( newChatIndex < currentChatIndex ) ? newChatIndex : ( newChatIndex + 1 )));
        }
        std::sort(this->chatList.begin(), this->chatList.end(), compareChats);
        this->chatIndexMap.clear();
        for (int i = 0; i < this->chatList.length(); i++) {
            this->chatIndexMap.insert(this->chatList.at(i).toMap().value("id").toString(), i);
        }
        if (deltaUpdates) {
            endMoveRows();
        }

    }
}
