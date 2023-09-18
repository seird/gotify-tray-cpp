#ifndef GOTIFYAPI_H
#define GOTIFYAPI_H


#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>
#include <QUrlQuery>


class GotifyApi : public QNetworkAccessManager
{
    Q_OBJECT
public:
    GotifyApi(QUrl severUrl, QByteArray clientToken, QObject * parent = nullptr);
    void updateAuth(QUrl serverUrl, QByteArray clientToken);
    QNetworkReply * applications();
    QNetworkReply * applicationMessages(int applicationId, int limit = 100, int since = 0);
    QNetworkReply * messages(int limit = 100, int since = 0);
    QNetworkReply * deleteMessage(int messageId);
    QNetworkReply * deleteMessages();
    QNetworkReply * deleteApplicaitonMessages(int applicationId);
    QNetworkReply * health();
    QNetworkReply * version();

private:
    QUrl serverUrl;
    QByteArray clientToken;
    QNetworkRequest request;

    QNetworkReply * get(QString endpoint, QUrlQuery query = QUrlQuery());
    QNetworkReply * deleteResource(QString endpoint);
};

#endif // GOTIFYAPI_H
