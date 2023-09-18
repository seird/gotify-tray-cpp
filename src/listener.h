#ifndef LISTENER_H
#define LISTENER_H


#include <QObject>
#include <QUrl>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

#include "gotifymodels.h"


class Listener : public QWebSocket
{
    Q_OBJECT
public:
    Listener(QUrl serverUrl, QByteArray clientToken, QObject * parent = nullptr);

    void updateAuth(QUrl serverUrl, QByteArray clientToken);
    void open();
    bool isListening();

signals:
    void messageReceived(GotifyModel::Message * message);

private slots:
    void textMessageReceivedHandler(const QString &message);

private:
    QUrl serverUrl;
    QByteArray clientToken;

    QTimer * timer;
    float secDelay;

    QUrl buildUrl();
};


#endif // LISTENER_H
