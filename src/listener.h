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
    Listener(QUrl serverUrl, QByteArray clientToken, QString certPath = "", QObject* parent = nullptr);

    void updateAuth(QUrl serverUrl, QByteArray clientToken, QString certPath = "");
    void startListening();
    bool isConnected();

signals:
    void messageReceived(GotifyModel::Message * message);

private slots:
    void textMessageReceivedHandler(const QString &message);

private:
    QUrl serverUrl;
    QByteArray clientToken;
    QString certPath;

    QTimer * timer;
    float secDelay;

    QUrl buildUrl();
};


#endif // LISTENER_H
