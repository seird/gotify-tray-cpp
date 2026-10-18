#ifndef MESSAGEITEMMODEL_H
#define MESSAGEITEMMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "messageitem.h"
#include <rapidfuzz/fuzz.hpp>

class MessageItemModel : public QStandardItemModel
{
    Q_OBJECT

  public:
    explicit MessageItemModel(QObject *parent = nullptr);
    void insertMessage(int row, GotifyModel::Message *message);
    void appendMessage(GotifyModel::Message *message);
    MessageItem *itemFromIndex(const QModelIndex &index);

private:
    void updateLastId(int id);
};

class MessageProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

  public:
    explicit MessageProxyModel(MessageItemModel *messageItemModel);

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

  private:
    double scoreForItem(MessageItem *item, const QString &pattern) const;
    const double score_cutoff = 1.0;
};

#endif // MESSAGEITEMMODEL_H
