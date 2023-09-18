#include "messageitemmodel.h"
#include "settings.h"


MessageItemModel::MessageItemModel(QObject *parent)
    : QStandardItemModel(parent)
{

}


void MessageItemModel::updateLastId(int id)
{
    if (id > settings->lastId())
        settings->setLastId(id);
}


void MessageItemModel::insertMessage(int row, GotifyModel::Message * message)
{
    updateLastId(message->id);
    MessageItem * item = new MessageItem(message);
    insertRow(row, item);
}


void MessageItemModel::appendMessage(GotifyModel::Message * message)
{
    updateLastId(message->id);
    MessageItem * item = new MessageItem(message);
    appendRow(item);
}


MessageItem * MessageItemModel::itemFromIndex(const QModelIndex &index)
{

    return static_cast<MessageItem *>(QStandardItemModel::itemFromIndex(index));
}
