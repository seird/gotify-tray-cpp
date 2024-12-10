#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include "gotifymodels.h"

#include <QObject>
#include <QThread>

namespace ProcessThread {

class Applications : public QThread
{
    Q_OBJECT

  public:
    void process(GotifyModel::Applications* applications);

  signals:
    void processed(GotifyModel::Applications* applications);

  protected:
    virtual void run();

  private:
    GotifyModel::Applications* applications;
};

class Message : public QThread
{
    Q_OBJECT

  public:
    void process(GotifyModel::Message* message);

  signals:
    void processed(GotifyModel::Message* message);

  protected:
    virtual void run();

  private:
    GotifyModel::Message* message;
};

}

#endif // PROCESSTHREAD_H
