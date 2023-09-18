#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H


#include <QStandardItem>

#include "gotifymodels.h"


class ApplicationItem : public QStandardItem
{

public:
    explicit ApplicationItem(const QString &text, GotifyModel::Application * application = nullptr);
    int id();
    QString token();
    QString name();
    QString description();
    bool internal();
    QString image();
    bool allMessages();


private:
    enum ApplicationRole {
        Id          = Qt::UserRole + 1,
        Token       = Qt::UserRole + 2,
        Name        = Qt::UserRole + 3,
        Description = Qt::UserRole + 4,
        Internal    = Qt::UserRole + 5,
        Image       = Qt::UserRole + 6,
        AllMessages = Qt::UserRole + 7
    };
};


#endif // APPLICATIONITEM_H
