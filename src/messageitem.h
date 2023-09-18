#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H


#include <QStandardItem>

#include "gotifymodels.h"


class MessageItem : public QStandardItem
{

public:
    explicit MessageItem(GotifyModel::Message * message);
    int id();
    int appId();
    int priority();
    QString title();
    QDateTime date();
    QString message();
    bool markdown();


private:
    enum MessageRole {
        Id          = Qt::UserRole + 1,
        AppId       = Qt::UserRole + 2,
        Priority    = Qt::UserRole + 3,
        Title       = Qt::UserRole + 4,
        Date        = Qt::UserRole + 5,
        Message     = Qt::UserRole + 6,
        Markdown    = Qt::UserRole + 7,
    };
};


#endif // MESSAGEITEM_H
