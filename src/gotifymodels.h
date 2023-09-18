#ifndef GOTIFYMODELS_H
#define GOTIFYMODELS_H


#include <QObject>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>


namespace GotifyModel {


class Message : public QObject
{
    Q_OBJECT

public:
    explicit Message(QJsonObject object, QObject * parent = nullptr);

    int id;
    int appId;
    int priority;
    QDateTime date;
    QString title;
    QString message;
    bool markdown;
};


class Messages : public QObject
{
    Q_OBJECT

public:
    explicit Messages(QJsonArray array, QObject * parent = nullptr);

    QList<Message *> messages;
};


class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QJsonObject object, QObject * parent = nullptr);

    int id;
    QString image;
    bool internal;
    QString description;
    QString name;
    QString token;
};


class Applications : public QObject
{
    Q_OBJECT

public:
    explicit Applications(QJsonArray array, QObject * parent = nullptr);

    QList<Application *> applications;
};

}

#endif // GOTIFYMODELS_H
