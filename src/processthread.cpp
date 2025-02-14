#include "processthread.h"
#include "utils.h"
#include "cache.h"
#include "settings.h"

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
        if (url.scheme() == "https" && !settings->selfSignedCertificatePath().isEmpty())
            reply->ignoreSslErrors(Utils::getSelfSignedExpectedErrors(settings->selfSignedCertificatePath()));
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

}
