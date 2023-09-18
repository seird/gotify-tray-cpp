#include "applicationitemmodel.h"
#include "settings.h"


ApplicationItemModel::ApplicationItemModel(QObject * parent)
    : QStandardItemModel(parent)
{

}


ApplicationItem * ApplicationItemModel::itemFromIndex(const QModelIndex &index)
{
    return static_cast<ApplicationItem *>(QStandardItemModel::itemFromIndex(index));
}


ApplicationItem * ApplicationItemModel::itemFromId(int id)
{
    for (int r=0; r<rowCount(); ++r) {
        ApplicationItem * applicationItem = static_cast<ApplicationItem *>(item(r, 0));
        if (id == applicationItem->id() && !applicationItem->allMessages()) {
            return applicationItem;
        }
    }
    return nullptr;
}


ApplicationProxyModel::ApplicationProxyModel(ApplicationItemModel * applicationItemModel)
    : QSortFilterProxyModel()
{
    setSourceModel(applicationItemModel);
    setSortCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    if (settings->sortApplications()) {
        sort(0, Qt::SortOrder::AscendingOrder);
    }
}


bool ApplicationProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    ApplicationItemModel * source = static_cast<ApplicationItemModel *>(sourceModel());

    if (source->itemFromIndex(source_left)->allMessages()) {
        return true;
    } else if (source->itemFromIndex(source_right)->allMessages()) {
        return false;
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
