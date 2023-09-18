#ifndef MESSAGEITEMMODEL_H
#define MESSAGEITEMMODEL_H


#include <QObject>
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

#endif // MESSAGEITEMMODEL_H
