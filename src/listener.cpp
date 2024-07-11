#include "listener.h"

#include <QUrlQuery>
#include <QTimer>
#include <QJsonDocument>


#define MIN(x, y) ((x) < (y) ? (x) : (y))


Listener::Listener(QUrl serverUrl, QByteArray clientToken, QObject * parent)
    : QWebSocket(QString(), QWebSocketProtocol::VersionLatest, parent),
    secDelay(0.1)
{
    updateAuth(serverUrl, clientToken);
    connect(this, &QWebSocket::connected, this, [this]{secDelay = 0.1;});
    connect(this, &QWebSocket::textMessageReceived, this, &Listener::textMessageReceivedHandler);
}


void Listener::updateAuth(QUrl serverUrl, QByteArray clientToken)
{
    this->serverUrl = serverUrl;
    this->clientToken = clientToken;
}


QUrl Listener::buildUrl()
{
    QUrl url(serverUrl);
    url.setScheme(url.scheme().compare("https") ? "ws" : "wss");
    url.setPath(url.path() + "/stream");
    QUrlQuery query;
    query.addQueryItem("token", clientToken);
    url.setQuery(query);
    return url;
}


void Listener::open()
{
    qDebug() << "Opening listener";
    QTimer::singleShot(1000 * secDelay, this, [this]{
        if (!isConnected())
            open(buildUrl());
    });
    secDelay  = qBound(0.1f, secDelay * 2, 120.0f);
}


bool Listener::isListening()
{
    return state() == QAbstractSocket::SocketState::ListeningState;
}


void Listener::textMessageReceivedHandler(const QString &message)
{
    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());
    if (document.isNull()) {
        qWarning() << "json document is null; message: " << message;
        return;
    }

    QJsonObject object = document.object();
    GotifyModel::Message * gotifyMessage = new GotifyModel::Message(object);
    emit messageReceived(gotifyMessage);
}
