#ifndef APPLICATIONITEMMODEL_H
#define APPLICATIONITEMMODEL_H


#include <QObject>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "applicationitem.h"


class ApplicationItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit ApplicationItemModel(QObject * parent = nullptr);
    ApplicationItem * itemFromIndex(const QModelIndex &index);
    ApplicationItem * itemFromId(int id);

private:

};


class ApplicationProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ApplicationProxyModel(ApplicationItemModel * applicationItemModel);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

private:

};


#endif // APPLICATIONITEMMODEL_H
