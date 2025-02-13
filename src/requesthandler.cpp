#include "requesthandler.h"
#include "utils.h"
#include "cache.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


RequestHandler * requestHandler = nullptr;


RequestHandler * RequestHandler::getInstance()
{
    if(!requestHandler) {
        requestHandler = new RequestHandler();
    }
    return requestHandler;
}


void RequestHandler::applications()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
    if(!reply) {
        emit finished();
        return;
    }

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        emit replyError(reply->error(), reply->errorString());
        emit finished();
        reply->deleteLater();
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (document.isNull()) {
        qDebug() << "json document is null";
        emit parseError();
        emit finished();
        reply->deleteLater();
        return;
    }

    QJsonArray applicationsArray = document.array();
    emit finishedApplications(new GotifyModel::Applications(applicationsArray));
    emit finished();
    reply->deleteLater();
}


void RequestHandler::messages(bool missed)
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
    if(!reply) {
        emit finished();
        return;
    }

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        emit replyError(reply->error(), reply->errorString());
        emit finished();
        reply->deleteLater();
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (document.isNull()) {
        qDebug() << "json document is null";
        emit parseError();
        emit finished();
        reply->deleteLater();
        return;
    }

    QJsonObject object = document.object();
    QJsonArray messagesArray = object["messages"].toArray();

    if (missed) {
        emit finishedMissedMessages(new GotifyModel::Messages(messagesArray));
    } else {
        emit finishedMessages(new GotifyModel::Messages(messagesArray));
    }


    emit finished();
    reply->deleteLater();
}


void RequestHandler::testServer()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
    if(!reply) {
        emit finished();
        return;
    }

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        emit replyError(reply->error(), reply->errorString());
        emit finished();
        reply->deleteLater();
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (document.isNull()) {
        qDebug() << "json document is null";
        emit parseError();
        emit finished();
        reply->deleteLater();
        return;
    }

    emit serverOk();
    emit finished();
    reply->deleteLater();
}


void RequestHandler::imagePopup(QPoint pos)
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
    if(!reply) {
        emit finished();
        return;
    }

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        emit replyError(reply->error(), reply->errorString());
        emit finished();
        reply->deleteLater();
        return;
    }

    QString fileName = Utils::getUuid();
    QString filePath = cache->getFilesDir() + fileName;
    Utils::writeFile(filePath, reply->readAll());
    QUrl url = reply->request().url();
    cache->store(url.toString(), fileName);

    emit finishedImagePopup(filePath, url,  pos);
    emit finished();
    reply->deleteLater();
}
