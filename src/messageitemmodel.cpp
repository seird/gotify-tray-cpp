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

void MessageItemModel::insertMessage(int row, GotifyModel::Message *message)
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

MessageItem *MessageItemModel::itemFromIndex(const QModelIndex &index)
{

    return static_cast<MessageItem*>(QStandardItemModel::itemFromIndex(index));
}

MessageProxyModel::MessageProxyModel(MessageItemModel* messageItemModel)
  : QSortFilterProxyModel()
{
    setSourceModel(messageItemModel);
    sort(0);
}

bool
MessageProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    beginFilterChange();
    filterQuery = pattern;
    endFilterChange();
    if (filterQuery.isEmpty())
        invalidate();
}

bool MessageProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    MessageItemModel *source = static_cast<MessageItemModel *>(sourceModel());
    MessageItem *item = static_cast<MessageItem *>(source->item(sourceRow));
    if (filterQuery.isEmpty())
        return true;
    if (!item)
        return false;
    return scoreForItem(item) >= score_cutoff;
}

double MessageProxyModel::scoreForItem(MessageItem *item) const
{
    std::string query = filterQuery.toLower().toLatin1().constData();
    QString key_qstring = item->title().toLower() + " " + item->message().toLower();
    std::string key = key_qstring.toLatin1().constData();
    key.erase(std::remove(key.begin(), key.end(), '?'), key.end());
    rapidfuzz::fuzz::CachedRatio<QString> scorer(query);
    double offset = (key_qstring.contains(filterQuery.toLower()) ? 100. : 0.);
    double score = scorer.similarity(key, score_cutoff) + offset;
    return score;
}

bool MessageProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    MessageItemModel *source = static_cast<MessageItemModel *>(sourceModel());
    MessageItem *leftItem = source->itemFromIndex(source_left);
    MessageItem *rightItem = source->itemFromIndex(source_right);

    if (filterQuery.isEmpty())
    {
        return false;
    }
    if (!leftItem || !rightItem)
    {
        return false;
    }
    double leftScore = scoreForItem(leftItem);
    double rightScore = scoreForItem(rightItem);
    if (leftScore == rightScore)
    {
        return leftItem->id() > rightItem->id();
    }
    else
    {
        return leftScore > rightScore;
    }
}
