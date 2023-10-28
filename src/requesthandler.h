#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>
#include <QNetworkReply>
#include <QPoint>

#include "gotifymodels.h"


/**
 * @brief Handle QNetworkReply from QNetworkManager get/post/... requests.
 *
 * Each handler:
 *      - corresponds to a GotifyApi method.
 *      - emits a GotifyModel upon successful handling
 *      - emits a QNetworkReply::NetworkError when a handler error occurs
 *
 * Connect the corresponding handler to the GotifyApi::<> finished signal.
 * Connect to the finished<> signals to receive the GotifyModel.
 */
class RequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit RequestHandler(QObject * parent = nullptr) : QObject(parent) {};
    static RequestHandler * getInstance();

    void applications();
    void messages(bool missed=false);
    void testServer();
    void imagePopup(QPoint pos);

signals:
    /* Finished - successful or not */
    void finished();
    /* Finished successfully */
    void finishedMessages(GotifyModel::Messages * messagesModel);
    void finishedMissedMessages(GotifyModel::Messages * messagesModel);
    void finishedApplications(GotifyModel::Applications * applicationsModel);
    void finishedImagePopup(const QString& fileName, const QUrl& url, QPoint pos);
    void serverOk();
    /* Finished with an error */
    void replyError(QNetworkReply::NetworkError error);
    void parseError();
};


extern RequestHandler * requestHandler;


#endif // REQUESTHANDLER_H
