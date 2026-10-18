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
    // setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool
MessageProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    MessageItemModel* source = static_cast<MessageItemModel*>(sourceModel());
    MessageItem* item = static_cast<MessageItem*>(source->item(sourceRow));
    if (!item)
        return false;

    QRegularExpression filter = filterRegularExpression();

    if (filter.pattern().isEmpty())
        return true;
    return scoreForItem(item, filter.pattern()) >= score_cutoff;
}

double MessageProxyModel::scoreForItem(MessageItem *item, const QString &pattern) const
{
    std::string query = pattern.toLower().toLatin1().constData();
    QString key_qstring = item->title().toLower() + " " + item->message().toLower();
    std::string key = key_qstring.toLatin1().constData();
    key.erase(std::remove(key.begin(), key.end(), '?'), key.end());
    rapidfuzz::fuzz::CachedRatio<QString> scorer(query);
    double offset =
        (item->title().toLower().contains(pattern) || item->message().toLower().contains(pattern) ? 100. : 0.);
    return scorer.similarity(key, score_cutoff) + offset;
}

bool MessageProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    MessageItemModel *source = static_cast<MessageItemModel *>(sourceModel());
    MessageItem *leftItem = source->itemFromIndex(source_left);
    MessageItem *rightItem = source->itemFromIndex(source_right);

    if (!leftItem || !rightItem)
        return false;

    const QString pattern = filterRegularExpression().pattern();
    return scoreForItem(leftItem, pattern) > scoreForItem(rightItem, pattern);
}
