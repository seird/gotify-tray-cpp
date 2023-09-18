#include "gotifymodels.h"


namespace GotifyModel {


Message::Message(QJsonObject object, QObject * parent) : QObject(parent)
{
    id = object["id"].toInt();
    appId = object["appid"].toInt();
    priority = object["priority"].toInt();
    date = QDateTime::fromString(object["date"].toString(), Qt::DateFormat::ISODate).toLocalTime();
    title = object["title"].toString();
    message = object.value("message").toString();
    markdown = object["extras"].toObject()["client::display"].toObject()["contentType"].toString() == "text/markdown";
}


Messages::Messages(QJsonArray array, QObject * parent) : QObject(parent)
{
    for (auto x: array) {
        QJsonObject object = x.toObject();

        GotifyModel::Message * messageModel = new GotifyModel::Message(object);
        messages.append(messageModel);
    }
}


Application::Application(QJsonObject object, QObject * parent) : QObject(parent)
{
    id = object["id"].toInt();
    image = object["image"].toString();
    internal = object["internal"].toBool();
    name = object["name"].toString();
    token = object["token"].toString();
}


Applications::Applications(QJsonArray array, QObject * parent) : QObject(parent)
{
    for (auto x: array) {
        QJsonObject object = x.toObject();

        GotifyModel::Application * applicationModel = new GotifyModel::Application(object);
        applications.append(applicationModel);
    }
}


}
