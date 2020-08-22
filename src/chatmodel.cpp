#include "chatmodel.h"

ChatModel::ChatModel(TDLibWrapper *tdLibWrapper)
{
    this->tdLibWrapper = tdLibWrapper;
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
    this->messages.insert(row, this->messagesToBeAdded);
    this->messageIndexMap.clear();
    for (int i = 0; i < this->messages.size(); i++) {
        this->messageIndexMap.insert(this->messages.at(i).toMap().value("id").toString(), i);
    }
    endInsertRows();
    return true;
}
