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

MessageItem*
MessageItemModel::itemFromIndex(const QModelIndex& index)
{

    return static_cast<MessageItem*>(QStandardItemModel::itemFromIndex(index));
}

MessageProxyModel::MessageProxyModel(MessageItemModel* messageItemModel)
  : QSortFilterProxyModel()
{
    setSourceModel(messageItemModel);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool
MessageProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    MessageItemModel* source = static_cast<MessageItemModel*>(sourceModel());
    MessageItem* item = static_cast<MessageItem*>(source->item(sourceRow));
    if (!item)
        return false;

    const QRegularExpression& filter = filterRegularExpression();

    if (filter.pattern().isEmpty())
        return true;

    return item->message().contains(filter) || item->title().contains(filter);
}
