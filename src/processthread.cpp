#include "processthread.h"
#include "cache.h"
#include "settings.h"
#include "utils.h"

#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>



namespace ProcessThread {


void Applications::process(GotifyModel::Applications * applications)
{
    this->applications = applications;
    start();
}


void Applications::run()
{
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QEventLoop eventLoop;
    QUrl url = settings->serverUrl();

    for (auto application : applications->applications) {
        // Check the cache
        QString file = cache->getFile(application->id);
        if (!file.isNull()) {
            continue;
        }

        // Make a get request
        url.setPath("/" + application->image);
        request.setUrl(url);
        QNetworkReply * reply = manager.get(request);
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        // Write the file and store in cache
        QFileInfo fi(application->image);
        QString filePath = cache->getFilesDir() + fi.fileName();
        Utils::writeFile(filePath, reply->readAll());
        cache->store(application->id, fi.fileName());

        reply->deleteLater();
    }
    emit processed(applications);
}

void
Message::process(GotifyModel::Message* message)
{
    this->message = message;
    start();
}

void
Message::run()
{
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QEventLoop eventLoop;

    // Check for image and download if not in cache
    QList<QString> urls = Utils::extractURLs(message->message);
    for (auto url : urls) {
        if (!url.isNull() && cache->getFile(url).isNull()) {
            request.setUrl(QUrl(url));
            QNetworkReply* reply = manager.get(request);
            connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
            eventLoop.exec();

            // Write the file and store in cache
            QString fileName = Utils::getUuid();
            QString filePath = cache->getFilesDir() + fileName;
            Utils::writeFile(filePath, reply->readAll());
            cache->store(url, fileName);
            reply->deleteLater();
        }
    }
    emit processed(message);
}

}
