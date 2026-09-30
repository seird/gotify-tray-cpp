#ifndef MESSAGEITEMMODEL_H
#define MESSAGEITEMMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "messageitem.h"

class MessageItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit MessageItemModel(QObject * parent = nullptr);
    void insertMessage(int row, GotifyModel::Message * message);
    void appendMessage(GotifyModel::Message * message);
    MessageItem * itemFromIndex(const QModelIndex &index);

private:
    void updateLastId(int id);
};

class MessageProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

  public:
    explicit MessageProxyModel(MessageItemModel* messageItemModel);

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

  private:
};

#endif // MESSAGEITEMMODEL_H
