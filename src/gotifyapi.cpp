#include "gotifyapi.h"
#include "utils.h"

GotifyApi::GotifyApi(QUrl severUrl, QByteArray clientToken, QString certPath, QObject* parent)
  : QNetworkAccessManager(parent)
{
    updateAuth(severUrl, clientToken, certPath);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
}


void GotifyApi::updateAuth(QUrl severUrl, QByteArray clientToken, QString certPath)
{
    this->serverUrl = severUrl;
    this->clientToken = clientToken;
    this->certPath = certPath;
    request.setRawHeader("X-Gotify-Key", clientToken);
}


QNetworkReply * GotifyApi::get(QString endpoint, QUrlQuery query)
{
    QUrl url(serverUrl);
    url.setPath(endpoint);
    url.setQuery(query);
    request.setUrl(url);

    QNetworkReply* reply = QNetworkAccessManager::get(request);

    if (serverUrl.scheme() == "https" && !certPath.isNull())
        reply->ignoreSslErrors(Utils::getSelfSignedExpectedErrors(certPath));

    return reply;
}

QNetworkReply*
GotifyApi::deleteResource(QString endpoint)
{
    QUrl url(serverUrl);
    url.setPath(endpoint);
    request.setUrl(url);
    QNetworkReply* reply = QNetworkAccessManager::deleteResource(request);

    if (serverUrl.scheme() == "https" && !certPath.isNull())
        reply->ignoreSslErrors(Utils::getSelfSignedExpectedErrors(certPath));

    return reply;
}

QNetworkReply*
GotifyApi::applications()
{
    return get("/application");
}


QNetworkReply * GotifyApi::applicationMessages(int applicationId, int limit, int since)
{
    return get("/application/" + QString::number(applicationId) + "/message");
}


QNetworkReply * GotifyApi::messages(int limit, int since)
{
    QUrlQuery query;
    query.addQueryItem("limit", QString::number(limit));
    query.addQueryItem("since", QString::number(since));
    return get("/message", query);
}


QNetworkReply * GotifyApi::deleteMessage(int messageId)
{
    return deleteResource("/message/" + QString::number(messageId));
}


QNetworkReply * GotifyApi::deleteMessages()
{
    return deleteResource("/message");
}


QNetworkReply * GotifyApi::deleteApplicaitonMessages(int applicationId)
{
    return deleteResource("/application/" + QString::number(applicationId) + "/message");
}


QNetworkReply * GotifyApi::health()
{
    return get("/health");
}


QNetworkReply * GotifyApi::version()
{
    return get("/version");
}
