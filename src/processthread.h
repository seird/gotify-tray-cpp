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
    void process(GotifyModel::Applications * applications);

protected:
    virtual void run();

private:
    GotifyModel::Applications * applications;
};

}

#endif // PROCESSTHREAD_H
