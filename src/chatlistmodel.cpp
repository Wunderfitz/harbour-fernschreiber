#include "chatlistmodel.h"

ChatListModel::ChatListModel(TDLibWrapper *tdLibWrapper)
{
    this->tdLibWrapper = tdLibWrapper;
    connect(this->tdLibWrapper, SIGNAL(newChatDiscovered(QString, QVariantMap)), this, SLOT(handleChatDiscovered(QString, QVariantMap)));
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
    endInsertRows();
    return true;
}

void ChatListModel::handleChatDiscovered(const QString &chatId, const QVariantMap &chatInformation)
{
    qDebug() << "[ChatListModel] Adding new chat " << chatId;
    this->chatToBeAdded = chatInformation;
    insertRows(rowCount(QModelIndex()), 1);
}
