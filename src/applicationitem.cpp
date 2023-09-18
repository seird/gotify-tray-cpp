#include "applicationitem.h"


ApplicationItem::ApplicationItem(const QString &text, GotifyModel::Application * application)
    : QStandardItem(text)
{
    if (application) {
        setData(application->id, ApplicationRole::Id);
        setData(application->token, ApplicationRole::Token);
        setData(application->name, ApplicationRole::Name);
        setData(application->description, ApplicationRole::Description);
        setData(application->internal, ApplicationRole::Internal);
        setData(application->image, ApplicationRole::Image);
        setData(false, ApplicationRole::AllMessages);
    } else {
        setData(true, ApplicationRole::AllMessages);
    }
}


int ApplicationItem::id()
{
    return data(ApplicationRole::Id).toInt();
}


QString ApplicationItem::token()
{
    return data(ApplicationRole::Token).toString();
}


QString ApplicationItem::name()
{
    return data(ApplicationRole::Name).toString();
}


QString ApplicationItem::description()
{
    return data(ApplicationRole::Description).toString();
}


QString ApplicationItem::image()
{
    return data(ApplicationRole::Image).toString();
}


bool ApplicationItem::internal()
{
    return data(ApplicationRole::Internal).toBool();
}


bool ApplicationItem::allMessages()
{
    return data(ApplicationRole::AllMessages).toBool();
}
